#ifndef SKELETON_RULESET_H
#define SKELETON_RULESET_H

#include <QVector>
#include <QMap>
#include <QPoint>
#include <QPolygonF>
#include <geometryalgorithms.h>


class Skeleton_RuleSet
{
public:
    Skeleton_RuleSet();
    ~Skeleton_RuleSet();
    // Used by client code (AlienModeller) to specify all the skeleton rulesets
    // based on the total number of joints, bones, etc
    void setSkeletonRuleSet(QVector<QPoint> inBrackets);

    //  0 - Number of Joints: min Joints=3, max Joints=100
    //  1 - Number of Bones: min Bones=same as min Joints, max Bones=200 // TODO(ismail): seems irrelevant
    //  Think of more level_0 BPs and their arrangements: exoskeletons, cartlidges, hard tissue
    QVector<QPoint> skeletonSpecsBrackets;

    //
    // ++++ Following are the RULES defined for when the artists selects the type of alien they wants to model ++++
    //      These are dealing with the structure of the Alien's skeleton
    //

    // Each QPolygon is made up of a pair of values. Each pair correspond to a specific alien type, where the index identifies the alien_type.
    // ... Each pair is: (MIN of this BP, MAX of this BP). Here key(int) is the BP_TYPE_ID
    QMap<int, QPolygon> numOfSkeletalBodyParts_RS;
    void initNumOfSkeletalBodyParts_RS(); // default it to the original version
    void addNewBP_TypeEntryToNumOfSkeletalBodyParts_RS(int BP_TypeID, const QPolygon &newEntryPoly);
    void setNumOfSkeletalBodyParts_RS(QMap<int, QPolygon> numOfSkeletalBodyParts_RS);
    void modifyNumOfSkeletalBodyParts_RS(int alienType, QMap<int, QPair<int,int>> tempBP_AmntMap);
    void modifyMinMaxJntsForBodyParts_RS(int alienType, QMap<int, QPoint> tempJnts_AmntMap);
    QMap<int, QPolygon> getNumOfSkeletalBodyParts_RS();

    // num of joints for each BP. The index of the pair is the Alien_Type_ID. Each QPolygon is... (Min Joints, Max Joints)
    QMap<int, QPolygon> numOfJointsPerBP_RS;
    void initNumOfJointsPerBP_RS();
    void setNumOfJointsPerBP_RS(QMap<int, QPolygon> numOfJointsPerBP_RS);
    void addNewEntryToNumOfJointsPerBP_RS(int BP_TypeID, const QPolygon &newEntryPoly);

    // Joint Arrangement Trends Rulesets for each alien type
    // key(int) is the BP_Type_ID
    // ... These rulesets specify the angle and energy via which each joint will be created from its parent joint.
    // ... The parent and child joint relationships are always stored in Jnt2JntRelations DS in AlienModeller class.
    // ... Total 12 pairs. First six pairs correspond to energy trends likelihood, next six pairs correspond to angle trends likelihood,
    // ......   13th pair correspond to minimum and maximum starting enegy.
    // ......   14th pair correspond to minimum and maximum starting angle.
    // ......   (TODO: ismail) perhaps 15th pair for bone scaling trend (ascending/descending/constant/varying)
    // ......   (TODO: ismail) perhaps 16th pair for bone scaling multiplier/factor
    // ......   (TODO: ismail) perhaps 17th pair for X or Y scaling for bone
    QMap<int, QPolygon> jointsArrangementTrends_RS; // caters for all alien types
    void initJointsArrangementTrends_RS();
    void setJointsArrangementTrends_RS(QMap<int, QPolygon> jointsArrangementTrends_RS);
    // valuePoly points are as follows:
    //  Point 1: (ID of energy trend, ID of angle trend)
    //  Point 2: (min energy multiplier, max energy multiplier)
    //  Point 3: (min angle multiplier, max angle multiplier)
    void modifyJointsArrangementTrends_RS(int BP_Type, QPolygon valuePoly);
    QMap<int, QPolygon> getJointsArrangementTrends_RS();

//    QMap<int, QPolygon> bipedal_JAT_RS;
//    QMap<int, QPolygon> quadpedal_JAT_RS;
//    QMap<int, QPolygon> amphibious_JAT_RS;
//    QMap<int, QPolygon> aquatic_JAT_RS;
//    QMap<int, QPolygon> octapedal_JAT_RS;
//    QMap<int, QPolygon> anthropodal_JAT_RS;
    // QMap<int, QPolygon> aerial_JAT_RS;
    // blobby creatures

    // BP size -- compared to back bone size
    QMap<int, QPolygon> BP_SizeComparedToBackBone_RS;
    void initBP_SizeComparedToBackBone_RS();
    void setBP_SizeComparedToBackBone_RS(QMap<int, QPolygon> BP_SizeComparedToBackBone_RS);

    // BP angle arrangement trends -- angle range that will be randomly specified for the DNA, at which another BP will attach to the BackBone
    // Key(int) is the BP_Type_ID
    // ... Each pair of QPOlygon is (Min Angle, Max Angle). The index of the pair is the Alien_Type_ID.
    QMap<int, QPolygon> BP_AngleArrangementsFromBackBone_RS;
    void initBP_AngleArrangementsFromBackBone_RS();
    void setBP_AngleArrangementsFromBackBone_RS(QMap<int, QPolygon> BP_AngleArrangementsFromBackBone_RS);

    // BP to Grow on other BPs (should be a part of Alien_Ruleset class. This class could be an engulfing class that contains Skeleton_Ruleset,
    // ... Muscle_Ruleset, and Level_2_Feature_Rulset)
    // ... The polygon is made of several pairs, each correspond to another BP that can be attached on this BP (the index refers to the BP_TYPE_ID).
    // ...... Each pair is made up of (MIN BPs that can grow, MAX BPs that can grow)
    // ...... If the index does not correspond to the BP_TYPE_ID, then put a (-9,-9) for that pair.
    QMap<int, QPolygon> bipedal_OtherBPsToGrow_RS;
    QMap<int, QPolygon> quadpedal_OtherBPsToGrow_RS;
    QMap<int, QPolygon> amphibious_OtherBPsToGrow_RS;
    QMap<int, QPolygon> aquatic_OtherBPsToGrow_RS;
    QMap<int, QPolygon> octapedal_OtherBPsToGrow_RS;
    QMap<int, QPolygon> anthropodal_OtherBPsToGrow_RS;

    // BP_AttachmentToBackBone_ZoneAndScattering Ruleset
    // Outer Map: key(int) is the BP_TYPE_ID other than BONE and JOINT
    // Nested map: Key(int) is the Alien_Type_ID, QPolygonF has 10 points
    // First 5 points = (Zone_id, likelihood). Next 5 points = (min scattering %age, max scattering %age)
    // Zones:   These refer to: (BackBone Joint Zone, likelihood). BackBone Joint Zone ==> 1=start zone, 2=end zone, 3=upper middle zone, 4=dead middle zone,
    // ...      5=lower middle zone.
    // ...      These zones specify where and at what angle the other BPs will most likely be attached to the Back Bone of the ALien
    // TODO: Simplify this RS. Check if we really need an Alien ID
    QMap<int, QMap<int, QPolygonF>> BP_AttachmentToBackBone_ZoneAndScattering_RS;
    void initBP_AttachmentToBackBone_ZoneAndScattering_RS();
    void modifyBP_AttachmentToBackBone_ZoneAndScattering_RS(int BP_TypeID, int alienTypeID, QPolygonF inputAttmntRS_Poly);

    // This DS is the ruleset for generating the DNA DS GroupBP_AttachmentToAnotherBP_ZoneScatteringTrend_DNA
    // It contains the following four Qpoints:
    // 1st QPoint: (min num of Group BPs, max num of Group BPs)
    // 2nd QPoint: (0,5). Bone Zone ID ==> 0=start zone, 1=end zone, 2=upper middle zone, 3=dead middle zone, 4=lower middle zone
    // 3rd QPoint: (min scattering %age, max scattering %age)
    // 4th QPoint: (0,4). Grouping Trend ID: 0 = Solo Trend, 1 = Leader trend, 2 = Diagonal Trend, 3 = Circular trend
    // 5th QPoint: min and max pitch and yaw angles for attachments
    QPolygon GroupBP_AttachmentToAnotherBP_ZoneScatteringTrend_RS;
    void initGroupBP_AttachmentToAnotherBP_ZoneScatteringTrend_RS();

    // This ruleset suggests how much the bones of a Skeletal BP are bulked up.
    // key(int) is the BP_Type_ID
    // It has the following pairs:
    // First 4 pairs: (Bulkiness Trend ID, likelihood)
    // ... Bulkiness Trend ID is one of the following (0=irregular, 1=ascending, 2=descending, 3=constant)
    // ... Example of Likelihood can be (0,4), (1,4), (2,6), (3,2)
    // 5th pair: (min number of sub cells of bones, max number of sub cells of bones)
    // 6th pair: (min starting bone scaling=1.0, max starting bone scaling=6.0)
    // 7th pair: (min scale multiplier=1.0, max scale multiplier=1.15)
    // 8th pair: (-100, 100)   ==> -100 to -80 = scaling on the Y axis (Y multiplies with scaling factor, X remains unchanged)
    //                          ==> -80 to -60 = scaling on the X axis (X multiplies with scaling factor, Y remains unchanged)
    //                          ==> -60 to 100 = scaling on both X and Y
    QMap<int, QPolygonF> Skeletal_BP_Bulkiness_RS;
    void initSkeletal_BP_Bulkiness_RS();
    void setSkeletal_BP_Bulkiness_RS(QMap<int, QPolygonF> Skeletal_BP_Bulkiness_RS);
    //  Point 0: (BP_Type ID, Bulkiness Trend ID)
    //  Point 1: (min start scale value, max start scale value)
    //  Point 2: (min scale multiplier, max scale multiplier)
    //  Point 3: (X+Y axis flag, X axis flag)
    void modifySkeletal_BP_Bulkiness_RS(QPolygonF modPoly);
    // A map that gives us the code/ID for the bulkiness trends from a string
    // This map gets the keys as strings from the GUI items
    QMap<QString,int> BulkinessTrendsMap;
    void initBulkinessTrendsMap();

    // This ruleset suggests how much the subdiv cells of the bones of BP are created and bulked up.
    // There are 11 Pairs in total
    // P0: (min and max num of subdiv cells)
    // P1: (0,2) SubDiv Points to be found at equal distance or varying distance?...0=equal dist; 1=varying dist)
    // P2: (min and max value for varying distance amount to find the points for subdiv cells)
    // P3: (0, 6) ... Bulkification Trend
    // P4: (min and max Starting Scale Value for Bulkification)
    // P5: (min and max Bulkification Scale Multiplier)
    // P6: (min and max %age of Ascending then Descending OR Asc then Desc)
    // P7: (min and max %age of subdiv cells for generating stacked densers on one of the faces of the subdiv cells)
    // P8: (Should Scales come out from surrounding faces? 0 OR 1, rotation angle for face's normal vector -> 10 to 75)
    //     ... TODO: Think about which direction the normal vector be rotated
    //     ... 2nd value is 0 to 75 degrees rotation of the normal vector of the chosen face
    // P9: (min and max amount of Strafing) ... 0 to 20 units
    // P10: (min and max for the Magic Number)
    QPolygonF Skeletal_BP_SubDivCells_RS;
    void initSkeletal_BP_SubDivCells_RS();
    void setSkeletal_BP_SubDivCells_RS(QPolygonF Skeletal_BP_SubDivCells_RS);

    // Bone Bending Ruleset
        //  QPolygon:   (min number of bends = 0, max number of bends = 3) i.e. 0 = no bending; 1 = C shaped bend; 2 = S shaped bend
        //              (min % of bone for bend, max % of bone for bend)
        //              (min sharpness of bend, max sharpness of bend) i.e. sharpness refers to the multiplier that will magnify the rotated (90 degrees) unit vector
        //              (min amount of smoothness required, max amount of smoothness required)
    QPolygon BoneBending_RS;
    void initBoneBending_RS();

    // This ruleset specifies the densification parameters for the densification DNA
    // ... for each bone of each BP
    // There are 5 QPoints:
    //  P0            (number of bones acting as pivot bones - at the moment we're only specifying one bone)
    //              ... (1, 2)
    //  P1            (percentage of bones selected on either side of the pivot bone)
    //              ... (30%, 100)
    //  P2            (edge on the left face used for main vector for generating stacked densers, %age of distance to travel on this vector to find the location for all its stacked densers)
    //              ... (0 to 3 i.e. any of the four edges of the left face, 0 to 100%)
    //  P3            (Protrusion level factor, number of densers stacked on top of this base denser)
    //              ... Protrusion level tells how high or low the normal distribution is for densers
    //              ... 1.2 = low protrusion; 1.5 = medium prot ; 1.8 = high prot. IMPORTANT: range b/w 1.1 to 2.5 for some nice results
    //              ... NOTE: a low value like <0.5 makes things very slow and expensive
    //              ... (1.2 OR 1.5 OR 1.8 for protrusion level, 0 to 30 for number of stacked densers)
    //              ... for Head Sub BP originators it could be (4,10)
    //   P4           (X scaling value for the denser lineage)
    //              ... (0.9, 1.0)
    //              for HEAD Sub BP originators it could be (0.9,1.3)
    //   P5           (Y scaling value for the denser lineage)
    //              ... (0.9, 1.0)
    //   P6           (Z scaling value for the denser lineage)
    //              ... (0.9, 1.0)
    //   P7           (roll angle for each denser, Should it mirror?)
    //              ... (0 to 20, 0 to 2)
    //   P8           (should roll angle increase after certain %age of densers?, roll angle change amount)
    //              ... (max 2 here (0 to 2), max 20 - DNA will randomly choose b/w 0 and 20)
    //   P9           (max start %age after which roll angle increases, %age of the remaining densers until the roll angle increase)
    //              ... (20 - max70, 20 - max60)
    //   P10           (should pitch angle change? (Y/N), pitch angle)
    //              ... (max 2 here (0 to 2), max 40 - DNA will randomly choose b/w 0 and 40)
    //   P11          (should yaw angle change after certain %age of densers? (Y/N), yaw angle)
    //              ... (max 2 here (0 to 2), max 40 - DNA will randomly choose b/w 0 and 40)
    //   P12          (should densers swell after certain %age of densers (Y/N), maximum swelling amount i.e. max y-scale factor)
    //              ... (max 2 here (0 to 2), max 10 - DNA will randomly choose b/w 0 and 10)
    //   P13          (max %age after which densers start swelling, %age of the remaining densers until the swelling happens)
    //              ... (20 - max70, 20 - max60)
    //   P14          (min and max %age for moving the mid point of face along the bottom edge vector)
    //              ... (-50,+50)


    //              POTENTIALLY more QPoints
    //              (pitch angle for each denser, yaw angle for each denser)
    //              ... (0 to 20, 0 to 20)
    //              (min number of additional parallel densers from the bone face, max number of additional parallel densers)
    //              ... (0, 3)
    //              ** The following are for LATER **
    //              (can densers generate additional densers?, type of additional densers)
    //              ... Additional Densers Type:
    //              ... (1 - tentacle, 2 - sword, 3 - club shaped, 4 - armour plate denser)

    QPolygonF BP_Densification_RS;
    void initBP_Densification_RS();

    //              (min and max number of bones acting as pivot bones)
    //              ... (1, 2)
    //              (percentage of bones selected on either side of the pivot bone)
    //              ... (90%, 100%)
    //              (edge on the left face used for main vector for generating stacked densers, %age of distance to travel on this vector to find the location for all its stacked densers)
    //              ... (0 to 3 i.e. any of the four edges of the left face, 0 to 100%)
    //              (Protrusion level factor, number of densers stacked on top of this base denser)
    //              ... (1.2 OR 1.5 OR 1.8, 0 to 10)
    //              ... Protrusion level tells how high or low the normal distribution is for densers
    //              ... 1.2 = low protrusion; 1.5 = medium prot ; 1.8 = high prot. IMPORTANT: range b/w 1.1 to 2.5 for some nice results
    //              ... NOTE: a low value like <0.5 makes things very slow and expensive
    //              ... for Head Sub BP originators it could be (4,10)
    //              (min and max starting X scaling value for the denser lineage)
    //              ... (0.9,1.3)
    //              (min and max starting Y scaling value for the denser lineage)
    //              ... (0.9, 1.0)
    //              (min and max starting Z scaling value for the denser lineage)
    //              ... (0.9, 1.3)
    //              (min and max roll angle for each denser, Should it mirror?) // Phase 1 densers will not do 'roll' angle tilting
    //              ... (0 to 20, 0 to 2)  NOTE: if the angle range is 0 to 1 then no roll angle will be applied
    //              (should roll angle change after certain num of densers? (Y/N))
    //              ... (0,2) i.e. 0 = false, 1 = true
    //              (min and max %age of densers after which the roll angle will increase/change)
    //              ... (30%, 100%) i.e. this will only happen if the above value is true
    //              (min and max pitch angle for each denser, min and max yaw angle for each denser) // Phase 1 densers will not do 'yaw' angle tilting
    //              ... (0 to 20, 0 to 20)
    //              (should pitch angle change after certain num of densers? (Y/N))
    //              ... (0,2) i.e. 0 = false, 1 = true
    //              (min and max %age of densers after which the pitch angle will increase/change)
    //              ... (30%, 100%) i.e. this will only happen if the above value is true
    //              (should yaw angle change after certain num of densers? (Y/N))
    //              ... (0,2) i.e. 0 = false, 1 = true
    //              (min and max %age of densers after which the yaw angle will increase/change)
    //              ... (30%, 100%) i.e. this will only happen if the above value is true
    //              ** The following are for LATER **
    //              (min and max number of additional parallel densers from the bone face)
    //              ... (0, 3)
    //              (can densers generate additional densers?, type of additional densers)
    //              ... (1 - tentacle, 2 - sword, 3 - club shaped, 4 - armour plate denser)



    // *** BP_Bulgification_RS **
    // This RS deals with the range values for the parameters for a BP's Bulgification DNA
    //
    // P0: Number of subdiv cells
    // P1: curving allowed on last bone?
    // P2: bulking trend...0=decreasing,1=increasing,2=swellingOnEnds,3=swelling in middle
    // P3: extend subdiv cells at the tip/end of last bone?
    // P4: cell smoothing allowed? // This means a cell will be made like a ball using the method smoothCell
    // P5: x/y/z-scaling-smoothing-factor - to be converted to float
    // P6: curving angle
    // P7: should-curving-angle-change?
    // P8: generate-mounds-on-subdivcells/bone-root-cell?
    // P9: generate-bridges-from-mounds/subcells/bone-root-cell?
    QPolygon BP_Bulgification_RS;
    void initBP_Bulgification_RS();

    QPolygonF HeadSubBP_Densification_RS; // slightly different than BP_Densification_RS
    void initHeadSubBP_Densification_RS();

    // This ruleset specifies the different alignments of the Head sub BPs
    // Possible QPoints are:
    //  QPoint 0 : (min and max displacement along +ve y-axis for maxilla)
    //  QPoint 1 : (min and max displacement along -ve y-axis for mandible)
    //  QPoint 2 : (tilting trend for maxilla...0=Ascending or 1=Descending, Pitch angle for tilting of maxilla...min=0 and max=max angle)
    //  QPoint 3 : (tilting trend for mandible...0=Ascending or 1=Descending, Pitch angle for tilting of maxilla...min=0 and max=max angle)
    //  QPoint 4 : (tilting trend for zygomatic...0=Ascending or 1=Descending, Yaw angle for tilting of maxilla...min=0 and max=max angle)
    //  QPoint 5 : (min and max distance where the zygomatic will be placed from maxilla and mandible's left faces)
    // ... More QPoints for other Sub BPs such as Frontal Bone, Nasal Bone, and Parietal Bone
    QPolygon HeadSubBP_AlignmentRS;
    void initHeadSubBP_AlignmentRS();

    //  P0      (should squeezing happen?, max %age of the total number of bones whose stacked denser will act as the main squeezed lineage)
    //  P1      (min and max of %age of neighbouring denser lineages that will be squeezed alongside this denser lineage)
    //  P2      (min and max starting squeezing amount) i.e. z-scaling amount
    //  P3      (min and max squeezing factor that will be applied to each subsequent z-scaling)
    //  P4      (min and max %age after which stacked densers will have z translation)
    //  P5      (min and max %age amount of travelling on the line for z-translation)
    QPolygonF denserSqueezing_RS;

    // TODO(ismail)
    //  This ruleset deals with changing the curvature of a denser lineage to an irregular curvature
    //  P0  ()
    QPolygon denserLineageRecurvify_RS;

    // A map that gives us the code for the energy and angle trends for the JATs
    // This map gets the keys as strings from the GUI items
    QMap<QString,int> JATsTrendsMap;
    void initJATsTrendsMap();

    //  P0  :   (min %age of neighbouring Jnts, max %age of neighbouring Jnts)
    //  ........(20,40)
    //  P1  :   (min starting stretching energy/strength, max starting stretching energy/strength)
    //  ........(30,300)
    //  P2  :   (min and max stretching energy decreasing/scaling %age)
    //  ........(60,80)

    QPolygon headStretching_RS;
    void intHeadStretching_RS();

    /*
     * Extremity Ruleset - this RS includes rules for the extremity base and digits
     *
     * P0: min and max digits (b/w 0 and 8)
     * P1: min and max carpals (b/w 1 and 4)
     * P2: min and max angle between digits (b/w 10 and 40)
     * P3: // min and max distance between carpals ... these are in %ages
     *     ... The dist shouldn't be too much, as carpals are usually packed closer to each other
     * P4: min and max size compared to parent BP in %age
     * P5: (size increase or decrease ... chose a random number b/w (0,2) ...  0=increase, 1=decrease (MAY NOT BE NEEDED)
     * P6: carpal alignment trend (0,1) .. 0=side-ways
     * P7,P8,P9,P10:   likelihood of    usual extremity (hand, feet, claws; id=0),
     *                                  winged bone (branching; id=1),
     *                                  baseball glove (id=2), and
     *                                  mushroom formations (id=3)
     * Carpal and Digit densification rules
     * add JAT rulesets
     * add numOfJnts ruleset
     * add bulgification rulesets in Skeletal_BP_SubDivCells_RS
     * add bulkiness ruleset in Skeletal_BP_Bulkiness_RS
    */
    QPolygon extremity_RS;
    void intExtremity_RS();

    // This RS deals with identifying which parent BP this extremity will most likely be attached to
    // key(int) is the extremity type ID:   usual extremity (hand, feet, claws; id=0),
    //                                      winged bone (branching; id=1),
    //                                      baseball glove (id=2), and
    //                                      mushroom formations (id=3)
    // QPOlygon is:
    // P0: (BB ID, likelihood)
    // P1: (Lower LImb ID, likelihood)
    // P2: (Upper LImb ID, likelihood)
    QMap<int,QPolygon> extremityAttachment_RS;
    void initExtremityAttachment_RS();

    /*
     * BP_Denisification_RS
     *
     * QPolygon is ...
     *      (%age of bones involved in densification 0 - 4 ... this is multiplied by 20, %age of densers that will mirror 10 - 100)
     *      (%age of densers with center point generations 0 - 10, number of levels of densers 1 - 6)
     *      (min = 0.5, max = 1) X scaling factor
     *      (min = 0.5, max = 1) Y scaling factor
     */

    // mirroring ruleset
    // Bulging ruleset
    // free growth ruleset

    // cell rearrangement(changing attachment position, Roll angle rotation) ruleset
};

#endif // SKELETON_RULESET_H
