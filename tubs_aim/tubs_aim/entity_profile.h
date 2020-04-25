#pragma once

#define MAX_PLAYERS_NUM				64

class EntityProfile {

public:
	EntityProfile();
	~EntityProfile();

public:
	bool	isResetAfterDeathNeeded;

	int		lastHealth;
	int		nextToLastHealth;
	int		lastKillsNum;
	int		lastDeathsNum;

	bool	isMovingMouseToTarget;
	float	lastDistanceToTarget;
};

//extern EntityProfile entityProfile[MAX_PLAYERS_NUM + 1];