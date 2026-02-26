// ------------------------------------------------------------------------------
// File Name: TDS_DungeonGenerator.cpp
// Author: Aidan Straker(aidan.straker@gmail.com)
// Project: TDS_Procedural
// Date Created: 2025-12-06
// Date Last Modified: 2026-02-25
// Copyright © 2025 Aidan Straker
// ------------------------------------------------------------------------------

#include "PCG/TDS_DungeonGenerator.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Kismet/KismetArrayLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "System/ProceduralGameInstance.h"

// Sets default values
ATDS_DungeonGenerator::ATDS_DungeonGenerator()
	: Seed(100)
	, Room_Count(1)
	, Room_SizeMax(15)
	, Room_SizeMin(3)
	, bIsMerging(false)
	, Floor_CullMin(0)
	, Floor_CullMax(0)
	, bIsBranching(false)
	, Branching_Threshold(0)
	, Last_Branch(0)
	, Branching_Chance(0.0f)
	, bIgnoreCorridors(true)
	, bTruncateCorridors(true)
	, DungeonState(ELevelState::Setup)
	, MaxLoops(0)
	, Scale(200.0f)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	

	Dungeon_Telemetry = CreateDefaultSubobject<UTelemetry_Component>("Telemetry_");
}

FVector ATDS_DungeonGenerator::GetPlayerSpawn() { return PlayerSpawnLocation; }

// Called when the game starts or when spawned
void ATDS_DungeonGenerator::BeginPlay()
{
	Super::BeginPlay();

	// In the editor this makes the NewSeed boolean act as a button
	if (NewSeed)
	{
		NewSeed = false;
		Seed = FMath::RandRange(0, 900000);
		Stream.Initialize(Seed);
	}

	// Manually update stream seed
	Stream.Initialize(Seed);

	Dungeon_Telemetry->AddTrackersAsInt("Seed", false, Seed);
	Dungeon_Telemetry->AddTrackersAsInt("StreamSeed", false, Stream.GetCurrentSeed());
	Dungeon_Telemetry->AddTrackersAsInt("Room_Count", false, Room_Count);
	Dungeon_Telemetry->AddTrackersAsInt("Room_SizeMax", false, Room_SizeMax);
	Dungeon_Telemetry->AddTrackersAsInt("Room_SizeMin", false, Room_SizeMin);
	Dungeon_Telemetry->AddTrackersAsInt("Floor_CullMin", false, Floor_CullMin);
	Dungeon_Telemetry->AddTrackersAsInt("Floor_CullMax", false, Floor_CullMax);
	Dungeon_Telemetry->AddTrackersAsInt("MaxLoops", false, MaxLoops);
	Dungeon_Telemetry->AddTrackersAsInt("Branching_Threshold", false, Branching_Threshold);
	Dungeon_Telemetry->AddTrackersAsFloat("Scale", false, Scale);
	Dungeon_Telemetry->AddTrackersAsFloat("Branching_Chance", false, Branching_Chance);
	Dungeon_Telemetry->AddTrackersAsBool("IsCulling", bIsFloorCulling);
	Dungeon_Telemetry->AddTrackersAsBool("IsBranching", bIsBranching);
	Dungeon_Telemetry->AddTrackersAsBool("IsMerging", bIsMerging);

	PlayerSpawnLocation = FVector(PreviousLocation) * Scale;
	PlayerSpawnRotation = GetActorRotation();
	
	GenerateDungeon();

	Dungeon_Telemetry->AddTrackersAsInt("Tile_count", false, FloorTiles.Num());
}

void ATDS_DungeonGenerator::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	Dungeon_Telemetry->PrintTelemetry(true, false);
}

// Called every frame
void ATDS_DungeonGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATDS_DungeonGenerator::GenerateDungeon()
{
	for (int i = 0; i < Room_Count; i++)
	{
		if (i == 0)
		{
			// Generate first room at relative location 0, 0, 0
			FIntVector Extents = Room_MakeFloor(PreviousLocation, FloorTiles);
			RoomsMap.Add(PreviousLocation, Extents);
		}
		else
		{
			if (bIsBranching)
			{
				TArray<FIntVector> MapKeys;
				RoomsMap.GetKeys(MapKeys);
				if ((MapKeys.Num() - 1 >= Branching_Threshold + Last_Branch) && UKismetMathLibrary::RandomBoolWithWeightFromStream(Stream, Branching_Chance))
				{
					PreviousLocation = Room_BranchingPath();
					Room_MakeRoom();
				}
			}
			else
			{
				Room_MakeRoom();
			}
		}
	}

	Dungeon_SpawnTiles();
}

FIntVector ATDS_DungeonGenerator::Room_MakeFloor(const FIntVector& Location, TArray<FIntVector>& ConnectedTiles)
{
	// This is where the room starts generating from
	const FIntVector RootLocation = Location;
	// All the tiles that comprise the room floor
	TArray<FIntVector> Tiles;
	TArray<FIntVector> TilesCopy;
	int LoopCount = 0;

	TArray<FIntPoint> Neighbours =
	{
		{1, 0},
		{0, 1},
	    {-1, 0},
		{0, -1}
	};

	TArray<int> Extents_X;
	TArray<int> Extents_Y;

	// Get random X and Y extents
	const int OutX = Stream.RandRange(Room_SizeMin, Room_SizeMax);
	const int OutY = Stream.RandRange(Room_SizeMin, Room_SizeMax);

	bool Working = true;

	const int Area = (OutX * OutY);

	// Formulate the location for the tiles that make up a room
	for (int i = 0; i < Area; ++i)
	{
		const int NewXVector = (i / OutY) + RootLocation.X;
		const int NewYVector = (i % OutY) + RootLocation.Y;

		Tiles.Add(FIntVector(NewXVector, NewYVector, RootLocation.Z));
	}

	// Randomly remove tiles from floor
	if (bIsFloorCulling)
	{
		while (Working)
		{
			// Fail out without culling tiles
			if (LoopCount > MaxLoops)
			{
				Working = false;
				ConnectedTiles = Tiles;
				break;
			}
			TilesCopy = Tiles;

			int LastIndex = Stream.RandRange(Floor_CullMin, Floor_CullMax) - 1;
			// Only a quarter of the tiles in a room will be culled
			LastIndex = FMath::Clamp(LastIndex, 0, TilesCopy.Num() / 4);
			for (int i = 0; i < LastIndex; ++i)
			{
				TilesCopy.RemoveAt(Stream.RandRange(0, TilesCopy.Num() - 1));
			}
			// Clear the array to deal with barely connected tiles
			ConnectedTiles.Empty();
			ConnectedTiles.Add(TilesCopy[0]);

			bool TileFound = true;

			// Loop through tiles checking if neighbours are present
			while (TileFound)
			{
				TileFound = false;
				for (TArray<FIntVector>::TIterator Tile = ConnectedTiles.CreateIterator(); Tile; ++Tile)
				{
					for (int i = 0; i < 4; i++)
					{
						if (FIntVector Candidate = FIntVector(Tile->X + Neighbours[i].X, +Tile->Y + Neighbours[i].Y, Tile->Z); TilesCopy.Contains(Candidate))
						{
							ConnectedTiles.Add(Candidate);
							TilesCopy.RemoveSingle(Candidate);
							TileFound = true;
						}
					}
				}
			}

			// Is array of connected tiles larger than minimum viable area?
			if (ConnectedTiles.Num() > (Room_SizeMin * Room_SizeMin))
			{
				Working = false;
			}
			else
			{
				Working = true;
				LoopCount++;
			}
		}
	}
	else
	{
		ConnectedTiles = Tiles;
	}

	// Get outer extents of room
	for (const auto& ConnectedTile : ConnectedTiles)
	{
		Extents_X.Add(ConnectedTile.X);
		Extents_Y.Add(ConnectedTile.Y);
	}

	// Output final results
	return FIntVector(FMath::Max(Extents_X), FMath::Max(Extents_Y), 0);
}

void ATDS_DungeonGenerator::Dungeon_SpawnTiles()
{
	// The offsets for the neighbours
	TArray<FIntPoint> Neighbours =
	{
		{1, 0},
		{0, 1},
		{-1, 0},
		{0, -1}
	};

	// The different rotations for the walls
	TArray<FRotator> Rotations =
	{
		{0.0f, 0.0f, 0.0f},
	    {0.0f, 90.0f, 0.0f},
		{0.0f, 180.0f, 0.0f},
		{0.0f, -90.0f, 0.0f}
	};

	TArray<FIntPoint> Diagonals =
	{
		{1, -1},
		{1, 1},
		{-1, 1},
		{-1, -1}
	};

	// Loop through and remove any overlapping corridors
	for (const auto& Tile : FloorTiles)
	{
		if (int Index = HallwayTiles.Find(Tile); HallwayTiles.IsValidIndex(Index))
		{
			HallwayTiles.RemoveAt(Index);
		}
	}

	// Spawn Doors
	for (const auto& Tile : HallwayTiles)
	{
		for (int i = 0; i < 4; i++)
		{
			if (FIntVector Candidate = FIntVector(Tile.X + Neighbours[i].X, Tile.Y + Neighbours[i].Y, Tile.Z); FloorTiles.Contains(Candidate))
			{
				FVector WallTranslation = FVector(Tile.X, Tile.Y, Tile.Z) * Scale;
				FTransform WallTransform = UKismetMathLibrary::MakeTransform(WallTranslation, Rotations[i]);
				Doors->AddInstance(WallTransform);
			}
		}
	}
	
	FloorTiles.Append(HallwayTiles);
	
	// Spawn floors
	for (const auto& Tile : FloorTiles)
	{
		FVector TileLocation = FVector(Tile.X, Tile.Y, Tile.Z) * Scale;
		Floor->AddInstance(FTransform(TileLocation));
	}

	// Spawn walls
	for (const auto& Tile : FloorTiles)
	{
		for (int i = 0; i < 4; i++)
		{
			if (FIntVector Candidate = FIntVector(Tile.X + Neighbours[i].X, Tile.Y + Neighbours[i].Y, Tile.Z); !FloorTiles.Contains(Candidate))
			{
				FVector WallTranslation = FVector(Tile.X, Tile.Y, Tile.Z) * Scale;
				FTransform WallTransform = UKismetMathLibrary::MakeTransform(WallTranslation, Rotations[i]);
				Walls->AddInstance(WallTransform);
			}
		}
	}

	// Spawn outer corners
	for (const auto& Tile : FloorTiles)
	{
		for (int i = 0; i < 4; i++)
		{
			switch (i)
			{
			case 0: Room_GenerateOuterCorners(Tile, Rotations[i], Diagonals[i],Neighbours[0], Neighbours[3]);
				break;

			case 1: Room_GenerateOuterCorners(Tile, Rotations[i], Diagonals[i], Neighbours[0], Neighbours[1]);
				break;

			case 2: Room_GenerateOuterCorners(Tile, Rotations[i], Diagonals[i], Neighbours[2], Neighbours[1]);
				break;

			case 3: Room_GenerateOuterCorners(Tile, Rotations[i], Diagonals[i], Neighbours[2], Neighbours[3]);
				break;
				
				default:
					break;
			}
		}
	}

	// Spawn inner corners
	for (const auto& Tile : FloorTiles)
	{
		for (int i = 0; i < 4; i++)
		{
			switch (i)
			{
			case 0:
				{
					FIntVector Candidate = FIntVector(Tile.X + Neighbours[0].X, Tile.Y + Neighbours[0].Y, Tile.Z);
					FIntVector Candidate1 = FIntVector(Tile.X + Neighbours[1].X, Tile.Y + Neighbours[1].Y, Tile.Z);
					if ((!FloorTiles.Contains(Candidate) && !FloorTiles.Contains(Candidate1)) == true)
					{
						FVector InnerCornerTranslation = FVector(Tile.X, Tile.Y, Tile.Z) * Scale;
						FTransform InnerCornerTransform = UKismetMathLibrary::MakeTransform(InnerCornerTranslation, Rotations[i]);
						InnerCorner->AddInstance(InnerCornerTransform);
					}
				}
				break;

			case 1:
				{
					FIntVector Candidate = FIntVector(Tile.X + Neighbours[1].X, Tile.Y + Neighbours[1].Y, Tile.Z);
					FIntVector Candidate1 = FIntVector(Tile.X + Neighbours[2].X, Tile.Y + Neighbours[2].Y, Tile.Z);
					if ((!FloorTiles.Contains(Candidate) && !FloorTiles.Contains(Candidate1)) == true)
					{
						FVector InnerCornerTranslation = FVector(Tile.X, Tile.Y, Tile.Z) * Scale;
						FTransform InnerCornerTransform = UKismetMathLibrary::MakeTransform(InnerCornerTranslation, Rotations[i]);
						InnerCorner->AddInstance(InnerCornerTransform);
					}
				}
				break;

			case 2:
				{
					FIntVector Candidate = FIntVector(Tile.X + Neighbours[2].X, Tile.Y + Neighbours[2].Y, Tile.Z);
					FIntVector Candidate1 = FIntVector(Tile.X + Neighbours[3].X, Tile.Y + Neighbours[3].Y, Tile.Z);
					if ((!FloorTiles.Contains(Candidate) && !FloorTiles.Contains(Candidate1)) == true)
					{
						FVector InnerCornerTranslation = FVector(Tile.X, Tile.Y, Tile.Z) * Scale;
						FTransform InnerCornerTransform = UKismetMathLibrary::MakeTransform(InnerCornerTranslation, Rotations[i]);
						InnerCorner->AddInstance(InnerCornerTransform);
					}
				}
				break;

			case 3:
				{
					FIntVector Candidate = FIntVector(Tile.X + Neighbours[0].X, Tile.Y + Neighbours[0].Y, Tile.Z);
					FIntVector Candidate1 = FIntVector(Tile.X + Neighbours[3].X, Tile.Y + Neighbours[3].Y, Tile.Z);
					if ((!FloorTiles.Contains(Candidate) && !FloorTiles.Contains(Candidate1)) == true)
					{
						FVector InnerCornerTranslation = FVector(Tile.X, Tile.Y, Tile.Z) * Scale;
						FTransform InnerCornerTransform = UKismetMathLibrary::MakeTransform(InnerCornerTranslation, Rotations[i]);
						InnerCorner->AddInstance(InnerCornerTransform);
					}
				}
				break;
			default:
				break;
			}
		}
	}

	// When the dungeon is finished fire off an event
	OnGenerationFinished.Broadcast(this, PlayerSpawnLocation, FVector(PreviousLocation));
	Dungeon_Telemetry->PrintTelemetry(true, false);
}

bool ATDS_DungeonGenerator::Room_FindNextLocation(FIntVector& NewLocation) const
{
	// The offset value changes based on if we allow merging or not
	const int32 M = bIsMerging ? Room_SizeMax : Room_SizeMax + 1;

	// Construct an array of offsets
	TArray<FIntPoint> Offsets = {
		{M, 0}, // Right
		{M, M}, // Upper Right
		{0, M}, // Up
		{-M, M}, // Upper left
		{-M, 0}, // Left
		{-M, -M}, // Lower left
		{0, -M}, //  Down
		{M, -M} //  lower right
	};

	while (Offsets.Num() > 0)
	{
		const int32 RandIndex = Stream.RandRange(0, Offsets.Num() - 1);
		const FIntPoint Offset = Offsets[RandIndex];

		// The candidate location we need to evaluate 
		FIntVector Candidate = FIntVector(PreviousLocation.X + Offset.X, PreviousLocation.Y + Offset.Y, PreviousLocation.Z);

		// If the candidate location is already being used 
		if (FloorTiles.Contains(Candidate))
		{
			Offsets.RemoveAt(RandIndex);
			continue;
		}

		NewLocation = Candidate;

		if (bIgnoreCorridors)
		{
			return true;
		}

		// Test for overlapping corridors
		return Dungeon_OverlappingCorridors(NewLocation.Z);
	}

	return false;
}

void ATDS_DungeonGenerator::Room_MakeRoom()
{
	// Find location to place next room
	if (Room_FindNextLocation(Dungeon_NextRoomLocation))
	{
		// Calculate floor tiles for new room
		FIntVector Location = Dungeon_NextRoomLocation;
		TArray<FIntVector> Tiles;
		FIntVector Extents = Room_MakeFloor(Location, Tiles);
		FloorTiles.Append(Tiles);
		RoomsMap.Add(Location, Extents);
		// Make corridors between last two rooms
		HallwayTiles.Append(Dungeon_MapCorridors(PreviousLocation, Dungeon_NextRoomLocation));
		// Update location for next loop
		PreviousLocation = Dungeon_NextRoomLocation;
	}
	else
	{
		PreviousLocation = Room_BranchingPath();
	}
}

bool ATDS_DungeonGenerator::TestRelativeTileLocation(const FIntVector ReferenceLocation, const int32 X, const int32 Y,
                                                     FIntVector& OutLocation) const
{
	OutLocation = FIntVector(ReferenceLocation.X + X, ReferenceLocation.Y + Y, ReferenceLocation.Z);
	return FloorTiles.Contains(OutLocation);
}

FIntVector ATDS_DungeonGenerator::Room_BranchingPath()
{
	TArray<FIntVector> MapKeys;
	RoomsMap.GetKeys(MapKeys);
	Last_Branch = MapKeys.Num() - 1;
	return MapKeys[UKismetMathLibrary::RandomIntegerFromStream(Stream, MapKeys.Num() - 1)];
}

TObjectPtr<UInstancedStaticMeshComponent> ATDS_DungeonGenerator::CreateStaticMesh(const TCHAR* MeshFilePath,
	const TCHAR* IMCName)
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(MeshFilePath);
	const TObjectPtr<UStaticMesh> Mesh = MeshAsset.Object;

	TObjectPtr<UInstancedStaticMeshComponent> MeshComponent = CreateDefaultSubobject<
		UInstancedStaticMeshComponent>(IMCName);
	MeshComponent->RegisterComponent();
	MeshComponent->SetStaticMesh(Mesh);
	MeshComponent->SetFlags(RF_Transactional);
	this->AddInstanceComponent(MeshComponent);
	return MeshComponent;
}

void ATDS_DungeonGenerator::Room_GenerateOuterCorners(const FIntVector& Tile, const FRotator& Rotation,
	const FIntPoint& Diagonal, const FIntPoint& Neighbour_One, const FIntPoint& Neighbour_Two)
{
	FIntVector OutLocal = FIntVector();
	const bool bIsTileNotInArray = !TestRelativeTileLocation(Tile, Diagonal.X, Diagonal.Y, OutLocal);
	const bool bAreNeighboursInArray = TestRelativeTileLocation(Tile, Neighbour_One.X, Neighbour_One.Y, OutLocal)
									&& TestRelativeTileLocation(Tile, Neighbour_Two.X, Neighbour_Two.Y, OutLocal);

	if ((bIsTileNotInArray && bAreNeighboursInArray) == true)
	{
		FVector OutCornerTranslation = FVector(Tile.X, Tile.Y, Tile.Z) * Scale;
		FTransform OutCornerTransform = UKismetMathLibrary::MakeTransform(OutCornerTranslation, Rotation);
		OuterCorner->AddInstance(OutCornerTransform);
	}
}

TArray<FIntVector> ATDS_DungeonGenerator::Dungeon_MapCorridors(const FIntVector RoomA, const FIntVector RoomB)
{
	// Get the starting positions of Room A and B
	int StartX = RoomA.X, StartY = RoomA.Y, StartZ = RoomA.Z;
	int EndX = RoomB.X, EndY = RoomB.Y, EndZ = RoomB.Z;

	// Get the extents of Room A and B
	FIntVector StartExtent = *RoomsMap.Find(RoomA);
	FIntVector EndExtent = *RoomsMap.Find(RoomB);

	TArray<FIntVector> CorridorTiles;

	// Are rooms parallel on X with overlapping sides?
	if (FMath::Max(StartX, EndX) <= FMath::Min(StartExtent.X, EndExtent.X))
	{
		// Is Room B to the right of A?
		if (EndY > StartY && EndY - StartExtent.Y > 1)
		{
			CorridorTiles = Dungeon_MakeCorridor_Parallel(false, {StartX, StartExtent.Y, StartZ}, StartExtent,
				{EndX, EndY, EndZ}, EndExtent);
		}
		// Room B is to the left of A
		else if (StartY - EndExtent.Y > 1)
		{
			CorridorTiles = Dungeon_MakeCorridor_Parallel(false, {StartX, StartY, StartZ}, StartExtent,
				{EndX, EndExtent.Y, EndZ}, EndExtent);
		}
	}
	// The rooms are not parallel on the X
	else if (FMath::Max(StartY, EndY) <= FMath::Min(StartExtent.Y, EndExtent.Y))
	{
		if (EndX > StartX && EndX - StartExtent.X > 1)
		{
			CorridorTiles = Dungeon_MakeCorridor_Parallel(true, 
				{ StartExtent.X, StartY, StartZ }, StartExtent, 
				{ EndX, EndY, EndZ }, EndExtent);
		}
		else if (StartX - EndExtent.X > 1)
		{
			CorridorTiles = Dungeon_MakeCorridor_Parallel(true, 
				{ StartX, StartY, StartZ }, StartExtent, 
				{ EndExtent.X, EndY, EndZ }, EndExtent);
		}
	}
	// The rooms are not parallel to each other on either axis (RoomB is above and to the right of RoomA)
	else if (EndX > StartX && EndY > StartY)
	{
		// Randomly decide if the 1st corridor segment should start from the right or top of RoomA
		if (UKismetMathLibrary::RandomBoolFromStream(Stream))
		{
			
			int LoopCount = 0;
			// Attempt to make corridor from A to Corner (X), from Corner to B (Y), hook up then right
			while (LoopCount < MaxLoops)
			{
				const int OutputX = UKismetMathLibrary::RandomIntegerInRangeFromStream(Stream, EndX, EndExtent.X);
				const int OutputY = UKismetMathLibrary::RandomIntegerInRangeFromStream(Stream, StartY, StartExtent.Y);

				CorridorTiles = Dungeon_MakeCorridor_Perpendicular(Up_Right,StartExtent.X, OutputY, OutputX, EndY, OutputX, OutputY, StartZ);
				if (CorridorTiles.Num() > 0)
				{
					break;
				}
				LoopCount++;
			}
		}
		else
		{
			int LoopCount = 0;
			// Attempt to make corridor from A to Corner (Y), from Corner to B (X), hook right then up
			while (LoopCount < MaxLoops)
			{
				const int OutputX = UKismetMathLibrary::RandomIntegerInRangeFromStream(Stream, StartX, StartExtent.X);
				const int OutputY = UKismetMathLibrary::RandomIntegerInRangeFromStream(Stream, EndY, EndExtent.Y);

				CorridorTiles = Dungeon_MakeCorridor_Perpendicular(Right_Up,OutputX, StartExtent.Y, EndX, OutputY, OutputX, OutputY, StartZ);
				if (CorridorTiles.Num() > 0)
				{
					break;
				}
				LoopCount++;
			}
		}
	}
	// If B is above A and to the left of A
	else if (EndX > StartX && EndY < StartY)
	{
		// Randomly decide if the 1st corridor segment should start from the right or top of RoomA
		if (UKismetMathLibrary::RandomBoolFromStream(Stream))
		{
			
			int LoopCount = 0;
			// Attempt to make corridor from A to Corner (X), from B to Corner (Y), hook up then left
			while (LoopCount < MaxLoops)
			{
				const int OutputX = UKismetMathLibrary::RandomIntegerInRangeFromStream(Stream, EndX, EndExtent.X);
				const int OutputY = UKismetMathLibrary::RandomIntegerInRangeFromStream(Stream, StartY, StartExtent.Y);

				CorridorTiles = Dungeon_MakeCorridor_Perpendicular(Up_Left,StartExtent.X, OutputY, OutputX, EndExtent.Y, OutputX, OutputY, StartZ);
				if (CorridorTiles.Num() > 0)
				{
					break;
				}
				LoopCount++;
			}
		}
		else
		{
			int LoopCount = 0;
			// Attempt to make corridor from Corner to A (Y), from Corner to B (X), hook left then up
			while (LoopCount < MaxLoops)
			{
				const int OutputX = UKismetMathLibrary::RandomIntegerInRangeFromStream(Stream, StartX, StartExtent.X);
				const int OutputY = UKismetMathLibrary::RandomIntegerInRangeFromStream(Stream, EndY, EndExtent.Y);

				CorridorTiles = Dungeon_MakeCorridor_Perpendicular(Left_Up,OutputX, StartY, EndX, OutputY, OutputX, OutputY, StartZ);
				if (CorridorTiles.Num() > 0)
				{
					break;
				}
				LoopCount++;
			}
		}
	}
	// The rooms are not parallel to each other on either axis (RoomB is below and to the right of RoomA)
	else if (EndX < StartX && EndY > StartY)
	{
		// Randomly decide if the 1st corridor segment should start from the right or top of RoomA
		if (UKismetMathLibrary::RandomBoolFromStream(Stream))
		{
			
			int LoopCount = 0;
			// Attempt to make corridor from A to Corner (Y), from B to Corner (X), Hook Right Then Down
			while (LoopCount < MaxLoops)
			{
				const int OutputX = UKismetMathLibrary::RandomIntegerInRangeFromStream(Stream, StartX, StartExtent.X);
				const int OutputY = UKismetMathLibrary::RandomIntegerInRangeFromStream(Stream, EndY, EndExtent.Y);

				CorridorTiles = Dungeon_MakeCorridor_Perpendicular(Right_Down,OutputX, StartExtent.Y, EndExtent.X, OutputY, OutputX, OutputY, StartZ);
				if (CorridorTiles.Num() > 0)
				{
					break;
				}
				LoopCount++;
			}
		}
		else
		{
			int LoopCount = 0;
			// Attempt to make corridor Corner to A (X), from Corner to B (Y), hook down then right
			while (LoopCount < MaxLoops)
			{
				const int OutputX = UKismetMathLibrary::RandomIntegerInRangeFromStream(Stream, EndX, EndExtent.X);
				const int OutputY = UKismetMathLibrary::RandomIntegerInRangeFromStream(Stream, StartY, StartExtent.Y);

				CorridorTiles = Dungeon_MakeCorridor_Perpendicular(Down_Right,StartX, OutputY, OutputX, EndY, OutputX, OutputY, StartZ);
				if (CorridorTiles.Num() > 0)
				{
					break;
				}
				LoopCount++;
			}
		}
	}
	// If B is below A and to the left of A
	else if (EndX < StartX && EndY < StartY)
	{
		// Randomly decide if the 1st corridor segment should start from the right or top of RoomA
		if (UKismetMathLibrary::RandomBoolFromStream(Stream))
		{
			
			int LoopCount = 0;
			// Attempt to make corridor from Corner to A (Y), from B to Corner (X), hook left then down
			while (LoopCount < MaxLoops)
			{
				const int OutputX = UKismetMathLibrary::RandomIntegerInRangeFromStream(Stream, StartX, StartExtent.X);
				const int OutputY = UKismetMathLibrary::RandomIntegerInRangeFromStream(Stream, EndY, EndExtent.Y);

				CorridorTiles = Dungeon_MakeCorridor_Perpendicular(Left_Down,OutputX, StartY, EndExtent.X, OutputY, OutputX, OutputY, StartZ);
				if (CorridorTiles.Num() > 0)
				{
					break;
				}
				LoopCount++;
			}
		}
		else
		{
			int LoopCount = 0;
			// Attempt to make corridor from Corner to A (X), from B to Corner (Y), hook down then left
			while (LoopCount < MaxLoops)
			{
				const int OutputX = UKismetMathLibrary::RandomIntegerInRangeFromStream(Stream, EndX, EndExtent.X);
				const int OutputY = UKismetMathLibrary::RandomIntegerInRangeFromStream(Stream, StartY, StartExtent.Y);

				CorridorTiles += Dungeon_MakeCorridor_Perpendicular(Down_Left,StartX, OutputY, OutputX, EndExtent.Y, OutputX, OutputY, StartZ);
				if (CorridorTiles.Num() > 0)
				{
					break;
				}
				LoopCount++;
			}
		}
	}
	return CorridorTiles;
}

TArray<FIntVector> ATDS_DungeonGenerator::Corridor_Make_Y(FIntVector From, FIntVector To)
{
	TArray<FIntVector> Tiles;
	FIntVector Vector = FIntVector();

	if (UKismetMathLibrary::RandomBoolFromStream(Stream))
	{
		for (int i = 0; i < FMath::Abs(From.Y - To.Y); i++)
		{
			Vector = FIntVector(From.X, From.Y + i, From.Z);
			if (!FloorTiles.Contains(Vector))
			{
				Tiles.Add(Vector);
			}
		}
	}
	// Alcove
	else
	{
		for (int i = 0; i < FMath::Abs(From.Y - To.Y); i++)
		{
			if (i % 2 != 0)
			{
				FIntVector TopAlcove = FIntVector(From.X + 1, From.Y + i, From.Z);
				Vector = FIntVector(From.X, From.Y + i, From.Z);
				FIntVector BottomAlcove = FIntVector(From.X - 1, From.Y + i, From.Z);
				if (!FloorTiles.Contains(Vector))
				{
					Tiles.Add(Vector);
				}

				if (!FloorTiles.Contains(TopAlcove))
				{
					Tiles.Add(TopAlcove);
				}

				if (!FloorTiles.Contains(BottomAlcove))
				{
					Tiles.Add(BottomAlcove);
				}
			}
			else
			{
				Vector = FIntVector(From.X, From.Y + i, From.Z);
				if (!FloorTiles.Contains(Vector))
				{
					Tiles.Add(Vector);
				}
			}
		}
	}
	
	

	return Tiles;
}

TArray<FIntVector> ATDS_DungeonGenerator::Corridor_Make_X(FIntVector From, FIntVector To)
{
	TArray<FIntVector> Tiles;
	for (int i = 0; i < FMath::Abs(From.X - To.X); i++)
	{
		if (!FloorTiles.Contains(FIntVector(From.X + i, From.Y, From.Z)))
		{
			Tiles.Add(FIntVector(From.X + i, From.Y, From.Z));
		}
	}

	return Tiles;
}

TArray<FIntVector> ATDS_DungeonGenerator::Dungeon_MakeCorridor_Parallel(const bool bIsXAxis, const FIntVector& StartA,
	const FIntVector& EndA, const FIntVector& StartB, const FIntVector& EndB)
{
	TArray<FIntVector> CorridorTiles;
	int LoopCount = 0;

	while (LoopCount < MaxLoops)
	{
		if (bIsXAxis)
		{
			const int MinY = FMath::Min(StartA.Y, StartB.Y);
			const int MaxY = FMath::Max(EndA.Y, EndB.Y);
			const int OutputY = UKismetMathLibrary::RandomIntegerInRangeFromStream(Stream, MinY, MaxY);

			FIntVector PointA(StartA.X, OutputY, StartA.Z);
			FIntVector PointB(EndB.X, OutputY, EndB.Z);

			// Ensure proper direction
			if (PointB.X < PointA.X)
				Swap(PointA, PointB);

			if (FloorTiles.Contains(PointA) && FloorTiles.Contains(PointB))
			{
				CorridorTiles = Corridor_Make_X(PointA, PointB);
				break;
			}
		}
		else
		{
			// Always ensure Min <= Max
			const int MinX = FMath::Min(StartA.X, StartB.X);
			const int MaxX = FMath::Max(EndA.X, EndB.X);
			const int OutputX = UKismetMathLibrary::RandomIntegerInRangeFromStream(Stream, MinX, MaxX);

			FIntVector PointA(OutputX, StartA.Y, StartA.Z);
			FIntVector PointB(OutputX, EndB.Y, EndB.Z);

			// Ensure proper direction
			if (PointB.Y < PointA.Y)
				Swap(PointA, PointB);

			if (FloorTiles.Contains(PointA) && FloorTiles.Contains(PointB))
			{
				CorridorTiles = Corridor_Make_Y(PointA, PointB);
				break;
			}
		}
		LoopCount++;
	}

	if (LoopCount >= MaxLoops && GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1, 10.f, FColor::Yellow,
			FString::Printf(TEXT("Corridor Failed (%s)"), bIsXAxis ? TEXT("X") : TEXT("Y"))
		);
	}

	return CorridorTiles;
}

TArray<FIntVector> ATDS_DungeonGenerator::Dungeon_MakeCorridor_Perpendicular(ECorridorDirection Direction, int PointAX, int PointAY, int PointBX,
                                                                             int PointBY, int PointCX, int PointCY, int Z)
{
	TArray<FIntVector> CorridorTiles;
	FIntVector PointA = FIntVector(PointAX, PointAY, Z);
	FIntVector PointB = FIntVector(PointBX, PointBY, Z);
	FIntVector PointC = FIntVector(PointCX, PointCY, Z);

	if (FloorTiles.Contains(PointA) && FloorTiles.Contains(PointB))
	{
		switch (Direction)
		{
		case Up_Right:
			{
				CorridorTiles = Corridor_Make_X(PointA, PointC);
				CorridorTiles += Corridor_Make_Y(PointC, PointB);
				CorridorTiles.Add(PointC);
			}
			break;
		case Right_Up:
			{
				CorridorTiles = Corridor_Make_Y(PointA, PointC);
				CorridorTiles += Corridor_Make_X(PointC, PointB);
				CorridorTiles.Add(PointC);
			}
			break;
		case Up_Left:
			{
				CorridorTiles = Corridor_Make_X(PointA, PointC);
				CorridorTiles += Corridor_Make_Y(PointB, PointC);
				CorridorTiles.Add(PointC);
			}
			break;
		case Left_Up:
			{
				CorridorTiles = Corridor_Make_Y(PointC, PointA);
				CorridorTiles += Corridor_Make_X(PointC, PointB);
				CorridorTiles.Add(PointC);
			}
			break;
		case Down_Right:
			{
				CorridorTiles = Corridor_Make_X(PointC, PointA);
				CorridorTiles += Corridor_Make_Y(PointC, PointB);
				CorridorTiles.Add(PointC);
			}
			break;
		case Right_Down:
			{
				CorridorTiles = Corridor_Make_Y(PointA, PointC);
				CorridorTiles += Corridor_Make_X(PointB, PointC);
				CorridorTiles.Add(PointC);
			}
			break;
		case Down_Left:
			{
				CorridorTiles = Corridor_Make_X(PointC, PointA);
				CorridorTiles += Corridor_Make_Y(PointB, PointC);
				CorridorTiles.Add(PointC);
			}
			break;
		case Left_Down:
			{
				CorridorTiles = Corridor_Make_Y(PointC, PointA);
				CorridorTiles += Corridor_Make_X(PointB, PointC);
				CorridorTiles.Add(PointC);
			}
			break;
		}
	}

	return CorridorTiles;
}

bool ATDS_DungeonGenerator::Dungeon_OverlappingCorridors(const int ZAxis) const
{
	// Create a bounding box
	for (int i = 0; i < (Room_SizeMax * Room_SizeMax) - 1; ++i)
	{
		// Is there a corridor in this bounding box
		if (HallwayTiles.Contains(FIntVector(i / Room_SizeMax, i % Room_SizeMax, ZAxis)))
		{
			return true;
		}
	}

	return false;
}
