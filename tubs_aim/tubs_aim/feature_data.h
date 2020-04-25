#pragma once

#define MAX_PLAYERS_NUM				64
#define MAX_REDCORDED_ANGLES		150

class FeatureData {

public:
	FeatureData();
	~FeatureData();

public:

	struct f1_struct {
		bool	saved;
		INT64	startTime;
		float	angle;
		int		timesSeenBefor;
	} f1;

	struct f2_struct {
		json	hits;
	} f2;

	struct f5_struct {
		bool	saved;
		bool	started;
		INT64	startTime;
	} f5;

	struct f6_struct {
		bool	saved;
		bool	started;
		INT64	startTime;
		int		weaponId;
	} f6;

	struct f8_struct {
		bool	saved;
		bool	started;
		int		closestBone;
	} f8;

	struct f9_struct {
		bool	started;
		int		totalHitsFromLastAimingPeriod;
		int		totalAmmoBefor;
	} f9;

	struct f10_struct {
		bool	saved;
		bool	started;
		int		shotCounter;
		bool	hit;
	} f10;

	struct f11_struct {
		bool	saved;
		bool	started;
		float	angle;
		INT64	startTime;
		bool	hit;
	} f11;

	struct f12_struct {
		bool	saved;
		bool	started;
		Vector	startViewAngles;
		Vector	startPunchAngles;
		Vector	recordedAngles[MAX_REDCORDED_ANGLES];
		int		recordedAnglesIndex;
		int		numRecordingCalls;
	} f12;

	struct f13_struct {
		bool	saved;
		bool	started;
		Vector	viewAngles[MAX_REDCORDED_ANGLES];
		Vector	viewAnglesDelta[MAX_REDCORDED_ANGLES][3];
		int		counter;
	} f13;

	struct f14_struct {
		bool	started;
	} f14;

};

//extern FeatureData featureData[MAX_PLAYERS_NUM + 1];
//extern FeatureData *featureData = new FeatureData[MAX_PLAYERS_NUM + 1];