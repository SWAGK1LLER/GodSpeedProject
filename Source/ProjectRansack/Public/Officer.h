// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Base3C.h"
#include "OfficerInteractibleActor.h"
#include "Officer.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTRANSACK_API AOfficer : public ABase3C
{
	GENERATED_BODY()

public:
	TArray<IOfficerInteractibleActor*> closeItems;

	virtual void Interact() override;
};
