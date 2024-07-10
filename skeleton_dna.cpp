#include "skeleton_dna.h"

Skeleton_DNA::Skeleton_DNA()
{
    // TODO(ismail): perhaps have default values for the members
    bulkinessDNA_ManipulationFlags.fill(false,5);
    //    bulkinessDNA_ManipulationFlags[3] = true;
}

Skeleton_DNA::~Skeleton_DNA()
{
    qDebug() << "deleting Sketon DNA";
    numOfSkeletalBodyParts_DNA.clear();
    numOfJointsPerBP_DNA.clear();
    BP_AttachmentToBackBone_ZoneAndScattering_DNA.clear();
    BP_SizeComparedToBackBone_DNA.clear();
    BP_AngleArrangementsFromBackBone_DNA.clear();
    OtherBPsToGrow_DNA.clear();
    Skeletal_BP_Bulkiness_DNA_ForEachBP.clear();
    bulkinessDNA_ManipulationFlags.clear();
    BP_listToDeleteFromBulkinessDNA.clear();
    BP_listToMutateInBulkinessDNA.clear();
    BoneBending_DNA.clear();
    BP_SpecificDensificationDNA.clear();
    BP_AttachmentToOtherBP_withAttachPositionAndVector_DNA.clear();
    qDebug() << "Sketon DNA deleted";
}

void Skeleton_DNA::addOrUpdateNewEntryIn__ChildBP_ToParentBP_Attachment_DNA( int childID, int parentID,
                                                                            int boneIndPos, int faceID,
                                                                            int uParam, int vParam,
                                                                            int pitch, int yaw)
{
    QPolygon tempPoly; tempPoly.clear();
    tempPoly << QPoint(boneIndPos,faceID) << QPoint(uParam,vParam) << QPoint(pitch,yaw);
    ChildBP_ToParentBP_Attachment_DNA.insert(QPair<int,int>(childID,parentID), tempPoly);
}

void Skeleton_DNA::initializeExtremityBP_StateMachine()
{
    // Set up the state machine
    QPolygon tempPoly; tempPoly.clear();
    tempPoly << QPoint(0,1) << QPoint(3,0);
    extremityBP_StateMachine.insert(0, tempPoly);

    tempPoly.clear();
    tempPoly << QPoint(1,0) << QPoint(2,3) << QPoint(4,4);
    extremityBP_StateMachine.insert(1, tempPoly);

    tempPoly.clear();
    tempPoly << QPoint(2,4) << QPoint(0,1);
    extremityBP_StateMachine.insert(2, tempPoly);

    tempPoly.clear();
    tempPoly << QPoint(3,1);
    extremityBP_StateMachine.insert(3, tempPoly);

    tempPoly.clear();
    tempPoly << QPoint(4,0) << QPoint(3,1) << QPoint(2,1);
    extremityBP_StateMachine.insert(4, tempPoly);

    tempPoly.clear();
}

void Skeleton_DNA::initializeExtremityBP_ScultpingStateMachine()
{
    // Set up the state machine for Sculpting Extremity BP
    QPolygon tempPoly; tempPoly.clear();
    tempPoly << QPoint(0,1) << QPoint(3,0);
    extremityBP_ScultpingStateMachine.insert(0, tempPoly);

    tempPoly.clear();
    tempPoly << QPoint(0,1) << QPoint(2,0);
    extremityBP_ScultpingStateMachine.insert(1, tempPoly);

    tempPoly.clear();
    tempPoly << QPoint(0,3);
    extremityBP_ScultpingStateMachine.insert(2, tempPoly);

    tempPoly.clear();
    tempPoly << QPoint(0,1) << QPoint(3,0);
    extremityBP_ScultpingStateMachine.insert(3, tempPoly);

    tempPoly.clear();
}

void Skeleton_DNA::initializeheadStateMachine()
{
    // Set up the state machine
    QPolygon tempPoly; tempPoly.clear();

    tempPoly << QPoint(1,2) << QPoint(4,5) << QPoint(6,7) << QPoint(9,10);
    headStateMachine.insert(0, tempPoly);

    for (int state=1; state<=10; state++)
    {
        tempPoly.clear();
        for (int i=1; i<=10; i+=2)
        {
            tempPoly << QPoint(i,i+1);
        }
        headStateMachine.insert(state, tempPoly);
    }

    tempPoly.clear();
}
