// Fill out your copyright notice in the Description page of Project Settings.


#include "Ship.h"

#include <string>

#include "Gold.h"
#include "LevelGenerator.h"

// Sets default values
AShip::AShip()
{
 	// Set this actor to call Tick() every frame.
	PrimaryActorTick.bCanEverTick = true;
	MoveSpeed = 200;
	Tolerance = 10;

	// instantiate a box collision
	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Box colliision"));
	BoxCollision->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AShip::BeginPlay()
{
	Super::BeginPlay();
	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &AShip::OnOverlapBegin);
}

// Called every frame
void AShip::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Morale <= 0)
	{
		// Onscreen debug
		GEngine->AddOnScreenDebugMessage(-1, 12.f, FColor::Red,
			FString::Printf(TEXT("You ship has undergone a Mutiny! Your captain has been forced to walk the plank")));
		//log debug message
		UE_LOG(LogTemp, Warning, TEXT("You ship has undergone a Mutiny! Your captain has been forced to walk the plank"));
	}
	
	if (Path.Num() > 0)
	{
		FVector CurrentPosition = GetActorLocation();
		float TargetXPos = Path[0]->X * ALevelGenerator::GRID_SIZE_WORLD;
		float TargetYPos = Path[0]->Y * ALevelGenerator::GRID_SIZE_WORLD;

		FVector TargetPosition(TargetXPos, TargetYPos, CurrentPosition.Z);

		FVector Direction = TargetPosition - CurrentPosition;
		Direction.Normalize();

		CurrentPosition += Direction * MoveSpeed * DeltaTime;
		SetActorLocation(CurrentPosition);

		// Rotating the ship to face the heading direction
		Heading = TargetPosition - CurrentPosition;
		SetActorRotation(FMath::RInterpTo(GetActorRotation(), (Heading).Rotation(), DeltaTime, 10));

		if (FVector::Dist(CurrentPosition, TargetPosition) <= Tolerance)
		{
			CurrentPosition = TargetPosition;
			Level->StartNode = Path[0];
			Path.RemoveAt(0, 1, true);
			// decrease morale
			Morale -= 1;
		}
	}
}

// called when the box component is overlapped
void AShip::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Other->IsA(Level->GoldBlueprint))
	{
	    // Log Morale
		UE_LOG(LogTemp, Warning, TEXT("Morale Check: %d"), Morale);
		GEngine->AddOnScreenDebugMessage(-1, 12.f, FColor::White, FString::Printf(TEXT("Morale Check %d"), Morale));
		GeneratePath = true;
		Morale = 50;
	}
}
