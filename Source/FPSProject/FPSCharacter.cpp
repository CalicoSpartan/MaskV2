// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSCharacter.h"
#include "AssaultRifle.h"
#include "FPSPlayerController.h"
#include "Net/UnrealNetwork.h"


// Sets default values
AFPSCharacter::AFPSCharacter()
{
	CurrentState = EPlayerState::EPlayerPlaying;
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	//bReplicates = true;
	// Create a first person camera component.
	FPSCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	// Attach the camera component to our capsule component.
	FPSCameraComponent->SetupAttachment(GetCapsuleComponent());

	// Position the camera slightly above the eyes.
	FPSCameraComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f + BaseEyeHeight));
	// Allow the pawn to control camera rotation.
	FPSCameraComponent->bUsePawnControlRotation = true;

	// Create a first person mesh component for the owning player.
	FPSMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonMesh"));
	// Only the owning player sees this mesh.
	FPSMesh->SetOnlyOwnerSee(true);
	// Attach the FPS mesh to the FPS camera.
	FPSMesh->SetupAttachment(FPSCameraComponent);
	// Disable some environmental shadowing to preserve the illusion of having a single mesh.
	FPSMesh->bCastDynamicShadow = false;
	FPSMesh->CastShadow = false;

	// The owning player doesn't see the regular (third-person) body mesh.
	GetMesh()->SetOwnerNoSee(true);

	CollectionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollectionBox"));
	CollectionBox->SetupAttachment(RootComponent);
	CollectionBox->bGenerateOverlapEvents = true;

	//set base weapon values
	IsFiring = false;
	HasWeapon = false;
	InitialHealth = 100.0f;
	CurrentHealth = InitialHealth;
	HealthPercentage = 1.0f;


}


void AFPSCharacter::TriggerUpdateUIBlueprint_Implementation() {

}

bool AFPSCharacter::TriggerUpdateUI_Validate() {
	return true;
}

void AFPSCharacter::TriggerUpdateUI_Implementation()
{

	TriggerUpdateUIBlueprint();

}

void AFPSCharacter::TriggerDeathUIBlueprint_Implementation() {
	
}

bool AFPSCharacter::TriggerDeathUI_Validate() {
	return true;
}

void AFPSCharacter::TriggerDeathUI_Implementation()
{

	TriggerDeathUIBlueprint();

}

EPlayerState AFPSCharacter::GetCurrentState() const
{
	return CurrentState;
}
void AFPSCharacter::SetCurrentState(EPlayerState NewState)
{
	if (Role == ROLE_Authority) {
		CurrentState = NewState;
	}
}
void AFPSCharacter::OnPlayerDeath_Implementation()
{
	SetCurrentState(EPlayerState::EPlayerDead);
	TriggerDeathUI();
	// disconnect controller from pawn
	DetachFromControllerPendingDestroy();

	if (GetMesh())
	{
		static FName CollisionProfileName(TEXT("Ragdoll"));
		GetMesh()->SetCollisionProfileName(CollisionProfileName);
	}
	SetActorEnableCollision(true);
	//ragdoll (init physics)
	GetMesh()->SetAllBodiesSimulatePhysics(true);
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->WakeAllRigidBodies();
	GetMesh()->bBlendPhysics = true;
	GetMesh()->SetOwnerNoSee(false);
	//disable movement
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->SetComponentTickEnabled(false);
	//disable collisions on the capsule
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		if (Iterator->Get()->AcknowledgedPawn == this) {
			APlayerController* PController = Iterator->Get();
			AFPSPlayerController* PC = Cast<AFPSPlayerController>(PController);
			if (PC)
			{

				PC->OnKilled();
			}
			else
			{
				UE_LOG(LogClass, Log, TEXT("cast failed"));
			}
		}
	}


}

float AFPSCharacter::GetCurrentHealth()
{
	return CurrentHealth;
}



void AFPSCharacter::ServerChangeHealthBy_Implementation(float delta)
{
	CurrentHealth += delta;
	HealthPercentage = CurrentHealth / InitialHealth;

}
float AFPSCharacter::GetInitialHealth()
{
	return InitialHealth;
}

bool  AFPSCharacter::ServerUpdateRotation_Validate(FRotator Rotation)
{

	return true;

}


void  AFPSCharacter::ServerUpdateRotation_Implementation(FRotator Rotation)
{

	FPSCameraComponent->SetWorldRotation(Rotation);
	
}


void AFPSCharacter::OnShoot()
{
	if (CurrentPrimary != NULL) {
		ServerOnShoot();
	}

}
void AFPSCharacter::OnStopShoot()
{
	ServerOnStopShoot();
}



bool AFPSCharacter::ServerOnStopShoot_Validate()
{
	return true;
}
void AFPSCharacter::ServerOnStopShoot_Implementation()
{
	if (Role == ROLE_Authority) {
		IsFiring = false;
		UWorld* World = GetWorld();
		check(World);
		GetWorld()->GetTimerManager().ClearTimer(WeaponFireRateTimer);
	}
}

void AFPSCharacter::FireAgain()
{
	ServerOnShoot();
}

bool AFPSCharacter::ServerOnShoot_Validate()
{
	return true;
}
void AFPSCharacter::ServerOnShoot_Implementation()
{
	if (Role = ROLE_Authority) {
		UWorld* World = GetWorld();
		check(World);
		FHitResult hit;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);
		if (CurrentPrimary->AmmoLeftInMag > 0 && IsFiring == false) {
			if (CurrentPrimary->IsProjectile == false) {
				if (CurrentPrimary->IsAutomatic == true)
				{
					GetWorld()->GetTimerManager().SetTimer(WeaponFireRateTimer, this, &AFPSCharacter::FireAgain, CurrentPrimary->FireRate, false);
				}
				if (World->LineTraceSingleByChannel(hit, FPSCameraComponent->GetComponentLocation(), FPSCameraComponent->GetComponentLocation() + FPSCameraComponent->GetForwardVector() * CurrentPrimary->Range, ECollisionChannel::ECC_Visibility, QueryParams)) {
					DrawDebugLine(World, CurrentPrimary->MuzzleLocation->GetComponentLocation(), hit.Location, FColor::Red, true);
					UE_LOG(LogClass, Log, TEXT("We hit %s"), *hit.GetActor()->GetName());
					if (AFPSCharacter* const hitplayer = Cast<AFPSCharacter>(hit.GetActor())) {
						hitplayer->Shooter = this;
						hitplayer->ServerChangeHealthBy(-5.0f);
						hitplayer->TriggerUpdateUI();
						//UE_LOG(LogClass, Log, FString::SanitizeFloat(hitplayer->GetCurrentHealth());
						//hitplayer->HealthPercentage = hitplayer->GetCurrentHealth() / hitplayer->GetInitialHealth();
						GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, FString::SanitizeFloat(hitplayer->GetCurrentHealth()));
						GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, FString::SanitizeFloat(hitplayer->HealthPercentage));
						//UE_LOG(LogClass, Log, TEXT("%s health is now %s"), *hitplayer->GetName(), FString::SanitizeFloat(hitplayer->GetCurrentHealth()));
					}
				}

			}
		}
	}
}


// Called when the game starts or when spawned
void AFPSCharacter::BeginPlay()
{
	Super::BeginPlay();
	CurrentState = EPlayerState::EPlayerPlaying;

	switch (GetCurrentState())
	{
	case EPlayerState::EPlayerWaiting:

		break;
	case EPlayerState::EPlayerPlaying:


		break;
	case EPlayerState::EPlayerDead:

		break;
	default:
	case EPlayerState::EPlayerUnknown:
		break;
	}
	if (GEngine)
	{
		// Put up a debug message for five seconds. The -1 "Key" value (first argument) indicates that we will never need to update or refresh this message.
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("We are using FPSCharacter."));
	}
}


void AFPSCharacter::PickupWeapon()
{
	ServerPickupWeapon();
}

bool AFPSCharacter::ServerPickupWeapon_Validate()
{
	return true;
}

void AFPSCharacter::ServerPickupWeapon_Implementation()
{
	if (Role == ROLE_Authority)
	{
		// Get all overlapping actors and store them in an array
		TArray<AActor*> CollectedActors;
		CollectionBox->GetOverlappingActors(CollectedActors);

		for (int i = 0; i < CollectedActors.Num(); ++i)
		{
			AGun* const Gun = Cast<AGun>(CollectedActors[i]);
			if (Gun != NULL && !Gun->IsPendingKill() && Gun->IsActive()) {
				CurrentPrimary = Gun;
				Gun->PickedUpBy(this);

				UE_LOG(LogClass, Log, TEXT("Current Primary: %s"), *CurrentPrimary->GetName());
				HasWeapon = true;

			}
		}
	}
}

void AFPSCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFPSCharacter, FPSMesh);
	DOREPLIFETIME(AFPSCharacter, FPSCameraComponent);
	DOREPLIFETIME(AFPSCharacter, CurrentPrimary)

}

// Called every frame
void AFPSCharacter::Tick(float DeltaTime)
{
	if (CurrentState == EPlayerState::EPlayerWaiting)
	{
		GetCharacterMovement()->DisableMovement();
	}
	if (Role == ROLE_AutonomousProxy)
	{
		ServerUpdateRotation(FPSCameraComponent->GetComponentRotation());
	}
	//WeaponHoldLocation->SetWorldLocation(FPSMesh->GetSocketLocation(FName(TEXT("WeaponLocation"))));
}

// Called to bind functionality to input
void AFPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set up "movement" bindings.
	PlayerInputComponent->BindAxis("MoveForward", this, &AFPSCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AFPSCharacter::MoveRight);

	// Set up "look" bindings.
	PlayerInputComponent->BindAxis("Turn", this, &AFPSCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &AFPSCharacter::AddControllerPitchInput);

	// Set up "action" bindings.
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AFPSCharacter::StartJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AFPSCharacter::StopJump);
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AFPSCharacter::PickupWeapon);
	PlayerInputComponent->BindAction("Shoot", IE_Pressed, this, &AFPSCharacter::OnShoot);
	PlayerInputComponent->BindAction("Shoot", IE_Released, this, &AFPSCharacter::OnStopShoot);
}


void AFPSCharacter::MoveForward(float Value)
{
	// Find out which way is "forward" and record that the player wants to move that way.
	FVector Direction = FRotationMatrix(FRotator(0, Controller->GetControlRotation().Yaw, 0)).GetScaledAxis(EAxis::X);
	AddMovementInput(Direction, Value);
}

void AFPSCharacter::MoveRight(float Value)
{
	// Find out which way is "right" and record that the player wants to move that way.
	FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::Y);
	AddMovementInput(Direction, Value);
}

void AFPSCharacter::StartJump()
{
	bPressedJump = true;
}

void AFPSCharacter::StopJump()
{
	bPressedJump = false;
}
