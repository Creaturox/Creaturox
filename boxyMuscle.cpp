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

#include "boxyMuscle.h"
#include <QMatrix4x4>
#include <qmath.h>

//
// ***** Costruct the initial boxy muscle (just a cube) ******
//
BoxyMuscle::BoxyMuscle()
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
//    sceleBoxyMuscle(10, 10, 10);
//    rotateBoxyMuscle(0,1,0,0);

    recomputeNormals();

    constInitBM(x1, y1, x2, y2, x3, y3, x4, y4);

        qDebug() << count();

        qDebug() << "boxy muscle";
        for (int i=0; i<m_data.length();i+=6)
        {
            qDebug() << m_data[i] << " " << m_data[i+1] << " " << m_data[i+2] << " normal: " << m_data[i+3] << " " << m_data[i+4] << " " << m_data[i+5];
        }


//    for (int i=0; i<BM_normals.length(); i++)
//    {
//        qDebug() << BM_normals[i];
//    }
}

void BoxyMuscle::constInitBM(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2, GLfloat x3, GLfloat y3, GLfloat x4, GLfloat y4) // our inital BM is made of tris and tris will be used throughout
{
    BM_vertices.append(QVector3D(x1, y1,  0.5f)); // x0
    BM_vertices.append(QVector3D(x2, y2,  0.5f)); // x1
    BM_vertices.append(QVector3D(x3, y3,  0.5f)); // x2
    BM_vertices.append(QVector3D(x4, y4,  0.5f)); // x3
    BM_vertices.append(QVector3D(x2, y2, -0.5f)); // x4
    BM_vertices.append(QVector3D(x1, y1, -0.5f)); // x5
    BM_vertices.append(QVector3D(x4, y4, -0.5f)); // x6
    BM_vertices.append(QVector3D(x3, y3, -0.5f)); // x7

    QVector<int> face(3);

    face[0]=0; face[1]=1; face[2]=2;         BM_faces.append(face);
    face[0]=0; face[1]=2; face[2]=3;         BM_faces.append(face);
    face[0]=4; face[1]=5; face[2]=6;         BM_faces.append(face);
    face[0]=4; face[1]=6; face[2]=7;         BM_faces.append(face);

    face[0]=1; face[1]=4; face[2]=7;         BM_faces.append(face);
    face[0]=1; face[1]=7; face[2]=2;         BM_faces.append(face);
    face[0]=2; face[1]=7; face[2]=6;         BM_faces.append(face);
    face[0]=2; face[1]=6; face[2]=3;         BM_faces.append(face);

    face[0]=5; face[1]=0; face[2]=6;         BM_faces.append(face);
    face[0]=6; face[1]=0; face[2]=3;         BM_faces.append(face);
    face[0]=5; face[1]=4; face[2]=1;         BM_faces.append(face);
    face[0]=5; face[1]=1; face[2]=0;         BM_faces.append(face);

    // now construct the list of normals
    updateBM_normals();
}

void BoxyMuscle::add(const QVector3D &v, const QVector3D &n, int ind)
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
void BoxyMuscle::quad(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2, GLfloat x3, GLfloat y3, GLfloat x4, GLfloat y4)
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
    BM_vertices.append(QVector3D(x1, y1,  0.5f));
    BM_vertices.append(QVector3D(x2, y2,  0.5f));
    BM_vertices.append(QVector3D(x3, y3,  0.5f));
    BM_vertices.append(QVector3D(x4, y4,  0.5f));
    // back face
    BM_vertices.append(QVector3D(x2, y2, -0.5f));
    BM_vertices.append(QVector3D(x1, y1, -0.5f));
    BM_vertices.append(QVector3D(x4, y4, -0.5f));
    BM_vertices.append(QVector3D(x3, y3, -0.5f));

    // store the front and back faces in faceList. vert indices: 0,1,2,3,4,5,6,7. vertex indices are stored in counter clock-wise order
    for (int i=0; i<8; i++)
    {
        faceList.append(i);
    }
}

void BoxyMuscle::extrude(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2)
{
    QVector3D n = QVector3D::normal(QVector3D(0.0f, 0.0f, -0.1f), QVector3D(x2 - x1, y2 - y1, 0.0f));

    add(QVector3D(x1, y1, +0.5f), n,-1);
    add(QVector3D(x1, y1, -0.5f), n,-1);
    add(QVector3D(x2, y2, +0.5f), n,-1);

    add(QVector3D(x2, y2, -0.5f), n,-1);
    add(QVector3D(x2, y2, +0.5f), n,-1);
    add(QVector3D(x1, y1, -0.5f), n,-1);
}



// ******* Transformation functions *******
void BoxyMuscle::rotateBoxyMuscle(GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
    QMatrix4x4 rotMat;

    rotMat.setToIdentity();
    rotMat.rotate(angle,x,y,z);

//    for (int i=0; i<m_data.length(); i+=6)
//    {
//        QVector3D resVec;

//        resVec.setX(m_data[i]);
//        resVec.setY(m_data[i+1]);
//        resVec.setZ(m_data[i+2]);

//        resVec = rotMat * resVec;

//        // store the new vertex
//        m_data[i] = resVec.x();
//        m_data[i+1] = resVec.y();
//        m_data[i+2] = resVec.z();

//        // update the vertices in the vertices list 'BM_vertices'
//        if (vertIndLookup[i] != -1)
//        {
//            BM_vertices[vertIndLookup[i]].setX(m_data[i]);
//            BM_vertices[vertIndLookup[i]].setY(m_data[i+1]);
//            BM_vertices[vertIndLookup[i]].setZ(m_data[i+2]);
//        }
//    }

    // update the vertices in the vertices list 'BM_vertices'
    for (int i=0; i<BM_vertices.length(); i+=6)
    {
        BM_vertices[i] = rotMat * BM_vertices[i];
    }

    // Once we have updated the vertices, we need to Update/Recompute the normals
    recomputeNormals();
}

void BoxyMuscle::sceleBoxyMuscle(GLfloat x, GLfloat y, GLfloat z)
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

        // update the vertices in the vertices list 'BM_vertices'
        if (vertIndLookup[i] != -1)
        {
            BM_vertices[vertIndLookup[i]].setX(m_data[i]);
            BM_vertices[vertIndLookup[i]].setY(m_data[i+1]);
            BM_vertices[vertIndLookup[i]].setZ(m_data[i+2]);
        }
    }

    // Once we have updated the vertices, we need to Update/Recompute the normals
    recomputeNormals();
}

void BoxyMuscle::translateBoxyMuscle(GLfloat x, GLfloat y, GLfloat z)
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
void BoxyMuscle::constructBoxyMusclePolysArr()
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
                boxyMusclePolys.append(tempPnt);
                till = boxyMusclePolys.length()-1;
                continue;
            }
            // now compare
            found=false;
            for (int k=till; k<boxyMusclePolys.length(); k++)
            {
                if (tempPnt == boxyMusclePolys[k])
                {
                    found=true;
                    break;
                }
            }
            if (found == false)
            {
                boxyMusclePolys.append(tempPnt);
            }
        }
    }
}

void BoxyMuscle::updateRenderableArray()
{
    m_data.resize(0);
    for (int i=0; i<BM_faces.length(); i++)
    {
        m_data.append(BM_vertices[BM_faces[i][0]].x());
        m_data.append(BM_vertices[BM_faces[i][0]].y());
        m_data.append(BM_vertices[BM_faces[i][0]].z());
        m_data.append(BM_normals[i].x());
        m_data.append(BM_normals[i].y());
        m_data.append(BM_normals[i].z());

        m_data.append(BM_vertices[BM_faces[i][1]].x());
        m_data.append(BM_vertices[BM_faces[i][1]].y());
        m_data.append(BM_vertices[BM_faces[i][1]].z());
        m_data.append(BM_normals[i].x());
        m_data.append(BM_normals[i].y());
        m_data.append(BM_normals[i].z());

        m_data.append(BM_vertices[BM_faces[i][2]].x());
        m_data.append(BM_vertices[BM_faces[i][2]].y());
        m_data.append(BM_vertices[BM_faces[i][2]].z());
        m_data.append(BM_normals[i].x());
        m_data.append(BM_normals[i].y());
        m_data.append(BM_normals[i].z());
    }
}
