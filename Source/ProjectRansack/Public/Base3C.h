// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Engine/DataTable.h"
#include "Base3C.generated.h"

USTRUCT(BlueprintType)
struct FBase3CTable : public FTableRowBase
{
GENERATED_BODY()
public:

	//Health
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Health, meta = (AllowPrivateAccess = "true"))
		float currentHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Health, meta = (AllowPrivateAccess = "true"))
		float maxHealth;

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
		class UInputMappingContext* inputHandler;

	/**Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* moveAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* interactAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* lookAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* AimAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* FireAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* SprintAction;
};

UENUM()
enum CharacterState
{
	Idle		UMETA(DisplayName = "Idle"),
	Attacking   UMETA(DisplayName = "Attacking"),
	Stunned		UMETA(DisplayName = "Stunned"),
};


UCLASS()
class PROJECTRANSACK_API ABase3C : public ACharacter
{
	GENERATED_BODY()

public:
	UPROPERTY(editAnywhere, blueprintReadWrite)
	class USkeletalMeshComponent* skeletalMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComp* cameraComponent;

	/*Health Component*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Health, meta = (AllowPrivateAccess = "true"))
	class UHealthComponent* healthComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString nickName = "";

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Data, meta = (AllowPrivateAccess = "true"))
		class UDataTable* dataTable;

	FBase3CTable* tableInstance = nullptr;

	float normalWalkSpeed;

	CharacterState currentState;


	ABase3C();

	bool CheckTableInstance();
	void SendDataToComponents();

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SetClientNickname(const FString& pNickName);
	void SetClientNickname_Implementation(const FString& pNickName);

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void MulticastSetClientNickname(const FString& pNickName);
	void MulticastSetClientNickname_Implementation(const FString& pNickName);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	virtual void Interact();
	
	virtual void StopInteract();

	void StartAim();

	void StopAim();

	void Fire();

	void Sprint();

	void StopSprint();

	void BindInputHandler();
	
	UFUNCTION(Server, Unreliable)
	void TestDamage(AActor* DamageActor);
};
