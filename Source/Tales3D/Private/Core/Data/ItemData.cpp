// Title: TalesWeaver 3D clone game / License: GPLv3 / Github: Sillybum


#include "Core/Data/ItemData.h"

#include "IDetailTreeNode.h"

const FName UItemData::AssetTypeName(TEXT("Item"));

void UItemData::GetDetailLines_Implementation(TArray<FText>& OutLines) const
{
	if (!DisplayName.IsEmpty())
	{
		OutLines.Add(DisplayName);
	}
	if (!Description.IsEmpty())
	{
		OutLines.Add(Description);
	}
}

FPrimaryAssetId UItemData::GetPrimaryAssetId() const
{
	if (!ItemId.IsNone())
	{
		return FPrimaryAssetId(UItemData::AssetTypeName, ItemId);
	}
	// For occasion(no ItemId)
	return Super::GetPrimaryAssetId();
}
