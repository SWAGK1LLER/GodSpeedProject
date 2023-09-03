#include "Inventory.h"
#include "Item.h"


FItemLooted::FItemLooted(AItem& pItem, FPosition pPos) : item(&pItem), position(pPos)
{
}

FItemLooted::FItemLooted(AItem& pItem) : item(&pItem)
{
}

bool FItemLooted::operator==(const FItemLooted& pOther)
{
	return item == pOther.item;
}

AItem& FItemLooted::GetItem()
{
	return *item;
}

bool UInventory::ValidateSpace(AItem& pItem)
{
	FSize size = pItem.SizeNeeded;

	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			bool haveSpaceY = true;
			for (int y = i; y < i + size.SizeX; y++)
				if (space[y][j] != nullptr || y == 2)
				{
					haveSpaceY = false;
					break;
				}

			if (!haveSpaceY)
				continue;


			bool haveSpaceX = true;
			for (int x = j; x < j + size.SizeY; x++)
				if (space[i][x] != nullptr || x == 4)
				{
					haveSpaceX = false;
					break;
				}

			if (!haveSpaceX)
				continue;

			return true;
		}
	}

	return false;
}

bool UInventory::ValidateSpace(AItem& pItem, FPosition& pOut)
{
	FSize size = pItem.SizeNeeded;

	for ( int i = 0; i < 2; i++)
	{
		for ( int j = 0; j < 4; j++)
		{
			bool haveSpaceY = true;
			for ( int y = i; y < i + size.SizeX; y++)
				if (space[y][j] != nullptr || y == 2)
				{
					haveSpaceY = false;
					break;
				}

			if (!haveSpaceY)
				continue;


			bool haveSpaceX = true;
			for ( int x = j; x < j + size.SizeY; x++)
				if (space[i][x] != nullptr || x == 4)
				{
					haveSpaceX = false;
					break;
				}

			if (!haveSpaceX)
				continue;

			pOut.X = i;
			pOut.Y = j;
			return true;
		}
	}

	return false;
}

void UInventory::AddItem(AItem& pItem, FPosition& pPos)
{
	if (pPos.X == -1 || pPos.Y == -1)
		return;

	score += pItem.score;
	items.Add(FItemLooted(pItem, pPos));

	for ( int i = pPos.X; i < pPos.X + pItem.SizeNeeded.SizeX; i++)
		for ( int j = pPos.Y; j < pPos.Y + pItem.SizeNeeded.SizeY; j++)
			space[i][j] = &pItem;
}

void UInventory::AddItem(AItem& pItem)
{
	FPosition pos;
	if (!ValidateSpace(pItem, pos))
		return;

	AddItem(pItem, pos);
}

void UInventory::RemoveItem(AItem& pItem)
{
	if (items.Remove(FItemLooted(pItem)) == 0)
		return;

	for ( int i = 0; i < 2; i++)
		for ( int j = 0; j < 4; j++)
			if (space[i][j] == &pItem)
				space[i][j] = nullptr;
}

void UInventory::ClearInventory()
{
	items.Empty();

	for (int i = 0; i < 2; i++)
		for (int j = 0; j < 4; j++)
			space[i][j] = nullptr;

	score = 0;
}