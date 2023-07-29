#include "HelperClass.h"

void HelperClass::deactivateActor(AActor* actor)
{
	// Deactivate this actor
	actor->SetActorHiddenInGame(true);
	actor->SetActorEnableCollision(false);
	actor->SetActorTickEnabled(false);

	// Call on children
	TArray<AActor*> children;
	actor->GetAllChildActors(children);			// Actual children
	actor->GetAttachedActors(children, false);	// Attached actors. Don't reset array
	for (auto child : children)
		deactivateActor(child);

}

void HelperClass::activateActor(AActor* actor)
{
	// Activate this actor
	actor->SetActorHiddenInGame(false);
	actor->SetActorEnableCollision(true);
	actor->SetActorTickEnabled(true);

	// Call on children
	TArray<AActor*> children;
	actor->GetAllChildActors(children);			// Actual children
	actor->GetAttachedActors(children, false);	// Attached Actors. Don't reset array
	for (auto child : children)
		activateActor(child);
}

void HelperClass::deactivateTrigger(UShapeComponent* actor)
{
	actor->SetVisibility(false);
	actor->SetGenerateOverlapEvents(false);
	actor->SetComponentTickEnabled(false);
}

void HelperClass::activateTrigger(UShapeComponent* actor)
{
	actor->SetVisibility(true);
	actor->SetGenerateOverlapEvents(true);
	actor->SetComponentTickEnabled(true);
}

float HelperClass::mapValue(float x, float in_min, float in_max, float out_min, float out_max)
{
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}