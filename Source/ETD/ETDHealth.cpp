// Fill out your copyright notice in the Description page of Project Settings.


#include "ETDHealth.h"

// Sets default values for this component's properties
UETDHealth::UETDHealth()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	DefaultHealth = 100;
	Health = DefaultHealth;


	// ...
}


// Called when the game starts
void UETDHealth::BeginPlay()
{
	Super::BeginPlay();

	AActor* Owner = GetOwner();
	if (Owner)
	{
		Owner->OnTakeAnyDamage.AddDynamic(this, &UETDHealth::TakeDamage);
	}

	// ...
	
}


void UETDHealth::TakeDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage<=0)
	{
		return;
	}

	Health = FMath::Clamp((Health-Damage), 0.0f, DefaultHealth);
}

// Called every frame
void UETDHealth::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

