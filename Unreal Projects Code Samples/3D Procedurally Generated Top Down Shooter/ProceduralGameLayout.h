// ------------------------------------------------------------------------------
// File Name: ProceduralGameLayout.h
// Author: Aidan Straker(aidan.straker@gmail.com)
// Project: TDS_Procedural
// Date Created: 2025-10-21
// Date Last Modified: 2026-02-25
// Copyright © 2025 Aidan Straker
// ------------------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "GameplayTagContainer.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "ProceduralGameLayout.generated.h"

/**
 * This UI widget is the root widget we are using in our game.
 * It contains a list of UI layers (containers) where we can push our "activatable" widgets onto.
 * At any given time only one widget per layer is being displayed.
 */
UCLASS(Abstract, Blueprintable, meta = (DisableNativeTick))
class TDS_PROCEDURAL_API UProceduralGameLayout : public UCommonUserWidget
{
	GENERATED_BODY()


public:
	
	/// @brief Create, initialise, and push an activatable widget onto the specified widget container
	/// @tparam ActivatableWidgetT A template for any class that is or derives from a UCommonActivatableWidget
	/// @param LayerName The tag that tells us which layer this widget should be pushed onto
	/// @param ActivatableWidgetClass The actual class of the UCommonActivatableWidget
	/// @return A pointer to the newly created widget
	template< typename ActivatableWidgetT = UCommonActivatableWidget>
	ActivatableWidgetT* PushWidgetToLayerStack(FGameplayTag LayerName, UClass* ActivatableWidgetClass)
	{
		return PushWidgetToLayerStack<ActivatableWidgetT>(LayerName, ActivatableWidgetClass, [](ActivatableWidgetT&){});
	}
	
	/// @brief Create, initialise, and push an activatable widget onto the specified widget container
	/// @tparam ActivatableWidgetT A template for any class that derives or is a UCommonActivatableWidget
	/// @param LayerName The tag that tells us which layer this widget should be pushed onto
	/// @param ActivatableWidgetClass The actual class of the UCommonActivatableWidget
	/// @param InitInstanceFunc A function reference to the constructor of the class
	/// @return A pointer to the newly created widget
	template< typename ActivatableWidgetT = UCommonActivatableWidget>
	ActivatableWidgetT* PushWidgetToLayerStack(FGameplayTag LayerName, UClass* ActivatableWidgetClass, TFunctionRef<void(ActivatableWidgetT&)> InitInstanceFunc)
	{
		// Ensures that only CommonActivatableWidgets can be used with this function
		static_assert(TIsDerivedFrom<ActivatableWidgetT, UCommonActivatableWidget>::IsDerived, "Only UCommonActivatableWidget can be used here");

		ensureMsgf(IsValid(ActivatableWidgetClass) && !ActivatableWidgetClass->HasAnyClassFlags(CLASS_Abstract),
			TEXT("UProceduralGameLayout::PushWidgetToLayerStack, Invalid Widget Class, check calls to function"));

		if (UCommonActivatableWidgetContainerBase* Container = GetLayerWidget(LayerName))
		{
			return Container->AddWidget<ActivatableWidgetT>(ActivatableWidgetClass, InitInstanceFunc);
		}

		return nullptr;
	}
	
	/// @brief Find a specified widget container
	/// @param LayerName The name of the widget container we want to find
	/// @return A pointer to the widget container, if it exists
	TObjectPtr<UCommonActivatableWidgetContainerBase> GetLayerWidget(const FGameplayTag LayerName) const;
	
	/// @brief Search the registered containers (layers) and remove the specified widget if it exists
	/// @param ActiveWidget The widget we want to remove
	/// @return Whether the widget was found and removed
	bool FindAndRemoveWidgetFromLayer(UCommonActivatableWidget* ActiveWidget);
	
protected:
	
	/// @brief Registers new widget containers 
	/// @param LayerTag An identifier for the layer we are creating
	/// @param LayerWidget The actual container that future activatable widgets will be pushed onto
	UFUNCTION(BlueprintCallable, Category = "Layer")
	void RegisterLayer(UPARAM(meta = (Categories = "UI.Layer")) FGameplayTag LayerTag, UCommonActivatableWidgetContainerBase* LayerWidget);

private:

	// The registered layers for the primary layout
	UPROPERTY(Transient, meta = (Categories = "UI.Layer"))
	TMap<FGameplayTag, UCommonActivatableWidgetContainerBase*> Layers;
	
};