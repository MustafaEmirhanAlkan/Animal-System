#include "AnimalSystem/GameMode/AnimalSystem_GM.h"
#include "AnimalSystem/Characters/PlayerCharacter/AnimalSystemCharacter.h"
#include "AnimalSystem/Manager/AnimalManager.h"
#include "GameFramework/HUD.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

AAnimalSystem_GM::AAnimalSystem_GM()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	static ConstructorHelpers::FClassFinder<AHUD> HUDClassBP(TEXT("/Game/Blueprints/HUD/Crosshair_HUD"));
	if (HUDClassBP.Class != NULL)
	{
		HUDClass = HUDClassBP.Class;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> PoolCountWidgetBPClass(TEXT("/Game/Blueprints/Widgets/PoolCountWidget_BP"));
	if (PoolCountWidgetBPClass.Class != nullptr)
	{
		PoolCountWidgetClass = PoolCountWidgetBPClass.Class;
	}
}

void AAnimalSystem_GM::BeginPlay()
{
	Super::BeginPlay();

	if (AnimalManager == nullptr)
	{
		AnimalManager = Cast<AAnimalManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AAnimalManager::StaticClass()));		if (AnimalManager)
		{
			UE_LOG(LogTemp, Warning, TEXT("Animal Manager successfully spawned!"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to spawn Animal Manager!"));
		}
	}
}