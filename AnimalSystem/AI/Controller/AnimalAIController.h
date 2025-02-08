#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "Perception/AISenseConfig_Sight.h"
#include "AnimalAIController.generated.h"

UCLASS()
class ANIMALSYSTEM_API AAnimalAIController : public AAIController
{
	GENERATED_BODY()
public:
	AAnimalAIController();
	UFUNCTION(BlueprintCallable, Category = "AI")
		void InitializeAI(UBehaviorTree* behaviorTree_, UBlackboardData* blackboardData_);
	UFUNCTION()
		void SetCanSeePlayer(bool seePlayer_, class UObject* player_);
	UFUNCTION()
		void OnPerception(AActor* actor_, FAIStimulus stimulus_);

	virtual void Tick(float DeltaTime) override;
	virtual void OnPossess(APawn* inPawn_) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
		UBlackboardComponent* BlackboardComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
		UBehaviorTree* AnimalBehaviorTree;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI")
		UBehaviorTree* RabbitBehaviorTree;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI")
		UBehaviorTree* RatBehaviorTree;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI")
		UBehaviorTree* SquirrelBehaviorTree;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
		UBlackboardData* AnimalBlackboard;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI")
		UBlackboardData* RabbitBlackboard;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI")
		UBlackboardData* RatBlackboard;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI")
		UBlackboardData* SquirrelBlackboard;

	UPROPERTY(EditDefaultsOnly, Category = "AI|Perception")
		UAISenseConfig_Sight* SightConfig;
	UPROPERTY(EditAnywhere, Category = "AI")
		bool bIsRabbit;
	UPROPERTY(EditAnywhere, Category = "AI")
		bool bIsRat;
	UPROPERTY(EditAnywhere, Category = "AI")
		bool bIsSquirrel;

private:
	APawn* controlledPawn;

	UPROPERTY(EditAnywhere, Category = "AI")
		FName PlayerTargetKey = "player_target";
	UPROPERTY(EditAnywhere, Category = "AI")
		FName CanSeePlayerKey = "can_see_player";
	UPROPERTY(EditAnywhere, Category = "AI")
		FName DistanceToEnemyKey = "distance_to_enemy";
	UPROPERTY(EditAnywhere, Category = "AI")
		FName IsRunningKey = "is_running";
	UPROPERTY(EditAnywhere, Category = "AI")
		FName IsWalkingKey = "is_walking";
	UPROPERTY(EditAnywhere, Category = "AI")
		FName HealthKey = "health";
};
