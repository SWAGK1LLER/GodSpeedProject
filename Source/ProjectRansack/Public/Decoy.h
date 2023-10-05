#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Decoy.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTRANSACK_API UDecoy : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class ADecoyActor> DecoyActorClass;

	class AThief* owner = nullptr;
	float timer = 0;

	UDecoy();
	void SetupComp(class AThief* pThief);
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SpawnDecoy();
};
