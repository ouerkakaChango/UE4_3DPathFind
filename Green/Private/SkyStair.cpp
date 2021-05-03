// Fill out your copyright notice in the Description page of Project Settings.


#include "SkyStair.h"

// Sets default values
ASkyStair::ASkyStair()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DefaultRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SetRootComponent(DefaultRootComponent);

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
void ASkyStair::BeginPlay()
{
	Super::BeginPlay();
	ISMCom->RegisterComponent();
	ISMCom->RegisterComponentWithWorld(GetWorld());
	CreateStair();
}

// Called every frame
void ASkyStair::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASkyStair::CreateStair()
{
	FVector dis= End->GetActorLocation()- GetActorLocation();
	float disLen = dis.Size();
	float d = 0.0f;
	FVector t= FVector(0,0,0);
	float f1 = dis.X > 0 ? 1 : -1;
	float f2 = dis.Y > 0 ? 1 : -1;
	float f3 = dis.Z > 0 ? 1 : -1;
	for (; d < disLen; d += objDis * 1.414f)
	{
		t += FVector(objDis*f1, objDis*f2, objDis*f3);
		ISMCom->AddInstance(FTransform(FRotator(0, 0, 0), t, FVector(InsScale, InsScale, InsScale)));
	}
}