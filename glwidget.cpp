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

/*
 *  Problems:
 *  1 - There is no UNDO funtionality
 *  2 - To make the apearance of the results exactly like the figure, can we introduce NPR or some other kind of shading?
 *
*/

#include "glwidget.h"
#include "torsomodeller.h"
#include <QMouseEvent>
#include <QOpenGLShaderProgram>
#include <QCoreApplication>
#include <math.h>
#include <QImage>
#include <qmath.h>
#include <QMessageBox>
#include <algorithm>
//#include <boost/algorithm/algorithm.hpp>
//#include <boost/asio/coroutine.hpp>

GLWidget::GLWidget(QWidget *parent)
    : QOpenGLWidget(parent),
      m_xRot(0),
      m_yRot(0),
      m_zRot(0),
      m_XTrans(0),
      m_YTrans(0),
      m_ZTrans(0),
      m_program(0),
      m_pitchRotForSelectedBP(0),
      m_yawRotForSelectedBP(0)
{
    m_core = QCoreApplication::arguments().contains(QStringLiteral("--coreprofile"));
    // --transparent causes the clear color to be transparent. Therefore, on systems that
    // support it, the widget will become transparent apart from the BoxyMuscle.
    m_transparent = QCoreApplication::arguments().contains(QStringLiteral("--transparent"));
    if (m_transparent)
        setAttribute(Qt::WA_TranslucentBackground);
    T_keyPressed=false;
    R_keyPressed=false;
    S_keyPressed=false;
    drawRay = false;
//    plane = new PlaneXY();
    translateBox=false;
//    BP_Color = QVector3D(0.25,0.50,1.0);
//    this->setFocusPolicy( Qt::StrongFocus );

    // TODO(ismail): REFACTOR: Try moving the AlienModeller declaration to the mainwindow class in order for
    // alienmodeller to stay alive throughout the program. This is also good if the user wants to
    // manipulates/modifies the alien more, after it has been modelled by the system
//    AlienModeller tempAM_Object;
//    tempAM_Object.initializeAlienModeller();
//    tempAM_Object.modelNewAlien();
//    alienModellerObject.modelNewAlien_TypeGuided();

//    allAliensToRender = alienModellerObject.alienList;
//    this->alienModellerPtr = AM_Ptr;
    this->allBPsToRender = new QVector<BodyPart*>(0);

    BP_colorFlagList.clear();
    selectedAlienAndBP_ID.first = -1; selectedAlienAndBP_ID.second = -1;
    this->allAliensToRender.clear();
    this->displayAlienFlag = true;
    this->glWidgetID = -1;
    BP_Cell_Pair.first = NULL; BP_Cell_Pair.second = NULL;

    createRotGizmoGeometry();
}

GLWidget::~GLWidget()
{
    this->allAliensToRender.clear();
    this->allBPsToRender->clear();
    if (this->alienModellerPtr)
        delete this->alienModellerPtr;

    cleanup();
}

void GLWidget::setAlienModeller(AlienModeller *AM_Ptr)
{
    this->alienModellerPtr = AM_Ptr;
}

QSize GLWidget::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize GLWidget::sizeHint() const
{
    return QSize(400, 400);
}

static void qNormalizeAngle(int &angle)
{
    while (angle < 0)
        angle += 360 * 16;
    while (angle > 360 * 16)
        angle -= 360 * 16;
}

void GLWidget::setXRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != m_xRot) {
        m_xRot = angle;
        emit xRotationChanged(angle);
        update();
    }
}

void GLWidget::setYRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != m_yRot) {
        m_yRot = angle;
        emit yRotationChanged(angle);
        update();
    }
}

void GLWidget::setZRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != m_zRot) {
        m_zRot = angle;
        emit zRotationChanged(angle);
        update();
    }
}

void GLWidget::cleanup()
{
    makeCurrent();
    mVBO.destroy();
    delete m_program;
    m_program = 0;
    doneCurrent();
}

// static const char *vertexShaderSourceCore =
//     "#version 150\n"
//     "in vec4 vertex;\n"
//     "in vec3 normal;\n"
//     "out vec3 vert;\n"
//     "out vec3 vertNormal;\n"
//     "uniform mat4 projMatrix;\n"
//     "uniform mat4 mvMatrix;\n"
//     "uniform mat3 normalMatrix;\n"
//     "void main() {\n"
//     "   vert = vertex.xyz;\n"
//     "   vertNormal = normalMatrix * normal;\n"
//     "   gl_Position = projMatrix * mvMatrix * vertex;\n"
//     "}\n";

// static const char *fragmentShaderSourceCore =
//     "#version 150\n"
//     "in highp vec3 vert;\n"
//     "in highp vec3 vertNormal;\n"
//     "out highp vec4 fragColor;\n"
//     "uniform highp vec3 lightPos;\n"
//     "void main() {\n"
//     "   highp vec3 L = normalize(lightPos - vert);\n"
//     "   highp float NL = max(dot(normalize(vertNormal), L), 0.0);\n"
//     "   highp vec3 color = vec3(0.39, 1.0, 0.0);\n"
//     "   highp vec3 col = clamp(color * 0.2 + color * 0.8 * NL, 0.0, 1.0);\n"
//     "   fragColor = vec4(col, 1.0);\n"
//     "}\n";

static const char *vertexShaderSource =
    "attribute vec4 vertex;\n"
    "attribute vec3 normal;\n"
    // "varying vec3 vert;\n"
    "varying vec3 vertNormal;\n"
    "varying vec3 FragPos;\n"   // new
    "varying vec4 FragPosLightSpace;\n" // new
    "uniform mat4 projMatrix;\n"
    "uniform mat4 mvMatrix;\n"
    "uniform mat4 modelMatrix;\n"   // new
    "uniform mat3 normalMatrix;\n"
    "uniform mat4 lightSpaceMatrix;\n"
    "void main() {\n"
    // "   vert = vertex.xyz;\n"
    "   FragPos = vec3(modelMatrix * vec4(vertex.xyz, 1.0));\n" // new
    "   vertNormal = normalMatrix * normal;\n"
    "   FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);\n" // new
    "   gl_Position = projMatrix * mvMatrix * vertex;\n"
    "}\n";

static const char *fragmentShaderSource =
    // "varying highp vec3 vert;\n"
    "varying highp vec3 vertNormal;\n"
    "varying highp vec3 FragPos;\n"
    "varying highp vec4 FragPosLightSpace;\n"
    "uniform highp vec3 lightPos;\n"
    "uniform highp vec3 lightPos_2;\n"
    "uniform highp vec3 color;\n"
    "uniform sampler2D shadowMap;\n"

    "float ShadowCalculation(vec4 fragPosLightSpace){\n"
    // perform perspective divide
    "   vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;\n"
    // transform to [0,1] range
    "   projCoords = projCoords * 0.5 + 0.5;\n"
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    "   float closestDepth = texture2D(shadowMap, projCoords.xy).r;\n"
    // get depth of current fragment from light's perspective
    "   float currentDepth = projCoords.z;\n"
    // check whether current frag pos is in shadow
    "   float bias = max(0.05 * (1.0 - dot(vertNormal, normalize(lightPos - FragPos))), 0.005);\n"
    "   float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;\n"
    "   return shadow;\n"
    "}\n"

    "void main() {\n"
    "   highp vec3 L = normalize(lightPos - FragPos);\n"
    "   highp vec3 L2 = normalize(lightPos_2 - FragPos);\n"
    "   highp float NL = max(dot(normalize(vertNormal), L), 0.0);\n"
    "   highp float NL2 = max(dot(normalize(vertNormal), L2), 0.0);\n"
    "   float shadow = ShadowCalculation(FragPosLightSpace);\n"
    "   highp vec3 col = clamp(color * 0.2 + (1.0 - shadow)*(color * 0.8 * NL), 0.0, 1.0);\n"
    "   highp vec3 col2 = clamp(color * 0.6 * NL2, 0.0, 1.0);\n"
    "   gl_FragColor = vec4(col+col2, 1.0);\n"
    "}\n";

static const char *simple_shadow_mapping_depth_VS =
    "attribute vec3 aPos;\n"
    "uniform mat4 lightSpaceMatrix;\n"
    "uniform mat4 modelMatrix;\n"

    "void main(){\n"
    "   gl_Position = lightSpaceMatrix * modelMatrix * vec4(aPos, 1.0);\n"
    "}\n";

static const char *simple_shadow_mapping_depth_FS =
    "void main(){\n"
        // "gl_FragDepth = gl_FragCoord.z;\n"
    "}\n";


void GLWidget::initializeGL()
{
    // In this example the widget's corresponding top-level window can change
    // several times during the widget's lifetime. Whenever this happens, the
    // QOpenGLWidget's associated context is destroyed and a new one is created.
    // Therefore we have to be prepared to clean up the resources on the
    // aboutToBeDestroyed() signal, instead of the destructor. The emission of
    // the signal will be followed by an invocation of initializeGL() where we
    // can recreate all resources.
    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &GLWidget::cleanup);

    initializeOpenGLFunctions();
    glClearColor(0, 0, 0, m_transparent ? 0 : 1);


    m_program = new QOpenGLShaderProgram();
    // m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, m_core ? vertexShaderSourceCore : vertexShaderSource);
    // m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, m_core ? fragmentShaderSourceCore : fragmentShaderSource);
    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
    m_program->bindAttributeLocation("vertex", 0);
    m_program->bindAttributeLocation("normal", 1);
    m_program->link();

    m_program->bind();
    m_projMatrixLoc = m_program->uniformLocation("projMatrix");
    m_mvMatrixLoc = m_program->uniformLocation("mvMatrix");
    m_modelMatrixLoc = m_program->uniformLocation("modelMatrix");   // new
    m_normalMatrixLoc = m_program->uniformLocation("normalMatrix");
    m_lightPosLoc = m_program->uniformLocation("lightPos");
    m_lightPosLoc_2 = m_program->uniformLocation("lightPos_2");
    m_colorLoc = m_program->uniformLocation("color");
    m_lightSpaceMatrixLoc = m_program->uniformLocation("lightSpaceMatrix"); // new
    m_shadowMapLoc = m_program->uniformLocation("shadowMap"); // new

    // Our camera never changes in this example. Its actually the world (m_world) that rotates
    m_camera.setToIdentity();
    m_camera.lookAt(QVector3D(0,2000,2000), QVector3D(0,0,0), QVector3D(0.0,1.0,0.0)); // (eye,center,up_vector)

    // Light position is fixed.
    m_program->setUniformValue(m_lightPosLoc, QVector3D(2000, 2000, 2000));
    m_program->setUniformValue(m_lightPosLoc_2, QVector3D(-2000, 2000, -2000));
    m_program->setUniformValue(m_shadowMapLoc, shadowMapTexVal);
    qDebug() << m_program->log();

    m_program->release();


    // simpleDepthShader = new QOpenGLShaderProgram();
    // simpleDepthShader->addShaderFromSourceCode(QOpenGLShader::Vertex, simple_shadow_mapping_depth_VS);
    // simpleDepthShader->addShaderFromSourceCode(QOpenGLShader::Fragment, simple_shadow_mapping_depth_FS);
    // simpleDepthShader->bindAttributeLocation("aPos", 0);
    // simpleDepthShader->link();

    // simpleDepthShader->bind();
    // simpleDepthShader->setUniformValue(simpleDepthShader->uniformLocation("lightSpaceMatrix"), 0);
    // simpleDepthShader->release();
    // qDebug() << "reached initializeGL 0";
}

/*  // The working method - for backup
void GLWidget::initializeGL()
{
    // In this example the widget's corresponding top-level window can change
    // several times during the widget's lifetime. Whenever this happens, the
    // QOpenGLWidget's associated context is destroyed and a new one is created.
    // Therefore we have to be prepared to clean up the resources on the
    // aboutToBeDestroyed() signal, instead of the destructor. The emission of
    // the signal will be followed by an invocation of initializeGL() where we
    // can recreate all resources.
    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &GLWidget::cleanup);

    initializeOpenGLFunctions();
    glClearColor(0, 0, 0, m_transparent ? 0 : 1);

    m_program = new QOpenGLShaderProgram();
    // m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, m_core ? vertexShaderSourceCore : vertexShaderSource);
    // m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, m_core ? fragmentShaderSourceCore : fragmentShaderSource);
    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
    m_program->bindAttributeLocation("vertex", 0);
    m_program->bindAttributeLocation("normal", 1);
    m_program->link();

    m_program->bind();
    m_projMatrixLoc = m_program->uniformLocation("projMatrix");
    m_mvMatrixLoc = m_program->uniformLocation("mvMatrix");
    m_normalMatrixLoc = m_program->uniformLocation("normalMatrix");
    m_lightPosLoc = m_program->uniformLocation("lightPos");
    m_lightPosLoc_2 = m_program->uniformLocation("lightPos_2");
    m_colorLoc = m_program->uniformLocation("color");

    // Our camera never changes in this example. Its actually the world (m_world) that rotates
    m_camera.setToIdentity();
    m_camera.lookAt(QVector3D(0,200,2000), QVector3D(0,0,0), QVector3D(0.0,1.0,0.0)); // (eye,center,up_vector)

    // Light position is fixed.
    m_program->setUniformValue(m_lightPosLoc, QVector3D(0, 0, 700));
    m_program->setUniformValue(m_lightPosLoc_2, QVector3D(0, 0, -700));

    m_program->release();
}
*/

void GLWidget::setupVertexAttribs()
{
}

// draw our 3D scene

void GLWidget::paintGL()
{

    QPainter painter(this);

    painter.beginNativePainting();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // // configure depth map FBO
    // // -----------------------
    // const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    // glGenFramebuffers(1, &depthMapFBO);
    // // create depth texture
    // glGenTextures(1, &depthMapTextVal);
    // glBindTexture(GL_TEXTURE_2D, depthMapTextVal);
    // glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // // attach depth texture as FBO's depth buffer
    // glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTextVal, 0);
    // glDrawBuffer(GL_NONE);
    // glReadBuffer(GL_NONE);
    // glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // qDebug() << "reached paintGL 0";


    // here we are rotating the world...I think (instead of the camera). So this could be an easy hack to avoid the trouble of implementing an orbiting camera
    //  TODO(Ismail):   ** I thing its working ** Make the camera rotate around the object i.e. around an invisible bounding sphere or convex hull.
    //                  This can be useful when we have several aliens to display and we can switch cameras from one alien to another, i.e. each alien will
    //                  have its own camera and bounding sphere (convex hull) around which its camera orbits
    m_world.setToIdentity();
    m_camera.setToIdentity();
    m_world.rotate(m_xRot / 16.0f, 1, 0, 0);
    m_world.rotate(m_yRot / 16.0f, 0, 1, 0);
    m_world.rotate(m_zRot / 16.0f, 0, 0, 1);
    // m_camera.rotate(m_xRot / 16.0f, 1, 0, 0);
    // m_camera.rotate(m_yRot / 16.0f, 0, 1, 0);
    // m_camera.rotate(m_zRot / 16.0f, 0, 0, 1);

    // QMatrix4x4 tempMat; tempMat.setToIdentity();
    // m_camera = tempMat * m_world;

    // Try translating the world based on key presses
   // m_camera.translate(m_XTrans,m_YTrans,m_ZTrans);
    mVBO.create();
    mVBO.bind();



    m_program->bind();
    m_program->setUniformValue(m_projMatrixLoc, m_proj);
    m_program->setUniformValue(m_mvMatrixLoc, m_camera * m_world);
    m_program->setUniformValue(m_modelMatrixLoc, m_world);
    QMatrix3x3 normalMatrix = m_world.normalMatrix();
//    QMatrix3x3 normalMatrix = (m_camera * m_world).normalMatrix();
    m_program->setUniformValue(m_normalMatrixLoc, normalMatrix);

    // change the RGB color
    m_program->setUniformValue(m_colorLoc, QVector3D(0.39,1.0,1.0));
    qDebug() << "reached paintGL 1";

    // // Render the gound plane
    // // set up ground plane vertex data (and buffer(s)) and configure vertex attributes
    // // ------------------------------------------------------------------
    // float groundPlaneData[] = {
    //     // positions            // normals
    //     250.0f, -0.5f,  250.0f,  0.0f, 1.0f, 0.0f,
    //     -250.0f, -0.5f,  250.0f,  0.0f, 1.0f, 0.0f,
    //     -250.0f, -0.5f, -250.0f,  0.0f, 1.0f, 0.0f,

    //     250.0f, -0.5f,  250.0f,  0.0f, 1.0f, 0.0f,
    //     -250.0f, -0.5f, -250.0f,  0.0f, 1.0f, 0.0f,
    //     250.0f, -0.5f, -250.0f,  0.0f, 1.0f, 0.0f
    // };
    // glGenVertexArrays(1, &groundPlaneVAO);
    // glGenBuffers(1, &groundPlaneVBO);
    // glBindVertexArray(groundPlaneVAO);
    // glBindBuffer(GL_ARRAY_BUFFER, groundPlaneVBO);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(groundPlaneData), groundPlaneData, GL_STATIC_DRAW);
    // glEnableVertexAttribArray(0);
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    // glEnableVertexAttribArray(1);
    // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

    qDebug() << "reached paintGL 2";

//    for (int i=0; i<this->boxyMuscleArr.length(); i++)
//    {
//        m_BoxyMuscleVbo.allocate(boxyMuscleArr[i]->constData(), boxyMuscleArr[i]->count() * sizeof(GLfloat)); // here we can put conditions for choosing to display or not
//        glDrawArrays(GL_TRIANGLES, 0, boxyMuscleArr[i]->vertexCount()); // here we can put conditions for choosing to display or not
//    }


    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void *>(3 * sizeof(GLfloat)));

    qDebug() << "reached paintGL 3";

    // // --- SHADOW MAPPING -------------------------------------------------------------------------
    // // // 1. render depth of scene to texture (from light's perspective)
    // // // --------------------------------------------------------------
    // QMatrix4x4 lightProjection, lightView;
    // // float near_plane = 1.0f, far_plane = 7.5f;
    // float near_plane = -2000, far_plane = 4000;
    // lightProjection.ortho(0-width(),width(),-height(),height(), near_plane, far_plane);
    // lightView.lookAt(QVector3D(2000,2000,2000), QVector3D(0.f,0.f,0.f), QVector3D(0.0, 1.0, 0.0));
    // lightSpaceMatrix = lightProjection * lightView;
    // // render scene from light's point of view
    // simpleDepthShader->bind();
    // simpleDepthShader->setUniformValue(simpleDepthShader->uniformLocation("lightSpaceMatrix"), lightSpaceMatrix);
    // simpleDepthShader->setUniformValue(simpleDepthShader->uniformLocation("modelMatrix"), m_world);
    // // qDebug() << simpleDepthShader->log();
    // // qDebug() << "reached paintGL 4";

    // glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    // glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    // glClear(GL_DEPTH_BUFFER_BIT);
    if (!this->allAliensToRender.empty() && displayAlienFlag)
        renderAllAliens(m_program);

    // glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // // reset viewport
    // glViewport(0, 0, width(), height());
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // qDebug() << simpleDepthShader->log();


    // // 2. render scene as normal using the generated depth/shadow map
    // // --------------------------------------------------------------
    // m_program->bind();
    // // m_proj.perspective( 45, 16.f/9.f, near_plane, far_plane);
    // // m_proj.ortho(0-width(),width(),-height(),height(), near_plane, far_plane);
    // // m_program->setUniformValue(m_projMatrixLoc, m_proj);
    // // m_program->setUniformValue(m_mvMatrixLoc, m_camera * m_world);
    // // m_program->setUniformValue(m_modelMatrixLoc, m_world);

    // // set light uniforms
    // m_program->setUniformValue(m_lightPosLoc, QVector3D(2000, 2000, 2000));
    // m_program->setUniformValue(m_program->uniformLocation("lightSpaceMatrix"), lightSpaceMatrix);
    // // glActiveTexture(GL_TEXTURE0);
    // // glBindTexture(GL_TEXTURE_2D, depthMapTextVal);
    // if (!this->allAliensToRender.empty() && displayAlienFlag)
    //     renderAllAliens(m_program);
    // qDebug() << m_program->log();
// -------------------------------------------------------------------------------------------------------------

//    if (!this->allBPsToRender->empty())
//        renderAllBodyParts();
//    render3DVectors(allAliensToRender.last()->tempVectorsListForRendering);

//    renderBoxNormals();

//    m_BoxyMuscleVbo.allocate(plane->constData(), plane->count() * sizeof(GLfloat));
//    glDrawArrays(GL_TRIANGLES, 0, plane->vertexCount());

//    if (drawRay)
//    {
//        m_program->setUniformValue(m_colorLoc, QVector3D(1.0,0.0,0.0));
//        glEnableVertexAttribArray(0);
//        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
//        m_BoxyMuscleVbo.allocate(strokeLineIn3D.constData(), strokeLineIn3D.length() * sizeof(GLfloat));
//        glLineWidth(3.0);
//        glDrawArrays(GL_LINE_STRIP, 0, strokeLineIn3D.length()/3);

//        // +++ Test Rendering +++
//        // change the RGB color
///*
//        m_program->setUniformValue(m_colorLoc, QVector3D(0.39,1.0,0.60));
//        glEnableVertexAttribArray(0);
//        glEnableVertexAttribArray(1);
//        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0);
//        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void *>(3 * sizeof(GLfloat)));
//        m_BoxyMuscleVbo.allocate(cuttingPlane.constData(), cuttingPlane.length() * sizeof(GLfloat)); // here we can put conditions for choosing to display or not
//        glDrawArrays(GL_QUADS, 0, cuttingPlane.length()/6); // here we can put conditions for choosing to display or not
//*/
//    }

    //QImage res = QImage(width(), height(), QImage::Format_ARGB32);
    //glReadPixels(0, 0, width(), height(), GL_RGBA, GL_UNSIGNED_BYTE, res.bits());
    //res.save("D:/test.png");

    depthBufVals = QVector<float>(width() * height());
    glReadPixels(0, 0, width(), height(), GL_DEPTH_COMPONENT, GL_FLOAT, depthBufVals.data());

//    if (renderableVectorsList.length() % 5 == 0)
//        render3DVectors(renderableVectorsList);

    glDeleteVertexArrays(1, &groundPlaneVAO);
    mVBO.release();
    m_program->release();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    painter.endNativePainting();

    QPen pen(Qt::red);
    painter.setPen(pen);

//    painter.drawEllipse(10,10,100,100);

    if (cutStrokeList.length()>0)
    {
        for (int i=0; i<cutStrokeList.length(); i++)
        {
            painter.drawLine(*cutStrokeList[i]);
        }
    }

//    if (subDividedLinesList.length()>0)
//    {
//        painter.drawPolyline(*subDividedLinesList[0]);
//    }

//    painter.drawPolyline(cutStroke_1);
//    painter.end();

}



void GLWidget::resizeGL(int w, int h)
{
    glViewport(0,0,w,h);
    m_proj.setToIdentity();
//    m_proj.perspective(45.0f, GLfloat(w) / h, -2000.f, 4000.0f);
//    m_camera.translate(0,0,400);
    m_proj.ortho(0-w,w,-h,h,-2000.f, 4000.0f);
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    // selectedAlienAndBP_ID will always have only one item in it
    // For mouse picking
    if (event->button() == Qt::LeftButton)
    {
//        qDebug() << depthBufVals[(height() - event->pos().y()) * width() + event->pos().x()];
        glm::vec3 worPos1 = glm::unProject(
                                           glm::vec3(event->pos().x(),height() - event->pos().y(),depthBufVals[(height() - event->pos().y()) * width() + event->pos().x()]),
                                           glm::make_mat4((m_world * m_camera).constData()),
                                           glm::make_mat4(m_proj.constData()),
                                           glm::vec4(0,0,width(),height())
                                          );
        QVector3D hitPt3D(worPos1.x, worPos1.y, worPos1.z);
        qDebug() << hitPt3D;

        BodyPart *bpPtr = selectBP_FromMouseClick(hitPt3D);
    }
    m_lastPos = event->pos(); // our viewport coord system is such that: top left is (0,0,0) and z is negative outside the screen, while positive inside the screen
}

// Problem 1: After I move the box, when I click on that box again, sometimes it doesn't get picked/selected. Instead, another box gets picked up. Can be solved in getSelBoxyMuscleIndex() func. Color picker may be the best solution
void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->position().x() - m_lastPos.x();
    int dy = event->position().y() - m_lastPos.y();

    if (R_keyPressed)
    {
        if (event->buttons() & Qt::MiddleButton)
        {
        }
        else if (event->buttons() & Qt::RightButton)
        {
        }
        if (event->buttons() & Qt::LeftButton)
        {
            if (selectedAlienAndBP_ID.first != -1 && selectedAlienAndBP_ID.second != -1) // a BP is selected
            {
//                setBP_PitchRotation(m_pitchRotForSelectedBP + 8 * dx);
                setBP_PitchRotation(dx);
                setBP_YawRotation(dy);
                BodyPart *bpPtr = alienModellerPtr->getBodyPartFromID(alienModellerPtr->getAlienFromID(selectedAlienAndBP_ID.first),
                                                                      selectedAlienAndBP_ID.second);

                GeometryAlgorithms::rotateBodyPartUsingEulerAngles(*bpPtr, m_pitchRotForSelectedBP / 4.0f,
                                                                   m_yawRotForSelectedBP / 4.0f, 0);

                GeometryAlgorithms::updateBP_CellPrimitiveGeometry(bpPtr);

                BodyPart* mirrPtr = alienModellerPtr->getMirrorOrOriginalBP(alienModellerPtr->getAlienFromID(selectedAlienAndBP_ID.first),
                                                                            bpPtr);

                if (mirrPtr)
                {
                    GeometryAlgorithms::rotateBodyPartUsingEulerAngles(*mirrPtr, (1*m_pitchRotForSelectedBP) / 4.0f,
                                                                       (-1*m_yawRotForSelectedBP) / 4.0f, 0);
                    GeometryAlgorithms::updateBP_CellPrimitiveGeometry(mirrPtr);
                }
                // a flag to indicate the BP has been rotated. this flag will be used in mouseReleased button to re-JAT the BP in a cloning manner
            }
            update();
        }
    }

    else
    {
        if (event->buttons() & Qt::MiddleButton) {
//            setXRotation(m_xRot + 8 * dx);
//            setYRotation(m_yRot + 8 * dy);
//            setZRotation(m_zRot + 8 * dx);
        }
        else if (event->buttons() & Qt::RightButton) {

            setXRotation(m_xRot + 8 * dy);
            setZRotation(m_zRot + 8 * dx);
        }
        // moving while left button down - draw a stroke / line for cutting the polygon
        if (event->buttons() & Qt::LeftButton)
        {
//            cutStroke_1 << event->pos();
//            setXRotation(m_xRot + 8 * dy);
//            setZRotation(m_zRot + 8 * dx);

//            update();
        }
    }

    m_lastPos = event->pos();
}

void GLWidget::mouseReleaseEvent(QMouseEvent *event)
{

//    if (event->button() == Qt::LeftButton)
//    {
////        cutStroke_1 << event->pos();
////        // store the first and last points in a line that is to be subdivided and checked for itersection
////        if (cutStrokeList.length()<2) // maximum a 2 strokes are allowed for our cutting algorithm
////        {
////            QLineF* tempCutStroke = new QLineF(cutStroke_1[0], event->pos());
////            cutStrokeList.append(tempCutStroke);
////        }
////        cutStroke_1.resize(0);

//        // *** For bending of the bone,
//        // We just need to click inside the glwidget and the BP should change its color.

//        this->alienModellerPtr->BPToBend = this->allBPsToRender->at(0); // first check if it's non-null
//        // change the color to red/orange
//        this->BP_Color = QVector3D(1.0, 0, 0);
//        // Call a method on alienmodeller to set the color of all other BPs to their default colors

        update();
//    }

        // send a message to MainWindow1 to update all widgets
}

void GLWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (!this->allAliensToRender.isEmpty())
    {
        // When the user double clicks inside this glwidget, the alien it renders is highlighted to RED
        if (event->button() == Qt::LeftButton)
        {
            // if the alien already exists in the selectedAliensInGLWidgets, then remove it from this list and reset its color
            QPair<int,int> p;
            p.first = this->glWidgetID;
            p.second = this->allAliensToRender.last()->alienID;
            alienModellerPtr->selectedExtremBP_InGLWidgets = QVector3D(-1,-1,-1);

            glm::vec3 worPos1 = glm::unProject(
                glm::vec3(event->pos().x(),height() - event->pos().y(),depthBufVals[(height() - event->pos().y()) * width() + event->pos().x()]),
                glm::make_mat4((m_world * m_camera).constData()),
                glm::make_mat4(m_proj.constData()),
                glm::vec4(0,0,width(),height())
                );
            QVector3D hitPt3D(worPos1.x, worPos1.y, worPos1.z);
            qDebug() << hitPt3D;
            bool extremitySelected = false;
            BodyPart *bpPtr=NULL;
            for (int i=0; i<allAliensToRender.length(); i++)
            {
                if (allAliensToRender[i]->rootBP) // if this alien has a root BP i.e. Backbone
                {
                    BP_Cell_Pair.first = NULL; BP_Cell_Pair.second = NULL;
                    QVector<BodyPart *> allRenderableBPsList; allRenderableBPsList.clear();
                    QVector<int> allRenderableBPsIDsList; allRenderableBPsIDsList.clear();
                    if (displayAlienFlag)
                    {
                        // get a list of all those BPs that are currently displayable in this GLW
                        if (alienModellerPtr->getAllRenderableBPs(allAliensToRender[i], allRenderableBPsList, this->glWidgetID))
                            bpPtr = alienModellerPtr->getBP_PickedByMouse(allRenderableBPsList, hitPt3D, BP_Cell_Pair);
                    // }
                    // else
                    // {
                    //     qDebug() << "reached";
                    //     bpPtr = alienModellerPtr->getBP_PickedByMouse(*this->allBPsToRender, hitPt3D, BP_Cell_Pair);
                    }

                    if (bpPtr)
                    {
                        // ensure selectedAlienAndBP_ID brings back a valid Alien and BP ID
                        if (bpPtr->BP_Type == AlienModeller::BodyPartsEnum::ExtremBP_Carpal  ||  bpPtr->BP_Type == AlienModeller::BodyPartsEnum::ExtremBP_Digit)
                        {
                            Alien *selectedAlienPtr =  alienModellerPtr->getAlienFromID(allAliensToRender[i]->alienID);
                            if (selectedAlienPtr)
                            {
                                QVector<BodyPart *> allRenderableBPsList; allRenderableBPsList.clear();
                                for (BodyPart *bp : allRenderableBPsList)
                                {
                                    if (!allRenderableBPsIDsList.contains(bp->BP_ID))
                                        allRenderableBPsIDsList << bp->BP_ID;
                                }
                                QVector<int> BP_ListToDisplayInDiffColor; BP_ListToDisplayInDiffColor.clear();

                                for (ExtremityBP *extBP : selectedAlienPtr->alienExtremities)
                                {
                                    // check if this extBP has the selected BP (carpal or digit)
                                    QVector<BodyPart*> digits, carpals; digits.clear(); carpals.clear();
                                    extBP->getAllCarpalsAndDigits(digits, carpals);
                                    // Now check if this selected BP belongs to Carpals or Digits
                                    if (carpals.contains(bpPtr)  ||  digits.contains(bpPtr)) // we have found the Ext BP whose carpal or digit has been selected by mouse
                                    {
                                        // Now store this extBP in the alienModellerPtr->selectedExtremBP_InGLWidgets.
                                        alienModellerPtr->selectedExtremBP_InGLWidgets = QVector3D(this->glWidgetID, selectedAlienPtr->alienID, extBP->BP_ID);
                                        // also color this entire extBP as ORANGE
                                        for (BodyPart *ca : carpals)
                                            BP_ListToDisplayInDiffColor << ca->BP_ID;
                                        for (BodyPart *dg : digits)
                                            BP_ListToDisplayInDiffColor << dg->BP_ID;

                                        alienModellerPtr->updateGLW_to_Alien_to_BP_to_ColorDisplay_Map(selectedAlienPtr,
                                                                                                       this->glWidgetID, // get the GLW_ID
                                                                                                       allRenderableBPsIDsList, BP_ListToDisplayInDiffColor,
                                                                                                       AlienModeller::ColorEnum::BLUE_VIOLET);

                                        qDebug() << alienModellerPtr->selectedExtremBP_InGLWidgets;
                                        extremitySelected = true;
                                        break;
                                    }
                                }
                            }
                            else
                                qDebug() << "Alien does not exist";
                        }
                        else
                        {
                            qDebug() << "An entire Ext BP can only be selected using Right click. Please select any other BP using left click";
                        }
                    }
                    else
                    {
                        qDebug() << "No valid BP";
                    }
                }
                else
                    qDebug() << "No root joint for this alien";

                if (extremitySelected)
                    break;
            }


            if (!extremitySelected)
            {
                // remove it and reset its color
                if (this->alienModellerPtr->selectedAliensInGLWidgets.removeOne(p))
                {
                    // reset the color
                    // this->allAliensToRender.last()->initAlienColorMap();
                    QVector<int> BP_ListToDisplay;  BP_ListToDisplay.clear();
                    QVector<int> BP_ListToDisplayInDiffColor; BP_ListToDisplayInDiffColor.clear();
                    BP_ListToDisplay = this->alienModellerPtr->GLW_to_Alien_to_BP_to_ColorAndDisplay_Map.value(p.first).value(p.second).keys();
                    // update the GLW_to_Alien_to_BP_to_ColorDisplay_Map
                    this->alienModellerPtr->updateGLW_to_Alien_to_BP_to_ColorDisplay_Map(this->allAliensToRender.last(),
                                                                                         this->glWidgetID, // get the GLW_ID
                                                                                         BP_ListToDisplay, BP_ListToDisplayInDiffColor, -1);
                }
                else // Store the selected alien in the selectedAliensInGLWidgets
                {
                    this->alienModellerPtr->selectedAliensInGLWidgets.prepend(p);
                    // this->allAliensToRender.last()->setAlienColorMap(2,AlienModeller::ColorEnum::RED,
                    //                                                  3,AlienModeller::ColorEnum::RED,
                    //                                                  4,AlienModeller::ColorEnum::RED);
                    QVector<int> BP_ListToDisplay;  BP_ListToDisplay.clear();
                    QVector<int> BP_ListToDisplayInDiffColor; BP_ListToDisplayInDiffColor.clear();
                    BP_ListToDisplay = this->alienModellerPtr->GLW_to_Alien_to_BP_to_ColorAndDisplay_Map.value(p.first).value(p.second).keys();
                    BP_ListToDisplayInDiffColor.append(BP_ListToDisplay);
                    // update the GLW_to_Alien_to_BP_to_ColorDisplay_Map
                    this->alienModellerPtr->updateGLW_to_Alien_to_BP_to_ColorDisplay_Map(this->allAliensToRender.last(),
                                                                                         this->glWidgetID, // get the GLW_ID
                                                                                         BP_ListToDisplay, BP_ListToDisplayInDiffColor, AlienModeller::ColorEnum::RED);
                }
                qDebug() << alienModellerPtr->selectedExtremBP_InGLWidgets;
            }
        }

        update();
    }
}


// checks the intersection of the ray with each box's polygons and return the index of the box which the poly belongs to
int GLWidget::getSelBoxyMuscleIndex(QVector3D pt)
{
    int index=-1;
    float min_dst = std::numeric_limits<float>::max();
    for (int i=0; i<boxyMuscleArr.length(); i++)
    {
        QVector<QVector3D> &tempBoxyMusclePolys = boxyMuscleArr[i]->boxyMusclePolys;
        for (int j=0,k=0; j<tempBoxyMusclePolys.length(); j++,k++) // for each face/polygon defined by 4 vertices
        {
            GLfloat res = (tempBoxyMusclePolys[j]-pt).length();
            if(res < min_dst)
            {
                index = i;
                min_dst = res;
            }
            //qDebug() << res;
        }
    }
    return index;
}

void GLWidget::renderBoxNormals()
{
    QVector<QVector3D> &tempNormList = rotBoxyMuscle->polyNormals;
    QVector<QVector3D> midPnts;

    boxNormals.resize(0);

    // find the midpoints of the faces
    for (int i=0,j=0; i<rotBoxyMuscle->faceList.length(); i+=4,j++)
    {
        midPnts.append(QVector3D( (rotBoxyMuscle->BM_vertices[rotBoxyMuscle->faceList[i]].x()+rotBoxyMuscle->BM_vertices[rotBoxyMuscle->faceList[i+2]].x())/2.0, (rotBoxyMuscle->BM_vertices[rotBoxyMuscle->faceList[i]].y()+rotBoxyMuscle->BM_vertices[rotBoxyMuscle->faceList[i+2]].y())/2.0, (rotBoxyMuscle->BM_vertices[rotBoxyMuscle->faceList[i]].z()+rotBoxyMuscle->BM_vertices[rotBoxyMuscle->faceList[i+2]].z())/2.0  ));
        QVector3D secondPnt(midPnts.last().x()+tempNormList[j].x(), midPnts.last().y()+tempNormList[j].y(), midPnts.last().z()+tempNormList[j].z());

        boxNormals.append(midPnts.last().x());
        boxNormals.append(midPnts.last().y());
        boxNormals.append(midPnts.last().z());
        boxNormals.append(secondPnt.x());
        boxNormals.append(secondPnt.y());
        boxNormals.append(secondPnt.z());
    }

    m_program->setUniformValue(m_colorLoc, QVector3D(1.0,0.0,0.0));

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
    mVBO.allocate(boxNormals.constData(), boxNormals.length() * sizeof(GLfloat));
    glLineWidth(2.0);
    glDrawArrays(GL_LINES, 0, boxNormals.length()/3);
}

void GLWidget::render3DVectors(QVector<QPair<QVector3D, QVector3D> > &vectorsList)
{
    QVector<GLfloat> tempVectorList;
    tempVectorList.resize(0);

    // find the midpoints of the faces
    for (int i=0; i<vectorsList.length(); i++)
    {
        tempVectorList.append(vectorsList[i].first.x());
        tempVectorList.append(vectorsList[i].first.y());
        tempVectorList.append(vectorsList[i].first.z());
        tempVectorList.append(vectorsList[i].second.x());
        tempVectorList.append(vectorsList[i].second.y());
        tempVectorList.append(vectorsList[i].second.z());
    }

    m_program->setUniformValue(m_colorLoc, QVector3D(1.0,0.0,0.0));

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
    mVBO.allocate(tempVectorList.constData(), tempVectorList.length() * sizeof(GLfloat));
    glLineWidth(2.0);
    glDrawArrays(GL_LINES, 0, tempVectorList.length()/3);
}

void GLWidget::createRotGizmoGeometry()
{
    rotGizmoPivotVec = QVector3D(0,0,1);
    QVector3D point = QVector3D(0,0,50);

    // rederable rotation circles that will serve as the Rotation Gizmo
    pitchCircle.clear(); yawCircle.clear(); rollCircle.clear();

    // now start rotating the point, which will be stored for the vertices of the rederable rotation circles
    QMatrix4x4 yawPitchRotMat, rollRotMat; yawPitchRotMat.setToIdentity();
    yawPitchRotMat.rotate(10, QVector3D(1,0,0));
    for (int i=0; i<36; i++)
    {
        point = yawPitchRotMat.map(point);
        pitchCircle.append(point);
    }

    point = QVector3D(0,0,50);
    yawPitchRotMat.setToIdentity();
    yawPitchRotMat.rotate(10, QVector3D(0,1,0));
    for (int i=0; i<36; i++)
    {
        point = yawPitchRotMat.map(point);
        yawCircle.append(point);
    }

    point = QVector3D(0,50,0);
    yawPitchRotMat.setToIdentity();
    yawPitchRotMat.rotate(10, QVector3D(0,0,1));
    for (int i=0; i<36; i++)
    {
        point = yawPitchRotMat.map(point);
        rollCircle.append(point);
    }
}

void GLWidget::renderRotGizmo(BodyPart *bpPtr)
{
    // First store all three rotation circles in one list
    QVector<QVector3D> allRotGizmoGeometries; allRotGizmoGeometries.clear();

    allRotGizmoGeometries << rollCircle << yawCircle << pitchCircle;
    allRotGizmoGeometries.prepend(QVector3D(0,0,0));

    // Transform these
    QQuaternion tempQuat = QQuaternion::rotationTo((rotGizmoPivotVec), bpPtr->getPivotVector());
    for (int j=0; j<allRotGizmoGeometries.length(); j++)
    {
        allRotGizmoGeometries[j] = tempQuat.rotatedVector(allRotGizmoGeometries[j]);
    }
    rotGizmoPivotVec = tempQuat.rotatedVector(rotGizmoPivotVec);

    QMatrix4x4 translMat; translMat.setToIdentity();
    translMat.translate(bpPtr->getPivotPoint() - allRotGizmoGeometries.first());
    for (int j=0; j<allRotGizmoGeometries.length(); j++)
    {
        allRotGizmoGeometries[j] = translMat.map(allRotGizmoGeometries[j]);
    }

    // Get the list ready for rendering
    QVector<GLfloat> tempVerticesList;
    tempVerticesList.resize(0);
    for (int i=0; i<allRotGizmoGeometries.length(); i++)
    {
        tempVerticesList.append(allRotGizmoGeometries[i].x());
        tempVerticesList.append(allRotGizmoGeometries[i].y());
        tempVerticesList.append(allRotGizmoGeometries[i].z());
    }

    m_program->setUniformValue(m_colorLoc, QVector3D(1.0,0.0,0.0));

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
    mVBO.allocate(tempVerticesList.constData(), tempVerticesList.length() * sizeof(GLfloat));
    glLineWidth(2.0);
    glDrawArrays(GL_LINES, 0, tempVerticesList.length()/3);

}

void GLWidget::renderAllBodyParts()
{
//    QMap<int, int> BP_colorFlagList; BP_colorFlagList.clear();
    if (!this->allBPsToRender->empty())
    {
    //    QVector<Cell*> tempCellList;
        QVector<QVector<Cell*>> tempCellMatrix;

        // TODO(ismail): make a method containing the code below and call it from this method and renderAllAliens()
//        alienModellerPtr->getAllBodyPartsCellListForRendering(allAliensToRender.last(), *allBPsToRender, tempCellMatrix, this->BP_colorFlagList);

        // Extract the entry from the BP_colorFlagList to figure out which BP's cells need to be highlighted(colored red)
        QPair<int,int> tempPair;
        tempPair.first = -1; tempPair.second = -1;
        QMapIterator<int, QPair<int,int>> iter(this->BP_colorFlagList);
        while (iter.hasNext())
        {
            iter.next();
            if (iter.value().second == 1)
            {
                tempPair = iter.value();
                break;
            }
        }

//        qDebug() << "tempCellMatrix size : " << tempCellMatrix.length();

        for (int j=0; j<tempCellMatrix.length(); j++)
        {
//            BP_Color = alienModellerPtr->colorMap.value(allAliensToRender.last()->BP_TypeColorMap.value(j));

//            qDebug() << tempPair.first;
//            if (tempPair.first == j)
//                BP_Color = QVector3D(1.0,0.0,0.0);

//            m_program->setUniformValue(m_colorLoc, BP_Color);

            for (int k=0; k<tempCellMatrix[j].length(); k++)
            {
                // here we can put conditions for choosing to display or not
                mVBO.allocate(tempCellMatrix[j][k]->cellBox->constData(), tempCellMatrix[j][k]->cellBox->m_data.length() * sizeof(GLfloat));
                glDrawArrays(GL_TRIANGLES, 0, tempCellMatrix[j][k]->cellBox->m_data.length()/6);
            }
        }
    }
}

void GLWidget::renderAllAliens(QOpenGLShaderProgram *shader)
{
    // First draw the ground plane
    // glBindVertexArray(groundPlaneVAO);
    // glDrawArrays(GL_TRIANGLES, 0, 6);

    // Now draw the alien
    QVector<QVector<Cell*>> tempCellMatrix; // Each row contains all the cells of one BP
    QMap<int, int> tempCellListToBP_Map__forColoring; tempCellListToBP_Map__forColoring.clear();
    QVector3D BP_Color;
    for (int i=0; i<allAliensToRender.length(); i++)
    {
        Alien *tempAlien = allAliensToRender[i];
        // Get a list of only those BPs that need to be displayed in this GLW
        QVector<BodyPart *> allRenderableBPsList; allRenderableBPsList.clear();
        if (alienModellerPtr->getAllRenderableBPs(tempAlien, allRenderableBPsList, this->glWidgetID))
        {
//            qDebug() << allRenderableBPsList.length();
            alienModellerPtr->getAllBPsCellListForRendering(allRenderableBPsList, tempCellMatrix, tempCellListToBP_Map__forColoring);

            for (int j=0; j<tempCellMatrix.length(); j++)
            {
                BP_Color = alienModellerPtr->getBP_Color(this->glWidgetID, tempAlien, tempCellListToBP_Map__forColoring.value(j));

                shader->setUniformValue(m_colorLoc, BP_Color);

                for (int k=0; k<tempCellMatrix[j].length(); k++)
                {
                    if (BP_Cell_Pair.second == tempCellMatrix[j][k])
                        shader->setUniformValue(m_colorLoc, QVector3D(0,1,0));
                    else
                        shader->setUniformValue(m_colorLoc, BP_Color);
                    // here we can put conditions for choosing to display or not
                    if (!tempCellMatrix[j][k]->cellBox->bevelBox_m_data.isEmpty())
                    {
                        mVBO.allocate(tempCellMatrix[j][k]->cellBox->constData(), tempCellMatrix[j][k]->cellBox->bevelBox_m_data.length() * sizeof(GLfloat));
                        glDrawArrays(GL_TRIANGLES, 0, tempCellMatrix[j][k]->cellBox->bevelBox_m_data.length()/6);
                    }
                    else
                    {
                        mVBO.allocate(tempCellMatrix[j][k]->cellBox->constData(), tempCellMatrix[j][k]->cellBox->m_data.length() * sizeof(GLfloat));
                        glDrawArrays(GL_TRIANGLES, 0, tempCellMatrix[j][k]->cellBox->m_data.length()/6);
                    }
                }
            }
        }
    }
}

void GLWidget::assignAlienForRendering(Alien *alienPtr)
{
    if (!allAliensToRender.contains(alienPtr))
        allAliensToRender.append(alienPtr);
}

void GLWidget::removeAlienForRendering(Alien *alienPtr)
{
    allAliensToRender.removeOne(alienPtr);
}

void GLWidget::assignBodyPartsToRender(Alien *alienPtr, QVector<int> &allBPsIDToRender, bool displayAlienFlag)
{
    this->allBPsToRender = new QVector<BodyPart*>(0);

    for (int i=0; i<allBPsIDToRender.length(); i++)
    {
        allBPsToRender->append(this->alienModellerPtr->getBodyPartFromID(alienPtr, allBPsIDToRender[i]));
    }
    if (this->allAliensToRender.empty())
        assignAlienForRendering(alienPtr);

    this->displayAlienFlag = displayAlienFlag;
}

void GLWidget::attachAllGeometryToCenterPoint()
{

}

void GLWidget::setBP_PitchRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != m_pitchRotForSelectedBP)
    {
        m_pitchRotForSelectedBP = angle;
    }
}

void GLWidget::setBP_YawRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != m_yawRotForSelectedBP)
    {
        m_yawRotForSelectedBP = angle;
    }
}

void GLWidget::setBP_RollRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != m_rollRotForSelectedBP)
    {
        m_rollRotForSelectedBP = angle;
    }
}

/*
void GLWidget::renderAlienGeometryRecursively(Cell *rootPtr)
{
    // here we can put conditions for choosing to display or not
    // render the parent cell
    m_BoxyMuscleVbo.allocate(rootPtr->cellBox->constData(), rootPtr->cellBox->m_data.length() * sizeof(GLfloat)); // here we can put conditions for choosing to display or not
    glDrawArrays(GL_TRIANGLES, 0, rootPtr->cellBox->m_data.length()/6); // here we can put conditions for choosing to display or not

    // go through all the children of this cell and render them recursively
    for (int i=0; i<rootPtr->childCells.length(); i++)
    {
        m_program->setUniformValue(m_colorLoc, QVector3D(i*0.25,i*0.25f,1.0));
        renderAlienGeometryRecursively(rootPtr->childCells[i]);
    }
}
*/

/*
void GLWidget::renderAlien(BodyPart &rootBP) // rootBP is the first bodypart in the list of BodyPartList
{
    // first go through all the cells of this BP and render these recursively
    for (int i=0; i<rootBP.cellList.length(); i++)
    {
        renderAlienGeometryRecursively(&rootBP.cellList[i]);
    }
    // go through all the alien's body parts and render their cells recursively
    for (int i=0; i<rootBP.subBodyParts.length(); i++)
    {
        renderAlien(rootBP.subBodyParts[i]);
    }
}
*/

// this method returns the face on which the stroke is drawn
int GLWidget::getStrokeFace(QVector<int> &whichFaceList)
{
    QVector<QVector3D> &tempNormList = rotBoxyMuscle->polyNormals;

    bool found=false;

    for (int i=0; i<strokeLineIn3D.length(); i+=3)
    {
        QVector3D currPnt(strokeLineIn3D[i], strokeLineIn3D[i+1], strokeLineIn3D[i+2]);
        found=false;
        for (int j=0,k=0; j<rotBoxyMuscle->faceList.length(); j+=4,k++)
        {
            QVector3D vec1(rotBoxyMuscle->BM_vertices[rotBoxyMuscle->faceList[j]].x()-currPnt.x(),
                           rotBoxyMuscle->BM_vertices[rotBoxyMuscle->faceList[j]].y()-currPnt.y(),
                           rotBoxyMuscle->BM_vertices[rotBoxyMuscle->faceList[j]].z()-currPnt.z());

            QVector3D vec2(rotBoxyMuscle->BM_vertices[rotBoxyMuscle->faceList[j+1]].x()-currPnt.x(),
                           rotBoxyMuscle->BM_vertices[rotBoxyMuscle->faceList[j+1]].y()-currPnt.y(),
                           rotBoxyMuscle->BM_vertices[rotBoxyMuscle->faceList[j+1]].z()-currPnt.z());

            // find the normal vector
            QVector3D nor1 = QVector3D::normal(vec1,vec2);
            for (int nI=0; nI<3; nI++)
            {
                double diff = qAbs(nor1[nI] - (tempNormList[k])[nI]);
                if (diff <= 0.04999)
                {
                    nor1[nI] = (tempNormList[k])[nI];
                }
            }

            qDebug() << nor1 << "   " << tempNormList[k];

            if ( QVector3D::crossProduct(nor1,tempNormList[k]) == QVector3D(0,0,0) )
            {
                whichFaceList.append(k);
                found=true;
            }
        }
        if (found == false)
            whichFaceList.append(-1);
    }

//    qDebug() << whichFaceList.length();
    QVector<int> allFreq;
    allFreq.resize(0);

    for (int fI=0; fI<tempNormList.length(); fI++)
    {
        int freq = std::count(whichFaceList.begin(), whichFaceList.end(), fI);
        allFreq.append(freq);
    }
    int freq = std::count(whichFaceList.begin(), whichFaceList.end(), -1);
    allFreq.append(freq); // the last one will be for -1

    for (int i=0; i<whichFaceList.length(); i++)
    {
        qDebug() << whichFaceList[i];
    }

    int maxFreq = *std::max_element(allFreq.begin(), allFreq.end());
    int finalRes;
    for (int i=0; i<allFreq.length(); i++)
    {
        if (allFreq[i] == maxFreq)
        {
            finalRes = i;
            break;
        }
    }

    return finalRes; // return the face number

}


void GLWidget::createCuttingPlane(BoxyMuscle &boxyMuscle, QVector<QVector3D> &cutPlaneVertices)
{
    // try rendering the line onto the plane (in 3D)
    // first find the discrete points on the line
    strokeLineIn3D.resize(0);

    if (cutStrokeList.length()>0)
    {
        // we'll try drawing only the first stroke in 3D
        subdivideLine(cutStrokeList[0]);
        // now find the 3D counterparts of these points using glm::unproject
        QPolygon *poly = subDividedLinesList[0];
        for (int i=0; i<poly->length(); i++)
        {
            QPoint* tempPnt = new QPoint(poly->at(i));
            glm::vec3 worPos1 = glm::unProject(glm::vec3(tempPnt->x(),height() - tempPnt->y(),depthBufVals[(height() - tempPnt->y()) * width() + tempPnt->x()]),glm::make_mat4((m_camera * m_world).constData()),glm::make_mat4(m_proj.constData()),glm::vec4(0,0,width(),height()));
            strokeLineIn3D.append(worPos1.x);
            strokeLineIn3D.append(worPos1.y);
            strokeLineIn3D.append(worPos1.z);

            qDebug() << worPos1.x << "," << worPos1.y << "," << worPos1.z;
        }
        drawRay=true;
    }

    // now i need to create the cutting plane and populate it
    getCuttingPlaneVert(cutPlaneVertices);

    // cutting plane normal vector
    QVector3D norCtPl = QVector3D::normal(cutPlaneVertices[1], cutPlaneVertices[2], cutPlaneVertices[0]);

    // check the points. lets render this plane
    cuttingPlane.resize(0);
    for (int i=0; i<4; i++)
    {
        qDebug() << cutPlaneVertices[i];

        cuttingPlane.append(cutPlaneVertices[i].x());
        cuttingPlane.append(cutPlaneVertices[i].y());
        cuttingPlane.append(cutPlaneVertices[i].z());

        cuttingPlane.append(norCtPl.x());
        cuttingPlane.append(norCtPl.y());
        cuttingPlane.append(norCtPl.z());
    }
}


void GLWidget::getCuttingPlaneVert(QVector<QVector3D> &cutPlaneVertices)
{

    QVector<int> whichFaceList; // list to store the face number that each unprojected point (3D stroke vertex) lies on
    whichFaceList.resize(0);

    int faceNum = getStrokeFace(whichFaceList);
    qDebug() << "stroked face: " <<  faceNum; // this is the face number

    int midIndWhFcLst = qRound(whichFaceList.length()/2.0); // some bug here needs removal

    if (whichFaceList[midIndWhFcLst] == faceNum)
    {
        int ltMost=0, rtMost=0;
        for (int i=midIndWhFcLst+1, j=midIndWhFcLst-1; i<whichFaceList.length() || j>0; i++, j--)
        {
            if (whichFaceList[i] != whichFaceList[j])
            {
                break;
            }
            ltMost=j; rtMost=i;
        }

        // find a vector
        QVector3D tempVec1(strokeLineIn3D[rtMost*3+0]-strokeLineIn3D[ltMost*3+0],
                           strokeLineIn3D[rtMost*3+1]-strokeLineIn3D[ltMost*3+1],
                           strokeLineIn3D[rtMost*3+2]-strokeLineIn3D[ltMost*3+2]);
        tempVec1 *= 5;

        QVector3D tempVtx1(strokeLineIn3D[midIndWhFcLst*3],
                           strokeLineIn3D[midIndWhFcLst*3+1],
                           strokeLineIn3D[midIndWhFcLst*3+2]);


        QVector3D tempPt1 = tempVtx1 + tempVec1;
        QVector3D tempPt2 = tempVtx1 + tempVec1*(-1);

        // get the normal vector of this face and add it to the end points to get the corner points of the cutting plane
        QVector3D tempNorVec1 = rotBoxyMuscle->polyNormals[faceNum]*1;

        QVector3D planePt1 = tempPt1 + tempNorVec1*(1000);
        QVector3D planePt2 = tempPt2 + tempNorVec1*(1000);
        QVector3D planePt3 = tempPt2 + tempNorVec1*(-2000);
        QVector3D planePt4 = tempPt1 + tempNorVec1*(-2000);

        cutPlaneVertices.resize(0);
        cutPlaneVertices.append(planePt1); cutPlaneVertices.append(planePt2);
        cutPlaneVertices.append(planePt3); cutPlaneVertices.append(planePt4);
    }
    else
    {
        qDebug() << "not a proper stroke - please draw a proper stroke";
        // the stroke is not very well  drawn...maybe ask the user to draw a new stroke
    }
}

void GLWidget::subdivideLine(QLineF *line)
{
    QPolygon* newPolyLine = new QPolygon();

    for (float i=0; i<=1; i+=0.025)
    {
        QPoint newPoint(line->pointAt(i).x(), line->pointAt(i).y());
        newPolyLine->append(newPoint);
        qDebug() << newPolyLine->last();
    }

    subDividedLinesList.append(newPolyLine);
}

BodyPart *GLWidget::selectBP_FromMouseClick(QVector3D hitPt3D)
{
    // make a ray from screen to hitPt3D. Add it to renderableVectorsList
    //        renderableVectorsList.append(QPair<QVector3D,QVector3D>(QVector3D(event->pos().x(),height() - event->pos().y(),(height() - event->pos().y()) * width() + event->pos().x()) , hitPt3D));

    // Now identify the BP that this ray has hit
    //        BodyPart *pickedBP = alienModellerPtr->getClosestBPFromPoint(hitPt3D, this->allBPsToRender);
    BodyPart *tempBP_Ptr=NULL;
    for (int i=0; i<allAliensToRender.length(); i++)
    {
        if (allAliensToRender[i]->rootBP) // if this alien has a root BP i.e. Backbone
        {
            BP_Cell_Pair.first = NULL; BP_Cell_Pair.second = NULL;
            if (displayAlienFlag)
            {
                // get a list of all those BPs that are currently displayable in this GLW
                QVector<BodyPart *> allRenderableBPsList; allRenderableBPsList.clear();
                if (alienModellerPtr->getAllRenderableBPs(allAliensToRender[i], allRenderableBPsList, this->glWidgetID))
                    tempBP_Ptr = alienModellerPtr->getBP_PickedByMouse(allRenderableBPsList, hitPt3D, BP_Cell_Pair);
            }
            else
            {
                qDebug() << "reached";
                tempBP_Ptr = alienModellerPtr->getBP_PickedByMouse(*this->allBPsToRender, hitPt3D, BP_Cell_Pair);
            }

            if (tempBP_Ptr != NULL)
            {
                qDebug() << "BP picked by mouse";

                selectedAlienAndBP_ID.first = allAliensToRender[i]->alienID;
                selectedAlienAndBP_ID.second = tempBP_Ptr->BP_ID;

                // If the Alien is fully selected (in red color due to double click), do not select the
                // ... BP and wait for the user to deselect the alien first
                QPair<int,int> p;
                p.first = this->glWidgetID;
                p.second = allAliensToRender[i]->alienID;
                if (!this->alienModellerPtr->selectedAliensInGLWidgets.contains(p))
                {
                    alienModellerPtr->selectedAlienAndBP_ID.clear();
                    alienModellerPtr->selectedAlienAndBP_ID.insert(this->glWidgetID,
                                                                   QPair<int,int>(selectedAlienAndBP_ID.first,selectedAlienAndBP_ID.second));

                    // change the color of the BP to red
                    // First reset all the colors
                    this->allAliensToRender.last()->initAlienColorMap();
                    // Update the color of this alien's BPs
                    QVector<int> BP_ListToDisplay;  BP_ListToDisplay.clear();
                    QVector<int> BP_ListToDisplayInDiffColor; BP_ListToDisplayInDiffColor.clear();
                    BP_ListToDisplay = QVector<int>::fromList(this->alienModellerPtr->GLW_to_Alien_to_BP_to_ColorAndDisplay_Map.value(p.first).value(p.second).keys());
                    BP_ListToDisplayInDiffColor << tempBP_Ptr->BP_ID;
                    // update the GLW_to_Alien_to_BP_to_ColorDisplay_Map
                    this->alienModellerPtr->updateGLW_to_Alien_to_BP_to_ColorDisplay_Map(allAliensToRender[i],
                                                                                         this->glWidgetID,
                                                                                         BP_ListToDisplay,
                                                                                         BP_ListToDisplayInDiffColor,
                                                                                         AlienModeller::ColorEnum::RED);
                    qDebug() << "BP color changed";
                }

            }
            else
            {
                qDebug() << "BP not picked by mouse";

                selectedAlienAndBP_ID.first = -1;
                selectedAlienAndBP_ID.second = -1;

                QPair<int,int> p;
                p.first = this->glWidgetID;
                p.second = allAliensToRender[i]->alienID;
                if (!this->alienModellerPtr->selectedAliensInGLWidgets.contains(p))
                {
                    alienModellerPtr->selectedAlienAndBP_ID.clear();

                    // change the color of the BP back to normal
                    this->allAliensToRender.last()->initAlienColorMap();
                    // Update the color of this alien's BPs
                    QVector<int> BP_ListToDisplay;  BP_ListToDisplay.clear();
                    QVector<int> BP_ListToDisplayInDiffColor; BP_ListToDisplayInDiffColor.clear();
                    BP_ListToDisplay = QVector<int>::fromList(this->alienModellerPtr->GLW_to_Alien_to_BP_to_ColorAndDisplay_Map.value(p.first).value(p.second).keys());
                    // update the GLW_to_Alien_to_BP_to_ColorDisplay_Map
                    this->alienModellerPtr->updateGLW_to_Alien_to_BP_to_ColorDisplay_Map(allAliensToRender[i],
                                                                                         this->glWidgetID,
                                                                                         BP_ListToDisplay,
                                                                                         BP_ListToDisplayInDiffColor,
                                                                                         -1);
                }

            }
        }
        else
            qDebug() << "No root joint for this alien";
    }
    return tempBP_Ptr;
}


void GLWidget::performCutOperation(BoxyMuscle &boxyMuscle)
{
    // Calculate the intersection points of all the faces with the cutting polygon
    QVector<QVector3D> cutPlaneVertices; // a list of 4 vertices defining the cutting plane
    createCuttingPlane(boxyMuscle, cutPlaneVertices);
    QVector3D norCtPl = QVector3D::normal(cutPlaneVertices[1], cutPlaneVertices[2], cutPlaneVertices[0]);

    // go through all the faces in the muscle/BoxyMuscle and check if cutting plane intersects it or not. If it does, find the points of intersection


    // Update the muscle/BoxyMuscle geometry

}

void GLWidget::updateBoxyMuscleGeometry()
{

}

//void GLWidget::mouseHitsPlane(QVector3D pt)
//{
//    QVector3D a(*(plane->constData()+6),  *(plane->constData()+7),  *(plane->constData()+8));
//    QVector3D n(*(plane->constData()+3), *(plane->constData()+4), *(plane->constData()+5));

//    GLint res = QVector3D::dotProduct(a-pt,n);
//    if (res==0) // its a miss
//        qDebug() << "hits plane";
//    else // its a hit
//        qDebug() << "misses plane";
//}


// to draw both 2D and 3D contents
/*
void GLWidget::paintEvent(QPaintEvent *event)
{
//    glViewport(0,0,w,h);
//    m_proj.setToIdentity();
//    m_proj.perspective(45.0f, GLfloat(w) / h, 500.0f, 4000.0f);

    QPainter painter;



    painter.begin(this);


    painter.beginNativePainting();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    m_world.setToIdentity();
    m_world.rotate(m_xRot / 16.0f, 1, 0, 0);
    m_world.rotate(m_yRot / 16.0f, 0, 1, 0);
    m_world.rotate(m_zRot / 16.0f, 0, 0, 1);

    m_BoxyMuscleVbo.create();
    m_BoxyMuscleVbo.bind();

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void *>(3 * sizeof(GLfloat)));

    m_program->bind();
    m_program->setUniformValue(m_projMatrixLoc, m_proj);
    m_program->setUniformValue(m_mvMatrixLoc, m_camera * m_world);

    QMatrix3x3 normalMatrix = m_world.normalMatrix();
//    QMatrix3x3 normalMatrix = (m_camera * m_world).normalMatrix();
    m_program->setUniformValue(m_normalMatrixLoc, normalMatrix);




    glClearColor(0, 0, 0, m_transparent ? 0 : 1);

    // Our camera never changes in this example.
    m_camera.setToIdentity();
    m_camera.lookAt(QVector3D(0,0,2000), QVector3D(0,0,0), QVector3D(0.0,1.0,0.0)); // (eye,center,up_vector)

    // Light position is fixed.
    m_program->setUniformValue(m_lightPosLoc, QVector3D(0, 0, 700));

    this->resize(this->width(),this->height());

    for (int i=0; i<this->boxyMuscleArr.length(); i++)
    {
        m_BoxyMuscleVbo.allocate(boxyMuscleArr[i]->constData(), boxyMuscleArr[i]->count() * sizeof(GLfloat)); // here we can put conditions for choosing to display or not
        glDrawArrays(GL_TRIANGLES, 0, boxyMuscleArr[i]->vertexCount()); // here we can put conditions for choosing to display or not
    }

    m_BoxyMuscleVbo.allocate(rotBoxyMuscle->constData(), rotBoxyMuscle->count() * sizeof(GLfloat)); // here we can put conditions for choosing to display or not
    glDrawArrays(GL_TRIANGLES, 0, rotBoxyMuscle->vertexCount()); // here we can put conditions for choosing to display or not

//    m_BoxyMuscleVbo.allocate(plane->constData(), plane->count() * sizeof(GLfloat));
//    glDrawArrays(GL_TRIANGLES, 0, plane->vertexCount());

    if (drawRay)
    {
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
        m_BoxyMuscleVbo.allocate(rayVerts.constData(), rayVerts.length() * sizeof(GLfloat));
        glDrawArrays(GL_LINES, 0, 2);
    }

    //QImage res = QImage(width(), height(), QImage::Format_ARGB32);
    //glReadPixels(0, 0, width(), height(), GL_RGBA, GL_UNSIGNED_BYTE, res.bits());
    //res.save("D:/test.png");

    depthBufVals = QVector<float>(width() * height());
    glReadPixels(0, 0, width(), height(), GL_DEPTH_COMPONENT, GL_FLOAT, depthBufVals.data());

    m_BoxyMuscleVbo.release();
    m_program->release();

    painter.endNativePainting();



    QPen pen(Qt::red);
    painter.setPen(pen);

    painter.drawEllipse(10,10,100,100);
    painter.end();


}

// void GLWidget::paintGL()
// {

//     QPainter painter(this);

//     painter.beginNativePainting();

//     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//     glEnable(GL_DEPTH_TEST);
//     glEnable(GL_CULL_FACE);

//     // configure depth map FBO
//     // -----------------------
//     const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
//     glGenFramebuffers(1, &depthMapFBO);
//     // create depth texture
//     glGenTextures(1, &depthMapTextVal);
//     glBindTexture(GL_TEXTURE_2D, depthMapTextVal);
//     glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//     // attach depth texture as FBO's depth buffer
//     glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
//     glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTextVal, 0);
//     glDrawBuffer(GL_NONE);
//     glReadBuffer(GL_NONE);
//     glBindFramebuffer(GL_FRAMEBUFFER, 0);
//     qDebug() << "reached paintGL 0";


//     // here we are rotating the world...I think (instead of the camera). So this could be an easy hack to avoid the trouble of implementing an orbiting camera
//     //  TODO(Ismail):   ** I thing its working ** Make the camera rotate around the object i.e. around an invisible bounding sphere or convex hull.
//     //                  This can be useful when we have several aliens to display and we can switch cameras from one alien to another, i.e. each alien will
//     //                  have its own camera and bounding sphere (convex hull) around which its camera orbits
//     m_world.setToIdentity();
//     m_camera.setToIdentity();
//     m_world.rotate(m_xRot / 16.0f, 1, 0, 0);
//     m_world.rotate(m_yRot / 16.0f, 0, 1, 0);
//     m_world.rotate(m_zRot / 16.0f, 0, 0, 1);
//     // m_camera.rotate(m_xRot / 16.0f, 1, 0, 0);
//     // m_camera.rotate(m_yRot / 16.0f, 0, 1, 0);
//     // m_camera.rotate(m_zRot / 16.0f, 0, 0, 1);

//     // QMatrix4x4 tempMat; tempMat.setToIdentity();
//     // m_camera = tempMat * m_world;

//     // Try translating the world based on key presses
//    // m_camera.translate(m_XTrans,m_YTrans,m_ZTrans);
//     mVBO.create();
//     mVBO.bind();



//     m_program->bind();
//     m_program->setUniformValue(m_projMatrixLoc, m_proj);
//     m_program->setUniformValue(m_mvMatrixLoc, m_camera * m_world);
//     m_program->setUniformValue(m_modelMatrixLoc, m_world);
//     QMatrix3x3 normalMatrix = m_world.normalMatrix();
// //    QMatrix3x3 normalMatrix = (m_camera * m_world).normalMatrix();
//     m_program->setUniformValue(m_normalMatrixLoc, normalMatrix);

//     // change the RGB color
//     m_program->setUniformValue(m_colorLoc, QVector3D(0.39,1.0,1.0));
//     qDebug() << "reached paintGL 1";

//     // // Render the gound plane
//     // // set up ground plane vertex data (and buffer(s)) and configure vertex attributes
//     // // ------------------------------------------------------------------
//     // float groundPlaneData[] = {
//     //     // positions            // normals
//     //     250.0f, -0.5f,  250.0f,  0.0f, 1.0f, 0.0f,
//     //     -250.0f, -0.5f,  250.0f,  0.0f, 1.0f, 0.0f,
//     //     -250.0f, -0.5f, -250.0f,  0.0f, 1.0f, 0.0f,

//     //     250.0f, -0.5f,  250.0f,  0.0f, 1.0f, 0.0f,
//     //     -250.0f, -0.5f, -250.0f,  0.0f, 1.0f, 0.0f,
//     //     250.0f, -0.5f, -250.0f,  0.0f, 1.0f, 0.0f
//     // };
//     // glGenVertexArrays(1, &groundPlaneVAO);
//     // glGenBuffers(1, &groundPlaneVBO);
//     // glBindVertexArray(groundPlaneVAO);
//     // glBindBuffer(GL_ARRAY_BUFFER, groundPlaneVBO);
//     // glBufferData(GL_ARRAY_BUFFER, sizeof(groundPlaneData), groundPlaneData, GL_STATIC_DRAW);
//     // glEnableVertexAttribArray(0);
//     // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
//     // glEnableVertexAttribArray(1);
//     // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

//     qDebug() << "reached paintGL 2";

// //    for (int i=0; i<this->boxyMuscleArr.length(); i++)
// //    {
// //        m_BoxyMuscleVbo.allocate(boxyMuscleArr[i]->constData(), boxyMuscleArr[i]->count() * sizeof(GLfloat)); // here we can put conditions for choosing to display or not
// //        glDrawArrays(GL_TRIANGLES, 0, boxyMuscleArr[i]->vertexCount()); // here we can put conditions for choosing to display or not
// //    }


//     glEnableVertexAttribArray(0);
//     glEnableVertexAttribArray(1);
//     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0);
//     glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void *>(3 * sizeof(GLfloat)));

//     qDebug() << "reached paintGL 3";

//     // --- SHADOW MAPPING -------------------------------------------------------------------------
//     // // 1. render depth of scene to texture (from light's perspective)
//     // // --------------------------------------------------------------
//     QMatrix4x4 lightProjection, lightView;
//     // float near_plane = 1.0f, far_plane = 7.5f;
//     float near_plane = -2000, far_plane = 4000;
//     lightProjection.ortho(0-width(),width(),-height(),height(), near_plane, far_plane);
//     lightView.lookAt(QVector3D(2000,2000,2000), QVector3D(0.f,0.f,0.f), QVector3D(0.0, 1.0, 0.0));
//     lightSpaceMatrix = lightProjection * lightView;
//     // render scene from light's point of view
//     simpleDepthShader->bind();
//     simpleDepthShader->setUniformValue(simpleDepthShader->uniformLocation("lightSpaceMatrix"), lightSpaceMatrix);
//     simpleDepthShader->setUniformValue(simpleDepthShader->uniformLocation("modelMatrix"), m_world);
//     // qDebug() << simpleDepthShader->log();
//     // qDebug() << "reached paintGL 4";

//     glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
//     glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
//     glClear(GL_DEPTH_BUFFER_BIT);
//     if (!this->allAliensToRender.empty() && displayAlienFlag)
//         renderAllAliens(simpleDepthShader);

//     glBindFramebuffer(GL_FRAMEBUFFER, 0);

//     // reset viewport
//     glViewport(0, 0, width(), height());
//     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//     qDebug() << simpleDepthShader->log();


//     // 2. render scene as normal using the generated depth/shadow map
//     // --------------------------------------------------------------
//     m_program->bind();
//     // m_proj.perspective( 45, 16.f/9.f, near_plane, far_plane);
//     // m_proj.ortho(0-width(),width(),-height(),height(), near_plane, far_plane);
//     // m_program->setUniformValue(m_projMatrixLoc, m_proj);
//     // m_program->setUniformValue(m_mvMatrixLoc, m_camera * m_world);
//     // m_program->setUniformValue(m_modelMatrixLoc, m_world);

//     // set light uniforms
//     m_program->setUniformValue(m_lightPosLoc, QVector3D(2000, 2000, 2000));
//     m_program->setUniformValue(m_program->uniformLocation("lightSpaceMatrix"), lightSpaceMatrix);
//     // glActiveTexture(GL_TEXTURE0);
//     // glBindTexture(GL_TEXTURE_2D, depthMapTextVal);
//     if (!this->allAliensToRender.empty() && displayAlienFlag)
//         renderAllAliens(m_program);
//     qDebug() << m_program->log();
// // -------------------------------------------------------------------------------------------------------------

// //    if (!this->allBPsToRender->empty())
// //        renderAllBodyParts();
// //    render3DVectors(allAliensToRender.last()->tempVectorsListForRendering);

// //    renderBoxNormals();

// //    m_BoxyMuscleVbo.allocate(plane->constData(), plane->count() * sizeof(GLfloat));
// //    glDrawArrays(GL_TRIANGLES, 0, plane->vertexCount());

// //    if (drawRay)
// //    {
// //        m_program->setUniformValue(m_colorLoc, QVector3D(1.0,0.0,0.0));
// //        glEnableVertexAttribArray(0);
// //        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
// //        m_BoxyMuscleVbo.allocate(strokeLineIn3D.constData(), strokeLineIn3D.length() * sizeof(GLfloat));
// //        glLineWidth(3.0);
// //        glDrawArrays(GL_LINE_STRIP, 0, strokeLineIn3D.length()/3);

// //        // +++ Test Rendering +++
// //        // change the RGB color
// ///*
// //        m_program->setUniformValue(m_colorLoc, QVector3D(0.39,1.0,0.60));
// //        glEnableVertexAttribArray(0);
// //        glEnableVertexAttribArray(1);
// //        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0);
// //        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void *>(3 * sizeof(GLfloat)));
// //        m_BoxyMuscleVbo.allocate(cuttingPlane.constData(), cuttingPlane.length() * sizeof(GLfloat)); // here we can put conditions for choosing to display or not
// //        glDrawArrays(GL_QUADS, 0, cuttingPlane.length()/6); // here we can put conditions for choosing to display or not
// //*/
// //    }

// //QImage res = QImage(width(), height(), QImage::Format_ARGB32);
// //glReadPixels(0, 0, width(), height(), GL_RGBA, GL_UNSIGNED_BYTE, res.bits());
// //res.save("D:/test.png");

// depthBufVals = QVector<float>(width() * height());
// glReadPixels(0, 0, width(), height(), GL_DEPTH_COMPONENT, GL_FLOAT, depthBufVals.data());

// //    if (renderableVectorsList.length() % 5 == 0)
// //        render3DVectors(renderableVectorsList);

// glDeleteVertexArrays(1, &groundPlaneVAO);
// mVBO.release();
// m_program->release();

// glDisable(GL_DEPTH_TEST);
// glDisable(GL_CULL_FACE);

// painter.endNativePainting();

// QPen pen(Qt::red);
// painter.setPen(pen);

// //    painter.drawEllipse(10,10,100,100);

// if (cutStrokeList.length()>0)
// {
//     for (int i=0; i<cutStrokeList.length(); i++)
//     {
//         painter.drawLine(*cutStrokeList[i]);
//     }
// }

// //    if (subDividedLinesList.length()>0)
// //    {
// //        painter.drawPolyline(*subDividedLinesList[0]);
// //    }

// //    painter.drawPolyline(cutStroke_1);
// //    painter.end();

// }


// -0.5   -0.5   0.5  normal:  0   0   1
// 0.5   -0.5   0.5  normal:  0   0   1
// 0.5   0.5   0.5  normal:  0   0   1
// 0.5   0.5   0.5  normal:  0   0   1
// -0.5   0.5   0.5  normal:  0   0   1
// -0.5   -0.5   0.5  normal:  0   0   1
// 0.5   -0.5   0.5  normal:  1   0   0
// 0.5   -0.5   -0.5  normal:  1   0   0
// 0.5   0.5   -0.5  normal:  1   0   0
// 0.5   0.5   -0.5  normal:  1   0   0
// 0.5   0.5   0.5  normal:  1   0   0
// 0.5   -0.5   0.5  normal:  1   0   0
// 0.5   -0.5   -0.5  normal:  0   0   -1
// -0.5   -0.5   -0.5  normal:  0   0   -1
// -0.5   0.5   -0.5  normal:  0   0   -1
// -0.5   0.5   -0.5  normal:  0   0   -1
// 0.5   0.5   -0.5  normal:  0   0   -1
// 0.5   -0.5   -0.5  normal:  0   0   -1
// -0.5   -0.5   -0.5  normal:  -1   0   0
// -0.5   -0.5   0.5  normal:  -1   0   0
// -0.5   0.5   0.5  normal:  -1   0   0
// -0.5   0.5   0.5  normal:  -1   0   0
// -0.5   0.5   -0.5  normal:  -1   0   0
// -0.5   -0.5   -0.5  normal:  -1   0   0
// -0.5   0.5   0.5  normal:  0   1   0
// 0.5   0.5   0.5  normal:  0   1   0
// 0.5   0.5   -0.5  normal:  0   1   0
// 0.5   0.5   -0.5  normal:  0   1   0
// -0.5   0.5   -0.5  normal:  0   1   0
// -0.5   0.5   0.5  normal:  0   1   0
// 0.5   -0.5   0.5  normal:  0   -1   0
// -0.5   -0.5   0.5  normal:  0   -1   0
// -0.5   -0.5   -0.5  normal:  0   -1   0
// -0.5   -0.5   -0.5  normal:  0   -1   0
// 0.5   -0.5   -0.5  normal:  0   -1   0
// 0.5   -0.5   0.5  normal:  0   -1   0

// boxy muscle
// -0.5   0.5   0.5  normal:  0   0   1
// -0.5   -0.5   0.5  normal:  0   0   1
// 0.5   -0.5   0.5  normal:  0   0   1
// 0.5   -0.5   0.5  normal:  0   0   1
// 0.5   0.5   0.5  normal:  0   0   1
// -0.5   0.5   0.5  normal:  0   0   1
// -0.5   -0.5   -0.5  normal:  0   0   -1
// -0.5   0.5   -0.5  normal:  0   0   -1
// 0.5   -0.5   -0.5  normal:  0   0   -1
// 0.5   0.5   -0.5  normal:  0   0   -1
// 0.5   -0.5   -0.5  normal:  0   0   -1
// -0.5   0.5   -0.5  normal:  0   0   -1
// -0.5   -0.5   0.5  normal:  0   -1   0
// -0.5   -0.5   -0.5  normal:  0   -1   0
// 0.5   -0.5   0.5  normal:  0   -1   0
// 0.5   -0.5   -0.5  normal:  0   -1   0
// 0.5   -0.5   0.5  normal:  0   -1   0
// -0.5   -0.5   -0.5  normal:  0   -1   0
// 0.5   0.5   0.5  normal:  0   1   0
// 0.5   0.5   -0.5  normal:  0   1   0
// -0.5   0.5   0.5  normal:  0   1   0
// -0.5   0.5   -0.5  normal:  0   1   0
// -0.5   0.5   0.5  normal:  0   1   0
// 0.5   0.5   -0.5  normal:  0   1   0
// -0.5   0.5   0.5  normal:  -1   0   0
// -0.5   0.5   -0.5  normal:  -1   0   0
// -0.5   -0.5   0.5  normal:  -1   0   0
// -0.5   -0.5   -0.5  normal:  -1   0   0
// -0.5   -0.5   0.5  normal:  -1   0   0
// -0.5   0.5   -0.5  normal:  -1   0   0
// 0.5   -0.5   0.5  normal:  1   0   0
// 0.5   -0.5   -0.5  normal:  1   0   0
// 0.5   0.5   0.5  normal:  1   0   0
// 0.5   0.5   -0.5  normal:  1   0   0
// 0.5   0.5   0.5  normal:  1   0   0
// 0.5   -0.5   -0.5  normal:  1   0   0

// */
