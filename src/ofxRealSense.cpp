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
	else 
	{
		// create coordinate mapper
		mCoordinateMapper = mSenseMgr->QueryCaptureManager()->QueryDevice()->CreateProjection();
	}


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
	status = mSenseMgr->AcquireFrame(true,10); // this is blocking, but will wait for first new frame
	if (status < PXC_STATUS_NO_ERROR)
	{
		ofLogError("ofxRealSense") << "unable to acquire new frame, error status: " << status;
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
	
	// get depth image data from sample
	// PIXEL_FORMAT_DEPTH means pixel buffer is 16-bit unsigned int in millimeters
	PXCImage::ImageData depthData;
	pxcStatus status = sample->depth->AcquireAccess(PXCImage::ACCESS_READ, PXCImage::PIXEL_FORMAT_DEPTH, &depthData);
	if (status < PXC_STATUS_NO_ERROR)
	{
		ofLogError("ofxRealSense") << "can't access depth map image data, error status: " << status;
		sample->depth->ReleaseAccess(&depthData);
		return false;
	}
	
	// load depth image data to ofPixels
	PXCImage::ImageInfo depthInfo = sample->depth->QueryInfo();
	size_t w = depthInfo.width;
	size_t h = depthInfo.height;
	mDepthRawPix.setFromPixels(reinterpret_cast<uint16_t *>(depthData.planes[0]), w, h, OF_PIXELS_GRAY);

	// release access
	status = sample->depth->ReleaseAccess(&depthData);
	if (status < PXC_STATUS_NO_ERROR)
	{
		ofLogError("ofxRealSense") << "error releasing access to depth image data, error status: " << status;
		return false;
	}

	// load depth image data as 8-bit (for easy drawing)
	PXCImage::ImageData depth8Data;
	status = sample->depth->AcquireAccess(PXCImage::ACCESS_READ, PXCImage::PIXEL_FORMAT_RGB32, &depth8Data);
	if (status < PXC_STATUS_NO_ERROR)
	{
		ofLogError("ofxRealSense") << "can't access depth grayscale image data, error status: " << status;
		sample->depth->ReleaseAccess(&depth8Data);
		return false;
	}

	// load grayscale depth data to ofPixels
	ofPixelFormat pxF = bBGR ? OF_PIXELS_BGRA : OF_PIXELS_RGBA;
	mDepthPix.setFromExternalPixels(reinterpret_cast<uint8_t *>(depth8Data.planes[0]), w, h, pxF);

	// load to texture (if needed)
	if (bUseTex)
	{
		mDepthTex.loadData(mDepthPix);
	}

	// release
	status = sample->depth->ReleaseAccess(&depth8Data);
	if (status < PXC_STATUS_NO_ERROR)
	{
		ofLogError("ofxRealSense") << "error releasing access to depth grayscale image data, error status: " << status;
		return false;
	}

	return true;
}

//--------------------------------------------------------------
bool ofxRealSense::updateColor(PXCCapture::Sample *sample)
{
	if (!sample->color)
	{
		ofLogVerbose("ofxRealSense") << "can't update color, invalid sample";
		return false;
	}

	// get color image data from sample
	PXCImage::ImageData colorData;
	pxcStatus status = sample->color->AcquireAccess(PXCImage::ACCESS_READ, PXCImage::PIXEL_FORMAT_RGB24, &colorData);
	if (status < PXC_STATUS_NO_ERROR)
	{
		ofLogError("ofxRealSens") << "can't access color image data, error status: " << status;
		sample->color->ReleaseAccess(&colorData);
		return false;
	}

	// load color image data to ofPixels
	PXCImage::ImageInfo colorInfo = sample->color->QueryInfo();
	size_t w = (size_t)colorInfo.width;
	size_t h = (size_t)colorInfo.height;
	ofPixelFormat pxF = bBGR ? OF_PIXELS_BGR : OF_PIXELS_RGB;
	mColorPix.setFromPixels(reinterpret_cast<uint8_t *>(colorData.planes[0]), w, h, pxF);

	// load pixels to texture (if needed)
	if (bUseTex)
	{
		mColorTex.loadData(mColorPix);
	}

	// release access
	status = sample->color->ReleaseAccess(&colorData);
	if (status < PXC_STATUS_NO_ERROR)
	{
		ofLogError("ofxRealSense") << "error releasing access to color image data, error status: " << status;
		return false;
	}

	return true;
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

	// first, get mapped color img to depth frame
	PXCImage* colorInDepth = mCoordinateMapper->CreateColorImageMappedToDepth(sample->depth, sample->color);

	PXCImage::ImageData cData;
	pxcStatus status = colorInDepth->AcquireAccess(PXCImage::ACCESS_READ, PXCImage::PixelFormat::PIXEL_FORMAT_RGB32, &cData);
	if (status < pxcStatus::PXC_STATUS_NO_ERROR)
	{
		ofLogError("ofxRealSense") << "couldn't access color img mapped to depth, error code: " << status;
		success = false;
	}
	else
	{
		// load image to ofPixels
		PXCImage::ImageInfo cInfo = colorInDepth->QueryInfo();
		size_t w = (size_t)cInfo.width;
		size_t h = (size_t)cInfo.height;
		ofPixelFormat pxF = bBGR ? OF_PIXELS_BGRA : OF_PIXELS_RGBA;
		mColorInDepthFrame.setFromPixels(reinterpret_cast<uint8_t *>(cData.planes[0]), w, h, pxF);
	}

	status = colorInDepth->ReleaseAccess(&cData);
	if (status < pxcStatus::PXC_STATUS_NO_ERROR)
	{
		ofLogError("ofxRealSense") << "couldn't release access to color img mapped to depth, error code: " << status;
	}
	colorInDepth->Release();


	// next, get mapped depth img to color frame
	PXCImage* depthInColor = mCoordinateMapper->CreateDepthImageMappedToColor(sample->depth, sample->color);
	PXCImage::ImageData dData;
	status = depthInColor->AcquireAccess(PXCImage::ACCESS_READ, PXCImage::PixelFormat::PIXEL_FORMAT_RGB32, &dData);
	if (status < pxcStatus::PXC_STATUS_NO_ERROR)
	{
		ofLogError("ofxRealSense") << "couldn't access depth img mapped to color, error code: " << status;
		success = false;
	}
	else
	{
		// load image to ofPixels
		PXCImage::ImageInfo dInfo = depthInColor->QueryInfo();
		size_t w = (size_t)dInfo.width;
		size_t h = (size_t)dInfo.height;
		ofPixelFormat pxF = bBGR ? OF_PIXELS_BGRA : OF_PIXELS_RGBA;
		mDepthInColorFrame.setFromPixels(reinterpret_cast<uint8_t *>(dData.planes[0]), w, h, pxF);
	}

	status = depthInColor->ReleaseAccess(&dData);
	if (status < pxcStatus::PXC_STATUS_NO_ERROR)
	{
		ofLogError("ofxRealSense") << "couldn't release access to depth img mapped to color, error code: " << status;
	}
	depthInColor->Release();


	return success;
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

