#include "painter.h"

Painter::Painter(QWidget *parent) :
    QWidget(parent)
{
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
}

QSize Painter::minimumSizeHint() const
{
    return QSize(100, 100);
}

QSize Painter::sizeHint() const
{
    return QSize(800, 600);
}

void Painter::paintEvent(QPaintEvent * /* event */)
{
    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing, false);
    //painter.setCompositionMode(QPainter::CompositionMode_Overlay);

    //Draw rectangles
    painter.setPen(QPen(QBrush(QColor(80, 90, 200, 255), Qt::SolidPattern), 1));
    painter.setBrush(QBrush(QColor(80, 90, 200, 40), Qt::SolidPattern));

    for (int i=0 ; i<rectangles.size() ; i++)
        painter.drawRect(rectangles[i]);

    //Draw bounding box
    painter.setPen(QPen(QBrush(QColor(0, 0, 0, 255), Qt::SolidPattern), 3));
    painter.setBrush(Qt::NoBrush);

    painter.drawRect(boundingBox);
}

void Painter::draw(QList<QRect> const & rectangles, QRect const & boundingBox)
{
    this->rectangles = rectangles;
    this->boundingBox = boundingBox;
    update();
}
