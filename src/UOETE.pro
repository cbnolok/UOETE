#-------------------------------------------------
#
# Project created by QtCreator 2018-08-16T18:14:05
#
#-------------------------------------------------

QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
CONFIG += c++14

TARGET = UOETE
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
    main.cpp \
    uoppackage/uopblock.cpp \
    uoppackage/uopcompression.cpp \
    uoppackage/uoperror.cpp \
    uoppackage/uopfile.cpp \
    uoppackage/uophash.cpp \
    uoppackage/uopheader.cpp \
    uoppackage/uoppackage.cpp \
    uoclientfiles/uoart.cpp \
    uoclientfiles/uoidx.cpp \
    uoclientfiles/uohues.cpp \
    uoclientfiles/libsquish/alpha.cpp \
    uoclientfiles/libsquish/clusterfit.cpp \
    uoclientfiles/libsquish/colourblock.cpp \
    uoclientfiles/libsquish/colourfit.cpp \
    uoclientfiles/libsquish/colourset.cpp \
    uoclientfiles/libsquish/maths.cpp \
    uoclientfiles/libsquish/rangefit.cpp \
    uoclientfiles/libsquish/singlecolourfit.cpp \
    uoclientfiles/libsquish/singlecolourlookup.inl \
    uoclientfiles/libsquish/squish.cpp \
    cpputils/strings.cpp \
    cpputils/sysio.cpp \
    mainwindow.cpp \
    uotileart.cpp \
    uostringdictionary.cpp \
    uoclientfiles/ddsinfo.cpp

HEADERS += \
    uoppackage/uopblock.h \
    uoppackage/uopcompression.h \
    uoppackage/uoperror.h \
    uoppackage/uopfile.h \
    uoppackage/uophash.h \
    uoppackage/uoppackage.h \
    uoppackage/zconf.h \
    uoppackage/zlib.h \
    uoclientfiles/uoart.h \
    uoclientfiles/uoidx.h \
    uoclientfiles/uohues.h \
    uoclientfiles/libsquish/alpha.h \
    uoclientfiles/libsquish/clusterfit.h \
    uoclientfiles/libsquish/colourblock.h \
    uoclientfiles/libsquish/colourfit.h \
    uoclientfiles/libsquish/colourset.h \
    uoclientfiles/libsquish/config.h \
    uoclientfiles/libsquish/maths.h \
    uoclientfiles/libsquish/rangefit.h \
    uoclientfiles/libsquish/simd.h \
    uoclientfiles/libsquish/simd_float.h \
    uoclientfiles/libsquish/simd_sse.h \
    uoclientfiles/libsquish/simd_ve.h \
    uoclientfiles/libsquish/singlecolourfit.h \
    uoclientfiles/libsquish/squish.h \
    cpputils/strings.h \
    cpputils/sysio.h \
    mainwindow.h \
    uotileart.h \
    uostringdictionary.h \
    uoclientfiles/ddsinfo.h

FORMS += \
    mainwindow.ui

###### Compiler/Linker settings

# windows
win32:!unix
{
    contains(QMAKE_CC, gcc) {
        # MinGW

        #   Dynamically link zlib
        contains(QT_ARCH, x86_64) {
            LIBS += -L\"$$PWD\\..\winlibs\\64\" -lz
        } else {
            LIBS += -L\"$$PWD\\..\winlibs\\32\" -lz
        }
    }

    contains(QMAKE_CC, cl) {
        # Visual Studio

        #   Dynamically link zlib
        contains(QT_ARCH, x86_64) {
            LIBS += -L\"$$PWD\\..\winlibs\\64\" -lzlib
        } else {
            LIBS += -L\"$$PWD\\..\winlibs\\32\" -lzlib
        }
        DEFINES += "ZLIB_WINAPI=1"
        DEFINES += "ZLIB_DLL=1"
    }

    contains(QMAKE_COPY, copy) {    # When it's compiled by AppVeyor, QMAKE_COPY is cp -f, not copy
                                    # this way, we'll copy dlls to build directory only when building locally on Windows
        contains(QT_ARCH, x86_64) {
            QMAKE_PRE_LINK += $$QMAKE_COPY \"$$PWD\\..\winlibs\\64\\*.dll\" \"$$DESTDIR\"
        } else {
            QMAKE_PRE_LINK += $$QMAKE_COPY \"$$PWD\\..\winlibs\\32\\*.dll\" \"$$DESTDIR\"
        }
    }
}

# linux
unix {
    LIBS += -lz         # dynamically link zlib
}
