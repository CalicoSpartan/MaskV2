// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSPlayerController.h"
#include "FPSProjectGameModeBase.h"
#include "Net/UnrealNetwork.h"


void AFPSPlayerController::OnKilled()
{
	/*
	UnPossess();
	UE_LOG(LogClass, Log, TEXT("should respawn"));
	GetWorldTimerManager().SetTimer(TimerHandle_Respawn, this, &AFPSPlayerController::Respawn,5.0f);
	*/
}


void AFPSPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFPSPlayerController, TeamNumber);


}


void AFPSPlayerController::Respawn()
{
	/*
	AGameModeBase * GameMode = GetWorld()->GetAuthGameMode();
	if (GameMode)
	{
		
		APawn *NewPawn = GameMode->SpawnDefaultPawnFor(this, GameMode->ChoosePlayerStart_Implementation(this));
		
		//GEngine->AddOnScreenDebugMessage(-1, 20, FColor::Blue,NewPawn->GetController()->PlayerState->GetName());
		Possess(NewPawn);
	}
	*/
}


void AFPSPlayerController::ServerSetPlayerTeam_Implementation(int32 NewTeam)
{
	TeamNumber = NewTeam;
	ServerSpawnPlayer();
}

bool AFPSPlayerController::ServerSetPlayerTeam_Validate(int32 NewTeam)
{
	return true;
}

void AFPSPlayerController::ServerSpawnPlayer_Implementation()
{
	// Spawn the new player
	AFPSProjectGameModeBase * GameMode = Cast<AFPSProjectGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GameMode) {
		GameMode->StartNewPlayer(this);
	}
	
}

bool AFPSPlayerController::ServerSpawnPlayer_Validate()
{
	return true;
}


void AFPSPlayerController::ServerSetPlayerTeamClient_Implementation(int32 NewTeam)
{
	TeamNumber = NewTeam;
	ServerSpawnPlayerClient();
}

bool AFPSPlayerController::ServerSetPlayerTeamClient_Validate(int32 NewTeam)
{
	return true;
}

void AFPSPlayerController::ServerSpawnPlayerClient_Implementation()
{
	// Spawn the new player
	AFPSProjectGameModeBase * GameMode = Cast<AFPSProjectGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GameMode) {
		GameMode->StartNewPlayerClient(this);
	}

}

bool AFPSPlayerController::ServerSpawnPlayerClient_Validate()
{
	return true;
}


int32 AFPSPlayerController::GetPlayerTeam()
{
	return TeamNumber;
}