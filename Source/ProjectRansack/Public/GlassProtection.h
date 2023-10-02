#pragma once

#include "CoreMinimal.h"
#include "ProtectionLoot.h"
#include "Components/TimelineComponent.h"
#include "GlassProtection.generated.h"

UENUM(BlueprintType)
enum class EGlassSlidingAxis : uint8
{
	X,
	Y,
	Z
};

UCLASS()
class PROJECTRANSACK_API AGlassProtection : public AProtectionLoot
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MotionVision, meta = (AllowPrivateAccess = "true"))
	EGlassSlidingAxis slidingAxis = EGlassSlidingAxis::Z;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MotionVision, meta = (AllowPrivateAccess = "true"))
	UCurveFloat* GlassAnimation = nullptr;

	FTimeline GlassTimeLine;
	FVector startingLocation;

	bool animationRunning = false;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void TimelineProgress(float value);

	UFUNCTION()
	void TimelineFinished();

	void PlayAnimation(bool reverse = false) override;
};
