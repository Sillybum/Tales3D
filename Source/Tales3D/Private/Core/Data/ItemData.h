// Title: TalesWeaver 3D clone game / License: GPLv3 / Github: Sillybum

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ItemData.generated.h"

/**
 * 인벤토리 아이템 MetaData
 * ItemId = FName
 * Primary Asset ID ex) Item:Potion
 */
UCLASS(BlueprintType)
class UItemData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	static const FName AssetTypeName;
	// Key Value (Unique)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item")
	FName ItemId = NAME_None;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item")
	FText DisplayName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item", meta=(MultiLine=true))
	FText Description;
	
	// soft ref: loads when needed
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item")
	TSoftObjectPtr<UTexture2D> Icon;
	
	// UI: Item itself can show its info
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Item|UI")
	void GetDetailLines(TArray<FText>& OutLines) const;
	virtual void GetDetailLines_Implementation(TArray<FText>& OutLines) const;
	
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
};
