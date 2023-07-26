// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Base3C.h"
#include "Inventory.h"
#include "ThiefInteractibleActor.h"
#include "EOSGameInstance.h"
#include "Thief.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTRANSACK_API AThief : public ABase3C
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int costOnArrest = 400;

	TArray<IThiefInteractibleActor*> closeItems;
	IThiefInteractibleActor* ItemUsing = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UInventory* inventory = nullptr;

	AThief();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SRAddItem(class AItem* pItem);
	void SRAddItem_Implementation(class AItem* pItem);

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void MUlAddItem(class AItem* pItem);
	void MUlAddItem_Implementation(class AItem* pItem);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SRClearItems(int score, ETeam pTeam);
	void SRClearItems_Implementation(int score, ETeam pTeam);

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void MUlClearItems();
	void MUlClearItems_Implementation();


	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SRDropInventory();
	void SRDropInventory_Implementation();

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void MUlDropInventory();
	void MUlDropInventory_Implementation();


	bool ValidateSpaceItem(class AItem& pItem);

	void ChangeStencilOnMovement();

	virtual void Interact() override;
	virtual void StopInteract() override;
};
