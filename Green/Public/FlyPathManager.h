// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PathNode.h"
#include "FlyPathManager.generated.h"

UCLASS()
class GREEN_API AFlyPathManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFlyPathManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	int32 xNum;
	int32 yNum;
	int32 zNum;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Scene Com
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* DefaultRootComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Size;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float UnitSize=500.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int SearchCount = 1;

	FVector x,y,z;

	TArray<APathNode*> NodeArr;
	TArray<APathNode*> OpenList;

	void InitObstacles();

	UFUNCTION(BlueprintCallable)
	void QuestPath(FVector startPos,FVector endPos,bool& bSuccess,TArray<FVector>& path);

	APathNode* GetNodeAtPos(FVector);
	void CheckOpenList();
	void CheckNode(APathNode*);
	void CheckAndOpenAtPos(APathNode* father,FVector,float GOffset);
};
