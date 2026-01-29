#ifndef ZIPDIYMANAGER_H
#define ZIPDIYMANAGER_H

#include "zips/zip.h"
#include "zips/unzip.h"
#include <QString>
#include <QMap>
#include <QDebug>
#include <QDir>
#include <windows.h>
class ZipDiyManager : public QObject
{
    Q_OBJECT
public:
    ZipDiyManager();
    virtual ~ZipDiyManager(){}
    bool zipDiyCompress(QString strSrcDir,QString strDstZip,QString password);
    bool zipUnCompress(QString strSrcZip, QString strDstPath,QString password);
    bool zipCompressFilterDir(QString strSrcDir,QString strDstZip,QString password,QStringList filteredDirList,QString strSelectedDir);
};

#endif // ZIPDIYMANAGER_H
