// Fill out your copyright notice in the Description page of Project Settings.


#include "ETDCharacterBase.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/Controller.h"
#include "DrawDebugHelpers.h"
#include "ETDInteractableInterface.h"
#include "InteractableBase.h"
#include "Components/TimelineComponent.h"


// Sets default values
AETDCharacterBase::AETDCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->TargetArmLength = 400;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp);

	StaticMeshComp=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComp"));
	StaticMeshComp->SetupAttachment(RootComponent);

	baseTurnRate = 45.0f;
	baseLookUpAtRate = 45.0f;
	traceDistance = 2000;

	ImpulseForce = 500.0f;

	bAppliRadialForce = true;
	ImpactRadius = 200.0f;
	ImpactForce = 2000.0f;

	StartScale = FVector(1.0f, 1.0f, 1.0f);
	TargetScale = FVector(1.3f, 1.3f, 0.8f);

}

// Called when the game starts or when spawned
void AETDCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	StaticMeshComp->OnComponentBeginOverlap.AddDynamic(this, &AETDCharacterBase::OnOverlapBegin);
	
}

void AETDCharacterBase::MoveForward(float Value)
{
	if ((Controller)&&Value!=0.0f)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRot(0, Rotation.Yaw, 0);
		const FVector Dir = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
		AddMovementInput(Dir, Value);
	}
}

void AETDCharacterBase::MoveRight(float Value)
{
	if ((Controller) && Value != 0.0f)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRot(0, Rotation.Yaw, 0);
		const FVector Dir = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);
		AddMovementInput(Dir, Value);
	}
}

void AETDCharacterBase::TurnAtRate(float Value)
{
	AddControllerYawInput(Value * baseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AETDCharacterBase::LookUpAtRate(float Value)
{
	AddControllerPitchInput(Value * baseLookUpAtRate * GetWorld()->GetDeltaSeconds());
}

void AETDCharacterBase::InteractOnPressed()
{
	TraceForward();

	if (focusedActor)
	{
		IETDInteractableInterface* interactableInterface = Cast<IETDInteractableInterface>(focusedActor);
		if (interactableInterface)
		{
			interactableInterface->Execute_OnInteract(focusedActor,this);
		}
	}
}

void AETDCharacterBase::TraceForward_Implementation()
{
	FVector Pos;
	FRotator Rot;
	FHitResult Hit;

	GetController()->GetPlayerViewPoint(Pos, Rot);

	FVector Start = Pos;
	FVector End = Start + Rot.Vector() * traceDistance;

	FCollisionQueryParams TraceParams;

	bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, TraceParams);
	//DrawDebugLine(GetWorld(), Start, End, FColor::Orange, false, 2.0f);

	if (bHit)
	{
		//DrawDebugBox(GetWorld(), Hit.ImpactPoint, FVector(5, 5, 5), FColor::Green, false, 2.0f);
	}

	AActor* Interactable = Hit.GetActor();

	if (Interactable)
	{
		if (Interactable!=focusedActor)
		{
			if (focusedActor)
			{
				IETDInteractableInterface* interactableInterface = Cast<IETDInteractableInterface>(focusedActor);
				if (interactableInterface)
				{
					interactableInterface->Execute_EndFocus(focusedActor);
				}
			}
			IETDInteractableInterface* interactableInterface = Cast<IETDInteractableInterface>(Interactable);
			if (interactableInterface)
			{
				interactableInterface->Execute_StartFocus(Interactable);
			}
			focusedActor = Interactable;
		}
	}
	else
	{
		if (focusedActor)
		{
			IETDInteractableInterface* interactableInterface = Cast<IETDInteractableInterface>(focusedActor);
			if (interactableInterface)
			{
				interactableInterface->Execute_EndFocus(focusedActor);
			}
		}
		focusedActor = nullptr;
	}
}

void AETDCharacterBase::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	IETDInteractableInterface* interactableInterface = Cast<IETDInteractableInterface>(OtherActor);
	if (interactableInterface)
	{
		interactableInterface->Execute_OnInteract(OtherActor, this);
	}
}

void AETDCharacterBase::FireForward()
{
	GetWorld()->GetFirstPlayerController()->PlayerCameraManager->PlayCameraShake(CamShake);
	
	FVector Pos;
	FRotator Rot;
	FHitResult Hit;

	GetController()->GetPlayerViewPoint(Pos, Rot);

	FVector Start = Pos;
	FVector End = Start + Rot.Vector() * traceDistance;

	FCollisionQueryParams TraceParams;

	bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, TraceParams);

	if (bHit)
	{
		SpawnActor(Hit.Location, Rot);
		if (Hit.Actor->IsRootComponentMovable())
		{
			UStaticMeshComponent* meshComp = Cast<UStaticMeshComponent>(Hit.GetActor()->GetRootComponent());
			if (meshComp)
			{
				FVector CameraForward = CameraComp->GetForwardVector();
				meshComp->AddImpulse(CameraForward * ImpulseForce * meshComp->GetMass());
			}
		}

		if (bAppliRadialForce)
		{
			FCollisionShape sphereCol = FCollisionShape::MakeSphere(ImpactRadius);
			bool bSweepHit = GetWorld()->SweepMultiByChannel(HitActors, Hit.Location, Hit.Location + FVector(0.01f, 0.01f, 0.01f), FQuat::Identity, ECC_WorldStatic, sphereCol);
			DrawDebugSphere(GetWorld(), Hit.Location, ImpactRadius, 50, FColor::Green, false, 2.0f);
			if (bSweepHit)
			{
				for (auto& HitResult : HitActors)
				{
					UStaticMeshComponent* meshComp = Cast<UStaticMeshComponent>(HitResult.GetActor()->GetRootComponent());
					if (meshComp)
					{
						meshComp->AddRadialImpulse(HitResult.Location, ImpactRadius, ImpactForce, ERadialImpulseFalloff::RIF_Constant, true);
					}
				}
			}
		}
	}
}

void AETDCharacterBase::SpawnActor(FVector Loc, FRotator Rot)
{
	FActorSpawnParameters spawnParams;
	AInteractableBase* refToSpawned = GetWorld()->SpawnActor<AInteractableBase>(actorToSpawn, Loc, Rot, spawnParams);
}

// Called every frame
void AETDCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TraceForward();
	SquashTimeLine.TickTimeline(DeltaTime);

}

// Called to bind functionality to input
void AETDCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed , this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AETDCharacterBase::InteractOnPressed);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AETDCharacterBase::FireForward);

	PlayerInputComponent->BindAxis("MoveForward", this, &AETDCharacterBase::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AETDCharacterBase::MoveRight);

	PlayerInputComponent->BindAxis("TurnAtRate", this, &AETDCharacterBase::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpAtRate", this, &AETDCharacterBase::LookUpAtRate);
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

}

void AETDCharacterBase::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	if (curveFloat)
	{
		FOnTimelineFloatStatic TimeLineFloat;
		TimeLineFloat.BindUFunction(this, FName("SquashProgress"));
		SquashTimeLine.AddInterpFloat(curveFloat, TimeLineFloat);
		SquashTimeLine.SetLooping(false);
		SquashTimeLine.PlayFromStart();
	}
}

void AETDCharacterBase::SquashProgress(float Value)
{
	FVector newScale = FMath::Lerp(StartScale, TargetScale, Value);
	StaticMeshComp->SetWorldScale3D(newScale);
}

