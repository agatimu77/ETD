// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractableBase.h"
#include "Components/TimelineComponent.h"


// Sets default values
AInteractableBase::AInteractableBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void AInteractableBase::TimeLineProgress(float value)
{
	FVector newLoc = FMath::Lerp(StartLoc, EndLoc, value);
	SetActorLocation((newLoc));
}

// Called when the game starts or when spawned
void AInteractableBase::BeginPlay()
{
	Super::BeginPlay();
	
	if (curveFloat)
	{
		FOnTimelineFloatStatic timeLineProgress;
		timeLineProgress.BindUFunction(this, FName("TimeLineProgress"));
		curveTimeLine.AddInterpFloat(curveFloat, timeLineProgress);
		curveTimeLine.SetLooping(true);

		StartLoc = EndLoc = GetActorLocation();
		EndLoc.Z += ZOffset;

		curveTimeLine.PlayFromStart();
	}
}

// Called every frame
void AInteractableBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	curveTimeLine.TickTimeline(DeltaTime);

}

void AInteractableBase::OnInteract_Implementation(AActor* Caller)
{
	Destroy();
}

void AInteractableBase::StartFocus_Implementation()
{
	if (GEngine)
	{
		//GEngine->AddOnScreenDebugMessage (- 1, 15.0f, FColor::Green, TEXT("StartFocus"));
	}
	
}

void AInteractableBase::EndFocus_Implementation()
{
	if (GEngine)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("EndFocus"));
	}
}

