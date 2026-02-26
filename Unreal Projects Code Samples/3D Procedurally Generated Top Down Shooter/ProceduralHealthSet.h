// ------------------------------------------------------------------------------
// File Name: ProceduralHealthSet.h
// Author: Aidan Straker(aidan.straker@gmail.com)
// Project: TDS_Procedural
// Date Created: 2025-10-27
// Date Last Modified: 2026-02-25
// Copyright © 2025 Aidan Straker
// ------------------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Attributes/ProceduralAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "NativeGameplayTags.h"
#include "ProceduralHealthSet.generated.h"

#define UE_API TDS_PROCEDURAL_API

class UObject;
struct FFrame;

TDS_PROCEDURAL_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_Damage);
TDS_PROCEDURAL_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_DamageImmunity);
TDS_PROCEDURAL_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_DamageSelfDestruct);
TDS_PROCEDURAL_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_FellOutOfWorld);
TDS_PROCEDURAL_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_HasShield);

struct FGameplayEffectModCallbackData;

/**
 * UProceduralHealthSet
 *
 * Class that defines attributes necessary for taking damage.
 * Attribute examples include: health, shields, resistances
 * 
 */
UCLASS()
class TDS_PROCEDURAL_API UProceduralHealthSet : public UProceduralAttributeSet
{
	GENERATED_BODY()

public:
	/**
	 * Constructor
	 */
	UProceduralHealthSet();

	ATTRIBUTE_ACCESSORS(UProceduralHealthSet, Health);
	ATTRIBUTE_ACCESSORS(UProceduralHealthSet, MaxHealth)
	ATTRIBUTE_ACCESSORS(UProceduralHealthSet, Damage);
	ATTRIBUTE_ACCESSORS(UProceduralHealthSet, Healing);
	
	// Delegate when health changes due to damage/healing
	mutable FProceduralAttributeEvent OnHealthChanged;
	// Delegate when max health changes
	mutable FProceduralAttributeEvent OnMaxHealthChanged;
	// Delegate to broadcast when the health attribute reaches 0
	mutable FProceduralAttributeEvent OnOutOfHealth;
	// Delegate to broadcast when damage is taken
	mutable FOnDamageTakenEvent OnDamageTaken;

protected:

	// -------------------------------------------------------------------
	//	Replication Functions 
	// -------------------------------------------------------------------
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldValue);
	
	virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

	void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;

private:

	// The current health attribute. This will be capped by the max health attribute
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Procedural|Health", Meta = (HideFromeModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData Health;

	// The current max health attribute. Max health is an attribute since gameplay effects can modify it.
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Procedural|Health", Meta = (HideFromeModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData MaxHealth;
	
	// Used to track when health reaches 0
	bool OutOfHealth;

	// Store health before any changes
	float MaxHealthBeforeAttributeChange;
	float HealthBeforeAttributeChange;
	
	// -------------------------------------------------------------------
	//	Meta Attributes 
	// -------------------------------------------------------------------

	// Incoming healing
	UPROPERTY(BlueprintReadOnly, Category = "Procedural|Health", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Healing;

	// The incoming damage
	UPROPERTY(BlueprintReadOnly, Category = "Procedural|Health", Meta = (HideFromeModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData Damage;
	
};

#undef UE_API
