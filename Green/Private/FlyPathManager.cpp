// Fill out your copyright notice in the Description page of Project Settings.


#include "FlyPathManager.h"
#include "DrawDebugHelpers.h"
#include "Algo/Reverse.h"

// Sets default values
AFlyPathManager::AFlyPathManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DefaultRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SetRootComponent(DefaultRootComponent);

}

// Called when the game starts or when spawned
void AFlyPathManager::BeginPlay()
{
	Super::BeginPlay();
	x = DefaultRootComponent->GetForwardVector();
	y = DefaultRootComponent->GetRightVector();
	z = DefaultRootComponent->GetUpVector();
	
	xNum = Size.X / UnitSize;
	yNum = Size.Y / UnitSize;
	zNum = Size.Z / UnitSize;

	FVector tpos,td;
	APathNode* tnode;
	for (int32 ix = 0; ix < xNum; ix++) {
		for (int32 iy = 0; iy < yNum; iy++) {
			for (int32 iz = 0; iz < zNum; iz++) {
				tpos = GetActorLocation() + ix * UnitSize*x + iy * UnitSize*y + iz * UnitSize*z;
				tnode = GetWorld()->SpawnActor<APathNode>(tpos, FRotator(0, 0, 0));
				NodeArr.Add(tnode);
				DrawDebugPoint(
					GetWorld(),
					tpos,
					10,  					//size
					FColor(40, 40, 40),  //pink
					true,  				//persistent (never goes away)
					0.03 					//point leaves a trail on moving object
				);
			}
		}
	}
	/*
	DrawDebugPoint(
		GetWorld(),
		Location,
		20,  					//size
		FColor(255, 0, 255),  //pink
		false,  				//persistent (never goes away)
		0.03 					//point leaves a trail on moving object
	);
	*/
	InitObstacles();
	//QuestPath(FVector(-500,3000,0), FVector(0, -3000, 0));
}

void AFlyPathManager::InitObstacles()
{
	FVector ori = FVector(-1000, -1000, -1000);
	FVector tpos;
	APathNode* tnode;
	for (int32 ix = 0; ix < 4; ix++) {
		for (int32 iy = 0; iy < 4; iy++) {
			for (int32 iz = 0; iz < 6; iz++) {
				tpos = ori+ ix * UnitSize*x + iy * UnitSize*y + iz * UnitSize*z;
				tnode = GetNodeAtPos(tpos);
				tnode->bObstacle = true;
				DrawDebugPoint(
					GetWorld(),
					tpos,
					10,  					//size
					FColor(0, 0, 255),  //pink
					true,  				//persistent (never goes away)
					0.03 					//point leaves a trail on moving object
				);
			}
		}
	}
}

// Called every frame
void AFlyPathManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

APathNode* AFlyPathManager::GetNodeAtPos(FVector pos)
{
	FVector index = (pos - GetActorLocation()) / UnitSize;
	//UE_LOG(LogTemp, Warning, TEXT("index1 %f %f %f"),index1.X,index1.Y,index1.Z);
	return NodeArr[index.Z + index.Y*zNum + index.X*(zNum*yNum)];
}

void AFlyPathManager::CheckOpenList()
{
	CheckNode(OpenList[0]);
	//Swap best to front
	float tminF = OpenList[0]->G + OpenList[0]->H;
	float tdis;
	int tminIndex = 0;
	for (int32 i = 0; i < OpenList.Num(); i++)
	{
		tdis = OpenList[i]->G + OpenList[i]->H;
		if (tdis < tminF)
		{
			tminIndex = i;
			tminF = tdis;
		}
	}
	if (tminIndex != 0)
	{
		OpenList.Swap(0, tminIndex);
	}
}

void AFlyPathManager::CheckAndOpenAtPos(APathNode* father,FVector pos, float GOffset)
{
	FVector index = (pos - GetActorLocation()) / UnitSize;
	if (index.X < 0 || index.X >= xNum || index.Y < 0 || index.Y >= yNum || index.Z < 0 || index.Z >= zNum)
	{
		return;
	}
	else
	{
		APathNode* node = NodeArr[index.Z + index.Y*zNum + index.X*(zNum*yNum)];
		if (node->bObstacle||node->bChecked)
		{
			return;
		}
		else
		{
			//if not in OpenList
			if (OpenList.Find(node)==INDEX_NONE)
			{
				node->Father = father;
				node->bChecked = true;
				node->G = father->G+GOffset;
				OpenList.Add(node);
				DrawDebugPoint(
					GetWorld(),
					node->GetActorLocation(),
					10,  					//size
					FColor(0, 255, 0),  //pink
					true,  				//persistent (never goes away)
					0.03 					//point leaves a trail on moving object
				);
			}
			//### if has been in OpenList,check which G smaller
			else
			{
				if (father->G + GOffset < node->G)
				{
					node->G = father->G + GOffset;
					node->Father = father;
				}
			}
		}
	}
}

void AFlyPathManager::CheckNode(APathNode* node)
{
	OpenList.Remove(node);
	FVector pos = node->GetActorLocation();
	//6 dir neighbour(axis cube)(zhou kuai)
	CheckAndOpenAtPos(node,pos + x * UnitSize,UnitSize);
	CheckAndOpenAtPos(node,pos - x * UnitSize,UnitSize);
	CheckAndOpenAtPos(node,pos + y * UnitSize,UnitSize);
	CheckAndOpenAtPos(node,pos - y * UnitSize,UnitSize);
	CheckAndOpenAtPos(node,pos + z * UnitSize,UnitSize);
	CheckAndOpenAtPos(node,pos - z * UnitSize,UnitSize);
	//12 1.414 neighbour(edge cube)(leng kuai)
	CheckAndOpenAtPos(node,pos + x * UnitSize + y * UnitSize,UnitSize*1.414f);
	CheckAndOpenAtPos(node,pos + x * UnitSize - y * UnitSize,UnitSize*1.414f);
	CheckAndOpenAtPos(node,pos - x * UnitSize + y * UnitSize,UnitSize*1.414f);
	CheckAndOpenAtPos(node,pos - x * UnitSize - y * UnitSize,UnitSize*1.414f);

	CheckAndOpenAtPos(node,pos + x * UnitSize + z * UnitSize,UnitSize*1.414f);
	CheckAndOpenAtPos(node,pos + x * UnitSize - z * UnitSize,UnitSize*1.414f);
	CheckAndOpenAtPos(node,pos - x * UnitSize + z * UnitSize,UnitSize*1.414f);
	CheckAndOpenAtPos(node,pos - x * UnitSize - z * UnitSize,UnitSize*1.414f);

	CheckAndOpenAtPos(node,pos + y * UnitSize + z * UnitSize,UnitSize*1.414f);
	CheckAndOpenAtPos(node,pos + y * UnitSize - z * UnitSize,UnitSize*1.414f);
	CheckAndOpenAtPos(node,pos - y * UnitSize + z * UnitSize,UnitSize*1.414f);
	CheckAndOpenAtPos(node,pos - y * UnitSize - z * UnitSize,UnitSize*1.414f);
	//8 1.732 neighbour(corner cube)(jiao kuai)
	CheckAndOpenAtPos(node,pos + x * UnitSize + y * UnitSize + z * UnitSize,UnitSize*1.732f);
	CheckAndOpenAtPos(node,pos + x * UnitSize + y * UnitSize - z * UnitSize,UnitSize*1.732f);
	CheckAndOpenAtPos(node,pos + x * UnitSize - y * UnitSize + z * UnitSize,UnitSize*1.732f);
	CheckAndOpenAtPos(node,pos - x * UnitSize + y * UnitSize + z * UnitSize,UnitSize*1.732f);
	CheckAndOpenAtPos(node,pos - x * UnitSize - y * UnitSize + z * UnitSize,UnitSize*1.732f);
	CheckAndOpenAtPos(node,pos - x * UnitSize + y * UnitSize - z * UnitSize,UnitSize*1.732f);
	CheckAndOpenAtPos(node,pos + x * UnitSize - y * UnitSize - z * UnitSize,UnitSize*1.732f);
	CheckAndOpenAtPos(node,pos - x * UnitSize - y * UnitSize - z * UnitSize,UnitSize*1.732f);
}

void AFlyPathManager::QuestPath(FVector startPos, FVector endPos, bool& bSuccess, TArray<FVector>& path)
{
	DrawDebugPoint(
		GetWorld(),
		startPos,
		10,  					//size
		FColor(255, 0, 0),  //pink
		true,  				//persistent (never goes away)
		0.03 					//point leaves a trail on moving object
	);
	DrawDebugPoint(
		GetWorld(),
		endPos,
		10,  					//size
		FColor(255, 0, 0),  //pink
		true,  				//persistent (never goes away)
		0.03 					//point leaves a trail on moving object
	);
	APathNode* startNode = GetNodeAtPos(startPos);
	APathNode* endNode = GetNodeAtPos(endPos);

	APathNode* tnode;
	FVector td;
	for (int32 ix = 0; ix < xNum; ix++) {
		for (int32 iy = 0; iy < yNum; iy++) {
			for (int32 iz = 0; iz < zNum; iz++) {
				tnode = NodeArr[iz + iy * zNum + ix * (zNum*yNum)];
				td = endPos - tnode->GetActorLocation();
				//!!! Change H calculate here
				//tnode->H = abs(td.X) + abs(td.Y) + abs(td.Z);
				tnode->H = td.Size();
			}
		}
	}
	startNode->bChecked = true;
	OpenList.Add(startNode);

	while (OpenList.Find(endNode)==INDEX_NONE||SearchCount>=NodeArr.Num())
	{
		CheckOpenList();
	}
	if (SearchCount >= NodeArr.Num() && OpenList.Find(endNode) == INDEX_NONE)
	{
		bSuccess = false;
	}
	else
	{
		tnode = endNode;
		while (tnode->Father != startNode)
		{
			tnode = tnode->Father;
			path.Add(tnode->GetActorLocation());
			DrawDebugPoint(
				GetWorld(),
				tnode->GetActorLocation(),
				10,  					//size
				FColor(255, 255, 0),  //pink
				true,  				//persistent (never goes away)
				0.03 					//point leaves a trail on moving object
			);
		}
		Algo::Reverse(path);
		bSuccess = true;
	}
}