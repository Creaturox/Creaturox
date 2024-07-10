#ifndef SKELETON_DNA_H
#define SKELETON_DNA_H

#include "jointsarrangementtrend.h"

#include <QMap>
#include <QPolygonF>
#include <qdebug.h>


class Skeleton_DNA
{
public:
    // TODO(ismail): Must have a copy constructor
    // ... and copy assignment operator

    Skeleton_DNA();
    ~Skeleton_DNA();

    // **** Member Variables ****

    // randomly selected from numOfSkeletalBodyParts_RS
    // key(int) is the BP_TYPE_ID. The value(int) is the number of that BPs to be created for that type
    QMap<int, int> numOfSkeletalBodyParts_DNA;

    // Key(int) is the BP TYPE ID .
    // Each pair in QPolygon is (index starting at 0, numOfJnts) Randomly selected from numOfJointsPerBP_RS
    QMap<int, QPolygon> numOfJointsPerBP_DNA;
    // BP_to_NumOfJoints_Map_DNA ... This DS will allow the alienmodeller to consult how many joints are
    //                               for this BP
//    QMap<int,int> BP_to_NumOfJoints_Map_DNA;

    JointsArrangementTrend skeletal_JATs_DNA;

    // key(int) is the BP_Type_ID (other than the BB, Joints, and Bones). The attachment of all BPs of a particular BP_Type is a collective operation
    // QPair: (zoneJnt_ID on BB, scattering value (%age)).
    // ...  Note that Only one pair is stored for each key (BP Type)
    // TODO(ismail): Make it more general so it can house RS for other BPs as well
    // For example key will be: <childBP_Type,parentBP_Type>
    QMap<int, QPair<float, float>> BP_AttachmentToBackBone_ZoneAndScattering_DNA;

    // This DS is used to compute the attachment specs for Group BPs that
    // ... are attached to another BP
    // Key(QPair) is (Source Group_BP_Type_ID, Destination BP ID)
    // QPolygon is:
    // P0: (Bone Zone ID - This Zone refers to the bone that acts as the pivot bone, number of group BPs to be generated and attached)
    // ... Bone Zone ID ==> 0=start zone, 1=end zone, 2=upper middle zone, 3=dead middle zone, 4=lower middle zone
    // P1: (Scattering %age, Grouping Trend ID)
    // ... Grouping Trend ID: 1) Solo Trend, 2) Leader trend, 3) Diagonal Trend, 4) Circular trend
    // P2: (pitch angle of attachement for source BP, yaw angle of attachement for source BP)
    // P3, P4, P5 are for faces of the cell box: (TP,BM) (LT,RT) (FR,BK) // If any of these are entered as -1 then that face is not considered by the algorithm
    // Group BPs are: Digits, Ribs, thorns, horns, tentacles, fangs, dragon wing bones, teeth, upper limbs (arms)
    QMap<QPair<int,int>, QPolygon> GroupBP_AttachmentToAnotherBP_ZoneScatteringTrend_DNA;

    // Key(QPair) is QPair<ChildBP_ID,ParenBP_ID>
    // QPolygon has 2 QPOints:
    //  P0: (bone index position i.e. the position of bone in the orderedBonesList, face ID)
    //  P1: (u paramter i.e. %age to travel on BOTTOM edge, v parameter i.e. %age to travel on LEFT edge)
    //  ... the u and v paramters allow us to find a point on the face of the bone
    //  P2: (pitch angle, yaw angle)
    QMap<QPair<int,int>, QPolygon> ChildBP_ToParentBP_Attachment_DNA;
    void addOrUpdateNewEntryIn__ChildBP_ToParentBP_Attachment_DNA(int childID, int parentID,
                                                                 int boneIndPos, int faceID,
                                                                 int uParam, int vParam,
                                                                 int pitch, int yaw);

    // This DS can be useful to figure out which BP is to attached to which other BP
    // ... including its attachement location/position and attachment Vector
    // ... This can be useful for bookkeeping when cloning is required
    // Outer Map key(int) is the input child BP_ID other than Joint or Bone
    // Inner Map: key(int) is the parent BP it is attached to i.e. the parent BP
    // QPair<Attmnt Position, Attmnt Vector>
    // Needs to be updated whenever attachments happen
    // TODO(ismail): I believe this should be part of the Skeleton
    // ... because this DS cannot be copied as is when cloning, otherwise it will ruin the cloning
    // There is no bug with this DS, because for any child BP, there will always
    // be only one parent BP
    QMap<int, QMap<int, QPair<QVector3D, QVector3D>>> BP_AttachmentToOtherBP_withAttachPositionAndVector_DNA;

    // This DS will find the face any given BP is attached to and the Bone it is
    // ... attached to
    // key(int) is the BP_ID.
    // QPolygon is only 2 QPoints per BP, where the QPoints are:
    // (Bone ID, Cell ID) (Face ID, dummy Value)
//    QMap<int, QPolygon> BP_AttachmentBoneCellAndFaceFinder_DNA;



    // Size of each BP as compared to BB. randomly selected from BP_SizeComparedToBackBone_RS
    // Key(int) is the BP_Type_ID.
    // Each pair in QPolygon is (index starting at 0, size). Each pair corresponds to a single instance of
    // ... that BP. For example, if there are 3 lower limbs to be created for this alien, then this
    // ... QPolygon will have three pairs for a lower limb entry in the QMap, each with different sizes
    // ... Size values are in percentages. Example LL is 200% of the BB size
    // TODO: Inspect this. This is not really being used so might need to Remove it
    QMap<int, QPolygon> BP_SizeComparedToBackBone_DNA;
    // for bookkeeping for future cloning purposes
    // QMap<int,int> BP_to_sizeComparedToBB_Map_DNA

    // This list contains the angle trends from Backbone for all the BPs to be attached to the BackBone
    // ... We use the normal vector of the face of the attachement joint where the BP is to be attached.
    // ... The angle (this same angle for: Pitch & Yaw) is the angle at which we need to rotate the normal vector's clone.
    // ... That clone vector will then guide the final rotation of the BP
    // ... Key(int) is the BP_Type_ID.
    // ... Each point in the polygon is (index starting at 0, 'angle' that is randomly selected from RS)
    // ... There are as many points as there are BP for that BP Type. For example. If this alien has 4 lower limbs,
    // ... then we will have 4 points.
    QMap<int, QPolygon> BP_AngleArrangementsFromBackBone_DNA;
    // This DS stores the loction and vector of orientation for each BP
    // QMap<int,Qpair<QVector3D, QVector3D>> BP_to_AngleArrangementFromBackBone_Map_DNA


    //  DNA DS to specify the bulkiness of the bones of each skeletal BP (other than the Bone or Joint)
    //  This (bookkeeping DS) can be useful for cloning purposes
    //  key(int) is the BP_ID other than the Bone or Joint
    //  Each QPolygonF has 4x(number of bones for that BP) pairs. i.e. each QPolygon has four QPoints for each bone. E.g. if there are 8 bones
    //  ... then we have 32 pairs
    //  TODO(ismail): Perhaps we don't need to have 4xpairs for each bone,
    //  ... because this can be a global QPolygon for the specific BP.
    //  ... As there is no property here that is specific to the bone.
    //  ... We can simply have one 4xpairs for the first bone and then rest of the bones will follow the same pattern
    //  Each couple of pairs are:
    //  First pair:     (Bulkiness Trend, number of subcells this Bone will be divided into)
    //  Second pair:    (starting scaling factor, scaling multiplier)
    //  Third pair:     (X Scaling Flag, Y Scaling Flag). either 1 or 0. If both are 1, then scaling on both X and Y
    //  Fourth pair:    (Z scaling flag, Z scaling factor) ... Scaling Flag value: -1=no scaling, 0=sqwashing, 1=stretching
    // TODO(Ismail): After the BP is sqwashed, its JAT DNA must also change to facilitate
    // ... perfect cloning
    //  ** Potential Fourth pair: (Should more cells be grown?, how many more cells to grow)
    //  ** potential more pairs...
    //  4th Pair:   (number of bones with same scaling, 0=reduce overall bulkiness, 1=increase overall bulkiness)
    //  5th Pair:   (amount of scaling changes: 0=no changes; 1=small changes; 2=large changes,  offset for scaling multiplier: 0.001=small changes, 0.00=no changes, 0.1=large changes)
    //  6th Pair:   (apply sandwitch densers T/F, sandwitch densers fashion: 0=all bones, 1=bell curve fashion)
    //  7th Pair:   (use descending bulkiness towards end of BP, %age of bones where this descending bulkiness will happen)
    QMap<int, QPolygonF> Skeletal_BP_Bulkiness_DNA_ForEachBP;

    //  DNA DS for the SubDivision Cells of the Bones of the BP. It also includes some of the BULKINESS DNA specs.
    //  key(int) is the BP_ID other than the Bone or Joint
    //  Each QPolygon has 8x(number of bones for that BP) pairs. i.e. each QPolygon has 8 QPoints for each bone. E.g. if there are 8 bones
    //  ... then we have 32 pairs
    //  QPolygonF: ... 8 Pairs in total for each bone
    //  P0:     (Num of SubDivCells, SubDiv Points to be found at equal distance or varying distance?...0=equal dist; 1=varying dist)
    //  P1:     (Varying distance amount, Magic number for varying distance amount)
    //          ... A value for varying distance will shift the point slightly left or right depending on some magic condition
    //          ... Magic number for varying trend is a number between 1 and 100. It alternates b/w magNum /= 1.3 AND magNum *= 2.3
    //          ... If the result (magNum) is odd we shift point left according to varying dist amount, and if the result is even we shift point right according to varying dist amount
    //  P2:     (Bulkification Trend (BT), Bulkification Starting Scaling Value) ... Bulkification Trend is:
    //                                                                          0=Ascending, 1=Descending, 2=Varying
    //                                                                          3=Desc->Asc, 4=Asc->Desc
    //  P3:     (Bulkification Scaling Multiplier, Bulkification Scaling Multiplier Modifier Magic number)
    //          ... Magic number will follow the same calculation as above.
    //          ... An odd result will increase the scaling multiplier, An even result will decrease the scaling multiplier
    //  P4:     (%age of Ascending then Descending BT OR Desc then Asc BT, Magic number for 'varying' Bulk trend) ... Only if the Asc->Desc OR Desc->Asc flag is ON in P2
    //          ... Magic number for varying trend is a number between 1 and 100. It alternates b/w magNum /= 1.3 AND magNum *= 2.3
    //          ... If the result (magNum) is odd we use Ascending BT, and if the result is even we use Descending BT
    //  P5:     (%age of subdiv cells for generating stacked densers on one of the face of the subdiv cells, chosen face for generating stacked densers)
    //          ... At the moment we are only generating 3 stacked densers
    //  P6:     (Should Scales come out from surrounding faces, Angle of scales to come out)
    //  P7:     (Amount of STRAFING of subdiv cells, Magic Number for strafing within subdiv cells)
    //          ... The magic number will follow the same calculations as above.
    //          ... Odd result will do left Strafing and even result will do right strafing
    //  NOTE: Must always be updated when a BP changes its number of bones i.e. increases or decreases joints
    QMap<int, QPolygonF> Skeletal_BP_SubDivCells_DNA;
    // An associated State Machine for sculpting


    // flags indexed as: 0=clone DNA. 1=mutate BP entries.
    //              .... 2=delete BP entries. 3=create new BP entries
    //              .... 4=update BP entry (for e.g. when the BP is extended or shortened i.e. its joints inc OR dec)
    // ... If all entries are False, then the Bulkiness DNA should remain
    // ... unchanged
    QVector<bool> bulkinessDNA_ManipulationFlags;
    QVector<int> BP_listToDeleteFromBulkinessDNA;
    QVector<int> BP_listToMutateInBulkinessDNA;
    QVector<int> BP_listToUpdateInBulkinessDNA;

    // Bone Bending DNA
        // int is the Bone ID;
        //  Each Polygon    (Bone ID to be bent, number of bends i.e either 1 or 2)
        //                  (% of 1st bend, % of 2nd bend)
        //                  (sharpness of 1st bend, sharpness of 2nd bend)
        //                  (smoothness required, amount of smoothness if required)
    QMap<int, QPolygonF> BoneBending_DNA;


    // BP General Densification DNA
    // key(int) is the BP ID other than Bone and Joint
    // QPolygonF is exactly the same as copied from the Ruleset
    // TODO: Check that if we're copying the same entry for each BP in this map, there is lots of redundancy
    QMap<int, QPolygonF> BP_GenDensificationDNA;

    // BP Bone Specific Densifying DNA
        // Outer map: key(int) is the BP ID (other than Joint or Bone)
        // Each inner QMap is:
        // key(int) is the Bone ID
        // QPolygon is:
        //   P0           (Is this a pivot bone?, number of bones selected on either sides)
        //                Potential entry: (is this bone a hanging denser spawner, -1) hanging densers are densers that are grown from either bottom or top face of the bone
        //   P1           (edge on the left face used for main vector for generating stacked densers, %age of distance to travel on this vector to find the location for all its stacked densers)
        //   P2           (Protrusion level factor, number of densers stacked on top of this base denser)
        //   P3           (starting X scaling value for lineage, change in scaling value on lineage)
        //   P4           (starting Y scaling value for lineage, change in scaling value on lineage)
        //   P5           (starting Z scaling value for lineage, change in scaling value on lineage)
        //   P6           (start roll angle for lineage, Should it mirror? 1 or 0)
        //   P7           (should roll angle increase after certain %age of densers? (Y/N), roll angle increment amount)
        //   P8           (%age after which roll angle increases, %age of the remaining densers until the roll angle increase)
        //   P9           (should pitch angle change? (Y/N), pitch angle)
        //   P10          (should yaw angle change? (Y/N), yaw angle)
        //   P11          (should densers swell after certain %age of densers (Y/N), maximum swelling amount)
        //   P12          (%age after which densers start swelling - min20 - max70, %age of the remaining densers until the swelling happens - min20 - max60)
        //                ... Swelling means that the y-scaling will change
        //   P13          (%age of the moving of the mid point of the face along the bottom edge vector - b/w -50 and +50, -1)
        //                ... This is used to move the stacked densers up and down the z-axis
        // ** Additional **
        //   P14          (Flattening effect: %age after which the z-scaling stops changing i.e z-scaling = 1, %age after which the x-scaling stops changing)
        //   P15          (%age after which the y-scaling stops changing, -1)

        //              OTHER POTENTIAL Points...
        //              min and max (%age of stacked densers after which the pitch and yaw angles will change)
        //              min and max (%age of stacked densers after which the densers will get closer to the BP)
    QMap<int, QMap<int, QPolygonF>> BP_SpecificDensificationDNA;

    /*  outer map: key(int) is the BP_ID. Inner map: key(int) is the bone ID
     *
     *  P0: (Number of subdiv cells,    curving allowed on last bone?)
     *  P1: (bulking trend...0=decreasing,1=increasing,2=swellingOnEnds,3=swelling in middle,   extend subdiv cells at the tip/end of last bone?)
     *  P2: (subdiv cell smoothing allowed?, x-scaling-smoothing-factor)
     *  P3: (y-scaling-smoothing-factor, z-scaling-smoothing-factor)
     *  P4: (curving angle, should-curving-angle-change?)
     *  P5: (generate-mounds-on-subdivcells?, generate-bridges-from-mounds/subcells/bone-root-cell?)
    */
    // TODO: This DNA must be clonable just like the densification DNA
    QMap<int,QMap<int,QPolygon>> BP_BulgificationDNA;
    // QMap<int,QPolygon> BP_BulgLastBoneGrowth_DNA; // for generating the stacked densers on the last bone's FRONT face

    // BP General Densification DNA for Head
    // key(int) is the Head ID other than Bone and Joint
    // QPolygonF is the same as copied from the Ruleset
    QMap<int, QPolygonF> HeadSubBP_GenDensification_DNA;

    // Outer map: key(int) is the Head Sub BP ID
    // Each inner QMap is:
    // key(int) is the Bone ID
    // QPolygon is:
    // P0             (Is this a pivot bone?, number of bones selected on either sides)
    // P1             (edge on the left face used for main vector for generating stacked densers, %age of distance to travel on this vector to find the location for all its stacked densers)
    // P2             (Protrusion level factor, number of densers stacked on top of this base denser)
    // P3             (starting X scaling value for lineage, change in scaling value on lineage)
    // P4             (starting Y scaling value for lineage, change in scaling value on lineage)
    // P5             (starting Z scaling value for lineage, change in scaling value on lineage)
    // P6             (roll angle for lineage, Should it mirror? 1 or 0)
    // P7             (should roll angle change after certain num of densers? (Y/N), %age of densers after which the roll angle will increase/change)
    // P8             (pitch angle for lineage, yaw angle for lienage)
    // P9             (should pitch angle change after certain num of densers? (Y/N), %age of densers after which the pitch angle will increase/change)
    // P10            (should yaw angle change after certain num of densers? (Y/N), %age of densers after which the yaw angle will increase/change)
    // ** POTENTIAL more points
    // P11            (number of additional parallel densers from the bone face)
    QMap<int, QMap<int, QPolygonF>> HeadSubBP_SpecificDensification_DNA;

    // This DNA specifies the alignment of the different sub BPs of the Head
    // key(int) is the Head ID
    // QPolygon is:
    // QPoint 0: (displacement along +ve y-axis for maxilla, displacement along -ve y-axis for mandible)
    // QPoint 1: (Pitch angle trend for maxilla, Pitch angle for tilting of maxilla)
    // QPoint 2: (Pitch angle trend for mandible, Pitch angle for tilting of mandible)
    // QPoint 3: (Yaw angle trend for mandible, Yaw angle for tilting of mandible)
    // QPoint 4: (distance where the zygomatic will be placed from left faces of maxilla and mandible,-1)
    QMap<int, QPolygon> headSubBP_AlignmentDNA;

    //  key(int) is the head ID
    //  QPlygonF is:
    //  P0  (should squeezing happen, %age from 0 to 100 that will determine which bone index is selected for main desner lineage squeezing)
    //  P1  (%age determining the number of denser lineages to be selected on either side of the main denser lineage, -1)
    //  P2  (starting squeezing z-value, squeezing factor for z-scaling i.e. its like scaling multiplier)
    //  P3  (%age determining the index of the cell denser from which the z-translation will occur, %age amount of travelling on the line for z-translation)
    QMap<int, QPolygonF> denserSqueezingDNA;

    // This DNA DS is useful for branching out other sub BPs from the parent BP
    // Key(int) here is the BP_ID.
    // Each Point in QPolygon has as many points as there are BP Types
    // ... Each Point is: Point (BP_TYPE_ID, number of BPs)
    QMap<int, QPolygon> OtherBPsToGrow_DNA; // MIGHT NOT NEED IT


    // ******* TEMPORAL DNA members ********

    // FREE GROWTH DNA (conceptual)
    /*
     * QMap<int,QPolygon>
     * int can be the FreeGrowthID
     * QPolygon ...
     *              (BP_ID, Bone_ID) (Number of jnts in lineage, mirroring? T/F)
     *              (Multiple Growths? T/F, %age of bones involved on either side of starting bone)
     *              (Same JATs? T/F, Same Bulkiness? T/F)
     *              (Form Bridge with other BP's bone? T/F, Maximum Distance for bridging)
     */

    //  ** Bone Bulging DNA **  // PERHAPS NOT NEEDED as all these specs are present in the Skeletal_BP_SubDivCells_DNA
    //  key(int) is the Bone ID
    //  QPolygon is:
    //  P0  :   (number of cells this bone is divided into, %age of division of bone)
    //  P1  :   (starting scale factor of middle cell, scaling multiple of neighbouring cells)
    //  P2  :   (roll angle for each bone, face to ignore for generation of 'BULGERS')
    QMap<int, QPolygonF> boneBulginessDNA;

    //  ** Head Stretching DNA **
    //  key(int) is the Head ID
    //  QPolygon is:
    //  P0  :   (Pivot Jnt Index...usually it is 0, %age of neighbouring joints)
    //  P1  :   (starting stretching energy/strength of pivot joint, stretching energy decreasing/scaling %age)
    QMap<int, QPolygon> headStretchingDNA;

    /*
     * Extremity DNA - this DNA includes specs for the extremity base and digits
     * key(int) is the extremity ID
     * P0: (number of digits, number of carpals)
     * P1: (angle between digits, distance between carpals)
     * P2: (size compared to parent BP in %age, size increase or decrease)
     * P3: (carpal alignment trend,-1)
     * Carpal and Digit densification DNA
     * add JAT DNA
     * add numOfJnts DNA
     * add bulgification DNA in Skeletal_BP_SubDivCells_DNA
     * add bulkiness DNA in Skeletal_BP_Bulkiness_DNA_ForEachBP
    */
    QMap<int, QPolygon> extremityBP_DNA;

    // This state machine will model a new and varied kind of extremity BP
    // Key(int) is the state ID. There are 4 states at the moment.
    // ... State 0 : Create Carpal BP
    // ... State 1 : Create & Attach Digit to Carpal
    // ... State 2 : Create & Attach Digit to Digit
    // ... State 3 : Increase Jnts for Carpal
    // ... State 4 : Increase Jnts for Digit
    // QPolygon are the states (stored in pairs (Sn,Sm)) that are directly reachable from the key state
    // ... For example: If state S2 is directly connected to S1, S3 and S4 then we have QPOints as (1,3) , (4,-1)
    QMap<int,QPolygon> extremityBP_StateMachine;
    void initializeExtremityBP_StateMachine();

    // This state machine will model a new and varied kind of extremity BP
    // Key(int) is the state ID. There are 4 states at the moment.
    // ... State 0 : Select a carpal at random
    // ... State 1 : Generate mound cells on selected carpal
    // ... State 2 : Join nearby mound cell (to bridge two mounds)
    // ... State 3 : Align carpals in curvy fashion
    // ... State 4 : Create webs or bridges b/w two carpals
    // QPolygon are the states (stored in pairs (Sn,Sm)) that are directly reachable from the key state
    // ... For example: If state S2 is directly connected to S1, S3 and S4 then we have QPOints as (1,3) , (4,-1)
    QMap<int,QPolygon> extremityBP_ScultpingStateMachine;
    void initializeExtremityBP_ScultpingStateMachine();



    // This state machine will model a new and varied kind of extremity BP
    // Key(int) is the state ID. There are 4 states at the moment.
    // ... State 0 : Make base head BP
    // ... State 1 : Generate cell lineage from a random point
    // ... State 2 : Make a lone cell
    // ... State 3 : Mirror Cell Lineage around a plane
    // ... State 4 : Rotate cell
    // ... State 5 : Attach Extremity Digit to a cell/head
    // ... State 6 : Smooth cell
    // ... State 7 : Clone/split cell & translate all cells to a corner/side by side
    // ... State 8 : Interwine cell lineage
    // ... State 9 : Make a bridge from one cell to another
    // ... State 10 : Explode Cell
    // Additional States (Secondary)
    // ... State 11 : Extend cell lineage
    // ... State 12 : Make webs b/w two nearby lineages

    // QPolygon are the states (stored in pairs (Sn,Sm)) that are directly reachable from the key state
    // ... For example: If state S2 is directly connected to S1, S3 and S4 then we have QPOints as (1,3) , (4,-1)
    QMap<int,QPolygon> headStateMachine;
    void initializeheadStateMachine();
    QPolygon headStateMachineSelectionDNA;


    // BP Mirroring DNA
    // BP Smoothness DNA - to smooth out a subset of the bones in the BP
    // BP free growth DNA - Should more cells be grown?, how many more cells to grow

    // The following DS may be used for bookkeeping when cloning is required
    // *** Might Not Needed ***
    // QMap<int, QVector3D> BP_to_BackBoneAttachmentLocation_Map_DNA
};

#endif // SKELETON_DNA_H


//  ************ LEGACY DS - PERHAPS NOT NEEDED *************
//  DNA DS to specify the bulkiness of the bones of a skeletal BP
//  key(int) is the BP_ID
//  Each QPolygon has 2x(number of bones for that BP) pairs. i.e. each polygon has two pairs for each bone. E.g. if there are 8 bones
//  ... then we have 16 pairs
//  Each couple of pairs are:
//  First pair:     (Bulkiness Trend, Scaling multiplier)
//  Second pair:    (X Scaling Flag, Y Scaling Flag). either 1 or 0. If both are 1, then scaling on both X and Y
//   QMap<int, QPolygonF> Skeletal_BP_Bulkiness_DNA; //  ************ LEGACY DS - PERHAPS NOT NEEDED *************

//    // ******* NOT NEEDED *******
//    // This DNA DS is created and updated when each joints position is being computed inside the buildAlienSkeleton_TypeGuided()
//    // It stores the energy trend ID and angle trend ID of each Joint for a particular skeletal BP
//    // Each QPolygonF contains as many pairs as the number of joints in the skeletal BP
//    // int is the BP_ID.
//    // Each two pairs(QPoint) is (current energy trend ID, current angle trend ID)
//    // This DS is stored in the system so that when the next clone of created, it copies everything from this and can change anything willingly
//    QMap<int, QPolygon> BP_JAT_Energy_Angle_Per_Joint_DNA;

//    QMap<int, QPolygonF> BP_JAT_allJoints_DNA;
