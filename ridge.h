#ifndef RIDGE_H
#define RIDGE_H

#include "bodypart.h"

/*
 * A Ridge is a feature that can be grown over a BP such as a Bone, muscle, head, other features
 *
*/

// TODO(ismail): Think about whether a Ridge be derived from a BodyPart or not. Should be a standalone thing, not containing
//               ... all the members of BodyPart
class Ridge : public BodyPart
{
public:
    Ridge();
    ~Ridge();

    BodyPart *duplicateBP(Alien *parentAlienPtr, QMap<int, bool> &tempClonedBP_Checklist) override;

    // **** Member Variables ***

    // Series of lines making up the ridge
    // This list contain a pair of points, where each point is the end point of the lines/vectors making up the ridge
    // Each line will be on a single face
    QVector<QPair<QVector3D,QVector3D>> ridgeLinesList;

    // List of Faces covered by this ridge

    // Protrusion trend of this ridge (This could be an inner class defining the different protrusion aesthetics
    // ... of this list)


    // *** Member Methods ***
};

#endif // RIDGE_H
