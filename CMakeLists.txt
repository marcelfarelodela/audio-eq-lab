cmake_minimum_required(VERSION 3.16)
project(audio_eq_lab CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(audio_eq_lab
  src/main.cpp
  src/wav.cpp
  src/dsp/biquad.cpp
  src/dsp/eq.cpp
)

target_include_directories(audio_eq_lab PRIVATE src)
