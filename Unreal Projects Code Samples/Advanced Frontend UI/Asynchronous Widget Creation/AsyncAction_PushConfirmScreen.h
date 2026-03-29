// ------------------------------------------------------------------------------
// File Name: AsyncAction_PushConfirmScreen.h
// Author: Aidan Straker(aidan.straker@gmail.com)
// Project: SSFrontend
// Date Created: 2026-02-04
// Date Last Modified: 2026-03-22
// Copyright © 2026 Aidan Straker
// ------------------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "FrontendTypes/FrontendEnumTypes.h"
#include "AsyncAction_PushConfirmScreen.generated.h"

class UWidget_ActivatableWidgetBase;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnConfirmScreenButtonClicked, EConfirmScreenButtonType, ClickedButtonType);

// Blueprint implementation for modal widgets to the screen 
UCLASS()
class SSFRONTEND_API UAsyncAction_PushConfirmScreen : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
	
public:
	
	/// @brief Blueprint interface for pushing confirmation screens to the screen
	/// @param WorldContextObject 
	/// @param ScreenType The type of confirmation screen we are making
	/// @param InScreenTitle The title of the confirmation screen
	/// @param InScreenMessage The message of the body
	/// @return A pointer to the action
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject", HidePin = "WorldContextObject", BlueprintInternalUseOnly = "true", DisplayName = "Show Confirmation Screen"))
	static UAsyncAction_PushConfirmScreen* PushConfirmScreen(
		const UObject* WorldContextObject, 
		EConfirmScreenType ScreenType,
		FText InScreenTitle,
		FText InScreenMessage
	);
	
	//~ Begin UBlueprintAsyncActionBase Interface
	virtual void Activate() override;
	//~ End UBlueprintAsyncActionBase Interface
	
	// An event for letting us know which button was clicked on the screen
	UPROPERTY(BlueprintAssignable)
	FOnConfirmScreenButtonClicked  OnButtonClicked;
	
private:
	
	TWeakObjectPtr<UWorld> CachedOwningWorld;
	EConfirmScreenType CachedScreenType;
	FText CachedScreenTitle;
	FText CachedScreenMessage;
};
