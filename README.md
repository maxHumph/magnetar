# Magnetar
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


### Windows
Is not supported yet.
### Linux
``` shell
cd magnetar
./build-proj-linux.sh
cd bin
./test
```
### Mac
``` shell
cd magnetar
./build-proj-mac.sh
cd bin
./test
```
