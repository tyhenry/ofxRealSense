#include "ofxRealSense.h"

ofxRealSense::ofxRealSense() 
{

}

//--------------------------------------------------------------
bool ofxRealSense::setup(bool grabColor, bool useTextures, bool trackFaces, bool doScan)
{
	if (bIsOpen || mSenseMgr != NULL)
	{
		ofLogError("ofxRealSense") << "can't setup Real Sense, already open!";
		return false;
	}

	// set configuration here - actual SDK init will happen in open()
	bColor = grabColor;
	bUseTex = useTextures;
	bFaces = trackFaces;
	bScan = doScan;

	bBGR = isLittleEndian(); // Real Sense using BGR or RGB channel order?

	return true;
}

//--------------------------------------------------------------
bool ofxRealSense::open()
{

	if (bIsOpen || mSenseMgr != NULL)
	{
		ofLogError("ofxRealSense") << "can't open Real Sense, already open!";
		return false;
	}

	// instantiate RS SDK interface manager
	mSenseMgr = PXCSenseManager::CreateInstance();
	if (mSenseMgr == NULL) 
	{ 
		ofLogError("ofxRealSense") << "couldn't create Real Sense manager";
		return false; 
	}

	pxcStatus status;

	// init depth stream
	status = mSenseMgr->EnableStream(PXCCapture::STREAM_TYPE_DEPTH, 640, 480, 30.f);
	if (status < PXC_STATUS_NO_ERROR)
	{
		ofLogError("ofxRealSense") << "couldn't enable depth stream, error status: " << status;
		close();
		return false;
	}

	// init RGB stream, if desired
	if (bColor)
	{
		status = mSenseMgr->EnableStream(PXCCapture::STREAM_TYPE_COLOR, 1920, 1080, 30.f);
		if (status < PXC_STATUS_NO_ERROR)
		{
			ofLogError("ofxRealSense") << "couldn't enable color stream, error status: " << status;
			close();
			return false;
		}
	}

	// enable 3D scanning module, if desired
	if (bScan)
	{ 
		if (mScanner.enable(mSenseMgr) == false) // try
		{ 
			close(); // close if failed
			return false; 
		}
	}

	// init streams - this must be done before 3D scan config
	status = mSenseMgr->Init();
	if (status < PXC_STATUS_NO_ERROR)
	{
		ofLogError("ofxRealSense") << "couldn't initialize Real Sense capture stream, error status: " << status;
		close();
		return false;
	}

	// create coordinate mapper
	mCoordinateMapper = mSenseMgr->QueryCaptureManager()->QueryDevice()->CreateProjection();

	// config 3D scanning
	if (bScan) { bScan = mScanner.configure(); }

	/* to do: face tracker */

	bIsOpen = true;
	return true;
}

//--------------------------------------------------------------
bool ofxRealSense::update()
{
	// make sure we have a valid RSSDK sense manager
	if (!bIsOpen || mSenseMgr == NULL)
	{
		ofLogError("ofxRealSense") << "unable to update, not yet opened!";
		return false;
	}

	pxcStatus status;

	// get new frame
	/* TO DO: threading a la ofxKinect */
	status = mSenseMgr->AcquireFrame(true,10); // this is blocking for 10ms, returns error if not all frames ready (e.g. depth + color + scan)
	if (status < PXC_STATUS_NO_ERROR)
	{
		ofLogWarning("ofxRealSense") << "unable to acquire new frame, error status: " << status;
		return false;
	}


	/* process images & algorithm modules data */

	// get image sample for depth and color processing
	PXCCapture::Sample *sample = mSenseMgr->QuerySample();

	updateDepth(sample); // update depth

	if (bColor) { updateColor(sample); } // update color
	
	if (bColor && bUVMap && mCoordinateMapper && sample->color && sample->depth) // map color <--> depth if desired
	{
		mapDepthAndColor(sample);
	}


	// update face scanning preview image
	if (bScan) { mScanner.updatePreview(bUseTex); }

	/* to do: update face tracking */


	// release frame to continue
	mSenseMgr->ReleaseFrame();

	return true;
}

//--------------------------------------------------------------
bool ofxRealSense::updateDepth(PXCCapture::Sample *sample)
{
	if (!sample->depth)
	{
		ofLogVerbose("ofxRealSense") << "can't update depth, invalid sample";
		return false;
	}
	
	// load raw depth data into mDepthRawPix (raw depth is measured in mm, as 16bit single channel ofShortPixels)
	bool hasDepthRaw = pxcImageToOfPixels(sample->depth, PXCImage::PIXEL_FORMAT_DEPTH, &mDepthRawPix);

	// load 8-bit grayscale depth image data into mDepthPix (for easy drawing)
	bool hasDepth8 = pxcImageToOfPixels(sample->depth, PXCImage::PIXEL_FORMAT_RGB32, &mDepthPix);

	// load 8-bit depth to texture (if needed + available)
	if (bUseTex && hasDepth8) { mDepthTex.loadData(mDepthPix); }

	return (hasDepthRaw && hasDepth8);
}

//--------------------------------------------------------------
bool ofxRealSense::updateColor(PXCCapture::Sample *sample)
{
	if (!sample->color)
	{
		ofLogVerbose("ofxRealSense") << "can't update color, invalid sample";
		return false;
	}

	// copy color image data to ofPixels
	bool hasColor = pxcImageToOfPixels(sample->color, PXCImage::PIXEL_FORMAT_RGB24, &mColorPix);

	// load pixels to texture (if needed + available)
	if (bUseTex & hasColor) { mColorTex.loadData(mColorPix); }

	return hasColor;
}

//--------------------------------------------------------------
bool ofxRealSense::mapDepthAndColor(PXCCapture::Sample* sample)
{
	if (!sample->depth || !sample->color)
	{
		string which = (!sample->depth) ? "depth" : "color";
		ofLogError("ofxRealSense") << "can't UV map depth and color images, no " << which << " image available";
		return false;
	}

	bool success = true;

	// map color img to depth frame
	PXCImage* colorInDepth = mCoordinateMapper->CreateColorImageMappedToDepth(sample->depth, sample->color);

	// copy to ofPixels
	bool hasColorInDepth = pxcImageToOfPixels(colorInDepth, PXCImage::PIXEL_FORMAT_RGB32, &mColorInDepthFrame);
	colorInDepth->Release();

	// map depth img to color frame
	PXCImage* depthInColor = mCoordinateMapper->CreateDepthImageMappedToColor(sample->depth, sample->color);

	// copy to ofPixels
	bool hasDepthInColor = pxcImageToOfPixels(depthInColor, PXCImage::PIXEL_FORMAT_RGB32, &mDepthInColorFrame); // 8 bit rgb
	bool hasDepthRawInColor = pxcImageToOfPixels(depthInColor, PXCImage::PIXEL_FORMAT_DEPTH, &mDepthRawInColorFrame); // 16 bit gray (raw)
	depthInColor->Release();

	return (hasColorInDepth && hasDepthInColor && hasDepthRawInColor);
}

bool ofxRealSense::uvMapDepthToColor(PXCImage* depth)
{
	/*
	if (!depth) return false;
	
	PXCImage::ImageInfo info = depth->QueryInfo();
	int nDepthPts = info.width * info.height;

	// Calculate the UV map.
	PXCPointF32 *uvmap = new PXCPointF32[nDepthPts];
	mCoordinateMapper->QueryUVMap(depth, uvmap);

	// Translate depth points uv[] to color ij[]
	for (int i = 0; i<nDepthPts; i++) {
		ij[i].x = uvmap[(int)uv[i].y*dinfo.width + (int)uv[i].x].x*cinfo.width;
		ij[i].y = uvmap[(int)uv[i].y*dinfo.width + (int)uv[i].x].y*cinfo.height;
	}
	*/
	return false;
}

//--------------------------------------------------------------
void ofxRealSense::drawColor(float x, float y, float w, float h)
{
	if (!mColorPix.isAllocated()) 
	{ 
		ofLogError("ofxRealSense") << "can't draw color, no pixels stored";
		return; 
	}

	if (!bUseTex) { mColorTex.loadData(mColorPix); }

	if (w > 0 && h > 0) 
	{ 
		mColorTex.draw(x, y, w, h); 
	}
	else
	{
		mColorTex.draw(x, y);
	}
}

//--------------------------------------------------------------
void ofxRealSense::drawDepth(float x, float y, float w, float h)
{
	if (!mDepthPix.isAllocated()) 
	{ 
		ofLogError("ofxRealSense") << "can't draw depth, no pixels stored";
		return; 
	}

	if (!bUseTex) { mDepthTex.loadData(mDepthPix); }

	if (w > 0 && h > 0)
	{
		mDepthTex.draw(x, y, w, h);
	}
	else
	{
		mDepthTex.draw(x, y);
	}
}

void ofxRealSense::drawScanPreview(float x, float y, float w, float h)
{
	if (!mScanner.previewPix.isAllocated())
	{
		ofLogError("ofxRealSense") << "can't draw scan preview, no pixels stored";
		return;
	}

	if (!bUseTex) { mScanner.previewTex.loadData(mScanner.previewPix); }

	if (w > 0 && h > 0)
	{
		mScanner.previewTex.draw(x, y, w, h);
	}
	else
	{
		mScanner.previewTex.draw(x, y);
	}
}


//--------------------------------------------------------------
bool ofxRealSense::close()
{
	if (mSenseMgr != NULL)
	{
		mSenseMgr->Close();
		mSenseMgr->Release(); // ? needed
		mSenseMgr = NULL;
		return true;
	}
	return false;
}

