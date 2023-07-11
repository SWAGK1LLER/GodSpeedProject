// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Base3C.generated.h"

UCLASS()
class PROJECTRANSACK_API ABase3C : public ACharacter
{
	GENERATED_BODY()

public:
	UPROPERTY(editAnywhere, blueprintReadWrite)
	class USkeletalMeshComponent* skeletalMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComp* cameraComponent;

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

	/*Health Component*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Health, meta = (AllowPrivateAccess = "true"))
	class UHealthComponent* healthComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString nickName = "";

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		float MovementSpeed = 1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		float SprintSpeed = 1.5f;

	float NormalWalkSpeed;

	ABase3C();

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

	void Interact();

	void Aim();

	void Fire();

	void Sprint();

	void StopSprint();

	void BindInputHandler();
	
	UFUNCTION(Server, Unreliable)
	void TestDamage(AActor* DamageActor);
};
