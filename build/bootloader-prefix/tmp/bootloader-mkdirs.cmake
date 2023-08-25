# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "D:/application/ESPIDFcontainer/esp-idf/components/bootloader/subproject"
  "D:/Desktop/flexible-sensing-esp32-/build/bootloader"
  "D:/Desktop/flexible-sensing-esp32-/build/bootloader-prefix"
  "D:/Desktop/flexible-sensing-esp32-/build/bootloader-prefix/tmp"
  "D:/Desktop/flexible-sensing-esp32-/build/bootloader-prefix/src/bootloader-stamp"
  "D:/Desktop/flexible-sensing-esp32-/build/bootloader-prefix/src"
  "D:/Desktop/flexible-sensing-esp32-/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "D:/Desktop/flexible-sensing-esp32-/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "D:/Desktop/flexible-sensing-esp32-/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
