#ifndef WIREITEM_H
#define WIREITEM_H
#include "HelperFunctions.h"
#include <QGraphicsItem>
#include <QPainter>
#include <QtMath>
class WireItem : public QGraphicsItem {
public:
    QString name;
    WireItem(const QVector<QPointF>& scenePoints) 
    {
        if(scenePoints.isEmpty()) 
            return;
        // Place the item at the first point and store local points
        QPointF origin=scenePoints.first();
        setPos(origin);
        localPoints.reserve(scenePoints.size());
        for(const QPointF& p:scenePoints) 
            localPoints.append(p-origin);
        setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemIsMovable);
        setAcceptHoverEvents(true);
    }
    QRectF boundingRect() const override 
    {
        if(localPoints.isEmpty()) 
            return QRectF();
        qreal minx=localPoints[0].x(),maxx=localPoints[0].x(),miny=localPoints[0].y(),maxy=localPoints[0].y();
        for(const QPointF& p:localPoints) 
        { 
            minx=qMin(minx,p.x()); 
            maxx=qMax(maxx,p.x()); 
            miny=qMin(miny,p.y()); 
            maxy=qMax(maxy,p.y()); 
        }
        return QRectF(QPointF(minx-4,miny-4),QPointF(maxx+4,maxy+4));
    }
    void paint(QPainter* painter,const QStyleOptionGraphicsItem* option,QWidget* widget) override 
    {
        Q_UNUSED(option); Q_UNUSED(widget);
        painter->setRenderHint(QPainter::Antialiasing);
        QPen pen(Qt::black);
        pen.setWidth(2);
        if(isSelected()) 
            pen.setStyle(Qt::DashLine);
        painter->setPen(pen);
        for(int i=0;i+1<localPoints.size();++i) 
            painter->drawLine(localPoints[i],localPoints[i+1]);
    }
    bool containsPoint(const QPointF& scenePt,qreal tolerance=6.0) const 
    {
        QPointF local=mapFromScene(scenePt);
        for(int i=0;i+1<localPoints.size();++i)
            if(distancePointToSegment(local,localPoints[i],localPoints[i+1])<=tolerance) 
                return true;
        return false;
    }
    QPointF firstPoint() const { return mapToScene(localPoints.isEmpty()?QPointF():localPoints.first()); }
    QPointF lastPoint() const { return mapToScene(localPoints.isEmpty()?QPointF():localPoints.last()); }
    QVector<QPointF> localPoints;
};
#endif // !WIREITEM_H


