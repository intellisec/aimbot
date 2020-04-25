#pragma once

#define M_RADPI					57.295779513082f
#define M_PI					3.14159265358979323846
#define M_PI_F					((float)(M_PI))

#define SQUARE(a)				(a*a)
#define DEG2RAD(x)				((float)(x) * (float)(M_PI_F / 180.f))
#define RAD2DEG(x)				((float)(x) * (float)(180.f / M_PI_F))

double inline __declspec (naked) __fastcall FastSQRT(double n)
{
	_asm fld qword ptr[esp + 4]
		_asm fsqrt
	_asm ret 8
}

struct tMath {
	bool			canHit(Vector current, Vector target, float distance, float max);

	void			anglesToVector(Vector angle, Vector& vector);
	void			clampAngles(Vector& angles);

	float			angleNormalize(float angle);
	float			vectorNormalize(Vector& vector);
	float			getFieldOfView(Vector viewAngles, Vector src, Vector dst);

	Vector			vectorToAngles(const Vector& direction);
	Vector			calculateAngles(Vector src, Vector dst);
};

extern tMath pMath;