# Chrome OS Camera Architecture :innocent:
### System Overview ###
![cros_camera_stack](https://github.com/SarahWang729/Internal-Camera-Architecture/assets/111257156/37cc9710-2767-410f-8719-25022b43d987)
# Content #
- Hardware
- Kernel (V4L2)
  - Introduction to V4L
  - V4L Commands to Interact with Video Devices
  - V4L2 framwork
  - USB Video Class (UVC)
  - Internal Topology of the USB camera
  - Three states of the Frame Buffer
  - The Management of Video Buffer
- User Space
  - User Space Services
  - Terminology Explanation
  - The overall workflow of Application
- Source and Reference
## Documentation ##
USB Video Class (UVC) camera implementation guide [Microsoft Official Document](https://learn.microsoft.com/en-us/windows-hardware/drivers/stream/uvc-camera-implementation-guide)
## Hardware ##
![Camera hardware architecture1](https://github.com/user-attachments/assets/256e55da-ae22-4a5f-afba-c6e0466319da) ![Camera hardware architecture2](https://github.com/user-attachments/assets/c029b250-4a77-42d8-b09b-bd6dd3b777ff)

## Kernel ##
### Introduction to V4L ###
- **Video4Linux** is a framework that provides an API for handling video devices in the Linux kernel
- **V4L2** provides a set of tools and utilities that make it easy to configure and test video devices
  - V4L2’s library location:  /usr/include/linux/videodev2.h
  - support for various video formats
    -MPEG-2, MPEG-4, and H.264
  - a set of APIs for applications that need to control video devices
    - set video parameters such as resolution, frame rate, and color format
    - e.g. Gstreamer, OpenCV, and FFMPEG
### V4L Commands to Interact with Video Devices ###
Install V4L2:
```
sudo apt install v4l-utils
```
### V4L2 Framework ###
![v4l2 framework](https://github.com/user-attachments/assets/f97a3db6-acda-4b6f-ae82-4e674e0ae787)
### UVC (USB Video device Class) ###
![uvc](https://github.com/user-attachments/assets/633acc36-9ddb-4ed4-865a-3c52a451446d)
### Internal topology of the USB camera ###
![usb camera](https://github.com/user-attachments/assets/dd67cef3-8c92-410c-9788-2e1b5920be80)
### Example: one single I/O function ###
![I/O flow](https://github.com/user-attachments/assets/6ab98d9e-1ffb-4f50-86d5-c8ab0c3e5133)
### Video buffer management ###
![MMAP](https://github.com/user-attachments/assets/577bda3c-b520-4c6d-b794-45a96e8aa70c)
### 3 states of the frame buffer ###
![buffer](https://github.com/user-attachments/assets/b5c2cc94-1f52-4294-ad55-cbb1d0bc3935)

### User space services ###
- Platform(Board)-specific camera HAL
  - interfaces with the kernel drivers
  - provides all the camera functions (based on Android camera HAL v3 APIs)
- Camera HAL v3 adapter (functions)
  - provides Mojo IPC interface for all the clients on Chrome OS
  - 2 clients on Chrome OS: **Chrome browser** and **Android container**
  - 2 clients connect to the camera HAL adapter through the **Camera Module Mojo IPC interface** and the **Camera Device Mojo IPC interface**
### Terminology ###
- 3A libraries
  - 3A: Auto Exposure, Auto Focus, Auto White-Balance
  - provides the algorithms to control the camera subsystem for capturing high quality images
  - Why isolated? Use **sandboxing** for security consideration
- HAL (Hardware Abstraction Layer)
  - encapsulates diverse vendor implementations and offers a uniform API for top levels
  - offers APIs for applications that allow services to send requests to the device
- Mojo IPC (the IPC in chrome): is 1/3 faster and uses 1/3 fewer context switches than the old chrome IPC
- Encoding / Decoding JPEG images
  - **HW codec**: On platform that supports hardware-accelerated JPEG encoder/decoder, the JPEG libraries connect to the Chrome GPU process through Mojo IPC channels to encode/decode JPEG images
  - **SW codec**: Otherwise done through software
### The overall workflow ###
![workflow](https://github.com/user-attachments/assets/7a35eeaa-3681-4e75-b64e-c37ffb5a6fa0)
