// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/Classes/Components/InstancedStaticMeshComponent.h"
#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include "PalaceCpp.generated.h"

UCLASS()
class GREEN_API APalaceCpp : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APalaceCpp();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//
	FVector MakeAligned(FVector);

	/* Handle to manage the timer */
	FTimerHandle IDPatchTimerHandle;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	bool GetStaticMeshVertexLocations();

	//PalaceSM Com
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* meshCom;

	//Scene Com
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* DefaultRootComponent;

	//Cube Ins
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UInstancedStaticMeshComponent *ISMCom;

	//Cube Ins Del
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 InsDelta=1;

	//Cube Scale
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float InsScale = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 alignDis = 20;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isDoFilter = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PlayerCloseDis = 30000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float UpdateInterval = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isUpdate = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 UpdateNum = 40;

	UFUNCTION()
	void OnUpdateNearPlayer();

	TArray<FVector> finalVertexPosArr;

	//IDPatch Blueprint
	TSubclassOf<AActor> IDPatchBlueprintVar;

	//Cube Blueprint
	TSubclassOf<AActor> CubeBlueprintVar;

	TArray<bool> FlagArr;

	int32 playerPosIDX, playerPosIDY;

	bool MapHasValue(const std::map<float, int32>&, int32);
};