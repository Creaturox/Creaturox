#include "sketchviewscene.h"
#include <QGraphicsLineItem>
#include <QMessageBox>

#include <QDebug>

SketchViewScene::SketchViewScene()
{
    leftButtonDown=false;
    mouseMoved = false;
}

SketchViewScene::~SketchViewScene()
{

}

void SketchViewScene::displayY_Junction()
{
    for (int i=0; i<Y_juncEdges.length(); i+=2)
    {
        QPointF initPos;
        QPointF finalPos;

        initPos.setX(rotLogoPtr->vertIndices[Y_juncEdges[i]].x());
        initPos.setY(rotLogoPtr->vertIndices[Y_juncEdges[i]].y()*(-1));

        finalPos.setX(rotLogoPtr->vertIndices[Y_juncEdges[i+1]].x());
        finalPos.setY(rotLogoPtr->vertIndices[Y_juncEdges[i+1]].y()*(-1));

        QGraphicsLineItem *myLine = new QGraphicsLineItem(initPos.x(),initPos.y(),finalPos.x(),finalPos.y());
        this->addItem(myLine);

    }
}

void SketchViewScene::display2D_Y_JuncLines(QVector<QPointF> &Y_juncLines)
{
    for (int i=0; i<Y_juncLines.length(); i+=2)
    {
        QGraphicsLineItem *myLine = new QGraphicsLineItem(Y_juncLines[i].x(),Y_juncLines[i].y(),Y_juncLines[i+1].x(),Y_juncLines[i+1].y());
        this->addItem(myLine);
    }

}

void SketchViewScene::drawLineFromYtoMid(QVector<QPointF> &testingLine1)
{
    QGraphicsLineItem *myLine = new QGraphicsLineItem(testingLine1[0].x(),testingLine1[0].y(),testingLine1[1].x(),testingLine1[1].y());
    this->addItem(myLine);
}



void SketchViewScene::mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
    if (mouseEvent->button() == Qt::LeftButton)
    {
        initPos = mouseEvent->scenePos();
        // store this init pos as the first point of the line
        QLineF newLine(initPos.x(),initPos.y(),0,0);
        tempBoxLines.append(newLine);
        leftButtonDown=true;
    }
    else
        return;
}

void SketchViewScene::mouseMoveEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
    if (leftButtonDown)
    {
        mouseMoved = true;
        finalPos = mouseEvent->scenePos();
        QGraphicsLineItem *myLine = new QGraphicsLineItem(initPos.x(),initPos.y(),finalPos.x(),finalPos.y());
        this->addItem(myLine);
        initPos.setX(finalPos.x());
        initPos.setY(finalPos.y());
    }
}

void SketchViewScene::mouseReleaseEvent ( QGraphicsSceneMouseEvent * mouseEvent )
{
    if (leftButtonDown && !mouseMoved)
    {
        points2D.append(mouseEvent->scenePos());
        qDebug() << points2D.last();
    }
    else if (leftButtonDown && mouseMoved)
    {
        // store this final pos as the second point of the line
        tempBoxLines[tempBoxLines.length()-1].setP2(mouseEvent->scenePos());
        qDebug() << tempBoxLines[tempBoxLines.length()-1].p1() << " : " << tempBoxLines[tempBoxLines.length()-1].p2();
    }
    leftButtonDown=false;
    mouseMoved = false;
}

