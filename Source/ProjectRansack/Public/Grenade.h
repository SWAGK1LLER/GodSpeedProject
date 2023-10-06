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

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	UParticleSystem* particleEffect = nullptr;

	AGrenade();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	void SetVelocity(FVector velocity);

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) {};

	virtual void Explose() {};
};
