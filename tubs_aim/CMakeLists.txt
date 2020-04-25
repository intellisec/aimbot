cmake_minimum_required(VERSION 3.13.4)
project(tubs)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_STANDARD 11)

include_directories(
    tubs_aim/Include/DirectX
	tubs_aim/Include/nlohmann
    tubs_aim/Include/ValveBSP
    tubs_aim
)

file(GLOB_RECURSE SOURCES
    "tubs_aim/*.cpp"
	"tubs_aim/Include/ValveBSP/*.cpp"
)

add_library(tubs SHARED ${SOURCES})

if(WIN32)
  	target_link_libraries(tubs "../tubs_aim/Lib/x86/d3d9")
	target_link_libraries(tubs "../tubs_aim/Lib/x86/d3dx9")
endif()