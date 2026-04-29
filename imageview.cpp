#include "imageview.h"
#include <QGraphicsScene>
#include <QGraphicsLineItem>

ImageView::ImageView(QWidget *parent)
    : QGraphicsView(parent), drawing(false)
{
    setDragMode(QGraphicsView::NoDrag);
}

void ImageView::wheelEvent(QWheelEvent *event)
{
    double scaleFactor = 1.15;

    if(event->delta() > 0)
        scale(scaleFactor, scaleFactor);
    else
        scale(1.0/scaleFactor, 1.0/scaleFactor);
}

void ImageView::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        startPoint = mapToScene(event->pos());
        drawing = true;
    }
    lastPos = event->pos();
}

void ImageView::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons() & Qt::RightButton)
    {
        QPointF delta = mapToScene(event->pos()) - mapToScene(lastPos);
        translate(delta.x(), delta.y());
    }
    lastPos = event->pos();
}

void ImageView::mouseReleaseEvent(QMouseEvent *event)
{
    if(drawing)
    {
        QPointF endPoint = mapToScene(event->pos());
        scene()->addLine(QLineF(startPoint, endPoint));

        double dist = QLineF(startPoint, endPoint).length();
        scene()->addText(QString::number(dist))->setPos(endPoint);

        drawing = false;
    }
}
