// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "Base3C.h"
#include "Templates/SubclassOf.h"
#include "Particles/ParticleSystem.h"
#include "StunWeapon.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTRANSACK_API UStunWeapon : public UWeapon
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")
	TArray<TSubclassOf<AActor>> EnemyHittable;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	UParticleSystem* particleEffect = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Reach = 10000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StunDuration = 5;

	UStunWeapon(const FObjectInitializer& ObjectInitializer);
	
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void Fire() override;

	AActor* HitScan(FVector& hitLocation);

	bool CheckHittableActor(AActor* pActorToCheck);

	void HitEntity(class AGamePlayerController* PlayerController, AActor* pActorToHit);
};
