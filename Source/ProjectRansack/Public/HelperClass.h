#pragma once
#include "GameFramework/Actor.h"
#include "Containers/Array.h"
#include <Components/ShapeComponent.h>

class HelperClass
{
public:
	static void deactivateActor(AActor* actor);
	static void activateActor(AActor* actor);

	static void deactivateTrigger(UShapeComponent* actor);
	static void activateTrigger(UShapeComponent* actor);

	static float mapValue(float x, float in_min, float in_max, float out_min, float out_max);
};