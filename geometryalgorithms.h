#ifndef GEOMETRYALGORITHMS_H
#define GEOMETRYALGORITHMS_H

#include "bodypart.h"
//#include "alienmodeller.h" // TODO(ismail): Circular dependency compile time problem.

#include <QVector>
#include <QVector3D>
#include <QDebug>
#include <math.h>       /* for fabs */
#include <box.h>
#include <QMatrix4x4>
#include <cell.h>

#define SMALL_NUM   0.00000001 // anything that avoids division overflow


class GeometryAlgorithms
{
public:
    GeometryAlgorithms();

    // Some important methods:
    // ** local transformations (position, orientation, size)
    // Code improvement point: Perhaps have a generic class of 3D models that can be passed to these transformation methods.
    // Code improvement point: Perhaps make seperate classes for mutation algorithms, formations algorithms, transformation algorithms etc
    // Need to design several methods for translations. 1) Translate to a given point. 2) Translate to a given distance along the given vector
    static void translateCell(Cell *cellPtr, QVector3D displacementVec); // Translate the cell's pivot point to the destination point
    static void translateBP(BodyPart *bp, GLfloat xDist, GLfloat yDist, GLfloat zDist);
    // Need to design another method for rotation that rotates a model (i.e its roll vector) towards a given vector (using quaternion)
    static void rotateCellUsingEulerAngles(Cell *cellPtr, GLfloat pitch, GLfloat yaw, GLfloat roll);
    static void rotateCellTowardsVecDir(Cell *cellPtr, const QVector3D &vec); // Rotates the cell (its forward direction) toward a given vector direction.
    static void rotateBodyPartUsingEulerAngles(BodyPart &bp, GLfloat pitch, GLfloat yaw, GLfloat roll);
    static void rotateBodyPartTowardsVecDir(BodyPart &bp, const QVector3D &vec);
    static QVector3D rotateVec_EulerAngles(QVector3D inVec, float pitch, float yaw, float roll);
    static void rotateVecTowardsAnotherVec(/*GrowthVector, QVector3D &axis, GLfloat angle*/);
    static void scaleModel(Box *boxPtr, float xfactor, float yfactor, float zfactor);
    static void scaleCell(Cell *cell, float xfactor, float yfactor, float zfactor); // x, y and zfactor are the pitch, yaw and roll axes respectively
    static void scaleBP(BodyPart *bp, float xfactor, float yfactor, float zfactor); // parent the BP with the bounding box and scale the BB.
    static void scalePlane(QVector<QVector3D> &plane, float xfactor, float yfactor, float zfactor);
    // v1, v2, v3 and v4 are the four vertices of the face/plane, in anti-clockwise order starting at lower-left
    static void reflectBodyPartAroundPlane(BodyPart *bpPtr, QVector3D v1, QVector3D v2, QVector3D v3, QVector3D norm);
    static void reflectCellListAroundPlane(QVector<Cell*> &tempCellList, QVector3D v1, QVector3D v2, QVector3D v3, QVector3D norm);
    // The input vertex can be a vector as well
    static void reflectVertexAroundPlane(QVector3D &inVertex, QVector3D v1, QVector3D v2, QVector3D v3, QVector3D norm);

    static void translateBP_AccordingToInputTranslationMat(BodyPart *bpPtr, QMatrix4x4 translationMat);
    static void rotateBP_AccordingToInputRotationMat(BodyPart *bpPtr, QQuaternion quat);

    static void updateBP_CellPrimitiveGeometry(BodyPart *bpPtr);

    static int getLinePlaneIntersectionPoint(QVector3D linePt1, QVector3D linePt2, QVector3D planePt, QVector3D planeNormal, QVector3D &intersectionPt);

    // This method returns a list of intermediate points including the ending/input two points.
    // It uses the input percentages list of division of the vector between the two points
    // For e.g. (pnt1 newPt newPt newPt pnt2)
    static QVector<QVector3D> findIntermediatePntsBetweenTwoGivenPnts(QVector3D pnt1, QVector3D pnt2, QVector<float> percList);

    /**************************
     * Some other utiliy algorithms
     * **********************/

    static float getRandomFloat(const float lo, const float hi)
    {
        return lo + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(hi-lo)));
    }
    static int getRandomInteger(int lo, int hi)
    {
        return lo + rand() / (RAND_MAX/(hi-lo));
    }

/*
 * 3D Line Intersection Algorithms (Skew Lines). Source: https://vicrucann.github.io/tutorials/3d-geometry-algorithms/
*/

    /*! An algorithm to calculate the shortest distance between two skew lines.
    * \param P1 is the first point of the first line,
    * \param P12 is the second point on the first line,
    * \param P2 is the first point on the second line,
    * \param P22 is the second point on the second line.
    * \return the shortest distance
    */
    static float getSkewLinesDistance(const QVector3D &P1, const QVector3D &P12,
                                const QVector3D &P2, const QVector3D &P22)
    {
        QVector3D u1 = P12-P1;
        QVector3D u2 = P22-P2;
        QVector3D u3 = QVector3D::crossProduct(u1,u2);
        if (u3.length() == 0) return 1;
        u3.normalize();
        QVector3D dir = P1 - P2;
//        return std::fabs((dir*u3)); // u3 is already normalized
        return fabs(QVector3D::dotProduct(dir,u3));
    }


    /*! A method to project one skew line onto another.
    * \param P1 is a first point that belonds to first skew line,
    * \param P12 is the second point that belongs to first skew line,
    * \param P2 is the first point that belongs to second skew line,
    * \param P22 is the second point that belongs to second skew line,
    * \param X1 is the result projection point of line P2P22 onto line P1P12.
    * \return true if such point exists, false - otherwise.
    */
    static bool getSkewLinesProjection(const QVector3D &P1, const QVector3D &P12,
                                const QVector3D &P2, const QVector3D &P22,
                                QVector3D &X1)
    {
        QVector3D d = P2 - P1;
        QVector3D u1 = P12-P1;
        u1.normalize();
        QVector3D u2 = P22 - P2;
        u2.normalize();
        QVector3D u3 = QVector3D::crossProduct(u1,u2);

        double EPSILON = 0.00001; // original setting: double EPSILON = 0.00001;
        if (fabs(u3.x())<=EPSILON &&
            fabs(u3.y())<=EPSILON &&
            fabs(u3.z())<=EPSILON){
            qDebug() << "The rays are almost parallel.";
            return false;
        }

        // X1 and X2 are the closest points on lines
        // we want to find X1 (lies on u1)
        // solving the linear equation in r1 and r2: Xi = Pi + ri*ui
        // we are only interested in X1 so we only solve for r1.
        float a1 = QVector3D::dotProduct(u1,u1), b1 = QVector3D::dotProduct(u1,u2), c1 = QVector3D::dotProduct(u1,d);
        float a2 = QVector3D::dotProduct(u1,u2), b2 = QVector3D::dotProduct(u2,u2), c2 = QVector3D::dotProduct(u2,d);
        if (!(fabs(b1) > EPSILON)){
            qDebug() << "Denominator is close to zero.";
            return false;
        }
        if (!(a2!=-1 && a2!=1)){
            qDebug() << "Lines are parallel.";
            return false;
        }

        double r1 = (c2 - (b2*c1)/b1) / (a2-(b2*a1)/b1);
        X1 = P1 + u1*r1;

        return true;
    }


    /*! An algorithm to calculate an (approximate) intersection of two lines in 3D.
    * \param La1 is the first point on the first line,
    * \param La2 is the second point on the first line,
    * \param Lb1 is the first point on the second line,
    * \param Lb2 is the second point on the second line,
    * \param intersection is the result intersection, if it can be found.
    * \return true if the intersection can be found, false - otherwise.
    */
    static bool getLinesIntersection(const QVector3D &La1, const QVector3D &La2,
                                const QVector3D &Lb1, const QVector3D &Lb2,
                                QVector3D &intersection)
    {
        // first check if lines have an exact intersection point
        // do it by checking if the shortest distance is exactly 0
        float distance = GeometryAlgorithms::getSkewLinesDistance(La1, La2, Lb1, Lb2);
        if (distance == 0.f)
        {
            qDebug() << "3d lines have exact intersection point\n";
            QVector3D C = La2;
            QVector3D D = Lb2;
            QVector3D e = La1-La2;
            QVector3D f = Lb1-Lb2;
            QVector3D g = D-C;
            if (QVector3D::crossProduct(f,g).length()==0 || QVector3D::crossProduct(f,e).length()==0)
            {
                qDebug() << "Lines have no intersection, are they parallel?\n";
                return false;
            }

            QVector3D fgn = QVector3D::crossProduct(f,g);
            fgn.normalize();

            QVector3D fen = QVector3D::crossProduct(f,e);
            fen.normalize();

            int di = -1;
            if (fgn == fen) // same direction?
                di *= -1;

            intersection = C + e*di*( QVector3D::crossProduct(f,g).length() / QVector3D::crossProduct(f,e).length() );
            return true;
        }

        // try to calculate the approximate intersection point
        QVector3D X1, X2;
        bool firstIsDone = GeometryAlgorithms::getSkewLinesProjection(La1, La2, Lb1, Lb2, X1);
        bool secondIsDone = GeometryAlgorithms::getSkewLinesProjection(Lb1, Lb2, La1, La2, X2);

        if (!firstIsDone || !secondIsDone)
        {
            qDebug() << "Could not obtain projection point.\n";
            return false;
        }

        intersection = (X1 + X2)/2.f;
        return true;
    }
};

#endif // GEOMETRYALGORITHMS_H
