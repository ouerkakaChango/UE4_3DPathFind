// Fill out your copyright notice in the Description page of Project Settings.


#include "PalaceCpp.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/GameFramework/Character.h"

// Sets default values
APalaceCpp::APalaceCpp()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//meshCom = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshCom"));
	//ConstructorHelpers::FObjectFinder<UStaticMesh> meshAsset(TEXT("/Game/Mesh/file.file"));
	//meshCom->SetStaticMesh(meshAsset.Object);
	

	DefaultRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SetRootComponent(DefaultRootComponent);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> palaceStaticMesh(TEXT("/Game/Mesh/file.file"));
	meshCom = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshCom"));
	meshCom->SetStaticMesh(palaceStaticMesh.Object);
	//meshCom->SetCollisionProfileName(UCollisionProfile:);
	meshCom->AttachTo(DefaultRootComponent, "Mesh", EAttachLocation::KeepRelativeOffset, false);

	//IDPatch Blueprint
	static ConstructorHelpers::FObjectFinder<UBlueprint> IDPatchAsset(TEXT("/Game/Blueprint/IDPatch.IDPatch"));
	IDPatchBlueprintVar = (UClass*)IDPatchAsset.Object->GeneratedClass;

	//Cube Blueprint
	static ConstructorHelpers::FObjectFinder<UBlueprint> CubeAsset(TEXT("/Game/Blueprint/GreenCubeBP.GreenCubeBP"));
	CubeBlueprintVar = (UClass*)CubeAsset.Object->GeneratedClass;

	//instanced Cube
	static ConstructorHelpers::FObjectFinder<UStaticMesh> insCubeStaticMesh(TEXT("/Game/Mesh/InsGreenCube.InsGreenCube"));
	ISMCom = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("InsMeshCom"));
	if (insCubeStaticMesh.Object)
	{
		UE_LOG(LogTemp, Warning, TEXT("has"));
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("not"));
	}
	ISMCom->SetFlags(RF_Transactional);
	ISMCom->SetStaticMesh(insCubeStaticMesh.Object);
	AddInstanceComponent(ISMCom);

}

// Called when the game starts or when spawned
void APalaceCpp::BeginPlay()
{
	Super::BeginPlay();
	ISMCom->RegisterComponent();
	ISMCom->RegisterComponentWithWorld(GetWorld());
	GetStaticMeshVertexLocations();
	if (isUpdate)
	{
		GetWorld()->GetTimerManager().SetTimer(IDPatchTimerHandle, this, &APalaceCpp::OnUpdateNearPlayer, UpdateInterval, true);
	}
}

// Called every frame
void APalaceCpp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FVector APalaceCpp::MakeAligned(FVector inVec)
{
	float dis = alignDis;
	int outx = (inVec.X - fmodf(inVec.X, dis)) / dis;
	int outy = (inVec.Y - fmodf(inVec.Y, dis)) / dis;
	int outz = (inVec.Z - fmodf(inVec.Z, dis)) / dis;
	return FVector(outx*alignDis, outy*alignDis, outz*alignDis);
}

bool APalaceCpp::GetStaticMeshVertexLocations() 
{
	FStaticMeshLODResources* res = &meshCom->GetStaticMesh()->RenderData->LODResources[0];
	if (res)
	{
		const int32 VertexCount = res->GetNumVertices();
		UE_LOG(LogTemp, Warning, TEXT("%d"), VertexCount);

		int count = 0;
		UWorld* const World = GetWorld(); // get a reference to the world
		if (isDoFilter) {
			for (int32 Index = 0; Index < VertexCount; Index++)
			{
				FVector relativeLocation = GetTransform().TransformVector(res->VertexBuffers.PositionVertexBuffer.VertexPosition(Index));
				count++;
				if (World&&count >= InsDelta) {
					finalVertexPosArr.AddUnique(MakeAligned(relativeLocation));
					//AActor* Cube = World->SpawnActor<AActor>(IDPatchBlueprintVar, WorldSpaceVertexLocation, FRotator());
					//ISMCom->AddInstance(FTransform(FRotator(0,0,0), ReletiveLocation,FVector(InsScale, InsScale, InsScale)));
					count = 0;
				}
			}
		}
		else {
			for (int32 Index = 0; Index < VertexCount; Index++)
			{
				FVector relativeLocation = GetTransform().TransformVector(res->VertexBuffers.PositionVertexBuffer.VertexPosition(Index));
				count++;
				if (World&&count >= InsDelta) {
					finalVertexPosArr.AddUnique(relativeLocation);
					count = 0;
				}
			}
		}
		//Get Player's Position
		FVector playerLoc;
		ACharacter* myCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
		if (myCharacter) {
			playerLoc = myCharacter->GetActorLocation();
		}

		for (int32 Index = 0; Index < finalVertexPosArr.Num(); Index++)
		{
			FlagArr.Add(false);
		}
		for (int32 Index = 0; Index < finalVertexPosArr.Num(); Index++)
		{
			//First time,for those near player
			if ((GetActorLocation() + finalVertexPosArr[Index] - playerLoc).Size() <= PlayerCloseDis)
			{
				//spawn IDPatch
				World->SpawnActor<AActor>(IDPatchBlueprintVar, GetActorLocation() + finalVertexPosArr[Index]+InsScale*FVector(-50,50,50), FRotator(0,0,0));
				//spawn CubeActor
				World->SpawnActor<AActor>(CubeBlueprintVar, GetActorLocation() + finalVertexPosArr[Index], FRotator(0, 0, 0));
				FlagArr[Index] = true;
			}
			else
			{
				ISMCom->AddInstance(FTransform(FRotator(0, 0, 0), finalVertexPosArr[Index], FVector(InsScale, InsScale, InsScale)));
			}
		}

		//calcu now player posID
		playerPosIDX = int(playerLoc.X / PlayerCloseDis);
		playerPosIDY = int(playerLoc.Y / PlayerCloseDis);
	}
	else {
		return false;
	}
	return true;
}

void APalaceCpp::OnUpdateNearPlayer()
{
	ACharacter* myCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	FVector playerLoc = myCharacter->GetActorLocation();

	int orixID = playerPosIDX;
	int oriyID = playerPosIDY;
	playerPosIDX = int(playerLoc.X / PlayerCloseDis);
	playerPosIDY = int(playerLoc.Y / PlayerCloseDis);

	//TArray<INT32> nearIndexArr;
	std::map<float, int32> nearMap;
	//std::vector<std::pair<float, int32> > nearMap;
	APlayerController* mpc = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	FVector2D screenLoc;
	int32 sizex, sizey;
	mpc->GetViewportSize(sizex, sizey);

	float tdis;

	if (orixID == playerPosIDX && oriyID == playerPosIDY)
	{
		//do nothing
	}
	else
	{
		UWorld* const World = GetWorld();
		if (World)
		{
			//ISMCom->ClearInstances();
			//Record those near...
			for (int32 Index = 0; Index < finalVertexPosArr.Num(); Index++)
			{
				tdis = (GetActorLocation() + finalVertexPosArr[Index] - playerLoc).Size();
				if (tdis <= PlayerCloseDis && !FlagArr[Index])
				{
					//if in sight,add
					mpc->ProjectWorldLocationToScreen(GetActorLocation() + finalVertexPosArr[Index], screenLoc);
					//if (screenLoc.X >= 0 && screenLoc.X <= sizex && screenLoc.Y >= 0 && screenLoc.Y <= sizey)
					//{
						nearMap[tdis] = Index;
					//}
				}
			}
			auto iter = nearMap.begin();
			for (int i = 0; i < UpdateNum; i++)
			{
				iter++;
			}
			if (nearMap.size() > UpdateNum)
			{
				nearMap.erase(iter, nearMap.end());
			}
			UE_LOG(LogTemp, Warning, TEXT("%d"), nearMap.size());
			for (int32 Index = 0; Index < finalVertexPosArr.Num(); Index++)
			{
				if (MapHasValue(nearMap,Index)&&!FlagArr[Index])
				{
					World->SpawnActor<AActor>(IDPatchBlueprintVar, GetActorLocation() + finalVertexPosArr[Index] + InsScale * FVector(-50, 50, 50), FRotator(0, 0, 0));
					World->SpawnActor<AActor>(CubeBlueprintVar, GetActorLocation() + finalVertexPosArr[Index], FRotator(0, 0, 0));
					FlagArr[Index] = true;
				}
				else if(!FlagArr[Index])
				{
					//ISMCom->AddInstance(FTransform(FRotator(0, 0, 0), finalVertexPosArr[Index], FVector(InsScale, InsScale, InsScale)));
				}
			}
				
		}
	}
}


bool APalaceCpp::MapHasValue(const std::map<float, int32>& tmap,int32 val) {
	for (auto& iter : tmap) {
		if (iter.second == val) {
			return true;
		}
	}
	return false;
}