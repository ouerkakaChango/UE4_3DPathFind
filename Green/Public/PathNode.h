// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PathNode.generated.h"

UCLASS()
class GREEN_API APathNode : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APathNode();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Scene Com
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* DefaultRootComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	APathNode* Father;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bObstacle=false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bChecked = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float G = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float H = 0.f;
};
