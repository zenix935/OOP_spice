#pragma once
#include <QGraphicsScene>
#include <QPainter>
class GridScene : public QGraphicsScene 
{
public:
    GridScene(int grid=20,QObject* parent=nullptr) : QGraphicsScene(parent),gridSize(grid) {}
    int gridSize;
protected:
    void drawBackground(QPainter* painter,const QRectF& rect) override 
    {
        painter->fillRect(rect,Qt::white);
        QPen pen(Qt::lightGray);
        pen.setWidth(0);
        painter->setPen(pen);
        int step=gridSize;
        const int left=std::floor(rect.left());
        const int top=std::floor(rect.top());
        for(int x=left-(left%step); x<rect.right(); x+=step)
            painter->drawLine(x,rect.top(),x,rect.bottom());
        for(int y=top-(top%step); y<rect.bottom(); y+=step)
            painter->drawLine(rect.left(),y,rect.right(),y);
    }
};