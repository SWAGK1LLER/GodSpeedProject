#pragma once

#include "CoreMinimal.h"
#include "Grenade.h"
#include "StunGrenade.generated.h"

UCLASS()
class PROJECTRANSACK_API AStunGrenade : public AGrenade
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Radius = 300;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StunDuration = 5;

	AStunGrenade();

	void BeginPlay();

	bool exploded = false;

	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

	void Explose() override;

	void CheckHit();
};
