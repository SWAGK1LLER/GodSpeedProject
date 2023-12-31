#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Engine/DataTable.h"
#include "PlayerUI.h"
#include "Base3C.generated.h"

USTRUCT(BlueprintType)
struct FBase3CTable : public FTableRowBase
{
GENERATED_BODY()
public:
	//Movement
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MovementValues, meta = (AllowPrivateAccess = "true"))
	float movementSpeed = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MovementValues, meta = (AllowPrivateAccess = "true"))
	float sprintSpeed = 1.5f;

	//Camera
	UPROPERTY(editAnywhere, BlueprintReadWrite, Category = CameraParameters, meta = (AllowPrivateAccess = "true"))
	float maxPitchBottom = 30;

	UPROPERTY(editAnywhere, BlueprintReadWrite, Category = CameraParameters, meta = (AllowPrivateAccess = "true"))
	float maxPitchTop = 30;

	UPROPERTY(editAnywhere, BlueprintReadWrite, Category = CameraParameters, meta = (AllowPrivateAccess = "true"))
	float aimZoom = 30;
	
	UPROPERTY(editAnywhere, BlueprintReadWrite, Category = CameraParameters, meta = (AllowPrivateAccess = "true"))
	float aimZoomSpeed = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* inputHandler = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* moveAction = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* interactAction = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* lookAction = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* AimAction = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* FireAction = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* SprintAction = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* TabAction = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* PauseAction = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* Belt1Action = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* Belt2Action = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* Belt3Action = nullptr;
};

UCLASS()
class PROJECTRANSACK_API ABase3C : public ACharacter
{
	GENERATED_BODY()

public:
	bool isPaused = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UPlayerUI> WidgetClass;
	UPlayerUI* WidgetUI = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComp* cameraComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = damage, meta = (AllowPrivateAccess = "true"))
	class UDamageIndicatorComp* damageIndicator = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = sound, meta = (AllowPrivateAccess = "true"))
	class UAudioComponent* audioComp = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UEquipement* equipement = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString nickName = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int playTime = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Data, meta = (AllowPrivateAccess = "true"))
	class UDataTable* dataTable = nullptr;

	FBase3CTable* tableInstance = nullptr;

	float normalWalkSpeed;

	bool bFreezeInput = true;
	float FreezeDuration = -1;
	float TimeFreezed = 0;
	bool bIsZooming = false;
	float zoomTime = 0;

	FTransform SpawnTransform;

	bool Revealed = false;

	bool HasMagnetCard = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsRunning = false;

	UPROPERTY(Category = "Animation", EditDefaultsOnly)
	class UAnimMontage* GrenadeThrowMontage;

	ABase3C(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual bool CreateTableInstance();
	virtual void SendDataToComponents();
	virtual void CreateTimeline() {};

	void DispatchController(class AGamePlayerController* controller);

	UFUNCTION(Client, Reliable, BlueprintCallable)
	void SetClientUI();
	virtual void SetClientUI_Implementation();

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SetClientNickname(const FString& pNickName);
	void SetClientNickname_Implementation(const FString& pNickName);

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void MulticastSetClientNickname(const FString& pNickName);
	void MulticastSetClientNickname_Implementation(const FString& pNickName);

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void ClientFreezeInput(float duration, FVector DamageActorLocation);
	virtual void ClientFreezeInput_Implementation(float duration, FVector DamageActorLocation);

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void UnFreezeInput();
	virtual void UnFreezeInput_Implementation();

	UFUNCTION(Server, Unreliable)
	virtual void SRReset();
	virtual void SRReset_Implementation();

	UFUNCTION(NetMulticast, Unreliable)
	virtual void MulReset(FTransform transform);
	virtual void MulReset_Implementation(FTransform transform);

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void Move(const FInputActionValue& Value);

	virtual void Interact();

	virtual void StopInteract();

	void StartAim();

	void StopAim();

	virtual void Fire();

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SRStartSprinting();
	void SRStartSprinting_Implementation();

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void MulStartSprinting();
	void MulStartSprinting_Implementation();

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SRStopSprinting();
	void SRStopSprinting_Implementation();

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void MulStopSprinting();
	void MulStopSprinting_Implementation();

	void Sprint();

	void StopSprint();

	virtual void Tab();

	virtual void StopTab();

	void BindInputHandler();
	
	UFUNCTION(NetMulticast, Reliable)
	void ChangeStencilFromServer(int pNewStencilValue);

	UFUNCTION(BlueprintCallable)
	UCameraComp* GetCameraComponent();

	void TryGeneratingOverlapEvent();

	UFUNCTION(Client, Reliable, BlueprintCallable)
	void ToggleMagnetCardUI(bool visible);
	void ToggleMagnetCardUI_Implementation(bool visible);

	void TogglePauseMenu();

	UFUNCTION(BlueprintCallable)
	void PlayFootstep();

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void MUlThrowGrenade();
	void MUlThrowGrenade_Implementation();

	UFUNCTION(BlueprintCallable)
	void ThrowGrenade();

	UFUNCTION(BlueprintCallable)
	void ThrowFinish();
};
