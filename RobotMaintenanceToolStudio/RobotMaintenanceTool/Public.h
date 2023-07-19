#ifndef PUBLIC_H
#define PUBLIC_H

#include <QFile>

#define SET_STYLE_SHEET(className)                \
do{                                               \
    QString strQss;                               \
    QFile file(":/qss/qss/"#className".qss");     \
    if (file.open(QIODevice::ReadOnly)){          \
        strQss = QLatin1String(file.readAll());   \
        file.close();                             \
        this->setStyleSheet(strQss);              \
    }                                             \
}while(false)

namespace Public
{

};

#endif // PUBLIC_H
