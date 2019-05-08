QT += core gui opengl widgets xml

TARGET   = lf
TEMPLATE = app

QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3 -DNDEBUG -DLIGHTFIELD_VERSION=\\\"1.0.1\\\" -Winvalid-pch
QMAKE_CXXFLAGS_DEBUG   += -Og -D_DEBUG -DLIGHTFIELD_VERSION=\\\"1.0.1\\\" -Winvalid-pch

SOURCES +=                     \
    ../src/debug.cpp           \
    ../src/app.cpp             \
    ../src/main.cpp            \
    ../src/canvas.cpp          \
    ../src/mesh.cpp            \
    ../src/glmesh.cpp          \
    ../src/loader.cpp          \
    ../src/window.cpp          \
    ../src/backdrop.cpp        \
    ../src/shepherd.cpp        \
    ../src/strings.cpp         \
    ../src/printmanager.cpp    \
    ../src/pngdisplayer.cpp    \
    ../src/svgrenderer.cpp     \
    ../src/processrunner.cpp   \
    ../src/signalhandler.cpp   \
    ../src/tabbase.cpp         \
    ../src/filetab.cpp         \
    ../src/preparetab.cpp      \
    ../src/printtab.cpp        \
    ../src/statustab.cpp       \
    ../src/advancedtab.cpp     \
    ../src/maintenancetab.cpp  \
    ../src/constants.cpp       \
    ../src/utils.cpp           \
    ../src/gesturelistview.cpp \
    ../src/hasher.cpp

HEADERS  += \
    ../src/debug.h           \
    ../src/app.h             \
    ../src/canvas.h          \
    ../src/mesh.h            \
    ../src/glmesh.h          \
    ../src/loader.h          \
    ../src/window.h          \
    ../src/backdrop.h        \
    ../src/shepherd.h        \
    ../src/strings.h         \
    ../src/printmanager.h    \
    ../src/printjob.h        \
    ../src/pngdisplayer.h    \
    ../src/svgrenderer.h     \
    ../src/processrunner.h   \
    ../src/signalhandler.h   \
    ../src/tabbase.h         \
    ../src/filetab.h         \
    ../src/preparetab.h      \
    ../src/printtab.h        \
    ../src/statustab.h       \
    ../src/advancedtab.h     \
    ../src/maintenancetab.h  \
    ../src/constants.h       \
    ../src/utils.h           \
    ../src/gesturelistview.h \
    ../src/hasher.h          \
    ../src/uistate.h

CONFIG += c++1z precompile_header
PRECOMPILED_HEADER = ../src/pch.h

RESOURCES += \
    ../images/images.qrc \
    ../gl/gl.qrc         \
    ../text/text.qrc \
    ../fonts/fonts.qrc

linux {
    target.path = /usr/bin
    INSTALLS += target
}

static {
    CONFIG += static
}

debug {
    QMAKE_CXXFLAGS_WARN_ON += -Wno-class-memaccess -Wno-unused-parameter -Wno-unused-result
}
