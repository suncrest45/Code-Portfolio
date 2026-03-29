// ------------------------------------------------------------------------------
// File Name: FrontendUISubsystemSample.cpp
// Author: Aidan Straker(aidan.straker@gmail.com)
// Project: SSFrontend
// Date Created: 2026-02-04
// Date Last Modified: 2026-02-25
// Copyright © 2026 Aidan Straker
//
// Description: A code sample of the widget creation framework I made, includes the C++
//				implementation and the blueprint interface to create widgets
// ------------------------------------------------------------------------------

	/// @brief Native implementation for asynchronously pushing a widget to the player screen
	/// @param InWidgetStackTag The tag that indicates what category of widget this is, thus which stack it should be in
	/// @param InSoftWidgetClass A soft class pointer of the widget we are pushing to the stack
	/// @param AsyncPushStateCallback A lambda that allows us to inject extra initialisation code before widget is added to a stack
	void PushSoftWidgetToStackAsync(const FGameplayTag& InWidgetStackTag, TSoftClassPtr<UWidget_ActivatableWidgetBase> InSoftWidgetClass,
		TFunction<void(EAsyncPushWidgetState, UWidget_ActivatableWidgetBase*)> AsyncPushStateCallback
	);

	
	void UFrontendUISubsystem::PushSoftWidgetToStackAsync(const FGameplayTag& InWidgetStackTag,
		TSoftClassPtr<UWidget_ActivatableWidgetBase> InSoftWidgetClass,
		TFunction<void(EAsyncPushWidgetState, UWidget_ActivatableWidgetBase*)> AsyncPushStateCallback)
	{
		// Make sure the widget pointer is not null
		check(!InSoftWidgetClass.IsNull());
		// Load in the widget 
		 UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(
			InSoftWidgetClass.ToSoftObjectPath(),
			FStreamableDelegate::CreateLambda(
				[InSoftWidgetClass, this, InWidgetStackTag, AsyncPushStateCallback]()
				{
					UClass* LoadedWidgetClass = InSoftWidgetClass.Get();
					check(LoadedWidgetClass && CreatedPrimaryLayout);
					
					// Get a pointer to the appropriate widget stack for this widget
					UCommonActivatableWidgetContainerBase* FoundWidgetStack = CreatedPrimaryLayout->FindWidgetStackByTag(InWidgetStackTag);
					// Actually create and perform custom initialisation on the widget we want to push to the stack
					UWidget_ActivatableWidgetBase* CreatedWidget = FoundWidgetStack->AddWidget<UWidget_ActivatableWidgetBase>(
						LoadedWidgetClass,
						[AsyncPushStateCallback](UWidget_ActivatableWidgetBase& CreatedWidgetInstance)
						{
							// Call the before push event
							AsyncPushStateCallback(EAsyncPushWidgetState::OnCreatedBeforePush, &CreatedWidgetInstance);
						}
					);
					
					// Call the after push event
					AsyncPushStateCallback(EAsyncPushWidgetState::AfterPush, CreatedWidget);
				}
			)
		);
	}
	
	
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
	
	

	/// @brief Native implementation for asynchronously pushing a confirmation widget to the player screen
	/// @param InScreenType The type of confirmation screen we want to make (how many buttons it will have)
	/// @param InScreenTitle The title of the confirmation screen
	/// @param InScreenMessage The message to be displayed in the body of the confirmation screen
	/// @param ButtonClickedCallback A lambda that lets us know which button type was pressed (yes, no, etc)
	void PushConfirmScreenToModalStackAsync(EConfirmScreenType InScreenType, const FText& InScreenTitle, const FText& InScreenMessage, TFunction<void(EConfirmScreenButtonType)> ButtonClickedCallback);
	
	void UFrontendUISubsystem::PushConfirmScreenToModalStackAsync(EConfirmScreenType InScreenType,
		const FText& InScreenTitle, const FText& InScreenMessage,
		TFunction<void(EConfirmScreenButtonType)> ButtonClickedCallback)
	{
		UConfirmScreenInfoObject* CreatedInfoObject = nullptr;
		
		// Handle creating the matching info objects for the different confirmation screens
		switch (InScreenType)
		{
		case EConfirmScreenType::OK:
			CreatedInfoObject = UConfirmScreenInfoObject::CreateOKScreen(InScreenTitle, InScreenMessage);
			break;
		case EConfirmScreenType::YesNo:
			CreatedInfoObject = UConfirmScreenInfoObject::CreateYesNoScreen(InScreenTitle, InScreenMessage);
			break;
		case EConfirmScreenType::OKCancel:
			CreatedInfoObject = UConfirmScreenInfoObject::CreateOKCancelScreen(InScreenTitle, InScreenMessage);
			break;
		case EConfirmScreenType::YesNoCancel:
			CreatedInfoObject = UConfirmScreenInfoObject::CreateYesNoCancelScreen(InScreenTitle, InScreenMessage);
			break;
		case EConfirmScreenType::Unknown:
			break;
		case EConfirmScreenType::Apply:
			break;
		}
		
		check(CreatedInfoObject);
		
		// After we create the data for the chosen confirmation screen call the native function for creating widgets
		PushSoftWidgetToStackAsync(SSFrontendGameplayTags::Frontend_WidgetStack_Modal,
			UFrontendFunctionLibrary::GetFrontendSoftWidgetClassByTag(SSFrontendGameplayTags::Frontend_Widget_ConfirmScreen),
			[CreatedInfoObject, ButtonClickedCallback](EAsyncPushWidgetState InPushState, UWidget_ActivatableWidgetBase* PushedWidget)
			{
				// Before the confirm screen is pushed to the screen, pass in all the info about the screen
				if (InPushState == EAsyncPushWidgetState::OnCreatedBeforePush)
				{
					UWidget_ConfirmScreen* CreatedConfirmScreen = CastChecked<UWidget_ConfirmScreen>(PushedWidget);
					CreatedConfirmScreen->InitConfirmScreen(CreatedInfoObject, ButtonClickedCallback);
				}
			}
		);
	}
	
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
	
	void UAsyncAction_PushConfirmScreen::Activate()
	{
		// Get the UI subsystem and create the widget
		UFrontendUISubsystem::Get(CachedOwningWorld.Get())->PushConfirmScreenToModalStackAsync(
			CachedScreenType, 
			CachedScreenTitle, 
			CachedScreenMessage, 
			// This lambda broadcasts the event associated with an option on the pop-up screen
			[this](EConfirmScreenButtonType ClickedButtonType)
			{
				OnButtonClicked.Broadcast(ClickedButtonType);
				
				SetReadyToDestroy();
			}
		);
	}