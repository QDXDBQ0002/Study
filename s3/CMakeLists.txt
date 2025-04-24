cmake_minimum_required(VERSION 3.10)

# Set the project name and version
project(DirectXTriangle VERSION 1.0)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Windows SDK paths
set(WINDOWS_SDK_DIR "E:/Windows Kits/10")
set(WINDOWS_SDK_VERSION "10.0.22621.0")

# Include directories
include_directories(include)
include_directories("${WINDOWS_SDK_DIR}/Include/${WINDOWS_SDK_VERSION}/ucrt")
include_directories("${WINDOWS_SDK_DIR}/Include/${WINDOWS_SDK_VERSION}/shared")
include_directories("${WINDOWS_SDK_DIR}/Include/${WINDOWS_SDK_VERSION}/um")
include_directories("${WINDOWS_SDK_DIR}/Include/${WINDOWS_SDK_VERSION}/winrt")

# Add all source files
file(GLOB SOURCES "src/*.cpp")

# Add the executable
add_executable(DirectXTriangle WIN32 ${SOURCES})

# Link DirectX libraries
target_link_libraries(DirectXTriangle 
    d3d11.lib 
    d3dcompiler.lib
    dxgi.lib
    dxguid.lib
    winmm.lib
)