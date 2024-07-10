#include "planexy.h"
#include <QMatrix4x4>
#include <qmath.h>

PlaneXY::PlaneXY()
{
    const GLfloat x1 = -0.5;
    const GLfloat y1 = -0.5;
    const GLfloat x2 =  0.5;
    const GLfloat y2 = -0.5;
    const GLfloat x3 =  0.5;
    const GLfloat y3 =  0.5;
    const GLfloat x4 = -0.5;
    const GLfloat y4 =  0.5;

    quad(x1, y1, x2, y2, x3, y3, x4, y4);

    scelePlane(100,100,1);
}

PlaneXY::~PlaneXY()
{

}

void PlaneXY::add(const QVector3D &v, const QVector3D &n)
{
    m_data.append(v.x());
    m_data.append(v.y());
    m_data.append(v.z());
    m_data.append(n.x());
    m_data.append(n.y());
    m_data.append(n.z());

    m_count += 6;
}

void PlaneXY::quad(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2, GLfloat x3, GLfloat y3, GLfloat x4, GLfloat y4)
{
    QVector3D n = QVector3D::normal(QVector3D(x1 - x4, y1 - y4, 0.0f), QVector3D(x2 - x4, y2 - y4, 0.0f));

    add(QVector3D(x4, y4, 0.0f), n);
    add(QVector3D(x1, y1, 0.0f), n);
    add(QVector3D(x2, y2, 0.0f), n);

    add(QVector3D(x2, y2, 0.0f), n);
    add(QVector3D(x3, y3, 0.0f), n);
    add(QVector3D(x4, y4, 0.0f), n);
}

void PlaneXY::scelePlane(GLfloat x, GLfloat y, GLfloat z)
{
    QMatrix4x4 scalMat;

    scalMat.setToIdentity();
    scalMat.scale(x,y,z);

    for (int i=0; i<m_data.length(); i+=6)
    {
        QVector3D resVec;

        resVec.setX(m_data[i]);
        resVec.setY(m_data[i+1]);
        resVec.setZ(m_data[i+2]);

        resVec = scalMat * resVec;

        m_data[i] = resVec.x();
        m_data[i+1] = resVec.y();
        m_data[i+2] = resVec.z();
    }
}
