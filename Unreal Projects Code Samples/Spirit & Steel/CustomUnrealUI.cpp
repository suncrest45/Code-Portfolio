// ------------------------------------------------------------------------------
// File Name: CustomUnrealUI.cpp
// Author: Aidan Straker(aidan.straker@gmail.com)
// Project: Spirit & Steel
// Date Created: 2025-01-10
// Date Last Modified: 2026-02-25
// Copyright © 2025 Aidan Straker
//
// Description: Showcases the work I did to make a custom UI framework for Unreal
// specifically disabling focus and handling horizontal navigation
// ------------------------------------------------------------------------------




void USpellcastGameInstance::SetupCustomNavigationKeys()
{
	// Disable all focus navigation.
	// Unfortunately, "A" button could not be disabled, but we can work with that.
	// use static bool condition to make sure it only happens once even during in-engine play
	static bool menuCtrlsAdded = false;

	if (!menuCtrlsAdded)
	{
		menuCtrlsAdded = true;
		TSharedRef<FNavigationConfig> NavConfig = FSlateApplication::Get().GetNavigationConfig();
		NavConfig.Get().KeyEventRules.Empty();
		NavConfig.Get().KeyActionRules.Empty();
		NavConfig.Get().bAnalogNavigation = false;
		NavConfig.Get().bKeyNavigation = false;
		FSlateApplication::Get().SetNavigationConfig(NavConfig);

		// This is a hack to disable Virtual Accept
		// Associated with FCommonAnalogCursor::HandleKeyDownEvent and FAnalogCursor::HandleKeyDownEvent
		FKey& virtual_key = const_cast<FKey&>(EKeys::Virtual_Accept);
		virtual_key = EKeys::Invalid;
		virtual_key = const_cast<FKey&>(EKeys::Virtual_Back);
		virtual_key = EKeys::Invalid;
	}
}

// This function santises controller/keyboard input
void UBaseSettingsWidget::ChangeSetting(float InputValue, bool bIsOverride)
{
	if (bIsOverride == false)
	{
		if (InputValue < 0.0f)
		{
			InputValue = -1.0f;
		}
		else if (InputValue > 0.0f)
		{
			InputValue = 1.0f;
		}
	}
	// Then finds the delegate in a map that changes the settings we are currently hovering on
	SettingsFunctionMap.Find(SettingName.ToString())->Broadcast(InputValue, bIsOverride);
}

// Initialises the map of names to events. These events handle changing the settings value and updating the setting UI element
void UBaseSettingsWidget::InitSettingWidget()
{
	SettingsFunctionMap.Add("UI.Settings.Window.Fullscreen"			      , OnWindowModeChanged			     );
	SettingsFunctionMap.Add("UI.Settings.Window.Resolution"				  , OnResolutionChanged				 );
	SettingsFunctionMap.Add("UI.Settings.Window.VSync"					  , OnVSyncChanged					 );
	SettingsFunctionMap.Add("UI.Settings.Gameplay.CameraSensitivity"		  , OnCameraSensitivityChanged		 );
	SettingsFunctionMap.Add("UI.Settings.Gameplay.Vibration"			      , OnVibrationChanged				 );
	SettingsFunctionMap.Add("UI.Settings.Volume.Master"					  , OnMasterVolumeChanged			 );
	SettingsFunctionMap.Add("UI.Settings.Volume.Music"					  , OnMusicVolumeChanged			 );
	SettingsFunctionMap.Add("UI.Settings.Volume.Dialogue"					  , OnDialogueVolumeChanged			 );
	SettingsFunctionMap.Add("UI.Settings.Volume.SFX"						  , OnSFXVolumeChanged				 );
	SettingsFunctionMap.Add("UI.Settings.Volume.Ambient"			          , OnAmbientVolumeChanged			 );
	SettingsFunctionMap.Add("UI.Settings.Gameplay.GamepadCameraToggle"	  , OnGamepadCameraToggle			 );
	SettingsFunctionMap.Add("UI.Settings.Gameplay.GamepadCameraSensitivity" , OnGamepadCameraSensitivityChanged);
	SettingsFunctionMap.Add("UI.Settings.Graphics.OverallScalability"		  , OnOverallScalabilityChanged		 );
	SettingsFunctionMap.Add("UI.Settings.Graphics.RenderScale"			  , OnRenderScaleChanged			 );
	SettingsFunctionMap.Add("UI.Settings.Graphics.ViewDistance"			  , OnViewDistanceChanged			 );
	SettingsFunctionMap.Add("UI.Settings.Graphics.AAMethod"				  ,	OnAAMethodChanged				 );
	SettingsFunctionMap.Add("UI.Settings.Graphics.AAQuality"				  , OnAAQualityChanged				 );
	SettingsFunctionMap.Add("UI.Settings.Graphics.PostProcessing"			  , OnPostProcessQualityChanged		 );
	SettingsFunctionMap.Add("UI.Settings.Graphics.ShadowQuality"			  , OnShadowQualityChanged			 );
	SettingsFunctionMap.Add("UI.Settings.Graphics.TextureQuality"			  , OnTextureQualityChanged			 );
	SettingsFunctionMap.Add("UI.Settings.Graphics.EffectQuality"			  , OnEffectQualityChanged			 );
}