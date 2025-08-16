#pragma once
#include <QLabel>

class LabelNetWidget : public QLabel
{
	Q_OBJECT
public:
    LabelNetWidget(const QString& name,QWidget* parent=nullptr) : QLabel(parent),m_name(name)
    {
        setText(name);
        setAlignment(Qt::AlignCenter);
        setFixedSize(30,20); 
        setAttribute(Qt::WA_StyledBackground);
        setStyleSheet("QLabel{background:#777777;border:1px solid #333;border-radius:1px;color:#00FFFF;font-size:10px;}");
        setContextMenuPolicy(Qt::DefaultContextMenu);
    }
    QString name() { return m_name; }
    void setName(const QString& n) { m_name=n; }
signals:
    void doubleClicked();
protected:
    void mouseDoubleClickEvent(QMouseEvent* ev) override
    {
        emit doubleClicked();
        QLabel::mouseDoubleClickEvent(ev);
    }
private:
    QString m_name;
};