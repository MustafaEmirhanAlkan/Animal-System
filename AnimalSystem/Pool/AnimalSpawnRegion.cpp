#include "AnimalSystem/Pool/AnimalSpawnRegion.h"
#include "AnimalSystem/Characters/Animals/BaseAnimal.h"
#include "AnimalSystem/Manager/AnimalManager.h"
#include "AnimalSystem/Characters/PlayerCharacter/AnimalSystemCharacter.h"
#include "Components/BoxComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

AAnimalSpawnRegion::AAnimalSpawnRegion()
{
	PrimaryActorTick.bCanEverTick = false;

	SpawnRegion = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnRegion"));
	RootComponent = SpawnRegion;

	SpawnRegion->SetBoxExtent(FVector(500.0f, 500.0f, 100.0f));
	SpawnRegion->SetCollisionProfileName(TEXT("Trigger"));

	SpawnRegion->OnComponentBeginOverlap.AddDynamic(this, &AAnimalSpawnRegion::OnOverlapBegin);
}

void AAnimalSpawnRegion::BeginPlay()
{
	Super::BeginPlay();

	DrawDebugBox(GetWorld(), GetActorLocation(), SpawnRegion->GetScaledBoxExtent(), FColor::Green, true, -1.0f, 0, 5.0f);
	AnimalManager = Cast<AAnimalManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AAnimalManager::StaticClass()));
}

TSubclassOf<ABaseAnimal> AAnimalSpawnRegion::GetRandomAnimalClass()
{
	float TotalPercentage = 0.0f;
	for (const FAnimalSpawnPercentage& SpawnInfo : AnimalSpawnPercentages)
	{
		TotalPercentage += SpawnInfo.SpawnPercentage;
	}

	float RandomValue = FMath::FRandRange(0.0f, TotalPercentage);
	float AccumulatedPercentage = 0.0f;

	for (const FAnimalSpawnPercentage& SpawnInfo : AnimalSpawnPercentages)
	{
		AccumulatedPercentage += SpawnInfo.SpawnPercentage;
		if (RandomValue <= AccumulatedPercentage)
		{
			return SpawnInfo.AnimalClass;
		}
	}
	return nullptr;
}

void AAnimalSpawnRegion::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	if (AAnimalSystemCharacter* Character = Cast<AAnimalSystemCharacter>(OtherActor))
	{
		DisplayAnimalPercentages(Character);
		UE_LOG(LogTemp, Log, TEXT("Character entered spawn region: %s"), *GetName());

		if (!GetWorldTimerManager().IsTimerActive(spawnTimerHandle))
		{
			GetWorldTimerManager().SetTimer(
				spawnTimerHandle,
				this,
				&AAnimalSpawnRegion::SpawnAnimalInRegion,
				spawnInterval,
				true
			);
		}
	}
}

void AAnimalSpawnRegion::SpawnAnimalInRegion()
{
	TSubclassOf<ABaseAnimal> SelectedAnimalClass = GetRandomAnimalClass();
	FVector SpawnLocation = UKismetMathLibrary::RandomPointInBoundingBox(
		SpawnRegion->GetComponentLocation(),
		SpawnRegion->GetScaledBoxExtent()
	);

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	ABaseAnimal* SpawnedAnimal = GetWorld()->SpawnActor<ABaseAnimal>(SelectedAnimalClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
	if (SpawnedAnimal)
	{
		UE_LOG(LogTemp, Log, TEXT("%s spawned in region: %s"), *SpawnedAnimal->GetName(), *GetName());
	}

}

void AAnimalSpawnRegion::DisplayAnimalPercentages(AActor* Character)
{
	if (AnimalSpawnPercentages.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No spawn percentages defined in region: %s"), *GetName());
		return;
	}

	FString PercentagesMessage = FString::Printf(TEXT("Now animal percentages in %s are:"), *GetName());
	for (const FAnimalSpawnPercentage& SpawnInfo : AnimalSpawnPercentages)
	{
		if (SpawnInfo.AnimalClass)
		{
			PercentagesMessage += FString::Printf(TEXT("\n%s: %0.1f%%"),
				*SpawnInfo.AnimalClass->GetName(),
				SpawnInfo.SpawnPercentage);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("%s"), *PercentagesMessage);
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, PercentagesMessage);
}