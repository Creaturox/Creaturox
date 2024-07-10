/****************************************************************************
**
** Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/
#ifndef BoxyMuscle_H
#define BoxyMuscle_H

#include <qopengl.h>
#include <QVector>
#include <QVector3D>
#include <QDebug>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

/*
* A blocky muscle that may undergo a lot of (careful) modifications to make it look like an actual muscle
*/

class BoxyMuscle
{
public:
    BoxyMuscle();
    void constInitBM(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2,
                     GLfloat x3, GLfloat y3, GLfloat x4, GLfloat y4); // constructs just a cube, which is our first most shape of the muscle
    const GLfloat *constData() const { return m_data.constData(); }
    int count() const { return m_count; }
    int vertexCount() const { return m_count / 6; }
    void rotateBoxyMuscle(GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
    void sceleBoxyMuscle(GLfloat x, GLfloat y, GLfloat z);
    void translateBoxyMuscle(GLfloat x, GLfloat y, GLfloat z);
    void constructBoxyMusclePolysArr(); // stores in boxyMusclePolys and polyNormals lists all the faces/polys and their normals respectively
    void updateRenderableArray();

    void displayPolys() // displays in console all the polygons and their normals
    {
        qDebug() << boxyMusclePolys.length()/4.0;
        for (int i=0; i<boxyMusclePolys.length(); i++)
        {
            qDebug() << boxyMusclePolys[i];
        }
        qDebug() << polyNormals.length();
        for (int i=0; i<polyNormals.length(); i++)
        {
            qDebug() << polyNormals[i];
        }
    }

    void recomputeNormals()
    {
        polyNormals.resize(0);
        for (int i=0; i<faceList.length(); i+=4) // 6 normals will be stored for 6 faces indexed from 0 to 5
        {
            QVector3D n = QVector3D::normal(QVector3D(BM_vertices[faceList[i+1]].x() - BM_vertices[faceList[i]].x(), BM_vertices[faceList[i+1]].y() - BM_vertices[faceList[i]].y(), BM_vertices[faceList[i+1]].z() - BM_vertices[faceList[i]].z()), QVector3D(BM_vertices[faceList[i+3]].x() - BM_vertices[faceList[i]].x(), BM_vertices[faceList[i+3]].y() - BM_vertices[faceList[i]].y(), BM_vertices[faceList[i+3]].z() - BM_vertices[faceList[i]].z()));
            polyNormals.append(n);
        }
    }

    void updateBM_normals() // computes/updates the normals of all faces and stores them in BM_normals
    {
        BM_normals.resize(0);
        for (int i=0; i<BM_faces.length(); i++)
        {
            QVector3D n = QVector3D::normal(BM_vertices[BM_faces[i][0]], BM_vertices[BM_faces[i][1]], BM_vertices[BM_faces[i][2]]);
            BM_normals.append(n);
        }
    }

    QVector<QVector3D> BM_vertices; // stores all the actual 3D vertices of the boxy muscle in a sequence (i.e. v0, v1, v2...).
    QVector< QVector<int> > BM_faces; // triangular faces.
    QVector<QVector3D> BM_normals; // this list stores all the normals of faces. Its size is equal to faceList

    QVector<int> vertIndLookup; // this will be used as a lookup to update the values of vertices in the BM_vertices list whenever the vertices change their values. The updated BM_vertices will be used to update the face normals. not needed.
    QVector<int> faceList; // store the indices of the vertices. Each face is stored as its four vertices in counter clockwise fashion. Repeatations positive. 6 faces. modify it as the faces should be stored with only 3 vertices in cc-wise fashion.
    QVector<int> edgeList; // store the indices of the vertices. Each edge is stored in two connected vetices. Repeatations positive. 12 edges. not needed.

    QVector<QVector3D> polyNormals; // this list stores all the normals of faces. Its size is equal to faceList. no needed after careful examination.
    QVector<QVector3D> boxyMusclePolys; // list to store all the quads of the BoxyMuscle. The vertices will repeat becuase a vertex is shared by 3 faces. total entries = 4x6 = 24. not needed.

private:
    void quad(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2, GLfloat x3, GLfloat y3, GLfloat x4, GLfloat y4);
    void extrude(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2);
    void add(const QVector3D &v, const QVector3D &n, int ind);

    QVector<GLfloat> m_data; // this is our complete data for BoxyMuscle geometry including vertices and normals
    int m_count;
};

#endif // BoxyMuscle_H
