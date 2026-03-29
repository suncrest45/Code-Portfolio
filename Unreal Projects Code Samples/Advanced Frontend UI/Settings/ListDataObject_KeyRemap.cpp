// ------------------------------------------------------------------------------
// File Name: ListDataObject_KeyRemap.cpp
// Author: Aidan Straker(aidan.straker@gmail.com)
// Project: SSFrontend
// Date Created: 2026-03-27
// Date Last Modified: 2026-03-29
// Copyright © 2026 Aidan Straker
// ------------------------------------------------------------------------------

#include "Widgets/Options/DataObjects/ListDataObject_KeyRemap.h"

#include "CommonInputBaseTypes.h"
#include "CommonInputSubsystem.h"
#include "FrontendDebugHelper.h"

void UListDataObject_KeyRemap::InitKeyRemapData(UEnhancedInputUserSettings* InOwningUserInputSettings,
                                                UEnhancedPlayerMappableKeyProfile* InKeyProfile, ECommonInputType InDesiredInputKeyType,
                                                const FPlayerKeyMapping& InOwningPlayerKeyMapping)
{
	CachedOwningInputUserSettings = InOwningUserInputSettings;
	CachedOwningKeyProfile = InKeyProfile;
	CachedDesiredInputKeyType = InDesiredInputKeyType;
	CachedOwningMappingName = InOwningPlayerKeyMapping.GetMappingName();
	CachedOwningMappableKeySlot = InOwningPlayerKeyMapping.GetSlot();
}

FSlateBrush UListDataObject_KeyRemap::GetIconFromCurrentKey() const
{
	// Do nothing if the Input User Settings is invalid
	check(CachedOwningInputUserSettings);
	
	// We need the common input subsystem to know the name of the controller we are using right now
	UCommonInputSubsystem* CommonInputSubsystem = UCommonInputSubsystem::Get(CachedOwningInputUserSettings->GetLocalPlayer());
	check(CommonInputSubsystem);
	
	// The icon associated with this input will be stored in this variable
	FSlateBrush FoundBrush;
	// Attempt to get the icon
	const bool bIsFoundBrush = UCommonInputPlatformSettings::Get()->TryGetInputBrush(FoundBrush, GetOwningKeyMapping()->GetCurrentKey(), CachedDesiredInputKeyType, CommonInputSubsystem->GetCurrentGamepadName());
	
	// if no icon was found, display a message
	if (!bIsFoundBrush)
	{
		Debug::Print(TEXT("Unable to find icon for the key ") + GetOwningKeyMapping()->GetCurrentKey().GetDisplayName().ToString() +
			TEXT(". Empty brush was applied"));
	}
	
	return FoundBrush;
}

void UListDataObject_KeyRemap::BindNewInputKey(const FKey& InNewKey)
{
	check(CachedOwningInputUserSettings);
	
	// Set up the arguments for mapping a new key
	FMapPlayerKeyArgs KeyArgs;
	KeyArgs.MappingName = CachedOwningMappingName;
	KeyArgs.Slot = CachedOwningMappableKeySlot;
	KeyArgs.NewKey = InNewKey;
	// An empty container that contains why the mapping failed
	FGameplayTagContainer Container;
	
	CachedOwningInputUserSettings->MapPlayerKey(KeyArgs, Container);
	CachedOwningInputUserSettings->SaveSettings();
	NotifyListDataModified(this);
}

bool UListDataObject_KeyRemap::HasDefaultValue() const
{
	return GetOwningKeyMapping()->GetDefaultKey().IsValid();
}

bool UListDataObject_KeyRemap::CanResetToDefault() const
{
	return HasDefaultValue() && GetOwningKeyMapping()->IsCustomized();
}

bool UListDataObject_KeyRemap::TryResetToDefault()
{
	if (CanResetToDefault())
	{
		check(CachedOwningInputUserSettings);
		GetOwningKeyMapping()->ResetToDefault();
		CachedOwningInputUserSettings->SaveSettings();
		NotifyListDataModified(this, EOptionsListDataModifyReason::ResetToDefault);
		
		return true;
	}
	
	return false;
}

FPlayerKeyMapping* UListDataObject_KeyRemap::GetOwningKeyMapping() const
{
	// If the key profile is invalid, crash
	check(CachedOwningKeyProfile);
	// This helps us find the exact mapping we want
	FMapPlayerKeyArgs KeyArgs;
	KeyArgs.MappingName = CachedOwningMappingName;
	KeyArgs.Slot = CachedOwningMappableKeySlot;
	
	return CachedOwningKeyProfile->FindKeyMapping(KeyArgs);
}
