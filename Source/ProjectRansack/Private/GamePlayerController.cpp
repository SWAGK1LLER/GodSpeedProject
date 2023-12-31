#include "GamePlayerController.h"
#include <Base3C.h>
#include "EOSGameInstance.h"
#include "GameGameMode.h"
#include <Kismet/GameplayStatics.h>
#include "Blueprint/UserWidget.h"
#include <Thief.h>
#include "RoundUI.h"
#include "DuffleBagUI.h"
#include "Item.h"
#include "TeamDuffleBagUI.h"
#include "Particles/ParticleSystemComponent.h"
#include "GenericParticleSystemComponent.h"
#include <Officer.h>
#include "LightFuseBoxe.h"
#include <Door.h>
#include "SlidingDoor.h"
#include "Terminal.h"
#include "SensorGadget.h"
#include "CameraCompThief.h"
#include "Baton.h"
#include "ProtectionLoot.h"
#include "MyCharacterMovementComponent.h"
#include "GrenadeTrajectory.h"
#include "DecoyActor.h"
#include "Equipement.h"
#include "DeguiseComp.h"
#include "Engine/SkeletalMesh.h"
#include "Animation/AnimInstance.h"
#include "Claymore.h"
#include "Grenade.h"

void AGamePlayerController::BeginPlay()
{
	Super::BeginPlay();

	if(!IsLocalPlayerController())
		return;

	UEOSGameInstance* instance = Cast<UEOSGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (instance == nullptr)
		return;

	if (RoundUIWidget == nullptr)
	{
		RoundUIWidget = CreateWidget<URoundUI>(GetWorld(), RoundUIClass);
		RoundUIWidget->AddToViewport();
	}

	//debug mimic load save game
	instance->ServerGameSlot.debugSave = !instance->bIsLogin;
	instance->LoadSaveGame();
	//instance->LoadSaveSettings();

	int a = FMath::RandRange(0, 1);
	SRSpawnPlayer(instance->team);
}

void AGamePlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (RespawnUI != nullptr)
	{
		respawnCurrentTime -= DeltaTime;
		RespawnUI->SetTime(respawnCurrentTime);

		if (respawnCurrentTime <= 0)
		{
			RespawnUI->RemoveFromParent();
			RespawnUI = nullptr;
		}
	}
}

void AGamePlayerController::PawnIsPossess(APawn* InPawn)
{
	if (InPawn == nullptr)
		return;

	ABase3C* base = Cast<ABase3C>(InPawn);
	base->DispatchController(this);

	if (!IsLocalPlayerController())
		return;

	base->equipement->GrenateTrajectory->SetIslocalController(true);

	AThief* thief = Cast<AThief>(InPawn);
	if (thief == nullptr)
	{
		AOfficer* officer = Cast<AOfficer>(InPawn);
		if (officer != nullptr)
			officer->SetClientUI();
		SetUpUI(officer);
		return;
	}

	SetUpUI(thief);
	SRGetTeamData();
	SRUpdatePlayerName();
}

void AGamePlayerController::SetUpUI_Implementation(APawn* InPawn)
{
	if (InPawn == nullptr)
		return;

	PauseUI = (UGamePauseMenu*)CreateWidget<UGamePauseMenu>(GetWorld(), PauseMenuClass);
	PauseUI->AddToViewport(10000);

	AThief* thief = Cast<AThief>(InPawn);
	if (thief == nullptr)
		return;

	if (DuffleBagUIWidget == nullptr)
	{
		DuffleBagUIWidget = CreateWidget<UDuffleBagUI>(GetWorld(), DuffleBagUIClass);
		DuffleBagUIWidget->AddToViewport();
	}
	else
	{
		if (thief->inventory == nullptr)
			thief->inventory = NewObject<UInventory>();

		DuffleBagUIWidget->UpdateUI(thief->inventory->items);
	}

	if (TeamDuffleBagUIWidget == nullptr)
	{
		TeamDuffleBagUIWidget = CreateWidget<UTeamDuffleBagUI>(GetWorld(), TeamDuffleBagUIClass);
		TeamDuffleBagUIWidget->AddToViewport();
	}

	thief->SetClientUI();
}

void AGamePlayerController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);

	ABase3C* player = Cast<ABase3C>(InPawn);
	if (player != nullptr)
		player->BindInputHandler();

	PawnIsPossess(InPawn);
}

void AGamePlayerController::SRSpawnPlayer_Implementation(const ETeam& pTeam)
{
	AGameGameMode* gameMode = Cast<AGameGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (gameMode != nullptr)
		gameMode->SpawnPlayer(pTeam, this);
}

void AGamePlayerController::SRUpdatePlayerName_Implementation()
{
	AGameGameMode* gameMode = Cast<AGameGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (gameMode != nullptr)
		gameMode->SendPlayerName();
}

void AGamePlayerController::SRGetTeamData_Implementation()
{
	AGameGameMode* gameMode = Cast<AGameGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (gameMode != nullptr)
		gameMode->SendTeamData();
}

void AGamePlayerController::ClientGetTeamA_Implementation(const TArray<ABase3C*>& pTeamA)
{
	TeamA = pTeamA;
	TeamA.Remove(Cast<ABase3C>(GetPawn()));

	UpdateTeamDuffleBagUI();
}

void AGamePlayerController::ClientGetTeamB_Implementation(const TArray<ABase3C*>& pTeamB)
{
	TeamB = pTeamB;
	TeamB.Remove(Cast<ABase3C>(GetPawn()));
}

void AGamePlayerController::UpdateDuffleBagUI(const TArray<FItemLooted>& pItems)
{
	if (!IsLocalPlayerController())
		return;

	DuffleBagUIWidget->UpdateUI(pItems);
}

void AGamePlayerController::UpdateTeamDuffleBagUI()
{
	SRUpdateTeamDuffleBagUI();
}

void AGamePlayerController::ToogleTeamDuffleBagUI(bool show)
{
	TeamDuffleBagUIWidget->ToogleView(show);
}

void AGamePlayerController::SRUpdateTeamDuffleBagUI_Implementation()
{
	AGameGameMode* gameMode = Cast<AGameGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

	if (gameMode != nullptr)
		gameMode->UpdateTeamDuffleBag();
}

void AGamePlayerController::ClientUpdateTeamDuffleBagUI_Implementation()
{
	if (!IsLocalPlayerController())
		return;

	if (TeamA.Num() > 0)
	{
		AThief* thief = Cast<AThief>(TeamA[0]);
		if (thief == nullptr)
			return;

		if (thief->inventory != nullptr)
			TeamDuffleBagUIWidget->UpdateUITeamMate1(thief->nickName, thief->inventory->items);
		else
			TeamDuffleBagUIWidget->UpdateUIBagName1(thief->nickName);
	}

	if (TeamA.Num() > 1)
	{
		AThief* thief = Cast<AThief>(TeamA[1]);
		if (thief == nullptr)
			return;

		if (thief->inventory != nullptr)
			TeamDuffleBagUIWidget->UpdateUITeamMate2(thief->nickName, thief->inventory->items);
		else
			TeamDuffleBagUIWidget->UpdateUIBagName2(thief->nickName);
	}
}

void AGamePlayerController::ClientUpdateRoundTimeRemaining_Implementation(const FString& pTime)
{
	if (RoundUIWidget == nullptr || !IsLocalPlayerController())
		return;

	RoundUIWidget->SetTime(pTime);
}

void AGamePlayerController::ClientUpdateCustomTimer_Implementation(const FString& text, const float& pTime)
{
	if (RoundUIWidget == nullptr || !IsLocalPlayerController())
		return;

	RoundUIWidget->SetTextTimer(text, pTime);
}

void AGamePlayerController::ClientUpdateScore_Implementation(int pTeamA, int pTeamB)
{
	if (RoundUIWidget == nullptr)
		return;

	RoundUIWidget->SetScoreA(pTeamA);
	RoundUIWidget->SetScoreB(pTeamB);
}

UUserWidget* AGamePlayerController::AddInteractibleWidgetUI(AActor* pItem, TSubclassOf<UUserWidget> pWidget)
{
	if (interactibleUI.Contains(pItem))
	{
		return interactibleUI[pItem];
	}

	UUserWidget* Widget = CreateWidget<UUserWidget>(GetWorld(), pWidget);
	Widget->AddToViewport();

	interactibleUI.Add(pItem, Widget);

	return Widget;
}

void AGamePlayerController::RemoveInteractibleWidgetUI(AActor* pItem)
{
	if (interactibleUI.Find(pItem) == nullptr)
		return;

	interactibleUI[pItem]->RemoveFromParent();
	interactibleUI.Remove(pItem);
}

UUserWidget* AGamePlayerController::GetWidget(AActor* pItem)
{
	if (interactibleUI.Find(pItem) == nullptr)
		return nullptr;

	return interactibleUI[pItem];
}

void AGamePlayerController::SRToogleHackLights_Implementation(ALightFuseBoxe* actor, bool isHack)
{
	actor->ToogleHackLights(isHack);
}

void AGamePlayerController::SRToggleLights_Implementation(ALightFuseBoxe* actor, bool pOpen)
{
	actor->ToggleLights(pOpen);
}

void AGamePlayerController::SRToogleHackDoor_Implementation(ASlidingDoor* actor, bool isHack)
{
	actor->ToogleHackDoor(isHack);
}

void AGamePlayerController::SRToggleDoor_Implementation(ADoor* actor, bool pOpen, bool manully)
{
	actor->ToggleDoor(pOpen, manully);
}

void AGamePlayerController::SRToggleCameras_Implementation(ATerminal* actor, bool pOn)
{
	actor->ToggleCameras(pOn);
}

void AGamePlayerController::SRToogleHackTerminal_Implementation(ATerminal* actor, bool isHack)
{
	actor->HackTerminal(isHack);
}

void AGamePlayerController::SRFreezeInput_Implementation(float duration, ABase3C* actor, FVector DamageActorLocation)
{
	AGameGameMode* gameMode = Cast<AGameGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

	if (gameMode != nullptr)
		gameMode->FreezeInput(duration, actor, DamageActorLocation);
}

void AGamePlayerController::CameraFreezeInput_Implementation(AActor* actor)
{
	AGameGameMode* gameMode = Cast<AGameGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

	if (gameMode != nullptr)
		gameMode->FreezeCamera(actor);
}

void AGamePlayerController::SRSpawnParticle_Implementation(UParticleSystem* particleEffect, const FTransform& position, const float& duration)
{
	AGameGameMode* gameMode = Cast<AGameGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

	if (gameMode != nullptr)
		gameMode->SpawnParticle(particleEffect, position, duration);
}

void AGamePlayerController::ClientSpawnParticle_Implementation(UParticleSystem* particleEffect, const FTransform& position, const float& duration)
{
	UGenericParticleSystemComponent* particle = (UGenericParticleSystemComponent*)UGameplayStatics::SpawnEmitterAtLocation(
																															GetWorld(),
																															particleEffect,
																															position,
																															true,
																															EPSCPoolMethod::AutoRelease,
																															true
																														);
	if (duration != -1)
		particle->setLifeSpan(duration);
}

void AGamePlayerController::ClientBeingArrest_Implementation()
{
	AThief* thief = Cast<AThief>(GetPawn());
	if (thief == nullptr)
		return;

	thief->SRDropInventory(thief->GetActorLocation());
	thief->SRReset();
}

void AGamePlayerController::ClientFinishArrest_Implementation(float respawnDuration)
{
	AThief* thief = Cast<AThief>(GetPawn());
	if (thief == nullptr)
		return;

	for (auto& it : interactibleUI)
	{
		it.Value->RemoveFromParent();
	}

	UpdateDuffleBagUI(thief->inventory->items);
	UpdateTeamDuffleBagUI();

	if (RespawnUI == nullptr)
	{
		respawnCurrentTime = respawnDuration;
		RespawnUI = CreateWidget<URespawnUI>(GetWorld(), RespawnWidgetClass);
		RespawnUI->AddToViewport();
	}
}

void AGamePlayerController::RestartRound_Implementation()
{
	UEOSGameInstance* instance = Cast<UEOSGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (instance == nullptr)
		return;

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AItem::StaticClass(), FoundActors);

	for (int i = 0; i < FoundActors.Num(); i++)
		Cast<AItem>(FoundActors[i])->reset();

	instance->team = instance->team == ETeam::A ? ETeam::B : ETeam::A;

	ABase3C* base3c = Cast<ABase3C>(GetPawn());
	if (base3c->WidgetUI != nullptr)
	{
		base3c->WidgetUI->RemoveFromParent();
		base3c->WidgetUI = nullptr;
	}
	

	if (DuffleBagUIWidget != nullptr)
	{
		DuffleBagUIWidget->RemoveFromParent();
		DuffleBagUIWidget = nullptr;
	}

	if (TeamDuffleBagUIWidget != nullptr)
	{
		TeamDuffleBagUIWidget->RemoveFromParent();
		TeamDuffleBagUIWidget = nullptr;
	}

	int playTimeDiff = UGameplayStatics::GetRealTimeSeconds((GetWorld())) - base3c->playTime;
	if (base3c->IsA(AThief::StaticClass()))
		instance->GetPlayerSaveGame()->timePlayThief += playTimeDiff;
	else
		instance->GetPlayerSaveGame()->timePlaySecurity += playTimeDiff;


	SRSpawnPlayer(instance->team);
}

void AGamePlayerController::EndGame_Implementation(bool isWin, bool isTie)
{
	UEOSGameInstance* instance = Cast<UEOSGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (instance == nullptr)
		return;

	if (instance->GetPlayerSaveGame() == nullptr)
		return;

	if (isTie)
	{
		instance->SaveGameFinish.AddDynamic(this, &AGamePlayerController::SaveGameFinish);
		instance->SaveGame();
		return;
	}

	if (isWin)
		instance->GetPlayerSaveGame()->totalWin++;
	else
		instance->GetPlayerSaveGame()->totalLoose++;

	instance->SaveGameFinish.AddDynamic(this, &AGamePlayerController::SaveGameFinish);
	instance->SaveGame();
}

void AGamePlayerController::SaveGameFinish()
{
	UEOSGameInstance* instance = Cast<UEOSGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (instance == nullptr)
	{
		UGameplayStatics::OpenLevel(GetGameInstance(), FName("Game/Maps/MainMenu"));
		return;
	}

	instance->SaveGameFinish.RemoveDynamic(this, &AGamePlayerController::SaveGameFinish);
	UGameplayStatics::OpenLevel(GetGameInstance(), FName("Game/Maps/MainMenu"));
}

void AGamePlayerController::SRBeginArrestThief_Implementation(AThief* pThief, bool pArrest, AOfficer* pOfficer)
{
	AGameGameMode* gameMode = Cast<AGameGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

	if (gameMode != nullptr)
		gameMode->beginArrestThief(pArrest, pThief, pOfficer);
}

void AGamePlayerController::SendPingRequest_Implementation(ASensorGadget* sensor, AThief* thief)
{
	AGameGameMode* gameMode = Cast<AGameGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

	if (gameMode != nullptr)
		gameMode->PingThiefToAllOfficer(sensor, thief);
}

void AGamePlayerController::PingPlayerToSensor_Implementation(ASensorGadget* sensor, AThief* thief)
{
	sensor->PingPlayer(thief, Cast<AOfficer>(GetPawn()));
}

void AGamePlayerController::stealMagnetCard_Implementation(AThief* thief, AOfficer* officer)
{
	AGameGameMode* gameMode = Cast<AGameGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

	if (gameMode != nullptr)
		gameMode->stealMagnetCard(thief, officer);
}

void AGamePlayerController::TogglePauseMenu()
{
	// ugly hack for when code is execute twice at same time client and server
	{
		float realtimeSeconds = UGameplayStatics::GetRealTimeSeconds(GetWorld());

		float diff = realtimeSeconds - time;
		if (diff < 0.10f && time != 0)
			return;

		time = realtimeSeconds;
	}

	isPaused = !isPaused;

	if (isPaused)
	{
		CenterViewportCursor();
		SetInputMode(FInputModeGameAndUI());
	}
	else
		SetInputMode(FInputModeGameOnly());

	Cast<ABase3C>(GetPawn())->isPaused = isPaused;

	bShowMouseCursor = isPaused;
	PauseUI->ToggleView(bShowMouseCursor);
}

void AGamePlayerController::CenterViewportCursor()
{
	const ULocalPlayer* LocalPlayer = GetLocalPlayer();
	if (LocalPlayer && LocalPlayer->ViewportClient)
	{
		FViewport* Viewport = LocalPlayer->ViewportClient->Viewport;
		if (Viewport)
		{
			FVector2D ViewportSize;
			LocalPlayer->ViewportClient->GetViewportSize(ViewportSize);
			const int32 X = static_cast<int32>(ViewportSize.X * 0.5f);
			const int32 Y = static_cast<int32>(ViewportSize.Y * 0.5f);

			Viewport->SetMouse(X, Y);
		}
	}
}

void AGamePlayerController::MUlSetLerpRot_Implementation(UCameraCompThief* comp, FRotator rot)
{
	comp->MUlSetLerpRot(rot);
}

void AGamePlayerController::MUlSetRot_Implementation(UCameraCompThief* comp, FRotator rot)
{
	comp->MUlSetLerpRot(rot);
}

void AGamePlayerController::SRDisableSystem_Implementation(AProtectionLoot* actor)
{
	actor->DisableSystem();
}

void AGamePlayerController::TryClimb_Implementation(AThief* thief)
{
	thief->MovementComponent->TryClimbing();
}

void AGamePlayerController::TryToggleCover_Implementation(AThief* thief)
{
	thief->MovementComponent->TryToggleCover();
}

void AGamePlayerController::MUlPlayAttackAnim_Implementation(UBaton* comp)
{
	comp->MUlPlayAttack();
}

void AGamePlayerController::SREquipWeapon_Implementation(UEquipement* equip, const TScriptInterface<IWeapon>& comp)
{ 
	equip->EquipWeapon(comp);
}

void AGamePlayerController::SRFire_Implementation(UEquipement* equip)
{
	equip->Fire();
}

void AGamePlayerController::SpawnDecoy_Implementation(TSubclassOf<ADecoyActor> DecoyActorClass, USkeletalMesh* mesh, UClass* anim, FVector location, FRotator rotation)
{
	ADecoyActor* actor = GetWorld()->GetWorld()->SpawnActor<ADecoyActor>(DecoyActorClass, location, rotation, FActorSpawnParameters());
	actor->SetMesh(mesh, anim);
}

void AGamePlayerController::SetGrenade_Implementation(UEquipement* equip, GrenadeType grenade)
{
	equip->SetGrenadeType(grenade);
}

void AGamePlayerController::SwithcSkeletalMesh_Implementation(UDeguiseComp* comp, USkeletalMesh* mesh, UClass* anim)
{
	comp->SetMesh(mesh, anim);
}

void AGamePlayerController::SpawnClaymore_Implementation(TSubclassOf<AClaymore> ClaymoreTospawn, FVector Location, FRotator rotation, ABase3C* pOwner)
{
	AClaymore* actor = GetWorld()->SpawnActor<AClaymore>(ClaymoreTospawn, Location, rotation, FActorSpawnParameters());
	actor->owner = pOwner;
}

void AGamePlayerController::SetPawnVisibility_Implementation(ABase3C* pOwner, bool visible)
{
	pOwner->GetMesh()->SetVisibility(visible);
}

void AGamePlayerController::SpawnGrenade_Implementation(ABase3C* pPawn, FVector Location, FVector throwingVelo, TSubclassOf<AGrenade> GrenadeClass)
{
	AGrenade* newGrenade = GetWorld()->SpawnActor<AGrenade>(GrenadeClass, Location, FRotator(), FActorSpawnParameters());
	newGrenade->MUlSetVelocity(pPawn, throwingVelo);
}

void AGamePlayerController::SetPossessItem_Implementation(ABase3C* pPawn)
{
	pPawn->equipement->PossessItem();
}