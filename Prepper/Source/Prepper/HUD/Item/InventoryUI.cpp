// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryUI.h"

#include "InventoryItemUI.h"
#include "ItemUIData.h"
#include "Prepper/Interfaces/Inventory.h"

void UInventoryUI::UpdateData()
{
	if (TargetInventory == nullptr) return;

	TArray<IInventory::InventoryItem> Items = TargetInventory->GetIter();
	ItemList->ClearListItems();
	for (int i = 0; i < Items.Num(); i++)
	{
		UItemUIData* Data = NewObject<UItemUIData>(GetWorld(), UItemUIData::StaticClass());
		IInventory::InventoryItem Item = Items[i];
		
		if (!ItemData.GetItemData(Item.ItemCode, Data->TextureIcon, Data->ItemName)) continue;
		Data->ItemCount = Items[i].Count;
		Data->ItemCode = Items[i].ItemCode;
		Data->TargetInventoryUI = this;
		
		ItemList->AddItem(Data);
	}
}

void UInventoryUI::Set(IInventory* Target)
{
	TargetInventory = Target;
}


void UInventoryUI::SetVisibility(ESlateVisibility InVisibility)
{
	Super::SetVisibility(InVisibility);
	if (InVisibility != ESlateVisibility::Visible) return;
	UpdateData();
}

void UInventoryUI::UseItem(const FString& ItemCode)
{
	TargetInventory->TryUseItem(ItemCode);
	UpdateData();
}

void UInventoryUI::NativeOnInitialized()
{
	Super::NativeOnInitialized();
    
}
