# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/Espressif/frameworks/esp-idf-v5.2.2/components/bootloader/subproject"
  "D:/OneDrive/Disk_2/MEASURE-POWER-AC/Firmware/ESP32/Measure_Power_AC/build/bootloader"
  "D:/OneDrive/Disk_2/MEASURE-POWER-AC/Firmware/ESP32/Measure_Power_AC/build/bootloader-prefix"
  "D:/OneDrive/Disk_2/MEASURE-POWER-AC/Firmware/ESP32/Measure_Power_AC/build/bootloader-prefix/tmp"
  "D:/OneDrive/Disk_2/MEASURE-POWER-AC/Firmware/ESP32/Measure_Power_AC/build/bootloader-prefix/src/bootloader-stamp"
  "D:/OneDrive/Disk_2/MEASURE-POWER-AC/Firmware/ESP32/Measure_Power_AC/build/bootloader-prefix/src"
  "D:/OneDrive/Disk_2/MEASURE-POWER-AC/Firmware/ESP32/Measure_Power_AC/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "D:/OneDrive/Disk_2/MEASURE-POWER-AC/Firmware/ESP32/Measure_Power_AC/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "D:/OneDrive/Disk_2/MEASURE-POWER-AC/Firmware/ESP32/Measure_Power_AC/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
