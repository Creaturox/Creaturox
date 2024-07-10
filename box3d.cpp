#include "box3d.h"

Box3D::Box3D()
{
}

Box3D::~Box3D()
{

}

void Box3D::makeBox3D(const QVector<QPointF> &ref2DPnts, QVector2D &v1, QVector2D &v2)
{
    GLfloat len1 = v1.length();
    GLfloat len2 = v2.length();

    GLfloat x1 = ref2DPnts[1].x();
    GLfloat y1 = ref2DPnts[1].y();
    GLfloat x2 = x1;
    GLfloat y2 = y1+len1;
    GLfloat x3 = x1+len2;
    GLfloat y3 = y1+len1;
    GLfloat x4 = ref2DPnts[1].x()+len2;
    GLfloat y4 = y1;

    quad(x1, y1, x2, y2, x3, y3, x4, y4);

}

void Box3D::add(const QVector3D &v, const QVector3D &n)
{
    boxData.append(v.x());
    boxData.append(v.y());
    boxData.append(v.z());
    boxData.append(n.x());
    boxData.append(n.y());
    boxData.append(n.z());
}

void Box3D::quad(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2, GLfloat x3, GLfloat y3, GLfloat x4, GLfloat y4)
{
    QVector3D n = QVector3D::normal(QVector3D(x4 - x1, y4 - y1, 0.0f), QVector3D(x2 - x1, y2 - y1, 0.0f));

    add(QVector3D(x1, y1, -0.05f), n);
    add(QVector3D(x4, y4, -0.05f), n);
    add(QVector3D(x2, y2, -0.05f), n);

    add(QVector3D(x3, y3, -0.05f), n);
    add(QVector3D(x2, y2, -0.05f), n);
    add(QVector3D(x4, y4, -0.05f), n);
}

