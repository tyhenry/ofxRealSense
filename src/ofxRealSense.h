#pragma once

#ifdef _DEBUG
#pragma comment(lib, "libpxcmd_d.lib")
#else
#pragma comment(lib, "libpxcmd.lib")
#endif

#include "ofMain.h"
#include "pxcsensemanager.h"	// main
#include "pxcprojection.h"		// coord mapper
#include "pxcfacemodule.h"		// face tracker module
#include "pxc3dscan.h"			// 3d scanner module
#include "ofxRSUtils.h"
#include "ofxRSScan.h"
#include "ofxRSFaceTracker.h"

using namespace ofxRSUtils;

class ofxRealSense 
{
friend class ofxRSScan;

public:

	ofxRealSense();
	~ofxRealSense() {}

	// initialize camera and modules (face tracking and face scanning)
	bool setup(bool grabColor = true, bool useTextures = true, bool mirror = false, bool trackFaces = true, bool doScan = true);
	void enableDepthColorMap(bool enable = true) { bUVMap = enable; } // only works if grabbing color

	bool open();		// start streaming data
	bool update();		// update data, call each frame
	bool close();		// close data stream

	// draw functions
	void drawColor(float x, float y, float w=0.f, float h=0.f);
	void drawColor() { drawColor(0, 0); }

	void drawDepth(float x, float y, float w=0.f, float h=0.f);
	void drawDepth() { drawDepth(0, 0); }

	void drawScanPreview(float x, float y, float w = 0.f, float h = 0.f);
	void drawScanPreview() { drawScanPreview(0, 0); }

	// face tracker
	const vector<ofxRSFace>& getTrackedFaces() const { return mFaceTracker.getFaces(); }
	int getNumTrackedFaces() { return mFaceTracker.getNumFaces(); }
	const ofxRSFace* getTrackedFace(int index) const
		{ return mFaceTracker.getFace(index); } // returns nullptr if no face at index
	const vector<ofxRSFace::Landmark>* getTrackedFaceLandmarks(int index) const 
		{ return mFaceTracker.getLandmarksByFace(index); } // null if no face


	// scanning (face only for now)
	bool startScan() { return mScanner.start(); }
	bool stopScan() { return mScanner.stop(); }
	bool saveScan(string filename) { return mScanner.save(ofToDataPath(filename, true)); }
	bool isFaceScanReady() { return mScanner.isFaceScanReady(); }
	bool isScanning() { return mScanner.isScanning(); }

	const ofPixels& getColorPixelsRef() { return mColorPix; }
	const ofPixels& getDepthPixelsRef() { return mDepthPix; }
	const ofShortPixels& getDepthRawPixelsRef() { return mDepthRawPix; }
	const ofPixels& getScanPreviewPixelsRef() { return mScanner.getPreviewPixRef(); }

	const ofPixels& getColorPixelsInDepthFrame() { return mColorInDepthFrame; }
	const ofPixels& getDepthPixelsInColorFrame() { return mDepthInColorFrame; }
	const ofShortPixels& getDepthRawPixelsInColorFrame() { return mDepthRawInColorFrame; }
	//const vector<uint16_t>& getDepthRawInColorFrameBuffer() { return mDepthRawInColorFrameBuffer; }

private:

	bool updateDepth(PXCCapture::Sample* sample);
	bool updateColor(PXCCapture::Sample* sample);
	bool mapDepthAndColor(PXCCapture::Sample* sample);
	bool uvMapDepthToColor(PXCImage* depth);

	bool	bColor = true,
			bUseTex = true,
			bMirror = false,
			bFaces = true,
			bScan = true,
			bUVMap = true, // map color <--> depth
			bIsOpen = false,
			bBGR = true;

	ofPixels	mColorPix,				// rgb cam
				mDepthPix,				// depth -> 8bit grayscale
				mColorInDepthFrame,		// UV mapped color into depth frame
				mDepthInColorFrame;		// UV mapped depth into color frame

	ofShortPixels	mDepthRawPix,			// raw depth values in mm (16bit int)
					mDepthRawInColorFrame;	// UV mapped raw depth into color frame
	//vector<uint16_t> mDepthRawInColorFrameBuffer;

	ofTexture	mColorTex, mDepthTex;	// for drawing

	vector<PXCPointF32> mUVDepthToColor; // [640 * 480]; // uv map depth -> color coords
	vector<PXCPointF32> mUVColorToDepth; // [1920 * 1080]; // inverse uv map

	// RealSense SDK interface
	PXCSenseManager *mSenseMgr;
	PXCProjection *mCoordinateMapper;
	PXCCapture::Sample *mCurrentSample;
	
	// face tracking module
	ofxRSFaceTracker mFaceTracker;

	// 3d scanning module
	ofxRSScan mScanner;

};
