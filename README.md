#ofxRealSense

##WORK IN PROGRESS! Your mileage may vary. This addon is in active development.

##Description

ofxRealSense is an openFrameworks addon for the Intel RealSense SR300 depth camera.  Windows only.  
Tested with Windows 10, openFrameworks 0.9.8, Visual Studio Community 2015  

*Requires Intel RealSense SDK 2016 R2 (a.k.a v10.0)*

###Installation:

[Download and install Intel RealSense SDK 2016 *R2* here](http://registrationcenter-download.intel.com/akdlm/irc_nas/vcp/9078/intel_rs_sdk_offline_package_10.0.26.0396.exe)  
_Note: This is a direct download of an 1.8GB exe installation file._  
(Also: Intel RealSense SDK R3 was released early Dec. 2016, which removed face tracking, person tracking, and other modules from the SDK.  *This addon requires R2!!!*  [More info at Intel RealSense SDK page](https://software.intel.com/en-us/intel-realsense-sdk))

__To create new project:__

* use project generator, include ofxRealSense addon, open project in VS2015
* right click on project in Solution Explorer, select Properties
* change "Configuration" to "All Configurations" and "Platform" to "All Platforms"
* under "C\C++" > "General" > "Additional Include Directories", add:  
$(RSSDK_DIR)include;
* under "Linker" > "General" > Additional Library Directories", add:  
$(RSSDK_DIR)lib\$(Platform);

example:  
![additional include and library directories](https://raw.githubusercontent.com/tyhenry/ofxRealSense/master/readme_addDirs.png)


###Version Notes:

This addon is based on Seth Gibson's [ofxRSSDKv2](https://github.com/tyhenry/ofxRSSDKv2), but with a re-structured interface and design.

####Features:

__In Progress/Working__

* (unsynced) RGB & Depth camera --> ofPixels & ofTexture
* Face Scanning module: preview, scan, export to OBJ, MTL, JPG, JSON (face tracking landmarks)

__To Do:__

* threaded synced RGB & Depth capture
* auto point cloud (auto-mesh?)
* face tracking module
* face expression module
* person (skeleton) tracking module

#Stay Tuned!

Pull requests welcome.


