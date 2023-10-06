#pragma once

#include "CoreMinimal.h"
#include "Grenade.h"
#include "SmokeGrenade.generated.h"

UCLASS()
class PROJECTRANSACK_API ASmokeGrenade : public AGrenade
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float timeBeforeExploseOnContact = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float effectDuration = 5;

	bool counterStarted = false;

	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

	void Explose() override;
};
