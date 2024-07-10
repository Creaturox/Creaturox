#ifndef ALIENMODELLER_H
#define ALIENMODELLER_H

#include "alien.h"
//#include "bone.h"
#include "bone_ruleset.h"
//#include "joint.h"
#include "joint_ruleset.h"
#include "jointsarrangementtrend.h"
#include "level_2_feature.h"
#include "ridge_ruleset.h"
#include "skeleton_ruleset.h"

#include <torsomodeller.h>
#include <uppertorso.h>
#include <qopengl.h>
#include <geometryalgorithms.h>
#include "level_1_2_featureplaceholderspecs.h"
#include <QPair>
#include <QColor>
#include <algorithm>
#include <math.h>
#include <QFile>
#include <QDataStream>

/*
 * This class is responsible for modelling the entire alien part by part. It might house TorsoModeller, LegsModeller,
 * ArmsModeller, AbnormalFeatureModeller, HeadModeller, FeatureRandomizer etc
 *
*/

/*
 * Making the code more professional. Find opportunities for:
 *      better randomization classes using std random generators
 *      using std::algorithms for other tasks
 *      using iterators to traverse through vectors and lists
 *      using smart pointers
 *      move and copy semantics
 *      lambda expressions
 *      Generic Programming : template classes and methods
 *      Design Patterns
 *      Refactoring (THIS CLASS IS GETTING VERY BIG). Perhaps break it into 5 to 7 composite classes
*/

class AlienModeller
{
public:
    AlienModeller();
    ~AlienModeller();

    //  This method initializes all the things before modelling the alien, including:
    //  initialize Skeleton_Ruleset
    //  initialize Bone_Ruleset
    //  initialize Joint_Ruleset
    //  initialize any other rulesets
    //  BP growth/generation state machines + random forests + algorithms
//    void initializeAlienModeller(); // TODO(ismail): Might need to deprecate as NOT NEEDED

    void modelNewAlien();

    void modelNewAlien_TypeGuided(QString alienTypeStr, QMap<int, QPair<int,int>> bpAmntsMap);
    Alien *getAlienFromID(int alienID);
    void specifyBP_TypeColorMapForAlien(Alien *alienPtr);
    void initAlienTypeMap();
    void initBP_TypeMap();

    // The bones and joints of the BP are deleted inside the BP's destructor
    void removeAlienBodyParts(Alien *alienPtr, QVector<int> &BP_ID_List);
    void deleteAliens(QVector<int> aliensToDelete_ID_List);

    BodyPart* replaceBP_WithAnotherBP(Alien *alienPtr, BodyPart *origBP_Ptr, BodyPart *inBP_Ptr);
    QPair<int,int> applySeperateBPChangesToOrigBPInAlien(Alien *alienPtr, BodyPart *sepBP_Ptr, BodyPart *origBP_Ptr);

    // +++ RULESETS +++
    // This method calls initializeSkeletonRulesets, initializeMuscleRuleset, and initializeLevel_2_Feature_Rulesets to initialize all the rulesets
    // It might also initialize other rulesets as well.
    void initializeAllRulesets();
    void initializeSkeletonRuleset();


    // +++ ALIEN DNA +++
    void buildCompleteDNA_FromRulesets(Alien *alien, QMap<int, QPair<int, int> > bpAmntsMap);
    void buildSkeletonDNA(Alien *alien, QMap<int, QPair<int, int> > bpAmntsMap);
    void addNewEntryforNumOfJointsPerBP_DNA(Alien *alien, int BP_Type, QPolygon tempPoly, int BP_ID);
    void addNewEntryforBP_JAT_rootJoint_DNA(Alien *alien, int BP_Type, int numOfEntriesToAdd);
    void addEntryForBP_AttachmentToBackBone_ZoneAndScattering_DNA(Alien *alien, int BP_TypeID, int alienTypeID);

    // *** Modify the Skeletal_BP_Bulkiness_2_DNA for provided BPs - can be done after cloning an alien's DNA ***
    // This method is called whenever we need to modify the bulkiness of a BP. If the entry for the BP being bulked out is already present in the
    // ... DNA, we simply remove it and add an new one. If it doesn't exist, we add one
    // The BP_ID_List specifies for each BP whether its bulkiness DNA need to be mutated or not.
    void buildSkeletalBP_BulkinessDNA(Alien *clonedAlien, QVector<int> &allBP_ID_List);
    // BP's Bones SubDiv Cells DNA - Creation and Mutation
    void buildBoneSubDivCellsDNA_forGivenBP(Alien *alienPtr, BodyPart *bpPtr, QVector<Bone*> boneList);
    void buildBoneSubDivCellsDNA_forAllBPs(Alien *alienPtr);
//    void buildBoneSubDivCellsBulkinessDNA(Alien *alienPtr, BodyPart *bpPtr, Bone *bonePtr);
    // modifyingPoly should be 8 pairs long. This will replace the existing bone's QPoly
    void modifyBoneSubDivCellsDNA(Alien *alienPtr, BodyPart *bpPtr, QVector<Bone*> boneList, QPolygonF modifyingPoly);
//    void modifyBoneSubDivCellsBulkinessDNA(Alien *alienPtr, BodyPart *bpPtr, Bone *bonePtr);

    void increaseOrReduceBP_Bulkiness(Alien *alienPtr, int BP_ID, int increaseOrReduceOverallBulkiness, float amountOfScaleChange);
    void changeBP_BulkinessTrend(Alien *alienPtr, int BP_ID, float trendID);
    void changeBP_Bulkiness_XY_Scaling(Alien *alienPtr, int BP_ID, QPointF XY_ScalingFlags);
    void changeBP_BulkinessScalingValues(Alien *alienPtr, int BP_ID, float startScaleVal, float scalingMultiplier);
    void build_BP_DensificationDNA(Alien *alienPtr, QVector<int> &BP_ID_List);
    bool cloneDensificationDNA(Alien *srcAlienPtr, Alien *destAlienPtr, BodyPart *srcBP_Ptr, BodyPart *destBP_Ptr);
    void buildRebuildUpdate_BP_BulgificationDNA(Alien *alienPtr, int BP_ID, bool rebuildFromScratch=false);
    bool cloneBP_BulgificationDNA(Alien *srcAlienPtr, Alien *destAlienPtr, BodyPart *srcBP_Ptr, BodyPart *destBP_Ptr);
    void modify_BP_DensificationDNA(Alien *alienPtr, QVector<int> &BP_ID_List, int optionNum);
    BodyPart* squashOrStrechBP(Alien *alienPtr, int BP_ID, float squashOrStrechFlag, float squashOrStretchFactor);
    void adjustTotalLenghtsOfBPs(Alien *alienPtr, QVector<BodyPart *> &BP_PtrList);
    // This method might be only for testing. Might need to remove it. TODO(ismail): Identify whther it needs removing
    void initSkeletalBP_BulkinessDNA(Alien *alien);
    void updateBoneBendingDNA(Alien *alienPtr);
    void modifySkeletalBP_BulkinessDNA(Alien *alien, QVector<int> &BP_ID_List_toMutate, int incDecBP_Bulkiness=-1,
                                       int sqwashOrStretchBP=-1, float changeBulkTrend=-1, QPointF XY_ScalingFlags = QPointF(-1,-1),
                                       QPointF scaleValues = QPointF(-1,-1));
    void modifyNumOfSkeletalBPsDNA(Alien *alien, int BP_Type);

    // Also have parameters for whether to increase of decrease the joints and how much depending upon the options selected by the user
    // This method modifies the energies and angles of the BP joints, while retaining the JAT but the final appearance doesnot
    // ... stays faithful to the original BP appearance.
    // TODO(ismail): Need a better method
    void modifyNumOfJointsPerBP_DNA(Alien *alien, QVector<int> &BP_ID_List);

    void incDecNumOfBP(Alien *alien, int BP_Type, bool incOrDec, ushort changeNum, QVector<int> &newBP_IDs, BodyPart *bpToRemove=nullptr);

    // This method always appends new joints to the end of the BP lineage
    // If numOfJntsToIncDec > 0, then increase joints, else if numOfJntsToIncDec < 0, then decrease joints from the end of the BP lineage
    // changeJAT will tell the algo to change the JAT for the newly added joints
    // TODO(ismail): When a BP is extended or its Joints decreased, its bulkiness DNA must also be updated
    // TODO(ismail): We are effetively adding/removing bones to/from the end of the BP lineage.
    // ... Perhaps make a new method that does for the bones.
    BodyPart *incDecNumOfJointsForBP(Alien *alienPtr, BodyPart *inBP_Ptr, int numOfJntsToIncDec, bool changeJAT_forNewJoints);
    // for p1: (change energy trend (T/F), change starting energy (T/F)). For p2: (change angle trend (T/F), change starting angle (T/F))
    void modifyBP_JAT_rootJoint_DNA(Alien *alienPtr, QVector<int> &BP_ID_List,  QPair<int,int> &BP_MirrorPair, QPair<bool,bool> p1, QPair<bool,bool> p2,
                                    QPointF energyVals = QPointF(-1,-1), QPointF angleVals = QPointF(-1,-1));
    void manipulateSkeletalBulkinessDNA_Flags(Alien *alienPtr, bool resetAllEntires, bool cloneBulkDNA, bool mutateBP_Entries,
                                       bool deleteBP_Entries, bool createBP_Entries,
                                       QVector<int> *BP_listToDeleteFromBulkinessDNA, QVector<int> *BP_listToMutateInBulkinessDNA,
                                       bool updateBP_Entries=false, QVector<int> *BP_listToUpdate = new QVector<int>(0));

    void updateBP_AttachmentToOtherBP_withAttachPositionAndVector_DNA(Alien *alien, int bpToRemove, int parentBP, int childBP, QVector3D pos, QVector3D vec);

    void buildHeadStretchingDNA(Alien *alienPtr, int headID);


    // +++ ALIEN SKELETON +++
    // This method does not do bulkification
    BodyPart *modelNewBP_TypeGuided(Alien *alien, int BP_Type);

    // This method really uses the rulesets to initialize the alien skeleton including its shape
    // ... (number of joints, bones, joint arrangements, all bone rulesets)
    // ... It also updates(if needed) the skeleton_ruleset, bone_ruleset, and joint_ruleset
    void initializeAlienSkeleton(Alien *alien);

    // This method uses tempJntList_1 and tempJntList_2 to pair up joints and store their pairs in the Jnt2JntRelations DS
    // ... TODO (ismail): Perhaps need to change this function's name to createJnt2JntRelations.
    void createSkeletonSpecs(Alien *alien);

    void generateAlienSkeleton(Alien *alien);
    // growAlienSkeleton() // might be an important method to grow the skeleton further

    // Initialize joints arrangement trends for each joint for the currently modelled alien
    // Joint Arrangement Trends mainly pertain to specifying the distance and angle at which a joint can be placed from its parent
    // The specifications is defined inside the class JointsArrangementTrend
    void setUpJointArrangementTrend(Alien *alien, int randRootJnt);

    // This method also computes all the joints' positions, along with computing how the joints are arranged
    void computeJointArrangements(Alien *alien);

    BodyPart *mirrorBP_AroundPlane(Alien *alienPtr, BodyPart *bpPtr, QVector3D v1, QVector3D v2, QVector3D v3, QVector3D norm);
    // This method  creates a reflection clone of the BP on the face that is opposite to the face that it is attached to
    BodyPart *symmetrifyBP_AroundOppositeAttachmentFace(Alien *alienPtr, BodyPart *bpPtr);
    int getOppositeFaceID_OfInputFaceOfBox(int faceID);

    void symmetrifyBP_AroundOppositeAttachmentFace_Algo2(Alien *alienPtr, BodyPart *bpPtr);

    // mirror All BPs except BackBone.
    // Mirroring only happens around BackBone
    void symmetrifyAlienBodyParts(Alien *alienPtr);

//    // mirror All BPs around their parent BP - Algo2
//    void symmetrifyAlienBodyParts_Algo2(Alien *alienPtr);

    // This method should be named extendAndSmoothAlienSkeleton. Also, this method should call other utility mehtods...extendAlienSkeleton, and smoothAlienSkeleton
    // mainly my own developed algorithm is used here but could use Dennis Zorin's algorithm
    // TODO(ismail): Tweak this method to smooth out an extract of a BP, and call it smoothSkeletonExtract()
    void smoothAlienSkeleton(Alien *alien, int skeletonSmoothFactor);

    // This method smoothens an extract of a given skeletal BP
    // Here numBoneFromEnd refers to the number of bones to be smoothened
    BodyPart *smoothenSkeletalBP(Alien *alien, BodyPart *BP_Ptr, quint8 smoothnessAmnt=1);

    void smoothenSkeletalBP_Algo2(Alien *alien, BodyPart *BP_Ptr, QPolygon *Jnt2JntRelExtract, quint8 smoothnessAmnt=1);

    // TODO(ismail): Need to have a method that can smooth out a joints lineage list
//    smoothJntsLineageList(QPolygon &Jnt2JntRelExtract, quint8 smoothnessAmnt=1);

//    // This method will smooth an extract of the BP from the Alien Skeleton
//    // TODO(ismail): Investigate the issues with a 2D vector (jntLineageList) here. Check the algorithm and sort the issues. Perhaps only a 1D vector is needed here
//    void smoothLineageFromSkeleton(QPolygon *Jnt2JntRelPtr, quint8 smoothnessAmnt=0);

    void instantiateSkeletalBPsFromDNA_TypeGuided(Alien *alien);

    void instantiateNumOfJointsForAllBPs_TypeGuided(Alien *alien, QMap<int, QPolygon> &numOfJointsPerBP_DNA);

    // This method creates the Jnt2JntRelations_TypeGuided DS
    void specifyJnt2JntRelations_TypeGuided(Alien *alien, QVector<BodyPart*> &bodyParts, bool clearOldJnt2JntRelDS = true);
    void addMoreJointsToJnt2JntRelations_TypeGuided(Alien *alien, BodyPart *bpPtr, QVector<int> jntID_List);

    void initializeJointsArrangementTrendForCurrentAlien(Alien *alien, QVector<Joint*> &jointsList, bool clearOldVectorEnergyAngleTrends_TypeGuided = true,
                                                         QPointF energyPt = QPointF(-1,-1), QPointF anglePt = QPointF(-1,-1));

    // This computes VectorEnergyAngleTrends_TypeGuided and tempVectorLocationCoupleForEachJoint_TypeGuided for each joint of the BP
    // The above Data Structures can then be used for modelling the joints
    void compute_JAT_AND_VecLocCplForEachJnt_ForBP(Alien *alien, int BP_ID, QMap<int, QPolygonF> &tempCopy_BP_JAT_rootJoint_DNA,
                                                   QMap<int,int> &tempParentJntCompletionCheckList, QMap<int, int> &tempChildJntPositionCheckList, float newBP_startingX_Loc);

    void cloneJointsArrangementTrendForCurrentAlien(JointsArrangementTrend &inputJAT);

    QPolygonF getEnergyAngleTrendForJoint(Alien *alien, int jntID);

    // This method is responsible for generating the entire skeleton (in seperate BPs) from the Skeleton DNA, when the user selects the type of alien
    void buildAlienSkeleton_TypeGuided(Alien *alien); // TODO(ismail): rename to buildAlienSkeletonFromDNA_TypeGuided()

    // This method finds different points on a given BP (specifically on the joints of the BP), where other BPs (UL, LL, N, H etc) will be attached.
    // It also computes the attachementVector using the DNA specs from BP_AngleArrangementsFromBackBone_DNA
    void findPointsAndVectorsOnBackBoneForBP_Attachments(Alien *alien, int Dest_BP_ID, int sourceBP_Type,
                                                           int pivotZoneID_FromDNA, float scatteringValue_FromDNA,
                                                           int distributionTrend_FromDNA, int numOfAttachmentPointsToSelect,
                                                           QVector<QVector3D> &attachmentPnts, QVector<QVector3D> &attachmentVectors /*QMap<int, QVector3D> &attachmentVectors*/,
                                                           QVector<int> &jntLocPairsList);

    void findPointsAndVectorsOnDestBPForSourceBPsAttachments(Alien *alien, int destBP_ID,
                                                             int pivotZoneID_FromDNA, float scatteringValue_FromDNA,
                                                             int groupingTrendFromDNA, int numOfAttachmentPointsToSelect,
                                                             QVector<QVector3D> &attachmentPnts, QVector<QVector3D> &attachmentVectors,
                                                             QVector<int> &boneLocPairsList,
                                                             QVector<int> faceList);    // if there is only one element specified by the user and it's -1,
                                                                                        // then it means all faces are to be considered. Otherwise the user
                                                                                        // needs to specify the faces in the faceList

    // This method attaches the BPs (UL, LL, N, H etc) to the back bone of the alien.
    // Make this method more general, so any source BP can be attached to the dest BP
    void attachAllSkeletalBodyPartsToBackBone(Alien *alien);
    void attachSeveralChildBPsToParentBP(Alien *alienPtr, int parentBP_ID,
                                         QVector<int> childBPsIDs, bool changeDNA_Entry=false,
                                         bool symmetrifyChildBP=false, bool shouldItRotate=false);

    // This method detaches all the sub BPs from a parent/given BP. If there are subBPs of the parent BP
    // ... These will be returned in the subBPsList.
    // This method is useful when we need to modify the parent BP or replace it with its mutated version
    void detachAllSubBPsFromParentBPs(Alien *alienPtr, BodyPart *parentBP_Ptr, QVector<BodyPart*> &subBPsList);
    // This method detaches all the mirrors too
    void detachAllSubBPsFromParentBP_Algo2(Alien *alienPtr, BodyPart *parentBP_Ptr,
                                           QVector<BodyPart*> &subBPsList, bool removeFromDNA);

    // When we remove bones from a BP, all the other BPs that are attached to this bone should
    // ... also be removed/deleted
    // THis can be specified in this method what to do with those attached BPs. Should they be removed
    // ... or should they be spared and attached on other (surviving) bones
    void removeBonesFromBP(Alien *aPtr, BodyPart *inBP_Ptr, QVector<int> bonesToRemove_IDs);

    // HYBRID ALIEN: This method prunes a BP. The incoming selectedBones list specified which bones to keep
    // ... and this method finds those bones that need to be removed.
    // ... Then this method calls removeBonesFromBP to extract a part of the incoming BP
    // THis method can be useful for creating Hybrid Aliens
    // This method may also be useful for cloning Aliens with accurate BP attachment orientations/rotations
    BodyPart* pruneBP(Alien *aPtr, BodyPart *bpPtr, QVector<int> selectedBones_IDs);

    // HYBRID ALIEN: This method is used to create a hybrid alien, such that a BP is extracted from a source alien and
    // ... attached to the destination alien
    // THis method can be useful for creating Hybrid Aliens
    void attachBP_FromSourceAlienToDestAlien(Alien *srcAlienPtr, Alien *destAlienPtr, BodyPart *sourceBP, BodyPart *destBP);

    // This method builds and updates the DNA DS for grouped BPs attachment
    void updateGroupBP_AttachmentToAnotherBP_ZoneScatteringTrend_DNA(Alien *alienPtr, int groupBPsTypeID, int destBP_ID, bool toAddOrNot, QPolygon inputDNA_Poly);

    // This method uses the DNA DS to identfy the number of group BPs to generate, based on the specs mentioned in the DNA
    // It uses GroupBP_AttachmentToAnotherBP_ZoneScatteringTrend_DNA
    QVector<int> generateAndAttachGroupBPsToAnotherBP(Alien *alienPtr, QMap<QPair<int,int>, QPolygon> &GroupBP_AttachmentToAnotherBP_ZoneScatteringTrend_DNA);

    // This method created clones of BP and attaches them at or around the same location where the original
    // ... BP was attached.
    // TODO(ismail): Cater for the case when the BP is a HEAD
    QPair<int,int> generateAndAttachClonedBPNearby(Alien *alienPtr, int BP_ID, uint8_t numOfClones);

    void removeGroupBPsFromParentBP(Alien alienPtr, int groupBPsTypeID); // NOT IMPLEMENTED

    // This method removes the joints entries of the BP from all the JAT DSs
    // This method also removes the joint's entry from the vectorLocationCoupleForEachJoint_TypeGuided DS in the alien's skeleton
    void removeAssociatedJATsAndOtherDSsForRemovableJnts(Alien *alienPtr, QVector<int> BP_JntsIDsList);

    // This method attaches a list of source BPs (UL, LL, N, H etc) to a destination BP using attachment DNA
    // TODO(ismail): Make it more flexible so it detaches BPs as well
    void attachBodyPartToAnotherBodyPart(Alien *alien, QVector<int> &sourceBP_ID_List, int destBP_ID, bool isItGroupBPsAttachment,
                                         QVector<QVector3D> attachmentPnts, QVector<QVector3D> attachmentVectors, QVector<int> jntLocPairsList, bool isItReAttmt=false);
    void attachHeadToParentBP(Alien *alienPtr, BodyPart *headPtr, BodyPart *parentBP);
    // A light weight method to attach a set of source BPs to destination BP.
    // This method does not use any DNA but uses the input parameters.
    // void attachBodyPartToAnotherBodyPart_LightWeight(Alien *alien, QVector<int> sourceBP_ID_List, int destBP_ID, int pivBoneInd, int scatterPerc,
    //                                                  QVector3D eulerAngles, QVector<int> faceList);

    // This method attaches a set of child BPs from their current location to another location
    void reattachChildBPsToParentBP_DiffLocations(Alien *alien, QVector<int> &childBP_ID_List, int parentBP_ID);

    // attachPntsParam: The value QPair<float,float> = QPair<x parameter, y parameter>
    // attachVecParam: The value QVector3D = (x parameter, y parameter, scaling factor for normal vector starting at the point)
    void computeAttachementParametersForChildBPs(Alien *alien, QVector<int> &childBPs_IDs, QMap<int, QPair<float,float>> &attachPntsParam, QVector3D eulerAngles/*QMap<int, QVector3D> &attachVecParam*/);

    // TODO(ismail): Implement a method called detachBodyPartFromAnotherBodyPart

    // void changeSkeletonBPsScalings()
    // void mutateSkeleton() // This method can do the following things:
                             // ... 1) Attach skeletons from other Aliens to this alien
                             // ... 2) Grow new mutations from the skeletal BPs

    // This method takes in a skeletal BP, and identifies those joints that correspond to
    void createBP_Zones(int BP_ID);

    // TODO: make it more flexible so it accepts a list of BP pointers and returns the list of their IDs
    void getAllBP_ID_List(Alien *alienPtr, QVector<int> &allBP_ID_List, QVector<int> &allSepClonedBP_ID_Listm);
    // This method gets us the list of BPs that were are already been displayed in the given GLW
    bool getAllRenderableBPs(Alien *alienPtr, QVector<BodyPart*> &allRenderableBPsList, int glwID);
    void getAllBPsOfGivenType(Alien *alienPtr, int BP_Tpe, QVector<BodyPart*> &similarBPs, bool includeMirrors=false);

    // *** HEAD Modelling ALGORITHMS ***
    void buildHeadSubBP_AlignmentDNA(Alien *alienPtr, int headID);
    BodyPart *modelNewHead(Alien *alienPtr);    // densers based
    BodyPart *modelNewHead_Algo2(Alien *alienPtr);  // lineage based
    BodyPart *modelNewHead_UsingStateMachine(Alien *alienPtr); // box based
    void stretchHead(Alien *alienPtr, BodyPart *headPtr);
    void modelMaxilla(Alien *alienPtr);
    void modelMandible(Alien *alienPtr);
    // This method uses the alignment DNA for head Sub BPs
    void alignHeadSubBPs(Alien *alienPtr, int headID); // PERHAPS NOT NEEDED
    void attachHeadSubBP_ToHead(Alien *alienPtr, int headID); // PERHAPS NOT NEEDED
    void buildCompleteHeadSubBP_DensificationDNA(Alien *alienPtr, int headID);
    void createHeadSubBP_DensificationDNA(Alien *alienPtr, int headID, BodyPart *subPtr);
    void densifyHeadSubBPs(Alien *alienPtr, int headID);
    void densifyBodyPart(Alien *alienPtr, BodyPart *bpPtr);
    // This method doesn't use any DNA, it just densifies the Head wildly
    // ... and 'tries' to give it a unique appearance
    // IT also densifies the sub Jnt2Jnt Lineages of the Head
    void densifyHead(Alien *alienPtr, BodyPart *headPtr);
    QVector<QPolygon> generateSubJntsLineage(Alien *alienPtr, int headID, QVector<Cell*> &cellList);
    void createDentInDensers(BodyPart *bpPtr, QVector<Bone*> &boneOrderedList, QPolygon dentSpecs);
    void createDenserProtrusions(BodyPart *bpPtr, QVector<Bone*> &boneOrderedList, QPolygonF protSpecs);
    // Crater Specs
    // QPolygonF:
    // P0   :   (%age of boneList length to select the pivot bone, make a curvy formation or not i.e. 0=do not make curve formation; 1=make a curve formation)
    // P1   :   (%age of bones on upper side of pivot bone, %age of bones on lower side of pivot bone)
    // P2   :   (%age of denser lineage length to select the pivot cell, %age of cells on either side of pivot cell...usually an odd number)
    // P3   :   (minimum num of densers stacked below the pivot denser, max num of densers stacked below the pivot denser)
    // P4   :   (%age of densers to reduce on either side of the pivit denser on each subsequent bone, -1)
    // P5   :   (min y scaling factor for the rim denser around the crater, max y scaling factor for the rim denser around the crater)
    void createCratersInDensers(QVector<Bone*> &boneOrderedList, QPolygonF craterSpecs);
    void createCurvyRidgeOnDensers(BodyPart *bpPtr, QVector<Bone*> &boneOrderedList, QPolygonF ridgeSpecs);
//    void createJawDepressionsForHead(BodyPart *bpPtr, QVector<Bone*> &boneOrderedList, QPolygonF jawDepSpecs);

    // Vector<Cell*> forcastBoundaryDnsrOfFormation(Cell* pivotCell)
    // bool willThisDnsrFormationOverlapAnotherDnsrFormation(QVector<Cell*> boundaryCells_form1, QVector<Cell*> boundaryCells_form2)
    // Cell* findAnotherPivotCell(Cell* pivCell)

    // This method will generate stacked densers from any given cell according to the parameters -> growthPoly
    // growthPoly is a Polygon that specifies   6 points
    //                                          (number of Densers, face of input cell) (pitch angle, yaw angle)
    //                                          (roll angle, should it mirror) (%age at which the angle will change, angle change amount)
    //                                          (x scaling value, y scaling value) (z scaling value, -1)
    void generateStackedDensersFromCell(Bone *bonePtr, Cell *cellPtr, QPolygonF growthPoly, QVector<float> rollAngleList);
    // This method gets the lineage of stacked densers in order for a given cell
    void getStackedDensersOfGivenCell(Bone *bonePtr, Cell *cellPtr, QVector<Cell*> &stackDensersInOrder);
    // This method splits the denser lineage into multiple parallel densers
    void splitStackedDensersOnBone(int numOfSplits, bool originateOnSameLine);
    // This method squeezes a portion of the denser lineage to make it narrower
    void squeezeStackedDensers(QPair<Bone *, Bone *> adjBones, QVector<Cell*> listOfFirstCellOfStkDnsr, float sqzAmnt);
    // This method makes the stacked densers lineage flow more irregular or organic looking
    void changeDenserLineageFlow(QPolygon flowPattern);
    void growZygmaticBoneFromMaxilla(Alien *alienPtr, BodyPart *bpPtr);
    void getAllSubBPsOfGivenType(Alien *alienPtr, BodyPart *parentBP_Ptr, int subBP_TypeID, QVector<BodyPart*> &subBPsList);


    // +++ ALIEN SKELETON CLONING & Saving+Loading from file +++
    Alien *cloneAlien(Alien *sourceAlien);
    Alien *cloneAlien_Algo2(Alien *sourceAlien);    // TODO(ismail): Also have alienPtr parameter to specify if the source alien
                                                    // ... need to be destroyed at the end of this method
    // This method creates several slightly different clones of the alien.
    // If slightDiffFlag=false, the clones are significantly different from each other,
    // ... e.g. One clone having a much more joints than the other clone
    QVector<Alien*> generateSlightlyDiffClonesOfAlien(Alien *alienPtr, uint8_t numOfClones, bool slightDiffFlag=true);
    // Saving and Loading Aliens
    bool saveAlienToFile(Alien *alienPtr);
    void saveBP_FieldsToFile(int bpID); // NOT IMPLEMENTED
    BodyPart *loadBP_FieldsFromFile(QString filename); // NOT IMPLEMENTED
    Alien *loadAlienFromFile(QString filename);
    BodyPart *generateBP_FromFile(Alien *alienPtr, int origBP_ID, int clonedFrom, QPair<int,int> cloneFromAlienAndBP_Pair,
                                  bool isSymmetric, int mirrorOf, int smoothnessLevel, int BP_Type, int numOfJnts, QPolygon Jnt2JntRelations,
                                  QMap<int, int> randIntForBookKeeping_EnergyTrend, QMap<int, float> randFloatForBookKeeping_FinalEnergy,
                                  QMap<int, int> randIntForBookKeeping_AngleTrend, QMap<int, int> randIntForBookKeeping_quaterEulerAngleSelectorForFirstChild,
                                  QMap<int, QVector3D> randQVector3DForBookKeeping_quaterEulerAngleSelectorForNotFirstChild,
                                  QPolygonF BP_to_JAT_rootJoint, QPolygonF Skeletal_BP_Bulkiness_DNA, QPolygonF BP_GenDensificationDNA,
                                  QMap<int, QPolygonF> BP_SpecificDensificationDNA, QMap<int,QPolygon> BP_BulgificationDNA,
                                  QVector<int> bonesIDsInOrder, QMap<int, int> &origBP_ToGeneratedBP_Map);
    ExtremityBP *generateExtBP_FromFile(Alien *alienPtr, int extID, QVector<BodyPart*> &carpals, QVector<BodyPart*> &digits);

    void cloneAlienSkeleton(Alien *sourceAlien, Alien *clonedAlien);
    void cloneAlienDNA(Alien *sourceAlien, Alien *clonedAlien);
    void cloneSkeletonDNA(Alien *sourceAlien, Alien *clonedAlien);
    void cloneJATsDS(Alien *sourceAlien, Alien *clonedAlien);
    void cloneTempVecLocCoupleForEachJntDS(Alien *sourceAlien, Alien *clonedAlien);
    // Creates a duplicate/clone of any given skeletal BP
    // TODO(ismail): Ensure it works and is able to clone Joints and Bones properly
    BodyPart *cloneSkeletalBP_viaGeometry(Alien *sourceAlien, BodyPart *sourceBP_Ptr, Alien *alienPtr);
    // The preferred method. This method is not suitable for cloning Joints and Bones
    BodyPart* cloneSkeletalBP_FromAnotherAlien_viaDNA(Alien *alienPtr, Alien *sourceAlien, BodyPart *sourceBP_Ptr,
                                                      int numOfClones=1, bool isMirror = false,
                                                      QPointF JAT_EnergyPt = QPointF(-1,-1), QPointF JAT_AnglePt = QPointF(-1,-1));

    void attachAllClonedChildBPsToTheirClonedParentBPs_afterAlienIsCloned(Alien *clonedAlienPtr, Alien *origAlienPtr,
                                                                          QVector<BodyPart*> &allClonedBPsPtrs, BodyPart *origParentBP);
    // void visitBPsRecursivelyForClonedBPsAttachment(Alien *clonedAlienPtr, Alien *origAlienPtr, QVector<BodyPart*> &allClonedBPsPtrs, BodyPart *parentBP);

    // The following two methods are very similar to each other
    void createSmoothingBackupCloneBP(Alien *alienPtr, BodyPart *sourceBP_Ptr);
    BodyPart* createSeperateCloneBP(Alien *alienPtr, BodyPart *sourceBP_Ptr);

    void addBP_ToItsRespectiveList(Alien *inputAlien, BodyPart *BP_Ptr); // TODO(ismail): Not being used
    void addAllSubBPs_ToAlienRecursive(Alien *inputAlien, BodyPart *BP_Ptr); // TODO(ismail): Not being used
    void cloneJnt2JntRelations_TypeGuided_DS(Alien *sourceAlien, Alien *clonedAlien);

    // This method gets all the joints of this BP
    // TODO(ismail): Think about how to make this generic so that the method accepts the BP Type
    void getAllBP_Joints(BodyPart *bpPtr, QVector<Joint*> &BP_JntsList);
    QVector<int> getAllJntIDsOfBP(BodyPart *bpPtr);
    // This method gets all the bones of this BP
    void getAllBP_Bones(BodyPart *bpPtr, QVector<Bone *> &BP_BonesList);
    int getNumOfJntsForBP(BodyPart *bpPtr);
    // this method gets a list of bones as they are connected in the lineage (like Jnt2JntRelations_TypeGuided)
    bool getAllBP_BonesInOrder(Alien *alienPtr, BodyPart *bpPtr, QVector<Bone *> &BP_BonesList);

    // This method applies relative scalings to BP against a specified BP
    // BP_ComparisonList is a map where key(int) is the BP_TYPE_ID, and value(quint8) is a flag with any of three values (0,1,2)
    // ... This flag tells us that: (0=not involved in scalings,1=involved in scalings,2=this is the BP aginst which other BP are scaled...aka Reference BP)
    // TODO(ismail): Perhaps make BP_ComparisonList a part of the DNA
    void applyRelativeScalingsToBodyParts(Alien *alienPtr, QMap<int, quint8> BP_ComparisonList);
    // Utility method to compute the length of a BP by adding the lengths of all its bones
    // This method can be handy for squashing/stretching a BP based on its total length
    float getTotalLengthOfSkeletalBP(Alien *alien, BodyPart *BP_Ptr);
    // This method sorts all the BPs according to their total lengths and puts them in a map. It returns the average total length
    float sortBPsAccordingToTotalLengths(Alien *alien, QVector<BodyPart *> BP_PtrList, QMap<int,float> &sortedTotLengthsMap);

    // Scale all the BP of a given type. This can be useful to scale all the BP of a particular type, in one go whenever the user wishes
    void scaleAllBPsOfGivenType(Alien *alien, int BP_Type);

    void incDecBP_Size(Alien *alien, int BP_ID, QVector3D scalingFactorVec);

    // This method updates the list (JntToJntMappings_ForBookKeeping) that stores the mapping of the joint in the source BP to
    // ... the destination/cloned BP.
    void updateJntToJntMappingList_forBookKeeping(Alien *alienPtr, QPolygon *sourcePoly, QPolygon *destPoly);

    // +++++++ EXTREMITY BP MODELLING/SCULPTING METHODS ++++++++
    void createExtremityBP_DNA_FromRS(Alien *alienPtr, QVector<int> extremIDs); // Perhaps not needed
    void addNewEntryForExtremityBP_DNA(Alien *alienPtr, int extremBP_ID);
    void modifyExtremityBP_DNA(Alien *alienPtr, int extremBP_ID, const QPolygon &modifiedPoly);
    // This method models the extremity BPs by creating Carpals and Digits. It also uses a state machine to generate new and varied types of BPs
    ExtremityBP* modelNewExtermityBP(Alien *alienPtr, int parentBP_ID);
    ExtremityBP* modelNewExtermityBP_UsingStateMachine(Alien *alienPtr, int parentBP_ID);
    void modelManyExtremityBPs(Alien *alienPtr, int numOfExtBPs, QVector<ExtremityBP*> &newExtBPsList, int parentBP_ID);
    // allParents DS here will be inputted by window that includes all those BPs that are currently being rendered in the GLW
    // ... These should only include BB, LL or UL
    void modelAndAttachSeveralExtremBPsOnSeveralRandomParentBPs(Alien *alienPtr, int numOfExtBPs, QVector<ExtremityBP*> &newExtBPsList, QVector<BodyPart*> &allParents);
    ExtremityBP *instantiateNewExtremBP(Alien *alienPtr);
    ExtremityBP_Base *instantiateNewExtemBase(Alien *alienPtr, ExtremityBP *exBP_Ptr);
    ExtremityBP_Digit *modelNewExtremDigit(Alien *alienPtr);
    ExtremityBP_Base_Carpal* modelNewExtremCarpal(Alien *alienPtr);
    void bulkifyExtremityDigits();
    void bulkifyExtremityCarpals();
    void alignAndAttachDigitsToExtremityCarpals();
    // alignmentSpecs = (distance b/w BPs, alignment trend ... either side-ways OR circular OR wavy)
    void alignBPsInVicinity(Alien *alienPtr, const QVector<BodyPart *> &origBP_List, QPoint alignmentSpecs);
    void sculptExtremBP();  // (Sculpt carpals) make the carpals more curved on the sides,
                            // Add heals,
                            // add ankle mutations
                            // add densifiers and bulgifiers to Digits and Carpals
    // This method deals with atttaching digits to carpals. It deals with two trends of attachment:
    // 1) Only one digit is attached to one carpal. 2) several digits are attached to one carpal.
    // The method is not very general and mostly hard-coded.
    // Before calling this method, we need to parent all the carpals in the extremity to the root carpal.
    bool attachExtremBP_ToParentBP(Alien *alienPtr, BodyPart *parentPtr, QVector<int> extremBP_IDs);
    // This method detaches the given extermity from their parents BPs
    // These BPs are any BP that are not extremities
    bool detachExtremBP_FromParentBP(Alien *alienPtr, int extID);
    QVector<int> detachAllExtremBPsFromGivenParentBP(Alien *alienPtr, BodyPart *parentPtr);
    // TODO: Check if this method will work on any kind of parent and child BPs
    void attachDigitsToCarpalsForNewExtremBP(Alien *alienPtr, QVector<BodyPart *> &digits, QVector<BodyPart *> &carpals);
    ExtremityBP *transferExtBP_FromSourceAlienToDestAlienWithCloning(Alien *sourceAlien, Alien *destAlien, ExtremityBP *srcExtBP_Ptr, int parentBP_ID);
    // ExtremityBP *transferExtBP_ToAnotherParentWithoutCloning(Alien *destAlien, ExtremityBP *srcExtBP_Ptr, int newParentBP_ID);
    ExtremityBP *cloneExtremityBP(Alien *sourceAlien, Alien *destAlien, ExtremityBP *srcExtBP_Ptr);
    void removeAndDeleteExtremityBP_FromAlien(Alien *alienPtr, ExtremityBP *tempExBP, BodyPart *parentPtr=nullptr);

    // +++++++ BONE MODELLING METHODS ++++++++
    // for these I can make a generic method that can do these transformations to all the bodyparts
    void modelAllAlienBones(Alien *alien);
    void modelAllAlienBones_TypeGuided(Alien *alien, QMap<int, QPolygon*> &tempJnt2JntRelations);
    // This method only removes the bones. The joints remain there
    void removeAllAlienBones();
    void createSimpleBone(Bone &bone, QVector3D pnt1, QVector3D pnt2);
    // TODO(ismail): Make it a complete and robust method
    bool removeBone(Alien *aPtr, BodyPart *bpPtr, Bone *bonePtr);
    void setRulesetSpecsForAllBones();
    // BP_ID_List contains the IDs of BPs that need to be bulked up
    void bulkUpBodyPart(Alien *alien, QVector<int> &BP_ID_List); // Uses the Skeletal_BP_Bulkiness_DNA
    // This method will bulkup the subdiv cells of the incoming bones.
    void bulkUpBP_BoneSubdivCells(Alien *alien, BodyPart *bpPtr, QVector<Bone*> &boneList); // Uses the Skeletal_BP_SubDivCells_DNA


    /*Not implemented yet*/
    void getAdjacentBonesForGeneratingHangingDensers(Alien *alien, BodyPart *bpPtr);
    /*Not implemented yet*/
    void generateHangingDensersForBP(Alien *alien, BodyPart *bpPtr);
    /*Not implemented yet*/
    void generateDensersOnBone(Alien *alien, Bone *bonePtr /*some more parameters from the DNA*/);
    /*Not implemented yet*/
    void generateDensersBetweenTwoBones(Alien *alien, BodyPart *bpPtr);
    /*Not implemented yet*/
    void incDecSizeOfDensersOnBone(Alien *alien, Bone *bonePtr, bool incDecDenserSize, float incDecFactor);

    /*Utility Method*/
    // faceID represent the the same face for all the bones. e.g. the LEFT face for all the bones
    QVector<QVector3D> getNormalVectorsOfAdjacentBonesForGivenFace(Alien *alien, QVector<int> &bonesIDsList, int faceID);
    /*Utility Method*/
    QVector<float> getAnglesBetweenAdjacentVectors(QVector<QVector3D> listOfAdjVectors);
    /*Utility Method*/
    QVector<int> getAngleTrendsOfJntsOfBP(Alien *alienPtr, BodyPart *bpPtr);


    // *** DENSIFICATION / BP SCULPTING ALGORITHMS ***
    void densifyAllBodyParts(Alien *alienPtr, QVector<int> &BP_ID_List); // Uses the BP_DensificationDNA
    // increase or decrease spikiness of the densers.
    // ... True = increase; False = Decrease
    void incrDecrDensifierSpikiness(Alien *alien, BodyPart *bpPtr, bool incOrDec);
    // stacked densers are also called extruders
    void incrDecrStackedDensers(Alien *alien, BodyPart *bpPtr, bool incOrDecr);
    // Increase or decrease the curvature of the stacked densers. Basically it affects the roll angle
    void incrDecrDenserCurvature(Alien *alien, BodyPart *bpPtr, bool incOrDec);
    // this method sculpts a bone by adding ridges, according to the bone ruleset, using other methods
    void incDecDenserSpikeMovingUpDownPerc(Alien *alien, BodyPart *bpPtr, bool incOrDec);
    void moveDensersAcrossBP_Bones(Alien *alien, BodyPart *bpPtr, bool upOrDown);
    void sculptBones();
    void squashBone();
    void elongateBone();
    void bendBone(Alien *alienPtr, Bone *bonePtr);
    QVector<QVector3D> convertLineToCurve(QPair<QVector3D,QVector3D> line, bool isThisS_Bend, QPolygonF bendDNA_Poly);
    QPair<QVector3D,QVector3D> getPositionOfEndJointsOfBone(Alien *alienPtr, Bone *bonePtr);
    // method for making bone Bulginess. Uses the Bulginess DNA
    void bulgifyBodyPart(Alien *alienPtr, BodyPart *bpPtr);
    void bevelBP(Alien *alien, BodyPart *bpPtr);
    // The following two methods aim to make the BP more organic by adding more joints to it.
    void embedMoreJointsInBP(Alien *alienPtr, BodyPart *bpPtr, int numOfJntsToEmbed, bool regularOrIrregularSpacing);
    void appendMoreJointsToBP(Alien *alienPtr, BodyPart *bpPtr, int numOfJntsToAppend, bool regularOrIrregularSpacing);
    // UN-IMPLEMENTED METHOD
    QPolygon* subdivideBoneIntoSmallerBones(Alien* alienPtr, Bone *bonePtr, int numOfJntsToEmbed, QVector<float> percOfVecForNewJntsLoc); // this will insert new Joints and bones using the input Points and will remove the old bone from AlienBones and bodyParts
    QVector<Cell*> subdivideBoneIntoSubcells(Bone *bonePtr, QVector<QVector3D> &inputPts, QVector<QVector3D> &bulkVec); // this will insert new cells using the input Points
    // Subdivision is done accoriding to Skeletal_BP_SubDivCells_DNA
    void subDivideBP_BonesIntoSubdivCells(Alien *alienPtr, BodyPart *bpPtr);
    Cell *makeCellBetweenTwoPoints(QVector3D firstPnt, QVector3D secondPnt);
    // This method converts a boxy cell into a more smoothed version (like a ball i.e. a spherical cell) by adding more cell boxes to it.
    Cell* smoothCell(Cell *cellPtr, int smoothlevels);
    Cell* explodeCell(Cell *cellPtr, float distanceFromCorner, QVector3D xyzScaleVals, int numOfExplosions);
    void sculptBoneIntoEndSwellings(); // OOooOO
    void sculptBoneIntoMidSwellings(); // ooOOoo
    QVector<Cell*> createCellSmoothingsOnSpecifiedLocOnLine(Alien *alienPtr, int *bpID, QPair<QVector3D,QVector3D> line, QVector<QVector3D> &specLocOnLine);
    void computeFurtherGrowthPoints();
    void initializeBoneBulgingsSpecs();
    void generateBoneBulgings();
    void setBoneRidgesSpecs_global();
    void setBoneRidgesSpecs_local();
    void generateBoneRidges();
    void generateBoneDents();
    Bone *instantiateNewBone(Alien *alien);
//    void generateFurtherBranchingGrowth();

    // +++++++ JOINT MODELLING/SCULPTING METHODS ++++++++++//

    void createSimpleJoint(Joint *jnt, QVector3D loc, QVector3D dir);
    void modelAllAlienJoints_TypeGuided(Alien *alien, QVector<Joint*> &jntList);
    Joint* instantiateNewJoint(Alien *alien);
    bool removeJoint(Alien *alien, BodyPart *bpPtr, Joint *jPtr);
    bool updateNumOfJointsPerBP_DNA(Alien *alien, BodyPart *inBP_Ptr, int numOfJntsToIncDec);

    // TODO(ismail): This should return a list of new BPs created
    // TODO: Use a much better approach such as factory methods, and get rid of the
    // dedicated lists for each type of BP, such as QVector<UpperLimb*>
    // TODO: THis method should return a list of newly instantiated BPs
    void instantiateNewBodyParts(Alien *alien, int BP_Type, int numBPs);
    void addBodyPartToCurrentAlienSkeleton(Alien *alien, BodyPart *bpPtr);

    // Build a catalogue of seed joints. This catalogue can be made before any alien is modelled
    // TODO(Ismail): From this catalogue, more joints are generated using some interesting logic
    void generateSeedJoints();

    // void modelNewAlienUsingCells();
    void createChildCells(Cell *cellPtr, int depth); // recursive method

    // All the growth formation algorithms
    void surroundingGrowthFormations(Cell *inputCell, int depth, QVector<QVector3D> &tempFaceList);

    void gridBasedFormations(Cell *inputCell);

    void Stage1_Modelling(); // mostly deals with base growth formations and allocation of basic/main body parts (arms, legs, pelvis, head etc)
    // IDEA: Base Growth Formations Algos (a class housing several algorithms for base growth formations for the body part)
    // BGF --> Base Growth Formations
    // Input:   1) - Set of cells (0 to many)
    //          2) - The body Part
    void generateBGF(QVector<Cell*> cells, BodyPart *bPart); // BGF = Base Growth Formations
    // TODO(ismail): REFACTOR: make a seperate class of all the algorithms for modelling different
    // body parts of the alien. Perhaps explore design patterns to select an algorithm with just
    // one call from AlienModeller class
    // TODO(ismail): Perhaps make a pointer to Alien inside the AlienModeller class and assign it the current Alien object (the Alien that is currently being modelled).
    // This is because there can be several alien objects active at the moment. Passing the pointer to an Alien object
    // inside the method is not always a great idea.
    void modelBP_ThighBone(Alien *alien);
//    void modelThighMuscle_1_BP();
//    void modelThighMuscle_2_BP();
//    void modelUpperLegBP();

    void rearrangeBodyPartCells();
    void rearrangeBodyPart();

    void generateCellBranchings(Cell *cell, Alien *alien); // generates sub cells (branches) on any given cell

    void initBP_IDs();

    // PLACEHOLDERS
    void specifyLevel_1_2_FeaturePlaceHolders(/*body part*/); // this function makes a list of specs
    void computeLevel_1_2_FeaturePlaceHolders(/*body part*/); // this function actually computes the placeholder quads based on the
    void rearrangeLevel_1_2_FeaturePlaceHolders(/*body part*/); // this function can be called after all the level one placeholders have been finalized
    void attachLevel_1_2_FeatureOnPlaceHolder(); // This method is called after rearrangeLevel_1_2_FeaturePlaceHolders()
    void readLevel_1_2_FeatureFromOBJ_File();

    void makeAlienRuleSets(Alien *alien);

    // Utility Methods
    void computeRandomPointsOnFace(QVector<QVector3D> &tempFaceVertices, int numOfBranch, QVector<QVector3D> &points); // should take as input a vector composed of face vertices
    void computeRandomPointsOnCell(Cell *cell, int minPts, int maxPts, QVector<QPoint> &points);
    void computeRandomPointsOnBP(BodyPart &bp, int minPts, int maxPts, QVector<QPoint> &points);
    BodyPart *getClosestBPFromPoint(QVector3D inputPt, QVector<BodyPart*> *inputBPsList);
    BodyPart *getBP_PickedByMouse(QVector<BodyPart*> &bpList, QVector3D inputPt, QPair<BodyPart*,Cell*> &BP_Cell_Pair);
    bool checkCellForMousePicking(QVector3D inputPt, Cell *cellPtr);
    Joint *getJointFromID(Alien *alien, int BP_ID);
    Bone *getBoneFromID(Alien *alien, int BP_ID);
    // This method converts the Jnt to Jnt Relations Poly to an ordered list of Joints
    // ... i.e. the Joints Lineage List containing IDs of Jnts
    void convertJnt2JntPolyToJointsLineageList(QPolygon *Jnt2JntRelPtr, QVector<int> &jntLineageList);
    BodyPart *getBodyPartFromID(Alien *alien, int BP_ID);
    ExtremityBP *getExtremityBP_FromID(Alien *alienPtr, int extremID);
    void getAllCarpalsAndDigitsIDsOfBP(Alien *alienPtr, int BP_ID, QVector<int> &carpalDigitsIDs);
    BodyPart *getParentOfThisBP(Alien *alien, BodyPart *inBP_Ptr, ExtremityBP *exBP_Ptr=nullptr);
    // This method will get all the BPs and their parent (if any) in a map.
    QMap<int,int> getChildParentRelationsInMap(Alien *alienPtr, bool includeMirror=false);
    QMap<QPair<int,int>,QList<int>> getExtremityAndParentToCarpalsAndDigitsMap(Alien *alienPtr, bool includeMirrorAndTheirExtremities=false);
    BodyPart *getBP_containingThisBone(Alien *alien, int boneID);
    // TODO(ismail): Should be a parameter to specify the incoming BPs to be added to the existing list of
    // ... BPs that can be displayed in this GLW
    void updateGLW_to_Alien_to_BP_to_ColorDisplay_Map(Alien *alienPtr, int GLW_ID, QVector<int> BP_ListToDisplay, QVector<int> BP_ListToDisplayInDiffColor, int diffColor);
    int getGLW_ID_ThatDisplaysThisAlienAndBP(int alienID, int BP_ID, QVector<int> &GLW_List);
    QVector3D getBP_Color(int GLW_ID, Alien *alienPtr, int BP_ID);
    BodyPart *getBP_ThatWasClonedFromGivenBP(Alien* clonedAlien, BodyPart *srcBP_Ptr);
    BodyPart *getOriginalBP_FromClonedBP(BodyPart *clonedBP_Ptr);
    BodyPart *getMirrorOrOriginalBP(Alien *aPtr, BodyPart *inputBP_Ptr);
    // *** TODO(ismail): UNIMPLEMTED METHOD ***
    // This method computes the joint's energy from the magnitude of the vector between this joint and its parent joint in the
    // ... Jnt2JntRelations_TypeGuided. It also computes the angle between the vector if this joint and its parent joint in the
    // ... Jnt2JntRelations_TypeGuided
    void computeFinalEnergyAndAngleForBodyPartJointsFromJnt2JntRelations(Alien *alien, BodyPart *bpPtr);


    // RENDERING RELATED
    void getAllBPsCellListForRendering(QVector<BodyPart*> &bpList, QVector<QVector<Cell*>> &tempCellMatrix, QMap<int, int> &tempCellListToBP_Map__forColoring); // method to return geometric primitive arrays.
//    void getBodyPartCellList(BodyPart *thisBP, QVector<Cell*> &tempCellList);
//    void visitCellsRecursively(Cell *rootCell, QVector<Cell*> &tempCellList); // TODO(ismail): Perhaps make it private

    // DEBUGING/DIAGNOSES Methods
    void printAllJointLocations(Joint &rootJnt);

    // Further Growth Formations Algos (the same class as above housing several algorithms for further growth formations for the body part)

//    UpperTorso* getUppTorsoPtr()
//    {
//        return uppTorsoPtr;
//    }

/*---- Improvement suggestion:
         Duplicate Model: There should a method to duplicate any 3D model/box to be created
         ... at the same location

        change the term 'feature' to 'bodypart'
*/


    //*** Member Variables ***

    int AlienID_Counter;

//    TorsoModeller *torsoModellerPtr;
//    UpperTorso *uppTorsoPtr;

    QVector<Alien*> alienList;
    // This DS contains all the alien pointers that are to be cloned
    QVector<Alien*> aliensToCloneList;
    QMap<int, int> BP_ToType_Map; // Utility/helper memeber used for bookkeeping for cloning and mutating DNA

    //  A DS that stores the BP IDs of each BP that the user selects to be cloned and further mutated
    //  int is the BP ID
    //  QPolygon will have as many QPoints as the BPs to be selected for cloning.
    //  Each QPoint is: (BP ID, to be cloned or not? T/F)
    QMap<int, QPolygon> alienBPsToClone;
    QVector<Level_1_2_FeaturePlaceholderSpecs*> placeholderSpecs;

    // enum for: OriginatingBP, FeatureID, featureLevel, faceNum, ??

    QVector<QString> originLevel_0_BP_ID_List;
    QVector<QString> level_1_BP_ID_List;
    QVector<unsigned int> BP_Level_List;

    QVector<Joint> seedJoints; // all seed joints, which can be simple cubes, or imported from Maya
    QVector<Bone> seedBones; // all seed bones, which can be simple cuboids, or imported from Maya.

    Bone_RuleSet *boneRuleSet;
    Joint_RuleSet *jointRuleSet;
    Skeleton_RuleSet *skeletonRuleSet;
    Ridge_Ruleset *ridgeRuleSetPtr;

    // TODO(ismail): this should perhaps be part of the alien skeleton DNA
//    QVector<JointsArrangementTrend> jntsArrangeTrends; // joints arrangement trends for all aliens

    // These two lists are used to pair up joints and store their pairs in the Jnt2JntRelations DS
    QVector<int> tempJntList_1;
    QVector<int> tempJntList_2;

    QMap<int,QPoint> isJntAChild;   // for QPoint, x = child level, y = is this branched-out child?
                                    // A branched-out child is a child that does not follow the same vector direction as its parent

    // ** NOT REQUIRED FOR TYPE GUIDED** declare a map of pairs/couples, where the first element of the pair will store the vector for guiding the orientation
    // ... and the second vector is the position/location of the joint
    QMap<int, QPair<QVector3D, QVector3D>> tempVectorLocationCoupleForEachJoint;

    QMap<int, QVector3D> colorMap; // this map stores colors for each BP Type. Key is BP_TYpe, Value is the RGB color
    QMap<int, QVector3D> colorMap_2; // This map associates an RGB color to its enum number in the colorEnum
    void initializeBP_TypeColorMap(); // Initialize a color for a BP Type
    void setColorMap(); // Sets the RGB values for each color in the enum colorMap_2

    // This DS stores the alien ID and BP_ID of the BP selected most recently by the user in a GLWidget
    // key(int) is the GLWidget ID.
    // QPair is <ALien ID, BP_ID>
    QHash<int, QPair<int,int>> selectedAlienAndBP_ID;

    // When an alien is selected by the user this DS contains those ALienIDs along with their GLW_IDs
    // <GLW_ID, AlienID>
    QVector<QPair<int,int>> selectedAliensInGLWidgets;
    QVector3D selectedExtremBP_InGLWidgets; // (GLW_ID,AlienID,ExtremID)

    // THis map contains information about whether the GLW should display a BP or not and in which color
    // When a BP is displayed in a AOI GLW panel, its color will be slightly different from the one in the main AOI panel.
    // ... This will enable the system to identify whether the BP needs to be displayed in the centre.
    // A GLW can hold one or many Aliens, which can display one or many BPs
    //  GLW_ID    ALienID    BP_ID     QPair<toDisplay?, ColorInd>
    QMap<int, QMap<int, QMap<int, QPair<bool,int>> >> GLW_to_Alien_to_BP_to_ColorAndDisplay_Map;
    QMap<QString, int> alienTypeMap;
    QMap<QString, int> BP_TypeMap; // BP name to type ID map

    bool incDecButtonPressed=false;

    bool alienJustLoadedFlag=false;

//    // This map stores (pivot vector, location) for each joint
//    // Key(int) is the Joint ID (BP_ID). declare a map of pairs/couples, where the first element of the pair will store the vector for guiding the orientation
//    // ... and the second vector is the position/location of the joint
//    // TODO(ismail): This should be part of the Alien Skeleton
//    QMap<int, QPair<QVector3D, QVector3D>> tempVectorLocationCoupleForEachJoint_TypeGuided;

//    QVector<int> BP_ID_Counter_ForAllAliens;

    // TODO(ismail): A better thing to do is to implement a selection box. When the user drags a selection box around a BP, that entire BP is selected
    // ... and the alienmodeller gets a pointer to that selected BP. Then we can use it for any other operations such bending, scaling, bulking etc
//    BodyPart *BPToBend; // BP whose bone is to be bent. This is set by the GLWidget

    enum ALienTypesEnum  {
                            BIPEDAL = 0,
                            QUADPEDAL = 1,
                            AQUATIC = 2,
                            AMPHIBIOUS = 3,
                            OCTAPEDAL = 4,
                            ANTHROPOD = 5,
                            /*TODO(ismail): AERIAL CREATURE*/
                            ALienTypesEnum_Length
                        };

    enum BodyPartsEnum  {
                            JOINT = 0,
                            BONE = 1,
                            LOWER_LIMB = 2,
                            UPPER_LIMB = 3,
                            BACK_BONE = 4,
                            TAIL = 5,
                            HEAD = 6,
                            HAND = 7,
                            FOOT = 8,
                            NECK = 9,
                            RIB = 10,
                            HEADSubBP_Maxilla = 11,
                            HEADSubBP_Mandible = 12,
                            HEADSubBP_Zygomatic = 13,
                            HEADSubBP_NasalBone = 14,
                            HEADSubBP_FrontalBone = 15,
                            HEADSubBP_ParietalBone = 16,
                            ExtremBP_Carpal = 17, // Wrist bones
                            ExtremBP_Digit = 18, // fingers, toes, claws
//                            HeadSubBP_Lineage = 12,
                            // RIB, WING, HORN, FANG, SHELL, WEBBED_CAGE (https://en.wikipedia.org/wiki/Clathrus_ruber), HELIX_SHAPED, HYBRID_BP
                            BodyPartsEnum_Length
                        };

    // PERHAPS NOT NEEDED if Head is modelled using Densers/Extruders
    enum HeadSubBodyPartsEnum  {
                            FrontMandible = 12,
                            RearMandible = 13,
                            LowerMaxilla = 14,
                            UpperMaxilla = 15,
                            SideMaxilla = 16,
                            ZygomaticBone = 17,
                            NasalBone = 18,
                            FrontalBone = 19,
                            ParietalBone = 20,
                        };

    enum ColorEnum  {
                            RED = 0,
                            GREEN = 1,
                            BLUE = 2,
                            ORANGE = 3,
                            PURPLE = 4,
                            BLUE_VIOLET = 5,
                            STEELBLUE = 6,
                            OLIVE = 7,
                            COLOR_1 = 8,
                            COLOR_2 = 9,
                            COLOR_3 = 10,
                            ORCHID = 11,
                            SIENNA = 12,
                            DEFAULT_COLOR = 13,
                            ColorEnum_Length
                        };

//    QVector<Level_2_Features> level2Vocabulary;
};

#endif // ALIENMODELLER_H


/*
 * ++++++ All Algorithms ++++++
 *
 * ++++ Algorithm -- Densify BP (uses Densify Bone) +++++
 *
 *
 * +++ Update Densification DNA - algorithm +++
 ******** if (this alien is not a clone)
 ********    Go through the Densification Ruleset
 ********    specify the number of densers for this BP - store it in DNA
 ********    specify the number of densers with center point generation - store it in DNA
 ********    specify the points on the Bones which are involved in generating densers - store it in DNA
 ********    specify the number of levels of each denser - store it in DNA
 ********    specify the X and Y scalings for each denser - store it in DNA
 ********    specify whether this denser will mirror - store it in DNA
 ******** else
 ********    clone source DNA to destination DNA
 ********
 ********  if (DNA mutation is required)
 ********    modify the DNA
 ***
 *** go through all the points on the Bone i.e. Denser locations
 ***    generate the denser at the denser location i.e. a cell and store it in the cellList of the Bone
 ***    if this denser is to be mirrored, generate its mirrored clone on the opposite face of the Bone
 ***    go through the number of levels of this denser
 ***        if (the base denser has center point location enabled)
 ***            find the center point on the front face
 ***        else
 ***            specify a point other than the center point on the front face
 ***        generate the denser on this location based on X and Y scalings - store it in DNA
 ***        if (base denser is to be mirrored)
 ***            generate the mirror of this denser on the opposite face of the Bone
 ***
 ***
 *
 *
 *
 * +++++ Algorithm: Relative Scalings of BPs (BEGIN) +++++
 *
 * Get the SIZE of the LL and BB (A method defined in the Alien Modeller)
 *
 * if (LL SIZE < BB SIZE)
 * {
 *      while (LL SIZE < BB SIZE)
 *      {
 *          scale the LL up by 1.5
 *      }
 * }
 *
 * else
 * {
 *      while (LL SIZE > BB SIZE)
 *      {
 *          scale the LL down by 1.5
 *      }
 * }
 *
 * After this the difference between the SIZES of LL and BB should be very small
 *
 * Check the SIZING CRITERIA. (How small or large the LL should be compared to BB. For example, LL should be 0.1 to 10 times smaller/larger than BB)
 * Now scale the model (up or down) based on the SIZING CRITERIA
 *
 * +++++ Algorithm: Relative Scalings of BPs (END) +++++
 *
 *
 * +++++ Algorithm: Relative Scalings of BPs (END) +++++
 *
 *
 *
 *
 *
 *
 * Algorithm: Relative Scalings of BPs (END)
 *
 *
 *  Stage 1: Model Torso
 *  Stage 2: Model Body Parts
 *  Stage 3: Model Intermediate Formations
 *  Stage 4: Model Fine Grained Formations
 *
 * >> The DNA Engine
 *
 * >> The Formations Observer Engine
 *          // The obeserver travels through the body of the alien, region by region.
 *          // Along the way, it takes into consideration the region's local growth trend and
 *          // decides about what the next possible growth formations could be. This is based on
 *          // on perhaps the DNA of the entire alien, or the region's local DNA
 *          --- possible algorithms
 *              > region (set of boxes on close proxity) selection
 *              > Inferring the formation of a region and specifying further growth
 *              > shape identification
 *              > conjoining shapes
 *
 * >> The Native Growth Engine - Growth Formation Algorithms
 *
 *      1 -  LinearStep (for tentacle style growths)
 *      2 -  RockFormationEffect/Progressive formation effect (for scale and ridges effect growth)
 *                  __
 *               __| |
 *            __| | |
 *           | | | |
 *
 *      3 -  Surrounding Growth (bubbling effect growth)
 *      4 -  WaveEffect (for fins or petal style growth) (some likeness to Grid based algo)
 *      5 -  SpiralEffect (for most limbs)
 *      6 -  MirrorEffect
 *      7 -  Conjoining Effect
 *      8 -  Grid based decomposition algorithm (some likeness to Grid based algo)
 *      9 -  Bridge based formations
 *      10 - Exploding style formations (like crystal cave)
 *      11 - Gravity driven formations (using physics)
 *
 *
 * >> External Factors Sculpting Engine - External factors affecting growth formations (inspiration from various rock formations, planet formations, vegetation formations etc)
 *      // What are the external factors??
 *              1 - Turbulence inducer algorithm
 *              2 - wind based sculpting algorithm
 *              3 - pressure based sculpting algorithm
 *
 * >> The Sketch-based Guidance Engine
 *      // An interface for artists to draw the kind of growth they wish to appear
 *
 * >> Free Growth Engine
 *      // Growth to be shown in several time frames
 *
 *
 * // Other Utility Algorithms for the Alien Modelling Engine
 *      1 - Region Selection (selecting several cell boxes to serve as the budding cell boxes for different body parts)
 *      2 - resizing and/or relocating boxes in a later pass
 *      3 - Inferring possible body part
 *      4 - Maintaining the geometry data structures
 *
 *
 * // References to be taken for growth formation algorithms
 *
 *      > deep sea creatures
 *      > pre-historic creatures from Cambrian explosion (800 million years ago)
 *      > rock formations of different planets and their origins
 *      > Hydrothermal vents and the biodiversity around it
 *      > Speculative Biology creatures
 *
 *
 * ------ Stage 1: Model Torso (Algorithms)
 *
 *  select one of the following algorithms randomly
 *
 *  - Surrounding Growth
 *  - Grid based decomposition
 *  - base_body_part_selection
 *
 * //   Think about which of these algorithms can be used after any other algorithm
 *      that was used before. Like a state machine of algorithms
 *
 * --- Surrounding Growth Algorithm ---
 *
 *  Make the root cell box (aka the base torso cell model)
 *
 *  // consider using Johnson's Solids as the base torso cell model
 *
 *
 *      Method 2: (regular boxes surrounding)
 *          create additional vectors which are the copies of the normal vectors of the cell box (0 to many vectors for each face)
 *          rotate these vectors according to certain axes and angles
 *          create the cell boxes around the centre points where these vectors intersect the cell box faces
 *          rescale the newly created cell boxes according to certain criteria
 *
 *      Method 3: (box surrounding on upper portion)
 *          create several additional vectors that mostly point towards the upper portion of the root cell box
 *          create cell boxes along these vectors
 *          rescale the newly created cell boxes according to certain criteria
 *
 *      Method 4: (box surrounding on lower portion) // Method 3 and Method 4 can be generalized to any portion of the root cell box
 *          create several additional vectors that mostly point towards the lower portion of the root cell box
 *          create cell boxes along these vectors
 *          rescale the newly created cell boxes according to certain criteria
 *
 *
 * --- Grid-based decomposition ---
 *
 *      Create an imaginary polygon (think of other planar shapes for further diversities)
 *      Create N imaginary lines inside the polygon
 *      Store the lines in a data structure
 *      Specify M points on the line with equal spacing
 *      Each point will result as the cell location for a cell box
 *
 *      select one of the following algorithms
 *      - Step Formation
 *      - Curved Formation
 *      - Follow the leader formation
 *      - Wave formation
 *
 *      if (Step Formation selected)
 *      {
 *          move the location point at a specific direction along the normal vector of the plane
 *          move subsequent points in a step wise fashion to create the effect of the staircase
 *          create the cell boxes on these locations
 *      }
 *
 *      else if (Curved Formation selected)
 *      {
 *          Rotate the box towards its side vector direction.
 *          Generate another box in the same direction as its previous box and rotate
 *              the box in the side direction but at a slightly bigger angle.
 *      }
 *
 *      else if (Follow the leader formation)
 *      {
 *
 *      }
 *
 *
 *
 *
 *
 * create 3xN or Nx3 grid patterned cell vectors arranged as a grid
 *          rotate the side vectors in some angle (in a symmetrical manner)
 *          make cell boxes for each of these vectors
 *          rescale cell boxes if needed
 *
 *
 *  --- base_body_part_selection Algorithm ---
 *
 *      select the faces and assign these as budding points for the base body features at random (follow Rule set 1)
 *
 *      For each type of base body part growth
 *      {
 *          random growth generations on the budding points: identify what sort of base growth is possible from the
 *                                                       budding point growths (single or multiple boxes)
 *
 *          execute base growth formations
 *
 *          random further growth generations on the budding points: identify what sort of further growth is possible from the
 *                                                       budding point growths (single or multiple boxes)
 *
 *          execute further growth formations
 *      }
 *
 *
 * ------ Stage 2: Model Body Parts (Algorithms)
 *
 *      LEVEL 0 BODY PARTS:
 *          BONES:
 *          - Thigh Bone (1)
 *          - Lower Leg Bone (2)
 *          - Pelvic Bone (3)
 *          - Upper Arm Bone (4)
 *          - Lower Arm Bone (5)
 *          - Rib Cage Bone (6)
 *          - Back bone (7)
 *
 *          JOINTS:
 *          - joint b/w (1) and (2)
 *          - joint b/w (1) and (3)
 *          - joint b/w (4) and (5)
 *          - joint b/w (6) and (7)
 *          - joint b/w (3) and (7)
 *
 *          BLOBS (These are like fungus, mushroom style formations that occur between two joints)
 *
 *          FISH TAILS (These can be highly organic/smooth)
 *
 *
 *      IMPORTANT: Each of the Level 0 BPs will be modelled and will undergo mutations using some
 *      sub algorithms. To achieve diversity in skeletons, I need to use different permutations
 *      and combinations of the level 0 BPs. From Joints, we can give rise to other BPs which
 *      can belong to Level 1 or 2 BPs (for example Tentacles, clusters, fins etc)
 *
 *      +++ Lower limbs (legs) modelling +++
 *
 *      Level 0 -> bones structures/formations/mutations
 *      Level 1 -> base muscle formations
 *      Level 2 -> secondary features formations / further growth formations
 *
 *
 *      **** Level 0 modeling algorithms
 *
 *****  Algorithm: Model Thigh Bones (BEGIN)
 *      Summary:  This algorithm models the thigh bone
 *      Inputs:
 *      Outputs:
 *      Sub algorithms used (select randomly or according to a state machine):
 *          1 - Cell Branching algorithm
 *          2 - ?? ... <come up with 2 more algorithms here>
 *          3 - ??
 *
 *      Steps:
 *      generate a cell
 *      scale the BP so it looks like a femur bone (randomized scaling...using ruleset)
 *      Run one or more of the following algorithms (randomly depending on some criteria)
 *          <Cell Branching Algorithm> (with branching preference parameters for this body part)
 *          <fungal growth generation/formation algorithm> (possible formation...the fungi can grow out from the bone and penetrate its way out of the muscles)
 *
 *****  Algorithm: Model Thigh Bones (END)
 *
 *
 *****  Algorithm: Cell Branching Algorithm (BEGIN)
 *      Summary:    This algorithm performs branchings on any body part (bone, muscle etc). Takes aid from RuleSet. Need more general algos. Similar
 *                  algorithms can be Zone-selection, Twisting-growth-formations etc
 *      Inputs: Cell pointer
 *      Outputs: Cell with branchings
 *
 *      Steps:
 *      Check RuleSet for which faces to ignore
 *      For each face selected
 *          Check RuleSet for how many branchings
 *          Select random points on the face for braching spawn points (Run the random points selector algorithm)
 *          determine the direction for the branchings
 *          generate the cells for the branchings angled accordingly
 *          Randomly perform the scaling for each branch cell according to ruleset
 *          add the cells to the cellList of the BodyPart
 *
 *****  Algorithm: Cell Branching Algorithm (END)
 *****
 *****
 *
 *****  Algorithm: RearrangeBodyPartCells (BEGIN)
 *****
 *****
 *****
 *****
 ***** Algorithm: RearrangeBodyPartCells (END)
 *****
 *****
 *****
 *****
 ***** Algorithm: InitializeAlienModeller (BEGIN)
 *****
 ***** Algorithm: InitializeAlienModeller (END)
 *****
 *****  CLass: Level_1_2_FeaturesPlaceholderSpecs
 *****  notes: Each placeholder is represented as a quad (rectangle) which is extracted from the one of the faces of the bodypart
 *
 *      face number (the face on the bodypart where the placeholder is extracted from)
 *      featurelevel (feature level for the feature to be placed here...either 1 or 2)
 *      feature_ID/feature_name (to be attached to this placeholder) (the feature to be attached here depends on the feature level specified in the
 *                              above attribute)
 *      edge (the edge of the face from where we need to compute the placeholder quad)
 *      edge segment (a segment on the edge. This can be computed randomly and based on some criteria)
 *      %age of distance towards the center vector (randomly select the presentation within a certain boundary. for example b/w 25% and 125%)
 *
 *
 *
 *
 ***** Algorithm: jointsArrangements (BEGIN)
 ***** input:
 ***** output:
 *****
 *****
 ***** pick up the central joint (a joint chosen at random)
 *****
 ***** compute the JointToJointRelations DS (2D array) that associates all the joints attached to the joint on each row (this DS can be computed from skeleton_specs DS)
 ***** (call METHOD: computeJointToJointRelationsDS)
 *****
 *****
 ***** setup VectorEmanationTrend object (call METHOD: setupVectorEmanationTrend)
 *****
 ***** specify the position/location of the central joint
 *****
 ***** for (go through all the joints in the Jnt2JntDS)
 ***** {
 *****      for (go through all the neighbouring joints)
 *****      {
 *****          if (this neighbouring joint's position has been computed before)
 *****              continue;
 *****
 *****          else if (previous energy OR angle is negative) // this means that the system needs to specify the locations on the neighbouring joints from the central joint
 *****          {
 *****              // come up with a random energy and assign it to this neighbouring joint. Store this energy in the VectorEnergiesOnThisLevel (at the same index as the neighbouring joint)
 *****
 *****              // come up with a random angle and assign it to this neighbouring joint. Store this angle in the VectorAnglesOnThisLevel (at the same index as the neighbouring joint)
 *****              // Algorithm for vector computation:
 *****              //      select a face of the parent joint at random.
 *****              //      select a random point on this face
 *****              //      create a vector (0,0,1) at the origin
 *****              //      rotate this vector in the direction of the randomly chosen face's normal vector
 *****              //      now rotate it further according to a randomly chosen angle along its pitch, yaw and roll angles (b/w 0 and 90)
 *****          }
 *****
 *****          else
 *****          {
 *****
 *****              compute the new energy from the previous energy trend (computed from VectorEnergyAngleTrend). Store this energy in the VectorEnergiesOnThisLevel (at the same index as the neighbouring joint)
 *****
 *****              compute the new angle from the previous angle trend (computed from VectorEnergyAngleTrend). Store this angle in the VectorAngleOnThisLevel (at the same index as the neighbouring joint)
 *****
 *****          }
 *****
 *****          compute the new position/location of this neighbouring joint
 *****
 *****          set the orientation of the joint to rotate its forward vector towards the newly computed vector
 *****
 *****          also compute the new vector that attach the parent joint to this neighbouring joint (and store it somewhere for the renderer to draw it)
 *****      }
 ***** }
 *****
 *****
 ***** Algorithm: Joints Arrangements (END)
 *****
 *****
 ***** Algorithm: setupVectorEmanationTrend (BEGIN) --  VectorEmanationTrend is a class that houses a few DS for specifying the trend of the vectors
 *****                                                  that are emanated from parent joints. Each vector originates from a parent joint and ends up at the child joints
 *****                                                  At the moment the trend of the vectors are mostly localized and is defined by two parameters only:
 *****                                                  >>  Energy of vector (i.e. how far the vector will go)
 *****                                                  >>  Angle of vector (i.e. at what angle the vector will emanate from)
 *****                                                  >>  think of more trends (perhaps globalized trends involving several vectors in vicinity or several joints).
 *****                                                      All this will contribute to the overall modelling DNA of the alien skeleton
 *****
 *****  //  setup VectorEnergyAngleTrend DS  // an M x 4 array. M = number of joints in the alien.
 *****  //  In the columns we have:
 *****  //  1st column = newly calculated energy. 2nd column = energy trend (ASCENDING, DESCENDING, WAVY UP, WAVY DOWN, STABLE)
 *****  //  3rd column = newly calculated angle. 4th column = ANGLE trend (ASCENDING, DESCENDING, WAVY UP, WAVY DOWN, STABLE)
 *****  for (i goes through all the joints)
 *****  {
 *****      if (ith joint is the central joint)
 *****      {
 *****          column 0 = ith joint
 *****          column 1 = negative number
 *****          column 2 = STABLE
 *****          column 3 = negative number
 *****          column 4 = STABLE
 *****      }
 *****      else
 *****      {
 *****          column 0 = ith joint
 *****          column 1 = previous energy
 *****          column 2 = randomly choose b/w (ASCENDING, DESCENDING, WAVY, STABLE)
 *****          column 3 = previous angle
 *****          column 4 = randomly choose b/w (ASCENDING, DESCENDING, WAVY, STABLE)
 *****      }
 *****  }
 *****
 *****  initialize the following DS: VectorEnergiesOnThisLevel to be size 0. VectorAnglesOnThisLevel to be size 0.
 *****
 *****
 ***** Algorithm: setupVectorEmanationTrend (END)
 *****
 *****
 ***** class: VectorEmanationTrend (BEGIN) - also called JointArrangementTrend
 *****
 ***** member variables:
 *****
 *****      VectorEnergyAngleTrend
 *****      VectorEnergiesOnThisLevel
 *****      VectorAnglesOnThisLevel
 *****
 ***** class: VectorEmanationTrend (END)
 *****
 ***** sub Algorithm: computeJointToJointRelationsDS (BEGIN) -- might not need this algorithm
 ***** input:   skeleton_specs (member of Skeleton class)
 ***** output:  Jnt2JntRelDS (make it member of Skeleton class)
 *****          Jnt2JntRelDS is a data structure (a vector of vectors). Each row refers to the joint in the skeleton_specs and for each joint there are other joints in
 *****          its associated vector that are its neighbours
 *****
 ***** for (each ith joint in skeleton_specs)
 ***** {
 *****      append the joint in the Jnt2JntRelDS
 *****
 *****      for (each neighboring bone)
 *****      {
 *****              go through the entire skeleton_spec list to find another such bone (another nested loop???)
 *****
 *****              if (same bone found) // neighbouring joint found
 *****              {
 *****                  if (this neighboring joint doesn't already appear in the Jnt2JntRelDS)
 *****                  {
 *****                      append this neighboring joint in the vector with the ith joint in Jnt2JntRelDS
 *****                  }
 *****              }
 *****              else
 *****              {
 *****                  continue;
 *****              }
 *****      }
 ***** }
 *****
 ***** sub Algorithm: computeJointToJointRelationsDS (END)
 *****
 *****
 *****
 ***** sub Algorithm: computeJointToJointRelationsDS --- Algorithm 2 (BEGIN)
 ***** input:   tempIntermediateJnt2JntRelDS
 ***** output:  Jnt2JntDS (make it member of Skeleton class)
 *****          Jnt2JntDS is a data structure (a vector of vectors). Each row refers to the joint in the skeleton_specs and for each joint there are other joints in
 *****          its associated vector that are its neighbours
 *****
 *****
 ***** declare a vector of vectors called Jnt2JntRelDS. There must be N rows, where N is the number of joints
 *****
 *****
 ***** for (each i goes from 0 to number of joints)
 ***** {
 *****
 *****      find the next ith joint and append it to the vector for the ith vector in Jnt2JntRelDS
 *****
 *****      Now for all such ith Joint, append the joint in the 2nd column of tempIntermediateJnt2JntRelDS in the Jnt2JntRelDS
 ***** }
 *****
 *****
 ***** sub Algorithm: computeJointToJointRelationsDS --- Algorithm 2 (END)
 *****
 *****
 ***** IDEAS:
 *****      1 - Automatic specification of different parts of the body via assessment of the skeletal proportions
 *****          using some criteria. This will be useful in creating bones
 *****      2 - showing all the different skeletons as thumbnails
 *****
 *****
 *****
 ***** BP_Bulkiness_Trend_RS
 *****
 *****
 ***** Displaying the alien and its body parts in one layer
 *****
 *****
 *****
 *****
 *****
 *****
 *****
 *****
 *****
 ***** Base class and derived class constructors
# include<stdio.h>
# include<iostream>
# include<conio.h>

using namespace std;

class Base{
    public:
        Base(int i, float f, double d): i(i), f(f), d(d)
        {
        }
    virtual void Show()=0;
    protected:
        int i;
        float f;
        double d;
};


class Derived: public Base{
    public:
        Derived(int i, float f, double d): Base( i, f, d)
        {
        }
        void Show()
        {
            cout<< "int i = "<<i<<endl<<"float f = "<<f<<endl <<"double d = "<<d<<endl;
        }
};

int main(){
    Base * b = new Derived(10, 1.2, 3.89);
    b->Show();
    return 0;
}

//+++++++++ THINGS TO DO

Adjust the rotation, translation and scaling methods of geometry algos so that they dont rely on cell's or BP's quaternions or translation matrices

Get rid the orientation and translation matrices for cells and BPs

In the BulkUpBP() method, change the call ScaleCell to ScaleBP

In the createSimpleBone() mehtod, change the calls for rotateCellTowardsVecDir() and scaleCell() to respective rotateBP and scaleBP

*/
