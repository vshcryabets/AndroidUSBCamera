Original project here - https://github.com/jiangdongguo/AndroidUSBCamera

**Purpose of this project:** Prepare an easy-to-use library for Android applications that allows:
1. Getting video from UVC devices.
1. Getting audio from UAC devices.

## How to build

1. Clone this repository.
2. This project contains a git submodule in the libuvc-git directory. Initialize it by running:

   ```bash
   git submodule update --init --recursive
   ```
1. Open the project in Android Studio.
1. Make sure you have the NDK installed **27.0.12077973**. Also, you will need CMake **3.31.6**.
1. Build the project.


## Permissions

Android now often implicitly requires the CAMERA permission in addition to the explicit USB 
permission, especially when your app's targetSdkVersion is 28 or higher.
