// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Inventory.h"
#include "EnumTeam.h"
#include "Blueprint/UserWidget.h"
#include "GamePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTRANSACK_API AGamePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "widget")
	TSubclassOf<class UDuffleBagUI> DuffleBagUIClass;
	class UDuffleBagUI* DuffleBagUIWidget = nullptr;

	UPROPERTY(EditAnywhere, Category = "widget")
	TSubclassOf<class UTeamDuffleBagUI> TeamDuffleBagUIClass;
	class UTeamDuffleBagUI* TeamDuffleBagUIWidget = nullptr;

	UPROPERTY(EditAnywhere, Category = "widget")
	TSubclassOf<class URoundUI> RoundUIClass;
	class URoundUI* RoundUIWidget = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<class ABase3C*> TeamA;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<class ABase3C*> TeamB;

	TMap<class AItem*, class UItemWidgetUI*> interactibleUI;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	void PawnIsPossess(APawn* InPawn);

	virtual void SetPawn(APawn* InPawn) override;

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SRSpawnPlayer(const ETeam& pTeam);
	void SRSpawnPlayer_Implementation(const ETeam& pTeam);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SRUpdatePlayerName();
	void SRUpdatePlayerName_Implementation();

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SRGetTeamData();
	void SRGetTeamData_Implementation();

	UFUNCTION(Client, Reliable, BlueprintCallable)
	void ClientGetTeamA(const TArray<class ABase3C*>& pTeamA);
	void ClientGetTeamA_Implementation(const TArray<class ABase3C*>& pTeamA);

	UFUNCTION(Client, Reliable, BlueprintCallable)
	void ClientGetTeamB(const TArray<class ABase3C*>& pTeamB);
	void ClientGetTeamB_Implementation(const TArray<class ABase3C*>& pTeamB);

	void UpdateDuffleBagUI(const TArray<FItemLooted>& pItems);

	void UpdateTeamDuffleBagUI();

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SRUpdateTeamDuffleBagUI();
	void SRUpdateTeamDuffleBagUI_Implementation();

	UFUNCTION(Client, Reliable, BlueprintCallable)
	void ClientUpdateTeamDuffleBagUI();
	void ClientUpdateTeamDuffleBagUI_Implementation();

	UFUNCTION(Client, Unreliable, BlueprintCallable)
	void ClientUpdateRoundTimeRemaining(const FString& pTime);
	void ClientUpdateRoundTimeRemaining_Implementation(const FString& pTime);


	void AddInteractibleWidgetUI(class AItem* pItem, TSubclassOf<UItemWidgetUI> pWidget);
	void RemoveInteractibleWidgetUI(class AItem* pItem);

	UItemWidgetUI* GetWidget(class AItem* pItem);
};
