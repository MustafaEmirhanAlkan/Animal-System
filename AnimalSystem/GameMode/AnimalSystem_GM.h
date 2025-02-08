#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AnimalSystem/Manager/AnimalManager.h"
#include "AnimalSystem_GM.generated.h"

UCLASS()
class ANIMALSYSTEM_API AAnimalSystem_GM : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
		TSubclassOf<UUserWidget> PoolCountWidgetClass;

	AAnimalSystem_GM();
	AAnimalManager* AnimalManager;

	void BeginPlay();
};
