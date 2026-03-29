// ------------------------------------------------------------------------------
// File Name: OptionsDataRegistry.h
// Author: Aidan Straker(aidan.straker@gmail.com)
// Project: SSFrontend
// Date Created: 2026-02-24
// Date Last Modified: 2026-03-29
// Copyright © 2026 Aidan Straker
// ------------------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "OptionsDataRegistry.generated.h"

struct FSettingInfo;
class UListDataObject_Collection;
class UListDataObject_Base;


UCLASS()
class SSFRONTEND_API UOptionsDataRegistry : public UObject
{
	GENERATED_BODY()
	
public:
	
	/// @brief Initialises a data registry object after it has been created
	/// @param InLocalOwningPlayer - The local player (client) that owns this data
	void InitOptionsDataRegistry(ULocalPlayer* InLocalOwningPlayer);

	/// @brief Gets the array that holds all the registered tab data
	/// @return The array holding all the registered tab data
	const TArray<UListDataObject_Collection*>& GetRegisteredOptionsTabCollections() const { return RegisteredOptionsTabCollections; }

	/// @brief Gets all the settings in a particular tab
	/// @param InSelectedTabID The tab we want to get settings from
	/// @return All the settings in a particular tab
	TArray<UListDataObject_Base*> GetListSourceItemsByTabID(const FName& InSelectedTabID) const;
	
private:
	
	/// @brief Initialises the Gameplay Tab in the settings menu
	void InitGameplayCollectionTab();
	/// @brief Initialises the Audio Tab in the settings menu
	void InitAudioCollectionTab();
	/// @brief Initialises the Video Tab in the settings menu
	void InitVideoCollectionTab();
	/// @brief Initialises the Control Tab in the settings menu
	void InitControlCollectionTab(ULocalPlayer* InLocalOwningPlayer);
	/// @brief Initialises the Accessibility Tab in the settings menu
	void InitAccessibilityCollectionTab();

	/// @brief Find all the children of a given data object
	/// @param InParentData The data object we are searching 
	/// @param OutFoundChildListData An array containing all the children of this data object
	void FindChildListDataRecursively(UListDataObject_Base* InParentData, TArray<UListDataObject_Base*>& OutFoundChildListData) const;
	
	UPROPERTY(Transient)
	TArray<UListDataObject_Collection*> RegisteredOptionsTabCollections;
};
