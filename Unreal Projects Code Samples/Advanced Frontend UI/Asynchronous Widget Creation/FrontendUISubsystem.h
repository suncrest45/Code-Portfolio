// ------------------------------------------------------------------------------
// File Name: FrontendUISubsystem.h
// Author: Aidan Straker(aidan.straker@gmail.com)
// Project: SSFrontend
// Date Created: 2026-01-25
// Date Last Modified: 2026-03-22
// Copyright © 2026 Aidan Straker
// ------------------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "FrontendTypes/FrontendEnumTypes.h"
#include "FrontendUISubsystem.generated.h"


class UWidget_PrimaryLayout;
class UWidget_ActivatableWidgetBase;
class UFrontendButtonBase;
struct FGameplayTag;

// An enum allowing us to choose witch event to broadcast when creating a widget
enum class EAsyncPushWidgetState : uint8
{
	OnCreatedBeforePush = 0,
	AfterPush
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnButtonDescriptionTextChanged, UFrontendButtonBase*, BroadcastingButton, FText, Text);

// This projects UI subsystem
UCLASS()
class SSFRONTEND_API UFrontendUISubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	
	/// @brief Get a pointer to the UI subsystem
	/// @param WorldContextObject - A pointer to the world context
	/// @return A pointer to the UI subsystem
	static UFrontendUISubsystem* Get(const UObject* WorldContextObject);
	
	//~ Begin USubsystem Interface
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	//~ End USubsystem Interface

	/// @brief Registers the primary widget layout we create with the UI subsystem
	/// @param InCreatedWidget A pointer to the primary widget layout
	UFUNCTION(BlueprintCallable)
	void RegisterCreatedPrimaryLayoutWidget(UWidget_PrimaryLayout* InCreatedWidget);

	/// @brief Native implementation for asynchronously pushing a widget to the player screen
	/// @param InWidgetStackTag The tag that indicates what category of widget this is, thus which stack it should be in
	/// @param InSoftWidgetClass A soft class pointer of the widget we are pushing to the stack
	/// @param AsyncPushStateCallback A lambda that allows us to inject extra initialisation code before widget is added to a stack
	void PushSoftWidgetToStackAsync(const FGameplayTag& InWidgetStackTag, TSoftClassPtr<UWidget_ActivatableWidgetBase> InSoftWidgetClass,
		TFunction<void(EAsyncPushWidgetState, UWidget_ActivatableWidgetBase*)> AsyncPushStateCallback
	);

	/// @brief Native implementation for asynchronously pushing a confirmation widget to the player screen
	/// @param InScreenType The type of confirmation screen we want to make (how many buttons it will have)
	/// @param InScreenTitle The title of the confirmation screen
	/// @param InScreenMessage The message to be displayed in the body of the confirmation screen
	/// @param ButtonClickedCallback A lambda that lets us know which button type was pressed (yes, no, etc)
	void PushConfirmScreenToModalStackAsync(EConfirmScreenType InScreenType, const FText& InScreenTitle, const FText& InScreenMessage, TFunction<void(EConfirmScreenButtonType)> ButtonClickedCallback);
	
	// A delegate that is broadcasted when the button description changes
	UPROPERTY(BlueprintAssignable)
	FOnButtonDescriptionTextChanged OnButtonDescriptionTextChanged;
	
private:
	
	UPROPERTY(Transient)
	UWidget_PrimaryLayout* CreatedPrimaryLayout;
};
