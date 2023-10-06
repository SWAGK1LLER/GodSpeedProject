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
}

void UDamageIndicatorComp::ShowDamage(FVector DamageActorLocation)
{
	ui = CreateWidget<UDamageIndicatorUI>(GetWorld(), uiClass);

	// my player
	FVector myLocation = GetOwner()->GetActorLocation();
	FRotator myRotation = GetOwner()->GetActorRotation();

	// other player
	FVector DamageLocation = DamageActorLocation;

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