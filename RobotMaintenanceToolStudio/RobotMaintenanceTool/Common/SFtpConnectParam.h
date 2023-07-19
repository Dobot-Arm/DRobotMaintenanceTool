#ifndef SFTPCONNECTPARAM_H
#define SFTPCONNECTPARAM_H

#include<QString>
#include <QMetaType>

struct SFtpConnectParam
{
    QString strHost;
    quint16 iPort;
    QString strUserName;
    QString strPwd;
    int iTimeoutSeconds;
};
Q_DECLARE_METATYPE(SFtpConnectParam)

#endif // SFTPCONNECTPARAM_H
