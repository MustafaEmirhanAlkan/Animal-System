#include "AnimalSystem/UI/Widgets/PoolCountWidget.h"
#include "AnimalSystem/Characters/PlayerCharacter/AnimalSystemCharacter.h"
#include "AnimalSystem/Manager/AnimalManager.h"
#include "AnimalSystem/Pool/AnimalPool.h"
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

void UPoolCountWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (!rabbitsInPoolText || !activeRabbitsText || !ratsInPoolText || !activeRatsText || !squirrelsInPoolText || !activeSquirrelsText)
	{
		UE_LOG(LogTemp, Error, TEXT("One or more UI Text elements are not properly initialized in the Widget Blueprint."));
	}

	AAnimalPool* AnimalPool = Cast<AAnimalPool>(UGameplayStatics::GetActorOfClass(GetWorld(), AAnimalPool::StaticClass()));
	if (AnimalPool)
	{
		UpdateRabbitStats(AnimalPool->ActiveRabbits.Num(), AnimalPool->RabbitPool.Num());
		UpdateRatStats(AnimalPool->ActiveRats.Num(), AnimalPool->RatPool.Num());
		UpdateSquirrelStats(AnimalPool->ActiveSquirrels.Num(), AnimalPool->SquirrelPool.Num());
		UpdateTotalStats(AnimalPool->GetTotalActiveAnimals(), AnimalPool->GetTotalAnimalsInPool());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("RabbitPool or RatPool or SquirrelPool is null! Cannot update UI."));
	}
}

void UPoolCountWidget::UpdateRabbitStats(int32 activeRabbits_, int32 rabbitsInPool_)
{
	if (rabbitsInPoolText)
		rabbitsInPoolText->SetText(FText::FromString(FString::Printf(TEXT("Rabbits in Pool: %d"), rabbitsInPool_)));

	if (activeRabbitsText)
		activeRabbitsText->SetText(FText::FromString(FString::Printf(TEXT("Active Rabbits: %d"), activeRabbits_)));
}

void UPoolCountWidget::UpdateRatStats(int32 activeRats_, int32 ratsInPool_)
{
	if (ratsInPoolText)
		ratsInPoolText->SetText(FText::FromString(FString::Printf(TEXT("Rats in Pool: %d"), ratsInPool_)));

	if (activeRatsText)
		activeRatsText->SetText(FText::FromString(FString::Printf(TEXT("Active Rats: %d"), activeRats_)));
}

void UPoolCountWidget::UpdateSquirrelStats(int32 activeSquirrels_, int32 squirrelsInPool_)
{
	if (squirrelsInPoolText)
		squirrelsInPoolText->SetText(FText::FromString(FString::Printf(TEXT("Squirrels in Pool: %d"), squirrelsInPool_)));

	if (activeSquirrelsText)
		activeSquirrelsText->SetText(FText::FromString(FString::Printf(TEXT("Active Squirrels: %d"), activeSquirrels_)));
}

void UPoolCountWidget::UpdateTotalStats(int32 totalActiveAnimals_, int32 totalAnimalsInPool_)
{
	if (totalActiveAnimalsText)
		totalActiveAnimalsText->SetText(FText::FromString(FString::Printf(TEXT("Total Active Animals: %d"), totalActiveAnimals_)));

	if (totalAnimalsInPoolText)
		totalAnimalsInPoolText->SetText(FText::FromString(FString::Printf(TEXT("Total Animals in Pool: %d"), totalAnimalsInPool_)));
}