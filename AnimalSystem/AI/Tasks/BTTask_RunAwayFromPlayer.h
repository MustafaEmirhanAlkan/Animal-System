#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_RunAwayFromPlayer.generated.h"

UCLASS(Blueprintable)
class ANIMALSYSTEM_API UBTTask_RunAwayFromPlayer : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_RunAwayFromPlayer();

	UPROPERTY(EditAnywhere, Category = "Blackboard")
		FBlackboardKeySelector PlayerLocationKey;
	UPROPERTY(EditAnywhere, Category = "Blackboard")
		FBlackboardKeySelector EscapeLocationKey;
	UPROPERTY(EditAnywhere, Category = "AI")
		float AnimalEscapeDistance = 800.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
		float AnimalAlertDistance = 600.0f;
	UPROPERTY(EditAnywhere, Category = "AI")
		float AnimalRunSpeed = 600.0f;

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};