#pragma once
#include "pxcfacemodule.h"
#include "ofMain.h"
#include "ofxRSUtils.h"


/*------------*/
/* ofxRSFace */
/*-----------*/

class ofxRSFace {

public:

	enum LandmarkGroup // corresponds to PXCFaceData::LandmarksGroupType enum
	{
		LANDMARK_GROUP_UNKNOWN = 0x0000,
		LANDMARK_GROUP_LEFT_EYE = 0x0001,
		LANDMARK_GROUP_RIGHT_EYE = 0x0002,
		LANDMARK_GROUP_RIGHT_EYEBROW = 0x0004,
		LANDMARK_GROUP_LEFT_EYEBROW = 0x0008,
		LANDMARK_GROUP_NOSE = 0x00010,
		LANDMARK_GROUP_MOUTH = 0x0020,
		LANDMARK_GROUP_JAW = 0x0040,
		LANDMARK_GROUP_RIGHT_EAR = 0x0080,
		LANDMARK_GROUP_LEFT_EAR = 0x0100,
	};

	struct Landmark {
		int index;
		ofVec2f posColor;
		ofVec3f posWorld;
		int confidenceColor;
		int confidenceWorld;
		LandmarkGroup group = LANDMARK_GROUP_UNKNOWN;
		Landmark(PXCFaceData::LandmarkPoint pt, LandmarkGroup g = LANDMARK_GROUP_UNKNOWN)
			: index(pt.source.index),
			posColor(ofVec2f(pt.image.x, pt.image.y)), posWorld(ofVec3f(pt.world.x, pt.world.y, pt.world.z)),
			confidenceColor(pt.confidenceImage), confidenceWorld(pt.confidenceWorld),
			group(g) 
		{
		}
	};

	ofxRSFace( PXCFaceData::Face* face = nullptr ) { loadFrom(face); }
	~ofxRSFace() {}

	bool loadFrom(PXCFaceData::Face* face);

	int getNumLandmarks() const { return landmarks.size(); }
	const vector<Landmark>& getLandmarks() const { return landmarks; }
	
	vector<Landmark> getLandmarksByGroup(LandmarkGroup group);

private:

	vector<Landmark> landmarks;

};


/*------------------*/
/* ofxRSFaceTracker */
/*------------------*/

class ofxRSFaceTracker { 

public:

	ofxRSFaceTracker() {}
	~ofxRSFaceTracker() {}

	bool enable(PXCSenseManager* senseManagerPtr, bool useDepth = true);
	bool update();
	bool disable(); // ?

	int getNumFaces() { return faces.size(); }

	// get single face
	const vector<ofxRSFace::Landmark>& getLandmarksByFace()

	vector<ofVec3f> getFaceLandmarksWorld(int face);
	vector<ofVec2f> getFaceLandmarksColor(int face);
	vector<ofVec3f> getFaceLandmarksWorldByGroup(int face, PXCFaceData::LandmarksGroupType);
	vector<ofVec2f> getFaceLandmarksColorByGroup(int face, PXCFaceData::LandmarksGroupType);

	// get all faces
	const vector<ofxRSFace>& getFaces() { return faces; }

private:

	bool bActive = false;
	PXCSenseManager* senseMgr;

	PXCFaceModule* faceTracker;
	PXCFaceData* faceData;

	vector<ofxRSFace> faces;

	vector<map<PXCFaceData::LandmarksGroupType, vector<PXCFaceData::LandmarkPoint>>> facesGroups; 
	// ^ this is pretty complex, but basically stores each face as a map of <landmark group, points>
};