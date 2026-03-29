// ------------------------------------------------------------------------------
// File Name: Widget_ListEntry_String.h
// Author: Aidan Straker(aidan.straker@gmail.com)
// Project: SSFrontend
// Date Created: 2026-03-07
// Date Last Modified: 2026-03-29
// Copyright © 2026 Aidan Straker
// ------------------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Options/ListEntries/Widget_ListEntry_Base.h"
#include "Widget_ListEntry_String.generated.h"

class UListDataObject_String;
class UFrontendButtonBase;
class UFrontendCommonRotator;

/**
 * A widget that displays text based settings (window mode)
 */
UCLASS(Abstract, BlueprintType, meta = (DisableNativeTick))
class SSFRONTEND_API UWidget_ListEntry_String : public UWidget_ListEntry_Base
{
	GENERATED_BODY()
	
protected:
	
	//~ Begin UWidget_ListEntry_Base Interface
	virtual void OnOwningListDataObjectSet(UListDataObject_Base* InOwningListDataObject) override;
	virtual void OnOwningListDataObjectModified(UListDataObject_Base* OwningModifiedData, EOptionsListDataModifyReason ModifyReason) override;
	virtual void OnToggleEditableState(bool bIsEditable) override;
	virtual UWidget* OnKeyboardLeftInput(EUINavigation NavContext) override;
	virtual UWidget* OnKeyboardRightInput(EUINavigation NavContext) override;
	//~ End UWidget_ListEntry_Base Interface
	
	//~ Begin UUserWidget Interface
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	//~ End UUserWidget Interface
	
private:
	
	/// @brief A function called when the previous button is clicked
	void OnPreviousOptionButtonClicked();
	/// @brief A function called when the next button is clicked
	void OnNextOptionButtonClicked();

	/// @brief Handles changing the setting value with a gamepad
	/// @param Value The value the rotator now displays
	/// @param bUserInitiated Was this change caused by the user
	void OnRotatorValueChanged(int32 Value, bool bUserInitiated);
	
	// ***** Bound Widgets *****
	// The previous button, selects the previous option
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	UFrontendButtonBase* FrontendButton_PreviousOption;
	// The rotator that displays all options (fullscreen, windowed, borderless, etc)
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	UFrontendCommonRotator* FrontendCommonRotator_AvailableOptions;
	// The next button, selects the next option
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	UFrontendButtonBase* FrontendButton_NextOption;
	// ***** Bound Widgets *****
	
	// The data object associated with this widget
	UPROPERTY(Transient)
	UListDataObject_String* CachedOwningStringDataObject;
};
