#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AnimalSystem/Pool/AnimalPool.h"
#include "PoolCountWidget.generated.h"

UCLASS()
class ANIMALSYSTEM_API UPoolCountWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Rabbit Stats")
		void UpdateRabbitStats(int32 activeRabbits_, int32 rabbitsInPool_);
	UFUNCTION(BlueprintCallable, Category = "Rat Stats")
		void UpdateRatStats(int32 activeRats_, int32 ratsInPool_);
	UFUNCTION(BlueprintCallable, Category = "Squirrel Stats")
		void UpdateSquirrelStats(int32 activeSquirrels_, int32 squirrelsInPool_);
	UFUNCTION(BlueprintCallable, Category = "Total Stats")
		void UpdateTotalStats(int32 totalActiveAnimals, int32 totalInPool);

	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* activeRabbitsText;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* rabbitsInPoolText;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* activeRatsText;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* ratsInPoolText;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* activeSquirrelsText;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* squirrelsInPoolText;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* totalActiveAnimalsText;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* totalAnimalsInPoolText;
};