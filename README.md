# Magnetar

**This project is very early in progress and is not functional!!!**

A lightweight game engine written in C using Vulkan (for now).

## Build and Run Instructions

 You currently need the Vulkan SDK to build magnetar which can be found [here](https://vulkan.lunarg.com/sdk/home).

- In your **bashrc** or **zshrc** file add:
``` bash
source /path/to/VulkanSDK/VERSION/setup-env.sh
```
then
```bash
echo $VULKAN_SDK
```
to check.

You will also need a the **slangc** compiler for the [Slang Shading language](https://shader-slang.org/).


### Windows
Is not supported yet.
### Linux
``` shell
cd magnetar
make build-linux
cd bin
./test
```
For NixOS:
```shell
cd magnetar
nix develop
make build-linux
cd bin
./test
```
### Mac
``` shell
cd magnetar
./build-proj-mac.sh
./compile-shader.sh engine/src/renderer/vulkan/shaders/basic
./compile-shader.sh engine/src/renderer/vulkan/shaders/basic_ui
cd bin
./test
```
