#include "MyCharacterMovementComponent.h"

#include "ECustomMovement.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include <Thief.h>
#include <CameraCompThief.h>
#include "GamePlayerController.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DrawDebugHelpers.h"

UMyCharacterMovementComponent::UMyCharacterMovementComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UMyCharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	relativeMeshRotation = CharacterOwner->GetMesh()->GetRelativeRotation();

	AnimInstance = GetCharacterOwner()->GetMesh()->GetAnimInstance();
	
	ClimbQueryParams.AddIgnoredActor(GetOwner());
}

void UMyCharacterMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                  FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (IsCover())
		SweepAndStoreWallHitsCover();
	else
		SweepAndStoreWallHits();
}

void UMyCharacterMovementComponent::SweepAndStoreWallHits()
{
	const FCollisionShape CollisionShape = FCollisionShape::MakeCapsule(CollisionCapsuleRadius, CollisionCapsuleHalfHeight);

	const FVector StartOffset = UpdatedComponent->GetForwardVector() * 20;

	// Avoid using the same Start/End location for a Sweep, as it doesn't trigger hits on Landscapes.
	const FVector Start = UpdatedComponent->GetComponentLocation() + StartOffset;
	const FVector End = Start + UpdatedComponent->GetForwardVector();

	TArray<FHitResult> Hits;
	const bool HitWall = GetWorld()->SweepMultiByChannel(Hits, Start, End, FQuat::Identity,
		  ECC_WorldStatic, CollisionShape, ClimbQueryParams);

	HitWall ? CurrentWallHits = Hits : CurrentWallHits.Reset();
}

void UMyCharacterMovementComponent::SweepAndStoreWallHitsCover()
{
	const FCollisionShape CollisionShape = FCollisionShape::MakeCapsule(CollisionCapsuleRadius, CollisionCapsuleHalfHeight);
	UCapsuleComponent* Capsule = CharacterOwner->GetCapsuleComponent();

	const FVector StartOffset = Capsule->GetForwardVector() * 40;

	// Avoid using the same Start/End location for a Sweep, as it doesn't trigger hits on Landscapes.
	const FVector Start = UpdatedComponent->GetComponentLocation() + StartOffset;
	const FVector End = Start + Capsule->GetForwardVector();

	TArray<FHitResult> Hits;
	const bool HitWall = GetWorld()->SweepMultiByChannel(Hits, Start, End, FQuat::Identity,
		ECC_WorldStatic, CollisionShape, ClimbQueryParams);

	for (int i = 0; i < Hits.Num(); i++)
	{
		FHitResult& Hit = Hits[i];
		const FVector HorizontalNormal = Hit.Normal;

		const float HorizontalDot = FVector::DotProduct(Capsule->GetForwardVector(), -HorizontalNormal);
		const float VerticalDot = FVector::DotProduct(Hit.Normal, HorizontalNormal);

		const float HorizontalDegrees = FMath::RadiansToDegrees(FMath::Acos(HorizontalDot));

		const bool bIsCeiling = FMath::IsNearlyZero(VerticalDot);

		if (HorizontalDegrees >= 5 && HorizontalDegrees < 85)
		{
			Hits.RemoveAt(i);
			i--;
		}
	}

	HitWall ? CurrentWallHits = Hits : CurrentWallHits.Reset();
}

bool UMyCharacterMovementComponent::CanStartClimbing()
{
	for (FHitResult& Hit : CurrentWallHits)
	{
		const FVector HorizontalNormal = Hit.Normal.GetSafeNormal2D();

		const float HorizontalDot = FVector::DotProduct(UpdatedComponent->GetForwardVector(), -HorizontalNormal);
		const float VerticalDot = FVector::DotProduct(Hit.Normal, HorizontalNormal);

		const float HorizontalDegrees = FMath::RadiansToDegrees(FMath::Acos(HorizontalDot));

		const bool bIsCeiling = FMath::IsNearlyZero(VerticalDot);
		
		if (HorizontalDegrees <= MinHorizontalDegreesToStartClimbing &&
			!bIsCeiling && IsFacingSurface(VerticalDot))
		{
			return true;
		}
	}

	return false;
}

bool UMyCharacterMovementComponent::CanStartCover()
{
	for (int i = 0; i < CurrentWallHits.Num(); i++)
	{
		FHitResult& Hit = CurrentWallHits[i];

		const FVector HorizontalNormal = Hit.Normal.GetSafeNormal2D();

		const float HorizontalDot = FVector::DotProduct(UpdatedComponent->GetForwardVector(), -HorizontalNormal);
		const float VerticalDot = FVector::DotProduct(Hit.Normal, HorizontalNormal);

		const float HorizontalDegrees = FMath::RadiansToDegrees(FMath::Acos(HorizontalDot));

		const bool bIsCeiling = FMath::IsNearlyZero(VerticalDot);

		if (HorizontalDegrees <= MinHorizontalDegreesToStartClimbing && !bIsCeiling)
		{
			return true;
		}
	}

	return false;
}

bool UMyCharacterMovementComponent::IsFacingSurface(const float Steepness) const
{
	constexpr float BaseLength = 80;
	const float SteepnessMultiplier = 1 + (1 - Steepness) * 5;
	
	return EyeHeightTrace(BaseLength * SteepnessMultiplier);
}

bool UMyCharacterMovementComponent::EyeHeightTrace(const float TraceDistance) const
{
	FHitResult UpperEdgeHit;

	const float BaseEyeHeight = GetCharacterOwner()->BaseEyeHeight;
	const float EyeHeightOffset = IsClimbing() ? BaseEyeHeight + ClimbingCollisionShrinkAmount : BaseEyeHeight;
	
	const FVector Start = UpdatedComponent->GetComponentLocation() + UpdatedComponent->GetUpVector() * EyeHeightOffset;
	const FVector End = Start + (UpdatedComponent->GetForwardVector() * TraceDistance);

	return GetWorld()->LineTraceSingleByChannel(UpperEdgeHit, Start, End, ECC_WorldStatic, ClimbQueryParams);
}

bool UMyCharacterMovementComponent::BackEyeHeightTrace(const float TraceDistance) const
{
	FHitResult UpperEdgeHit;

	const float BaseEyeHeight = GetCharacterOwner()->BaseEyeHeight;
	const float EyeHeightOffset = IsCover() ? BaseEyeHeight + ClimbingCollisionShrinkAmount : BaseEyeHeight;

	const FVector Start = UpdatedComponent->GetComponentLocation() + UpdatedComponent->GetUpVector() * EyeHeightOffset;

	const UCapsuleComponent* Capsule = CharacterOwner->GetCapsuleComponent();
	const FVector End = Start + (Capsule->GetForwardVector() * TraceDistance);

	return GetWorld()->LineTraceSingleByChannel(UpperEdgeHit, Start, End, ECC_WorldStatic, ClimbQueryParams);
}

void UMyCharacterMovementComponent::OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity)
{
	if (bWantsToClimb)
	{
 		SetMovementMode(EMovementMode::MOVE_Custom, ECustomMovementMode::CMOVE_Climbing);
	}
	else if (bWantsToCover)
	{
		SetMovementMode(EMovementMode::MOVE_Custom, ECustomMovementMode::CMOVE_Cover);
	}

	Super::OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);
}

void UMyCharacterMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	if (IsClimbing())
	{
		bOrientRotationToMovement = false;
	
		UCapsuleComponent* Capsule = CharacterOwner->GetCapsuleComponent();
		Capsule->SetCapsuleHalfHeight(Capsule->GetUnscaledCapsuleHalfHeight() - ClimbingCollisionShrinkAmount);
	}
	else if (IsCover())
	{
		bOrientRotationToMovement = false;
	}

	const bool bWasClimbing = PreviousMovementMode == MOVE_Custom && PreviousCustomMode == CMOVE_Climbing;
	const bool bWasCover = PreviousMovementMode == MOVE_Custom && PreviousCustomMode == CMOVE_Cover;
	if (bWasCover)
	{
		bOrientRotationToMovement = true;
	}
	else if (bWasClimbing && !playingLedgeClimbAnim)
	{
		bOrientRotationToMovement = true;

		SetRotationToStand();

		UCapsuleComponent* Capsule = CharacterOwner->GetCapsuleComponent();
		Capsule->SetCapsuleHalfHeight(Capsule->GetUnscaledCapsuleHalfHeight() + ClimbingCollisionShrinkAmount);

		StopMovementImmediately();
	}

	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);
}

void UMyCharacterMovementComponent::SetRotationToStand() const
{
	const FRotator StandRotation = FRotator(0, UpdatedComponent->GetComponentRotation().Yaw, 0);
	UpdatedComponent->SetRelativeRotation(StandRotation);
}

void UMyCharacterMovementComponent::PhysCustom(float deltaTime, int32 Iterations)
{
	if (CustomMovementMode == ECustomMovementMode::CMOVE_Climbing)
	{
		PhysClimbing(deltaTime, Iterations);
	}
	else if (CustomMovementMode == ECustomMovementMode::CMOVE_Cover)
	{
		PhysCover(deltaTime, Iterations);
	}
	
	Super::PhysCustom(deltaTime, Iterations);
}

void UMyCharacterMovementComponent::UpdateClimbDashState(float deltaTime)
{
	if (!bIsClimbDashing)
	{
		return;
	}

	CurrentClimbDashTime += deltaTime;

	// Better to cache it when dash starts
	float MinTime, MaxTime;
	ClimbDashCurve->GetTimeRange(MinTime, MaxTime);
	
	if (CurrentClimbDashTime >= MaxTime)
	{
		StopClimbDashing();
	}
}

void UMyCharacterMovementComponent::PhysClimbing(float deltaTime, int32 Iterations)
{
	if (deltaTime < MIN_TICK_TIME)
	{
		return;
	}

	ComputeSurfaceInfo();
	
	if (ShouldStopClimbing() || ClimbDownToFloor())
	{
		StopClimbing(deltaTime, Iterations);
		return;
	}

	UpdateClimbDashState(deltaTime);

	ComputeClimbingVelocity(deltaTime);

	const FVector OldLocation = UpdatedComponent->GetComponentLocation();
	
	MoveAlongClimbingSurface(deltaTime);

	TryClimbUpLedge();

	if (!HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
	{
		Velocity = (UpdatedComponent->GetComponentLocation() - OldLocation) / deltaTime;
	}
	
	SnapToClimbingSurface(deltaTime);
}

void UMyCharacterMovementComponent::PhysCover(float deltaTime, int32 Iterations)
{
	if (deltaTime < MIN_TICK_TIME)
	{
		return;
	}

	ComputeSurfaceInfo();
	ComputeCoverVelocity(deltaTime);

	const FVector OldLocation = UpdatedComponent->GetComponentLocation();

	MoveAlongSideSurface(deltaTime, Iterations);

	TryCrouchCover();

	if (!HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
	{
		Velocity = (UpdatedComponent->GetComponentLocation() - OldLocation) / deltaTime;
	}

	//Rotate player mesh and capsule compoenent along the wall
	const FVector Start = UpdatedComponent->GetComponentLocation();
	const FVector End = Start + (UpdatedComponent->GetForwardVector() * 100);
	FHitResult hit;
	GetWorld()->LineTraceSingleByChannel(hit, Start, End, ECC_WorldStatic, ClimbQueryParams);

	if (hit.GetActor() == nullptr)
	{
		TryToggleCover();
		return;
	}

	UCapsuleComponent* Capsule = CharacterOwner->GetCapsuleComponent();
	Capsule->SetWorldRotation((-hit.ImpactNormal).Rotation());
	CharacterOwner->GetMesh()->SetWorldRotation(hit.ImpactNormal.Rotation() + relativeMeshRotation);
}

void UMyCharacterMovementComponent::ComputeSurfaceInfo()
{
	CurrentClimbingNormal = FVector::ZeroVector;
	CurrentClimbingPosition = FVector::ZeroVector;

	if (CurrentWallHits.IsEmpty())
	{
		return;
	}
	
	const FVector Start = UpdatedComponent->GetComponentLocation();
	const FCollisionShape CollisionSphere = FCollisionShape::MakeSphere(6);
	
	for (const FHitResult& WallHit : CurrentWallHits)
	{
		const FVector End = Start + (WallHit.ImpactPoint - Start).GetSafeNormal() * 120;
		
		FHitResult AssistHit;
		GetWorld()->SweepSingleByChannel(AssistHit, Start, End, FQuat::Identity,
		                                 ECC_WorldStatic, CollisionSphere, ClimbQueryParams);
		
		CurrentClimbingPosition += AssistHit.Location;
		CurrentClimbingNormal += AssistHit.Normal;
	}
	
	CurrentClimbingPosition /= CurrentWallHits.Num();
	CurrentClimbingNormal = CurrentClimbingNormal.GetSafeNormal();
}

bool UMyCharacterMovementComponent::ShouldStopClimbing() const
{
	const bool bIsOnCeiling = FVector::Parallel(CurrentClimbingNormal, FVector::UpVector);
	
	return !bWantsToClimb || CurrentClimbingNormal.IsZero() || bIsOnCeiling;
}

void UMyCharacterMovementComponent::StopClimbing(float deltaTime, int32 Iterations)
{
	StopClimbDashing();

	bWantsToClimb = false;
	SetMovementMode(EMovementMode::MOVE_Falling);
	StartNewPhysics(deltaTime, Iterations);
}

bool UMyCharacterMovementComponent::ClimbDownToFloor() const
{
	FHitResult FloorHit;
	if (!CheckFloor(FloorHit))
	{
		return false;
	}

	const bool bOnWalkableFloor = FloorHit.Normal.Z > GetWalkableFloorZ();
	
	const float DownSpeed = FVector::DotProduct(Velocity, -FloorHit.Normal);
	const bool bIsMovingTowardsFloor = DownSpeed >= MaxClimbingSpeed / 3 && bOnWalkableFloor;
	
	const bool bIsClimbingFloor = CurrentClimbingNormal.Z > GetWalkableFloorZ();
	
	return bIsMovingTowardsFloor || (bIsClimbingFloor && bOnWalkableFloor);
}

bool UMyCharacterMovementComponent::CheckFloor(FHitResult& FloorHit) const
{
	const FVector Start = UpdatedComponent->GetComponentLocation() + (UpdatedComponent->GetUpVector() * - 20);
	const FVector End = Start + FVector::DownVector * FloorCheckDistance;

	return GetWorld()->LineTraceSingleByChannel(FloorHit, Start, End, ECC_WorldStatic, ClimbQueryParams);
}

bool UMyCharacterMovementComponent::HasReachedEdge() const
{
	const UCapsuleComponent* Capsule = CharacterOwner->GetCapsuleComponent();
	const float TraceDistance = Capsule->GetUnscaledCapsuleRadius() * 2.5f;

	return !EyeHeightTrace(TraceDistance);
}

bool UMyCharacterMovementComponent::HasReachedEdgeCover() const
{
	const UCapsuleComponent* Capsule = CharacterOwner->GetCapsuleComponent();
	const float TraceDistance = Capsule->GetUnscaledCapsuleRadius() * 2.5f;

	return !BackEyeHeightTrace(TraceDistance);
}

bool UMyCharacterMovementComponent::CanMoveToLedgeClimbLocation() const
{
	const UCapsuleComponent* Capsule = CharacterOwner->GetCapsuleComponent();

	// Could use a property instead for fine-tuning.
	const FVector VerticalOffset = FVector::UpVector * 160.f;
	const FVector HorizontalOffset = UpdatedComponent->GetForwardVector() * 100.f;

	const FVector CheckLocation = UpdatedComponent->GetComponentLocation() + HorizontalOffset + VerticalOffset;
	
	if (!IsLocationWalkable(CheckLocation))
	{
		return false;
	}
	
	FHitResult CapsuleHit;
	const FVector CapsuleStartCheck = CheckLocation - HorizontalOffset;

	const bool bBlocked = GetWorld()->SweepSingleByChannel(CapsuleHit, CapsuleStartCheck,CheckLocation,
		FQuat::Identity, ECC_WorldStatic, Capsule->GetCollisionShape(), ClimbQueryParams);
	
	return !bBlocked;
}

bool UMyCharacterMovementComponent::CanMoveToSide(FVector direction) const
{
	const UCapsuleComponent* Capsule = CharacterOwner->GetCapsuleComponent();

	const FVector HorizontalOffset = FVector::UpVector * 50.f;
	const FVector VerticalOffset = UpdatedComponent->GetForwardVector() * 30.f;

	FVector dirNorm = FVector(direction);
	dirNorm.Normalize();
	dirNorm *= 100;

	const FVector CheckLocation = UpdatedComponent->GetComponentLocation() + dirNorm + HorizontalOffset + VerticalOffset;

	FHitResult CapsuleHit;
	const FVector CapsuleStartCheck = CheckLocation - HorizontalOffset;

	const bool bBlocked = GetWorld()->SweepSingleByChannel(CapsuleHit, CapsuleStartCheck, CheckLocation,
		FQuat::Identity, ECC_WorldStatic, Capsule->GetCollisionShape(), ClimbQueryParams);

	return !bBlocked;
}

bool UMyCharacterMovementComponent::IsLocationWalkable(const FVector& CheckLocation) const
{
	const FVector CheckEnd = CheckLocation + (FVector::DownVector * 250);

	FHitResult LedgeHit;
	const bool bHitLedgeGround = GetWorld()->LineTraceSingleByChannel(LedgeHit, CheckLocation, CheckEnd,
	                                                                  ECC_WorldStatic, ClimbQueryParams);

	return bHitLedgeGround && LedgeHit.Normal.Z >= GetWalkableFloorZ();
}

void UMyCharacterMovementComponent::ComputeClimbingVelocity(float deltaTime)
{
	RestorePreAdditiveRootMotionVelocity();

	if (!HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
	{
		if (bIsClimbDashing)
		{
			AlignClimbDashDirection();

			const float CurrentCurveSpeed = ClimbDashCurve->GetFloatValue(CurrentClimbDashTime);
			Velocity = ClimbDashDirection * CurrentCurveSpeed;
		}
		else
		{
			constexpr float Friction = 0.0f;
			constexpr bool bFluid = false;
			CalcVelocity(deltaTime, Friction, bFluid, BrakingDecelerationClimbing);
		}
	}

	ApplyRootMotionToVelocity(deltaTime);
}

void UMyCharacterMovementComponent::ComputeCoverVelocity(float deltaTime)
{
	RestorePreAdditiveRootMotionVelocity();

	if (!HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
	{
		constexpr float Friction = 0.0f;
		constexpr bool bFluid = false;
		CalcVelocity(deltaTime, Friction, bFluid, BrakingDecelerationClimbing);
	}

	ApplyRootMotionToVelocity(deltaTime);
}

void UMyCharacterMovementComponent::AlignClimbDashDirection()
{
	const FVector HorizontalSurfaceNormal = GetClimbSurfaceNormal();
	
	ClimbDashDirection = FVector::VectorPlaneProject(ClimbDashDirection, HorizontalSurfaceNormal);
}

void UMyCharacterMovementComponent::StopClimbDashing()
{
	bIsClimbDashing = false;
	CurrentClimbDashTime = 0.f;
	ClimbDashDirection = FVector::ZeroVector;
}

float UMyCharacterMovementComponent::GetMaxSpeed() const
{
	return IsClimbing() ? MaxClimbingSpeed : Super::GetMaxSpeed();
}

float UMyCharacterMovementComponent::GetMaxAcceleration() const
{
	return IsClimbing() ? MaxClimbingAcceleration : Super::GetMaxAcceleration();
}

void UMyCharacterMovementComponent::MoveAlongClimbingSurface(float deltaTime)
{
	const FVector Adjusted = Velocity * deltaTime;
	
	FHitResult Hit(1.f);
	
	SafeMoveUpdatedComponent(Adjusted, GetClimbingRotation(deltaTime), true, Hit);
	
	if (Hit.Time < 1.f)
	{
		HandleImpact(Hit, deltaTime, Adjusted);
		SlideAlongSurface(Adjusted, (1.f - Hit.Time), Hit.Normal, Hit, true);
	}
}

void UMyCharacterMovementComponent::MoveAlongSideSurface(float deltaTime, int32 Iterations)
{
	if (deltaTime < MIN_TICK_TIME)
	{
		return;
	}

	bJustTeleported = false;
	bool bCheckedFall = false;
	bool bTriedLedgeMove = false;
	float remainingTime = deltaTime;

	// Perform the move
	while ((remainingTime >= MIN_TICK_TIME) && (Iterations < MaxSimulationIterations) && CharacterOwner && (CharacterOwner->Controller || bRunPhysicsWithNoController || HasAnimRootMotion() || CurrentRootMotion.HasOverrideVelocity() || (CharacterOwner->GetLocalRole() == ROLE_SimulatedProxy)))
	{
		Iterations++;
		bJustTeleported = false;
		const float timeTick = GetSimulationTimeStep(remainingTime, Iterations);
		remainingTime -= timeTick;

		// Save current values
		UPrimitiveComponent* const OldBase = GetMovementBase();
		const FVector PreviousBaseLocation = (OldBase != NULL) ? OldBase->GetComponentLocation() : FVector::ZeroVector;
		const FVector OldLocation = UpdatedComponent->GetComponentLocation();
		const FFindFloorResult OldFloor = CurrentFloor;

		RestorePreAdditiveRootMotionVelocity();

		// Ensure velocity is horizontal.
		MaintainHorizontalGroundVelocity();
		const FVector OldVelocity = Velocity;
		Acceleration.Z = 0.f;

		// Apply acceleration
		if (!HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
		{
			CalcVelocity(timeTick, GroundFriction, false, GetMaxBrakingDeceleration());
		}

		ApplyRootMotionToVelocity(timeTick);

		// Compute move parameters
		const FVector MoveVelocity = Velocity;
		const FVector Delta = timeTick * MoveVelocity;

		//If player can move that means is out of the cover
		if (CanMoveToSide(Velocity))
			return;

		const bool bZeroDelta = Delta.IsNearlyZero();
		FStepDownResult StepDownResult;

		if (bZeroDelta)
		{
			remainingTime = 0.f;
		}
		else
		{
			MoveAlongFloor(MoveVelocity, timeTick, &StepDownResult);
		}

		// Update floor.
		// StepUp might have already done it for us.
		if (StepDownResult.bComputedFloor)
		{
			CurrentFloor = StepDownResult.FloorResult;
		}
		else
		{
			FindFloor(UpdatedComponent->GetComponentLocation(), CurrentFloor, bZeroDelta, NULL);
		}

		// check for ledges here
		const bool bCheckLedges = !CanWalkOffLedges();
		if (bCheckLedges && !CurrentFloor.IsWalkableFloor())
		{
			// calculate possible alternate movement
			const FVector GravDir = FVector(0.f, 0.f, -1.f);
			const FVector NewDelta = bTriedLedgeMove ? FVector::ZeroVector : GetLedgeMove(OldLocation, Delta, GravDir);
			if (!NewDelta.IsZero())
			{
				// first revert this move
				RevertMove(OldLocation, OldBase, PreviousBaseLocation, OldFloor, false);

				// avoid repeated ledge moves if the first one fails
				bTriedLedgeMove = true;

				// Try new movement direction
				Velocity = NewDelta / timeTick;
				remainingTime += timeTick;
				continue;
			}
			else
			{
				// see if it is OK to jump
				// @todo collision : only thing that can be problem is that oldbase has world collision on
				bool bMustJump = bZeroDelta || (OldBase == NULL || (!OldBase->IsQueryCollisionEnabled() && MovementBaseUtility::IsDynamicBase(OldBase)));
				if ((bMustJump || !bCheckedFall) && CheckFall(OldFloor, CurrentFloor.HitResult, Delta, OldLocation, remainingTime, timeTick, Iterations, bMustJump))
				{
					return;
				}
				bCheckedFall = true;

				// revert this move
				RevertMove(OldLocation, OldBase, PreviousBaseLocation, OldFloor, true);
				remainingTime = 0.f;
				break;
			}
		}
		else
		{
			// Validate the floor check
			if (CurrentFloor.IsWalkableFloor())
			{
				if (ShouldCatchAir(OldFloor, CurrentFloor))
				{
					HandleWalkingOffLedge(OldFloor.HitResult.ImpactNormal, OldFloor.HitResult.Normal, OldLocation, timeTick);
					if (IsMovingOnGround())
					{
						// If still walking, then fall. If not, assume the user set a different mode they want to keep.
						StartFalling(Iterations, remainingTime, timeTick, Delta, OldLocation);
					}
					return;
				}

				AdjustFloorHeight();
				SetBase(CurrentFloor.HitResult.Component.Get(), CurrentFloor.HitResult.BoneName);
			}
			else if (CurrentFloor.HitResult.bStartPenetrating && remainingTime <= 0.f)
			{
				// The floor check failed because it started in penetration
				// We do not want to try to move downward because the downward sweep failed, rather we'd like to try to pop out of the floor.
				FHitResult Hit(CurrentFloor.HitResult);
				Hit.TraceEnd = Hit.TraceStart + FVector(0.f, 0.f, MAX_FLOOR_DIST);
				const FVector RequestedAdjustment = GetPenetrationAdjustment(Hit);
				ResolvePenetration(RequestedAdjustment, Hit, UpdatedComponent->GetComponentQuat());
				bForceNextFloorCheck = true;
			}

			// check if just entered water
			if (IsSwimming())
			{
				StartSwimming(OldLocation, Velocity, timeTick, remainingTime, Iterations);
				return;
			}

			// See if we need to start falling.
			if (!CurrentFloor.IsWalkableFloor() && !CurrentFloor.HitResult.bStartPenetrating)
			{
				const bool bMustJump = bJustTeleported || bZeroDelta || (OldBase == NULL || (!OldBase->IsQueryCollisionEnabled() && MovementBaseUtility::IsDynamicBase(OldBase)));
				if ((bMustJump || !bCheckedFall) && CheckFall(OldFloor, CurrentFloor.HitResult, Delta, OldLocation, remainingTime, timeTick, Iterations, bMustJump))
				{
					return;
				}
				bCheckedFall = true;
			}
		}


		// Allow overlap events and such to change physics state and velocity
		if (IsMovingOnGround())
		{
			// Make velocity reflect actual move
			if (!bJustTeleported && !HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity() && timeTick >= MIN_TICK_TIME)
			{
				// TODO-RootMotionSource: Allow this to happen during partial override Velocity, but only set allowed axes?
				Velocity = (UpdatedComponent->GetComponentLocation() - OldLocation) / timeTick;
				MaintainHorizontalGroundVelocity();
			}
		}

		// If we didn't move at all this iteration then abort (since future iterations will also be stuck).
		if (UpdatedComponent->GetComponentLocation() == OldLocation)
		{
			remainingTime = 0.f;
			break;
		}
	}

	if (IsMovingOnGround())
	{
		MaintainHorizontalGroundVelocity();
	}
}

FQuat UMyCharacterMovementComponent::GetClimbingRotation(float deltaTime) const
{
	const FQuat Current = UpdatedComponent->GetComponentQuat();

	if (HasAnimRootMotion() || CurrentRootMotion.HasOverrideVelocity())
	{
		return Current;
	}
	
	const FQuat Target = FRotationMatrix::MakeFromX(-CurrentClimbingNormal).ToQuat();
	const float RotationSpeed = ClimbingRotationSpeed * FMath::Max(1, Velocity.Length() / MaxClimbingSpeed);

	return FMath::QInterpTo(Current, Target, deltaTime, RotationSpeed);
}

bool UMyCharacterMovementComponent::TryClimbUpLedge()
{
	if (AnimInstance && AnimInstance->Montage_IsPlaying(LedgeClimbMontage))
	{
		return false;
	}
	
	const float UpSpeed = FVector::DotProduct(Velocity, UpdatedComponent->GetUpVector());
	const bool bIsMovingUp = UpSpeed >= MaxClimbingSpeed / 3;
	
	if (bIsMovingUp && HasReachedEdge() && CanMoveToLedgeClimbLocation())
	{
		Cast<UCameraCompThief>(Cast<AThief>(GetOwner())->cameraComponent)->CameraBoom->bDoCollisionTest = false;

		AnimInstance->Montage_Play(LedgeClimbMontage);
		playingLedgeClimbAnim = true;
		
		return true;
	}
	
	return false;
}

bool UMyCharacterMovementComponent::UMyCharacterMovementComponent::TryCrouchCover()
{
	if (HasReachedEdgeCover()/* && CanMoveToLedgeClimbLocation()*/)
	{
		if (!CrouchCover)
		{
			CrouchCover = true;
			// Ajust capsule
			UCapsuleComponent* Capsule = CharacterOwner->GetCapsuleComponent();
			Capsule->SetCapsuleHalfHeight(Capsule->GetUnscaledCapsuleHalfHeight() - CrouchedHalfHeight);
		}

		return true;
	}
	else if (CrouchCover)
	{
		// Ajust capsule
		UCapsuleComponent* Capsule = CharacterOwner->GetCapsuleComponent();
		Capsule->SetCapsuleHalfHeight(Capsule->GetUnscaledCapsuleHalfHeight() + CrouchedHalfHeight);
	}

	CrouchCover = false;
	return false;
}

void UMyCharacterMovementComponent::SnapToClimbingSurface(float deltaTime) const
{
	const FVector Forward = UpdatedComponent->GetForwardVector();
	const FVector Location = UpdatedComponent->GetComponentLocation();
	const FQuat Rotation = UpdatedComponent->GetComponentQuat();
	
	const FVector ForwardDifference = (CurrentClimbingPosition - Location).ProjectOnTo(Forward);
	
	const FVector Offset = -CurrentClimbingNormal * (ForwardDifference.Length() - DistanceFromSurface);

	constexpr bool bSweep = true;

	const float SnapSpeed = ClimbingSnapSpeed * ((Velocity.Length() / MaxClimbingSpeed) + 1);
	UpdatedComponent->MoveComponent(Offset * SnapSpeed * deltaTime, Rotation, bSweep);
}

void UMyCharacterMovementComponent::TryClimbing_Implementation()
{
	if (CanStartClimbing())
	{
		if (bWantsToCover)
		{
			bWantsToCover = false;
			
			//Rerotate player
			CharacterOwner->GetMesh()->SetWorldRotation(CharacterOwner->GetActorForwardVector().Rotation() + relativeMeshRotation);

			SetMovementMode(MOVE_Walking);
		}

		bWantsToClimb = true;
	}
}

void UMyCharacterMovementComponent::TryClimbDashing()
{
	if (ClimbDashCurve && bIsClimbDashing == false)
	{
		bIsClimbDashing = true;
		CurrentClimbDashTime = 0.f;
		
		StoreClimbDashDirection();
	}
}

void UMyCharacterMovementComponent::TryToggleCover_Implementation()
{
	if (bWantsToCover)
	{
		bWantsToCover = false;
		
		if (CrouchCover)
		{
			CrouchCover = false;

			UCapsuleComponent* Capsule = CharacterOwner->GetCapsuleComponent();
			Capsule->SetCapsuleHalfHeight(Capsule->GetUnscaledCapsuleHalfHeight() + CrouchedHalfHeight);
		}

		//Rerotate player
		CharacterOwner->GetMesh()->SetWorldRotation(CharacterOwner->GetActorForwardVector().Rotation() + relativeMeshRotation);

		SetMovementMode(MOVE_Walking);
	}
	else if (CanStartCover())
	{
		bWantsToCover = true;
	}
}

void UMyCharacterMovementComponent::StoreClimbDashDirection()
{
	ClimbDashDirection = UpdatedComponent->GetUpVector();

	const float AccelerationThreshold = MaxClimbingAcceleration / 10;
	if (Acceleration.Length() > AccelerationThreshold)
	{
		ClimbDashDirection = Acceleration.GetSafeNormal();
	}
}

void UMyCharacterMovementComponent::CancelClimbing()
{
	bWantsToClimb = false;
}

FVector UMyCharacterMovementComponent::GetClimbSurfaceNormal() const
{
	return CurrentClimbingNormal;
}

FVector UMyCharacterMovementComponent::GetClimbDashDirection() const
{
	return ClimbDashDirection;
}

bool UMyCharacterMovementComponent::IsClimbing() const
{
	return MovementMode == EMovementMode::MOVE_Custom && CustomMovementMode == ECustomMovementMode::CMOVE_Climbing;
}

bool UMyCharacterMovementComponent::IsCover() const
{
	return MovementMode == EMovementMode::MOVE_Custom && CustomMovementMode == ECustomMovementMode::CMOVE_Cover;
}

bool UMyCharacterMovementComponent::IsClimbDashing() const
{
	return IsClimbing() && bIsClimbDashing;
}

void UMyCharacterMovementComponent::animationLedgeFinished()
{
	playingLedgeClimbAnim = false;

	bOrientRotationToMovement = true;

	SetRotationToStand();

	UCapsuleComponent* Capsule = CharacterOwner->GetCapsuleComponent();
	Capsule->SetCapsuleHalfHeight(Capsule->GetUnscaledCapsuleHalfHeight() + ClimbingCollisionShrinkAmount);

	StopMovementImmediately();

	Cast<UCameraCompThief>(Cast<AThief>(GetOwner())->cameraComponent)->CameraBoom->bDoCollisionTest = false;
}