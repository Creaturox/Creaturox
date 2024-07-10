#ifndef BOX2D_H
#define BOX2D_H

#include <QVector>
#include <QLineF>


class Box2D
{
public:
    Box2D();
    ~Box2D();

    QVector<QLineF> boxLines;
};

#endif // BOX2D_H
