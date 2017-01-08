#pragma once
#include "pxc3dscan.h"
#include "ofMain.h"
#include "ofxRSUtils.h"
#include "ofxRSScanAlertHandler.h"

using namespace ofxRSUtils;

class ofxRSScan {

friend class ofxRealSense;

public:
	
	enum ScanMode {
		RSSCAN_NULL,			// 0: not yet enabled, no sense manager
		RSSCAN_ENABLED,			// 1: enabled, but not configured
		RSSCAN_READY,			// 2: enabled, configured, ready to start
		RSSCAN_STARTED			// 3: scanning
	};

	ofxRSScan();

	bool enable(PXCSenseManager* senseManagerPtr);
	bool configure( /* to do: config options */ );
	bool start();
	bool updatePreview(bool bMakeTexture = true);
	bool stop();
	bool save(string path);
	void close();

	bool isFaceScanReady();
	bool isScanning();

	ofPixels& getPreviewPixRef() { return previewPix; }
	ofTexture& getPreviewTexRef() { return previewTex; }

private:
	PXCSenseManager* senseMgr;
	PXC3DScan* scanner;
	ofxRSScanAlertHandler alertHandler;

	ScanMode scanMode;
	bool bBGR = true;

	ofPixels previewPix;
	ofTexture previewTex;
};


