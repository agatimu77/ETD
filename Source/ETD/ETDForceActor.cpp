// Fill out your copyright notice in the Description page of Project Settings.


#include "ETDForceActor.h"

// Sets default values
AETDForceActor::AETDForceActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComp"));
	StaticMeshComp->SetupAttachment(RootComponent);

	forceAmount = 1000.0f;

}

// Called when the game starts or when spawned
void AETDForceActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AETDForceActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FVector up = StaticMeshComp->GetUpVector();
	StaticMeshComp->AddForce(up * forceAmount * StaticMeshComp->GetMass());
}

