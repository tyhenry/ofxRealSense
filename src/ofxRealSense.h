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
#include "ofxRSScan.h"

class ofxRealSense 
{
friend class ofxRSScan;

public:

	ofxRealSense();

	bool setup(bool grabColor = true, bool useTextures = true, bool trackFaces = true, bool doScan = true);		// initializes camera and modules (face tracking and face scanning)

	bool open();		// start streaming data
	bool update();		// updates data, called each frame
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

	const ofPixels& getColorPixelsRef() { return mColorPix; }
	const ofPixels& getDepthPixelsRef() { return mDepthPix; }
	const ofShortPixels& getDepthRawPixelsRef() { return mDepthRawPix; }
	const ofPixels& getScanPreviewPixelsRef() { return mScanner.getPreviewPixRef(); }

private:

	bool updateDepth(PXCCapture::Sample* sample);
	bool updateColor(PXCCapture::Sample* sample);

	bool	bColor = true,
			bUseTex = true,
			bFaces = true,
			bScan = true,
			bIsOpen = false,
			bBGR = true;

	ofPixels mColorPix, mDepthPix;	// rgb cam, and depth->8bit grayscale
	ofShortPixels mDepthRawPix;		// raw depth values in mm (16bit int)

	ofTexture mColorTex, mDepthTex;	// for drawing

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

	// utility to check if processor is little-endian (e.g. Intel)
	// if little-endian, Intel uses BGR (not RGB) pixel channel order
	bool isLittleEndian()
	{
		short int number = 0x1;
		char *numPtr = (char*)&number;
		return (numPtr[0] == 1);
	}

};
