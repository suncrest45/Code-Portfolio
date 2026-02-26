// ------------------------------------------------------------------------------
// File Name: ProceduralHealthSet.cpp
// Author: Aidan Straker(aidan.straker@gmail.com)
// Project: TDS_Procedural
// Date Created: 2025-10-27
// Date Last Modified: 2026-02-25
// Copyright © 2025 Aidan Straker
// ------------------------------------------------------------------------------

#include "AbilitySystem/Attributes/ProceduralHealthSet.h"
#include "ProceduralGameplayTags.h"
#include "AbilitySystem/ProceduralAbilitySystemComponent.h"
#include "Engine/World.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ProceduralHealthSet)

UE_DEFINE_GAMEPLAY_TAG(TAG_Gameplay_Damage, "Gameplay.Damage");
UE_DEFINE_GAMEPLAY_TAG(TAG_Gameplay_DamageImmunity, "Gameplay.DamageImmunity");
UE_DEFINE_GAMEPLAY_TAG(TAG_Gameplay_DamageSelfDestruct, "Gameplay.Damage.SelfDestruct");
UE_DEFINE_GAMEPLAY_TAG(TAG_Gameplay_FellOutOfWorld, "Gameplay.Damage.FellOutOfWorld");
UE_DEFINE_GAMEPLAY_TAG(TAG_Gameplay_HasShield, "Gameplay.HasShield");


UProceduralHealthSet::UProceduralHealthSet()
    : Health(100.0f)
    , MaxHealth(100.0f)
{
    OutOfHealth = false;
    MaxHealthBeforeAttributeChange = 0.0f;
    HealthBeforeAttributeChange = 0.0f;
}

void UProceduralHealthSet::OnRep_Health(const FGameplayAttributeData& OldValue)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UProceduralHealthSet, Health, OldValue);

    // Call the change callback without an instigator

    const float CurrentHealth = GetHealth();
    const float EstimatedMagnitude = CurrentHealth - OldValue.GetCurrentValue();

    OnHealthChanged.Broadcast(nullptr, nullptr, nullptr, EstimatedMagnitude, OldValue.GetCurrentValue(), CurrentHealth);

    if (!OutOfHealth && CurrentHealth <= 0.0f)
    {
        OnOutOfHealth.Broadcast(nullptr, nullptr, nullptr, EstimatedMagnitude, OldValue.GetCurrentValue(), CurrentHealth);
    }

    OutOfHealth = (CurrentHealth <= 0.0f);
}

void UProceduralHealthSet::OnRep_MaxHealth(const FGameplayAttributeData& OldValue)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UProceduralHealthSet, MaxHealth, OldValue);
    
    // Call the change callback without an instigator
    OnMaxHealthChanged.Broadcast(nullptr, nullptr, nullptr, GetMaxHealth() - OldValue.GetCurrentValue(), OldValue.GetCurrentValue(), GetMaxHealth());
}

bool UProceduralHealthSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
    if (!Super::PreGameplayEffectExecute(Data))
    {
        return false;
    }

    // Handle modifying incoming normal damage
    if (Data.EvaluatedData.Attribute == GetDamageAttribute())
    {
        // Check if there is any damage
        if (Data.EvaluatedData.Magnitude > 0.0f)
        {
            const bool bIsDamageFromSelfDestruct = Data.EffectSpec.GetDynamicAssetTags().HasTagExact(TAG_Gameplay_DamageSelfDestruct);
            // If the target has an immunity to damage, they take no damage
            if (Data.Target.HasMatchingGameplayTag(TAG_Gameplay_DamageImmunity) && !bIsDamageFromSelfDestruct)
            {
                Data.EvaluatedData.Magnitude = 0.0f;
                return false;
            }

            // If god mode is enabled 
            if (Data.Target.HasMatchingGameplayTag(ProceduralGameplayTags::Cheat_GodMode) && !bIsDamageFromSelfDestruct)
            {
                // The target takes no damage
                Data.EvaluatedData.Magnitude = 0.0f;
                return false;
            }
        }
    }

    // Save current health
    HealthBeforeAttributeChange = GetHealth();
    MaxHealthBeforeAttributeChange = GetMaxHealth();
    
    return true;
}

void UProceduralHealthSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
    Super::PostGameplayEffectExecute(Data);
    
    const bool bIsDamageFromSelfDestruct = Data.EffectSpec.GetDynamicAssetTags().HasTagExact(TAG_Gameplay_DamageSelfDestruct);
    float MinimumHealth = 0.0f;

    // If cheats are enabled
    if (!bIsDamageFromSelfDestruct
        && Data.Target.HasMatchingGameplayTag(ProceduralGameplayTags::Cheat_GodMode)
        || Data.Target.HasMatchingGameplayTag(ProceduralGameplayTags::Cheat_UnlimitedHealth))
    {
        MinimumHealth = 1.0f;
    }

    const FGameplayEffectContextHandle& EffectHandle = Data.EffectSpec.GetEffectContext();
    // Get the instigating actor. E.g. The enemy that fired the gun
    AActor* Instigator = EffectHandle.GetOriginalInstigator();
    // The causer is the actual thing that caused the damage. E.g. A bullet
    AActor* Causer = EffectHandle.GetEffectCauser();

    if (Data.EvaluatedData.Attribute == GetDamageAttribute())
    {
        if (Data.EvaluatedData.Magnitude > 0.0f)
        {
            OnDamageTaken.Broadcast(Instigator, Causer, Data.EffectSpec.CapturedSourceTags.GetSpecTags(), Data.EvaluatedData.Magnitude);
        }

       

        // Apply health damage
        if (Data.EvaluatedData.Magnitude > 0.0f)
        {
            SetHealth(FMath::Clamp(GetHealth() - GetDamage(), MinimumHealth, GetMaxHealth()));
            SetDamage(0.0f);
        }
    }
    // If the actor is being healed (e.g. health pick up)
    else if (Data.EvaluatedData.Attribute == GetHealingAttribute())
    {
        SetHealth(FMath::Clamp(GetHealth() + GetHealing(), MinimumHealth, GetMaxHealth()));
        SetHealing(0.0f);
    }
    else if (Data.EvaluatedData.Attribute == GetHealthAttribute())
    {
        SetHealth(FMath::Clamp(GetHealth(), MinimumHealth, GetMaxHealth()));
    }
    else if (Data.EvaluatedData.Attribute == GetMaxHealthAttribute())
    {
        // Clamp health between minimum health and the new max health
        SetHealth(FMath::Clamp(GetHealth(), MinimumHealth, Data.EvaluatedData.Magnitude));
        OnMaxHealthChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, MaxHealthBeforeAttributeChange, GetMaxHealth());
    }
    // If the health has been changed activate callback
    if (GetHealth() != HealthBeforeAttributeChange)
    {
        OnHealthChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, HealthBeforeAttributeChange, GetHealth());
    }
    
    if ((GetHealth() <= 0.0f) && !OutOfHealth)
    {
        OnOutOfHealth.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, HealthBeforeAttributeChange, GetHealth());
    }

    OutOfHealth = (GetHealth() <= 0.0f);
}

void UProceduralHealthSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
    Super::PreAttributeBaseChange(Attribute, NewValue);
    ClampAttribute(Attribute, NewValue);
}

void UProceduralHealthSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
    Super::PreAttributeChange(Attribute, NewValue);
    ClampAttribute(Attribute, NewValue);
}

void UProceduralHealthSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
    Super::PostAttributeChange(Attribute, OldValue, NewValue);

    if (Attribute == GetMaxHealthAttribute())
    {
        // Ensure health does not exceed the new max health
        if (GetHealth() > NewValue)
        {
            UProceduralAbilitySystemComponent* ProceduralASC = GetProceduralAbilitySystemComponent();
            check(ProceduralASC);
            ProceduralASC->ApplyModToAttribute(GetHealthAttribute(), EGameplayModOp::Override, NewValue);
        }
    }
    
    if (OutOfHealth && GetHealth() > 0.0f) { OutOfHealth = false; }
}

void UProceduralHealthSet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
    if (Attribute == GetHealthAttribute())
    {
        NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
    }
    else if (Attribute == GetMaxHealthAttribute())
    {
        NewValue = FMath::Max(NewValue, 1.0f);
    }
}

void UProceduralHealthSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION_NOTIFY(UProceduralHealthSet, Health, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UProceduralHealthSet, MaxHealth, COND_None, REPNOTIFY_Always);
}

