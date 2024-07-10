#ifndef ALIEN_H
#define ALIEN_H

#include "cell.h"
#include "bodypart.h"
#include "extremitybp.h"
#include "ruleset.h"
#include "bone.h"
#include "joint.h"
#include "skeleton.h"
#include "alien_dna.h"
#include "lowerlimb.h"
#include "upperlimb.h"
#include "backbone.h"
#include "tail.h"
#include "head.h"
#include "rib.h"
#include "headsubbp_maxilla.h"
#include "headsubbp_mandible.h"
#include "headsubbp_zygomatic.h"


class Alien
{
public:
    // TODO(ismail): Must have a copy constructor
    // ... and copy assignment operator
    Alien();
    ~Alien();
    void makeAllRuleSets();
    void getBodyPartListCellListForRendering(QVector<Cell*> &tempCellList); // method to return geometric primitive arrays.
                                                                    // tempCellList is provided by glwidget class.
    void visitCellsRecursively(Cell *rootCell, QVector<Cell*> &tempCellList);
    void getBonesListForRendering();
    // get the list of all BP IDs excluding the Joints and Bones
    void getAllBPsID_List(QVector<int> &allBPsID_List);
    void initAlienColorMap();
    // Perhaps not needed
    void setAlienColorMap(int bpType1, int color1, int bpType2, int color2, int bpType3, int color3);
    float getZScalingFactorFromBulkinessDNA(BodyPart *bpPtr);
    void mapExtremToParentBP(int extremID, int parentBP_ID);

    //*** Member Variables ***
    int alienID;
    int BP_ID_Counter;
    int clonedFrom; // ID of the Alien that this alien was originally cloned from
    BodyPart *rootBP;
    QVector<BodyPart*> bodyParts; // Body Part List ... // TODO(ismail): Better use a QMap for BP list
    QVector<BodyPart*> cloneBPs; // Body Parts that are cloned from other BPs. Basically whatever is in this list is also in the bodyParts list
    QVector<BodyPart*> seperateCloneBPs;    // Some clones are created to be displayed in GLW panels for the user to experiment. These are not really part of the main alien skeleton
                                            // ... unless they are attached to the BackBone. These BPs are not present in the bodyParts list
    QMap<int, BodyPart*> smoothingBackupCloneBPs;  // Body Parts that are cloned from other BPs when smoothing is performed
                                            // This list is created for backup purposes, so when a BP undergoes smoothing and then
                                            // its JAT need to be changed, it is selected from this list
                                            // key(int) is the source BP that is to be cloned. value(BP*) is the new clone of the source BP    
    QVector<Bone*> alienBones; // Bone List ... // TODO(ismail): Better use a QMap for bone list
    QVector<Joint*> alienJoints; // Joint List ... // TODO(ismail): Better use a QMap for joints list
    // Try removing the following BP lists, as they may only be contributing to memory leaks
    QVector<Rib*> alienRibs; // Ribs List ... // TODO(ismail): Better use a QMap for LowerLimb list
    QVector<LowerLimb*> alienLowerLimbs; // LowerLimb List ... // TODO(ismail): Better use a QMap for LowerLimb list
    QVector<UpperLimb*> alienUpperLimbs; // UpperLimb List ... // TODO(ismail): Better use a QMap for UpperLimb list
    QVector<BackBone*> alienBackBones; // BackBone List ... // TODO(ismail): Better use a QMap for BackBone list
    QVector<Tail*> alienTails; // Tail List ... // TODO(ismail): Better use a QMap for Tail list
    QVector<Head*> alienHeads; // Head List ... // TODO(ismail): Better use a QMap for Head list
    QVector<HeadSubBP_Maxilla*> alienHeadSubBP_Maxillas; // Maxilla List ... // TODO(ismail): Better use a QMap for Head list
    QVector<HeadSubBP_Mandible*> alienHeadSubBP_Mandibles; // Mandible List ... // TODO(ismail): Better use a QMap for Head list
    QVector<HeadSubBP_Zygomatic*> alienHeadSubBP_Zygomatics; // Zygomatic List ... // TODO(ismail): Better use a QMap for Head list

    // This list contains a list of all extremities this Alien has. It is important so keep it
    QVector<ExtremityBP*> alienExtremities;
    // This map forms a link b/w the extremity BP and its parent BP
    // key(int) is the extremity ID. Value is the parent BP ID
    QMap<int,int> extremToParentBP_Map;

    // TODO(ismail): Perhaps need to have a list of ridges as well
    // TODO(ismail): Perhaps make the skeleton dynamic
    Skeleton alienSkeleton;
    QVector<RuleSet> ruleSetList; // a set of ruleset for each BodyPart
    // TODO(ismail): alien ID == > a unique ID
    // Here are the Type IDs for all the Aliens
    // ...  BIPEDAL=0, QUADPEDAL=1, Aquatic=2, Amphibious=3, Octapedal=4, Anthropodal=5
    int Alien_Type_ID;
    Alien_DNA *alienDNA;

    // This DS stores a BP ID which is then replaced by another BP after all the BPs are removed and remodelled.
    // This DS acts as a bookkeeping DS that comes in handy to identify which legacy(deleted) BP has been replaced with which new BP
    // THis can be useful when we are modifying the number of joints of a BP and any other mutations of clones which involve removing all the BPs and then
    // ... remodelling the entire Skeleton
    QMap<int,int> BP_ReplacementMap; // TODO(ismail): Perhaps NOT NEEDED. need to remove this as it is not being REALLY used

    // This DS assigns a unique color to the BP type
    // key(int) is the BP_Type_ID. Value is the color taken from the enum in ALienModeller
    QMap<int, int> BP_TypeColorMap_2;

    bool alienJustLoadedFlag=false;

    //
    // +++ SOME TEMPORARY MEMBERS THAT WILL BE REMOVED LATER
    //
    // Each vector in this list represent a bone
    QVector< QPair<QVector3D, QVector3D> > tempVectorsListForRendering;

/*
 * Flags list (Bulkiness DNA)
 *
 *  clone DNA
 *  mutate DNA
 *  delete BP entries
 *  create new BP entries
 *
*/
};

#endif // ALIEN_H
