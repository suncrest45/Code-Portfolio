// ------------------------------------------------------------------------------
// File Name: FrontendGameUserSettings.h
// Author: Aidan Straker(aidan.straker@gmail.com)
// Project: SSFrontend
// Date Created: 2026-03-13
// Date Last Modified: 2026-03-29
// Copyright © 2026 Aidan Straker
// ------------------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "Engine/DataTable.h"
#include "FrontendGameUserSettings.generated.h"

/**
 *  The user settings for this project
 */
UCLASS()
class SSFRONTEND_API UFrontendGameUserSettings : public UGameUserSettings
{
	GENERATED_BODY()
	
public:
	
	/// @brief Constructor
	UFrontendGameUserSettings();
	
	/// @brief Get the user settings for this project
	/// @return A pointer to the data object that contains this project's settings
	static UFrontendGameUserSettings* Get();
	
	// ***** Gameplay Collection Tab ***** //
	UFUNCTION()
	FString GetCurrentGameDifficulty() const { return CurrentGameDifficulty;}
	UFUNCTION()
	void SetCurrentGameDifficulty(const FString& InNewDifficulty) { CurrentGameDifficulty = InNewDifficulty; }
	// ***** Gameplay Collection Tab ***** //
	
	// ***** Audio Collection Tab ***** //
	UFUNCTION()
	float GetOverallVolume() const { return OverallVolume; }
	UFUNCTION()
	void SetOverallVolume(float InNewVolume);
	UFUNCTION()
	float GetMusicVolume() const { return MusicVolume; }
	UFUNCTION()
	void SetMusicVolume(float InNewVolume);
	UFUNCTION()
	float GetSFXVolume() const { return SFXVolume; }
	UFUNCTION()
	void SetSFXVolume(float InNewVolume);
	UFUNCTION()
	float GetVoiceVolume() const { return VoiceVolume; }
	UFUNCTION()
	void SetVoiceVolume(float InNewVolume);
	UFUNCTION()
	bool GetAllowBackGroundAudio() const { return bAllowBackgroundAudio; }
	UFUNCTION()
	void SetAllowBackGroundAudio(bool IsAllowed);
	UFUNCTION()
	bool GetUseHDRAudioMode() const { return bUseHDRAudioMode; }
	UFUNCTION()
	void SetUseHDRAudioMode(bool IsEnabled);
	// ***** Audio Collection Tab ***** //
	
	// ***** Video Collection Tab ***** //
	UFUNCTION()
	float GetCurrentDisplayGamma() const;
	UFUNCTION()
	void SetCurrentDisplayGamma(float InNewDisplayGamma);
	// ***** Video Collection Tab ***** //
	
	
	
private:
	
	// ***** Gameplay Collection Tab ***** //
	// The user selected game difficulty
	UPROPERTY(Config)
	FString CurrentGameDifficulty;
	// ***** Gameplay Collection Tab ***** //
	
	// ***** Audio Collection Tab ***** //
	UPROPERTY(Config)
	float OverallVolume;
	UPROPERTY(Config)
	float MusicVolume;
	UPROPERTY(Config)
	float SFXVolume;
	UPROPERTY(Config)
	float VoiceVolume;
	UPROPERTY(Config)
	bool bAllowBackgroundAudio;
	UPROPERTY(Config)
	bool bUseHDRAudioMode;
	// ***** Audio Collection Tab ***** //
};
