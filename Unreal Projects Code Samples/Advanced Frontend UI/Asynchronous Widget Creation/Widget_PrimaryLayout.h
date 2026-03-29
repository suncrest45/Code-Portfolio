// ------------------------------------------------------------------------------
// File Name: Widget_PrimaryLayout.h
// Author: Aidan Straker(aidan.straker@gmail.com)
// Project: SSFrontend
// Date Created: 2026-01-25
// Date Last Modified: 2026-03-22
// Copyright © 2026 Aidan Straker
// ------------------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "GameplayTagContainer.h"
#include "Widget_PrimaryLayout.generated.h"

class UCommonActivatableWidgetContainerBase;

/// The container for the 4 different widget stacks in the project
UCLASS(Abstract, BlueprintType, meta = (DisableNativeTick))
class SSFRONTEND_API UWidget_PrimaryLayout : public UCommonUserWidget
{
	GENERATED_BODY()
	
public:
	/// @brief FInd the corresponding widget stack in the container
	/// @param InTag The key to use to find the widget stack
	/// @return A pointer to the widget stack
	UCommonActivatableWidgetContainerBase* FindWidgetStackByTag(const FGameplayTag& InTag) const;
	
protected:
	
	/// @brief Add a new widget stack map to the map
	/// @param InStackTag The key for the widget stack map
	/// @param InStack The container we are adding to the map
	UFUNCTION(BlueprintCallable)
	void RegisterWidgetStack(UPARAM(meta = (Categories = "Frontend.WidgetStack")) FGameplayTag InStackTag, UCommonActivatableWidgetContainerBase* InStack);
	
private:
	
	UPROPERTY(Transient)
	TMap<FGameplayTag, UCommonActivatableWidgetContainerBase*> RegisteredWidgetStackMap;
};
