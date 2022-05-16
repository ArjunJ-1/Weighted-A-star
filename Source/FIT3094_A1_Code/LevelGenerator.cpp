// Fill out your copyright notice in the Description page of Project Settings.

#include "LevelGenerator.h"
#include <string>
#include <solver/PxSolverDefs.h>

#include "Engine/World.h"

// Sets default values
ALevelGenerator::ALevelGenerator()
{
 	// Set this actor to call Tick() every frame. 
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ALevelGenerator::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ALevelGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(Ship && Ship->GeneratePath && GoldActors.Num() > 0)
	{
		CollectGold();
		ResetPath();
		SpawnNextGold();
		CalculateAStar();
		DetailPath();
	}
}

void ALevelGenerator::CollectGold()
{
	GoldArray.RemoveAt(0);
	GoldActors[0]->Destroy();
	GoldActors.RemoveAt(0);
}

void ALevelGenerator::GenerateWorldFromFile(TArray<FString> WorldArrayStrings)
{
	// If empty array exit immediately something is horribly wrong
	if(WorldArrayStrings.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("World Array is empty!"));
		return;
	}

	// Second line is Height (aka Y value)
	FString Height = WorldArrayStrings[1];
	Height.RemoveFromStart("Height ");
	MapSizeY = FCString::Atoi(*Height);
	UE_LOG(LogTemp, Warning, TEXT("Height: %d"), MapSizeY);

	// Third line is Width (aka X value)
	FString Width = WorldArrayStrings[2];
	Width.RemoveFromStart("width ");
	MapSizeX = FCString::Atoi(*Width);
	UE_LOG(LogTemp, Warning, TEXT("Width: %d"), MapSizeX);

	char CharMapArray[MAX_MAP_SIZE][MAX_MAP_SIZE];
	
	// Read through the Map section for create the CharMapArray
	for (int LineNum = 4; LineNum < MapSizeY + 4; LineNum++)
	{
		for (int CharNum = 0; CharNum < WorldArrayStrings[LineNum].Len(); CharNum++)
		{
			// Fixed the error pointed out by Nam Hoai Peter Tran on Ed forum
			CharMapArray[CharNum][LineNum-4] = WorldArrayStrings[LineNum][CharNum];
		}
	}

	// Read in the Gold positions
	for (int LineNum = 4 + MapSizeY; LineNum < WorldArrayStrings.Num(); LineNum++)
	{
		FString GoldX, GoldY;
		WorldArrayStrings[LineNum].Split(",", &GoldY, &GoldX);
		UE_LOG(LogTemp, Warning, TEXT("GoldX: %s"), *GoldX);
		UE_LOG(LogTemp, Warning, TEXT("GoldY: %s"), *GoldY);

		GoldArray.Add(FVector2D(FCString::Atof(*GoldX), FCString::Atof(*GoldY)));

	}

	GenerateNodeGrid(CharMapArray);
	SpawnWorldActors(CharMapArray);
	CalculateAStar();
	DetailPath();
}

void ALevelGenerator::SpawnWorldActors(char Grid[MAX_MAP_SIZE][MAX_MAP_SIZE])
{
	UWorld* World = GetWorld();

	// Make sure that all blueprints are connected. If not then fail
	if(DeepBlueprint && ShallowBlueprint && LandBlueprint)
	{
		// For each grid space spawn an actor of the correct type in the game world
		for(int x = 0; x < MapSizeX; x++)
		{
			for (int y = 0; y < MapSizeY; y++)
			{
				float XPos = x * GRID_SIZE_WORLD;
				float YPos = y * GRID_SIZE_WORLD;

				FVector Position(XPos, YPos, 0);

				switch (Grid[x][y])
				{
					case '.':
						World->SpawnActor(DeepBlueprint, &Position, &FRotator::ZeroRotator);
						break;
					case '@':
						World->SpawnActor(LandBlueprint, &Position, &FRotator::ZeroRotator);
						break;
					case 'T':
						World->SpawnActor(ShallowBlueprint, &Position, &FRotator::ZeroRotator);
						break;
					default:
						break;
				}
			}
		}
	}

	// Generate Initial Agent Positions
	if(ShipBlueprint)
	{
		int XPos = 16; //Default Intial X Position
		int YPos = 23; //Default Intial Y Position

		FVector Position(XPos * GRID_SIZE_WORLD, YPos * GRID_SIZE_WORLD, 20);
		Ship = World->SpawnActor<AShip>(ShipBlueprint, Position, FRotator::ZeroRotator);
		Ship->Level = this;

		WorldArray[XPos][YPos]->ObjectAtLocation = Ship;
		StartNode = WorldArray[XPos][YPos];
	}

	if (GoldBlueprint)
	{
		SpawnNextGold();
	}

	// Set Static Camera Position
	if(Camera)
	{
		FVector CameraPosition = Camera->GetActorLocation();
		
		CameraPosition.X = MapSizeX * 0.5 * GRID_SIZE_WORLD;
		CameraPosition.Y = MapSizeY * 0.5 * GRID_SIZE_WORLD;
		
		Camera->SetActorLocation(CameraPosition);
	}
}

void ALevelGenerator::SpawnNextGold()
{
	UWorld* World = GetWorld();
	// Generate next Gold Position
	if (GoldBlueprint && GoldArray.Num() > 0)
	{
		int XPos = GoldArray[0].X;
		int YPos = GoldArray[0].Y;


		FVector Position(XPos * GRID_SIZE_WORLD, YPos * GRID_SIZE_WORLD, 20);
		AGold* NewGold = World->SpawnActor<AGold>(GoldBlueprint, Position, FRotator::ZeroRotator);

		WorldArray[XPos][YPos]->ObjectAtLocation = NewGold;
		GoalNode = WorldArray[XPos][YPos];
		GoldActors.Add(NewGold);
	}
}

// Generates the grid of nodes used for pathfinding and also for placement of objects in the game world
void ALevelGenerator::GenerateNodeGrid(char Grid[MAX_MAP_SIZE][MAX_MAP_SIZE])
{
	for(int X = 0; X < MapSizeX; X++)
	{
		for(int Y = 0; Y < MapSizeY; Y++)
		{
			WorldArray[X][Y] = new GridNode();
			WorldArray[X][Y]->X = X;
			WorldArray[X][Y]->Y = Y;

			// Characters as defined from the map file
			switch(Grid[X][Y])
			{
				case '.':
					WorldArray[X][Y]->GridType = GridNode::DeepWater;
					break;
				case '@':
					WorldArray[X][Y]->GridType = GridNode::Land;
					break;
				case 'T':
					WorldArray[X][Y]->GridType = GridNode::ShallowWater;
					break;
				default:
					break;
			}
		}
	}
}

// Reset all node values (F, G, H & Parent)
void ALevelGenerator::ResetAllNodes()
{
	for (int X = 0; X < MapSizeX; X++)
	{
		for (int Y = 0; Y < MapSizeY; Y++)
		{
			WorldArray[X][Y]->F = 0;
			WorldArray[X][Y]->G = WorldArray[X][Y]->GetTravelCost();
			WorldArray[X][Y]->H = 0;
			WorldArray[X][Y]->Parent = nullptr;
			WorldArray[X][Y]->IsChecked = false;
		}
	}
}

// also calculates manhattan distance 
float ALevelGenerator::CalculateDistanceBetween(GridNode* first, GridNode* second)
{
	FVector distToTarget = FVector(second->X - first->X,
		second->Y - first->Y, 0);
	return distToTarget.Size();
}

// Calculates difference in x and y and returns abs(sum of the values). Manhattan Distance Heuristic
float ALevelGenerator::CalculateHeuristicDist(GridNode* TargetNode, GridNode* CurrentNode)
{
	float Dx = abs(TargetNode->X - CurrentNode->X);
	float Dy = abs(TargetNode->Y - CurrentNode->Y);
	return (Dx + Dy);
}

void ALevelGenerator::DetailPath()
{
	//Onscreen Debug (Don't forget the include!)
	GEngine->AddOnScreenDebugMessage(-1, 12.f, FColor::White, FString::Printf(TEXT("Total Cells searched: %d with a path length of: %d and a distance of: %f"), SearchCount, Ship->Path.Num(), CalculateDistanceBetween(StartNode, GoalNode)));
	GEngine->AddOnScreenDebugMessage(-1, 12.f, FColor::White, FString::Printf(TEXT("Heuristic Cost: %f G Cost: %d"), HeuristicCost, GCost));
	GEngine->AddOnScreenDebugMessage(-1, 12.f, FColor::White, FString::Printf(TEXT("The difference between the current implemented path and the direct flight path is: %f"), Ship->Path.Num() / CalculateDistanceBetween(StartNode, GoalNode)));
	//Log Debug message (Accessed through Window->Developer Tools->Output Log)
	UE_LOG(LogTemp, Warning, TEXT("Total Cells searched: %d with a path length of: %d and a distance of: %f"), SearchCount, Ship->Path.Num(), CalculateDistanceBetween(StartNode, GoalNode));
	UE_LOG(LogTemp, Warning, TEXT("Heuristic Cost: %f G Cost: %d"), HeuristicCost, GCost);
	UE_LOG(LogTemp, Warning, TEXT("The difference between the current implemented path and the direct flight path is: %f"), Ship->Path.Num() / CalculateDistanceBetween(StartNode, GoalNode));
}



//-----------------------------------------------Edit the Code Below here!----------------------------------------------------------------

void ALevelGenerator::CalculateBFS()
{
	GridNode* currentNode = nullptr;
	GridNode* tempNode = nullptr;
	bool isGoldFound = false;

	TArray<GridNode*> NodesToVisit;

	StartNode->IsChecked = true;
	NodesToVisit.Add(StartNode);

	while (NodesToVisit.Num() >= 0)
	{
		SearchCount++;
		currentNode = NodesToVisit[0];
		NodesToVisit.RemoveAt(0, 1, true);

		if (currentNode == GoalNode)
		{
			isGoldFound = true;
			break;
		}
		
		// Check the left neighbor
		// check to ensure not out of range

		if (currentNode->Y - 1 >= 0)
		{
			// get the left neighbor from the list
			tempNode = WorldArray[currentNode->X][currentNode->Y - 1];
			//Check to make sure the node hasn't been visited and is not closed (land)
			if (tempNode->GridType != GridNode::Land && !tempNode->IsChecked)
			{
				tempNode->IsChecked = true;
				tempNode->Parent = currentNode;
				NodesToVisit.Add(tempNode);
			}
		}

		// Check the top neighbor
		// check to ensure not out of range

		if (currentNode->X + 1 < MapSizeX-1)
		{
			// get the left neighbor from the list
			tempNode = WorldArray[currentNode->X + 1][currentNode->Y];
			//Check to make sure the node hasn't been visited and is not closed (land)
			if (tempNode->GridType != GridNode::Land && !tempNode->IsChecked)
			{
				tempNode->IsChecked = true;
				tempNode->Parent = currentNode;
				NodesToVisit.Add(tempNode);
			}
		}

		// Check the right neighbor
		// check to ensure not out of range

		if (currentNode->Y + 1 < MapSizeY-1)
		{
			// get the left neighbor from the list
			tempNode = WorldArray[currentNode->X][currentNode->Y + 1];
			//Check to make sure the node hasn't been visited and is not closed (land)
			if (tempNode->GridType != GridNode::Land && !tempNode->IsChecked)
			{
				tempNode->IsChecked = true;
				tempNode->Parent = currentNode;
				NodesToVisit.Add(tempNode);
			}
		}

		// Check the bottom neighbor
		// check to ensure not out of range

		if (currentNode->X - 1 > 0)
		{
			// get the left neighbor from the list
			tempNode = WorldArray[currentNode->X - 1][currentNode->Y];
			//Check to make sure the node hasn't been visited and is not closed (land)
			if (tempNode->GridType != GridNode::Land && !tempNode->IsChecked)
			{
				tempNode->IsChecked = true;
				tempNode->Parent = currentNode;
				NodesToVisit.Add(tempNode);
			}
		}
	}

	if (isGoldFound)
	{
		RenderPath();
		Ship->GeneratePath = false;
	}
}

// Predicate to overload operator () to compare heap values
struct my_Predicate
{
	bool operator () (const GridNode &currentNode, const GridNode &tempNode) const
	{
		return currentNode.F < tempNode.F;
	}
};

void ALevelGenerator::CalculateAStar()
{
	GridNode* currentNode = nullptr; 
	GridNode* tempNode = nullptr;
	bool isGoldFound = false;

	TArray<GridNode*> NodesToVisit; // open list of nodes to visit
	TArray<GridNode*> ClosedList; // already visited nodes

	StartNode->G = 0;
	StartNode->H = CalculateHeuristicDist(GoalNode, StartNode);
	StartNode->F = StartNode->G + StartNode->H;
	NodesToVisit.HeapPush(StartNode, my_Predicate()); // Push using my predicate to create a heap
	HeuristicCost = StartNode->H;

	while(NodesToVisit.Num() > 0)
	{
		SearchCount++;  // number of nodes expanded
		// current node is the Node with minimum F value
		NodesToVisit.HeapPop(currentNode, my_Predicate(), true);
		GCost = currentNode->G;
		ClosedList.Add(currentNode);

		if (currentNode == GoalNode)
		{
			isGoldFound = true;
			break;
		}
	
		// Check the left neighbor
		// check to ensure not out of range

		if (currentNode->Y - 1 >= 0)
		{
			// get the left neighbor from the list
			tempNode = WorldArray[currentNode->X][currentNode->Y - 1];
			//Check to make sure the node hasn't been visited and is not closed (land)
			RelaxValidNodes(tempNode, currentNode, ClosedList, NodesToVisit);
		}

		// Check the top neighbor
		// check to ensure not out of range

		if (currentNode->X + 1 < MapSizeX - 1)
		{
			// get the left neighbor from the list
			tempNode = WorldArray[currentNode->X + 1][currentNode->Y];
			//Check to make sure the node hasn't been visited and is not closed (land)
			RelaxValidNodes(tempNode, currentNode, ClosedList, NodesToVisit);
		}

		// Check the right neighbor
		// check to ensure not out of range

		if (currentNode->Y + 1 < MapSizeY - 1)
		{
			// get the left neighbor from the list
			tempNode = WorldArray[currentNode->X][currentNode->Y + 1];
			//Check to make sure the node hasn't been visited and is not closed (land)
			RelaxValidNodes(tempNode, currentNode, ClosedList, NodesToVisit);
		}

		// Check the bottom neighbor
		// check to ensure not out of range

		if (currentNode->X - 1 > 0)
		{
			// get the left neighbor from the list
			tempNode = WorldArray[currentNode->X - 1][currentNode->Y];
			//Check to make sure the node hasn't been visited and is not closed (land)
			RelaxValidNodes(tempNode, currentNode, ClosedList, NodesToVisit);
		}
	}
	if (isGoldFound)
	{
		RenderPath();
		Ship->GeneratePath = false;
	}
}

// relax all nodes that are neighbors of current node and change their parent
void ALevelGenerator::RelaxValidNodes(GridNode* &tempNode, GridNode* &currentNode, TArray<GridNode*> &closedNodes,
	TArray<GridNode*> &NodesToVisit)
{
	// if already visited then return
	if (closedNodes.Contains(tempNode))
		return;

	// if not land then relax the neighbor node and assign parent
	if (tempNode->GridType != GridNode::Land)
	{
		// g cost from current node to next node
		float possible_G = tempNode->GetTravelCost() + currentNode->G;
		bool possible_G_isBetter = false;

		// if never expanded then expand node
		if (!NodesToVisit.Contains(tempNode))
		{
			tempNode->H = CalculateHeuristicDist(GoalNode, tempNode);
			NodesToVisit.HeapPush(tempNode, my_Predicate());
			possible_G_isBetter = true;
		}
		else if (possible_G < tempNode->G)
			possible_G_isBetter = true;

		// relax node and heapify to maintain heap invariant
		if (possible_G_isBetter)
		{
			tempNode->Parent = currentNode;
			tempNode->G = possible_G;
			tempNode->F = tempNode->G + tempNode->H;
			NodesToVisit.Heapify(my_Predicate());
		}
	}
}

void ALevelGenerator::RenderPath()
{
	UWorld* World = GetWorld();
	GridNode* CurrentNode = GoalNode;

	while (CurrentNode->Parent != nullptr)
	{
		FVector Position(CurrentNode->X * GRID_SIZE_WORLD, CurrentNode->Y * GRID_SIZE_WORLD, 20);
		// Spawn Path Actors
		AActor* PDActor = World->SpawnActor<AActor>(PathDisplayBlueprint, Position, FRotator::ZeroRotator);
		PathDisplayActors.Add(PDActor);

		Ship->Path.EmplaceAt(0, WorldArray[CurrentNode->X][CurrentNode->Y]);
		CurrentNode = CurrentNode->Parent;
	}
}

void ALevelGenerator::ResetPath()
{
	IsPathCalculated = false;
	SearchCount = 0;
	ResetAllNodes();

	for(int i = 0; i < PathDisplayActors.Num(); i++)
	{
		PathDisplayActors[i]->Destroy();
	}
	PathDisplayActors.Empty();

	Ship->Path.Empty();
}



