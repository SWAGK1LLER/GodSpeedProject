#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Grenade.generated.h"

UCLASS()
class PROJECTRANSACK_API AGrenade : public AActor
{
	GENERATED_BODY()
	
public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* mesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float timeBeforeExploseOnContact = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float effectDuration = 5;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	UParticleSystem* particleEffect = nullptr;

	bool counterStarted = false;

	FVector startingVelo;

	AGrenade();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	void Explose();

	void SetVelocity(FVector velocity);
};
