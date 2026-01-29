#ifndef FORMLOOKXMLVERSION_H
#define FORMLOOKXMLVERSION_H

#include <QWidget>
#include "baseUI/UIBaseWidget.h"
#include <QResizeEvent>

namespace Ui {
class FormLookXmlVersion;
}

class FormLookXmlVersion : public UIBaseWidget
{
    Q_OBJECT

public:
    explicit FormLookXmlVersion(QWidget *parent = nullptr);
    ~FormLookXmlVersion();

private:
    void readXml();
    void resizeEvent(QResizeEvent *event) override;
private:
    Ui::FormLookXmlVersion *ui;
    static QString m_strXmlVersion;
    static QString m_strXmlVersionOld;
};

#endif // FORMLOOKXMLVERSION_H
