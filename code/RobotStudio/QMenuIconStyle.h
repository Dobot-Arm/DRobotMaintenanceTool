#ifndef QMENUICONSTYLE_H
#define QMENUICONSTYLE_H

#include <QProxyStyle>

class QMenuIconStyle : public QProxyStyle
{
    Q_OBJECT
public:
    QMenuIconStyle();
    virtual ~QMenuIconStyle(){};

    int pixelMetric(QStyle::PixelMetric metric, const QStyleOption *option = nullptr, const QWidget *widget = nullptr) const override;
};

#endif // QMENUICONSTYLE_H
