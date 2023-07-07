#pragma once

#include "CoreMinimal.h"
#include "Inventory.generated.h"

USTRUCT(BlueprintType)
struct FPosition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int X = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Y = -1;

	FPosition(int x, int y) : X(x), Y(y) {}
	FPosition() {}
};

USTRUCT(BlueprintType)
struct FItemLooted
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class AItem* item = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FPosition position;

	FItemLooted(class AItem& pItem, FPosition pPos);
	FItemLooted(class AItem& pItem);
	FItemLooted() {};
	bool operator==(const FItemLooted& pOther);

	class AItem& GetItem();
};

class PROJECTRANSACK_API Inventory
{
public:
	TArray<FItemLooted> items;
	class AItem* space[2][4];

	Inventory();
	~Inventory();

	bool ValidateSpace(class AItem& pItem);
	bool ValidateSpace(class AItem& pItem, FPosition& pOut);
	void AddItem(class AItem& pItem, FPosition& pPos);
	void AddItem(class AItem& pItem);
	void RemoveItem(class AItem& pItem);
	void ClearInventory();
};
