// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Base3C.h"
#include "Inventory.h"
#include "Thief.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTRANSACK_API AThief : public ABase3C
{
	GENERATED_BODY()

public:
	UPROPERTY(/*replicated,*/ EditAnywhere, BlueprintReadWrite)
	UInventory* inventory = nullptr;

	AThief();

	//To replicate inventory on network
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const;
	
	void AddItem(class AItem& pItem);
	bool ValidateSpaceItem(class AItem& pItem);
};
