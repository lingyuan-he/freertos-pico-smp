Setup
=====

sudo apt install git cmake gcc-arm-none-eabi build-essential minicom  
mkdir freertos-pico  
cd freertos-pico  
git clone https://github.com/RaspberryPi/pico-sdk --recurse-submodules  
git clone https://github.com/FreeRTOS/FreeRTOS-Kernel --recurse-submodules  
git clone https://github.com/lingyuan-he/freertos-pico-smp.git  
export PICO_SDK_PATH=$PWD/pico-sdk  
export FREERTOS_KERNEL_PATH=$PWD/FreeRTOS-Kernel  

Compile a sample
================

smpTasks for example:  
cd freertos-pico-smp/smpTasks  
cmake .  
make  

Put onto Pico
=============

Connect Pico while pressing BOOTSEL  
cp smpTasks.uf2 /media/<user>/RPI-RP2  
Pico will reset  

Read serial with:  
sudo minicom -b 115200 -o -D /dev/ttyACM0
