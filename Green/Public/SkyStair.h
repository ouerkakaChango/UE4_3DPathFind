// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/Classes/Components/InstancedStaticMeshComponent.h"
#include "SkyStair.generated.h"

UCLASS()
class GREEN_API ASkyStair : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASkyStair();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* End;

	//Cube Ins
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UInstancedStaticMeshComponent *ISMCom;

	void CreateStair();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float objDis=2000.0f;

	//Scene Com
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* DefaultRootComponent;

	//Cube Scale
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float InsScale = 20.0f;
};
