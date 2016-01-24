#ifndef PAINTER_H
#define PAINTER_H

#include <QWidget>
#include <QPainter>

class Painter : public QWidget
{
    Q_OBJECT
public:
    explicit Painter(QWidget *parent = 0);

    QSize minimumSizeHint() const Q_DECL_OVERRIDE;
    QSize sizeHint() const Q_DECL_OVERRIDE;

private:
    QList<QRect> rectangles;
    QRect boundingBox;

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

signals:

public slots:
    void draw(QList<QRect> const & rectangles, QRect const & boundingBox);

};

#endif // PAINTER_H
