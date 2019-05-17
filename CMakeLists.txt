#### Fast .stl viewer #####
# Original Project Author: Matt Keeter Copyright 2014 -2017
# Author: Paul Tsouchlos Copyright 2017

cmake_minimum_required(VERSION 3.3)

# Setting -std=c++17
set(CMAKE_CXX_STANDARD 17)
# Setting standard to required, as requisted by DeveloperPaul123 on github
set(CXX_STANDARD_REQUIRED ON)

# Set the version number
set (LF_VERSION_MAJOR "1")
set (LF_VERSION_MINOR "0")
set (LF_VERSION_PATCH "1")
set (PROJECT_VERSION "${LF_VERSION_MAJOR}.${LF_VERSION_MINOR}.${LF_VERSION_PATCH}")

if(WIN32)
# Set the Qt5 path
set (Qt5_DIR "F:\\\\Qt\\\\5.12.3\\\\msvc2017_64\\\\lib\\\\cmake\\\\Qt5")
endif(WIN32)

project(lf)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)

#set project sources
set(Project_Sources src/debug.cpp
src/app.cpp
src/main.cpp
src/canvas.cpp
src/mesh.cpp
src/glmesh.cpp
src/loader.cpp
src/window.cpp
src/backdrop.cpp
src/shepherd.cpp
src/strings.cpp
src/printmanager.cpp
src/pngdisplayer.cpp
src/svgrenderer.cpp
src/processrunner.cpp
src/signalhandler.cpp
src/tabbase.cpp
src/filetab.cpp
src/preparetab.cpp
src/printtab.cpp
src/statustab.cpp
src/advancedtab.cpp
src/maintenancetab.cpp
src/constants.cpp
src/utils.cpp
src/gesturelistview.cpp
src/hasher.cpp)

#set project headers. 
set(Project_Headers src/debug.h
src/pch.h
src/app.h
src/canvas.h
src/mesh.h
src/glmesh.h
src/loader.h
src/window.h
src/backdrop.h
src/shepherd.h
src/strings.h
src/printmanager.h
src/printjob.h
src/pngdisplayer.h
src/svgrenderer.h
src/processrunner.h
src/signalhandler.h
src/tabbase.h
src/filetab.h
src/preparetab.h
src/printtab.h
src/statustab.h
src/advancedtab.h
src/maintenancetab.h
src/constants.h
src/utils.h
src/gesturelistview.h
src/hasher.h
src/uistate.h)

#set project resources and icon resource
set(Project_Resources images/images.qrc gl/gl.qrc text/text.qrc)

#set Policy CMP0072 FindOpenGL behavior
set(OpenGL_GL_PREFERENCE GLVND)

#find required packages. 
find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets OpenGL Xml)
find_package(OpenGL REQUIRED)
find_package(Threads REQUIRED)

#add resources to RCC
qt5_add_resources(Project_Resources_RCC ${Project_Resources})

#tell CMake AUTOGEN to skip autogen on the generated qrc files
set_property(SOURCE ${Project_Resources_RCC} PROPERTY SKIP_AUTOGEN ON)

#include opengl files. 
include_directories(${QT_QTOPENGL_INCLUDE_DIR} ${OPENGL_INCLUDE_DIR} )

add_executable(lf WIN32 ${Project_Sources} ${Project_Headers} ${Project_Resources_RCC} ${Icon_Resource})
target_link_libraries(lf Qt5::Widgets Qt5::Core Qt5::Gui Qt5::OpenGL ${OPENGL_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
if(WIN32)
  set(Fstl_LINK_FLAGS ${CMAKE_CURRENT_SOURCE_DIR}/${Icon_Resource})
  set_target_properties(lf PROPERTIES LINK_FLAGS ${Fstl_LINK_FLAGS})
endif(WIN32)

# Add version definitions to use within the code. 
target_compile_definitions(lf PRIVATE -DLF_VERSION="${PROJECT_VERSION}")

#installer information that is platform independent
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Fast .stl file viewer.")
set(CPACK_PACKAGE_VERSION_MAJOR ${LF_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${LF_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${LF_VERSION_PATCH})

install(TARGETS lf RUNTIME DESTINATION bin)

set(CPACK_GENERATOR "DEB;RPM")
set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-${PROJECT_VERSION}")

include(CPack)
