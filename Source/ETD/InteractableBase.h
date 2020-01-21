// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ETDInteractableInterface.h"
#include "Components/TimelineComponent.h"
#include "InteractableBase.generated.h"

class UCurveFloat;

UCLASS()
class ETD_API AInteractableBase : public AActor, public IETDInteractableInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInteractableBase();

	UFUNCTION()
	void TimeLineProgress(float value);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FTimeline curveTimeLine;

	UPROPERTY(EditAnywhere, Category = "TimeLine")
	UCurveFloat* curveFloat;

	UPROPERTY()
	FVector StartLoc;

	UPROPERTY()
	FVector EndLoc;

	UPROPERTY(EditAnywhere, Category = "TimeLine")
	float ZOffset;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void OnInteract(AActor* Caller);
	virtual void OnInteract_Implementation(AActor* Caller);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void StartFocus();
	virtual void StartFocus_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void EndFocus();
	virtual void EndFocus_Implementation();
};
