#ifndef BODYPART_H
#define BODYPART_H

#include "cell.h"

class Alien; // POTENTIAL PROBLEM!! try not to have an alien pointer inside BodyPart...Inspect this

// This class represents the body part of the alien such as arms, pelvis, fins, head, and more granular body parts such as
//      muscles (deltoid, Tensor etc). Each bodypart is made up of cells

class BodyPart
{
public:
    // TODO(ismail): Must have a copy constructor
    // ... and copy assignment operator
    BodyPart();
    virtual ~BodyPart();
    void makeCell(QVector3D cellLocation, GLfloat cellEnergy);
    // addSubBodyPart()
    void computeBoundingBox();
    void makeVertexList(); // TODO(ismail): Perhaps not needed
    QVector3D getPivotPoint();
    QVector3D getPivotVector();
    BodyPart *getSubBP_Ptr(int BP_ID);
    void duplicateAllCommonMembers(BodyPart *clonedBP, Alien *parentAlienPtr, QMap<int, bool> &tempClonedBP_Checklist);
    void getBodyPartCellList(QVector<Cell*> &tempCellList);
    void visitCellsRecursively(Cell *rootCell, QVector<Cell*> &tempCellList); // TODO(ismail): Perhaps make it private
    void updateTranslationMatrixForAllSubBPs(QMatrix4x4 BP_TranslateMatrix);
    void updateRotQuatForAllSubBPs(QQuaternion BP_OrientationQuat);
    // TODO: Need to make it more versatile so this method accepts input cells
    // This cell should only be called on a BONE
    bool deleteAllBP_Cells(bool leaveFirCell);

    virtual BodyPart* duplicateBP(Alien *parentAlienPtr, QMap<int, bool> &tempClonedBP_Checklist);

    // members
    Alien *parentAlienPtr; // PROBLEM!! try not to have an alien pointer inside BodyPart...Inspect this
    int parentAlienID; // this is fine

//    QString BP_Name;
    int BP_Type; // an integer value specifying the type of the BP. All the different variations of a particular BP will
                 // ... share the same BP_Type
    int BP_ID; // consider using uint16_t or unsigned short
    Box *boundingBox; // a bounding volume algorithm would be good such as a convex hull
                        // TODO(ismail): Implement this and initialise it to something default.
    int level; // TODO(ismail): PERHAPS NOT NEEDED. It is perhaps the level of the BP. Level_0, Level_1 etc
    // TODO(ismail): Suggest making the cloneFrom a Pair<Alien_ID the source BP belongs to, source BP ID>
    int clonedFrom; // the BodyPart it was cloned/duplicated from. PERHAPS NOT NEEDED
    QPair<int,int> cloneFromAlienAndBP_Pair; // the Alien and BodyPart it was cloned/duplicated from
    bool isSymmetric;
    int mirrorOf; // BP_ID of the BP it was mirrored from. This can be useful for some algorithms
    quint8 smoothnessLevel;
//    bool isSmoothingBackup; // If this BP's clone is a smoothing backup.

//    int cellCount;
    QVector<Cell*> cellList; // list of sub cells. If a body part is made out of several cells, the first cell should be the
                            // ... be the leader/root cell. This kind of topology might be important for a variety of alien muscles and tissues
                            // NOTE: Only Joints and Bones are made out of cells
                            // NOTE: Any BP other than Joint and Bone will have its native cellList empty. This is because they are made out of subBPs (Joints, Bones, etc) and
                            //       ... so they don't have a native cellList
    QVector<BodyPart*> subBodyParts; // list of sub body part pointers. These could be joints and bones and other BPs etc
                                     // A backbone also has subBPs other than joints and bones. These are the BPs that are attached
                                     // ... to it.
                                     // Any BP that is attached to another BP becomes its subBP, and will be included in this list.

    QVector<int> subExtremityBPsIDs;    // A list to store the IDs of the sub extem BP IDs that are
                                        // ... attached to this BP

    // TODO (ismail): Perhaps the below quaternions/matrices are to be used for the Bounding Box/Volume for this BP
    QQuaternion BP_OrientationQuat; // used to keep a track of the rotations
    QMatrix4x4 BP_TranslateMatrix; // used to keep a track of the translations
    QMatrix4x4 BP_ScalingMatrix;
    // TODO(ismail): perhaps have a scaling matrix OR scaling factor as well.

    QVector3D BP_PivotPt; // A 3D point that serves as the pivot point for rotations, scalings and translations
                          // A pivot point can also be used as the location/originating/central point
    // a list of all faces of this BP (to be used for placeholders) - for advanced algorithms
    // parentBodyPartPointer
    // TODO(ismail): For joints and bones, this should be given a value of -1 by default to indicate that there is no root joint for joints and bones
    int rootJntID;
    QVector3D BP_PivotVector;   // A 'Pivot Vector' is a vector that is the orientation vector of a box. It is computed as:
                                // ... center point of Front Face of root joint minus center point of back Face of root joint
                                // ... This vector must be updated whenever this BP or its parent BP undergoes rotation
    QVector<BodyPart*> grownBodyParts;  //  other BPs grown from this BP in a later stage. These can be skeletal,
                                        //  ... as well as muscular
    // THis DS stores all the boundary cells/densers that are generated when more stacked densers formations are created
    // ... on the base densers. When a formation such as 'crater' or 'protrusion' is created, the boundry densers
    // ... are stored in this list. This helps to identify a new stacked denser formation is not intruding/overlapping
    // ... another existing formation.
    QVector<Cell*> boundaryDnsrs;

    // TODO(ismail): Every skeletal BP should have a starting joint and an ending joint. This information will be useful when attaching BPs to other BPs and also to grow additional BPs
    // ... from other BPs. Solution: We can find that from the ordered Bones list
};

#endif // BODYPART_H

// ** Possible members **
// Category/Type (This defines the main type of the body part...Could be any of arm, pelvis, head etc...might be useful for DBMS)
// ID (from 0 ... N, as there can be more than 1 head of an alien creature)
// Level (either 0,1,2, or 3, where 0 being the most basic level such as femur bone)
// Budding Face List (list of faces which serve as the origin of this body part)
// Budding Box List (list of boxes which serve as the origin of this body part)
// parent body part (a pointer to the parent body part. For torso it will be NULL)
// SubBodyParts (List of sub/children body parts originating from this main body part.
//      for example: lower leg can be parented to the overall leg or upper leg, or even front torso!)
//      Horns/Thorns can be parented to head body part of upper arm body part. And there can be many combinations
// Cells (List of cells constructing this body part)
// Base Growth Formation Geometry (A data structure...this stores a root pointer to tree of cell boxes that defines the actual shape of the base growth formations)
// Further Growth Formation Geometry (A data structure...this stores a tree of cell boxes that defines the actual shape of the further growth formations)
// Pivot point (A central pivot point which can be used for rotations or scaling. This point can be changed)
// bounding box (an imaginary/nonrenderable box that bounds this entire body part, including its sub body parts.
//               Used for several purposes such as location of the body part. It will keep on updating as the body part grows/mutates)
// VertexList (A list of vertices making up all the subparts. This can be computed only when needed (temporarily) to save memory
//            This can be used for transformation purposes, and other purposes...)
// CellCount (can be useful for some algorithms and perhaps optimizations)
