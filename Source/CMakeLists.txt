cmake_minimum_required (VERSION 2.8)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

set(GRAPIC_API "NENE_GL" CACHE STRING "Select which graphic api you want to use.")

set_property(CACHE GRAPIC_API PROPERTY STRINGS NENE_GL NENE_DX)

project(NeneEngine)

add_subdirectory(Sample)

add_subdirectory(Source)
