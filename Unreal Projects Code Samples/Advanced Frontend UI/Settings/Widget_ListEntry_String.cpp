// ------------------------------------------------------------------------------
// File Name: Widget_ListEntry_String.cpp
// Author: Aidan Straker(aidan.straker@gmail.com)
// Project: SSFrontend
// Date Created: 2026-03-17
// Date Last Modified: 2026-03-29
// Copyright © 2026 Aidan Straker
// ------------------------------------------------------------------------------

#include "Widgets/Options/ListEntries/Widget_ListEntry_String.h"

#include "Widgets/Components/FrontendButtonBase.h"
#include "Widgets/Components/FrontendCommonRotator.h"
#include "Widgets/Options/DataObjects/ListDataObject_String.h"
#include "FrontendDebugHelper.h"
#include "CommonInputSubsystem.h"
#include "Framework/Application/SlateApplication.h"
#include "Framework/Application/NavigationConfig.h"


void UWidget_ListEntry_String::OnOwningListDataObjectSet(UListDataObject_Base* InOwningListDataObject)
{
	Super::OnOwningListDataObjectSet(InOwningListDataObject);
	
	CachedOwningStringDataObject = CastChecked<UListDataObject_String>(InOwningListDataObject);
	
	// Populates the rotator with the different display options
	FrontendCommonRotator_AvailableOptions->PopulateTextLabels(CachedOwningStringDataObject->GetAvailableOptionsTextArray());
	FrontendCommonRotator_AvailableOptions->SetSelectedOptionByText(CachedOwningStringDataObject->GetCurrentDisplayText());
	
	// When you click on the rotator highlight the entire setting
	FrontendCommonRotator_AvailableOptions->OnClicked().AddLambda([this](){SelectThisEntryWidget();});
	
	// To handle changing the value of a setting with a gamepad. 
	// We will bind a function to when the rotator display text is changed
	FrontendCommonRotator_AvailableOptions->OnRotatedEvent.AddUObject(this, &ThisClass::OnRotatorValueChanged);
}

void UWidget_ListEntry_String::OnOwningListDataObjectModified(UListDataObject_Base* OwningModifiedData,
	EOptionsListDataModifyReason ModifyReason)
{
	// Update the text of the rotator to the current display text
	if (CachedOwningStringDataObject)
	{
		FrontendCommonRotator_AvailableOptions->SetSelectedOptionByText(CachedOwningStringDataObject->GetCurrentDisplayText());
	}
}

void UWidget_ListEntry_String::OnToggleEditableState(bool bIsEditable)
{
	Super::OnToggleEditableState(bIsEditable);
	
	// Set the state of the three component widgets of this list entry
	FrontendButton_PreviousOption->SetIsEnabled(bIsEditable);
	FrontendCommonRotator_AvailableOptions->SetIsEnabled(bIsEditable);
	FrontendButton_NextOption->SetIsEnabled(bIsEditable);
}

UWidget* UWidget_ListEntry_String::OnKeyboardLeftInput(EUINavigation NavContext)
{
	OnPreviousOptionButtonClicked();
	return this;
}

UWidget* UWidget_ListEntry_String::OnKeyboardRightInput(EUINavigation NavContext)
{
	OnNextOptionButtonClicked();
	return this;
}

void UWidget_ListEntry_String::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	// On start up bind the OnClicked events of the two buttons to these functions
	FrontendButton_PreviousOption->OnClicked().AddUObject(this, &ThisClass::OnPreviousOptionButtonClicked);
	FrontendButton_NextOption->OnClicked().AddUObject(this, &ThisClass::OnNextOptionButtonClicked);
}

void UWidget_ListEntry_String::NativeConstruct()
{
	Super::NativeConstruct();
	
	TMap<FKey, EUINavigation>& KeysForFocus = FSlateApplication::Get().GetNavigationConfig()->KeyEventRules;
	KeysForFocus.Emplace(EKeys::D, EUINavigation::Right);
	KeysForFocus.Emplace(EKeys::A, EUINavigation::Left);
	KeysForFocus.Emplace(EKeys::Left, EUINavigation::Left);
	KeysForFocus.Emplace(EKeys::Right, EUINavigation::Right);
	
	// Handle pressing left arrow or the button on the keyboard
	LeftNavigationDelegate.BindDynamic(this, &ThisClass::OnKeyboardLeftInput);
	SetNavigationRuleCustom(EUINavigation::Left, LeftNavigationDelegate);
	
	RightNavigationDelegate.BindDynamic(this, &ThisClass::OnKeyboardRightInput);
	SetNavigationRuleCustom(EUINavigation::Right, RightNavigationDelegate);
}

void UWidget_ListEntry_String::OnPreviousOptionButtonClicked()
{
	// If the string data object is valid
	if (CachedOwningStringDataObject)
	{
		// Proceed to the next option
		CachedOwningStringDataObject->RetreatToPreviousOption();
	}
}

void UWidget_ListEntry_String::OnNextOptionButtonClicked()
{
	// If the string data object is valid
	if (CachedOwningStringDataObject)
	{
		// Proceed to the next option
		CachedOwningStringDataObject->AdvanceToNextOption();
	}
}

void UWidget_ListEntry_String::OnRotatorValueChanged(int32 Value, bool bUserInitiated)
{
	if (!CachedOwningStringDataObject) return;
	
	// Get the common input subsystem
	UCommonInputSubsystem* CommonInputSubsystem = GetInputSubsystem();
	// If the subsystem is invalid or the user did not make this change, do nothing
	if (!CommonInputSubsystem || !bUserInitiated) return;
	
	// If the user is using a gamepad
	if (CommonInputSubsystem->GetCurrentInputType() == ECommonInputType::Gamepad)
	{
		// Since we do not know if the user changed to the previous or next option
		// We will use the latest selected text to change the value of the setting
		CachedOwningStringDataObject->OnRotatorInitiatedValueChange(FrontendCommonRotator_AvailableOptions->GetSelectedText());
	}
}
