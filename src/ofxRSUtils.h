#pragma once
#include "ofMain.h"
#include "pxcsensemanager.h"	// main

namespace ofxRSUtils
{
	// check if processor is little-endian (e.g. Intel), if so channel order is BGR not RGB
	inline bool isLittleEndian()
	{
		short int number = 0x1;
		char *numPtr = (char*)&number;
		return (numPtr[0] == 1);
	}

	// convert PXCImage to ofPixels instance (or ofShortPixels)
	template <class PixelType>
	bool pxcImageToOfPixels(PXCImage* image, PXCImage::PixelFormat imgFormat, ofPixels_<PixelType>* toPixels);

	// copy pixels from pxc image data to ofPixels (used by above template function)
	void setOfPixels(ofPixels* pix, PXCImage::ImageData& data, PXCImage::ImageInfo info, ofPixelFormat pxf);
	void setOfPixels(ofShortPixels* pix, PXCImage::ImageData& data, PXCImage::ImageInfo info, ofPixelFormat pxf);

}

	//// utility to check if processor is little-endian (e.g. Intel)
	//// if little-endian, Intel uses BGR (not RGB) pixel channel order
	//bool isLittleEndian()
	//{
	//	short int number = 0x1;
	//	char *numPtr = (char*)&number;
	//	return (numPtr[0] == 1);
	//}

	//// converts PXCImage to ofPixels (or ofShortPixels)
	//template <class PixelType>
	//bool pxcImageToOfPixels(PXCImage* image, PXCImage::PixelFormat imgFormat, ofPixels_<PixelType>* toPixels)
	//{
	//	if (image == nullptr || toPixels == nullptr) return false;
	//	bool bgr = isLittleEndian(); // bgr or rgb order of PXCImage data?

	//	// check image format, only RGBA, RGB, or grayscale supported for now
	//	ofPixelFormat pxf = OF_PIXELS_UNKNOWN;
	//	switch (imgFormat)
	//	{
	//		case PXCImage::PixelFormat::PIXEL_FORMAT_RGB32:
	//			pxf = bgr ? OF_PIXELS_BGRA : OF_PIXELS_RGBA;
	//			break;
	//		case PXCImage::PixelFormat::PIXEL_FORMAT_RGB24:
	//			pxf = bgr ? OF_PIXELS_BGR : OF_PIXELS_RGB;
	//			break;
	//		case PXCImage::PixelFormat::PIXEL_FORMAT_DEPTH:
	//			pxf = OF_PIXELS_GRAY;
	//			break;
	//		default:
	//			ofLogError("ofxRealSense") << "unsupported pixel format for image -> ofPixels conversion";
	//			return false;
	//	}

	//	// get access to image data in requested format
	//	PXCImage::ImageData data;
	//	pxcStatus status = image->AcquireAccess(PXCImage::ACCESS_READ, imgFormat, &data);

	//	if (status < PXC_STATUS_NO_ERROR) // error check
	//	{
	//		ofLogError("ofxRealSense") << "can't access image data, error status: " << status;
	//		image->ReleaseAccess(&data);
	//		return false;
	//	}

	//	// load image data into ofPixels
	//	//PXCImage::ImageInfo info = image->QueryInfo();
	//	//size_t w = info.width;
	//	//size_t h = info.height;
	//	//toPixels->setFromPixels(reinterpret_cast<uint8_t *>(data.planes[0]), w, h, OF_PIXELS_GRAY);
	//	setOfPixels(toPixels, data, image->QueryInfo(), pxf);

	//	// release access to image
	//	status = image->ReleaseAccess(&data);
	//	if (status < PXC_STATUS_NO_ERROR)
	//	{
	//		ofLogError("ofxRealSense") << "error releasing access to image data, error status: " << status;
	//	}
	//	return true;
	//}

	//// overloaded set pixels function to handle ofPixels or ofShortPixels
	//void setOfPixels(ofPixels* pix, PXCImage::ImageData& data, PXCImage::ImageInfo info, ofPixelFormat pxf)
	//{
	//	pix->setFromPixels(reinterpret_cast<uint8_t *>(data.planes[0]), (size_t)info.width, (size_t)info.height, pxf);
	//}
	//void setOfPixels(ofShortPixels* pix, PXCImage::ImageData& data, PXCImage::ImageInfo info, ofPixelFormat pxf)
	//{
	//	pix->setFromPixels(reinterpret_cast<uint16_t *>(data.planes[0]), (size_t)info.width, (size_t)info.height, pxf);
	//}






