#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Inventory.h"
#include "EnumTeam.h"
#include "RespawnUI.h"
#include "Blueprint/UserWidget.h"
#include <GamePauseMenu.h>
#include "GamePlayerController.generated.h"

UCLASS()
class PROJECTRANSACK_API AGamePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UGamePauseMenu> PauseMenuClass;
	UGamePauseMenu* PauseUI = nullptr;
	bool isPaused = false;
	float time = 0;

	UPROPERTY(EditAnywhere, Category = "widget")
	TSubclassOf<class UDuffleBagUI> DuffleBagUIClass;
	class UDuffleBagUI* DuffleBagUIWidget = nullptr;

	UPROPERTY(EditAnywhere, Category = "widget")
	TSubclassOf<class UTeamDuffleBagUI> TeamDuffleBagUIClass;
	class UTeamDuffleBagUI* TeamDuffleBagUIWidget = nullptr;

	UPROPERTY(EditAnywhere, Category = "widget")
	TSubclassOf<URespawnUI> RespawnWidgetClass;
	URespawnUI* RespawnUI;
	float respawnCurrentTime = 0;

	UPROPERTY(EditAnywhere, Category = "widget")
	TSubclassOf<class URoundUI> RoundUIClass;
	class URoundUI* RoundUIWidget = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<class ABase3C*> TeamA; //Thief

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<class ABase3C*> TeamB; //Security

	TMap<class AActor*, class UUserWidget*> interactibleUI;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	void PawnIsPossess(APawn* InPawn);

	virtual void SetPawn(APawn* InPawn) override;

	UFUNCTION(Client, Reliable, BlueprintCallable)
	void SetUpUI(APawn* InPawn);
	void SetUpUI_Implementation(APawn* InPawn);

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

	void ToogleTeamDuffleBagUI(bool show);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SRUpdateTeamDuffleBagUI();
	void SRUpdateTeamDuffleBagUI_Implementation();

	UFUNCTION(Client, Reliable, BlueprintCallable)
	void ClientUpdateTeamDuffleBagUI();
	void ClientUpdateTeamDuffleBagUI_Implementation();

	UFUNCTION(Client, Unreliable, BlueprintCallable)
	void ClientUpdateRoundTimeRemaining(const FString& pTime);
	void ClientUpdateRoundTimeRemaining_Implementation(const FString& pTime);

	UFUNCTION(Client, Unreliable, BlueprintCallable)
	void ClientUpdateCustomTimer(const FString& text, const float& pTime);
	void ClientUpdateCustomTimer_Implementation(const FString& text, const float& pTime);

	UFUNCTION(Client, Unreliable, BlueprintCallable)
	void ClientUpdateScore(int pTeamA, int pTeamB);
	void ClientUpdateScore_Implementation(int pTeamA, int pTeamB);


	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SRFreezeInput(float duration, ABase3C* actor, AActor* pActor);
	void SRFreezeInput_Implementation(float duration, ABase3C* actor, AActor* pActor);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void CameraFreezeInput(AActor* actor);
	void CameraFreezeInput_Implementation(AActor* actor);

	UUserWidget* AddInteractibleWidgetUI(class AActor* pItem, TSubclassOf<UUserWidget> pWidget);
	void RemoveInteractibleWidgetUI(class AActor* pItem);

	UUserWidget* GetWidget(class AActor* pItem);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SRToggleLights(ALightFuseBoxe* actor, bool pOpen);
	void SRToggleLights_Implementation(ALightFuseBoxe* actor, bool pOpen);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SRToogleHackLights(ALightFuseBoxe* actor, bool isHack);
	void SRToogleHackLights_Implementation(ALightFuseBoxe* actor, bool isHack);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SRToggleDoor(ADoor* actor, bool pOpen, bool manully);
	void SRToggleDoor_Implementation(ADoor* actor, bool pOpen, bool manully);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SRToogleHackDoor(class ASlidingDoor* actor, bool isHack);
	void SRToogleHackDoor_Implementation(class ASlidingDoor* actor, bool isHack);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SRToggleCameras(class ATerminal* actor, bool pOn);
	void SRToggleCameras_Implementation(ATerminal* actor, bool pOn);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SRToogleHackTerminal(ATerminal* actor, bool isHack);
	void SRToogleHackTerminal_Implementation(ATerminal* actor, bool isHack);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SRSpawnParticle(class UParticleSystem* particleEffect, const FTransform& position, const float& duration);
	void SRSpawnParticle_Implementation(class UParticleSystem* particleEffect, const FTransform& position, const float& duration);

	UFUNCTION(Client, Reliable, BlueprintCallable)
	void ClientSpawnParticle(class UParticleSystem* particleEffect, const FTransform& position, const float& duration);
	void ClientSpawnParticle_Implementation(class UParticleSystem* particleEffect, const FTransform& position, const float& duration);

	UFUNCTION(Client, Reliable, BlueprintCallable)
	void ClientBeingArrest();
	void ClientBeingArrest_Implementation();

	UFUNCTION(Client, Reliable, BlueprintCallable)
	void ClientFinishArrest(float respawnDuration);
	void ClientFinishArrest_Implementation(float respawnDuration);

	UFUNCTION(Client, Reliable, BlueprintCallable)
	void RestartRound();
	void RestartRound_Implementation();

	UFUNCTION(Client, Reliable, BlueprintCallable)
	void EndGame(bool isWin, bool isTie);
	void EndGame_Implementation(bool isWin, bool isTie);

	UFUNCTION()
	void SaveGameFinish();

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SRBeginArrestThief(AThief* pThief, bool pArrest, AOfficer* pOfficer);
	void SRBeginArrestThief_Implementation(AThief* pThief, bool pArrest, AOfficer* pOfficer);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SendPingRequest(class ASensorGadget* sensor, class AThief* thief);
	void SendPingRequest_Implementation(class ASensorGadget* sensor, class AThief* thief);

	UFUNCTION(Client, Reliable, BlueprintCallable)
	void PingPlayerToSensor(class ASensorGadget* sensor, class AThief* thief);
	void PingPlayerToSensor_Implementation(class ASensorGadget* sensor, class AThief* thief);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void stealMagnetCard(class AThief* thief, class AOfficer* officer);
	void stealMagnetCard_Implementation(class AThief* thief, class AOfficer* officer);

	UFUNCTION(BlueprintCallable)
	void TogglePauseMenu();

	void CenterViewportCursor();

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void MUlSetLerpRot(class UCameraCompThief* comp, FRotator rot);
	void MUlSetLerpRot_Implementation(class UCameraCompThief* comp, FRotator rot);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void MUlSetRot(class UCameraCompThief* comp, FRotator rot);
	void MUlSetRot_Implementation(class UCameraCompThief* comp, FRotator rot);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void MUlPlayAttackAnim(class UStunStick* comp);
	void MUlPlayAttackAnim_Implementation(class UStunStick* comp);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void MUlToggleEquipStunBaton(class UStunStick* comp, bool visibility);
	void MUlToggleEquipStunBaton_Implementation(class UStunStick* comp, bool visibility);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void MUlToggleEquipGrenade(class UGrenadeTrajectory* comp, bool visibility);
	void MUlToggleEquipGrenade_Implementation(class UGrenadeTrajectory* comp, bool visibility);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SRDisableSystem(class AProtectionLoot* actor);
	void SRDisableSystem_Implementation(class AProtectionLoot* actor);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void TryClimb(class AThief* thief);
	void TryClimb_Implementation(class AThief* thief);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void TryToggleCover(class AThief* thief);
	void TryToggleCover_Implementation(class AThief* thief);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SRThrowGrenade(class AThief* thief);
	void SRThrowGrenade_Implementation(class AThief* thief);
};
