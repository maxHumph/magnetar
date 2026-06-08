# Engine
This directory contains all enegine source code and build systems.

## Note
- Functions marked with **MGAPI** or **MGINLINE** can be used in magnetar applications.
- Building the engine requires the VulkanSDK on the system with the suitable environment variables set (described in README.md).

## About the engine
- The engine uses platform specific code in **src/platform/** for window creation and input using **xcb** for X11 and Wayland (Although I plan on implenting wayland specific platform code in the future), **winapi** for Windows and **Cocoa** for MacOS. (Some of these may not work currently).
- I chose to use Vulkan as the graphics API due to its broad compatibility and low level control. All Vulkan code is located in **src/renderer/vulkan/**. The renderer is use through the generic renderer backend type defined in **renderer_backend.h** to allow for additional graphics APIs to be included in the future.

