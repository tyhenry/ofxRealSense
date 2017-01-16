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

	const vector<vector<PXCFaceData::LandmarkPoint>>& getFaceLandmarks() { return faces; }
	int getNumFaces() { return faces.size(); }
	vector<ofVec3f> getFaceLandmarksWorld(int face);
	vector<ofVec2f> getFaceLandmarksColor(int face);
	vector < vector<ofVec3f>> getFaceLandmarksWorld();
	vector < vector<ofVec2f>> getFaceLandmarksColor();

private:

	bool bActive = false;
	PXCSenseManager* senseMgr;

	PXCFaceModule* faceTracker;
	PXCFaceData* faceData;
	vector<vector<PXCFaceData::LandmarkPoint>> faces;
};