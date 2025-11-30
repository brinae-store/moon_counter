# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/Users/baing/esp/esp-idf/components/bootloader/subproject"
  "C:/Users/baing/iCloudDrive/3D Stuff/For Erin/moon/Dev/moon_display/tests/build/bootloader"
  "C:/Users/baing/iCloudDrive/3D Stuff/For Erin/moon/Dev/moon_display/tests/build/bootloader-prefix"
  "C:/Users/baing/iCloudDrive/3D Stuff/For Erin/moon/Dev/moon_display/tests/build/bootloader-prefix/tmp"
  "C:/Users/baing/iCloudDrive/3D Stuff/For Erin/moon/Dev/moon_display/tests/build/bootloader-prefix/src/bootloader-stamp"
  "C:/Users/baing/iCloudDrive/3D Stuff/For Erin/moon/Dev/moon_display/tests/build/bootloader-prefix/src"
  "C:/Users/baing/iCloudDrive/3D Stuff/For Erin/moon/Dev/moon_display/tests/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/Users/baing/iCloudDrive/3D Stuff/For Erin/moon/Dev/moon_display/tests/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "C:/Users/baing/iCloudDrive/3D Stuff/For Erin/moon/Dev/moon_display/tests/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
