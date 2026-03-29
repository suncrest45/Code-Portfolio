// ------------------------------------------------------------------------------
// File Name: AsyncAction_PushSoftWidget.cpp
// Author: Aidan Straker(aidan.straker@gmail.com)
// Project: SSFrontend
// Date Created: 2026-01-26
// Date Last Modified: 2026-03-22
// Copyright © 2026 Aidan Straker
// ------------------------------------------------------------------------------

#include "AsyncActions/AsyncAction_PushSoftWidget.h"
#include "Subsystems/FrontendUISubsystem.h"
#include "Widgets/Widget_ActivatableWidgetBase.h"

UAsyncAction_PushSoftWidget* UAsyncAction_PushSoftWidget::PushSoftWidget(const UObject* WorldContextObject,
	APlayerController* OwningPlayerController, TSoftClassPtr<UWidget_ActivatableWidgetBase> InSoftWidgetClass,
	UPARAM(meta = (Categories = "Frontend.WidgetStack")) FGameplayTag InWidgetStackTag, bool bFocusOnNewlyPushedWidget)
{
	// Crash if the soft pointer is invalid
	checkf(!InSoftWidgetClass.IsNull(), TEXT("PushSoftWidgetToStack was passed a null soft widget class pointer"))
	
	if (GEngine)
	{
		// Attempt to get a pointer to the UWorld
		if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
		{
			// Construct the action
			UAsyncAction_PushSoftWidget* Node = NewObject<UAsyncAction_PushSoftWidget>();
			
			// Cache all the inputs
			Node->CachedOwningWorld = World;
			Node->CachedOwningPC = OwningPlayerController;
			Node->CachedSoftWidgetClass = InSoftWidgetClass;
			Node->CachedWidgetStackTag = InWidgetStackTag;
			Node->bCachedFocusOnNewlyPushedWidget = bFocusOnNewlyPushedWidget;
			
			Node->RegisterWithGameInstance(World);
			return Node;
		}
	}
	
	return nullptr;
}

void UAsyncAction_PushSoftWidget::Activate()
{
	UFrontendUISubsystem* FrontendUISubsystem = UFrontendUISubsystem::Get(CachedOwningWorld.Get());
	
	// Start loading the widget
	FrontendUISubsystem->PushSoftWidgetToStackAsync(CachedWidgetStackTag, CachedSoftWidgetClass, 
		[this](EAsyncPushWidgetState InPushState, UWidget_ActivatableWidgetBase* PushedWidget)
		{
			switch (InPushState) {
			// Fire off an event before the widget is added to the screen, allowing us to handle any special initialisation cases before the widget appears
			case EAsyncPushWidgetState::OnCreatedBeforePush:
				
				PushedWidget->SetOwningPlayer(CachedOwningPC.Get());
				OnWidgetCreatedBeforePush.Broadcast(PushedWidget);
				
				break;
			// Fire off an event letting us know the widget has been added to the screen
			case EAsyncPushWidgetState::AfterPush:
				
				AfterPush.Broadcast(PushedWidget);
				if (bCachedFocusOnNewlyPushedWidget)
				{
					if (UWidget* WidgetToFocus = PushedWidget->GetDesiredFocusTarget())
					{
						WidgetToFocus->SetFocus();
					}
				}
				
				SetReadyToDestroy();
				break;
			default:
				break;
			}
		}
	);
}
