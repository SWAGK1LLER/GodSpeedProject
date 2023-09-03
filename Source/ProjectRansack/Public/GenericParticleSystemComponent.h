// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Particles/ParticleSystemComponent.h"
#include "GenericParticleSystemComponent.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTRANSACK_API UGenericParticleSystemComponent : public UParticleSystemComponent
{
	GENERATED_BODY()
public:
	float duration = 0;
	float timeCalculate = 0;
	bool startTimer = false;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void setLifeSpan(float pDuration);
};
