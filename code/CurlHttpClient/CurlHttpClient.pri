SOURCES += \
    $$PWD/CurlHttpClient.cpp \
    $$PWD/CurlHttpRequest.cpp \
    $$PWD/CurlHttpResponse.cpp

HEADERS += \
    $$PWD/CurlHttpClient.h \
    $$PWD/CurlHttpRequest.h \
    $$PWD/CurlHttpResponse.h

win32{
    contains(QT_ARCH, i386) {
        message("platform-win32-x86")
        LIBS += -L$$PWD/libwin32/ -lcurldll
    } else {
        message("platform-win32-x64")
        LIBS += -L$$PWD/libwin32/ -lcurldll64
    }
}

INCLUDEPATH += $$PWD/include/curl
DEPENDPATH += $$PWD/include/curl
