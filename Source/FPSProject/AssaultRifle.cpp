// Fill out your copyright notice in the Description page of Project Settings.

#include "AssaultRifle.h"
#include "FPSCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"


AAssaultRifle::AAssaultRifle()
{
	//keep movement synced from server to clients
	bReplicates = true;
	bReplicateMovement = true;
	//this pickup is physics enabled and should move
	GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
	GetStaticMeshComponent()->SetSimulatePhysics(true);
	GetStaticMeshComponent()->SetEnableGravity(true);
	IsProjectile = false;
	IsAutomatic = true;
	IsBurst = false;
	FireRate = 0.1f;
	ReloadTime = 2.5f;
	TotalAmmo = 150;
	Range = 20000;
	BulletDamage = 5.0f;
	MagazineSize = 25;
	AmmoLeftInMag = MagazineSize;
	IsExplosive = false;
}

void AAssaultRifle::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

}

/*
void AAssaultRifle::PickedUpBy(APawn * Pawn)
{
Super::PickedUpBy(Pawn);


}
*/
