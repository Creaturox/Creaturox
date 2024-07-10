#include "torsomodeller.h"
#include <stdlib.h>     /* srand, rand */
#include <time.h> // for taking the system time for true random number generation
#include <geometryalgorithms.h>


TorsoModeller::TorsoModeller()
{

}

UpperTorso* TorsoModeller::modelUpTorso()
{
    UpperTorso *uppTorsoPtr = new UpperTorso();

    QVector<QVector3D> &tempVertList = uppTorsoPtr->upTorsoBoxTree->vertices;


// **** ALGORITHM 1: for Grid Style torso (unmirrored version) ****
// **** LEVEL 1 - Torso Modelling ****

    // Find points A, B, L, C, D, E, F, G, H, I, J, K, U, V W, X, Y, Z, and finally P on the box

    // find 2 boundary points, at 25% and 75% along the vector (v2-v3) starting at vertex 3, to define the legal line segment
    QVector3D tempBndPnt1 = 0.25*(tempVertList[1] - tempVertList[2]) + tempVertList[2];
    QVector3D tempBndPnt2 = 0.75*(tempVertList[1] - tempVertList[2]) + tempVertList[2];

    // Now compute a vector from these boundary points and compute a 'random' point on the legal line segment

    /* initialize random seed: */
    srand (time(NULL));
    bool ptP_Valid=false;
    QVector3D ptA, ptB, ptC, ptD, ptE, ptF, ptG, ptH, ptI, ptJ, ptK, ptL, ptU, ptV, ptW, ptX, ptY, ptZ, ptP;
    int tempRandNum_1;

    do
    {
        /* generate a random number between 0 and 100: */
        tempRandNum_1 = rand() % 100;


        // Point A is found by finding random point/vertex in the legal region. initial vertex plus the %age of the legal line vector.
        ptA = (tempRandNum_1/100.0f)*(tempBndPnt2-tempBndPnt1) + tempBndPnt1;
        qDebug() << ptA;
        ptB = (tempVertList[0]-tempVertList[1]) + ptA;
        qDebug() << ptB;
        ptL = (tempVertList[4]-tempVertList[0]) + ptB;
        qDebug() << ptL;
        ptC = (tempVertList[5]-tempVertList[4]) + ptL;
        qDebug() << ptC;


        // find 2 boundary points, at 25% and 75% along the vector (v4-v3) starting at vertex 3, to define the legal line segment
        tempBndPnt1 = 0.25*(tempVertList[3] - tempVertList[2]) + tempVertList[2];
        tempBndPnt2 = 0.75*(tempVertList[3] - tempVertList[2]) + tempVertList[2];

        // Now compute a vector from these boundary points and compute a 'random' point on the legal line segment

        /* generate a random number between 0 and 100: */
        tempRandNum_1 = rand() % 100;

        // Point D is found by finding random point/vertex in the legal region. initial vertex plus the %age of the legal line vector.
        ptD = (tempRandNum_1/100.0f)*(tempBndPnt2-tempBndPnt1) + tempBndPnt1;
        qDebug() << ptD;
        ptE = (tempVertList[1]-tempVertList[2]) + ptD;
        qDebug() << ptE;
        ptJ = (tempVertList[5]-tempVertList[1]) + ptE;
        qDebug() << ptJ;
        ptF = (tempVertList[6]-tempVertList[5]) + ptJ;
        qDebug() << ptF;


        // find 2 boundary points, at 25% and 75% along the vector (v7-v3) starting at vertex 3, to define the legal line segment
        tempBndPnt1 = 0.25*(tempVertList[6] - tempVertList[2]) + tempVertList[2];
        tempBndPnt2 = 0.75*(tempVertList[6] - tempVertList[2]) + tempVertList[2];

        // Now compute a vector from these boundary points and compute a 'random' point on the legal line segment

        /* generate a random number between 0 and 100: */
        tempRandNum_1 = rand() % 100;

        // Point D is found by finding random point/vertex in the legal region. initial vertex plus the %age of the legal line vector.
        ptH = (tempRandNum_1/100.0f)*(tempBndPnt2-tempBndPnt1) + tempBndPnt1;
        qDebug() << ptH;
        ptI = (tempVertList[5]-tempVertList[6]) + ptH;
        qDebug() << ptI;
        ptK = (tempVertList[0]-tempVertList[1]) + ptI;
        qDebug() << ptK;
        ptG = (tempVertList[3]-tempVertList[0]) + ptK;
        qDebug() << ptG;

        /** Test: with the following simple modifications I can get a proper project point of two skew lines (if there is no exact intersection). I guess this is the
            approximate intersection point

           ptD[2]+=1; ptD[0]+=1;
        */

        // Points U, V, W, X, Y, and Z are found using line intersections
        qDebug() << GeometryAlgorithms::getLinesIntersection(ptL, ptB, ptG, ptK, ptU);
        qDebug() << GeometryAlgorithms::getLinesIntersection(ptC, ptL, ptF, ptJ, ptV);
        qDebug() << GeometryAlgorithms::getLinesIntersection(ptI, ptK, ptJ, ptE, ptW);
        qDebug() << GeometryAlgorithms::getLinesIntersection(ptA, ptB, ptD, ptE, ptX);
        qDebug() << GeometryAlgorithms::getLinesIntersection(ptG, ptH, ptF, ptD, ptY);
        qDebug() << GeometryAlgorithms::getLinesIntersection(ptA, ptC, ptH, ptI, ptZ);

        // ++ Observation: The method getLinesIntersection() does not always bring a valid intersection point (ptP) and brings back P(0,0,0) instead.
        // ++              Thus a fix is needed. Perhaps keep calling the methods once we have a valid point, or fix the method from inside
        ptP_Valid =  GeometryAlgorithms::getLinesIntersection(ptX, ptV, ptZ, ptU, ptP);

        qDebug() << ptP;

    } while (!ptP_Valid);

    // Now that we have all the points, its time to create sub-boxes and attach them to the parent box
    if (ptP_Valid)
    {
        QVector<QVector3D> tempBoxPnts;

        tempBoxPnts << ptX << ptA << tempVertList[2] << ptD << ptP << ptZ << ptH << ptY;
        uppTorsoPtr->upTorsoBoxTree->subBoxes.append(new Box(tempBoxPnts));

        tempBoxPnts.resize(0);
        tempBoxPnts << ptE << tempVertList[1] << ptA << ptX << ptW << ptI << ptZ << ptP;
        uppTorsoPtr->upTorsoBoxTree->subBoxes.append(new Box(tempBoxPnts));

        tempBoxPnts.resize(0);
        tempBoxPnts << tempVertList[0] << ptE << ptX << ptB << ptK << ptW << ptP << ptU;
        uppTorsoPtr->upTorsoBoxTree->subBoxes.append(new Box(tempBoxPnts));

        tempBoxPnts.resize(0);
        tempBoxPnts << ptB << ptX << ptD << tempVertList[3] << ptU << ptP << ptY << ptG;
        uppTorsoPtr->upTorsoBoxTree->subBoxes.append(new Box(tempBoxPnts));

        tempBoxPnts.resize(0);
        tempBoxPnts << ptU << ptP << ptY << ptG << ptL << ptV << ptF << tempVertList[7];
        uppTorsoPtr->upTorsoBoxTree->subBoxes.append(new Box(tempBoxPnts));

        tempBoxPnts.resize(0);
        tempBoxPnts << ptK << ptW << ptP << ptU << tempVertList[4] << ptJ << ptV << ptL;
        uppTorsoPtr->upTorsoBoxTree->subBoxes.append(new Box(tempBoxPnts));

        tempBoxPnts.resize(0);
        tempBoxPnts << ptP << ptZ << ptH << ptY << ptV << ptC << tempVertList[6] << ptF;
        uppTorsoPtr->upTorsoBoxTree->subBoxes.append(new Box(tempBoxPnts));

        tempBoxPnts.resize(0);
        tempBoxPnts << ptW << ptI << ptZ << ptP << ptJ << tempVertList[5] << ptC << ptV;
        uppTorsoPtr->upTorsoBoxTree->subBoxes.append(new Box(tempBoxPnts));

    }

    // Now we need to scale the sub-boxes slightly smaller
    // *** IMPROVEMENT NEEDED: the boxes are still attached together. So a solution is needed to seperate them apart
    for (int i=0; i<uppTorsoPtr->upTorsoBoxTree->subBoxes.length(); i++)
    {
        GeometryAlgorithms::scaleModel(uppTorsoPtr->upTorsoBoxTree->subBoxes[i], 0.75f, 0.75f, 0.75f); // scaling factor can depend on the grammar or modelling algorithm
    }

    // **** LEVEL 2 - Torso Modelling ****
    // implementation of the grammars and rule sets





    return uppTorsoPtr;
}





/*  Algorithms for Torso Modelling
 *
 * ALGORITHM 1: for Grid Style torso (unmirrored version)
 *
 *
 *
 *
 *
 *
 * ALGORITHM 2: for layers/stack style torso (mirrored version)
 *
 *
*/
