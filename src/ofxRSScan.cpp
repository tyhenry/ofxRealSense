#include "ofxRSScan.h"

ofxRSScan::ofxRSScan() 
	: scanMode(RSSCAN_NULL), senseMgr(NULL), scanner(NULL)
{
	// check little endian-ness
	// to see if pixel channel order is BGR or RGB from Real Sense SDK
	short int number = 0x1;
	char *numPtr = (char*)&number;
	bBGR = (numPtr[0] == 1);
}

//--------------------------------------------------------------
bool ofxRSScan::enable(PXCSenseManager* senseManagerPtr)
{
	// error: can not re-enable without closing first
	if (scanMode >= RSSCAN_ENABLED)
	{
		ofLogError("RealSense::Scan") << "couldn't enable 3D scanning module, already enabled!";
		return false;
	}
	// error: no sense manager
	if (senseManagerPtr == NULL)
	{
		ofLogError("RealSense::Scan") << "couldn't enable 3D scanning module, invalid Real Sense SDK manager";
		return false;
	}

	senseMgr = senseManagerPtr;						// store sense manager ptr
	pxcStatus status = senseMgr->Enable3DScan();	// enable scanning module
	
	// RS SDK error?
	if (status < PXC_STATUS_NO_ERROR)
	{
		ofLogError("RealSense::Scan") << "couldn't enable 3D scanning module, error status: " << status;
		return false;
	}

	scanMode = RSSCAN_ENABLED;
	return true;
}

//--------------------------------------------------------------
bool ofxRSScan::configure( /* to do: options */ )
{
	// error: must enable module before configuring
	if (scanMode < RSSCAN_ENABLED || scanMode > RSSCAN_READY)
	{
		string msg = (scanMode < RSSCAN_ENABLED) ? "module not yet enabled" : "already scanning";
		ofLogError("RealSense::Scan") << "can't configure 3D scanning, " + msg;
		return false;
	}
	// error: no RS SDK manager (should be avoided by scanMode check, but test anyway)
	if (senseMgr == NULL)
	{
		ofLogError("RealSense::Scan") << "can't configure 3D scanning, invalid Real Sense SDK manager";
		return false;
	}

	// get 3d scanning module
	scanner = senseMgr->Query3DScan();

	// config scan parameters
	PXC3DScan::Configuration config = scanner->QueryConfiguration();

	config.startScan = false;														// don't start yet
	config.mode = PXC3DScan::ScanningMode::FACE;									// face scanning mode
	config.options = PXC3DScan::ReconstructionOption::TEXTURE;						// generate RGB texture as JPG
	config.options = config.options | PXC3DScan::ReconstructionOption::LANDMARKS;	// generate face landmarks as JSON file
	config.maxTriangles = 0;														// 0 == no limit
	config.maxVertices = 0;
	config.useMarker = false;														// not using tracking aids
	config.flopPreviewImage = true;													// mirror preview

	pxcStatus status = scanner->SetConfiguration(config);	// save configuration

	// RS SDK error?
	if (status < PXC_STATUS_NO_ERROR)
	{
		ofLogError("RealSense::Scan") << "unable to set scan configuration, error status: " << status;
		return false;
	}


	// scan alert callback handler
	scanner->Subscribe(&alertHandler);

	scanMode = RSSCAN_READY;
	return true;
}

//--------------------------------------------------------------
bool ofxRSScan::start() {

	// error: module not yet ready or already scanning
	if (scanMode != RSSCAN_READY)
	{
		string msg = (scanMode < RSSCAN_READY) ? "module not yet ready" : "already scanning";
		ofLogError("RealSense::Scan") << "can't start 3D scanning, " + msg;
		return false;
	}

	// start scan
	PXC3DScan::Configuration config = scanner->QueryConfiguration();
	config.startScan = true;
	pxcStatus status = scanner->SetConfiguration(config);

	// RS SDK error?
	if (status < PXC_STATUS_NO_ERROR)
	{
		ofLogError("RealSense::Scan") << "unable to start scan, error status: " << status;
		return false;
	}

	scanMode = RSSCAN_STARTED;
	return true;
}

//--------------------------------------------------------------
bool ofxRSScan::updatePreview(bool bMakeTexture) {

	// error: scanner must be enabled/configured (ready) before update

	if (scanMode < RSSCAN_READY)
	{
		ofLogError("RealSense::Scan") << "can't update scan preview, not ready!";
		return false;
	}

	// get scan preview image

	PXCImage* img = scanner->AcquirePreviewImage();
	if (!img)
	{
		ofLogError("RealSense::Scan") << "scan preview image null!";
		return false;
	}

	// get pixels data

	PXCImage::ImageData imgData;
	pxcStatus status = img->AcquireAccess(PXCImage::ACCESS_READ, PXCImage::PIXEL_FORMAT_RGB24, &imgData);
	if (status < PXC_STATUS_NO_ERROR)
	{
		ofLogError("RealSense::Scan") << "error reading scanner preview image, error status: " << status;
		img->ReleaseAccess(&imgData);
		img->Release();
		return false;
	}

	// copy to ofPixels storage

	PXCImage::ImageInfo info = img->QueryInfo();
	ofPixelFormat pxF = (bBGR) ? OF_PIXELS_BGR : OF_PIXELS_RGB;

	previewPix.setFromPixels(reinterpret_cast<unsigned char*>(imgData.planes[0]), (size_t)info.width, (size_t)info.height, pxF);

	// copy to ofTexture (if needed)
	if (bMakeTexture) { previewTex.loadData(previewPix); }
	

	// release image

	status = img->ReleaseAccess(&imgData);
	if (status < PXC_STATUS_NO_ERROR)
	{
		ofLogError("RealSense::Scan") << "error releasing access to scan preview image, error status: " << status;
		img->Release();
		return false;
	}

	img->Release();
	return true;
}

//--------------------------------------------------------------
bool ofxRSScan::stop()
{
	if (scanMode != RSSCAN_STARTED)
	{
		ofLogError("RealSense::Scan") << "can't stop scan, scan not started";
		return false;
	}

	PXC3DScan::Configuration config = scanner->QueryConfiguration();
	config.startScan = false;
	pxcStatus status = scanner->SetConfiguration(config);

	if (status < PXC_STATUS_NO_ERROR) 
	{
		ofLogError("RealSense::Scan") << "can't stop scan, error status: " << status;
		return false;
	}

	scanMode = RSSCAN_READY;
	return true;
}

//--------------------------------------------------------------
bool ofxRSScan::save(string path)
{
	if (scanMode < RSSCAN_READY)
	{
		ofLogError("RealSense::Scan") << "can't save scan, scanner not ready";
		return false;
	}

	const char* cs = path.c_str();
	cout << cs << endl;

	wchar_t  ws[256];
	swprintf(ws, 256, L"%hs", cs);
	pxcStatus status = scanner->Reconstruct(PXC3DScan::FileFormat::OBJ, ws);
	if (status < PXC_STATUS_NO_ERROR)
	{
		ofLogError("RealSense::Scan") << "couldn't save scan, error status: " << status;
		return false;
	}
	return true;
}

//--------------------------------------------------------------
void ofxRSScan::close()
{
	if (scanMode == RSSCAN_STARTED)
	{
		stop();
	}

	// deregister scan alert callback handler
	if (scanner)
	{
		scanner->Subscribe(NULL);
	}

	// lose pointers
	scanner = NULL;
	senseMgr = NULL;

	// reset scan mode
	scanMode = RSSCAN_NULL;
}