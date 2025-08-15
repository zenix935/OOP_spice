#pragma once
#include <qtypes.h>
#include <QGraphicsView>
static qreal distancePointToSegment(const QPointF& p,const QPointF& a,const QPointF& b) 
{
    QLineF lab(a,b);
    qreal l2=lab.length();
    if(l2==0.0) return QLineF(p,a).length();
    // project point p onto line ab param t
    qreal t=((p.x()-a.x())*(b.x()-a.x())+(p.y()-a.y())*(b.y()-a.y()))/(l2*l2);
    t=qMax(qreal(0.0),qMin(qreal(1.0),t));
    QPointF proj(a.x()+t*(b.x()-a.x()),a.y()+t*(b.y()-a.y()));
    return QLineF(p,proj).length();
}