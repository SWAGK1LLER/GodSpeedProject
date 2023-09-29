#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MyCharacterMovementComponent.generated.h"

UCLASS()
class PROJECTRANSACK_API UMyCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
public:
	UMyCharacterMovementComponent(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintPure)
	bool IsClimbing() const;

	UFUNCTION(BlueprintPure)
	bool IsCover() const;

	UFUNCTION(BlueprintPure)
	bool IsClimbDashing() const;

	UFUNCTION(BlueprintPure)
	FVector GetClimbSurfaceNormal() const;

	UFUNCTION(BlueprintPure)
	FVector GetClimbDashDirection() const;

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void TryClimbing();
	void TryClimbing_Implementation();

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void TryCover();
	void TryCover_Implementation();

	UFUNCTION(BlueprintCallable)
	void TryClimbDashing();

	UFUNCTION(BlueprintCallable)
	void CancelClimbing();

	UPROPERTY(Category="Character Movement: Climbing", EditAnywhere)
	int CollisionCapsuleRadius = 50;

	UPROPERTY(Category="Character Movement: Climbing", EditAnywhere)
	int CollisionCapsuleHalfHeight = 72;

	UPROPERTY(Category="Character Movement: Climbing", EditAnywhere, meta=(ClampMin="10.0", ClampMax="500.0"))
	float MaxClimbingSpeed = 120;

	UPROPERTY(Category="Character Movement: Climbing", EditAnywhere, meta=(ClampMin="10.0", ClampMax="2000.0"))
	float MaxClimbingAcceleration = 380;

	UPROPERTY(Category="Character Movement: Climbing", EditAnywhere, meta=(ClampMin="0.0", ClampMax="3000.0"))
	float BrakingDecelerationClimbing = 550.f;

	UPROPERTY(Category="Character Movement: Climbing", EditAnywhere, meta=(ClampMin="0.0", ClampMax="60.0"))
	float ClimbingSnapSpeed = 4.f;

	UPROPERTY(Category="Character Movement: Climbing", EditAnywhere, meta=(ClampMin="0.0", ClampMax="80.0"))
	float DistanceFromSurface = 45.f;

	UPROPERTY(Category="Character Movement: Climbing", EditAnywhere, meta=(ClampMin="1.0", ClampMax="60.0"))
	int ClimbingRotationSpeed = 5;

	UPROPERTY(Category="Character Movement: Climbing", EditAnywhere, meta=(ClampMin="0.0", ClampMax="80.0"))
	float ClimbingCollisionShrinkAmount = 30;

	UPROPERTY(Category="Character Movement: Climbing", EditAnywhere, meta=(ClampMin="1.0", ClampMax="500.0"))
	float FloorCheckDistance = 90.f;

	UPROPERTY(Category="Character Movement: Climbing", EditAnywhere, meta=(ClampMin="1.0", ClampMax="75.0"))
	float MinHorizontalDegreesToStartClimbing = 25;

	UPROPERTY(Category="Character Movement: Climbing", EditDefaultsOnly)
	UAnimMontage* LedgeClimbMontage;

	bool playingLedgeClimbAnim = false;

	UPROPERTY(Category="Character Movement: Climbing", EditDefaultsOnly)
	UCurveFloat* ClimbDashCurve;

	UPROPERTY()
	UAnimInstance* AnimInstance;
	
	TArray<FHitResult> CurrentWallHits;

	FCollisionQueryParams ClimbQueryParams;

	bool bWantsToClimb = false;

	bool bWantsToCover = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool CrouchCover = false;

	bool bIsClimbDashing = false;

	float CurrentClimbDashTime;

	FVector ClimbDashDirection;
	
	FVector CurrentClimbingNormal;
	
	FVector CurrentClimbingPosition;

	FRotator relativeMeshRotation;

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;
	
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;
	
	virtual float GetMaxSpeed() const override;
	
	virtual float GetMaxAcceleration() const override;
	
	virtual void PhysCustom(float deltaTime, int32 Iterations) override;
	
	void UpdateClimbDashState(float deltaTime);

	void PhysClimbing(float deltaTime, int32 Iterations);

	void PhysCover(float deltaTime, int32 Iterations);
	
	bool EyeHeightTrace(const float TraceDistance) const;

	bool BackEyeHeightTrace(const float TraceDistance) const;
	
	bool ShouldStopClimbing() const;
	
	bool ClimbDownToFloor() const;
	
	bool CheckFloor(FHitResult& FloorHit) const;
	
	void SetRotationToStand() const;
	
	bool TryClimbUpLedge();

	bool TryCrouchCover();
	
	bool HasReachedEdge() const;

	bool HasReachedEdgeCover() const;
	
	bool IsLocationWalkable(const FVector& CheckLocation) const;
	
	bool CanMoveToLedgeClimbLocation() const;

	bool CanMoveToSide(FVector direction) const;

	bool CanStartClimbing();

	bool CanStartCover();
	
	bool IsFacingSurface(float Steepness) const;

	FQuat GetClimbingRotation(float deltaTime) const;
	
	void StopClimbing(float deltaTime, int32 Iterations);
	
	void AlignClimbDashDirection();

	void StoreClimbDashDirection();
	
	void StopClimbDashing();

	void ComputeClimbingVelocity(float deltaTime);

	void ComputeCoverVelocity(float deltaTime);
	
	void MoveAlongClimbingSurface(float deltaTime);

	void MoveAlongSideSurface(float deltaTime, int32 Iterations);

	void SnapToClimbingSurface(float deltaTime) const;
	
	void ComputeSurfaceInfo();
	
	void SweepAndStoreWallHits();

	UFUNCTION(BlueprintCallable)
	void animationLedgeFinished();
};
