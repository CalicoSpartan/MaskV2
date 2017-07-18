// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSProjectGameModeBase.h"
#include "FPSCharacter.h"
#include "PlayerHUD.h"
#include "UObject/ConstructorHelpers.h"
#include "Runtime/Engine/Classes/GameFramework/GameMode.h"
#include "FPSPlayerState.h"

#include "FPSPlayerController.h"
#include "Engine.h"

AFPSProjectGameModeBase::AFPSProjectGameModeBase()
{

	
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/BP_FPSCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
	
	// set the type of HUD used in the game
	static ConstructorHelpers::FClassFinder<AHUD> PlayerHUDClass(TEXT("/Game/Blueprints/BP_PlayerHUD"));
	if (PlayerHUDClass.Class != NULL) {
		HUDClass = PlayerHUDClass.Class;
	}


	Team1Players = 0;
	Team2Players = 0;
	Team3Players = 0;
	Team4Players = 0;
	Team5Players = 0;

	HasLoggedIn = false;
	NumTeamA = 0;
	NumTeamB = 0;
	SpawnDelay = 2;

	AddKillFeedEntry = false;
	//set the type of gamestate used in the game
	GameStateClass = AFPSGameState::StaticClass();

	PlayerControllerClass = AFPSPlayerController::StaticClass();

	PlayerStateClass = AFPSPlayerState::StaticClass();
	NumberOfTeams = 2;
}


void AFPSProjectGameModeBase::StartNewPlayerClient(APlayerController* NewPlayer)
{
	NewPlayer->GetPawn()->Destroy();
	AFPSPlayerController* TestPlayerController = Cast<AFPSPlayerController>(NewPlayer);
	TArray<AFPSPlayerStart*> PreferredStarts;
	bool BlockCheck = true;
	for (int32 i = 0; i < 2; ++i) {

		for (TActorIterator<AFPSPlayerStart> PlayerStart(GetWorld()); PlayerStart; ++PlayerStart)
		{

			if (TestPlayerController)
			{
				if (TestPlayerController->GetPlayerTeam() == 1 && PlayerStart->Tags.Contains("Team1")) //&& PlayerStart->PlayerStartTag != FName(TEXT("Blocked")))
				{
					if (PlayerStart->PlayerStartTag == FName(TEXT("Blocked"))) {
						if (BlockCheck == false) {
							UE_LOG(LogClass, Log, TEXT("spawned anyway"));
							PreferredStarts.AddUnique(*PlayerStart);
						}
						else {
							UE_LOG(LogClass, Log, TEXT("team1 spawn blocked"));
						}
						
					}
					else
					{
						UE_LOG(LogClass, Log, TEXT("found team1 spawn with starttag: %s"), *PlayerStart->PlayerStartTag.ToString());
						PreferredStarts.AddUnique(*PlayerStart);
					}
					// Player should spawn on CT.
					

				}
				else if (TestPlayerController->GetPlayerTeam() == 2 && PlayerStart->Tags.Contains("Team2")) //&& PlayerStart->PlayerStartTag != FName(TEXT("Blocked")))
				{
					if (PlayerStart->PlayerStartTag == FName(TEXT("Blocked"))) {
						if (BlockCheck == false) {
							UE_LOG(LogClass, Log, TEXT("spawned anyway"));
							PreferredStarts.AddUnique(*PlayerStart);
						}
						else {
							UE_LOG(LogClass, Log, TEXT("team2 spawn blocked"));
						}
						
					}
					else {
						UE_LOG(LogClass, Log, TEXT("found team2 spawn with starttag: %s"), *PlayerStart->PlayerStartTag.ToString());
						PreferredStarts.AddUnique(*PlayerStart);
					}
					// Player should spawn on Suspects
					

				}

			}

		}
		if (PreferredStarts.Num() == 0) {
			UE_LOG(LogClass, Log, TEXT("allowing all spawns"));
			BlockCheck = false;
		}
		else {
			break;
		}
		
	}
	if (TestPlayerController && TestPlayerController->GetPlayerTeam() == 0)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, "Player is a spectator");
		return;
	}
	else
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, "Player has selected a Team");
		int32 PlayerStartIndex = FMath::RandRange(0, PreferredStarts.Num() - 1);
		NewPlayer->SetPawn(SpawnDefaultPawnFor(NewPlayer, PreferredStarts[PlayerStartIndex]));
		PreferredStarts[PlayerStartIndex]->PlayerStartTag = FName(TEXT("Blocked"));

		RestartPlayer(NewPlayer);
	}
}


void AFPSProjectGameModeBase::StartNewPlayer(APlayerController* NewPlayer)
{
	
	AFPSPlayerController* TestPlayerController = Cast<AFPSPlayerController>(NewPlayer);

	TArray<AFPSPlayerStart*> PreferredStarts;



	for (TActorIterator<AFPSPlayerStart> PlayerStart(GetWorld()); PlayerStart; ++PlayerStart)
	{

		
		if (TestPlayerController)
		{

			if (TestPlayerController->GetPlayerTeam() == 1 && PlayerStart->Tags.Contains("Team1"))
			{
				
				// Player should spawn on CT.
				PreferredStarts.Add(*PlayerStart);

			}
			else if (TestPlayerController->GetPlayerTeam() == 2 && PlayerStart->Tags.Contains("Team2"))
			{
				
				// Player should spawn on Suspects
				PreferredStarts.Add(*PlayerStart);

			}
			
		}
	}
	if (TestPlayerController && TestPlayerController->GetPlayerTeam() == 0)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, "Player is a spectator");
		return;
	}
	else
	{
		if (TestPlayerController->GetPlayerTeam() == 1) {
			Team1Players += 1;
			UE_LOG(LogClass, Log, TEXT("server chose team 1"));
		}
		if (TestPlayerController->GetPlayerTeam() == 2) {
			Team2Players += 1;
			UE_LOG(LogClass, Log, TEXT("server chose team 2"));
		}
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, "Player has selected a Team");
		NewPlayer->GetPawn()->Destroy();
		int32 StartSpotIndex = FMath::RandRange(0, PreferredStarts.Num() - 1);
		NewPlayer->SetPawn(SpawnDefaultPawnFor(NewPlayer, PreferredStarts[StartSpotIndex]));
		for (TActorIterator<AFPSPlayerStart> ClearPlayerStart(GetWorld()); ClearPlayerStart; ++ClearPlayerStart) {
			ClearPlayerStart->PlayerStartTag = FName(TEXT("Open"));
		}

		PreferredStarts[StartSpotIndex]->PlayerStartTag = FName(TEXT("Blocked"));

		RestartPlayer(NewPlayer);
	}


	//TArray<AFPSPlayerController> PlayerControllers;
	//UGameplayStatics::GetAllActorsOfClass(GetWorld(), AFPSPlayerController::StaticClass(), PlayerControllers);

	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		AFPSPlayerController* PlayerController = Cast<AFPSPlayerController>(*Iterator);
		if (PlayerController) {
			if (PlayerController != NewPlayer) {
				if (Team1Players > Team2Players) {
					UE_LOG(LogClass, Log, TEXT("player assigned to team 2"));
					Team2Players++;
					PlayerController->ServerSetPlayerTeamClient(2);
				}
				else {
					UE_LOG(LogClass, Log, TEXT("player assigned to team 1"));
					Team1Players++;
					PlayerController->ServerSetPlayerTeamClient(1);
				}
			}
			
		}
		
		
	}
	//UE_LOG(LogClass, Log, TEXT("Team1: %s   Team2: %d"),Team1Players,Team2Players);
}







void AFPSProjectGameModeBase::PostLogin(APlayerController * NewPlayer)
{
	Super::PostLogin(NewPlayer);
	
}

/*
AActor * AFPSProjectGameModeBase::ChoosePlayerStart_Implementation(AController * Player)
{
	for (TActorIterator<AFPSPlayerStart> StartItr(GetWorld()); StartItr; ++StartItr)
	{
		if (StartItr->TeamNumber == 1) {
			Team1PlayerStarts.AddUnique(*StartItr);
			UE_LOG(LogClass, Log, TEXT("added spawn to team 1"));

		}
		if (StartItr->TeamNumber == 2) {
			Team2PlayerStarts.AddUnique(*StartItr);
			UE_LOG(LogClass, Log, TEXT("added spawn to team 2"));
		}
		if (StartItr->TeamNumber == 3) {
			Team3PlayerStarts.AddUnique(*StartItr);
		}
		if (StartItr->TeamNumber == 4) {
			Team4PlayerStarts.AddUnique(*StartItr);
		}
		if (StartItr->TeamNumber == 5) {
			Team5PlayerStarts.AddUnique(*StartItr);
		}
	}
	if (Player)
	{
		AFPSPlayerState * PS = Cast<AFPSPlayerState>(Player->PlayerState);
		if (PS) {
			if (NumberOfTeams == 2) {

				if (Team1Players > Team2Players)
				{

					PS->TeamNumber = 2;
					Team2Players++;
					UE_LOG(LogClass, Log, TEXT("joined team2 and spawned at a team2 spawnpoint"));
					return Team2PlayerStarts[FMath::RandRange(0, Team2PlayerStarts.Num() - 1)];
						
				}
				else {
					PS->TeamNumber = 1;
					Team1Players++;
					UE_LOG(LogClass, Log, TEXT("joined team1 and spawned at a team1 spawnpoint"));
					return Team1PlayerStarts[FMath::RandRange(0, Team1PlayerStarts.Num() - 1)];
				}

			}
		}
	}
	return NULL;
}
*/
/*
AActor * AFPSProjectGameModeBase::ChoosePlayerStart_Implementation(AController * Player)
{
	UE_LOG(LogClass, Log, TEXT("//////////////////////NEW PLAYER CHOOSING START"));
	if (Player)
	{

		AFPSPlayerState * PS = Cast<AFPSPlayerState>(Player->PlayerState);
		if (PS)
		{
			bool stop = false;
			//return Team1PlayerStarts[FMath::RandRange(0, Team1PlayerStarts.Num() - 1)];
			if (HasLoggedIn == true)
			{ 
				if (NumberOfTeams == 2) {
					if (Team1Players > Team2Players)
					{
						PS->TeamNumber = 2;
						UE_LOG(LogClass, Log, TEXT("chose team 2"));

						//stop = true;
						Team2Players++;

					}
					else
					{
						PS->TeamNumber = 1;
						UE_LOG(LogClass, Log, TEXT("chose team 1"));

						//stop = true;
						Team1Players++;

					}



					UE_LOG(LogClass, Log, TEXT("pENIS"));
					//HasLoggedIn = true;


					if (PS->TeamNumber == 1) {
						//PS->GetInstigatorController()->GetControlledPawn()->GetName();
						//stop = false;
						UE_LOG(LogClass, Log, TEXT("spawned at team1 starts"));
				
						//GEngine->AddOnScreenDebugMessage(-1, 20, FColor::Blue, PS->GetName() + " is on team 1");
						return Team1PlayerStarts[FMath::RandRange(0, Team1PlayerStarts.Num() - 1)];

					}
					if (PS->TeamNumber == 2) {
						//PS->GetInstigatorController()->GetControlledPawn()->GetName();

						//stop = false;
						UE_LOG(LogClass, Log, TEXT("spawned at team2 starts"));
					
						//GEngine->AddOnScreenDebugMessage(-1, 20, FColor::Blue, PS->GetName() + "is on team 2");
						return Team2PlayerStarts[FMath::RandRange(0, Team2PlayerStarts.Num() - 1)];

					}
					if (PS->TeamNumber == 0) {
						UE_LOG(LogClass, Log, TEXT("zero"));
					
						return Team1PlayerStarts[FMath::RandRange(0, Team1PlayerStarts.Num() - 1)];

					}




				}
			}
			

		}
	}
	
	return NULL;
	
}

*/
void AFPSProjectGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	check(World);
	AFPSGameState* MyGameState = Cast<AFPSGameState>(GameState);
	check(MyGameState);
	HandleNewState(EGamePlayState::EPlaying);

	



}

void AFPSProjectGameModeBase::Update()
{
	UWorld* World = GetWorld();
	check(World);
	AFPSGameState* MyGameState = Cast<AFPSGameState>(GameState);
	check(MyGameState);

	/*
	for (FConstControllerIterator It = World->GetControllerIterator(); It; ++It) {
		if (APlayerController* PlayerController = Cast<APlayerController>(*It)) {
			if (AFPSCharacter* Player = Cast<AFPSCharacter>(PlayerController->GetPawn())) {
				if (Player->GetCurrentHealth() <= 0.0f) {
					if (Player->Shooter == NULL) {
						Player->Shooter = Player;
					}
					//UE_LOG(LogClass, Log, TEXT("KILLED"));
					if (APlayerHUD* HUD = Cast<APlayerHUD>(UGameplayStatics::GetPlayerController(this,0)->GetHUD())){
						
						LastKiller = Player->Shooter->GetName();
						LastVictim = Player->GetName();
						AddKillFeedEntry = true;
						FString KillFeedMessage = (TEXT("%s killed %s"), LastKiller, LastVictim);
						MyGameState->SetNewKillFeedMessage(KillFeedMessage);
						MyGameState->CallHUDUpdate();
						Player->OnPlayerDeath();
						//TheHUD->AddMessageEvent();
						ActuallyAddKillFeedEntry();
								
							
						
					}





				}
			}
		}
	}
	*/
}

void AFPSProjectGameModeBase::HandleNewState(EGamePlayState NewState)
{
	UWorld* World = GetWorld();
	check(World);
	AFPSGameState* MyGameState = Cast<AFPSGameState>(GameState);
	check(MyGameState);

	if (NewState != MyGameState->GetCurrentState())
	{
		//update the state, so clients know about the transition
		MyGameState->SetCurrentState(NewState);

		switch (NewState)
		{
		case EGamePlayState::EWaiting:

			break;
		case EGamePlayState::EPlaying:

			//start draining power
			GetWorldTimerManager().SetTimer(UpdateTimer, this, &AFPSProjectGameModeBase::Update, UpdateDelay, true);
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

}



