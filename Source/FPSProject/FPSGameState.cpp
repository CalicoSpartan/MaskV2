#include "FPSGameState.h"
#include "Net/UnrealNetwork.h"

AFPSGameState::AFPSGameState()
{
	CurrentState = EGamePlayState::EUnknown;
	KillFeedMessage = TEXT("WHATSUPP");
}
void AFPSGameState::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	check(World);
	CurrentState = EGamePlayState::EPlaying;

	switch (GetCurrentState())
	{
	case EGamePlayState::EWaiting:

		break;
	case EGamePlayState::EPlaying:

		//start draining power
		GetWorldTimerManager().SetTimer(UpdateTimer, this, &AFPSGameState::Update, UpdateDelay, true);
		break;
	case EGamePlayState::EGameOver:

		//stop draining power
		GetWorldTimerManager().ClearTimer(UpdateTimer);
		break;
	default:
	case EGamePlayState::EUnknown:
		break;
	}


}


void AFPSGameState::ClientUpdateKillFeedMessage_Implementation(FName text)
{

	KillFeedMessage = text.ToString();
	
}


void AFPSGameState::Update()
{
	UWorld* World = GetWorld();
	check(World);
	//UE_LOG(LogClass, Log, TEXT("KILLED"));
	for (FConstControllerIterator It = World->GetControllerIterator(); It; ++It) {
		if (APlayerController* PlayerController = Cast<APlayerController>(*It)) {
			if (AFPSCharacter* Player = Cast<AFPSCharacter>(PlayerController->GetPawn())) {
				if (Player->GetCurrentHealth() <= 0.0f) {
					if (Player->Shooter == NULL) {
						Player->Shooter = Player;
					}
					//UE_LOG(LogClass, Log, TEXT("KILLED"));
					if (APlayerHUD* HUD = Cast<APlayerHUD>(UGameplayStatics::GetPlayerController(this, 0)->GetHUD())) {

						LastKiller = Player->Shooter->GetName();
						LastKilled = Player->GetName();
					
						KillFeedMessage = TEXT("" + LastKiller + " killed " + LastKilled);//(TEXT("%s killed %f"), LastKiller, LastKilled);
						
						ClientUpdateKillFeedMessage(FName(*KillFeedMessage));
					
						//SetNewKillFeedMessage(KillFeedMessage);
						CallHUDUpdate();
						
						Player->OnPlayerDeath();
					
						//TheHUD->AddMessageEvent();




					}
					




				}
				
			}
		}
	}

}




void AFPSGameState::SetNewKillFeedMessage(FString Message)
{

	KillFeedMessage = TEXT("Something");
	
	/*
	if (APlayerHUD* HUD = Cast<APlayerHUD>(GetWorld()->GetFirstPlayerController()->GetHUD())) {
		HUD->AddMessageEvent();
	}
	*/
}

void AFPSGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AFPSGameState, KillFeedMessage);
	DOREPLIFETIME(AFPSGameState, CurrentState);
	DOREPLIFETIME(AFPSGameState, WinningPlayerName);
}

EGamePlayState AFPSGameState::GetCurrentState() const
{
	return CurrentState;
}

void AFPSGameState::SetCurrentState(EGamePlayState NewState)
{
	if (Role == ROLE_Authority) {
		CurrentState = NewState;
	}
}

void AFPSGameState::OnRep_CurrentState()
{

}





