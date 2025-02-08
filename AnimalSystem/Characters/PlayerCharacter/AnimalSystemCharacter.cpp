#include "AnimalSystem/Characters/PlayerCharacter/AnimalSystemCharacter.h"
#include "AnimalSystem/Characters/Animals/BaseAnimal.h"
#include "AnimalSystem/Characters/Animals/Rabbit.h"
#include "AnimalSystem/Characters/Animals/Rat.h"
#include "AnimalSystem/Characters/Animals/Squirrel.h"
#include "AnimalSystem/Manager/AnimalManager.h"
#include "AnimalSystem/UI/Widgets/PoolCountWidget.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Actor.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "UObject/Linker.h"

AAnimalSystemCharacter::AAnimalSystemCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
}

void AAnimalSystemCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &AAnimalSystemCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AAnimalSystemCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AAnimalSystemCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AAnimalSystemCharacter::LookUpAtRate);

	PlayerInputComponent->BindTouch(IE_Pressed, this, &AAnimalSystemCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AAnimalSystemCharacter::TouchStopped);

	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AAnimalSystemCharacter::OnResetVR);

	PlayerInputComponent->BindAction("PerformRaycast", IE_Pressed, this, &AAnimalSystemCharacter::PerformRaycast);
	PlayerInputComponent->BindAction("SpawnRabbitButton", IE_Pressed, this, &AAnimalSystemCharacter::SpawnRabbitOnKeyPress);
	PlayerInputComponent->BindAction("SpawnRatButton", IE_Pressed, this, &AAnimalSystemCharacter::SpawnRatOnKeyPress);
	PlayerInputComponent->BindAction("SpawnSquirrelButton", IE_Pressed, this, &AAnimalSystemCharacter::SpawnSquirrelOnKeyPress);

}

void AAnimalSystemCharacter::BeginPlay()
{
	Super::BeginPlay();

	AAnimalManager* AnimalManager = Cast<AAnimalManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AAnimalManager::StaticClass()));
	RabbitBlueprintClass = AnimalManager->RabbitClass;
	RatBlueprintClass = AnimalManager->RatClass;
	SquirrelBlueprintClass = AnimalManager->SquirrelClass;

	if (PoolCountWidgetClass)
	{
		PoolCountWidget_BP = CreateWidget<UPoolCountWidget>(GetWorld(), PoolCountWidgetClass);
		if (PoolCountWidget_BP)
		{
			PoolCountWidget_BP->AddToViewport();
			PoolCountWidget_BP->UpdateRabbitStats(0, 0);
			PoolCountWidget_BP->UpdateRatStats(0, 0);
			PoolCountWidget_BP->UpdateSquirrelStats(0, 0);
			PoolCountWidget_BP->UpdateTotalStats(0, 0);
		}
	}
}

void AAnimalSystemCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	DetectObjectsInVision();
	DrawVision();

	if (PoolCountWidget_BP)
	{
		UpdateRabbitStatsOnWidget();
		UpdateRatStatsOnWidget();
		UpdateSquirrelStatsOnWidget();
		UpdateAnimalStatsOnWidget();

		if (PoolCountWidget_BP)
		{
			AAnimalPool* AnimalPool = Cast<AAnimalPool>(UGameplayStatics::GetActorOfClass(GetWorld(), AAnimalPool::StaticClass()));
			if (AnimalPool)
			{
				int32 activeRabbits_ = AnimalPool->ActiveRabbits.Num();
				int32 rabbitsInPool_ = AnimalPool->RabbitPool.Num();
				PoolCountWidget_BP->UpdateRabbitStats(activeRabbits_, rabbitsInPool_);

				int32 activeRats_ = AnimalPool->ActiveRats.Num();
				int32 ratsInPool_ = AnimalPool->RatPool.Num();
				PoolCountWidget_BP->UpdateRatStats(activeRats_, ratsInPool_);

				int32 activeSquirrels_ = AnimalPool->ActiveSquirrels.Num();
				int32 squirrelsInPool_ = AnimalPool->SquirrelPool.Num();
				PoolCountWidget_BP->UpdateSquirrelStats(activeSquirrels_, squirrelsInPool_);

				int32 totalActiveAnimals_ = activeRabbits_ + activeRats_ + activeSquirrels_;
				int32 totalAnimalsInPool_ = rabbitsInPool_ + ratsInPool_ + squirrelsInPool_;
				PoolCountWidget_BP->UpdateTotalStats(totalActiveAnimals_, totalAnimalsInPool_);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("One or more animal pools are not initialized in AnimalManager!"));
			}
		}
	}
}

void AAnimalSystemCharacter::UpdateAnimalStatsOnWidget()
{
	if (PoolCountWidget_BP)
	{
		AAnimalPool* AnimalPool = Cast<AAnimalPool>(UGameplayStatics::GetActorOfClass(GetWorld(), AAnimalPool::StaticClass()));
		if (AnimalPool)
		{
			int32 activeRabbits_ = AnimalPool->ActiveRabbits.Num();
			int32 rabbitsInPool_ = AnimalPool->RabbitPool.Num();
			PoolCountWidget_BP->UpdateRabbitStats(activeRabbits_, rabbitsInPool_);

			int32 activeRats_ = AnimalPool->ActiveRats.Num();
			int32 ratsInPool_ = AnimalPool->RatPool.Num();
			PoolCountWidget_BP->UpdateRatStats(activeRats_, ratsInPool_);

			int32 activeSquirrels_ = AnimalPool->ActiveSquirrels.Num();
			int32 squirrelsInPool_ = AnimalPool->SquirrelPool.Num();
			PoolCountWidget_BP->UpdateSquirrelStats(activeSquirrels_, squirrelsInPool_);

			int32 totalActiveAnimals_ = activeRabbits_ + activeRats_ + activeSquirrels_;
			int32 totalAnimalsInPool_ = rabbitsInPool_ + ratsInPool_ + squirrelsInPool_;
			PoolCountWidget_BP->UpdateTotalStats(totalActiveAnimals_, totalAnimalsInPool_);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("One or more animal pools are not initialized in AnimalManager!"));
		}
	}
}

void AAnimalSystemCharacter::PerformRaycast()
{
	if (HasAuthority())
	{
		Server_PerformRaycast();
	}
	else
	{
		Server_PerformRaycast();
	}
}

void AAnimalSystemCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAnimalSystemCharacter, BaseTurnRate);
	DOREPLIFETIME(AAnimalSystemCharacter, BaseLookUpRate);
}

void AAnimalSystemCharacter::Server_PerformRaycast_Implementation()
{
	if (!HasAuthority())
	{
		Server_PerformRaycast();
		return;
	}

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		FVector Start = FollowCamera->GetComponentLocation() - FVector::UpVector * 5.0f;
		FVector End = Start + (FollowCamera->GetForwardVector() * 10000.0f);

		FHitResult HitResult;
		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(this);

		bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionParams);
		DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 1.0f, 0, 1.0f);

		if (bHit && HitResult.GetActor())
		{
			ABaseAnimal* HitAnimal = Cast<ABaseAnimal>(HitResult.GetActor());
			if (HitAnimal)
			{
				HitAnimal->TakeDamage(5.0f, FDamageEvent(), GetController(), this);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Hit: %s"), *HitResult.GetActor()->GetName());
			}
		}
	}
}

bool AAnimalSystemCharacter::Server_PerformRaycast_Validate()
{
	return true;
}

void AAnimalSystemCharacter::Multicast_DrawRay_Implementation(FVector Start, FVector End, FVector TargetPoint)
{
	DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 1.0f, 0, 1.0f);
	DrawDebugSphere(GetWorld(), TargetPoint, 5.0f, 12, FColor::Blue, false, 5.0f);
}

void AAnimalSystemCharacter::SpawnRabbitOnKeyPress()
{
	SpawnAnimal(RabbitBlueprintClass);
}

void AAnimalSystemCharacter::SpawnRatOnKeyPress()
{
	SpawnAnimal(RatBlueprintClass);
}

void AAnimalSystemCharacter::SpawnSquirrelOnKeyPress()
{
	SpawnAnimal(SquirrelBlueprintClass);
}

void AAnimalSystemCharacter::SpawnAnimal(TSubclassOf<ABaseAnimal> AnimalClass)
{
	AAnimalManager* AnimalManager = Cast<AAnimalManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AAnimalManager::StaticClass()));
	if (AnimalManager && AnimalClass)
	{
		AnimalManager->SpawnNewAnimal(AnimalClass);
	}
}

void AAnimalSystemCharacter::UpdateRabbitStatsOnWidget()
{
	UpdateAnimalStatsOnWidget();
}

void AAnimalSystemCharacter::UpdateRatStatsOnWidget()
{
	UpdateAnimalStatsOnWidget();
}

void AAnimalSystemCharacter::UpdateSquirrelStatsOnWidget()
{
	UpdateAnimalStatsOnWidget();
}

FVector AAnimalSystemCharacter::GetEyeLocation() const
{
	return FollowCamera->GetComponentLocation();
}

FVector AAnimalSystemCharacter::GetEyeDirection() const
{
	return FollowCamera->GetForwardVector();
}

void AAnimalSystemCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AAnimalSystemCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	Jump();
}

void AAnimalSystemCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	StopJumping();
}

void AAnimalSystemCharacter::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AAnimalSystemCharacter::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AAnimalSystemCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AAnimalSystemCharacter::MoveRight(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}

void AAnimalSystemCharacter::DetectObjectsInVision()
{
	FVector EyeLocation;
	FRotator EyeRotation;
	GetActorEyesViewPoint(EyeLocation, EyeRotation);

	FVector ForwardVector = EyeRotation.Vector();
	FVector LeftBoundary = ForwardVector.RotateAngleAxis(-VisionAngle / 3.0f, FVector::UpVector);
	FVector RightBoundary = ForwardVector.RotateAngleAxis(VisionAngle / 3.0f, FVector::UpVector);

	TArray<FHitResult> HitResults;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	bool bHit = GetWorld()->SweepMultiByChannel(
		HitResults,
		EyeLocation + FVector(0, 0, VisionHeightOffset),
		EyeLocation + ForwardVector * VisionDistance,
		FQuat::Identity,
		ECC_Visibility,
		FCollisionShape::MakeSphere(VisionDistance),
		QueryParams
	);

	TArray<AActor*> VisibleActors;

	if (bHit)
	{
		for (const FHitResult& Hit : HitResults)
		{
			AActor* HitActor = Hit.GetActor();
			if (HitActor)
			{
				FVector DirectionToActor = (HitActor->GetActorLocation() - EyeLocation).GetSafeNormal();
				float DotProduct = FVector::DotProduct(ForwardVector, DirectionToActor);
				float Angle = FMath::RadiansToDegrees(FMath::Acos(DotProduct));

				if (Angle <= VisionAngle / 2.0f)
				{
					VisibleActors.Add(HitActor);
				}
			}
		}
	}

	DisplayVisibleActors(VisibleActors);
}

void AAnimalSystemCharacter::DisplayVisibleActors(const TArray<AActor*>& VisibleActors)
{
	FString ActorNames;
	for (AActor* Actor : VisibleActors)
	{
		ActorNames += Actor->GetName() + TEXT("\n");
	}

	GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::White, ActorNames);
}

void AAnimalSystemCharacter::DrawVision()
{
	FVector EyeLocation;
	FRotator EyeRotation;
	GetActorEyesViewPoint(EyeLocation, EyeRotation);

	FVector ForwardVector = EyeRotation.Vector();

	FVector LeftBoundary = ForwardVector.RotateAngleAxis(-VisionAngle / 2.0f, FVector::UpVector) * VisionDistance;
	FVector RightBoundary = ForwardVector.RotateAngleAxis(VisionAngle / 2.0f, FVector::UpVector) * VisionDistance;

	DrawDebugLine(GetWorld(), EyeLocation, EyeLocation + LeftBoundary, FColor::Orange, false, -1.0f, 0, 2.0f);
	DrawDebugLine(GetWorld(), EyeLocation, EyeLocation + RightBoundary, FColor::Orange, false, -1.0f, 0, 2.0f);

	for (int32 i = 0; i <= 64; i++)
	{
		float Angle1 = (-VisionAngle / 2.0f) + (VisionAngle / 64) * i;
		float Angle2 = Angle1 + (VisionAngle / 64);

		FVector Point1 = EyeLocation + ForwardVector.RotateAngleAxis(Angle1, FVector::UpVector) * VisionDistance;
		FVector Point2 = EyeLocation + ForwardVector.RotateAngleAxis(Angle2, FVector::UpVector) * VisionDistance;

		DrawDebugLine(GetWorld(), Point1, Point2, FColor::Orange, false, -1.0f, 0, 1.0f);
	}
}