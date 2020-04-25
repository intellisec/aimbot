#include "stdafx.h"

/**
	Converts an angle into one normalised Vector.
	It points in the same direction as the angle ("forward").

	https://developer.valvesoftware.com/wiki/AngleVectors()

	@param angles
	@param forward
	@return nothing
*/
void tMath::anglesToVector(Vector angle, Vector& vector) {
	float pitch = float(angle[0] * M_PI / 180);
	float yaw = float(angle[1] * M_PI / 180);
	float tmp = float(cos(pitch));
	vector.x = float(-tmp * -cos(yaw));
	vector.y = float(sin(yaw)*tmp);
	vector.z = float(-sin(pitch));
}

/**
	Converts a single vector into a QAngle.
	VectorAngeles: https://developer.valvesoftware.com/wiki/AngleVectors()
	QAngle: https://developer.valvesoftware.com/wiki/QAngle

	@param angles the vector to convert
	@return the angles
*/
Vector tMath::vectorToAngles(const Vector& direction) {
	float pitch, yaw;

	if (!direction.x && !direction.y) {
		pitch = direction.z > 0.f ? 270.f : 90.f;
		yaw = 0.f;
	} else {
		pitch = RAD2DEG(atan2f(-direction.z, direction.LengthXY()));
		pitch += pitch < 0.f ? 360.f : 0.f;

		yaw = RAD2DEG(atan2f(direction.y, direction.x));
		yaw += yaw < 0.f ? 360.f : 0.f;
	}

	return Vector(pitch, yaw, 0.f);
}

/**
	Computes the length of the vector and normalizes the vector.

	@param vector the vector to normalize
	@return the length of the given vector
*/
float tMath::vectorNormalize(Vector& vector) {
	auto length = vector.Length();
	vector = length ? vector / length : Vector();
	return length;
}

/**
	Normalizes the angle. (from -180 to 180)

	@param angle the angle to normalize
	@return the normalized angle
*/
float tMath::angleNormalize(float angle) {
	return isfinite(angle) ? remainder(angle, 360.f) : 0.f;
}

/**
	Clamps the angles. Sets the out of range values to the upper or lower limit.
	Ranges: https://developer.valvesoftware.com/wiki/QAngle

	@param angles the angles to clamp
	@return the clamped angles
*/
void tMath::clampAngles(Vector& angles) {
	angles.x = std::max(-89.f, std::min(89.f, angleNormalize(angles.x)));
	angles.y = angleNormalize(angles.y);
	angles.z = 0.f;
}

/**
	Calculates the distance between the current view angles and the view angles the player must have to look at the target.

	@param viewAngles the view angles of the player
	@param src the position of the player
	@param dst the position of the target
	@return the calculated distance
*/
float tMath::getFieldOfView(Vector viewAngles, Vector src, Vector dst) {
	Vector aimVector, viewVector;
	aimVector = tMath::calculateAngles(src, dst);

	pMath.anglesToVector(viewAngles, viewVector);
	pMath.anglesToVector(aimVector, aimVector);

	double mag = viewVector.Length();
	double u_dot_v = viewVector.DotProduct(aimVector);

	return RAD2DEG(acos(u_dot_v / (pow(mag, 2))));
}

/**
	Calculates the view angles to look at the target.

	@param src the position of the player
	@param dst the position of the target
	@return the calculated view angles
*/
Vector tMath::calculateAngles(Vector src, Vector dst) {
	Vector vDelta = src - dst;
	float fHyp = vDelta.LengthXY();

	float yaw = (atanf(vDelta.y / vDelta.x)) * (180.0f / (float)M_PI);
	float pitch = (atanf(vDelta.z / fHyp)) * (180.0f / (float)M_PI);

	if (vDelta.x >= 0.0f)
		yaw += 180.0f;

	return Vector(pitch, yaw, 0.0f);
}

/**
	Calculated if a shot would hit with the given parameters.

	@param current the current ViewAngles from the local player
	@param target the target angles
	@param distance the distance between the two entities
	@param max the maximum permissible deviation from the target point
	@return true if a hit is possible, otherwise false
*/
bool tMath::canHit(Vector current, Vector target, float distance, float max) {
	//Console::getInstance().appendToLogFile("canHit");
	auto delta = target - current;
	tMath::clampAngles(delta);
	auto xdist = sinf(DEG2RAD(delta.Length())) * distance / 2.f;
	return xdist <= max;
}