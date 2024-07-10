#ifndef HEADSUBBP_MAXILLA_H
#define HEADSUBBP_MAXILLA_H

#include "bodypart.h"

//#include <QMap>
//#include <QPolygon>

class HeadSubBP_Maxilla : public BodyPart
{
public:
    HeadSubBP_Maxilla();
    ~HeadSubBP_Maxilla();

    BodyPart *duplicateBP(Alien *parentAlienPtr, QMap<int, bool> &tempClonedBP_Checklist) override;

//    int headSubBP_ID;
//    int subBP_type;

//    // This map includes the IDs of all the parallel lineages of the sub BP
//    // A parallel lineage is like a body part (it is actually derived from the BodyPart class)
//    // so it has all the properties of a normal BodyPart
//    // For example: The Front Mandible has three parallel lineages. Each lineage will be assigned an ID by the system
//    // ... and then which will be stored in this list
//    QVector<int> parallelLineagesList;
};

#endif // HEADSUBBP_H
