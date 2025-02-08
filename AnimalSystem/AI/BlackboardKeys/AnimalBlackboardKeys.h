#pragma once
#include "Runtime/Core/Public/UObject/NameTypes.h"
#include "Runtime/Core/Public/Containers/UnrealString.h"

namespace AnimalBlackboardKeys
{
	//Using;
	//Mines;
	TCHAR const* const target_location = TEXT("TargetLocation");
	TCHAR const* const can_see_player = TEXT("CanSeePlayer"); //EnemySpotted?
	TCHAR const* const player_target = TEXT("PlayerTarget"); //Enemy
	TCHAR const* const is_running = TEXT("IsRunning"); //Fear (DoIFearTheEnemy?)
	TCHAR const* const is_walking = TEXT("IsWalking");
	TCHAR const* const health = TEXT("Health");
	//Packs;
	TCHAR const* const distance_to_enemy = TEXT("DistanceToEnemy");

	//Not Using;
	//Mines;
	TCHAR const* const player_is_in_range = TEXT("PlayerIsInRange");
	TCHAR const* const is_patrolling = TEXT("IsPatrolling");
	TCHAR const* const is_dead = TEXT("IsDead");
	//Packs
	TCHAR const* const next_target = TEXT("NextTarget");
	TCHAR const* const random_point_near_my_location = TEXT("RandomPointNearMyLocation");
	TCHAR const* const should_attack_the_enemy = TEXT("ShouldIAttackTheEnemy?");
	TCHAR const* const is_enemy_alive = TEXT("EnemyIsAlive?");
	TCHAR const* const retreat_location = TEXT("RetreatLocation");
	TCHAR const* const gun_shot_location = TEXT("GunShotLocation");
	TCHAR const* const heard_gun_shot_location = TEXT("HeardAGunShot");
};