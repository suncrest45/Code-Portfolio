// ------------------------------------------------------------------------------
// File Name: ListDataObject_String.cpp
// Author: Aidan Straker(aidan.straker@gmail.com)
// Project: SSFrontend
// Date Created: 2026-02-28
// Date Last Modified: 2026-03-29
// Copyright © 2026 Aidan Straker
// ------------------------------------------------------------------------------

#include "Widgets/Options/DataObjects/ListDataObject_String.h"
#include "FrontendDebugHelper.h"
#include "Widgets/Options/OptionsDataInteractionHelper.h"


void UListDataObject_String::OnDataObjectInitialised()
{
	// Use the first option of a setting to initialise it
	if (!AvailableOptionsStringArray.IsEmpty())
	{
		CurrentStringValue = AvailableOptionsStringArray[0];
	}
	
	// If there is a default value for this setting use that instead
	if (HasDefaultValue())
	{
		CurrentStringValue = GetDefaultValueAsString();
	}
	
	// Finally, read in the value of the setting from the settings config file
	if (DataDynamicGetter)
	{
		if (!DataDynamicGetter->GetValueAsString().IsEmpty())
		{
			CurrentStringValue = DataDynamicGetter->GetValueAsString();
		}
	}
	
	// If there is no display text for the current option, display this
	if (!TrySetDisplayTextFromStringValue(CurrentStringValue))
	{
		CurrentDisplayText = FText::FromString(TEXT("Invalid Option"));
	}
}

void UListDataObject_String::AddDynamicOptions(const FString& InStringValue, const FText& InTextValue)
{
	AvailableOptionsStringArray.Add(InStringValue);
	AvailableOptionsTextArray.Add(InTextValue);
}

void UListDataObject_String::AdvanceToNextOption()
{
	// If there are no valid options, do nothing
	if (AvailableOptionsStringArray.IsEmpty() || AvailableOptionsTextArray.IsEmpty()) return;
	
	// Get the index of the current string value
	const int32 CurrentDisplayIndex = AvailableOptionsStringArray.IndexOfByKey(CurrentStringValue);
	const int32 NextDisplayIndex = CurrentDisplayIndex + 1;
	
	// Check if the next index is valid
	if (const bool bIsNextIndexValid = AvailableOptionsStringArray.IsValidIndex(NextDisplayIndex))
	{
		CurrentStringValue = AvailableOptionsStringArray[NextDisplayIndex];
	}
	// Perform wrapping
	else
	{
		CurrentStringValue = AvailableOptionsStringArray[0];
	}
	// Update the display text
	TrySetDisplayTextFromStringValue(CurrentStringValue);
	
	// Update the value in the game user settings
	if (DataDynamicSetter)
	{
		DataDynamicSetter->SetValueFromString(CurrentStringValue);
		
		// A little test to see if the dynamic getters and setters are working as intended
		Debug::Print(TEXT("DataDynamicSetter is used. Latest value from Getter: ") + DataDynamicGetter->GetValueAsString());
		
		// Notify that the value has changed
		NotifyListDataModified(this);
	}
}

void UListDataObject_String::RetreatToPreviousOption()
{
	// If there are no valid options, do nothing
	if (AvailableOptionsStringArray.IsEmpty() || AvailableOptionsTextArray.IsEmpty()) return;
	
	// Get the index of the current string value
	const int32 CurrentDisplayIndex = AvailableOptionsStringArray.IndexOfByKey(CurrentStringValue);
	const int32 PreviousDisplayIndex = CurrentDisplayIndex - 1;
	
	// Check if the next index is valid
	if (const bool bIsNextIndexValid = AvailableOptionsStringArray.IsValidIndex(PreviousDisplayIndex))
	{
		CurrentStringValue = AvailableOptionsStringArray[PreviousDisplayIndex];
	}
	// Perform wrapping
	else
	{
		CurrentStringValue = AvailableOptionsStringArray.Last();
	}
	
	// Update the display text
	TrySetDisplayTextFromStringValue(CurrentStringValue);
	
	// Update the value in game user settings
	if (DataDynamicSetter)
	{
		DataDynamicSetter->SetValueFromString(CurrentStringValue);
		
		// A little test to see if the dynamic getters and setters are working as intended
		Debug::Print(TEXT("DataDynamicSetter is used. Latest value from Getter: ") + DataDynamicGetter->GetValueAsString());
		
		NotifyListDataModified(this);
	}
}

void UListDataObject_String::OnRotatorInitiatedValueChange(const FText& InNewSelectedText)
{
	// Check if the rotator's new selected text is a valid option
	const int32 FoundIndex = AvailableOptionsTextArray.IndexOfByPredicate(
		[InNewSelectedText](const FText& AvailableText)->bool
		{
			return AvailableText.EqualTo(InNewSelectedText);
		}
	);
	
	// So if the index is valid and is a valid index in the options array
	if (FoundIndex != INDEX_NONE && AvailableOptionsStringArray.IsValidIndex(FoundIndex))
	{
		// Update the setting with the new value
		CurrentDisplayText = InNewSelectedText;
		CurrentStringValue = AvailableOptionsStringArray[FoundIndex];
		if (DataDynamicSetter)
		{
			DataDynamicSetter->SetValueFromString(CurrentStringValue);
			NotifyListDataModified(this);
		}
	}
}

bool UListDataObject_String::CanResetToDefault() const
{
	return HasDefaultValue() && CurrentStringValue != GetDefaultValueAsString();
}

bool UListDataObject_String::TryResetToDefault()
{
	if (CanResetToDefault())
	{
		CurrentStringValue = GetDefaultValueAsString();
		// Update the display text
		TrySetDisplayTextFromStringValue(CurrentStringValue);
		
		// Update the actual setting
		if (DataDynamicSetter)
		{
			DataDynamicSetter->SetValueFromString(CurrentStringValue);
			
			// Send our notification that we have modified this value
			NotifyListDataModified(this, EOptionsListDataModifyReason::ResetToDefault);
			
			return true;
		}
	}
	return false;
}

bool UListDataObject_String::CanSetToForcedStringValue(const FString& InForcedValue) const
{
	return CurrentStringValue != InForcedValue;
}

void UListDataObject_String::OnSetToForcedStringValue(const FString& InForcedValue)
{
	// Update the current value and the display text
	CurrentStringValue = InForcedValue;
	TrySetDisplayTextFromStringValue(CurrentStringValue);
	
	// Write the new value to the config file and notify everyone else
	if (DataDynamicSetter)
	{
		DataDynamicSetter->SetValueFromString(CurrentStringValue);
		NotifyListDataModified(this, EOptionsListDataModifyReason::DependencyModified);
	}
}

bool UListDataObject_String::TrySetDisplayTextFromStringValue(const FString& InStringValue)
{
	// Get the index of the current value
	const int32 CurrentFoundIndex = AvailableOptionsStringArray.IndexOfByKey(InStringValue);
	
	// Check if it is a valid index of the display text array
	if (AvailableOptionsTextArray.IsValidIndex(CurrentFoundIndex))
	{
		// Update current display text
		CurrentDisplayText = AvailableOptionsTextArray[CurrentFoundIndex];
		return true;
	}
	
	return false;
}

// *********************************** UListDataObject_StringBool ****************************************************//

void UListDataObject_StringBool::OverrideTrueDisplayText(const FText& InNewTrueDisplayText)
{
	// Override the "true" option display text
	if (!AvailableOptionsStringArray.Contains(TrueString))
	{
		AddDynamicOptions(TrueString, InNewTrueDisplayText);
	}
}

void UListDataObject_StringBool::OverrideFalseDisplayText(const FText& InNewFalseDisplayText)
{
	// Override the "false" option display text
	if (!AvailableOptionsStringArray.Contains(FalseString))
	{
		AddDynamicOptions(FalseString, InNewFalseDisplayText);
	}
}

void UListDataObject_StringBool::SetTrueAsDefaultValue() { SetDefaultValueFromString(TrueString); }

void UListDataObject_StringBool::SetFalseAsDefaultValue() { SetDefaultValueFromString(FalseString); }

void UListDataObject_StringBool::OnDataObjectInitialised()
{
	TryInitBoolValue();
	Super::OnDataObjectInitialised();
}

void UListDataObject_StringBool::TryInitBoolValue()
{
	// If the array that represents the setting's values does not contain the true string
	if (!AvailableOptionsStringArray.Contains(TrueString))
	{
		// Add in the string with a display text of "ON"
		AddDynamicOptions(TrueString, FText::FromString(TEXT("ON")));
	}
	
	// Do the same for the false option
	if (!AvailableOptionsStringArray.Contains(FalseString))
	{
		AddDynamicOptions(FalseString, FText::FromString(TEXT("OFF")));
	}
}

// *********************************** UListDataObject_StringBool ****************************************************//

// ********************************** UListDataObject_StringInteger **************************************************//

void UListDataObject_StringInteger::AddIntegerOption(int32 InIntegerValue, const FText& InDisplayText)
{
	// Add the new option
	AddDynamicOptions(LexToString(InIntegerValue), InDisplayText);
}

void UListDataObject_StringInteger::OnDataObjectInitialised()
{
	Super::OnDataObjectInitialised();
	// If the integer fails to find a text that corresponds to the current value, it is more than likely -1
	// Which means the setting should display "Custom"
	if (!TrySetDisplayTextFromStringValue(CurrentStringValue))
	{
		CurrentDisplayText = FText::FromString(TEXT("Custom"));
	}
}

void UListDataObject_StringInteger::OnEditDataDependencyModified(UListDataObject_Base* ModifiedDependencyData,
	EOptionsListDataModifyReason ModifyReason)
{
	// When the dependency changes, update the setting
	if (DataDynamicGetter)
	{
		// If the value is already the same, do not run the code below
		// This prevents an infinite loop
		if (CurrentStringValue == DataDynamicGetter->GetValueAsString()) return;
		
		
		CurrentStringValue = DataDynamicGetter->GetValueAsString();
		// handle the setting having a value of -1
		if (!TrySetDisplayTextFromStringValue(CurrentStringValue))
		{
			CurrentDisplayText = FText::FromString(TEXT("Custom"));
		}
		
		NotifyListDataModified(this, EOptionsListDataModifyReason::DependencyModified);
	}
	
	Super::OnEditDataDependencyModified(ModifiedDependencyData, ModifyReason);
}

// ********************************** UListDataObject_StringInteger **************************************************//
