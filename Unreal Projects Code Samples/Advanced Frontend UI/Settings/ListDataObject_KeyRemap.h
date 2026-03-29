// ------------------------------------------------------------------------------
// File Name: ListDataObject_KeyRemap.h
// Author: Aidan Straker(aidan.straker@gmail.com)
// Project: SSFrontend
// Date Created: 2026-03-27
// Date Last Modified: 2026-03-29
// Copyright © 2026 Aidan Straker
// ------------------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Options/DataObjects/ListDataObject_Base.h"
#include "CommonInputTypeEnum.h"
#include "UserSettings/EnhancedInputUserSettings.h"
#include "ListDataObject_KeyRemap.generated.h"

class UEnhancedPlayerMappableKeyProfile;

/**
 * The settings data object for handling remappable input
 */
UCLASS()
class SSFRONTEND_API UListDataObject_KeyRemap : public UListDataObject_Base
{
	GENERATED_BODY()
	
public:
	/// @brief Initialises this remappable data object and caches the necessary variables
	/// @param InOwningUserInputSettings The user input settings for this project
	/// @param InKeyProfile A pointer to the enhanced key profiles in this project
	/// @param InDesiredInputKeyType 
	/// @param InOwningPlayerKeyMapping 
	void InitKeyRemapData(UEnhancedInputUserSettings* InOwningUserInputSettings, UEnhancedPlayerMappableKeyProfile* InKeyProfile, ECommonInputType InDesiredInputKeyType, const FPlayerKeyMapping& InOwningPlayerKeyMapping);

	/// @brief Gets the icon associated with the current key
	/// @return The icon associated with this input
	FSlateBrush GetIconFromCurrentKey() const;

	/// @brief Gets the desired input type of this input setting
	/// @biref More accurately is this a gamepad input or keyboard input
	/// @return The desired input type
	FORCEINLINE ECommonInputType GetDesiredInputType() const {return CachedDesiredInputKeyType;}

	/// @brief Binds a new input to an action
	/// @param InNewKey The key we are going to bind to this action
	void BindNewInputKey(const FKey& InNewKey);
	
	//~ Begin UListDataObject_Base Interface
	virtual bool HasDefaultValue() const override;
	virtual bool CanResetToDefault() const override;
	virtual bool TryResetToDefault() override;
	//~ End UListDataObject_Base Interface
	
private:
	
	/// @brief Gets the key mapping from the input tied to this setting
	/// @return The key mapping of this input
	FPlayerKeyMapping* GetOwningKeyMapping() const;
	
	// Just a pointer to the Input User Settings so we only get it once
	UPROPERTY(Transient)
	UEnhancedInputUserSettings* CachedOwningInputUserSettings;
	// Just a pointer to the Key profile so we only get it once
	UPROPERTY(Transient)
	UEnhancedPlayerMappableKeyProfile* CachedOwningKeyProfile;
	// Is this input setting a keyboard input or gamepad input
	ECommonInputType CachedDesiredInputKeyType;
	// The ID of the input action
	FName CachedOwningMappingName;
	// A representation of the different keys that are mapped to an action
	EPlayerMappableKeySlot CachedOwningMappableKeySlot;
};
