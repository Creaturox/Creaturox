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

#include "logo.h"
#include <QMatrix4x4>
#include <qmath.h>

// Costruct the box
Logo::Logo()
    : m_count(0)
{
    const GLfloat x1 = -0.5;
    const GLfloat y1 = -0.5;
    const GLfloat x2 =  0.5;
    const GLfloat y2 = -0.5;
    const GLfloat x3 =  0.5;
    const GLfloat y3 =  0.5;
    const GLfloat x4 = -0.5;
    const GLfloat y4 =  0.5;

    quad(x1, y1, x2, y2, x3, y3, x4, y4); // stores the front and back faces, and also the indices of vertices in faceList

    // Then stores the side faces in opposite manner. The bottom face is stored first, then the top. Then the left and then the right
    extrude(x1, y1, x2, y2);
    extrude(x3, y3, x4, y4);
    extrude(x4, y4, x1, y1);
    extrude(x2, y2, x3, y3);

    // store the side faces in faceList. The first 8 were stored in the quad() function
    faceList.append(5);
    faceList.append(4);
    faceList.append(1);
    faceList.append(0);
    faceList.append(4);
    faceList.append(7);
    faceList.append(2);
    faceList.append(1);
    faceList.append(7);
    faceList.append(6);
    faceList.append(3);
    faceList.append(2);
    faceList.append(5);
    faceList.append(0);
    faceList.append(3);
    faceList.append(6);

    // lets make the edgeList.
    // front face edges
    edgeList.append(0);
    edgeList.append(1);
    edgeList.append(1);
    edgeList.append(2);
    edgeList.append(2);
    edgeList.append(3);
    edgeList.append(3);
    edgeList.append(0);
    // back face edges
    edgeList.append(4);
    edgeList.append(5);
    edgeList.append(5);
    edgeList.append(6);
    edgeList.append(6);
    edgeList.append(7);
    edgeList.append(7);
    edgeList.append(4);
    // side face edges
    edgeList.append(3);
    edgeList.append(6);
    edgeList.append(0);
    edgeList.append(5);
    edgeList.append(1);
    edgeList.append(4);
    edgeList.append(2);
    edgeList.append(7);

    // scale the box to start with
    sceleLogo(20, 20, 10);
//    rotateLogo(0,1,0,0);

    recomputeNormals();

    constrTriFaceList();
}

void Logo::rotateLogo(GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
    QMatrix4x4 rotMat;

    rotMat.setToIdentity();
    rotMat.rotate(angle,x,y,z);

    for (int i=0; i<m_data.length(); i+=6)
    {
        QVector3D resVec;

        resVec.setX(m_data[i]);
        resVec.setY(m_data[i+1]);
        resVec.setZ(m_data[i+2]);

        resVec = rotMat * resVec;

        // store the new vertex
        m_data[i] = resVec.x();
        m_data[i+1] = resVec.y();
        m_data[i+2] = resVec.z();

        // update the vertices in the vertices list 'vertIndices'
        if (vertIndLookup[i] != -1)
        {
            vertIndices[vertIndLookup[i]].setX(m_data[i]);
            vertIndices[vertIndLookup[i]].setY(m_data[i+1]);
            vertIndices[vertIndLookup[i]].setZ(m_data[i+2]);
        }
    }

    // Once we have updated the vertices, we need to Update/Recompute the normals
    recomputeNormals();

}

void Logo::sceleLogo(GLfloat x, GLfloat y, GLfloat z)
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

        // update the vertices in the vertices list 'vertIndices'
        if (vertIndLookup[i] != -1)
        {
            vertIndices[vertIndLookup[i]].setX(m_data[i]);
            vertIndices[vertIndLookup[i]].setY(m_data[i+1]);
            vertIndices[vertIndLookup[i]].setZ(m_data[i+2]);
        }
    }

    // Once we have updated the vertices, we need to Update/Recompute the normals
    recomputeNormals();

//    for (int i=0; i<m_data.length()+1; i++)
//    {
//        if (i%36 == 0 && i>0)
//        {
//            qDebug() << i;
//            // compute and store the updated normal
//            QVector3D n = (-1) * QVector3D::normal(QVector3D( m_data[(i-36)+0], m_data[(i-36)+1], m_data[(i-36)+2]),
//                                            QVector3D( m_data[(i-36)+6], m_data[(i-36)+7], m_data[(i-36)+8]),
//                                            QVector3D( m_data[(i-36)+12], m_data[(i-36)+13], m_data[(i-36)+14]));
//            for (int j=(i-36)+3; j<i; j+=3)
//            {
//                m_data[j]   = n.x();
//                m_data[j+1] = n.y();
//                m_data[j+2] = n.z();
//            }
//        }
//    }

}

void Logo::translateLogo(GLfloat x, GLfloat y, GLfloat z)
{

    QMatrix4x4 tranMat;

    tranMat.translate(x,y,z);

    for (int i=0; i<m_data.length(); i+=6)
    {
        QVector3D resVec;

        resVec.setX(m_data[i]);
        resVec.setY(m_data[i+1]);
        resVec.setZ(m_data[i+2]);
//        resVec.setW(1.0);
        resVec = tranMat * resVec;
        if (i==0)
            qDebug() << resVec;

        m_data[i] = resVec.x();
        m_data[i+1] = resVec.y();
        m_data[i+2] = resVec.z();
    }

//    for (int i=0; i<m_data.length()+1; i++)
//    {
//        if (i%36 == 0 && i>0)
//        {
//            qDebug() << i;
//            // compute and store the updated normal
//            QVector3D n = (-1) * QVector3D::normal(QVector3D( m_data[(i-36)+0], m_data[(i-36)+1], m_data[(i-36)+2]),
//                                            QVector3D( m_data[(i-36)+6], m_data[(i-36)+7], m_data[(i-36)+8]),
//                                            QVector3D( m_data[(i-36)+12], m_data[(i-36)+13], m_data[(i-36)+14]));
//            for (int j=(i-36)+3; j<i; j+=3)
//            {
//                m_data[j]   = n.x();
//                m_data[j+1] = n.y();
//                m_data[j+2] = n.z();
//            }
//        }
//    }
}

// Constructs a list of faces. The purpose of this function is to be used for mouse picking
void Logo::constructLogoPolysArr()
{
    // first store all the faces
    int till=0;
    for (int i=0; i<m_data.length(); i+=36)
    {
        // consider the 6 points and extract the 4 unique ones
        for (int j=i; j<i+36; j+=6)
        {
            QVector3D tempPnt(m_data[j], m_data[j+1], m_data[j+2]);
            bool found=false;
            // store the first one for comparison with
            if (j==i)
            {
                logoPolys.append(tempPnt);
                till = logoPolys.length()-1;
                continue;
            }
            // now compare
            found=false;
            for (int k=till; k<logoPolys.length(); k++)
            {
                if (tempPnt == logoPolys[k])
                {
                    found=true;
                    break;
                }
            }
            if (found == false)
            {
                logoPolys.append(tempPnt);
            }
        }
    }

    // now store the normals
//    for (int i=0; i<logoPolys.length(); i+=4)
//    {
//        QVector3D n = QVector3D::normal(logoPolys[i],logoPolys[i+1],logoPolys[i+2]);
//        polyNormals.append(n);
    //    }
}

// constructs the list of tirangular faces using the native renderable face list
void Logo::constrTriFaceList()
{
    for (int i=0; i<m_data.length(); i+=18)
    {
        QVector3D *newFace = new QVector3D[3];
        for (int j=i,k=0; j<i+18; j+=6,k++)
        {
            newFace[k].setX(m_data[j+0]);
            newFace[k].setY(m_data[j+1]);
            newFace[k].setZ(m_data[j+2]);
        }
        triFaceList_BM.append(newFace);
    }
}

void Logo::add(const QVector3D &v, const QVector3D &n, int ind)
{
    m_data.append(v.x());
    m_data.append(v.y());
    m_data.append(v.z());
    m_data.append(n.x());
    m_data.append(n.y());
    m_data.append(n.z());

    m_count += 6;

    vertIndLookup.append(ind); vertIndLookup.append(ind); vertIndLookup.append(ind);
    vertIndLookup.append(-1); vertIndLookup.append(-1); vertIndLookup.append(-1);
}

// front and back faces of the box
void Logo::quad(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2, GLfloat x3, GLfloat y3, GLfloat x4, GLfloat y4)
{
    QVector3D n = QVector3D::normal(QVector3D(x1 - x4, y1 - y4, 0.0f), QVector3D(x2 - x4, y2 - y4, 0.0f));

    // 1st triangle of front face
    add(QVector3D(x4, y4, 0.5f), n,3);
    add(QVector3D(x1, y1, 0.5f), n,0);
    add(QVector3D(x2, y2, 0.5f), n,1);
    // 2nd triangle of front face
    add(QVector3D(x2, y2, 0.5f), n,1);
    add(QVector3D(x3, y3, 0.5f), n,2);
    add(QVector3D(x4, y4, 0.5f), n,3);

    n = QVector3D::normal(QVector3D(x4 - x1, y4 - y1, 0.0f), QVector3D(x2 - x1, y2 - y1, 0.0f));
    // 1st triangle of back face
    add(QVector3D(x1, y1, -0.5f), n,5);
    add(QVector3D(x4, y4, -0.5f), n,6);
    add(QVector3D(x2, y2, -0.5f), n,4);
    // 2nd triangle of back face
    add(QVector3D(x3, y3, -0.5f), n,7);
    add(QVector3D(x2, y2, -0.5f), n,4);
    add(QVector3D(x4, y4, -0.5f), n,6);

    // front face
    vertIndices.append(QVector3D(x1, y1,  0.5f));
    vertIndices.append(QVector3D(x2, y2,  0.5f));
    vertIndices.append(QVector3D(x3, y3,  0.5f));
    vertIndices.append(QVector3D(x4, y4,  0.5f));
    // back face
    vertIndices.append(QVector3D(x2, y2, -0.5f));
    vertIndices.append(QVector3D(x1, y1, -0.5f));
    vertIndices.append(QVector3D(x4, y4, -0.5f));
    vertIndices.append(QVector3D(x3, y3, -0.5f));

    // store the front and back faces in faceList. vert indices: 0,1,2,3,4,5,6,7. vertex indices are stored in counter clock-wise order
    for (int i=0; i<8; i++)
    {
        faceList.append(i);
    }



}

void Logo::extrude(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2)
{
    QVector3D n = QVector3D::normal(QVector3D(0.0f, 0.0f, -0.1f), QVector3D(x2 - x1, y2 - y1, 0.0f));

    add(QVector3D(x1, y1, +0.5f), n,-1);
    add(QVector3D(x1, y1, -0.5f), n,-1);
    add(QVector3D(x2, y2, +0.5f), n,-1);

    add(QVector3D(x2, y2, -0.5f), n,-1);
    add(QVector3D(x2, y2, +0.5f), n,-1);
    add(QVector3D(x1, y1, -0.5f), n,-1);
}

