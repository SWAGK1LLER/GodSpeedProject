// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "Base3C.h"
#include "Templates/SubclassOf.h"
#include "StunWeapon.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTRANSACK_API UStunWeapon : public UWeapon
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	TSubclassOf<ABase3C> EnemyHittable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Reach = 1000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StunDuration = 5;

	UStunWeapon(const FObjectInitializer& ObjectInitializer);
	
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void Fire() override;

	AActor* HitScan();
};
