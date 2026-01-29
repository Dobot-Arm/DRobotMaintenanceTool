#include "QMenuIconStyle.h"
#include "Logger.h"

QMenuIconStyle::QMenuIconStyle()
{

}

int QMenuIconStyle::pixelMetric(PixelMetric metric, const QStyleOption *option, const QWidget *widget) const
{
    int s = QProxyStyle::pixelMetric(metric, option, widget);
    if (metric == QStyle::PM_SmallIconSize) {
        s = 30; //action菜单的图标大小
    }
    /*else if (metric == QStyle::PM_MenuPanelWidth)
    {
        s = -4; //级联菜单之间的间距（越大于0二者重叠越多，越小于0二者隔开越远）
    }*/
    return s;
}
