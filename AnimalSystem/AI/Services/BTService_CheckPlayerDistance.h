#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_CheckPlayerDistance.generated.h"

UCLASS(Blueprintable)
class ANIMALSYSTEM_API UBTService_CheckPlayerDistance : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_CheckPlayerDistance();
	UPROPERTY(EditAnywhere, Category = "Blackboard")
		FBlackboardKeySelector PlayerLocationKey;
	UPROPERTY(EditAnywhere, Category = "Blackboard")
		FBlackboardKeySelector CanSeePlayerKey;
	UPROPERTY(EditAnywhere, Category = "Blackboard")
		FBlackboardKeySelector DistanceToPlayerKey;
	UPROPERTY(EditAnywhere, Category = "Distance")
		float SightRange = 1000.0f;
	UPROPERTY(EditAnywhere, Category = "Distance")
		float RabbitEscapeDistance = 1000.0f;
	UPROPERTY(EditAnywhere, Category = "Distance")
		float RatEscapeDistance = 600.0f;
	UPROPERTY(EditAnywhere, Category = "Distance")
		float SquirrelEscapeDistance = 800.0f;

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

};
