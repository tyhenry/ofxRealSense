#ofxRealSense

Active development, YMMV, things are bound to change...

##Description

ofxRealSense is an openFrameworks addon for the Intel RealSense SR300 depth camera.  
It supports 1080p RGB and 480p depth streams, and 3D face scanning. Soon, it will support 3D face tracking, and hopefully skeleton tracking.  
Tested on Windows 10, openFrameworks 0.9.8, Visual Studio Community 2015.

__Windows only!__  
__Requires Intel RealSense SDK 2016 *R2* (v10.0)__

###Installation:

[Download and install Intel RealSense SDK 2016 __R2__ here](http://registrationcenter-download.intel.com/akdlm/irc_nas/vcp/9078/intel_rs_sdk_offline_package_10.0.26.0396.exe)  
_direct download of 1.8GB exe installation file_

Note: Intel RealSense SDK 2016 R3 (v11.0) was released Dec. 2016, which removed face tracking, person tracking, and other modules from the SDK.  
__As such, this addon requires R2!__  
[More info at Intel RealSense SDK page](https://software.intel.com/en-us/intel-realsense-sdk)

__To create new project:__

* after installing Intel RealSense SDK 2016 R2, clone this github repo into your ofx/addons directory
* use oF project generator, include ofxRealSense addon, open project in VS2015
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

* RGB & Depth camera --> ofPixels & ofTexture
* Image mapping from rgb<-->depth
* Face Scanning module: preview, scan, export to OBJ, MTL, JPG, JSON (face tracking landmarks)

__To Do:__

* threaded synced RGB & Depth capture
* auto point cloud (auto-mesh?)
* face tracking module
* face expression module
* person (skeleton) tracking module

###Stay Tuned!

Pull requests welcome.


