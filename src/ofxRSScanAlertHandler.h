#pragma once
#include "pxc3dscan.h"
#include "ofMain.h"

class ofxRSScanAlertHandler : public PXC3DScan::AlertHandler {

public:

	enum FaceStatusDetected
	{
		FaceStatusDetected_Detected,
		FaceStatusDetected_NotDetected
	};

	enum FaceStatusXAxis
	{
		FaceStatusXAxis_InRange,
		FaceStatusXAxis_TooFarLeft,
		FaceStatusXAxis_TooFarRight
	};

	enum FaceStatusYAxis
	{
		FaceStatusYAxis_InRange,
		FaceStatusYAxis_TooFarUp,
		FaceStatusYAxis_TooFarDown
	};

	enum FaceStatusZAxis
	{
		FaceStatusZAxis_InRange,
		FaceStatusZAxis_TooClose,
		FaceStatusZAxis_TooFar
	};

	enum FaceStatusYaw
	{
		FaceStatusYaw_InRange,
		FaceStatusYaw_TooFarLeft,
		FaceStatusYaw_TooFarRight,
	};

	enum FaceStatusPitch
	{
		FaceStatusPitch_InRange,
		FaceStatusPitch_TooFarUp,
		FaceStatusPitch_TooFarDown,
	};

	enum FaceStatusMotion
	{
		FaceStatusMotion_InRange,
		FaceStatusMotion_TooSlow,
		FaceStatusMotion_TooFast,
	};

	ofxRSScanAlertHandler();
	void reset();
	virtual void PXCAPI OnAlert(const PXC3DScan::AlertData& data);

	PXC3DScan::AlertData Data;

	FaceStatusDetected GetStatusDetected() { return mStatusDetected; }
	FaceStatusXAxis GetStatusXAxis() { return mStatusXAxis; }
	FaceStatusYAxis GetStatusYAxis() { return mStatusYAxis; }
	FaceStatusZAxis GetStatusZAxis() { return mStatusZAxis; }
	FaceStatusYaw GetStatusYaw() { return mStatusYaw; }
	FaceStatusPitch GetStatusPitch() { return mStatusPitch; }

private:

	const char* alertMap[27] =
	{
		"ALERT_IN_RANGE",
		"ALERT_TOO_CLOSE",
		"ALERT_TOO_FAR",
		"ALERT_TRACKING",
		"ALERT_LOST_TRACKING",
		"ALERT_SUFFICIENT_STRUCTURE",
		"ALERT_INSUFFICIENT_STRUCTURE",
		"ALERT_FACE_DETECTED",
		"ALERT_FACE_NOT_DETECTED",
		"ALERT_FACE_X_IN_RANGE",
		"ALERT_FACE_X_TOO_FAR_LEFT",
		"ALERT_FACE_X_TOO_FAR_RIGHT",
		"ALERT_FACE_Y_IN_RANGE",
		"ALERT_FACE_Y_TOO_FAR_UP",
		"ALERT_FACE_Y_TOO_FAR_DOWN",
		"ALERT_FACE_Z_IN_RANGE",
		"ALERT_FACE_Z_TOO_CLOSE",
		"ALERT_FACE_Z_TOO_FAR",
		"ALERT_FACE_YAW_IN_RANGE",
		"ALERT_FACE_YAW_TOO_FAR_LEFT",
		"ALERT_FACE_YAW_TOO_FAR_RIGHT",
		"ALERT_FACE_PITCH_IN_RANGE",
		"ALERT_FACE_PITCH_TOO_FAR_UP",
		"ALERT_FACE_PITCH_TOO_FAR_DOWN",
		"ALERT_FACE_MOTION_TOO_SLOW",
		"ALERT_FACE_MOTION_TOO_FAST",
		"ALERT_FACE_MOTION_IN_RANGE"
	};

	FaceStatusDetected mStatusDetected;
	FaceStatusXAxis mStatusXAxis;
	FaceStatusYAxis mStatusYAxis;
	FaceStatusZAxis mStatusZAxis;
	FaceStatusYaw mStatusYaw;
	FaceStatusPitch mStatusPitch;
	FaceStatusMotion mStatusMotion;

};