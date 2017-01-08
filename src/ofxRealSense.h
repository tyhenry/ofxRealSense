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

using namespace ofxRSUtils;

class ofxRealSense 
{
friend class ofxRSScan;

public:

	ofxRealSense();

	// initialize camera and modules (face tracking and face scanning)
	bool setup(bool grabColor = true, bool useTextures = true, bool trackFaces = true, bool doScan = true);
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
	const ofPixels& getDepthRawPixelsInColorFrame() { return mDepthRawInColorFrame; }

private:

	bool updateDepth(PXCCapture::Sample* sample);
	bool updateColor(PXCCapture::Sample* sample);
	bool mapDepthAndColor(PXCCapture::Sample* sample);
	bool uvMapDepthToColor(PXCImage* depth);

	bool	bColor = true,
			bUseTex = true,
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


	ofTexture	mColorTex, mDepthTex;	// for drawing

	// RealSense SDK interface
	PXCSenseManager *mSenseMgr;
	PXCProjection *mCoordinateMapper;
	PXCCapture::Sample *mCurrentSample;
	
	// face tracking module
	PXCFaceModule *mFaceTracker;
	PXCFaceData* mFaceData;
	vector<vector<PXCFaceData::LandmarkPoint>> mFaces;

	// 3d scanning module
	ofxRSScan mScanner;

};
