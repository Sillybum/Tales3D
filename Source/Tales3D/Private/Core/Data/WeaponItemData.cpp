// Title: TalesWeaver 3D clone game / License: GPLv3 / Github: Sillybum


#include "Core/Data/WeaponItemData.h"

void UWeaponItemData::GetDetailLines_Implementation(TArray<FText>& OutLines) const
{
	// Item name
	if (!DisplayName.IsEmpty())
	{
		OutLines.Add(DisplayName);
	}
	// Item Status
	OutLines.Add(FText::FromString(FString::Printf(TEXT("내구도: %d / %d"), DurabilityCurrent, DurabilityMax)));
	OutLines.Add(FText::FromString(FString::Printf(TEXT("경도: %d"), Hardness)));
	OutLines.Add(FText::FromString(FString::Printf(TEXT("찌르기 공격력: %d"), StabAttack)));
	OutLines.Add(FText::FromString(FString::Printf(TEXT("베기 공격력: %d"), HackAttack)));
	OutLines.Add(FText::FromString(FString::Printf(TEXT("물리 방어력: %d"), PhysicalDefense)));
	OutLines.Add(FText::FromString(FString::Printf(TEXT("마법 공격력: %d"), MagicAttack)));
	OutLines.Add(FText::FromString(FString::Printf(TEXT("명중률: %d"), Accuracy)));
	OutLines.Add(FText::FromString(FString::Printf(TEXT("회피율: %d"), Evasion)));
	OutLines.Add(FText::FromString(FString::Printf(TEXT("민첩성: %d"), Agility)));
	OutLines.Add(FText::FromString(FString::Printf(TEXT("크리티컬: %d"), Critical)));
	OutLines.Add(FText::FromString(FString::Printf(TEXT("무기 딜레이: %d%%"), WeaponDelayPercent)));
	OutLines.Add(FText::FromString(FString::Printf(TEXT("무게: %d"), Weight)));
	OutLines.Add(FText::FromString(FString::Printf(TEXT("합성 횟수: %d"), SynthesisCount)));
	
	// Item Requirements
	FString CharLine = TEXT("착용 가능 캐릭터: ");
	for (int32 i = 0; i < AllowedCharacters.Num(); ++i)
	{
		const UEnum* Enum = StaticEnum<EPlayableCharacter>();
		CharLine += Enum->GetDisplayNameTextByValue((int64)AllowedCharacters[i]).ToString();
		if (i != AllowedCharacters.Num() - 1)
		{
			CharLine += TEXT(", ");
		}
	}
	OutLines.Add(FText::FromString(CharLine));
	OutLines.Add(FText::FromString(FString::Printf(TEXT("레벨 제한: %d"), RequiredLevel)));
	OutLines.Add(FText::FromString(FString::Printf(TEXT("Stab 제한: %d"), RequiredStab)));
	OutLines.Add(FText::FromString(FString::Printf(TEXT("Hack 제한: %d"), RequiredHack)));
	
	// Item Description(if any)
	if (!Description.IsEmpty())
	{
		OutLines.Add(Description);
	}
}
