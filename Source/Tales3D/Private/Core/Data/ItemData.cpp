// Title: TalesWeaver 3D clone game / License: GPLv3 / Github: Sillybum


#include "Core/Data/ItemData.h"

#include "IDetailTreeNode.h"

const FName UItemData::AssetTypeName(TEXT("Item"));

FPrimaryAssetId UItemData::GetPrimaryAssetId() const
{
	if (!ItemId.IsNone())
	{
		return FPrimaryAssetId(UItemData::AssetTypeName, ItemId);
	}
	// For occasion(no ItemId)
	return Super::GetPrimaryAssetId();
}
