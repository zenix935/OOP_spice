#ifndef COMPONENT_ITEM
#define COMPONENT_ITEM
#include <QGraphicsProxyWidget>
#include <ComponentWidget.h>
class ComponentItem : public QGraphicsProxyWidget 
{
public:
    ComponentItem(ComponentWidget* w) : QGraphicsProxyWidget(nullptr) 
    {
        setWidget(w);
        setFlag(QGraphicsItem::ItemIsMovable);
        setFlag(QGraphicsItem::ItemIsSelectable);
        setAcceptHoverEvents(true);
    }
    void setGridSize(int g) { grid=g; }
    void snapToGrid() 
    {
        QPointF p=pos();
        qreal x=std::round(p.x()/grid)*grid;
        qreal y=std::round(p.y()/grid)*grid;
        setPos(x,y);
    }
    void rotateCW() { setRotation(rotation()+90); }
    QPointF pinPositionLeft() const 
    {
        QRectF r=sceneBoundingRect();
        return QPointF(r.left(),r.top()+r.height()/2);
    }
    QPointF pinPositionRight() const 
    {
        QRectF r=sceneBoundingRect();
        return QPointF(r.right(),r.top()+r.height()/2);
    }
protected:
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* ev) override 
    {
        QGraphicsProxyWidget::mouseReleaseEvent(ev);
        snapToGrid();
    }
private:
    int grid=20;
};


#endif // COMPONENT_ITEM