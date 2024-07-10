#ifndef SKETCHVIEWSCENE_H
#define SKETCHVIEWSCENE_H

#include <QObject>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPointF>
#include "logo.h"

class SketchViewScene : public QGraphicsScene
{
public:
    SketchViewScene();
    ~SketchViewScene();
    const QPointF *get2DPoints() const
    {
        return points2D.constData();
    }
    void resetPointsArr()
    {
        points2D.resize(0);
    }

    void displayY_Junction(); // For Testing
    void display2D_Y_JuncLines(QVector<QPointF> &Y_juncLines); // for testing
    void drawLineFromYtoMid(QVector<QPointF> &testingLine1); // For Testing: testingLine1: just draw a line from bounding box mid point to Y junc point

    QVector<int> Y_juncEdges;

    Logo *rotLogoPtr;
    QVector<QLineF> tempBoxLines;


private:
    bool leftButtonDown;
    bool mouseMoved;
    QPointF initPos;
    QPointF finalPos;
    // 3 points are needed by user and acquired via mouse input in 2D sketch view to compute the polygon
    // These points are needed by the inference engine
    QVector<QPointF> points2D;

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent);
    void mouseMoveEvent(QGraphicsSceneMouseEvent * mouseEvent);
    void mouseReleaseEvent ( QGraphicsSceneMouseEvent * mouseEvent );
};

#endif // SKETCHVIEWSCENE_H
