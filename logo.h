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
#ifndef LOGO_H
#define LOGO_H

#include <qopengl.h>
#include <QVector>
#include <QVector3D>
#include <QDebug>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class Logo
{
public:
    Logo();
    const GLfloat *constData() const { return m_data.constData(); }
    int count() const { return m_count; }
    int vertexCount() const { return m_count / 6; }
    void rotateLogo(GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
    void sceleLogo(GLfloat x, GLfloat y, GLfloat z);
    void translateLogo(GLfloat x, GLfloat y, GLfloat z);
    void const3DBox();
    void constructLogoPolysArr(); // stores in logoPolys and polyNormals lists all the faces/polys and their normals respectively
    void constrTriFaceList();
    void displayPolys() // displays in console all the polygons and their normals
    {
        qDebug() << logoPolys.length()/4.0;
        for (int i=0; i<logoPolys.length(); i++)
        {
            qDebug() << logoPolys[i];
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
            QVector3D n = QVector3D::normal(QVector3D(vertIndices[faceList[i+1]].x() - vertIndices[faceList[i]].x(), vertIndices[faceList[i+1]].y() - vertIndices[faceList[i]].y(), vertIndices[faceList[i+1]].z() - vertIndices[faceList[i]].z()), QVector3D(vertIndices[faceList[i+3]].x() - vertIndices[faceList[i]].x(), vertIndices[faceList[i+3]].y() - vertIndices[faceList[i]].y(), vertIndices[faceList[i+3]].z() - vertIndices[faceList[i]].z()));
            polyNormals.append(n);
        }
    }

    QVector3D computeNormal(GLfloat x1, GLfloat y1, GLfloat z1, GLfloat x2, GLfloat y2, GLfloat z2, GLfloat x3, GLfloat y3, GLfloat z3)
    {
        QVector3D n = QVector3D::normal(QVector3D(x2 - x1, y2 - y1, z2 - z1), QVector3D(x3 - x1, y3 - y1, z3 - z1));
        return n;
        // should return the normal
    }

    QVector<QVector3D*> triFaceList_BM; // This is a list of all the triangular faces of the boxy muscle. BM stands for boxy muscle

    QVector<QVector3D> logoPolys; // list to store all the quads/polys of the logo. 4 sided polys. The vertices will repeat becuase a vertex is shared by 3 faces. total entries = 4x6 = 24
    QVector<QVector3D> polyNormals; // this list stores all the normals of polys. Since our box contains 6 polygons so this list contains 6 normals. We need the updated normals to compute the vertex hidden from the camera, and thus finding the Y-junction
    QVector<QVector3D> logoOppPolys;
    QVector3D currPos;
    QVector<QVector3D> vertIndices; // stores 8 vertices in a sequence. The front face vertices are stored first and then the back face vertices
    QVector<int> vertIndLookup; // this will be used as a lookup to upodate the values of vertices in the vertIndices list whenever the vertices change their values. The updated vertIndices will be used to update the face normals
    QVector<int> faceList; // store the indices of the vertices. Each face is stored as its four vertices in counter clockwise fashion. Repeatations positive. 6 faces
    QVector<int> edgeList; // store the indices of the vertices. Each edge is stored in two connected vetices. Repeatations positive. 12 edges


private:
    void quad(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2, GLfloat x3, GLfloat y3, GLfloat x4, GLfloat y4);
    void extrude(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2);
    void add(const QVector3D &v, const QVector3D &n, int ind);

    QVector<GLfloat> m_data; // this is our complete data for logo geometry including vertices and normals
    int m_count;
};

#endif // LOGO_H
