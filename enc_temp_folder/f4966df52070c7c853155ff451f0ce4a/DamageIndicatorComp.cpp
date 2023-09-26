#include "DamageIndicatorComp.h"
#include "Kismet/KismetMathLibrary.h"

UDamageIndicatorComp::UDamageIndicatorComp()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UDamageIndicatorComp::BeginPlay()
{
	Super::BeginPlay();
}


void UDamageIndicatorComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	/*testTime += DeltaTime;
	if (testTime > DisappearTime)
	{
		testTime = 0;
		ShowDamage(GetOwner());
	}*/
}

void UDamageIndicatorComp::ShowDamage(AActor* DamageActor)
{
	ui = CreateWidget<UDamageIndicatorUI>(GetWorld(), uiClass);

	// my player
	FVector myLocation = GetOwner()->GetActorLocation();
	FRotator myRotation = GetOwner()->GetActorRotation();

	// other player
	FVector DamageLocation = DamageActor->GetActorLocation();

	//calculate rotation
	FRotator PlayerRot = UKismetMathLibrary::FindLookAtRotation(myLocation, DamageLocation);
	FRotator rot = UKismetMathLibrary::NormalizedDeltaRotator(myRotation, PlayerRot);

	FRotator finale;
	if (IsDynamic)
		finale = PlayerRot;
	else
		finale = rot;
	
	float length = (myLocation - DamageLocation).Length();

	bool smallerThanNearDistance = length < NearDamageDistance;

	UTexture2D* texture = smallerThanNearDistance ? NearIndicator : DistanceIndicator;

	FVector2D size = smallerThanNearDistance ? NearIndicatorSpriteSize : DistanceIndicatorSpriteSize;
	size *= UIScale;

	ui->ShowingDamage(IsDynamic, smallerThanNearDistance, DisappearTime, finale.Yaw, texture, size, IndicatorColor, IndicatorSpriteOffset, ScreenOffset);
	ui->AddToViewport();
}