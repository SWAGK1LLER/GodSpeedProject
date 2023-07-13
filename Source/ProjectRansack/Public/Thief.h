// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Base3C.h"
#include "Inventory.h"
#include "ThiefInteractibleActor.h"
#include "Thief.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTRANSACK_API AThief : public ABase3C
{
	GENERATED_BODY()

public:
	TArray<IThiefInteractibleActor*> closeItems;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UInventory* inventory = nullptr;

	AThief();
	
	void AddItem(class AItem& pItem);
	bool ValidateSpaceItem(class AItem& pItem);

	virtual void Interact() override;
};
