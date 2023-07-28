// Fill out your copyright notice in the Description page of Project Settings.

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

	//Health
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Health, meta = (AllowPrivateAccess = "true"))
		float currentHealth = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Health, meta = (AllowPrivateAccess = "true"))
		float maxHealth = 0;

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

	/* Input */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputMappingContext* inputHandler = nullptr;

	/**Input Action */
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

};

UENUM()
enum CharacterState
{
	Gun		UMETA(DisplayName = "Gun"),
	SensorGadget	UMETA(DisplayName = "SensorGadget"),
};


UCLASS()
class PROJECTRANSACK_API ABase3C : public ACharacter
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<UPlayerUI> WidgetClass;
	UPlayerUI* WidgetUI = nullptr;

	//UPROPERTY(editAnywhere, blueprintReadWrite)
	//class USkeletalMeshComponent* skeletalMesh = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComp* cameraComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UStunWeapon* StunWeapon = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Health, meta = (AllowPrivateAccess = "true"))
		class UHealthComponent* healthComp = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString nickName = "";

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Data, meta = (AllowPrivateAccess = "true"))
		class UDataTable* dataTable = nullptr;

	FBase3CTable* tableInstance = nullptr;

	float normalWalkSpeed;

	CharacterState currentState;

	bool bFreezeInput = false;
	float FreezeDuration = 0;
	float TimeFreezed = 0;

	FTransform SpawnTransform;

	bool shouldPingMovement = false;

	ABase3C();

	bool CheckTableInstance();
	void SendDataToComponents();
	
	UFUNCTION(Client, Reliable, BlueprintCallable)
	void SetClientUI();
	void SetClientUI_Implementation();
	
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SetClientNickname(const FString& pNickName);
	void SetClientNickname_Implementation(const FString& pNickName);
	
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void MulticastSetClientNickname(const FString& pNickName);
	void MulticastSetClientNickname_Implementation(const FString& pNickName);

	UFUNCTION(Client, Reliable, BlueprintCallable)
	void ClientFreezeInput(float duration);
	void ClientFreezeInput_Implementation(float duration);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(Server, Unreliable)
	void SRReset();
	void SRReset_Implementation();

	UFUNCTION(NetMulticast, Unreliable)
	virtual void MulReset();
	virtual void MulReset_Implementation();


	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	virtual void Interact();

	virtual void StopInteract();

	void StartAim();

	void StopAim();

	virtual void StartFire() {};

	void Fire();

	void Sprint();

	void StopSprint();

	void BindInputHandler();
	
	UFUNCTION(Server, Unreliable)
	void TestDamage(AActor* DamageActor);

	UFUNCTION(BlueprintCallable)
	UCameraComp* GetCameraComponent();
};
