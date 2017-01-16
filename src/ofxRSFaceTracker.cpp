#include "ofxRSFaceTracker.h"

bool ofxRSFaceTracker::enable(PXCSenseManager* senseManagerPtr, bool useDepth) {

	bActive = false;

	if (!senseManagerPtr)
	{
		ofLogError("ofxRealSense") << "couldn't enable face tracking module, invalid sense manager";
		return false;
	}
	senseMgr = senseManagerPtr;
	pxcStatus status = senseMgr->EnableFace();
	if (status < PXC_STATUS_NO_ERROR)
	{
		ofLogError("ofxRealSense") << "couldn't enable face tracker, error status: " << status;
		return false;
	}

	faceTracker = senseMgr->QueryFace();
	if (faceTracker == NULL) 
	{
		ofLogError("ofxRealSense") << "couldn't query face tracker module!";
		return false;
	}

	PXCFaceConfiguration *config = faceTracker->CreateActiveConfiguration();
	if (useDepth)
	{
		status = config->SetTrackingMode(PXCFaceConfiguration::TrackingModeType::FACE_MODE_COLOR_PLUS_DEPTH);
	}
	else
	{
		status = config->SetTrackingMode(PXCFaceConfiguration::TrackingModeType::FACE_MODE_COLOR);
	}
	if (status < PXC_STATUS_NO_ERROR)
	{
		ofLogError("ofxRealSense") << "couldn't configure face tracker " << (useDepth ? "(with" : "(without") << " depth), error status: " << status;
		return false;
	}

	if (config)
	{
		status = config->ApplyChanges();
		config->Release();
		if (status < PXC_STATUS_NO_ERROR)
		{
			ofLogError("ofxRealSense") << "couldn't apply configuration to face tracking module, error status: " << status;
			return false;
		}
	}
	else
	{
		ofLogError("ofxRealSense") << "face tracking configuration error";
		return false;
	}

	faceData = faceTracker->CreateOutput();
	bActive = true;
	return true;
}

bool ofxRSFaceTracker::update() {

	if (!bActive || !senseMgr || !faceTracker || !faceData)
	{
		ofLogError("ofxRealSense") << "couldn't update face tracker module, not yet enabled";
		return false;
	}

	pxcStatus status;
	// update face data
	status = faceData->Update();
	if (status < PXC_STATUS_NO_ERROR)
	{
		ofLogError("ofxRealSense") << "unable to update face tracker, error status: " << status;
		return false;
	}

	// num faces
	pxcI32 nFaces = faceData->QueryNumberOfDetectedFaces();

	faces.clear();
	faces.resize((size_t)nFaces);

	for (int i = 0; i < nFaces; i++) {

		// get landmarks
		PXCFaceData::Face* face = faceData->QueryFaceByIndex(i);
		if (!face) continue;

		PXCFaceData::LandmarksData* landmarkData = face->QueryLandmarks();
		if (!landmarkData) continue;

		faces[i].resize((size_t)landmarkData->QueryNumPoints()); // allocate space for landmarks

		if (!landmarkData->QueryPoints(faces[i].data())) // fills face vector data with landmarks
		{
			ofLogError("ofxRSSDK") << "unable to query landmarks for face " << i;
		}
	}

	return true;
}

bool ofxRSFaceTracker::disable() {

	if (bActive && senseMgr && faceData)
	{
		faceData->Release();
		bActive = false;
		return true;
	}
	return false;
}

vector<vector<ofVec3f>> ofxRSFaceTracker::getFaceLandmarksWorld()
{
	vector<vector<ofVec3f>> landmarks;
	for (auto& face : faces)
	{
		vector<ofVec3f> l;
		for (auto& landmark : face)
		{
			l.push_back(ofVec3f(landmark.world.x, landmark.world.y, landmark.world.z));
		}
		landmarks.push_back(l);
	}
	return landmarks;
}

vector<vector<ofVec2f>> ofxRSFaceTracker::getFaceLandmarksColor()
{
	vector<vector<ofVec2f>> landmarks;
	for (auto& face : faces)
	{
		vector<ofVec2f> l;
		for (auto& landmark : face)
		{
			l.push_back(ofVec2f(landmark.image.x, landmark.world.y));
		}
		landmarks.push_back(l);
	}
	return landmarks;
}



