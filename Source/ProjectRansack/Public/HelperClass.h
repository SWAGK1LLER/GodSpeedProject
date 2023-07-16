#pragma once
#include "GameFramework/Actor.h"
#include "Containers/Array.h"

class HelperClass
{
public:
	static void deactivateActor(AActor* actor);
	static void activateActor(AActor* actor);

};

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