// Fill out your copyright notice in the Description page of Project Settings.


#include "DuffleBagUI.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/WidgetTree.h"
#include "Components/GridPanel.h"
#include "Components/GridSlot.h"
#include <Components/Image.h>
#include "Item.h"

void UDuffleBagUI::UpdateUI(const TArray<FItemLooted>& items)
{
    grid->ClearChildren();

    //Hack to set full span of grid
    UImage* imgBackground = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
    imgBackground->SetOpacity(0);
    UGridSlot* slotBg = grid->AddChildToGrid(imgBackground, 0, 0);
    slotBg->SetColumnSpan(4);
    slotBg->SetRowSpan(2);

    for (const FItemLooted& item : items)
    {
        UImage* img = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
        img->SetBrushFromTexture(item.item->itemUIImg);

        UGridSlot* slot = grid->AddChildToGrid(img, item.position.X, item.position.Y);
        slot->SetPadding(FMargin(5, 5, 5, 5));
        
        slot->SetRowSpan(item.item->SizeNeeded.SizeX);
        slot->SetColumnSpan(item.item->SizeNeeded.SizeY);
    }
}