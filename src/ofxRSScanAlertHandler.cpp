#include "ofxRSScanAlertHandler.h"

ofxRSScanAlertHandler::ofxRSScanAlertHandler()
{
	reset();
}
void ofxRSScanAlertHandler::reset()
{
	ZeroMemory(&Data, sizeof(Data));
	mStatusDetected = (FaceStatusDetected)0;
	mStatusXAxis = (FaceStatusXAxis)0;
	mStatusYAxis = (FaceStatusYAxis)0;
	mStatusZAxis = (FaceStatusZAxis)0;
	mStatusPitch = (FaceStatusPitch)0;
	mStatusYaw = (FaceStatusYaw)0;
}

void PXCAPI ofxRSScanAlertHandler::OnAlert(const PXC3DScan::AlertData& data)
{
	Data = data;
	int labelIndex = data.label;
	if (labelIndex >= 0 && labelIndex <= 23/*ARRAYSIZE(alertMap)*/)
	{
		ofLogNotice("ofxRSScan Alert") << alertMap[labelIndex];
	}
	switch (data.label)
	{
		case PXC3DScan::ALERT_FACE_DETECTED: { mStatusDetected = FaceStatusDetected_Detected; } break;
		case PXC3DScan::ALERT_FACE_NOT_DETECTED: { mStatusDetected = FaceStatusDetected_NotDetected; } break;

		case PXC3DScan::ALERT_FACE_X_IN_RANGE: { mStatusXAxis = FaceStatusXAxis_InRange; } break;
		case PXC3DScan::ALERT_FACE_X_TOO_FAR_LEFT: { mStatusXAxis = FaceStatusXAxis_TooFarLeft; } break;
		case PXC3DScan::ALERT_FACE_X_TOO_FAR_RIGHT: { mStatusXAxis = FaceStatusXAxis_TooFarRight; } break;

		case PXC3DScan::ALERT_FACE_Y_IN_RANGE: { mStatusYAxis = FaceStatusYAxis_InRange; } break;
		case PXC3DScan::ALERT_FACE_Y_TOO_FAR_UP: { mStatusYAxis = FaceStatusYAxis_TooFarUp; } break;
		case PXC3DScan::ALERT_FACE_Y_TOO_FAR_DOWN: {mStatusYAxis = FaceStatusYAxis_TooFarDown; } break;

		case PXC3DScan::ALERT_FACE_Z_IN_RANGE: { mStatusZAxis = FaceStatusZAxis_InRange; } break;
		case PXC3DScan::ALERT_FACE_Z_TOO_CLOSE: { mStatusZAxis = FaceStatusZAxis_TooClose; } break;
		case PXC3DScan::ALERT_FACE_Z_TOO_FAR: { mStatusZAxis = FaceStatusZAxis_TooFar; } break;

		case PXC3DScan::ALERT_FACE_YAW_IN_RANGE: { mStatusYaw = FaceStatusYaw_InRange; } break;
		case PXC3DScan::ALERT_FACE_YAW_TOO_FAR_LEFT: { mStatusYaw = FaceStatusYaw_TooFarLeft; } break;
		case PXC3DScan::ALERT_FACE_YAW_TOO_FAR_RIGHT: { mStatusYaw = FaceStatusYaw_TooFarRight; } break;

		case PXC3DScan::ALERT_FACE_PITCH_IN_RANGE: { mStatusPitch = FaceStatusPitch_InRange; } break;
		case PXC3DScan::ALERT_FACE_PITCH_TOO_FAR_UP: { mStatusPitch = FaceStatusPitch_TooFarUp; } break;
		case PXC3DScan::ALERT_FACE_PITCH_TOO_FAR_DOWN: { mStatusPitch = FaceStatusPitch_TooFarDown; } break;
	}
}