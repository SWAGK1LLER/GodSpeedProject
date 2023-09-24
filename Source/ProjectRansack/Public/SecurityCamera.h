// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameFramework/Actor.h"
#include "SecurityCamera.generated.h"

USTRUCT(BlueprintType)
struct FSecurityCameraDataTable : public FTableRowBase
{
	GENERATED_BODY()
	UPROPERTY(editAnywhere, BlueprintReadWrite, Category = Rotation, meta = (AllowPrivateAccess = "true"))
	float rotateAmount = 30; //Amount to rotate in degrees

	UPROPERTY(editAnywhere, BlueprintReadWrite, Category = Rotation, meta = (AllowPrivateAccess = "true"))
	float rotationSpeed = 0.3f;

	UPROPERTY(editAnywhere, BlueprintReadWrite, Category = Stun, meta = (AllowPrivateAccess = "true"))
	float timeToUnfreeze = 3.0f;

	UPROPERTY(editAnywhere, BlueprintReadWrite, Category = Stun, meta = (AllowPrivateAccess = "true"))
	FLinearColor ColorPing = FLinearColor::Red;
};

UENUM(BlueprintType)
enum ERotationSide
{
	Left,
	Right
};

UCLASS()
class PROJECTRANSACK_API ASecurityCamera : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SecurityCamera, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* coneShape;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SecurityCamera, meta = (AllowPrivateAccess = "true"))
	class USpotLightComponent* spotLight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SecurityCamera, meta = (AllowPrivateAccess = "true"))
	int cameraNumber = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SecurityCamera, meta = (AllowPrivateAccess = "true"))
	class USoundCue* pingAudioCue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SecurityCamera, meta = (AllowPrivateAccess = "true"))
	class UAudioComponent* pingAudioComponent;

	UPROPERTY(editAnywhere, BlueprintReadWrite, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* cameraMesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Data, meta = (AllowPrivateAccess = "true"))
	class UDataTable* dataTable = nullptr;

	FSecurityCameraDataTable* tableInstance = nullptr;

	ERotationSide rotationDirection = ERotationSide::Right;

	FVector referenceForwardVector = FVector();

	TArray<AActor*> UndetectedThieves;

	bool hasPinged = false;

	bool frozen;

	float currentTimetoUnfreeze = 0;

	ASecurityCamera();
	
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	void CheckDataTable();

	UFUNCTION()
	void OnTriggerOverlapBegin(class UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnTriggerOverlapEnd(class UPrimitiveComponent* OverlappedComp, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(Server, Reliable)
	void NotifyAllSecurity(bool PingOrUnping);

	UFUNCTION(NetMulticast, Reliable)
	void PlayAudio();

	void CalculateBaseRotations();

	void CheckAndRotate();

	void CheckUndetectedThieves();

	UFUNCTION(NetMulticast, Reliable)
	void Mul_FreezeCamera();
};
