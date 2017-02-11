#pragma once

#include "ofMain.h"
#include "ofxRealSense.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
		void exit();

		ofxRealSense realsense;
		ofTexture colorInDepth, depthInColor; // for drawing calibrated color<->depth imgs
};
