// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FPSPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class FPSPROJECT_API AFPSPlayerController : public APlayerController
{
	GENERATED_BODY()

		FTimerHandle TimerHandle_Respawn;

public:



	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSpawnPlayer();
	virtual void ServerSpawnPlayer_Implementation();
	virtual bool ServerSpawnPlayer_Validate();
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSpawnPlayerClient();
	virtual void ServerSpawnPlayerClient_Implementation();
	virtual bool ServerSpawnPlayerClient_Validate();
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Interface")
	void ServerSetPlayerTeam(int32 NewTeam);
	virtual void ServerSetPlayerTeam_Implementation(int32 NewTeam);
	virtual bool ServerSetPlayerTeam_Validate(int32 NewTeam);
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Interface")
	void ServerSetPlayerTeamClient(int32 NewTeam);
	virtual void ServerSetPlayerTeamClient_Implementation(int32 NewTeam);
	virtual bool ServerSetPlayerTeamClient_Validate(int32 NewTeam);



	UFUNCTION(BlueprintCallable, Category = "Interface")
	int32 GetPlayerTeam();

	void OnKilled();

	void Respawn();
private:
	UPROPERTY(Replicated,VisibleAnywhere, Category = "Team")
		int32 TeamNumber = 0;
	
};
