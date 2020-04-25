#include "stdafx.h"
#include "dllmain.h"

//EntityProfile entityProfile[MAX_PLAYERS_NUM + 1];

/**
Constructor. Creates a new Entity object.

@return nothing
*/
EntityProfile::EntityProfile() {
	isResetAfterDeathNeeded = false;

	lastHealth = 0;
	nextToLastHealth = 0;
	lastKillsNum = 0;
	lastDeathsNum = 0;

	isMovingMouseToTarget = false;
	lastDistanceToTarget = 100.0f;
}

/**
	Destructor.

	@return nothing
*/
EntityProfile::~EntityProfile() {

}