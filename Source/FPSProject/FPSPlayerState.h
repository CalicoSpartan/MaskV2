// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "FPSPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class FPSPROJECT_API AFPSPlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	UPROPERTY(Replicated,VisibleAnywhere,Category = "Team")
		bool bTeamB;
	UPROPERTY(Replicated, VisibleAnywhere, Category = "Team")
		int32 TeamNumber = 1;
	//required network scaffolding
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
};
