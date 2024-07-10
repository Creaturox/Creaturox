#ifndef CUTTINGPLANE_H
#define CUTTINGPLANE_H

#include <QVector>
#include <QPointF>
#include <qopengl.h>
#include <QVector3D>


class CuttingPlane
{
public:
    CuttingPlane();
    ~CuttingPlane();

    void scalePlane(GLfloat x, GLfloat y, GLfloat z);
    void computeMidPnt();
    void computeNormalVec();

    QVector<QPointF> planePnts;
    QPointF midPoint;
    QVector3D placeNormal;
};

#endif // CUTTINGPLANE_H
