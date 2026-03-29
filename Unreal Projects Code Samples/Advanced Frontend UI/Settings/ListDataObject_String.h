// ------------------------------------------------------------------------------
// File Name: ListDataObject_String.h
// Author: Aidan Straker(aidan.straker@gmail.com)
// Project: SSFrontend
// Date Created: 2026-02-28
// Date Last Modified: 2026-03-29
// Copyright © 2026 Aidan Straker
// ------------------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Options/DataObjects/ListDataObject_Value.h"
#include "ListDataObject_String.generated.h"

/**
 * This class holds data for settings that are represented by a string with not set amount of options (for example difficulty)
 */
UCLASS()
class SSFRONTEND_API UListDataObject_String : public UListDataObject_Value
{
	GENERATED_BODY()
	
public:
	
	/// @brief Adds a new option to the string data object
	/// @param InStringValue The new string value. This gets saved to a config file
	/// @param InTextValue The new text display. This gets displayed in the UI
	void AddDynamicOptions(const FString& InStringValue, const FText& InTextValue);
	
	/// @brief Select the next setting value in the data object
	void AdvanceToNextOption();
	/// @brief Select the previous setting value in the data object
	void RetreatToPreviousOption();

	/// @brief When the rotator changes its value, change the current value of the setting
	/// @param InNewSelectedText The new selected text of the rotator
	void OnRotatorInitiatedValueChange(const FText& InNewSelectedText);
	
protected:
	
	//~ Begin ListDataObject_Base Interface
	virtual void OnDataObjectInitialised() override;
	virtual bool CanResetToDefault() const override;
	virtual bool TryResetToDefault() override;
	virtual bool CanSetToForcedStringValue(const FString& InForcedValue) const override;
	virtual void OnSetToForcedStringValue(const FString& InForcedValue) override;
	//~ End ListDataObject_Base Interface

	/// @brief Sets the display text for the setting based on the current value
	/// @param InStringValue The string value used to set the display text
	/// @return Was the display successfully set
	bool TrySetDisplayTextFromStringValue(const FString& InStringValue);
	
	
	// The current string setting value
	FString CurrentStringValue;
	// The current text representation of the value that is being displayed
	FText CurrentDisplayText;
	
	// Actual string values that get saved to a config folder
	TArray<FString> AvailableOptionsStringArray;
	// The display text for each string value that will be used in the rotator
	TArray<FText> AvailableOptionsTextArray;
	
public:
	/// @brief Gets the array containing the display text for the different setting's options
	/// @return The array containing the display text for the different option
	FORCEINLINE const TArray<FText>& GetAvailableOptionsTextArray() const {return AvailableOptionsTextArray;}
	/// @brief Get the current display text
	/// @return The current display text
	FORCEINLINE FText GetCurrentDisplayText() const {return CurrentDisplayText;}
};

/**
 * This class holds data for settings that are represented by a boolean (for example v-sync)
 */
UCLASS()
class SSFRONTEND_API  UListDataObject_StringBool : public UListDataObject_String
{
	GENERATED_BODY()
	
public:
	
	/// @brief Set a new display text for the "true" option of the setting
	/// @param InNewTrueDisplayText The new display text for the "true" option
	void OverrideTrueDisplayText(const FText& InNewTrueDisplayText);

	/// @brief Set a new display text for the "false" option of the setting
	/// @param InNewFalseDisplayText Thew new display text for the "false" option
	void OverrideFalseDisplayText(const FText& InNewFalseDisplayText);
	
	/// @brief Set the "true" option as the default value of this setting
	void SetTrueAsDefaultValue();
	/// @brief Set the "false" option as the default value of this setting
	void SetFalseAsDefaultValue();
	
protected:
	
	//~ Begin UListDataObject_String Interface
	virtual void OnDataObjectInitialised() override;
	//~ End UListDataObject_String Interface
	
private:
	
	/// @brief Initialise the data object
	void TryInitBoolValue();
	
	// This type of setting only has two options true/false
	const FString TrueString = TEXT("true");
	const FString FalseString = TEXT("false");
};

/**
 * This class holds data for settings that are represented by an enum (for example window mode)
 */
UCLASS()
class SSFRONTEND_API  UListDataObject_StringEnum : public UListDataObject_String
{
	GENERATED_BODY()
	
public:
	/// @brief For settings who data is an enum, we need to convert the enum into a text
	/// And then add it to our display options for the setting
	/// @tparam EnumType A template that represents an enum
	/// @param InEnumOption The enum value we are using
	/// @param InDisplayText The text that represents this enum
	template <typename EnumType>
	void AddEnumOption(EnumType InEnumOption, const FText& InDisplayText);

	/// @brief Converts the setting's current value from a string to an enum
	/// @tparam EnumType A template that represents our enum
	/// @return The current setting value as an enum
	template <typename EnumType>
	EnumType GetCurrentValueAsEnum() const;

	/// @brief Set the default value of a setting using an enum
	/// @tparam EnumType A template that represents an enum
	/// @param InEnumOption The default setting value represented as enum
	template <typename EnumType>
	void SetDefaultValueFromEnumOption(EnumType InEnumOption);
};

template <typename EnumType>
void UListDataObject_StringEnum::AddEnumOption(EnumType InEnumOption, const FText& InDisplayText)
{
	// Reflect the enum from the template
	const UEnum* StaticEnumOption = StaticEnum<EnumType>();
	// Convert the enum to string 
	const FString ConvertedEnumString = StaticEnumOption->GetNameStringByValue(InEnumOption);
	// Add the string as an option for the setting
	AddDynamicOptions(ConvertedEnumString, InDisplayText);
}

template <typename EnumType>
EnumType UListDataObject_StringEnum::GetCurrentValueAsEnum() const
{
	// Reflect the enum from the template
	const UEnum* StaticEnumOption = StaticEnum<EnumType>();
	return (EnumType)StaticEnumOption->GetValueByNameString(CurrentStringValue);
}

template <typename EnumType>
void UListDataObject_StringEnum::SetDefaultValueFromEnumOption(EnumType InEnumOption)
{
	// Reflect the enum from the template
	const UEnum* StaticEnumOption = StaticEnum<EnumType>();
	// Convert the enum to string 
	const FString ConvertedEnumString = StaticEnumOption->GetNameStringByValue(InEnumOption);
	// Set the default value
	SetDefaultValueFromString(ConvertedEnumString);
}

/**
 * This class holds data for settings that are represented by an integer (for example graphics settings)
 */
UCLASS()
class SSFRONTEND_API  UListDataObject_StringInteger : public UListDataObject_String
{
	GENERATED_BODY()
	
public:
	/// @brief Add a new option to the setting
	/// @param InIntegerValue The integer that correlates to the option. Ex. 0 - low, 1 - medium, 2 - high, 3 - epic
	/// @param InDisplayText The text to display for each integer
	void AddIntegerOption(int32 InIntegerValue, const FText& InDisplayText);
	
protected:
	
	//~ Begin UListDataObject_String Interface
	virtual void OnDataObjectInitialised() override;
	//~ End UListDataObject_String Interface
	
	//~ Begin UListDataObject_Base Interface
	virtual void OnEditDataDependencyModified(UListDataObject_Base* ModifiedDependencyData, EOptionsListDataModifyReason ModifyReason) override;
	//~ End UListDataObject_Base Interface
};