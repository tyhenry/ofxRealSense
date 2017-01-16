#pragma once
#include "pxcfacemodule.h"
#include "ofMain.h"
#include "ofxRSUtils.h"

class ofxRSFaceTracker { 

public:

	ofxRSFaceTracker() {}
	~ofxRSFaceTracker() {}

	bool enable(PXCSenseManager* senseManagerPtr, bool useDepth = true);
	bool update();
	bool disable(); // ?

private:

	bool bActive = false;
	PXCSenseManager* senseMgr;

	PXCFaceModule* faceTracker;
	PXCFaceData* faceData;
	vector<vector<PXCFaceData::LandmarkPoint>> faces;
};