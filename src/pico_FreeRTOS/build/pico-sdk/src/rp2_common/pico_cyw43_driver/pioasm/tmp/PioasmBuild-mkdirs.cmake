# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/sarmad-alvi/Documents/pico/pico-sdk/tools/pioasm"
  "/home/sarmad-alvi/OneDrive/Documents/a University/Fifth Year/capstone-project/src/pico_FreeRTOS/build/pioasm"
  "/home/sarmad-alvi/OneDrive/Documents/a University/Fifth Year/capstone-project/src/pico_FreeRTOS/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm"
  "/home/sarmad-alvi/OneDrive/Documents/a University/Fifth Year/capstone-project/src/pico_FreeRTOS/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/tmp"
  "/home/sarmad-alvi/OneDrive/Documents/a University/Fifth Year/capstone-project/src/pico_FreeRTOS/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/src/PioasmBuild-stamp"
  "/home/sarmad-alvi/OneDrive/Documents/a University/Fifth Year/capstone-project/src/pico_FreeRTOS/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/src"
  "/home/sarmad-alvi/OneDrive/Documents/a University/Fifth Year/capstone-project/src/pico_FreeRTOS/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/src/PioasmBuild-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/sarmad-alvi/OneDrive/Documents/a University/Fifth Year/capstone-project/src/pico_FreeRTOS/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/src/PioasmBuild-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/sarmad-alvi/OneDrive/Documents/a University/Fifth Year/capstone-project/src/pico_FreeRTOS/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/src/PioasmBuild-stamp${cfgdir}") # cfgdir has leading slash
endif()
