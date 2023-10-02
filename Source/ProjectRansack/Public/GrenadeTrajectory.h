#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Grenade.h"
#include "GrenadeTrajectory.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTRANSACK_API UGrenadeTrajectory : public USceneComponent
{
	GENERATED_BODY()

public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AGrenade> GrenadeClass;

	UGrenadeTrajectory();

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void ThrowGrenade();
};
