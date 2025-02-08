#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_MoveRandomLocation.generated.h"

UCLASS(Blueprintable)
class ANIMALSYSTEM_API UBTTask_MoveRandomLocation : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_MoveRandomLocation();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;

private:
	UPROPERTY(EditAnywhere, Category = "AI")
		float SearchRadius;
};
