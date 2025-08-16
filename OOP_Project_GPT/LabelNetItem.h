#pragma once
#include <QGraphicsProxyWidget>
#include "LabelNetWidget.h"

class LabelNetItem : public QGraphicsProxyWidget
{
public:
    LabelNetItem(LabelNetWidget* w) : QGraphicsProxyWidget(nullptr)
    {
        setWidget(w);
        setFlag(QGraphicsItem::ItemIsMovable);
        setFlag(QGraphicsItem::ItemIsSelectable);
        setAcceptHoverEvents(true);
    }
protected:
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* ev) override { QGraphicsProxyWidget::mouseReleaseEvent(ev); }
};