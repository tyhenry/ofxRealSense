#include "ofxRSUtils.h"


// converts PXCImage to ofPixels (or ofShortPixels)
template <class PixelType>
bool ofxRSUtils::pxcImageToOfPixels(PXCImage* image, PXCImage::PixelFormat imgFormat, ofPixels_<PixelType>* toPixels)
{
	if (image == nullptr || toPixels == nullptr) return false;
	bool bgr = isLittleEndian(); // bgr or rgb order of PXCImage data?

								 // check image format, only RGBA, RGB, or grayscale supported for now
	ofPixelFormat pxf = OF_PIXELS_UNKNOWN;
	switch (imgFormat)
	{
		case PXCImage::PixelFormat::PIXEL_FORMAT_RGB32:
			pxf = bgr ? OF_PIXELS_BGRA : OF_PIXELS_RGBA;
			break;
		case PXCImage::PixelFormat::PIXEL_FORMAT_RGB24:
			pxf = bgr ? OF_PIXELS_BGR : OF_PIXELS_RGB;
			break;
		case PXCImage::PixelFormat::PIXEL_FORMAT_DEPTH:
			pxf = OF_PIXELS_GRAY;
			break;
		default:
			ofLogError("ofxRealSense") << "unsupported pixel format for image -> ofPixels conversion";
			return false;
	}

	// get access to image data in requested format
	PXCImage::ImageData data;
	pxcStatus status = image->AcquireAccess(PXCImage::ACCESS_READ, imgFormat, &data);

	if (status < PXC_STATUS_NO_ERROR) // error check
	{
		ofLogError("ofxRealSense") << "can't access image data, error status: " << status;
		image->ReleaseAccess(&data);
		return false;
	}

	// load image data into ofPixels
	//PXCImage::ImageInfo info = image->QueryInfo();
	//size_t w = info.width;
	//size_t h = info.height;
	//toPixels->setFromPixels(reinterpret_cast<uint8_t *>(data.planes[0]), w, h, OF_PIXELS_GRAY);
	setOfPixels(toPixels, data, image->QueryInfo(), pxf);

	// release access to image
	status = image->ReleaseAccess(&data);
	if (status < PXC_STATUS_NO_ERROR)
	{
		ofLogError("ofxRealSense") << "error releasing access to image data, error status: " << status;
	}
	return true;
}

// template implementationts to avoid linker errors for template definition in .cpp file:
template bool ofxRSUtils::pxcImageToOfPixels<unsigned char>(PXCImage* image, PXCImage::PixelFormat imgFormat, ofPixels* toPixels);
template bool ofxRSUtils::pxcImageToOfPixels<unsigned short>(PXCImage* image, PXCImage::PixelFormat imgFormat, ofShortPixels* toPixels);

// overloaded set pixels function to handle ofPixels or ofShortPixels
void ofxRSUtils::setOfPixels(ofPixels* pix, PXCImage::ImageData& data, PXCImage::ImageInfo info, ofPixelFormat pxf)
{
	pix->setFromPixels(reinterpret_cast<uint8_t *>(data.planes[0]), (size_t)info.width, (size_t)info.height, pxf);
}
void ofxRSUtils::setOfPixels(ofShortPixels* pix, PXCImage::ImageData& data, PXCImage::ImageInfo info, ofPixelFormat pxf)
{
	pix->setFromPixels(reinterpret_cast<uint16_t *>(data.planes[0]), (size_t)info.width, (size_t)info.height, pxf);
}