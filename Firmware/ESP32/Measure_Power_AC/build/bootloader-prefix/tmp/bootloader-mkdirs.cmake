# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/Espressif/frameworks/esp-idf-v5.3.1/components/bootloader/subproject"
  "D:/Github/MEASURE-POWER-AC/Firmware/ESP32/Measure_Power_AC/build/bootloader"
  "D:/Github/MEASURE-POWER-AC/Firmware/ESP32/Measure_Power_AC/build/bootloader-prefix"
  "D:/Github/MEASURE-POWER-AC/Firmware/ESP32/Measure_Power_AC/build/bootloader-prefix/tmp"
  "D:/Github/MEASURE-POWER-AC/Firmware/ESP32/Measure_Power_AC/build/bootloader-prefix/src/bootloader-stamp"
  "D:/Github/MEASURE-POWER-AC/Firmware/ESP32/Measure_Power_AC/build/bootloader-prefix/src"
  "D:/Github/MEASURE-POWER-AC/Firmware/ESP32/Measure_Power_AC/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "D:/Github/MEASURE-POWER-AC/Firmware/ESP32/Measure_Power_AC/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "D:/Github/MEASURE-POWER-AC/Firmware/ESP32/Measure_Power_AC/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
