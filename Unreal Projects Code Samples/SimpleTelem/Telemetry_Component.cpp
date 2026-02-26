// Fill out your copyright notice in the Description page of Project Settings.


#include "Telemetry_Component.h"

#include <string>

#include "WriteToFile.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"

// Sets default values for this component's properties
UTelemetry_Component::UTelemetry_Component()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}

EKeyTpe UTelemetry_Component::GetKeyType(const FBlackboardEntry& Key) const
{
	return Telemetry_Map.FindRef(Key.KeyType.GetClass()->GetName());
}


// Called when the game starts
void UTelemetry_Component::BeginPlay()
{
	Super::BeginPlay();

	// Make sure the component has a blackboard 
	if (!this->GetBlackboardAsset()->IsValid())
	{
		return;
	}

	// Loop through the blackboard, getting all keys as strings
	for (auto& Keys : this->GetBlackboardAsset()->GetKeys())
	{
		// Do not record self actor
		if (Keys.EntryName == "SelfActor")
		{
			continue;
		}
		
		Telemetry_KeysString += Keys.EntryName.ToString() + ",";
	}
	
	Telemetry_KeysString += "\n";
}


// Called every frame
void UTelemetry_Component::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UTelemetry_Component::ConcatenateKeysAndValues()
{
	Telemetry_ValuesString += FString::SanitizeFloat(UGameplayStatics::GetTimeSeconds(GetWorld())) + ",";
	for (auto& Keys : this->GetBlackboardAsset()->GetKeys())
	{
		// Do not record self actor
		if (Keys.EntryName == "SelfActor")
		{
			continue;
		}
		
		// After we check the map for the relationship between key and enum, we call the approrpiate function to convert the data to a string
		switch (GetKeyType(Keys)) {
		case Type_Unknown: UE_LOG(LogTemp, Error, TEXT("Key %s is of unknown type"), *Keys.EntryName.ToString()); check(false);
		case Type_Int:
			{
				Telemetry_ValuesString += FString::FromInt(GetValueAsInt(Keys.EntryName)) + ",";
			}
			break;
		case Type_Float:
			{
				Telemetry_ValuesString += FString::SanitizeFloat(GetValueAsFloat(Keys.EntryName)) + ",";
			}
			break;
		case Type_String:
			{
				Telemetry_ValuesString += GetValueAsString(Keys.EntryName) + ",";
			}
			break;
		case Type_Bool:
			{
				FString Result = GetValueAsBool(Keys.EntryName) ? "true" : "false";
				Telemetry_ValuesString += Result + ",";
			}
			break;
		case Type_Enum:
			{
				Telemetry_ValuesString += FString::FromInt(GetValueAsEnum(Keys.EntryName)) + ",";
			}
			break;
		case Type_Vector:
			{
				FString Vector = "X: " + FString::SanitizeFloat(GetValueAsVector(Keys.EntryName).X)
								+ " Y: " + FString::SanitizeFloat(GetValueAsVector(Keys.EntryName).Y)
								+ " Z: " + FString::SanitizeFloat(GetValueAsVector(Keys.EntryName).Z);
				Telemetry_ValuesString += Vector + ",";
			}
			break;
		case Type_Rotator:
			{
				FString Vector = "Pitch: " + FString::SanitizeFloat(GetValueAsRotator(Keys.EntryName).Pitch)
								+ " Yaw: " + FString::SanitizeFloat(GetValueAsRotator(Keys.EntryName).Yaw)
								+ " Roll: " + FString::SanitizeFloat(GetValueAsRotator(Keys.EntryName).Roll);
				Telemetry_ValuesString += Vector + ",";
			}
			break;
		case Type_Name:
			{
				Telemetry_ValuesString += GetValueAsName(Keys.EntryName).ToString() + ",";
			}
			break;
		}
	}
}

FString UTelemetry_Component::GetHeadersString() { return Telemetry_KeysString; }

FString UTelemetry_Component::GetValuesString() { return Telemetry_ValuesString; }

void UTelemetry_Component::AddTrackersAsInt(const FString& Key, const bool CompoundAssignment, const int Value)
{
	if (CompoundAssignment)
	{
		int Stored_Value = GetValueAsInt(FName(Key));
		Stored_Value += Value;
		SetValueAsInt(FName(Key), Stored_Value);
	}
	else
	{
		SetValueAsInt(FName(Key), Value);
	}
}

void UTelemetry_Component::AddTrackersAsFloat(const FString& Key, const bool CompoundAssignment, const float Value)
{
	if (CompoundAssignment)
	{
		float Stored_Value = GetValueAsFloat(FName(Key));
		Stored_Value += Value;
		SetValueAsFloat(FName(Key), Stored_Value);
	}
	else
	{
		SetValueAsFloat(FName(Key), Value);
	}
}


void UTelemetry_Component::AddTrackersAsString(const FString& Key, const FString Value) { SetValueAsString(FName(Key), Value); }

void UTelemetry_Component::AddTrackersAsBool(const FString& Key, const bool Value) { SetValueAsBool(FName(Key), Value); }

void UTelemetry_Component::AddTrackersAsVector(const FString& Key, const FVector& Value){ SetValueAsVector(FName(Key), Value); }

void UTelemetry_Component::AddTrackersAsName(const FString& Key, const FName& Value) { SetValueAsName(FName(Key), Value); }

void UTelemetry_Component::AddTrackersAsRotator(const FString& Key, const FRotator& Value) { SetValueAsRotator(FName(Key), Value); }

void UTelemetry_Component::PrintTelemetry(const bool Append, const bool MinuteStamp)
{
	ConcatenateKeysAndValues();
	Telemetry_ValuesString += "\n";
	UWriteToFile::WriteToFile(FileName, GetHeadersString(), GetValuesString(), MinuteStamp, Append);

	// Reset the values
	Telemetry_ValuesString.Empty();
}
