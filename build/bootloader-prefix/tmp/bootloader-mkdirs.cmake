# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/Users/z7868/esp-idf/components/bootloader/subproject"
  "E:/project/esp32/flexible-sensing-esp32-/build/bootloader"
  "E:/project/esp32/flexible-sensing-esp32-/build/bootloader-prefix"
  "E:/project/esp32/flexible-sensing-esp32-/build/bootloader-prefix/tmp"
  "E:/project/esp32/flexible-sensing-esp32-/build/bootloader-prefix/src/bootloader-stamp"
  "E:/project/esp32/flexible-sensing-esp32-/build/bootloader-prefix/src"
  "E:/project/esp32/flexible-sensing-esp32-/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "E:/project/esp32/flexible-sensing-esp32-/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "E:/project/esp32/flexible-sensing-esp32-/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
