// ------------------------------------------------------------------------------
// File Name: AsyncAction_PushSoftWidget.h
// Author: Aidan Straker(aidan.straker@gmail.com)
// Project: SSFrontend
// Date Created: 2026-01-26
// Date Last Modified: 2026-03-22
// Copyright © 2026 Aidan Straker
// ------------------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "GameplayTagContainer.h"
#include "AsyncAction_PushSoftWidget.generated.h"

class UWidget_ActivatableWidgetBase;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPushSoftWidgetDelegate, UWidget_ActivatableWidgetBase*, PushedWidget);

// Blueprint implementation for pushing widgets to the screen
UCLASS()
class SSFRONTEND_API UAsyncAction_PushSoftWidget : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
	
public:
	
	/// @brief A blueprint helper function that pushes widgets onto the stack
	/// @param WorldContextObject The game's world context object
	/// @param OwningPlayerController The player controller that will own this widget
	/// @param InSoftWidgetClass The widget we want to push to a stack
	/// @param InWidgetStackTag The stack we want to push the widget onto
	/// @param bFocusOnNewlyPushedWidget When this widget is pushed should we immediately focus on it
	/// @return A pointer to an action that can be used to created widgets
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject", HidePin = "WorldContextObject", BlueprintInternalUseOnly = "true", DisplayName = "Push Soft Widget To Widget Stack"))
	static UAsyncAction_PushSoftWidget* PushSoftWidget(const UObject* WorldContextObject, 
		APlayerController* OwningPlayerController, 
		TSoftClassPtr<UWidget_ActivatableWidgetBase> InSoftWidgetClass, 
		UPARAM(meta = (Categories = "Frontend.WidgetStack")) FGameplayTag InWidgetStackTag, 
		bool bFocusOnNewlyPushedWidget = true);
	
	//~ Begin UBlueprintAsyncActionBase Interface
	virtual void Activate() override;
	//~ End UBlueprintAsyncActionBase Interface
	
	UPROPERTY(BlueprintAssignable)
	FOnPushSoftWidgetDelegate OnWidgetCreatedBeforePush;
	
	UPROPERTY(BlueprintAssignable)
	FOnPushSoftWidgetDelegate AfterPush;
	
private:
	
	// Cache all the inputs
	TWeakObjectPtr<UWorld> CachedOwningWorld;
	TWeakObjectPtr<APlayerController> CachedOwningPC;
	TSoftClassPtr<UWidget_ActivatableWidgetBase> CachedSoftWidgetClass;
	FGameplayTag CachedWidgetStackTag;
	bool bCachedFocusOnNewlyPushedWidget = false;
};
