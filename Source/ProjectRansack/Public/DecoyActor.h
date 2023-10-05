#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CollisionQueryParams.h"
#include "DecoyActor.generated.h"

UCLASS()
class PROJECTRANSACK_API ADecoyActor : public ACharacter
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float lifeSpan = 5;

	bool move = true;
	FCollisionQueryParams QueryParams;

	ADecoyActor();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	void Move();
};
