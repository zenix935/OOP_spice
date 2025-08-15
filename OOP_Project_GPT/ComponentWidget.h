#ifndef COMPONENTWIDGET_H
#define COMPONENTWIDGET_H
#include <QLabel>
class ComponentWidget : public QLabel 
{
    Q_OBJECT
public:
    ComponentWidget(const QString& type,QWidget* parent=nullptr) : QLabel(parent),m_type(type) 
    {
        setText(type);
        setAlignment(Qt::AlignCenter);
        setFixedSize(60,40);
        setAttribute(Qt::WA_StyledBackground);
        setStyleSheet("QLabel{background:#070707;border:1px solid #333;border-radius:1px;}");
        setContextMenuPolicy(Qt::DefaultContextMenu);
    }
    QString type() const { return m_type; }
    QString name;
    QString value;
    QString amplitude;
    QString phase;
    QString frequency;
signals:
    void doubleClicked();
protected:
    void mouseDoubleClickEvent(QMouseEvent* ev) override 
    {
        emit doubleClicked();
        QLabel::mouseDoubleClickEvent(ev);
    }
private:
    QString m_type;
};
#endif
