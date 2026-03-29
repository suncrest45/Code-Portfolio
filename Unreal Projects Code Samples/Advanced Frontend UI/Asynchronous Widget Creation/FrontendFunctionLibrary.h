// ------------------------------------------------------------------------------
// File Name: FrontendFunctionLibrary.h
// Author: Aidan Straker(aidan.straker@gmail.com)
// Project: SSFrontend
// Date Created: 2026-03-13
// Date Last Modified: 2026-03-22
// Copyright © 2026 Aidan Straker
// ------------------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FrontendFunctionLibrary.generated.h"

struct FSettingInfo;
class UWidget_ActivatableWidgetBase;

// A function library to help us get our widget references
UCLASS()
class SSFRONTEND_API UFrontendFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	
	/// @brief Get a widget reference from our developer settings using a tag
	/// @param InWidgetTag The tag letting us know which widget to get
	/// @return A soft class pointer to the desired widget
	UFUNCTION(BlueprintPure, Category = "Frontend Function Library")
	static TSoftClassPtr<UWidget_ActivatableWidgetBase> GetFrontendSoftWidgetClassByTag(UPARAM(meta = (Categories = "Frontend.Widget")) FGameplayTag InWidgetTag);

	/// @brief Get a pointer to the specified image
	/// @param InImageTag the tag letting us know which image to get
	/// @return A soft class pointer to the desired image
	UFUNCTION(BlueprintPure, Category = "Frontend Function Library")
	static TSoftObjectPtr<UTexture2D> GetOptionsSoftImageByTag(UPARAM(meta = (Categories = "Frontend.Image")) FGameplayTag InImageTag);
};
