QT += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QSSH_ROOT = $${PWD}/..
DESTDIR   = $${QSSH_ROOT}/lib

TEMPLATE = lib
DEFINES += QSSH_LIBRARY QT_NO_DEBUG_OUTPUT QT_NO_INFO_OUTPUT QT_NO_WARNING_OUTPUT

TARGET  = $$qtLibraryTarget(QSsh)

CONFIG += c++11

include ($${PWD}/ssh.pri)


