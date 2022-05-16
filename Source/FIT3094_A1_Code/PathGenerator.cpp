/*// Fill out your copyright notice in the Description page of Project Settings.


#include "PathGenerator.h"

#include "AITestSuite/Public/AITestsCommon.h"
#include "Containers/Queue.h"

// Sets default values
PathGenerator::PathGenerator()
{
	//GenerateNodeArray();
	IsPathCalculated = false;
}

TArray<GridNode*> PathGenerator::GeneratePath()
{
	return CalculateDFS();
}

TArray<GridNode*> PathGenerator::CalculateDFS()
{
	GridNode* currentNode = nullptr;
	GridNode* tempNode = nullptr;
	bool isGoalFound = false;

	TArray<GridNode*> nodesToVisit;

	StartNode->IsChecked = true;
	nodesToVisit.Add(StartNode);

	while (nodesToVisit.Num() > 0)
	{
		SearchCount++;
		currentNode = nodesToVisit.Last();
		nodesToVisit.RemoveAt(nodesToVisit.Num() - 1);

		if (currentNode == GoalNode)
		{
			isGoalFound = true;
			break;
		}

		// Check the left neighbour
		// Check to ensure not out of range
		if (currentNode->Y - 1 > 0)
		{
			// Get the Left neighbor from the list
			tempNode = GridNodes[currentNode->X][currentNode->Y - 1];
			// Check to make sure the node hasnt been visited AND is not closed (A wall)
			if (tempNode->GridType != GridNode::Land && !tempNode->IsChecked)
			{
				tempNode->IsChecked = true;
				tempNode->Parent = currentNode;
				nodesToVisit.Add(tempNode);
			}
		}

		// Check the top neighbour
		// Check to ensure not out of range
		if (currentNode->X + 1 < GRID_SIZE)
		{
			// Get the top neighbor from the list
			tempNode = GridNodes[currentNode->X + 1][currentNode->Y];
			// Check to make sure the node hasnt been visited AND is not closed (A wall)
			if (tempNode->GridType != GridNode::Land && !tempNode->IsChecked)
			{
				tempNode->IsChecked = true;
				tempNode->Parent = currentNode;
				nodesToVisit.Add(tempNode);
			}
		}

		// Check the right neighbour
		// Check to ensure not out of range
		if (currentNode->Y + 1 < GRID_SIZE)
		{
			// Get the right neighbor from the list
			tempNode = GridNodes[currentNode->X][currentNode->Y + 1];
			// Check to make sure the node hasnt been visited AND is not closed (A wall)
			if (tempNode->GridType != GridNode::Land && !tempNode->IsChecked)
			{
				tempNode->IsChecked = true;
				tempNode->Parent = currentNode;
				nodesToVisit.Add(tempNode);
			}
		}

		// Check the bottom neighbour
		// Check to ensure not out of range
		if (currentNode->X - 1 > 0)
		{
			// Get the bottom neighbor from the list
			tempNode = GridNodes[currentNode->X - 1][currentNode->Y];
			// Check to make sure the node hasnt been visited AND is not closed (A wall)
			if (tempNode->GridType != GridNode::Land && !tempNode->IsChecked)
			{
				tempNode->IsChecked = true;
				tempNode->Parent = currentNode;
				nodesToVisit.Add(tempNode);
			}
		}
	}

	if (isGoalFound)
	{
		RenderPath();
	}

	return nodesToVisit;
}

void PathGenerator::CalculateBFS()
{
	GridNode* currentNode = nullptr;
	GridNode* tempNode = nullptr;
	bool isGoalFound = false;

	TQueue<GridNode*> nodesToVisit;

	StartNode->IsChecked = true;
	nodesToVisit.Enqueue(StartNode);

	while (!nodesToVisit.IsEmpty())
	{
		SearchCount++;
		nodesToVisit.Dequeue(currentNode);

		if (currentNode == GoalNode)
		{
			isGoalFound = true;
			break;
		}

		// Check the left neighbour
		// Check to ensure not out of range
		if (currentNode->Y - 1 > 0)
		{
			// Get the Left neighbor from the list
			tempNode = GridNodes[currentNode->X][currentNode->Y - 1];
			// Check to make sure the node hasnt been visited AND is not closed (A wall)
			if (tempNode->GridType != GridNode::Land && !tempNode->IsChecked)
			{
				tempNode->IsChecked = true;
				tempNode->Parent = currentNode;
				nodesToVisit.Enqueue(tempNode);
			}
		}

		// Check the top neighbour
		// Check to ensure not out of range
		if (currentNode->X + 1 < GRID_SIZE)
		{
			// Get the top neighbor from the list
			tempNode = GridNodes[currentNode->X + 1][currentNode->Y];
			// Check to make sure the node hasnt been visited AND is not closed (A wall)
			if (tempNode->GridType != GridNode::Land && !tempNode->IsChecked)
			{
				tempNode->IsChecked = true;
				tempNode->Parent = currentNode;
				nodesToVisit.Enqueue(tempNode);
			}
		}

		// Check the right neighbour
		// Check to ensure not out of range
		if (currentNode->Y + 1 < GRID_SIZE)
		{
			// Get the right neighbor from the list
			tempNode = GridNodes[currentNode->X][currentNode->Y + 1];
			// Check to make sure the node hasnt been visited AND is not closed (A wall)
			if (tempNode->GridType != GridNode::Land && !tempNode->IsChecked)
			{
				tempNode->IsChecked = true;
				tempNode->Parent = currentNode;
				nodesToVisit.Enqueue(tempNode);
			}
		}

		// Check the bottom neighbour
		// Check to ensure not out of range
		if (currentNode->X - 1 > 0)
		{
			// Get the bottom neighbor from the list
			tempNode = GridNodes[currentNode->X - 1][currentNode->Y];
			// Check to make sure the node hasnt been visited AND is not closed (A wall)
			if (tempNode->GridType != GridNode::Land && !tempNode->IsChecked)
			{
				tempNode->IsChecked = true;
				tempNode->Parent = currentNode;
				nodesToVisit.Enqueue(tempNode);
			}
		}
	}

	if (isGoalFound)
	{
		RenderPath();
	}
}

struct my_Predicate
{
	bool operator () (const GridNode &currentNode, const GridNode &tempNode) const
	{
		return currentNode.F < tempNode.F;
	}
};

void PathGenerator::CalculateAStar()
{
	GridNode* currentNode = nullptr;  // priority queue node with GridNode and F(x)
	GridNode* tempNode = nullptr;
	bool isGoldFound = false;

	TArray<GridNode*> NodesToVisit;
	TArray<GridNode*> ClosedList;

	StartNode->G = 0;
	StartNode->H = CalculateHeuristicDist(GoalNode, StartNode);
	StartNode->F = StartNode->G + StartNode->H;
	NodesToVisit.HeapPush(StartNode, my_Predicate());

	while(NodesToVisit.Num() > 0)
	{
		SearchCount++;
		NodesToVisit.Heapify(my_Predicate());
		NodesToVisit.HeapPop(currentNode, my_Predicate(), true);
		TotalHeuristicCost = currentNode->F;
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
			ValidateNode(tempNode, currentNode, ClosedList, NodesToVisit);
		}

		// Check the top neighbor
		// check to ensure not out of range

		if (currentNode->X + 1 < MapSizeX - 1)
		{
			// get the left neighbor from the list
			tempNode = WorldArray[currentNode->X + 1][currentNode->Y];
			//Check to make sure the node hasn't been visited and is not closed (land)
			ValidateNode(tempNode, currentNode, ClosedList, NodesToVisit);
		}

		// Check the right neighbor
		// check to ensure not out of range

		if (currentNode->Y + 1 < MapSizeY - 1)
		{
			// get the left neighbor from the list
			tempNode = WorldArray[currentNode->X][currentNode->Y + 1];
			//Check to make sure the node hasn't been visited and is not closed (land)
			ValidateNode(tempNode, currentNode, ClosedList, NodesToVisit);
		}

		// Check the bottom neighbor
		// check to ensure not out of range

		if (currentNode->X - 1 > 0)
		{
			// get the left neighbor from the list
			tempNode = WorldArray[currentNode->X - 1][currentNode->Y];
			//Check to make sure the node hasn't been visited and is not closed (land)
			ValidateNode(tempNode, currentNode, ClosedList, NodesToVisit);
		}
	}
	if (isGoldFound)
	{
		RenderPath();
		Ship->GeneratePath = false;
	}
}

void ALevelGenerator::ValidateNode(GridNode* &tempNode, GridNode* &currentNode, TArray<GridNode*> &closedNodes,
	TArray<GridNode*> &NodesToVisit)
{
	if (closedNodes.Contains(tempNode))
		return;
	if (tempNode->GridType != GridNode::Land)
	{
		float possible_G = tempNode->GetTravelCost() + currentNode->G;
		bool possible_G_isBetter = false;

		if (!NodesToVisit.Contains(tempNode))
		{
			tempNode->H = CalculateHeuristicDist(GoalNode, tempNode);
			NodesToVisit.HeapPush(tempNode, my_Predicate());
			possible_G_isBetter = true;
		}
		else if (possible_G < tempNode->G)
			possible_G_isBetter = true;

		if (possible_G_isBetter)
		{
			tempNode->Parent = currentNode;
			tempNode->G = possible_G;
			tempNode->F = tempNode->G + tempNode->H;
			NodesToVisit.Heapify(my_Predicate());
		}
	}
}


void PathGenerator::ResetPath()
{
	IsPathCalculated = false;
	SearchCount = 0;

	for (int x = 0; x < GRID_SIZE; x++)
	{
		for (int y = 0; y < GRID_SIZE; y++)
		{
			GridNodes[x][y]->IsChecked = false;
		}
	}

	// Delete All Path Actors
	for (auto Component : Path) {
		Component->Destroy();
	}

	Path.Empty();
}

void PathGenerator::RenderPath()
{
	// Spawn Path Actors
	GridNode* CurrentNode = GoalNode;

	while (CurrentNode->Parent != nullptr)
	{
		float XPos = CurrentNode->X * GRID_WORLD_SIZE + OFFSET;
		float YPos = CurrentNode->Y * GRID_WORLD_SIZE + OFFSET;
		FVector NodePos(XPos, YPos, 0);

		AActor* PathComponent = GetWorld()->SpawnActor(PathActor, &NodePos, &FRotator::ZeroRotator);
		Path.Add(PathComponent);

		CurrentNode = CurrentNode->Parent;
	}
}

float PathGenerator::CalculateDistanceBetween(GridNode* node1, GridNode* node2)
{
	FVector DistToTarget(node1->X - node2->X, node1->Y - node2->Y, 0);
	return DistToTarget.Size();
}

void PathGenerator::GenerateNodeArray()
{
	for (int x = 0; x < GRID_SIZE; x++)
	{
		for (int y = 0; y < GRID_SIZE; y++)
		{
			GridNode* NewNode = new GridNode();
			NewNode->X = x;
			NewNode->Y = y;
			NewNode->IsChecked = false;
			NewNode->GridType = GridNode::Empty;

			GridNodes[x][y] = NewNode;

			if (WallSpawner->Grid[x][y] == 1)
				NewNode->GridType = GridNode::Wall;
			else if (WallSpawner->Grid[x][y] == 2)
			{
				StartNode = NewNode;
				// Spawn Start
				if (StartActorEntity)
				{
					float XPos = x * GRID_WORLD_SIZE + OFFSET;
					float YPos = y * GRID_WORLD_SIZE + OFFSET;
					FVector NodePos(XPos, YPos, 0);
					GetWorld()->SpawnActor(StartActorEntity, &NodePos, &FRotator::ZeroRotator);
				}
			}
			else if (WallSpawner->Grid[x][y] == 3)
			{
				GoalNode = NewNode;
				// Spawn End
				if (GoalActorEntity)
				{
					float XPos = x * GRID_WORLD_SIZE + OFFSET;
					float YPos = y * GRID_WORLD_SIZE + OFFSET;
					FVector NodePos(XPos, YPos, 0);
					GetWorld()->SpawnActor(GoalActorEntity, &NodePos, &FRotator::ZeroRotator);
				}
			}
		}
	}*/