#ifndef BACKBONE_H
#define BACKBONE_H

#include "bodypart.h"



class BackBone : public BodyPart
{
public:
    BackBone();
    ~BackBone();

    BodyPart *duplicateBP(Alien *parentAlienPtr, QMap<int, bool> &tempClonedBP_Checklist);

    // A data structure that stores all the BPs that are attached to the joints of this backbone along with their originating positions
    // Here int is the Joint ID, and the corresponding QMap contains the list of attached BPs
    // For the inner QMap, the int is the BP_ID to be attached. QVector3D is the poistion where the BP will be attached
    // TODO(ismail): This should be a member of BodyPart so that all inherited classes can have it, as it can be applicable to all BPs
    QMap<int, QMap<int, QVector3D>> attachmentBPsWithPositions;
};

#endif // BACKBONE_H
