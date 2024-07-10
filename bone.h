#ifndef BONE_H
#define BONE_H

#include "bodypart.h"
#include <QMap>


class Bone : public BodyPart
{
public:
    Bone();
    ~Bone();

    BodyPart *duplicateBP(Alien *parentAlienPtr, QMap<int, bool> &tempClonedBP_Checklist);
    QVector<Cell*> getAllLeafDensers();
    Cell* getLeafDenser();
    // This will bevel all the cells (including densers), and subdivCells of this bone
    void bevelBone();

//    bool deleteCells(QVector<Cell*> inputCells);

    int boneID; // TODO(ismail): not really required

    /* ---- Bone Specs ----
     * NumOfPnt = Number of Branches Points
     * CurvAmnt = amount of curvature. Calculated as difference b/w previous and current bone angle
     * LvlOfGrth = levels/steps of growth
     * NumOfParBones = Number of parallel bones
     * SquashinessOrStretchinessOfBones
     * HasMirroredBranches?
    */
//    QVector<int> boneSpecs; // TODO(ismail): perhaps not needed. generated from boneSpecsBrackets (present inside Bone_RuleSet)

    // a pair of values specifying the joints IDs this bone is connecting. Can become useful in some problems
    QPair<int, int> endJoints; // a pair of joint IDs
    QVector<int> allAttachedBP_IDs; // all the BPs attached to this bone

    // This list stores all the different ridge specs for global properties of the ridges. At the moment we have two global properties
    // ... 0 - Number of ridges. 1 - regular/irregular alignment b/w ridges
    QVector<int> boneRidgeSpecs_global;

    // This DS contains the specs for all the ridges appearing on this bone. The 'key' is the ridge ID (BP_ID),
    // ... while QPolygon is used as: series of QPoint(int, int), where x=type of ridge spec (starting from number 0)
    // ... and y=value of that ridge spec
    QMap<int, QPolygon*> boneRidgeSpecs_local;
    // This list stores all the stacked denser lineages that have grown from the bone.
    QVector<Cell*> allStackedDenserLineages; // DS_CHECK - a check to inform us:
                                                                //  - The size of the DS
                                                                //  - The contents of the DS
    QVector<Cell*> subdivisionCells;    // This DS is populated when the BP is bulgified. This will most probably be used for book keeping
    QVector<Cell*> nonRenderableCells;  // PERHAPS NOT NEEDED
    Cell* rootCell;
//    int ContainingBP_ID;
};

#endif // BONE_H
