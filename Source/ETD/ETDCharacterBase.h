// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Character.h"
#include "ETDCharacterBase.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UStaticMeshComponent;
class UCameraShake;
class AInteractableBase;
class UCurveFloat;

UCLASS()
class ETD_API AETDCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AETDCharacterBase();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player")
	UStaticMeshComponent* StaticMeshComp;

	UPROPERTY(EditDefaultsOnly, Category = "actorToSpawn")
	TSubclassOf<AInteractableBase> actorToSpawn;



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void TurnAtRate(float Value);
	void LookUpAtRate(float Value);
	void InteractOnPressed();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	float baseTurnRate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	float baseLookUpAtRate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	float traceDistance;

	UFUNCTION(BlueprintNativeEvent)
	void TraceForward();
	void TraceForward_Implementation();

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	//Impulse
	UPROPERTY(EditAnywhere)
	float ImpulseForce;

	void FireForward();

	//radial impulse
	UPROPERTY(EditAnywhere)
	bool bAppliRadialForce;

	UPROPERTY(EditAnywhere)
	float ImpactRadius;

	UPROPERTY(EditAnywhere)
	float ImpactForce;

	TArray<FHitResult> HitActors;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UCameraShake>CamShake;

	UFUNCTION()
	void SpawnActor(FVector Loc, FRotator Rot);

	UPROPERTY(EditAnywhere, Category = "TimeLine")
	UCurveFloat* curveFloat;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void Landed(const FHitResult& Hit) override;

private:

	AActor* focusedActor;

	FVector StartScale;
	FVector TargetScale;
	FTimeline SquashTimeLine;

	UFUNCTION()
	void SquashProgress(float Value);
};
