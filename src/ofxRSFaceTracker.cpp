#include "ofxRSFaceTracker.h"

/*------------*/
/* ofxRSFace */
/*-----------*/

bool ofxRSFace::loadFrom(PXCFaceData::Face* face)
{
	if (!face) return false;

	landmarks.clear();

	PXCFaceData::LandmarksData* landmarksData = face->QueryLandmarks();
	if (!landmarksData) return false;

	// loop through landmark groups enum
	for (int g = LANDMARK_GROUP_LEFT_EYE; g <= LANDMARK_GROUP_LEFT_EAR; g *= 2)
	{
		PXCFaceData::LandmarksGroupType group = (PXCFaceData::LandmarksGroupType)g;

		int nPts = landmarksData->QueryNumPointsByGroup(group);
		PXCFaceData::LandmarkPoint* points = new PXCFaceData::LandmarkPoint[nPts];

		if (landmarksData->QueryPointsByGroup(group, points)) // load points into array
		{
			// push points into landmarks vector
			for (int p = 0; p < nPts; p++)
				landmarks.push_back(Landmark(points[p], (LandmarkGroup)group));
		}
		delete[] points;
	}
	return true;
}

vector<ofxRSFace::Landmark> ofxRSFace::getLandmarksByGroup(LandmarkGroup group)
{
	vector<Landmark> lms;
	for (auto& landmark : landmarks) {
		if (landmark.group == group) lms.push_back(landmark);
	}
	return lms;
}



/*------------------*/
/* ofxRSFaceTracker */
/*------------------*/

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
	else if (status == PXC_STATUS_DATA_NOT_CHANGED)
	{
		return false; // no new data, no need to update
	}

	// num faces
	pxcI32 nFaces = faceData->QueryNumberOfDetectedFaces();

	faces.clear();

	for (int i = 0; i < nFaces; i++) {
		PXCFaceData::Face* face = faceData->QueryFaceByIndex(i);
		if (face) 
			faces.push_back(ofxRSFace(face));
	}

	return true;
}

void ofxRSFaceTracker::disable() {

	if (faceData)
	{
		faceData->Release();
		faceData = nullptr;
	}
	faceTracker = nullptr;
	bActive = false;
}

const ofxRSFace * ofxRSFaceTracker::getFace(int index) const
{
	if (index >= 0 && index < faces.size())
		return &faces[index];
	return nullptr;
}

const vector<ofxRSFace::Landmark> * ofxRSFaceTracker::getLandmarksByFace(int index) const
{
	if (index >= 0 && index < faces.size()) 
		return &(faces[index].getLandmarks());
	return nullptr;
}



