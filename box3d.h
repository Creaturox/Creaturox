#ifndef BOX3D_H
#define BOX3D_H

#include <qopengl.h>
#include <QVector>
#include <QVector3D>
#include <QVector2D>
#include <QPointF>

class Box3D
{

public:
    Box3D();
    ~Box3D();

    void makeBox3D(const QVector<QPointF> &ref2DPnts, QVector2D &v1, QVector2D &v2);

    const GLfloat *constData() const
    {
        return boxData.constData();
    }

    int numElem() const
    {
        return boxData.length();
    }


    int vertexCount() const
    {
        return boxData.length()/6;
    }


private:
    // A box will have its vertices, and normals
    QVector<GLfloat> boxData;


    void quad(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2, GLfloat x3, GLfloat y3, GLfloat x4, GLfloat y4);
//    void extrude(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2);
    void add(const QVector3D &v, const QVector3D &n);
};

#endif // BOX3D_H

//D:/Ismail Kazmi/Bournemouth University/PhD/Software/glm
