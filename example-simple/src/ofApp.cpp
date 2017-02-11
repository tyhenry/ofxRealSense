#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

	ofBackground(0);

	realsense.setup(true, true); // grab color, and use textures
	realsense.open(); // opens device

}

//--------------------------------------------------------------
void ofApp::update(){

	realsense.update();

}

//--------------------------------------------------------------
void ofApp::draw() {

	// you can draw depth/color streams from ofxRealSense

	realsense.drawDepth(0, 0, 480, 360); // top left
	realsense.drawColor(480, 0, 640, 360); // top right

	// to draw calibrated color<->depth images, grab the pixels and load to a texture

	colorInDepth.loadData(realsense.getColorPixelsInDepthFrame());
	colorInDepth.draw(0, 360, 480, 360); // bottom left

	depthInColor.loadData(realsense.getDepthPixelsInColorFrame());
	depthInColor.draw(480, 360, 640, 360); // bottom right


	// draw labels
	ofDrawBitmapStringHighlight("depth", 10, 20);
	ofDrawBitmapStringHighlight("color", 490, 20);
	ofDrawBitmapStringHighlight("color in depth frame", 10, 380);
	ofDrawBitmapStringHighlight("depth in color frame", 490, 380);
}

//--------------------------------------------------------------
void ofApp::exit() {
	realsense.close();
}