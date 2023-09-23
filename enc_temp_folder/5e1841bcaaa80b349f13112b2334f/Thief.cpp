// Fill out your copyright notice in the Description page of Project Settings.


#include "Thief.h"
#include "Item.h"
#include "HighTierItem.h"
#include <GamePlayerController.h>
#include <Kismet/GameplayStatics.h>
#include <GameGameMode.h>
#include <Components/BoxComponent.h>
#include <Officer.h>
#include "GamePlayerStart.h"
#include "HelperClass.h"
#include <EnhancedInputComponent.h>
#include "CameraCompThief.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include <Kismet/KismetMaterialLibrary.h>

AThief::AThief()
{
	bReplicates = true;

	inventory = NewObject<UInventory>();

	cameraComponent = CreateDefaultSubobject<UCameraCompThief>(TEXT("Camera Component"));
	cameraComponent->SetupCamera(GetMesh());

	ArrestArea = CreateDefaultSubobject<UBoxComponent>(FName("ArrestArea"));
	ArrestArea->SetGenerateOverlapEvents(true);
	ArrestArea->SetupAttachment(RootComponent);

	ClimbingArea = CreateDefaultSubobject<UBoxComponent>(FName("ClimbingArea"));
	ClimbingArea->SetGenerateOverlapEvents(true);
	ClimbingArea->SetupAttachment(RootComponent);
}

void AThief::BeginPlay()
{
	Super::BeginPlay();
	if (!CreateTableInstance())
		return;

	CreateTimeline();
	SendDataToComponents();

	ArrestArea->OnComponentBeginOverlap.AddDynamic(this, &AThief::OnArrestTriggerOverlapBegin);
	ArrestArea->OnComponentEndOverlap.AddDynamic(this, &AThief::OnArrestTriggerOverlapEnd);
	HelperClass::deactivateTrigger(ArrestArea);
	ArrestAreaActivate = false;
	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;

	ClimbingArea->OnComponentBeginOverlap.AddDynamic(this, &AThief::ClimbTriggerOverlapBegin);
	ClimbingArea->OnComponentEndOverlap.AddDynamic(this, &AThief::ClimbTriggerOverlapEnd);

	ArrestUISelf = (UArrestUI*)CreateWidget<UUserWidget>(GetWorld(), ArrestOfficerWidgetClass);
	ArrestUISelf->AddToViewport();
}

void AThief::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (beingArrest)
	{
		TArray<AActor*> actors;
		ArrestArea->GetOverlappingActors(actors, AOfficer::StaticClass());

		for (AActor* actor : actors)
		{
			if (actor == officerArresting)
				continue;

			AController* PC = ((AOfficer*)actor)->GetController();
			if (PC != nullptr && PC->IsLocalPlayerController())
			{
				AGamePlayerController* playerController = Cast<AGamePlayerController>(PC);
				UArrestUI* ui = (UArrestUI*)playerController->GetWidget(this);
				ui->ShowAlreadyArresting();
			}
		}
	}
	else if (bFreezeInput)
	{
		TArray<AActor*> actors;
		ArrestArea->GetOverlappingActors(actors, AOfficer::StaticClass());

		for (AActor* actor : actors)
		{
			AController* PC = ((AOfficer*)actor)->GetController();
			if (PC != nullptr && PC->IsLocalPlayerController())
			{
				AGamePlayerController* playerController = Cast<AGamePlayerController>(PC);
				UArrestUI* ui = (UArrestUI*)playerController->GetWidget(this);
				if (ui == nullptr)
				{
					AOfficer* player = Cast<AOfficer>(actor);
					if (player == nullptr)
						continue;

					player->closeThief.Add(this);
					ui = (UArrestUI*)playerController->AddInteractibleWidgetUI(this, ArrestWidgetClass);
				}
				
				ui->Reset();
			}
		}
	}
	else if (IsClimbing)
	{

		FHitResult Hit = ClimbingLineTrace();
		if (Hit.IsValidBlockingHit())
		{
			FVector normal = Hit.Normal;
			FRotator Rot = UKismetMathLibrary::MakeRotFromX(normal);
			Rot.Yaw += 180;
			FRotator CurrentRotation = GetActorRotation();
			CurrentRotation.Yaw = Rot.Yaw;
			SetActorRotation(CurrentRotation);
		}
	}
	ChangeStencilOnMovement();
}

void AThief::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	CreateTableInstance();

	//Moving
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(thiefTableInstance->crouchAction, ETriggerEvent::Started, this, &AThief::crouch);
		EnhancedInputComponent->BindAction(thiefTableInstance->crouchAction, ETriggerEvent::Completed, this, &AThief::unCrouch);
		EnhancedInputComponent->BindAction(thiefTableInstance->NightVisionAction, ETriggerEvent::Started, this, &AThief::HandleNightVision);
		EnhancedInputComponent->BindAction(thiefTableInstance->climbAction, ETriggerEvent::Started, this, &AThief::CheckCanClimb);
	}
}

void AThief::CreateTimeline() //Timeline is used for the Night vision to change between states
{
	if (thiefTableInstance->NightVisionFloatCurve)
	{
		FOnTimelineFloat TimelineProgress;
		FOnTimelineEventStatic onTimelineFinishedCallback;
		TimelineProgress.BindUFunction(this, FName("TimelineProgress2"));
		NightVisionTimeline.AddInterpFloat(thiefTableInstance->NightVisionFloatCurve, TimelineProgress);
		NightVisionTimeline.SetLooping(false);
		onTimelineFinishedCallback.BindUFunction(this, FName("TimelineFinished2"));
		NightVisionTimeline.SetTimelineFinishedFunc(onTimelineFinishedCallback);
	}
}

bool AThief::CreateTableInstance()
{
	ABase3C::CreateTableInstance();

	if (thiefTableInstance)
		return true;

	if (!thiefDataTable)
		return false;

	thiefTableInstance = thiefDataTable->FindRow<FThiefTable>(FName(TEXT("Thief")), "");
	return true;
}

void AThief::TimelineProgress2(float value)
{
	UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), thiefTableInstance->NightVisionMPC, "Alpha", value);
}

void AThief::TimelineFinished2()
{
	NightTimelineRunning = false;
}

void AThief::HandleNightVision() //Reacts to the input of MotionVision
{
	if (NightTimelineRunning)
		return;

	NightTimelineRunning = true;

	if (!NightSensorActive)
	{
		NightVisionTimeline.PlayFromStart();
		NightSensorActive = true;
	}
	else
	{
		NightVisionTimeline.ReverseFromEnd();
		NightSensorActive = false;
	}
}

void AThief::Move(const FInputActionValue& Value)
{
	if (bFreezeInput || beingArrest)
		return;

	ABase3C::Move(Value);
}

void AThief::SRReset_Implementation()
{
	ClientShowArrest(false);

	TArray<AActor*> spawnPoint;
	UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), AGamePlayerStart::StaticClass(), TEAM_A_STR, spawnPoint);

	int idx = FMath::RandRange(0, spawnPoint.Num() - 1);

	MulReset(spawnPoint[idx]->GetActorTransform());
}

void AThief::MulReset_Implementation(FTransform transform)
{
	HelperClass::deactivateActor(this);
	HelperClass::deactivateTrigger(ArrestArea);
	ArrestAreaActivate = false;
	beingArrest = false;

	AGamePlayerController* pc = Cast<AGamePlayerController>(GetController());
	if (pc == nullptr)
		return;

	pc->ClientFinishArrest(thiefTableInstance->respawnTime);

	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([=] {
		Super::MulReset_Implementation(transform);
		HelperClass::activateActor(this);
	}), thiefTableInstance->respawnTime, false);
}

void AThief::SRStartClimbing_Implementation()
{
	MulStartClimbing();
}


void AThief::MulStartClimbing_Implementation()
{
	IsClimbing = true;
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
}

void AThief::SRStopClimbing_Implementation()
{
	MulStopClimbing();
}

void AThief::MulStopClimbing_Implementation()
{
	IsClimbing = false;
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
}

bool AThief::ValidateSpaceItem(AItem& pItem)
{
	return inventory->ValidateSpace(pItem);
}

void AThief::ChangeStencilOnMovement()
{
	if (GetVelocity().Length() > 0)
	{
		if (GetMesh())
			GetMesh()->SetCustomDepthStencilValue(2); //TODO Implement datatable for thief and add a value for stencil buffer
	}
	else
	{
		if (GetMesh())
			GetMesh()->SetCustomDepthStencilValue(0);
	}
}

bool AThief::HasSpaceForItem(AItem* pItem)
{
	if (inventory == nullptr)
		inventory = NewObject<UInventory>();

	return inventory->ValidateSpace(*pItem);
}

void AThief::SRAddItem_Implementation(AItem* pItem)
{
	pItem->MulPlayerLootIt();
	MUlAddItem(pItem);
}

void AThief::MUlAddItem_Implementation(AItem* pItem)
{
	if (inventory == nullptr)
		inventory = NewObject<UInventory>();

	if (!inventory->AddItem(*pItem))
		return;

	AGamePlayerController* PC = Cast<AGamePlayerController>(Controller);
	if (PC != nullptr)
	{
		PC->UpdateDuffleBagUI(inventory->items);
		PC->UpdateTeamDuffleBagUI();

		if (pItem->IsA(AHighTierItem::StaticClass()))
		{
			UEOSGameInstance* instance = Cast<UEOSGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
			if (instance == nullptr)
				return;

			if (instance->GetPlayerSaveGame() == nullptr)
				return;

			instance->GetPlayerSaveGame()->totalHightLoot++;
		}
	}
}

void AThief::SRClearItems_Implementation(int score, ETeam pTeam)
{
	AGameGameMode* gameMode = Cast<AGameGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

	switch (pTeam)
	{
		case ETeam::A : gameMode->AddToScore(score, gameMode->ScoreTeamA);
			break;
		case ETeam::B : gameMode->AddToScore(score, gameMode->ScoreTeamB);
			break;
	}

	MUlClearItems(score);
}

void AThief::MUlClearItems_Implementation(int score)
{
	inventory->ClearInventory();

	AGamePlayerController* PC = Cast<AGamePlayerController>(Controller);
	if (PC != nullptr)
	{
		PC->UpdateDuffleBagUI(inventory->items);
		PC->UpdateTeamDuffleBagUI();

		UEOSGameInstance* instance = Cast<UEOSGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
		if (instance == nullptr)
			return;

		if (instance->GetPlayerSaveGame() == nullptr)
			return;

		instance->GetPlayerSaveGame()->totalCash += score;
		instance->GetPlayerSaveGame()->totalExtraction++;
	}
}

void AThief::SRDropInventory_Implementation(FVector location)
{
	AGameGameMode* gameMode = Cast<AGameGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

	ETeam team = Cast<UEOSGameInstance>(GetGameInstance())->team;

	switch (team)
	{
	case ETeam::A: gameMode->RemoveToScore(thiefTableInstance->costOnArrest, gameMode->ScoreTeamA);
		break;
	case ETeam::B: gameMode->RemoveToScore(thiefTableInstance->costOnArrest, gameMode->ScoreTeamB);
		break;
	}

	MUlDropInventory(location);
}

void AThief::MUlDropInventory_Implementation(FVector location)
{
	if (inventory == nullptr)
		return;

	for (int i = 0; i < inventory->items.Num(); i++)
	{
		inventory->items[i].item->dropItem(location);
	}

	inventory->ClearInventory();
}

void AThief::Interact()
{
	if (bFreezeInput || beingArrest)
		return;

	if (closeItems.Num() == 0)
		return;

	//Should use the camera forward to check wich item to use if multiple in array
	ItemUsing = closeItems[0];
	IThiefInteractibleActor::Execute_Interact(ItemUsing->_getUObject(), this);
}

void AThief::StopInteract()
{
	if (ItemUsing == nullptr)
		return;

	IThiefInteractibleActor::Execute_StopInteract(ItemUsing->_getUObject(), this);
	ItemUsing = nullptr;
}

void AThief::Tab()
{
	AGamePlayerController* PC = Cast<AGamePlayerController>(Controller);
	if (PC != nullptr)
		PC->ToogleTeamDuffleBagUI(true);
}

void AThief::StopTab()
{
	AGamePlayerController* PC = Cast<AGamePlayerController>(Controller);
	if (PC != nullptr)
		PC->ToogleTeamDuffleBagUI(false);
}

void AThief::ClientFreezeInput_Implementation(float duration)
{
	Super::ClientFreezeInput_Implementation(duration);
	SRActivateArrestTrigger();
}

void AThief::SRActivateArrestTrigger_Implementation()
{
	MulActivateArrestTrigger();
}

void AThief::MulActivateArrestTrigger_Implementation()
{
	HelperClass::activateTrigger(ArrestArea);
	ArrestAreaActivate = true;
}

void AThief::MulSetBeingArrest_Implementation(bool pArrest, AOfficer* pOfficer)
{
	beingArrest = pArrest;

	if (!beingArrest && !ArrestAreaActivate)
		ResetOfficerInArrestArea();

	officerArresting = pOfficer;
	
	ClientShowArrest(pArrest);
}

void AThief::ClientShowArrest_Implementation(bool pArrest)
{
	ArrestUISelf->ToggleBeingArrested(pArrest);
}

void AThief::CheckCanClimb()
{
	if (CanClimb)
	{
		if (IsClimbing)
			SRStopClimbing();
		else
			SRStartClimbing();
	}
}

void AThief::UnFreezeInput_Implementation()
{
	Super::UnFreezeInput_Implementation();
	HelperClass::deactivateTrigger(ArrestArea);
	ArrestAreaActivate = false;

	ResetOfficerInArrestArea(officerArresting);
}

void AThief::ResetOfficerInArrestArea(AOfficer* pOfficerToSkip)
{
	TArray<AActor*> actors;
	ArrestArea->GetOverlappingActors(actors, AOfficer::StaticClass());

	for (AActor* actor : actors)
	{
		if (beingArrest && actor == pOfficerToSkip)
			continue;

		AOfficer* player = Cast<AOfficer>(actor);
		player->closeThief.Remove(this);
		player->ArrestingThief = nullptr;

		AController* PC = player->GetController();
		if (PC != nullptr && PC->IsLocalPlayerController())
		{
			AGamePlayerController* playerController = Cast<AGamePlayerController>(PC);
			playerController->RemoveInteractibleWidgetUI(this);
		}
	}
}

void AThief::crouch()
{
	Crouch();
}

void AThief::unCrouch()
{
	UnCrouch();
}

void AThief::OnArrestTriggerOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AOfficer* player = Cast<AOfficer>(OtherActor);
	if (player == nullptr)
		return;

	player->closeThief.Add(this);

	AController* PC = player->GetController();
	if (PC != nullptr && PC->IsLocalPlayerController())
	{
		AGamePlayerController* playerController = Cast<AGamePlayerController>(PC);
		playerController->AddInteractibleWidgetUI(this, ArrestWidgetClass);
	}
}

void AThief::OnArrestTriggerOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AOfficer* player = Cast<AOfficer>(OtherActor);
	if (player == nullptr)
		return;

	player->closeThief.Remove(this);
	if (player->ArrestingThief == this)
	{
		player->ArrestingThief = nullptr;
		beingArrest = false;
		officerArresting = nullptr;
	}

	AController* PC = player->GetController();
	if (PC != nullptr && PC->IsLocalPlayerController())
	{
		AGamePlayerController* playerController = Cast<AGamePlayerController>(PC);
		playerController->RemoveInteractibleWidgetUI(this);
	}
}

void AThief::ClimbTriggerOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->ActorHasTag("CanClimb"))
	{
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("CLimbing!"));
		CanClimb = true;
	}
}

void AThief::ClimbTriggerOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->ActorHasTag("CanClimb"))
	{
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("stop cLimbing!"));
		CanClimb = false;
		SRStopClimbing();
	}
}

FHitResult AThief::ClimbingLineTrace()
{
	FHitResult OutHit;
	FVector Start = GetActorLocation();
	Start.Z -= GetActorScale().Z;

	FVector ForwardVector = GetActorForwardVector();
	FVector End = ((ForwardVector * 1000.f) + Start);
	FCollisionQueryParams CollisionParams;

	//DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 1, 0, 1);

	GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility, CollisionParams);

	return OutHit;
}
