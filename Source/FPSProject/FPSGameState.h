#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "PlayerHUD.h"
#include "FPSCharacter.h"
#include "EngineUtils.h"
#include "SpawnPoint.h"
#include "FPSGameState.generated.h"

UENUM(BlueprintType)
enum EGamePlayState
{
	EWaiting,
	EPlaying,
	EGameOver,
	EUnknown
};


/**
*
*/
UCLASS()
class FPSPROJECT_API AFPSGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	AFPSGameState();

	virtual void BeginPlay() override;


	UPROPERTY(Replicated,BlueprintReadWrite, Category = "KillFeed")
		FString KillFeedMessage;

	UPROPERTY(BlueprintReadWrite)
	FString LastKilled;

	UPROPERTY(BlueprintReadWrite)
	FString LastKiller;

	UFUNCTION(BlueprintCallable)
		void SetNewKillFeedMessage(FString Message);
	UFUNCTION(BlueprintImplementableEvent)
		void CallHUDUpdate();
	UFUNCTION(NetMultiCast,Reliable)
		void ClientUpdateKillFeedMessage(FName text);

	//required network scaffolding
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//this returns the current state of gameplay
	UFUNCTION(BlueprintPure)
		EGamePlayState GetCurrentState() const;
	//transition game into a new play state, Only usable on server
	void SetCurrentState(EGamePlayState NewState);

	// rep notify fired on clients to allow for client side changes in gameplay state
	UFUNCTION()
		void OnRep_CurrentState();

	// name of player who won the game
	UPROPERTY(Replicated, BlueprintReadWrite)
		FString WinningPlayerName;

	//how many times per second to update the game's UI and State
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
		float UpdateDelay;

	FTimerHandle UpdateTimer;
private:

	void Update();
	// track the current playing state
	UPROPERTY(ReplicatedUsing = OnRep_CurrentState)
		TEnumAsByte<enum EGamePlayState> CurrentState;


};

