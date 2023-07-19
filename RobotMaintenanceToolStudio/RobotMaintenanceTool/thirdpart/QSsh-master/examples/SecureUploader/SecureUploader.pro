#-------------------------------------------------
#
# Project created by QtCreator 2012-11-22T15:56:43
#
#-------------------------------------------------

QT       += core network

#TEMPLATE = lib
DEFINES += SZRSFTP_LIB QT_NO_DEBUG_OUTPUT
TEMPLATE = app

CONFIG(release, debug|release): {
    TARGET = SecureUploader
} else:CONFIG(debug, debug|release): {
    TARGET = SecureUploaderd
}

QSSH_ROOT = $${PWD}/../..

INCLUDEPATH += $${QSSH_ROOT}/include/ssh
DESTDIR = $${QSSH_ROOT}/lib
unix:LIBS += -L$${QSSH_ROOT}/lib -lQSsh

win32:CONFIG(release, debug|release): LIBS += -L$${QSSH_ROOT}/lib -lQSsh
else:win32:CONFIG(debug, debug|release): LIBS += -L$${QSSH_ROOT}/lib -lQSshd


SOURCES += \
    securefileuploader.cpp \
    SZRSFtpTools.cpp \
    main.cpp

HEADERS  += \
    securefileuploader.h \
    SZRSFtpTools.h


