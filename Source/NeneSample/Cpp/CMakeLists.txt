cmake_minimum_required (VERSION 2.8)

project(Example)

get_filename_component(ROOT_DIR ${PROJECT_SOURCE_DIR} DIRECTORY)

include_directories(${ROOT_DIR}/External/Inc)

link_directories(${PROJECT_SOURCE_DIR} ${ROOT_DIR}/External/Lib/x86/windows)

aux_source_directory(. SAMPLE_SRC)

add_executable(${PROJECT_NAME} ${SAMPLE_SRC})

if("${GRAPIC_API}" STREQUAL "NENE_GL")
	target_link_libraries(${PROJECT_NAME} opengl32 glfw3 glew32s assimp FreeImage)
elseif("${GRAPIC_API}" STREQUAL "NENE_DX")
	target_link_libraries(${PROJECT_NAME} dxgi d3d11 D3DCompiler dinput8 dxguid assimp)
endif()

target_link_libraries(${PROJECT_NAME} NeneEngine)

if("${GRAPIC_API}" STREQUAL "NENE_GL")
	ADD_DEFINITIONS(-DNENE_GL)
elseif("${GRAPIC_API}" STREQUAL "NENE_DX")
	ADD_DEFINITIONS(-DNENE_DX)
endif()

set(CMAKE_CXX_FLAGS "-std=c++11")

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory "${ROOT_DIR}/External/Bin/x86/windows/" $<TARGET_FILE_DIR:${PROJECT_NAME}>)
