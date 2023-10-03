#include "GenericParticleSystemComponent.h"
#include "GameFramework/Actor.h"
#include "TimerManager.h"
#include "Engine/World.h"

void UGenericParticleSystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (startTimer)
	{
		timeCalculate += DeltaTime;
		if (timeCalculate >= duration)
		{
			startTimer = false;
			DeactivateSystem();
		}
	}
}

void UGenericParticleSystemComponent::setLifeSpan(float pDuration)
{
	duration = pDuration;
	startTimer = true;
	timeCalculate = 0;


	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([&] { 
		DeactivateImmediate();
		KillParticlesForced();
		
		}), pDuration, false);
}