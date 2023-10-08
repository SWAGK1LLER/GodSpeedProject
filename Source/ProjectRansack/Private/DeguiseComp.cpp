#include "DeguiseComp.h"
#include "Engine/SkeletalMesh.h"
#include "Base3C.h"
#include "Thief.h"
#include "Officer.h"
#include "GamePlayerController.h"
#include "Animation/AnimInstance.h"

UDeguiseComp::UDeguiseComp()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UDeguiseComp::BeginPlay()
{
	Super::BeginPlay();
}

void UDeguiseComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UDeguiseComp::finishAttachement()
{
    originalMesh = pawn->GetMesh()->GetSkeletalMeshAsset();
    originalAnim = pawn->GetMesh()->GetAnimInstance();
}

void UDeguiseComp::MUlFire_Implementation()
{
    if (abilityUsed)
        return;

    abilityUsed = true;

    if (pawn->IsA(AThief::StaticClass()))
    {
        pawn->GetMesh()->SetSkeletalMesh(officerMesh);
        pawn->GetMesh()->SetAnimInstanceClass(officerAnim);
        if (controller != nullptr)
        {
            controller->SwithcSkeletalMesh(this, officerMesh, officerAnim.Get());
        }
    }
    else
    {
        pawn->GetMesh()->SetSkeletalMesh(thiefMesh);
        pawn->GetMesh()->SetAnimInstanceClass(thiefAnim);
        if (controller != nullptr)
        {
            controller->SwithcSkeletalMesh(this, thiefMesh, thiefAnim.Get());
        }
    }

    

    FTimerHandle Handle;
    GetWorld()->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([this] {
        abilityUsed = false;
        pawn->GetMesh()->SetSkeletalMesh(originalMesh);
        pawn->GetMesh()->SetAnimInstanceClass(originalAnim->GetClass());

        if (controller != nullptr)
        {
            controller->SwithcSkeletalMesh(this, originalMesh, originalAnim->GetClass());
        }

    }), duration, false);
}

void UDeguiseComp::MUlToggleVisibility_Implementation(bool visible)
{
    isActive = visible;
}

void UDeguiseComp::UpdateUI_Implementation()
{
    pawn->WidgetUI->ShowDeguisementEquiped();
}

void UDeguiseComp::SetMesh(USkeletalMesh* mesh, UClass* anim)
{
    pawn->GetMesh()->SetSkeletalMesh(mesh);
    pawn->GetMesh()->SetAnimInstanceClass(anim);
}