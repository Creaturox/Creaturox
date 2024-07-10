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

#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QMatrix4x4>
#include "QtGui/qevent.h"
#include "boxyMuscle.h"
#include "planexy.h"
#include "box.h"
#include "uppertorso.h"
#include "alien.h"
#include <QVector3D>
#include <QVector4D>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <QPainter>
#include <QPolygonF>
#include <QLineF>
#include "alienmodeller.h"


QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram)

// This is the widget that renders our 3D scene
class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    GLWidget(QWidget *parent = 0);
    ~GLWidget();

    void setAlienModeller(AlienModeller *AM_Ptr);

    QSize minimumSizeHint() const Q_DECL_OVERRIDE;
    QSize sizeHint() const Q_DECL_OVERRIDE;

    void setBoxyMusclePtr(BoxyMuscle* lPtr)
    {
        m_BoxyMuscle = lPtr;
    }

    void setRotBoxyMusclePtr(BoxyMuscle* lPtr)
    {
        rotBoxyMuscle = lPtr;
    }

    BoxyMuscle* getBoxyMusclePtr()
    {
        return m_BoxyMuscle;
    }

    void setBoxyMuscleArr(QVector<BoxyMuscle*> &boxyMuscleArr)
    {
        this->boxyMuscleArr = boxyMuscleArr;
    }

    void mouseHitsPlane(QVector3D pt);
    int getSelBoxyMuscleIndex(QVector3D pt); // get the index of the selected box

    bool T_keyPressed;
    bool R_keyPressed;
    bool S_keyPressed;
    bool translateBox;

    float m_XTrans;
    float m_YTrans;
    float m_ZTrans;

    // this method updates the geometry of the BoxyMuscle after the 'cut' operation has been done.
    void performCutOperation(BoxyMuscle &boxyMuscle);
    void createCuttingPlane(BoxyMuscle &boxyMuscle, QVector<QVector3D> &cutPlaneVertices);
    int getStrokeFace(QVector<int> &whichFaceList);
    void getCuttingPlaneVert(QVector<QVector3D> &cutPlaneVertices);
    void updateBoxyMuscleGeometry();
    void renderBoxNormals();
    void render3DVectors(QVector<QPair<QVector3D,QVector3D>> &vectorsList);
    void createRotGizmoGeometry();
    void renderRotGizmo(BodyPart *bpPtr); // TODO: Incomplete method. Needs to be completed
    void getEdgePlaneIntersectVertex(QVector<QVector3D> &edge, QVector<QVector3D> &cutPlaneVertices, QVector3D);
    void renderAllBodyParts();
    void renderAllAliens(QOpenGLShaderProgram *shader);
    void assignAlienForRendering(Alien *alienPtr);
    void removeAlienForRendering(Alien *alienPtr);
    void assignBodyPartsToRender(Alien *alienPtr, QVector<int> &allBPsIDToRender, bool displayAlienFlag);
    BodyPart* pickBodyPartFromMouse(QVector3D pickingPt); // pick a bodypart from a single click
    Skeleton* pickSkeletonFromMouse(QVector3D pickingPt); // pick the entire skeleton from double click
    // This is used when we need to rotate the world. This is an experimental hack to avoid the
    // ... selection problem.
    void attachAllGeometryToCenterPoint();

    AlienModeller *alienModellerPtr;
    QVector<Alien*> allAliensToRender;
    QVector<BodyPart*> *allBPsToRender;
    QMap<int, QPair<int,int>> BP_colorFlagList;
    // Whenever a BP is selected in the widget, this DS stores the alien's and its BP IDs
    // This is useful for when we need to mutate a clone of this BP in another widget
    QPair<int,int> selectedAlienAndBP_ID;
    QPair<BodyPart*,Cell*> BP_Cell_Pair;
    bool displayAlienFlag;
    int glWidgetID;
    void setBP_PitchRotation(int angle);
    void setBP_YawRotation(int angle);
    void setBP_RollRotation(int angle);

public slots:
    void setXRotation(int angle);
    void setYRotation(int angle);
    void setZRotation(int angle);
    void cleanup();

signals:
    void xRotationChanged(int angle);
    void yRotationChanged(int angle);
    void zRotationChanged(int angle);

protected:
    void initializeGL() Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;
//    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
    void resizeGL(int width, int height) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseDoubleClickEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

private:
    void setupVertexAttribs();
    void subdivideLine(QLineF *line);
    // This method selects a bp if it has been hit and colors it RED
    BodyPart *selectBP_FromMouseClick(QVector3D hitPt3D);

    bool m_core;
    int m_xRot; // x rotation angle for world's rotation
    int m_yRot;
    int m_zRot;
    int m_pitchRotForSelectedBP;
    int m_yawRotForSelectedBP;
    int m_rollRotForSelectedBP;
    QPoint m_lastPos;
    BoxyMuscle *m_BoxyMuscle; // our 3D object to be displayed.
    QOpenGLVertexArrayObject m_vao; // never used so perhaps not needed...please check
    QOpenGLBuffer mVBO;
    GLuint groundPlaneVAO;
    GLuint groundPlaneVBO;
    QOpenGLShaderProgram *m_program;
    QOpenGLShaderProgram *debugDepthQuadShader;
    QOpenGLShaderProgram *simpleDepthShader;
    int m_projMatrixLoc;
    int m_mvMatrixLoc;
    int m_modelMatrixLoc;   // new
    int m_normalMatrixLoc;
    int m_lightPosLoc;  // we need to have more lights
    int m_lightPosLoc_2;  // we need to have more lights
    int m_colorLoc;
    int m_lightSpaceMatrixLoc;
    int m_shadowMapLoc;
    QMatrix4x4 m_proj;
    QMatrix4x4 m_camera;
    QMatrix4x4 m_world;
    QMatrix4x4 lightSpaceMatrix;
    bool m_transparent;
    GLuint depthMapTextVal;
    GLuint depthMapFBO;
    GLuint shadowMapTexVal=0;

    QVector<BoxyMuscle*> boxyMuscleArr; // list of all the boxes

    PlaneXY *plane;

    QVector<float> depthBufVals;
    //QVector<uchar> depthBufVals;
    QVector<GLfloat> rayVerts;
    bool drawRay;

    int boxIndex;

    BoxyMuscle *rotBoxyMuscle;

    QPolygonF cutStroke_1; // stroke used to define a cut on a polygon
    QVector<QLineF*> cutStrokeList;
    QVector<QPolygon*> subDividedLinesList;
    QVector<GLfloat> strokeLineIn3D;
    QVector<GLfloat> boxNormals; // for renderig purposes. Each point stored one after another x,y,z,...
    QVector<GLfloat> cuttingPlane;

//    Box *torso;
    UpperTorso *upTorsoPtr;

    QVector<QPair<QVector3D, QVector3D>> renderableVectorsList;

    QVector<QVector3D> pitchCircle;
    QVector<QVector3D> yawCircle;
    QVector<QVector3D> rollCircle;
    QVector3D rotGizmoPivotVec;
};

#endif
