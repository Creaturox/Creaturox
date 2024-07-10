#ifndef PLANEXY_H
#define PLANEXY_H

#include <qopengl.h>
#include <QVector>
#include <QVector3D>

class PlaneXY
{
public:
    PlaneXY();
    int count() const { return m_count; }
    const GLfloat *constData() const { return m_data.constData(); }
    int vertexCount() const { return m_count / 6; }
    void scelePlane(GLfloat x, GLfloat y, GLfloat z);
    ~PlaneXY();

private:
    void quad(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2, GLfloat x3, GLfloat y3, GLfloat x4, GLfloat y4);
    void add(const QVector3D &v, const QVector3D &n);

    QVector<GLfloat> m_data; // this is our complete data for BoxyMuscle geometry including vertices and normals
    int m_count;
};

#endif // PLANEXY_H
