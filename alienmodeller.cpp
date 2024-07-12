#include "alienmodeller.h"
#include "ridge.h"
#include <random>


AlienModeller::AlienModeller()
{
    srand (time(NULL));

    AlienID_Counter = -1;

    // create an object of TorsoModeller that will be alive throughout the program
//    torsoModellerPtr = new TorsoModeller();

    // instantiate all the rulesets
    boneRuleSet = new Bone_RuleSet();
    jointRuleSet = new Joint_RuleSet();
    skeletonRuleSet = new Skeleton_RuleSet();
    ridgeRuleSetPtr = new Ridge_Ruleset();

    initAlienTypeMap();
    initBP_TypeMap();
    setColorMap();
    initializeBP_TypeColorMap();
    initializeAllRulesets();

    selectedExtremBP_InGLWidgets = QVector3D(-1,-1,-1);
}

AlienModeller::~AlienModeller()
{
    qDebug() << "Deleting AlienModeller";
    for (int i=0; i<alienList.length(); i++)
    {
        qDebug() << alienList.length() << " aliens to delete";
        if (alienList[i])
        {
            qDebug() << "Deleting Alien";
            qDebug() << alienList[i]->bodyParts.length();
            // add all the BPs from other lists to the main BP list so these are deleted all together
            // ... by the removeAlienBodyParts() method
            for (int j=0; j<alienList[i]->cloneBPs.length(); j++)
            {
                if (!alienList[i]->bodyParts.contains(alienList[i]->cloneBPs[j]))
                    alienList[i]->bodyParts.append(alienList[i]->cloneBPs[j]);
            }
            for (int j=0; j<alienList[i]->seperateCloneBPs.length(); j++)
            {
                if (!alienList[i]->bodyParts.contains(alienList[i]->seperateCloneBPs[j]))
                    alienList[i]->bodyParts.append(alienList[i]->seperateCloneBPs[j]);
            }
            // BPs need to be removed from the smoothingBackupCloneBPs list as well
            QMapIterator<int, BodyPart*> iter(alienList[i]->smoothingBackupCloneBPs);
            while (iter.hasNext())
            {
                alienList[i]->bodyParts.append(iter.next().value());
            }

            qDebug() << alienList[i]->bodyParts.length();
            QVector<int> tempBP_IDs; tempBP_IDs.clear();
            QVector<int> tempSepBP_IDs; tempSepBP_IDs.clear();
            getAllBP_ID_List(alienList[i], tempBP_IDs, tempSepBP_IDs);
            removeAlienBodyParts(alienList[i], tempBP_IDs);

            alienList[i]->cloneBPs.clear();
            alienList[i]->seperateCloneBPs.clear();
            alienList[i]->smoothingBackupCloneBPs.clear();

            delete alienList[i];
            if (alienList[i])
                alienList.removeAt(i--);
        }
    }
    alienList.clear();
    aliensToCloneList.clear();

    alienBPsToClone.clear();
    seedJoints.clear();
    seedBones.clear();

    qDebug() << "AlienModeller deleted";
}

void AlienModeller::initializeSkeletonRuleset()
{
    qDebug() << "initializing Skeleton Ruleset";

    // initialize the numOfSkeletalBodyParts_RS
    QPolygon tempPolygon;
    // LOWER LIMB = 2 (BP_TYPE_ID)
    tempPolygon << QPoint(2,4)/*Biped*/ << QPoint(2,4)/*Quadped*/ << QPoint(3,8)/*Aquatic*/
                << QPoint(2,5)/*Amphibious*/ << QPoint(4,10)/*Octapedal*/ << QPoint(6,10)/*Anthropodal*/;
    skeletonRuleSet->numOfSkeletalBodyParts_RS.insert(2, tempPolygon);

    // UPPER LIMB = 3
    tempPolygon.resize(0);
    tempPolygon << QPoint(1,6)/*Biped*/ << QPoint(2,8)/*Quadped*/ << QPoint(0,3)/*Aquatic*/
                << QPoint(0,6)/*Amphibious*/ << QPoint(0,6)/*Octapedal*/ << QPoint(12,14)/*Anthropodal*/;
    skeletonRuleSet->numOfSkeletalBodyParts_RS.insert(3, tempPolygon);

    // BACK BONE = 4
    tempPolygon.resize(0);
    tempPolygon << QPoint(1,2)/*Biped*/ << QPoint(1,2)/*Quadped*/ << QPoint(1,2)/*Aquatic*/
                << QPoint(1,2)/*Amphibious*/ << QPoint(1,2)/*Octapedal*/ << QPoint(1,2)/*Anthropodal*/;
    skeletonRuleSet->numOfSkeletalBodyParts_RS.insert(4, tempPolygon);

    // TAIL = 5
    tempPolygon.resize(0);
    tempPolygon << QPoint(0,1)/*Biped*/ << QPoint(0,1)/*Quadped*/ << QPoint(0,1)/*Aquatic*/
                << QPoint(0,1)/*Amphibious*/ << QPoint(0,1)/*Octapedal*/ << QPoint(0,1)/*Anthropodal*/;
    skeletonRuleSet->numOfSkeletalBodyParts_RS.insert(5, tempPolygon);

    // HEAD = 6
    tempPolygon.resize(0);
    tempPolygon << QPoint(0,1)/*Biped*/ << QPoint(0,1)/*Quadped*/ << QPoint(0,1)/*Aquatic*/
                << QPoint(0,1)/*Amphibious*/ << QPoint(0,1)/*Octapedal*/ << QPoint(0,1)/*Anthropodal*/;
    skeletonRuleSet->numOfSkeletalBodyParts_RS.insert(6, tempPolygon);

    // NECK = 9
    tempPolygon.resize(0);
    tempPolygon << QPoint(0,1)/*Biped*/ << QPoint(0,1)/*Quadped*/ << QPoint(0,1)/*Aquatic*/
                << QPoint(0,1)/*Amphibious*/ << QPoint(0,1)/*Octapedal*/ << QPoint(0,1)/*Anthropodal*/;
    skeletonRuleSet->numOfSkeletalBodyParts_RS.insert(9, tempPolygon);

    // RIB = 10
    tempPolygon.resize(0);
    tempPolygon << QPoint(0,1)/*Biped*/ << QPoint(0,1)/*Quadped*/ << QPoint(0,1)/*Aquatic*/
                << QPoint(0,1)/*Amphibious*/ << QPoint(0,1)/*Octapedal*/ << QPoint(0,1)/*Anthropodal*/;
    skeletonRuleSet->numOfSkeletalBodyParts_RS.insert(10, tempPolygon);

    // Maxilla = 11
    // This represent the min and max maxillas to be created for each Head of the Alien.
    tempPolygon.resize(0);
    tempPolygon << QPoint(0,1)/*Biped*/ << QPoint(0,1)/*Quadped*/ << QPoint(0,1)/*Aquatic*/
                << QPoint(0,1)/*Amphibious*/ << QPoint(0,1)/*Octapedal*/ << QPoint(0,1)/*Anthropodal*/;
    skeletonRuleSet->numOfSkeletalBodyParts_RS.insert(11, tempPolygon);

//    tempPolygon << QPoint(1,2)/*Biped*/ << QPoint(1,2)/*Quadped*/ << QPoint(1,2)/*Aquatic*/
//                << QPoint(1,2)/*Amphibious*/ << QPoint(1,2)/*Octapedal*/ << QPoint(1,2)/*Anthropodal*/;

    // Mandible = 12
    // This represent the min and max mandibles to be created for each Head of the Alien.
    tempPolygon.resize(0);
    tempPolygon << QPoint(0,1)/*Biped*/ << QPoint(0,1)/*Quadped*/ << QPoint(0,1)/*Aquatic*/
                << QPoint(0,1)/*Amphibious*/ << QPoint(0,1)/*Octapedal*/ << QPoint(0,1)/*Anthropodal*/;
    skeletonRuleSet->numOfSkeletalBodyParts_RS.insert(12, tempPolygon);

    // Zygomatic = 13
    // This represent the min and max zygomatic to be created for each Head of the Alien.
    tempPolygon.resize(0);
    tempPolygon << QPoint(0,1)/*Biped*/ << QPoint(0,1)/*Quadped*/ << QPoint(0,1)/*Aquatic*/
                << QPoint(0,1)/*Amphibious*/ << QPoint(0,1)/*Octapedal*/ << QPoint(0,1)/*Anthropodal*/;
    skeletonRuleSet->numOfSkeletalBodyParts_RS.insert(13, tempPolygon);

//    // HAND = 7
//    tempPolygon.resize(0);
//    tempPolygon << QPoint(2,4)/*Biped*/ << QPoint(2,4)/*Quadped*/ << QPoint(0,8)/*Aquatic*/
//                << QPoint(4,6)/*Amphibious*/ << QPoint(4,10)/*Octapedal*/ << QPoint(6,10)/*Anthropodal*/;
//    skeletonRuleSet->numOfSkeletalBodyParts_RS.insert(7, tempPolygon);



    // initialize the numOfJointsPerBP_RS
    tempPolygon.resize(0);
    // LOWER LIMB = 2 (BP_TYPE_ID)
    tempPolygon << QPoint(3,5)/*Biped*/ << QPoint(3,8)/*Quadped*/ << QPoint(3,10)/*Aquatic*/
                << QPoint(4,5)/*Amphibious*/ << QPoint(4,7)/*Octapedal*/ << QPoint(3,5)/*Anthropodal*/;
    skeletonRuleSet->numOfJointsPerBP_RS.insert(2, tempPolygon);

    // UPPER LIMB = 3
    tempPolygon.resize(0);
    tempPolygon << QPoint(3,5)/*Biped*/ << QPoint(3,8)/*Quadped*/ << QPoint(3,8)/*Aquatic*/
                << QPoint(3,7)/*Amphibious*/ << QPoint(3,7)/*Octapedal*/ << QPoint(3,4)/*Anthropodal*/;
    skeletonRuleSet->numOfJointsPerBP_RS.insert(3, tempPolygon);

    // BACK BONE = 4
    tempPolygon.resize(0);
    tempPolygon << QPoint(6,20)/*Biped*/ << QPoint(10,30)/*Quadped*/ << QPoint(7,20)/*Aquatic*/
                << QPoint(10,30)/*Amphibious*/ << QPoint(10,30)/*Octapedal*/ << QPoint(15,21)/*Anthropodal*/;
    skeletonRuleSet->numOfJointsPerBP_RS.insert(4, tempPolygon);

    // TAIL = 5
    tempPolygon.resize(0);
    tempPolygon << QPoint(2,7)/*Biped*/ << QPoint(2,7)/*Quadped*/ << QPoint(5,21)/*Aquatic*/
                << QPoint(2,7)/*Amphibious*/ << QPoint(3,11)/*Octapedal*/ << QPoint(5,21)/*Anthropodal*/;
    skeletonRuleSet->numOfJointsPerBP_RS.insert(5, tempPolygon);

    // HEAD = 6
    tempPolygon.resize(0);
    // RS for head modelling using state machine (using boxes)
    tempPolygon << QPoint(4,8)/*Biped*/ << QPoint(10,30)/*Quadped*/ << QPoint(7,20)/*Aquatic*/
                << QPoint(10,30)/*Amphibious*/ << QPoint(10,30)/*Octapedal*/ << QPoint(15,21)/*Anthropodal*/;
    // RS for head modelling using cell lineages
    // tempPolygon << QPoint(6,20)/*Biped*/ << QPoint(10,30)/*Quadped*/ << QPoint(7,20)/*Aquatic*/
    //             << QPoint(10,30)/*Amphibious*/ << QPoint(10,30)/*Octapedal*/ << QPoint(15,21)/*Anthropodal*/;
    // RS for head modelling using densers
    // tempPolygon << QPoint(30,40)/*Biped*/ << QPoint(2,3)/*Quadped*/ << QPoint(2,3)/*Aquatic*/
    //             << QPoint(2,3)/*Amphibious*/ << QPoint(2,3)/*Octapedal*/ << QPoint(2,3)/*Anthropodal*/;
    skeletonRuleSet->numOfJointsPerBP_RS.insert(6, tempPolygon);

    // NECK = 9
    tempPolygon.resize(0);
    tempPolygon << QPoint(3,9)/*Biped*/ << QPoint(3,9)/*Quadped*/ << QPoint(3,5)/*Aquatic*/
                << QPoint(3,9)/*Amphibious*/ << QPoint(3,9)/*Octapedal*/ << QPoint(3,9)/*Anthropodal*/;
    skeletonRuleSet->numOfJointsPerBP_RS.insert(9, tempPolygon);

    // RIB = 10
    tempPolygon.resize(0);
    tempPolygon << QPoint(7,15)/*Biped*/ << QPoint(7,15)/*Quadped*/ << QPoint(7,15)/*Aquatic*/
                << QPoint(7,15)/*Amphibious*/ << QPoint(7,15)/*Octapedal*/ << QPoint(7,15)/*Anthropodal*/;
    skeletonRuleSet->numOfJointsPerBP_RS.insert(10, tempPolygon);

    // HEAD SUB BP - Maxilla = 11
    tempPolygon.resize(0);
    // At the moment we have only one bone. Later on we can have 2 bones maximum
    tempPolygon << QPoint(35,40)/*Biped*/ << QPoint(2,5)/*Quadped*/ << QPoint(2,5)/*Aquatic*/
                << QPoint(2,5)/*Amphibious*/ << QPoint(2,5)/*Octapedal*/ << QPoint(2,5)/*Anthropodal*/;
    skeletonRuleSet->numOfJointsPerBP_RS.insert(11, tempPolygon);

    // HEAD SUB BP - Mandible = 12
    tempPolygon.resize(0);
    tempPolygon << QPoint(6,20)/*Biped*/ << QPoint(2,5)/*Quadped*/ << QPoint(2,5)/*Aquatic*/
                << QPoint(2,5)/*Amphibious*/ << QPoint(2,5)/*Octapedal*/ << QPoint(2,5)/*Anthropodal*/;
    skeletonRuleSet->numOfJointsPerBP_RS.insert(12, tempPolygon);

    // HEAD SUB BP - Zygomatic = 13
    tempPolygon.resize(0);
    tempPolygon << QPoint(6,20)/*Biped*/ << QPoint(2,3)/*Quadped*/ << QPoint(2,3)/*Aquatic*/
                << QPoint(2,3)/*Amphibious*/ << QPoint(2,3)/*Octapedal*/ << QPoint(2,3)/*Anthropodal*/;
    skeletonRuleSet->numOfJointsPerBP_RS.insert(13, tempPolygon);

    // ExtremityBP_Base_Carpal = 17
    tempPolygon.resize(0);
    tempPolygon << QPoint(3,4)/*Biped*/ << QPoint(4,7)/*Quadped*/ << QPoint(4,10)/*Aquatic*/
                << QPoint(4,10)/*Amphibious*/ << QPoint(4,7)/*Octapedal*/ << QPoint(4,7)/*Anthropodal*/;
    skeletonRuleSet->numOfJointsPerBP_RS.insert(17, tempPolygon);

    // ExtremityBP_Digit = 18
    tempPolygon.resize(0);
    tempPolygon << QPoint(4,7)/*Biped*/ << QPoint(4,7)/*Quadped*/ << QPoint(4,10)/*Aquatic*/
                << QPoint(4,10)/*Amphibious*/ << QPoint(4,7)/*Octapedal*/ << QPoint(4,7)/*Anthropodal*/;
    skeletonRuleSet->numOfJointsPerBP_RS.insert(18, tempPolygon);


    skeletonRuleSet->initJointsArrangementTrends_RS();

    // initialize the BP_SizeComparedToBackBone_RS. There is no BB in here as this serves as the reference BP at the moment.
    // TODO(ismail): Include BB entry as well so that ...

    tempPolygon.resize(0);
    // LOWER LIMB = 2 (BP_TYPE_ID)
    tempPolygon << QPoint(10,300)/*Biped*/ << QPoint(10,300)/*Quadped*/ << QPoint(10,300)/*Aquatic*/
                << QPoint(10,300)/*Amphibious*/ << QPoint(10,300)/*Octapedal*/ << QPoint(10,300)/*Anthropodal*/;
    skeletonRuleSet->BP_SizeComparedToBackBone_RS.insert(2, tempPolygon);

    // UPPER LIMB = 3
    tempPolygon.resize(0);
    tempPolygon << QPoint(10,150)/*Biped*/ << QPoint(10,150)/*Quadped*/ << QPoint(10,150)/*Aquatic*/
                << QPoint(10,150)/*Amphibious*/ << QPoint(10,150)/*Octapedal*/ << QPoint(10,150)/*Anthropodal*/;
    skeletonRuleSet->BP_SizeComparedToBackBone_RS.insert(3, tempPolygon);

    // TAIL = 5
    tempPolygon.resize(0);
    tempPolygon << QPoint(10,300)/*Biped*/ << QPoint(10,300)/*Quadped*/ << QPoint(10,300)/*Aquatic*/
                << QPoint(10,300)/*Amphibious*/ << QPoint(10,300)/*Octapedal*/ << QPoint(10,300)/*Anthropodal*/;
    skeletonRuleSet->BP_SizeComparedToBackBone_RS.insert(5, tempPolygon);

    // HEAD = 6
    tempPolygon.resize(0);
    tempPolygon << QPoint(30,200)/*Biped*/ << QPoint(30,200)/*Quadped*/ << QPoint(30,200)/*Aquatic*/
                << QPoint(30,200)/*Amphibious*/ << QPoint(30,200)/*Octapedal*/ << QPoint(30,200)/*Anthropodal*/;
    skeletonRuleSet->BP_SizeComparedToBackBone_RS.insert(6, tempPolygon);

    // NECK = 9
    tempPolygon.resize(0);
    tempPolygon << QPoint(10,200)/*Biped*/ << QPoint(10,200)/*Quadped*/ << QPoint(10,200)/*Aquatic*/
                << QPoint(10,200)/*Amphibious*/ << QPoint(10,200)/*Octapedal*/ << QPoint(10,200)/*Anthropodal*/;
    skeletonRuleSet->BP_SizeComparedToBackBone_RS.insert(9, tempPolygon);

    // RIB = 10
    tempPolygon.resize(0);
    tempPolygon << QPoint(40,100)/*Biped*/ << QPoint(10,70)/*Quadped*/ << QPoint(10,70)/*Aquatic*/
                << QPoint(10,70)/*Amphibious*/ << QPoint(10,70)/*Octapedal*/ << QPoint(10,60)/*Anthropodal*/;
    skeletonRuleSet->BP_SizeComparedToBackBone_RS.insert(10, tempPolygon);

    // Extremity Base Carpal = 17
    tempPolygon.resize(0);
    tempPolygon << QPoint(10,70)/*Biped*/ << QPoint(10,70)/*Quadped*/ << QPoint(10,70)/*Aquatic*/
                << QPoint(10,70)/*Amphibious*/ << QPoint(10,70)/*Octapedal*/ << QPoint(10,60)/*Anthropodal*/;
    skeletonRuleSet->BP_SizeComparedToBackBone_RS.insert(10, tempPolygon);

    // Extremity Digit = 18
    tempPolygon.resize(0);
    tempPolygon << QPoint(10,70)/*Biped*/ << QPoint(10,70)/*Quadped*/ << QPoint(10,70)/*Aquatic*/
                << QPoint(10,70)/*Amphibious*/ << QPoint(10,70)/*Octapedal*/ << QPoint(10,60)/*Anthropodal*/;
    skeletonRuleSet->BP_SizeComparedToBackBone_RS.insert(10, tempPolygon);


    // initialize the BP_AngleArrangementsFromBackBone_RS
    // BP angle arrangement trends -- from the joint of another BP that it attaches to. These angles are the YAW angles
    // ... Each pair of QPOlygon is (Min Angle, Max Angle). The index of the pair is the Alien_Type_ID.
    tempPolygon.resize(0);
    // LOWER LIMB = 2 (BP_TYPE_ID)
    tempPolygon << QPoint(10,60)/*Biped*/ << QPoint(10,60)/*Quadped*/ << QPoint(5,70)/*Aquatic*/
                << QPoint(10,60)/*Amphibious*/ << QPoint(10,140)/*Octapedal*/ << QPoint(10,40)/*Anthropodal*/;
    skeletonRuleSet->BP_AngleArrangementsFromBackBone_RS.insert(2, tempPolygon);

    // UPPER LIMB = 3
    tempPolygon.resize(0);
    tempPolygon << QPoint(10,80)/*Biped*/ << QPoint(10,80)/*Quadped*/ << QPoint(5,85)/*Aquatic*/
                << QPoint(10,80)/*Amphibious*/ << QPoint(10,30)/*Octapedal*/ << QPoint(10,40)/*Anthropodal*/;
    skeletonRuleSet->BP_AngleArrangementsFromBackBone_RS.insert(3, tempPolygon);

    // TAIL = 5
    tempPolygon.resize(0);
    tempPolygon << QPoint(0,70)/*Biped*/ << QPoint(0,70)/*Quadped*/ << QPoint(0,70)/*Aquatic*/
                << QPoint(0,70)/*Amphibious*/ << QPoint(0,70)/*Octapedal*/ << QPoint(0,70)/*Anthropodal*/;
    skeletonRuleSet->BP_AngleArrangementsFromBackBone_RS.insert(5, tempPolygon);

    // HEAD = 6
    tempPolygon.resize(0);
    tempPolygon << QPoint(0,70)/*Biped*/ << QPoint(0,70)/*Quadped*/ << QPoint(0,70)/*Aquatic*/
                << QPoint(0,70)/*Amphibious*/ << QPoint(0,70)/*Octapedal*/ << QPoint(0,70)/*Anthropodal*/;
    skeletonRuleSet->BP_AngleArrangementsFromBackBone_RS.insert(6, tempPolygon);

    // NECK = 9
    tempPolygon.resize(0);
    tempPolygon << QPoint(10,30)/*Biped*/ << QPoint(10,30)/*Quadped*/ << QPoint(10,30)/*Aquatic*/
                << QPoint(10,30)/*Amphibious*/ << QPoint(10,30)/*Octapedal*/ << QPoint(10,30)/*Anthropodal*/;
    skeletonRuleSet->BP_AngleArrangementsFromBackBone_RS.insert(9, tempPolygon);

    // RIB = 10
    tempPolygon.resize(0);
    tempPolygon << QPoint(10,30)/*Biped*/ << QPoint(10,30)/*Quadped*/ << QPoint(10,30)/*Aquatic*/
                << QPoint(10,30)/*Amphibious*/ << QPoint(10,30)/*Octapedal*/ << QPoint(10,30)/*Anthropodal*/;
    skeletonRuleSet->BP_AngleArrangementsFromBackBone_RS.insert(10, tempPolygon);


    // initializing the BP_AttachmentToBackBone_ZoneAndScattering_RS
    skeletonRuleSet->initBP_AttachmentToBackBone_ZoneAndScattering_RS();

    // initializing the Skeletal_BP_Bulkiness_RS
    // TODO(ismail): At the moment it is the same for all BP Types but perhaps we can customize for each BP type
    //               ... and see better results
    QPolygonF tempPolygonF; tempPolygonF.clear();
    // TODO: Get the 'varying' and 'constant' trends working
    tempPolygonF        // First 4 pairs: (Bulkiness Trend ID, likelihood)
            << QPointF(0,0)/*varying*/ << QPointF(1,2)/*ascending*/ << QPointF(2,5)/*descending*/ << QPointF(3,0)/*constant*/
            << QPointF(1.0,2.0)/*(min number of sub cells of bones, max number of sub cells of bones)*/
            << QPointF(1.0,7.0)/*(min starting bone scaling factor=1.0, max starting bone scaling factor=3.0)*/
            << QPointF(1.0,1.1)/*(min scale multiplier=1.0, max scale multiplier=1.15)*/
            << QPointF(-80, 100)/* Next pair:  ==> -100 to -80 = scaling on the Y axis only (Y multiplies with scaling factor, X remains unchanged)
                                                ==> -80 to 30 = scaling on the X axis only (X multiplies with scaling factor, Y remains unchanged)
                                                ==> 30 to 100 = scaling on both X and Y*/;
    skeletonRuleSet->Skeletal_BP_Bulkiness_RS.insert(2, tempPolygonF);
    skeletonRuleSet->Skeletal_BP_Bulkiness_RS.insert(3, tempPolygonF);
    skeletonRuleSet->Skeletal_BP_Bulkiness_RS.insert(4, tempPolygonF);
    skeletonRuleSet->Skeletal_BP_Bulkiness_RS.insert(5, tempPolygonF);
    skeletonRuleSet->Skeletal_BP_Bulkiness_RS.insert(9, tempPolygonF);
    skeletonRuleSet->Skeletal_BP_Bulkiness_RS.insert(10, tempPolygonF);
    skeletonRuleSet->Skeletal_BP_Bulkiness_RS.insert(17, tempPolygonF); // Extremity Base Carpal
   tempPolygonF.resize(0);
   tempPolygonF        // First 4 pairs: (Bulkiness Trend ID, likelihood)
           << QPointF(0,0)/*varying*/ << QPointF(1,1)/*ascending*/ << QPointF(2,1)/*descending*/ << QPointF(3,0)/*constant*/
           << QPointF(1.0,2.0)/*(min number of sub cells of bones, max number of sub cells of bones)*/
           << QPointF(5.0,9.0)/*(min starting bone scaling factor=1.0, max starting bone scaling factor=3.0)*/
           << QPointF(1.0,1.5)/*(min scale multiplier=1.0, max scale multiplier=1.15)*/
           << QPointF(50, 100)/* Next pair:  ==> -100 to -80 = scaling on the Y axis only (Y multiplies with scaling factor, X remains unchanged)
                                               ==> -80 to 30 = scaling on the X axis only (X multiplies with scaling factor, Y remains unchanged)
                                               ==> 30 to 100 = scaling on both X and Y*/;
    skeletonRuleSet->Skeletal_BP_Bulkiness_RS.insert(6, tempPolygonF); // TODO(ismail): change for Head as the head will just be a simple cube
//    tempPolygonF.resize(0);
//    tempPolygonF        // First 4 pairs: (Bulkiness Trend ID, likelihood)
//            << QPointF(0,0)/*varying*/ << QPointF(1,0)/*ascending*/ << QPointF(2,2)/*descending*/ << QPointF(3,2)/*constant*/
//            << QPointF(1.0,2.0)/*(min number of sub cells of bones, max number of sub cells of bones)*/
//            << QPointF(20.5,21.0)/*(min starting bone scaling factor=1.0, max starting bone scaling factor=3.0)*/
//            << QPointF(1.0,1.2)/*(min scale multiplier=1.0, max scale multiplier=1.15)*/
//            << QPointF(-80, 30)/* Next pair:  ==> -100 to -80 = scaling on the Y axis only (Y multiplies with scaling factor, X remains unchanged)
//                                                ==> -80 to 30 = scaling on the X axis only (X multiplies with scaling factor, Y remains unchanged)
//                                                ==> 30 to 100 = scaling on both X and Y*/;
    skeletonRuleSet->Skeletal_BP_Bulkiness_RS.insert(11, tempPolygonF); // TODO(ismail): change for Head Sub BPs
    skeletonRuleSet->Skeletal_BP_Bulkiness_RS.insert(12, tempPolygonF);
    tempPolygonF.resize(0);
        tempPolygonF        // First 4 pairs: (Bulkiness Trend ID, likelihood)
            << QPointF(0,0)/*varying*/ << QPointF(1,0)/*ascending*/ << QPointF(2,2)/*descending*/ << QPointF(3,2)/*constant*/
            << QPointF(1.0,2.0)/*(min number of sub cells of bones, max number of sub cells of bones)*/
            << QPointF(6.5,7.0)/*(min starting bone scaling factor=1.0, max starting bone scaling factor=3.0)*/
            << QPointF(1.0,1.2)/*(min scale multiplier=1.0, max scale multiplier=1.15)*/
            << QPointF(-80, 30)/* Next pair:  ==> -100 to -80 = scaling on the Y axis only (Y multiplies with scaling factor, X remains unchanged)
                                                ==> -80 to 30 = scaling on the X axis only (X multiplies with scaling factor, Y remains unchanged)
                                                ==> 30 to 100 = scaling on both X and Y*/;
    skeletonRuleSet->Skeletal_BP_Bulkiness_RS.insert(13, tempPolygonF);
    tempPolygonF.resize(0);
    tempPolygonF        // First 4 pairs: (Bulkiness Trend ID, likelihood)
            << QPointF(0,0)/*varying*/ << QPointF(1,0)/*ascending*/ << QPointF(2,2)/*descending*/ << QPointF(3,0)/*constant*/
            << QPointF(1.0,2.0)/*(min number of sub cells of bones, max number of sub cells of bones)*/
            << QPointF(1.5,6.0)/*(min starting bone scaling factor=1.0, max starting bone scaling factor=3.0)*/
            << QPointF(1.3,2.0)/*(min scale multiplier=1.0, max scale multiplier=1.15)*/
            << QPointF(30, 100)/* Next pair:  ==> -100 to -80 = scaling on the Y axis only (Y multiplies with scaling factor, X remains unchanged)
                                                ==> -80 to 30 = scaling on the X axis only (X multiplies with scaling factor, Y remains unchanged)
                                                ==> 30 to 100 = scaling on both X and Y*/;
    skeletonRuleSet->Skeletal_BP_Bulkiness_RS.insert(18, tempPolygonF); // Extremity Digit

    // initialize the BoneBending_RS
    skeletonRuleSet->BoneBending_RS << QPoint(1,2) /* (min number of bends = 1, max number of bends = 3) */
                                    << QPoint(30,70) /* (min % of bone for bend, max % of bone for bend) */
                                    << QPoint(5,15) /* (min protrusion of bend, max protrusion of bend) i.e. sharpness refers to the multiplier that will magnify the rotated (90 degrees) unit vector */
                                    << QPoint(1,5); /* (min amount of smoothness required, max amount of smoothness required) */

    skeletonRuleSet->initBP_Densification_RS();

    skeletonRuleSet->initSkeletal_BP_SubDivCells_RS();

    // TODO(ismail): initialize the HeadSubBP_Densification_RS
    skeletonRuleSet->initHeadSubBP_Densification_RS();

    skeletonRuleSet->initGroupBP_AttachmentToAnotherBP_ZoneScatteringTrend_RS();

    skeletonRuleSet->initHeadSubBP_AlignmentRS();
    // initialize the bipedal_OtherBPsToGrow_RS, quadpedal_OtherBPsToGrow_RS, ... and others

    skeletonRuleSet->intHeadStretching_RS();

    skeletonRuleSet->intExtremity_RS();

    skeletonRuleSet->initBP_Bulgification_RS();

    qDebug() << "Skeleton Ruleset initialized";



//  **** BULKINESS RS DEFAULT VALUES ****
//    tempPolygonF        // First 4 pairs: (Bulkiness Trend ID, likelihood)
//            << QPointF(0,0)/*varying*/ << QPointF(1,5)/*ascending*/ << QPointF(2,5)/*descending*/ << QPointF(3,0)/*constant*/
//            << QPointF(1.0,2.0)/*(min number of sub cells of bones, max umber of sub cells of bones)*/
//            << QPointF(1.0,3.0)/*(min starting bone scaling factor=1.0, max starting bone scaling factor=3.0)*/
//            << QPointF(1.0,1.09)/*(min scale multiplier=1.0, max scale multiplier=1.15)*/
//            << QPointF(60, 100)/* Next pair:  ==> -100 to -80 = scaling on the X axis (X multiplies with scaling factor, Y remains unchanged)
//                                                ==> -80 to -60 = scaling on the Y axis (Y multiplies with scaling factor, X remains unchanged)
//                                                ==> -60 to 100 = scaling on both X and Y*/;


}

void AlienModeller::buildCompleteDNA_FromRulesets(Alien *alien, QMap<int, QPair<int,int>> bpAmntsMap)
{
    alien->alienDNA = new Alien_DNA();
    buildSkeletonDNA(alien, bpAmntsMap);
}

void AlienModeller::buildSkeletonDNA(Alien *alien, QMap<int, QPair<int,int>> bpAmntsMap)
{
    qDebug() << "Starting to build Skeleton DNA";

    alien->alienDNA->skeletonDNA = new Skeleton_DNA();

    int randomAlienType = alien->Alien_Type_ID;

    // set the numOfSkeletalBodyParts_DNA from the numOfSkeletalBodyParts_RS
    for (int i=0/* Here i acts as the BP_TYPE_ID */; i<AlienModeller::BodyPartsEnum::BodyPartsEnum_Length; i++)
    {
        if (skeletonRuleSet->numOfSkeletalBodyParts_RS.contains(i))
        {
            QPolygon tempPoly = skeletonRuleSet->numOfSkeletalBodyParts_RS.value(i);
            if (bpAmntsMap.contains(i))
                alien->alienDNA->skeletonDNA->numOfSkeletalBodyParts_DNA.insert( i, GeometryAlgorithms::getRandomInteger(bpAmntsMap.value(i).first, bpAmntsMap.value(i).second) );
            else
                alien->alienDNA->skeletonDNA->numOfSkeletalBodyParts_DNA.insert( i, GeometryAlgorithms::getRandomInteger(tempPoly[randomAlienType].x(),tempPoly[randomAlienType].y()) );
        }
    }

    // **** TODO(Ismail): Make seperate methods for all the following DNA creations, which would also cater for
    // .................. DNA modifications/mutations
    // set the numOfJointsPerBP_DNA from the numOfJointsPerBP_RS
    for (int i=0; i<AlienModeller::BodyPartsEnum::BodyPartsEnum_Length; i++)
    {
        if (skeletonRuleSet->numOfJointsPerBP_RS.contains(i))
        {
            QPolygon tempPoly = skeletonRuleSet->numOfJointsPerBP_RS.value(i);
            for (int j=0; j<alien->alienDNA->skeletonDNA->numOfSkeletalBodyParts_DNA.value(i,0); j++)
            {
                addNewEntryforNumOfJointsPerBP_DNA(alien, i, tempPoly, j);
            }
        }
    }

    // set the BP_JAT_rootJoint_DNA from the jointsArrangementTrends_RS
    for (int i=0; i<AlienModeller::BodyPartsEnum::BodyPartsEnum_Length; i++)
    {
        if (skeletonRuleSet->jointsArrangementTrends_RS.contains(i))
        {
            addNewEntryforBP_JAT_rootJoint_DNA(alien, i, alien->alienDNA->skeletonDNA->numOfSkeletalBodyParts_DNA.value(i,0));
        }
    }

    // set the BP_SizeComparedToBackBone_DNA from the BP_SizeComparedToBackBone_RS
    for (int i=0; i<AlienModeller::BodyPartsEnum::BodyPartsEnum_Length; i++)
    {
        if (skeletonRuleSet->BP_SizeComparedToBackBone_RS.contains(i))
        {
            QPolygon tempPoly = skeletonRuleSet->BP_SizeComparedToBackBone_RS.value(i);
            QPolygon tempPoly2;
            tempPoly2.resize(0);
            for (int j=0; j<alien->alienDNA->skeletonDNA->numOfSkeletalBodyParts_DNA.value(i,0); j++)
            {
                tempPoly2 << QPoint(j, GeometryAlgorithms::getRandomInteger(tempPoly[randomAlienType].x(),tempPoly[randomAlienType].y()));
            }
            alien->alienDNA->skeletonDNA->BP_SizeComparedToBackBone_DNA.insert(i, tempPoly2);
        }
    }

    // set the BP_AngleArrangementsFromBackBone_DNA from the BP_AngleArrangementsFromBackBone_RS
    for (int i=0; i<AlienModeller::BodyPartsEnum::BodyPartsEnum_Length; i++)
    {
        if (skeletonRuleSet->BP_AngleArrangementsFromBackBone_RS.contains(i))
        {
            QPolygon tempPoly = skeletonRuleSet->BP_AngleArrangementsFromBackBone_RS.value(i);
            QPolygon tempPoly2;
            tempPoly2.resize(0);
            for (int j=0; j<alien->alienDNA->skeletonDNA->numOfSkeletalBodyParts_DNA.value(i,0); j++)
            {
                tempPoly2 << QPoint(-1, GeometryAlgorithms::getRandomInteger(tempPoly[randomAlienType].x(),tempPoly[randomAlienType].y()));
            }
            alien->alienDNA->skeletonDNA->BP_AngleArrangementsFromBackBone_DNA.insert(i, tempPoly2);
        }
    }


    // set the BP_AttachmentToBackBone_ZoneAndScattering_DNA from the BP_AttachmentToBackBone_ZoneAndScattering_RS
    for (int i=0; i<AlienModeller::BodyPartsEnum::BodyPartsEnum_Length; i++)
    {
        addEntryForBP_AttachmentToBackBone_ZoneAndScattering_DNA(alien, i, randomAlienType);
    }

    manipulateSkeletalBulkinessDNA_Flags(alien, false, false, false, false, true, new QVector<int>(0), new QVector<int>(0));

    qDebug() << "Skeleton DNA built successfully";

}

void AlienModeller::addNewEntryforNumOfJointsPerBP_DNA(Alien *alien, int BP_Type, QPolygon tempPoly, int BP_ID)
{
    if  (
            (
                BP_Type == AlienModeller::BodyPartsEnum::HEAD
                ||
                BP_Type == AlienModeller::BodyPartsEnum::HEADSubBP_Maxilla
                                                     ||
                BP_Type == AlienModeller::BodyPartsEnum::HEADSubBP_Mandible
                                                     ||
                BP_Type == AlienModeller::BodyPartsEnum::HEADSubBP_Zygomatic
            )
         && alien->alienHeads.isEmpty()
        )
    {
        // Do nothing as there are no heads
    }
    else
    {
        int numJts = GeometryAlgorithms::getRandomInteger(tempPoly[alien->Alien_Type_ID].x(),tempPoly[alien->Alien_Type_ID].y());
        if (numJts % 2 != 0)
            numJts++;
        alien->alienDNA->skeletonDNA->numOfJointsPerBP_DNA[BP_Type].append(QPoint(BP_ID, numJts));
    }
}

void AlienModeller::addNewEntryforBP_JAT_rootJoint_DNA(Alien *alien, int BP_Type, int numOfEntriesToAdd)
{
    if (skeletonRuleSet->jointsArrangementTrends_RS.contains(BP_Type)   &&  numOfEntriesToAdd > 0)
    {
        QPolygon tempPoly = skeletonRuleSet->jointsArrangementTrends_RS.value(BP_Type);
        // expand the likelihoods of the 'enerygy trends' and 'angle trends'
        QVector<int> tempLikelihoodExpansionList_1, tempLikelihoodExpansionList_2;
        tempLikelihoodExpansionList_1.resize(0); tempLikelihoodExpansionList_2.resize(0);
        for (int j=0; j<6; j++)
        {
            for (int k=0; k<tempPoly[j].y(); k++) // expand tempLikelihoodExpansionList_1 for energy trends
            {
                tempLikelihoodExpansionList_1.append(tempPoly[j].x());
            }
            for (int k=0; k<tempPoly[j+6].y(); k++) // expand tempLikelihoodExpansionList_2 for angle trends
            {
                tempLikelihoodExpansionList_2.append(tempPoly[j+6].x());
            }
        }
        // TODO(ismail): perhaps shuffle the tempLikelihoodExpansionList_1 and tempLikelihoodExpansionList_2
        qDebug() << numOfEntriesToAdd;

        // Now add the energy and angle trend for the root joint for every BP of that BP type
        for (int j=0; j<numOfEntriesToAdd; j++)
        {
            // pad out (energy trend, starting energy selected randomly) and (angle trend, starting angle selected randomly)
            // Select a random energy trend from the expanded energy trends
            // select a starting random energy.
            int tempRandInd_1 = GeometryAlgorithms::getRandomInteger(0, tempLikelihoodExpansionList_1.length()-1);
            if (!alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.BP_JAT_rootJoint_DNA.contains(BP_Type))
            {
                QPolygonF tempPolyF_2; tempPolyF_2.clear();
                alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.BP_JAT_rootJoint_DNA.insert(BP_Type, tempPolyF_2);
            }

            alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.BP_JAT_rootJoint_DNA[BP_Type].append(
                        QPointF(
                                static_cast<float>(tempLikelihoodExpansionList_1.at(tempRandInd_1)),
                                GeometryAlgorithms::getRandomFloat(static_cast<float>(tempPoly[12].x()), static_cast<float>(tempPoly[12].y()))
                                ));
            // Select a random angle trend from the expanded angle trends
            // select a starting random angle
            int tempRandInd_2 = GeometryAlgorithms::getRandomInteger(0, tempLikelihoodExpansionList_2.length()-1);
            alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.BP_JAT_rootJoint_DNA[BP_Type].append(
                        QPointF(
                                static_cast<float>(tempLikelihoodExpansionList_2.at(tempRandInd_2)),
                                GeometryAlgorithms::getRandomFloat(static_cast<float>(tempPoly[13].x()), static_cast<float>(tempPoly[13].y()))
                                ));
        }
    }
}

void AlienModeller::addEntryForBP_AttachmentToBackBone_ZoneAndScattering_DNA(Alien *alien, int BP_TypeID, int alienTypeID)
{
    if (skeletonRuleSet->BP_AttachmentToBackBone_ZoneAndScattering_RS.contains(BP_TypeID))
    {
        const QMap<int, QPolygonF> &tempMap = skeletonRuleSet->BP_AttachmentToBackBone_ZoneAndScattering_RS.value(BP_TypeID);
//            const auto &tempMap = skeletonRuleSet->BP_AttachmentToBackBone_ZoneAndScattering_RS.value(i);
        if (tempMap.contains(alienTypeID))
        {
            const QPolygonF &tempPolyF = tempMap.value(alienTypeID);
//                const auto &tempPolyF = tempMap.value(randomAlienType);
            // expand the likelihoods of the 'enerygy trends' and 'angle trends'
            QVector<float> tempLikelihoodExpansionList_1;
            tempLikelihoodExpansionList_1.resize(0);
            for (int j=0; j<5; j++)
            {
                for (int k=0; k<static_cast<int>(tempPolyF[j].y()); k++)
                {
                    tempLikelihoodExpansionList_1.append(tempPolyF[j].x());
                }
            }
            // TODO(ismail): perhaps shuffle the tempLikelihoodExpansionList_1
            int tempRandInd = GeometryAlgorithms::getRandomInteger(0, tempLikelihoodExpansionList_1.length());
            float zone = tempLikelihoodExpansionList_1.at(tempRandInd);
            float scatteringValue = GeometryAlgorithms::getRandomFloat(tempPolyF[static_cast<int>(zone)+4].x(), tempPolyF[static_cast<int>(zone)+4].y());

            alien->alienDNA->skeletonDNA->BP_AttachmentToBackBone_ZoneAndScattering_DNA.insert(BP_TypeID, QPair<float,float>(zone,scatteringValue));
        }
    }
}

void AlienModeller::buildSkeletalBP_BulkinessDNA(Alien *clonedAlien, QVector<int> &allBP_ID_List)
{
    QMap<int, QPolygonF> tempUpdateMapPoly; tempUpdateMapPoly.clear();

    // TODO(ismail): Make a method for cloning the Bulkiness DNA from one Alien to another
    if (clonedAlien->alienDNA->skeletonDNA->bulkinessDNA_ManipulationFlags[0]) // Just clone its bulkiness DNA!
    {
        clonedAlien->alienDNA->skeletonDNA->Skeletal_BP_Bulkiness_DNA_ForEachBP.clear();
        for (int i=0; i<alienList.length(); i++)
        {
            if (alienList[i]->alienID == clonedAlien->clonedFrom)
            {
                QMapIterator<int, int> iter(BP_ToType_Map);
                for (int bp=0; bp<allBP_ID_List.length(); bp++)
                {
                    BodyPart *bpPtr=getBodyPartFromID(clonedAlien, allBP_ID_List[bp]);
                    while (iter.hasNext())
                    {
                        iter.next();
                        if (bpPtr->BP_Type == iter.value())
                        {
                            QPolygonF tempPoly = alienList[i]->alienDNA->skeletonDNA->Skeletal_BP_Bulkiness_DNA_ForEachBP.value(iter.key());
                            clonedAlien->alienDNA->skeletonDNA->Skeletal_BP_Bulkiness_DNA_ForEachBP.insert(allBP_ID_List[bp], tempPoly);

                            BP_ToType_Map.remove(iter.key());
                            break;
                        }
                    }
                    iter.toFront();
                    // next one
                }

                break;
            }
        }
        BP_ToType_Map.clear();
    }

    else if (clonedAlien->alienDNA->skeletonDNA->bulkinessDNA_ManipulationFlags[1]) // mutation flag is ON, so the BP's
                                                                                    // entry from the bulkiness DNA will
                                                                                    // be removed
    {
        for (int bp=0; bp<clonedAlien->alienDNA->skeletonDNA->BP_listToMutateInBulkinessDNA.length(); bp++)
        {
            int tempBP_ID = clonedAlien->alienDNA->skeletonDNA->BP_listToMutateInBulkinessDNA[bp];
            if (clonedAlien->alienDNA->skeletonDNA->Skeletal_BP_Bulkiness_DNA_ForEachBP.remove(tempBP_ID))
            {
                BP_ToType_Map.remove(tempBP_ID); // remove it from BP_ToType_Map as well
                qDebug() << "Old entry removed from Skeletal_BP_Bulkiness_2_DNA while mutating";
            }
        }
    }

    else if (clonedAlien->alienDNA->skeletonDNA->bulkinessDNA_ManipulationFlags[4]) // update flag is ON
    {
        for (int i=0; i<clonedAlien->alienDNA->skeletonDNA->BP_listToUpdateInBulkinessDNA.length(); i++)
        {
            // BP_listToUpdateInBulkinessDNA will be the same as incoming allBP_ID_List
            int tempBP_ID = clonedAlien->alienDNA->skeletonDNA->BP_listToUpdateInBulkinessDNA[i];
            if (clonedAlien->alienDNA->skeletonDNA->Skeletal_BP_Bulkiness_DNA_ForEachBP.contains(tempBP_ID))
            {
                BodyPart *tempBP = getBodyPartFromID(clonedAlien, tempBP_ID);
                if (tempBP->smoothnessLevel > 0)
                {
                    QPolygonF FirstBoneBulkValues; FirstBoneBulkValues.clear();
                    FirstBoneBulkValues << clonedAlien->alienDNA->skeletonDNA->Skeletal_BP_Bulkiness_DNA_ForEachBP.value(tempBP_ID).at(0)
                             << clonedAlien->alienDNA->skeletonDNA->Skeletal_BP_Bulkiness_DNA_ForEachBP.value(tempBP_ID).at(1)
                             << clonedAlien->alienDNA->skeletonDNA->Skeletal_BP_Bulkiness_DNA_ForEachBP.value(tempBP_ID).at(2)
                             << clonedAlien->alienDNA->skeletonDNA->Skeletal_BP_Bulkiness_DNA_ForEachBP.value(tempBP_ID).at(3);

                    tempUpdateMapPoly.insert(tempBP_ID, FirstBoneBulkValues);
                }
                else
                    tempUpdateMapPoly.insert(tempBP_ID, clonedAlien->alienDNA->skeletonDNA->Skeletal_BP_Bulkiness_DNA_ForEachBP.value(tempBP_ID));
                if (clonedAlien->alienDNA->skeletonDNA->Skeletal_BP_Bulkiness_DNA_ForEachBP.remove(tempBP_ID))
                    qDebug() << "Entry stored for BP for updating bulkiness";
            }
        }
    }

    // Now add a new one if it doesn't already exists and not present in the delete_list
    // set the Skeletal_BP_Bulkiness_DNA from the Skeletal_BP_Bulkiness_RS
    // This also updates the bulkiness DNA
    QPolygonF tempPoly_3; tempPoly_3.clear();
    for (int i=0; i<allBP_ID_List.length(); i++)
    {
        BodyPart *bpPtr=getBodyPartFromID(clonedAlien, allBP_ID_List[i]);
        QVector<Bone*> allBonesInOrder; allBonesInOrder.clear();
        getAllBP_BonesInOrder(clonedAlien, bpPtr, allBonesInOrder);
        int numOfBones = allBonesInOrder.length();
        if (skeletonRuleSet->Skeletal_BP_Bulkiness_RS.contains(bpPtr->BP_Type))
        {
            if (!clonedAlien->alienDNA->skeletonDNA->Skeletal_BP_Bulkiness_DNA_ForEachBP.contains(allBP_ID_List[i])) // add new entry
            {
                // If the BP has been extended/shortened (i.e. joints inc OR dec), we need to add or delete entries
                if (tempUpdateMapPoly.contains(bpPtr->BP_ID))
                {
                    // First delete all the cells of the bones of this BP
                    // TODO(ismail): Need to make a seperate method
                    // TODO(ismail): Need to cater for the case when the BP has been 'Densified'
                    // ... as it will have sub-cells
                    for (int b=0; b<allBonesInOrder.length(); b++)
                    {
                        allBonesInOrder[b]->allStackedDenserLineages.clear();
                        allBonesInOrder[b]->subdivisionCells.clear();
                        while (!allBonesInOrder[b]->cellList.empty()) delete allBonesInOrder[b]->cellList.last(), allBonesInOrder[b]->cellList.removeLast();
                        allBonesInOrder[b]->BP_TranslateMatrix.setToIdentity();
                        allBonesInOrder[b]->BP_OrientationQuat = QQuaternion::fromDirection(QVector3D(0,0,1), QVector3D(0,1,0));
                        // if (allBonesInOrder[b]->deleteAllBP_Cells(false))
                        // {
                            int firJnt = allBonesInOrder.at(b)->endJoints.first;
                            int secJnt = allBonesInOrder.at(b)->endJoints.second;
                            QVector3D firJntLoc = getBodyPartFromID(clonedAlien, firJnt)->getPivotPoint();
                            QVector3D secJntLoc = getBodyPartFromID(clonedAlien, secJnt)->getPivotPoint();

                            createSimpleBone(*allBonesInOrder[b], firJntLoc, secJntLoc);
                        // }
                        // else
                        // {
                        //     qDebug() << "all cells not deleted";
                        //     exit(0);
                        // }
                    }

                    // Check whether the bones are more or less than the previous bone
                    // if the bones are more than the previous version, then add more entires to the DNA copied from the previous one
                    // else if the bones are less than the previous one then simply delete the additional entries from the the end
                    int prevBnsCnt = static_cast<int>(tempUpdateMapPoly.value(bpPtr->BP_ID).length()/4);
                    if (numOfBones > prevBnsCnt)
                    {
                        int diff = numOfBones - prevBnsCnt;
                        // copy the last 4 entries i.e. the entry for the last bone
                        QPolygonF last4Poly; last4Poly.clear();
                        last4Poly << tempUpdateMapPoly.value(bpPtr->BP_ID).at(tempUpdateMapPoly.value(bpPtr->BP_ID).length()-4)
                                  << tempUpdateMapPoly.value(bpPtr->BP_ID).at(tempUpdateMapPoly.value(bpPtr->BP_ID).length()-3)
                                  << tempUpdateMapPoly.value(bpPtr->BP_ID).at(tempUpdateMapPoly.value(bpPtr->BP_ID).length()-2)
                                  << tempUpdateMapPoly.value(bpPtr->BP_ID).at(tempUpdateMapPoly.value(bpPtr->BP_ID).length()-1);

                        while (diff>0)
                        {
                            tempUpdateMapPoly[bpPtr->BP_ID].append(last4Poly);
                            diff--;
                        }
                    }
                    else if (numOfBones < prevBnsCnt)
                    {
                        int diff = prevBnsCnt - numOfBones;
                        while (diff>0)
                        {
                            tempUpdateMapPoly[bpPtr->BP_ID].removeLast();
                            tempUpdateMapPoly[bpPtr->BP_ID].removeLast();
                            tempUpdateMapPoly[bpPtr->BP_ID].removeLast();
                            tempUpdateMapPoly[bpPtr->BP_ID].removeLast();
                            diff--;
                        }
                    }
                    clonedAlien->alienDNA->skeletonDNA->Skeletal_BP_Bulkiness_DNA_ForEachBP.insert(bpPtr->BP_ID, tempUpdateMapPoly.value(bpPtr->BP_ID));
                    tempUpdateMapPoly.remove(bpPtr->BP_ID);
                }
                else
                {
                    qDebug() << "reached new entry";
                    QPolygonF tempPoly_2 = skeletonRuleSet->Skeletal_BP_Bulkiness_RS.value(bpPtr->BP_Type);
    //                QPolygonF tempPoly_3;                 tempPoly_3.resize(0);
                    QVector<int> tempLikelihoodExpansionList_1 = QVector<int>(0);
                    if (tempLikelihoodExpansionList_1.length() == 0)
                    {
                        for (int j=0; j<4; j++)
                        {
                            for (int k=0; k<static_cast<int>(tempPoly_2[j].y()); k++)
                            {
                                tempLikelihoodExpansionList_1.append(static_cast<int>(tempPoly_2[j].x()));
                            }
                        }
                    }

                    bool isTrendVarying=true, x_y_both_Scaling=false;
                    int tempStart = 0;
                    int bulkTrend_ID=0;
                    int tempRandInd=0, flag_1=0;

                    tempRandInd = GeometryAlgorithms::getRandomInteger(tempStart, tempLikelihoodExpansionList_1.length());
                    bulkTrend_ID = tempLikelihoodExpansionList_1.at(tempRandInd);

                    /*
                     * Logic for when the bulkification trend is varying. It means some BPs will bulk up (ascend),
                     * ... while others will bulk down
                     *
                     * - create a list of bulk trends IDs (chosen randomly) for each bone
                     * - This list will be used in the loop below to specify the bulkiness trend of the bone
                    */

                    float xVal=0, yVal=0;
                    int tempVal = GeometryAlgorithms::getRandomInteger(static_cast<int>(tempPoly_2[7].x()),static_cast<int>(tempPoly_2[7].y()));
                    if (tempVal <= -80)
                    {
                        xVal = 0;
                        yVal = 1;
                    }
                    else if (tempVal > -80 && tempVal <= 30)
                    {
                        xVal = 1;
                        yVal = 0;
                    }
                    else if (tempVal > 30)
                    {
                        xVal = 1;
                        yVal = 1;
                    }

                    /*  *** Possible logic for when the user wants "ALL BPs" of an alien to follow the same scaling for X and Y
                     * .....i.e. Either both X and Y, OR only X scaling.
                     * .....e.g. All BPs follow X-only scaling i.e. tappered style
                    if (allSame)
                    {
                        if (i == 0)
                        {
                            int tempVal = GeometryAlgorithms::getRandomInteger(static_cast<int>(tempPoly_2[7].x()),static_cast<int>(tempPoly_2[7].y()));
                        }
                    }
                    else
                    {
                        int tempVal = GeometryAlgorithms::getRandomInteger(static_cast<int>(tempPoly_2[7].x()),static_cast<int>(tempPoly_2[7].y()));
                    }
                    */

                    tempPoly_3.clear();
                    for (int bInd=0; bInd<numOfBones; bInd++)
                    {
//                        if (isTrendVarying)
//                        {
////                            tempRandInd = GeometryAlgorithms::getRandomInteger(tempStart, tempLikelihoodExpansionList_1.length());
////                            bulkTrend_ID = tempLikelihoodExpansionList_1.at(tempRandInd);
//    //                                qDebug() << "bulkTrend_ID " << bulkTrend_ID;

//                            if (bulkTrend_ID == 0 || flag_1 == 1) // trend will be varying. This condition should only happen once if for the first time bulkTrend_ID=0
//                            {
//                                flag_1=1;
//                                tempStart = static_cast<int>(tempPoly_2[0].y());
//                                isTrendVarying = true;
//    //                                    qDebug() << "trend is varying";
//                                if (bulkTrend_ID == 0)
//                                {
//                                    tempRandInd = GeometryAlgorithms::getRandomInteger(tempStart, tempLikelihoodExpansionList_1.length());
//                                    bulkTrend_ID = tempLikelihoodExpansionList_1.at(tempRandInd);
//                                }
//                            }
//                            else
//                            {
//                                isTrendVarying = false;
//    //                                    qDebug() << "trend is consistent";
//    //                                    qDebug() << "bulkTrend_ID " << bulkTrend_ID;
//                            }
//                        }


                        tempPoly_3  << QPointF(static_cast<float>(bulkTrend_ID),GeometryAlgorithms::getRandomInteger(tempPoly_2[4].x(),tempPoly_2[4].y()))
                                    << QPointF(GeometryAlgorithms::getRandomFloat(tempPoly_2[5].x(),tempPoly_2[5].y()),GeometryAlgorithms::getRandomFloat(tempPoly_2[6].x(),tempPoly_2[6].y()))
                                    << QPointF(xVal, yVal)
                                    << QPointF(-1, 1.0);
                    }
                    qDebug() << tempPoly_3;
                    clonedAlien->alienDNA->skeletonDNA->Skeletal_BP_Bulkiness_DNA_ForEachBP.insert(allBP_ID_List[i], tempPoly_3);
                    qDebug() << "New entry added to Skeletal_BP_Bulkiness_2_DNA";
                    allBonesInOrder.clear();
                }
            }
        }
        else
        {
            qDebug() << "there is no entry for this BP in the Skeletal_BP_Bulkiness_RS ruleset";
            break;
        }
    }

    // reset all the bulkiness manipulation flags
    manipulateSkeletalBulkinessDNA_Flags(clonedAlien, true, false, false, false, false, new QVector<int>(0), new QVector<int>(0), false, new QVector<int>(0));
    //    BP_ToType_Map.clear();
}

void AlienModeller::buildBoneSubDivCellsDNA_forGivenBP(Alien *alienPtr, BodyPart *bpPtr, QVector<Bone *> boneList)
{
    if (!alienPtr->alienDNA->skeletonDNA->Skeletal_BP_SubDivCells_DNA.contains(bpPtr->BP_ID))
    {
        // Get the values chosen randomly from the Ruleset
        QPolygonF tempRS = skeletonRuleSet->Skeletal_BP_SubDivCells_RS;
        QPolygonF tempDNA_Poly; tempDNA_Poly.clear();
        for (int i=0; i<boneList.length(); i++)
        {
            int tempRan = GeometryAlgorithms::getRandomInteger(0,2);
            int face = Box::BoxFace::LEFT;
            if (tempRan == 1)
                face = Box::BoxFace::RIGHT;
            // set 8 pairs for this bone in the DNA
            tempDNA_Poly    << QPointF(GeometryAlgorithms::getRandomInteger(tempRS[0].x(),tempRS[0].y()) , GeometryAlgorithms::getRandomInteger(0,tempRS[1].y())) // P0
                            << QPointF(GeometryAlgorithms::getRandomFloat(tempRS[2].x(),tempRS[2].y()) , GeometryAlgorithms::getRandomInteger(tempRS[10].x(),tempRS[10].y())) // P1
                            << QPointF(GeometryAlgorithms::getRandomInteger(tempRS[3].x(),tempRS[3].y()) , GeometryAlgorithms::getRandomFloat(tempRS[4].x(),tempRS[4].y())) // P2
                            << QPointF(GeometryAlgorithms::getRandomFloat(tempRS[5].x(),tempRS[5].y()) , GeometryAlgorithms::getRandomInteger(tempRS[10].x(),tempRS[10].y())) // P3
                            << QPointF(GeometryAlgorithms::getRandomInteger(tempRS[6].x(),tempRS[6].y()) , GeometryAlgorithms::getRandomFloat(tempRS[4].x(),tempRS[4].y())) // P4
                            << QPointF(GeometryAlgorithms::getRandomInteger(tempRS[7].x(),tempRS[7].y()) , face) // P5
                            << QPointF(GeometryAlgorithms::getRandomInteger(0,tempRS[8].x()) , GeometryAlgorithms::getRandomFloat(10,tempRS[8].y())) // P6
                            << QPointF(GeometryAlgorithms::getRandomFloat(0,tempRS[9].y()) , GeometryAlgorithms::getRandomInteger(tempRS[10].x(),tempRS[10].y())); // P7
        }
        alienPtr->alienDNA->skeletonDNA->Skeletal_BP_SubDivCells_DNA.insert(bpPtr->BP_ID, tempDNA_Poly);
    }
    else
        qDebug() << "The entry for this BP already exists in Skeletal_BP_SubDivCells_DNA";
}

void AlienModeller::buildBoneSubDivCellsDNA_forAllBPs(Alien *alienPtr)
{
    for (BodyPart *bpPtr : alienPtr->bodyParts)
    {
        if (
                bpPtr->BP_Type != AlienModeller::BodyPartsEnum::HEAD
                &&
                bpPtr->BP_Type != AlienModeller::BodyPartsEnum::JOINT
                &&
                bpPtr->BP_Type != AlienModeller::BodyPartsEnum::BONE
            )
        {
            // get the bones of this BP
            QVector<Bone*> bones; bones.clear();
            getAllBP_BonesInOrder(alienPtr, bpPtr, bones);
            buildBoneSubDivCellsDNA_forGivenBP(alienPtr, bpPtr, bones);
        }
    }
}

void AlienModeller::increaseOrReduceBP_Bulkiness(Alien *alienPtr, int BP_ID, int increaseOrReduceOverallBulkiness, float amountOfScaleChange)
{
//    float scalingMultOffset=0.00;

//    if (increaseOrReduceOverallBulkiness == 1  ||  increaseOrReduceOverallBulkiness == 0)
//    {
//        if (alienPtr->alienDNA->skeletonDNA->Skeletal_BP_Bulkiness_DNA_ForEachBP.contains(BP_ID))
//        {
//            for (int i=1; i<alienPtr->alienDNA->skeletonDNA->Skeletal_BP_Bulkiness_DNA_ForEachBP.value(BP_ID).length(); i+=4)
//            {
//                if (increaseOrReduceOverallBulkiness == 1) // increase bulkiness
//                {
//                    if (amountOfScaleChange == 1) // small change required
//                    {
//                        scalingMultOffset = 0.001;
//                        alienPtr->alienDNA->skeletonDNA->Skeletal_BP_Bulkiness_DNA_ForEachBP[BP_ID][i].setY(
//                                 alienPtr->alienDNA->skeletonDNA->Skeletal_BP_Bulkiness_DNA_ForEachBP.value(BP_ID).at(i).y() + scalingMultOffset);
//                    }
//                    else if (amountOfScaleChange == 2) // large change required
//                    {
//                        scalingMultOffset = 0.1;
//                        alienPtr->alienDNA->skeletonDNA->Skeletal_BP_Bulkiness_DNA_ForEachBP[BP_ID][i].setY(
//                                 alienPtr->alienDNA->skeletonDNA->Skeletal_BP_Bulkiness_DNA_ForEachBP.value(BP_ID).at(i).y() + scalingMultOffset);
//                    }
//                }
//                else if (increaseOrReduceOverallBulkiness == 0) // reduce bulkiness
//                {
//                    if (amountOfScaleChange == 1) // small change required
//                    {
//                        scalingMultOffset = 0.001;
//                        alienPtr->alienDNA->skeletonDNA->Skeletal_BP_Bulkiness_DNA_ForEachBP[BP_ID][i].setY(
//                                 alienPtr->alienDNA->skeletonDNA->Skeletal_BP_Bulkiness_DNA_ForEachBP.value(BP_ID).at(i).y() - scalingMultOffset);
//                    }
//                    else if (amountOfScaleChange == 2) // large change required
//                    {
//                        scalingMultOffset = 0.1;
//                        alienPtr->alienDNA->skeletonDNA->Skeletal_BP_Bulkiness_DNA_ForEachBP[BP_ID][i].setY(
//                                 alienPtr->alienDNA->skeletonDNA->Skeletal_BP_Bulkiness_DNA_ForEachBP.value(BP_ID).at(i).y() - scalingMultOffset);
//                    }
//                }
//            }
//        }
//    }

    float scalingMultOffset=0.00;

    if (increaseOrReduceOverallBulkiness == 1  ||  increaseOrReduceOverallBulkiness == 2)
    {
        if (alienPtr->alienDNA->skeletonDNA->Skeletal_BP_Bulkiness_DNA_ForEachBP.contains(BP_ID))
        {
            for (int i=1; i<alienPtr->alienDNA->skeletonDNA->Skeletal_BP_Bulkiness_DNA_ForEachBP.value(BP_ID).length(); i+=4)
            {
                if (increaseOrReduceOverallBulkiness == 1) // increase bulkiness
                {
                    if (amountOfScaleChange == 1) // small change required
                    {
                        // The reason we're doing this swapping is that I have noticed when the bulkiness trend is descending,
                        // ... when we press the 'decrease bulkiness button', the bulkiness actually increasing.
                        // ... Works perfectly fine when the bulkiness trend is ascending
                        if (alienPtr->alienDNA->skeletonDNA->Skeletal_BP_Bulkiness_DNA_ForEachBP.value(BP_ID).at(0).x() == 2)
                            scalingMultOffset = 0.999;
                        else
                            scalingMultOffset = 1.001;
                    }
                    else if (amountOfScaleChange == 2) // large change required
                    {
                        if (alienPtr->alienDNA->skeletonDNA->Skeletal_BP_Bulkiness_DNA_ForEachBP.value(BP_ID).at(0).x() == 2)
                            scalingMultOffset = 0.990;
                        else
                            scalingMultOffset = 1.01;
                    }
                }
                else if (increaseOrReduceOverallBulkiness == 2) // reduce bulkiness
                {
                    if (amountOfScaleChange == 1) // small change required
                    {
                        if (alienPtr->alienDNA->skeletonDNA->Skeletal_BP_Bulkiness_DNA_ForEachBP.value(BP_ID).at(0).x() == 2)
                            scalingMultOffset = 1.001;
                        else
                            scalingMultOffset = 0.999;
                    }
                    else if (amountOfScaleChange == 2) // large change required
                    {
                        if (alienPtr->alienDNA->skeletonDNA->Skeletal_BP_Bulkiness_DNA_ForEachBP.value(BP_ID).at(0).x() == 2)
                            scalingMultOffset = 1.01;
                        else
                            scalingMultOffset = 0.990;
                    }
                }
                alienPtr->alienDNA->skeletonDNA->Skeletal_BP_Bulkiness_DNA_ForEachBP[BP_ID][i].setY(alienPtr->alienDNA->skeletonDNA->Skeletal_BP_Bulkiness_DNA_ForEachBP.value(BP_ID).at(i).y() * scalingMultOffset);
            }
        }
    }
}

void AlienModeller::changeBP_BulkinessTrend(Alien *alienPtr, int BP_ID, float trendID)
{
    if (alienPtr->alienDNA->skeletonDNA->Skeletal_BP_Bulkiness_DNA_ForEachBP.contains(BP_ID))
    {
        // We skip by 4 because there are 4 entries for each bone, and we need to change the trend for each bone
        for (int i=0; i<alienPtr->alienDNA->skeletonDNA->Skeletal_BP_Bulkiness_DNA_ForEachBP.value(BP_ID).length(); i+=4)
        {
            alienPtr->alienDNA->skeletonDNA->Skeletal_BP_Bulkiness_DNA_ForEachBP[BP_ID][i].setX(trendID);
        }
    }
}

void AlienModeller::changeBP_Bulkiness_XY_Scaling(Alien *alienPtr, int BP_ID, QPointF XY_ScalingFlags)
{
    if (alienPtr->alienDNA->skeletonDNA->Skeletal_BP_Bulkiness_DNA_ForEachBP.contains(BP_ID))
    {
        for (int i=2; i<alienPtr->alienDNA->skeletonDNA->Skeletal_BP_Bulkiness_DNA_ForEachBP.value(BP_ID).length(); i+=4)
        {
            alienPtr->alienDNA->skeletonDNA->Skeletal_BP_Bulkiness_DNA_ForEachBP[BP_ID][i].setX(XY_ScalingFlags.x());
            alienPtr->alienDNA->skeletonDNA->Skeletal_BP_Bulkiness_DNA_ForEachBP[BP_ID][i].setY(XY_ScalingFlags.y());
        }
    }
}

void AlienModeller::changeBP_BulkinessScalingValues(Alien *alienPtr, int BP_ID, float startScaleVal, float scalingMultiplier)
{
    if (alienPtr->alienDNA->skeletonDNA->Skeletal_BP_Bulkiness_DNA_ForEachBP.contains(BP_ID))
    {
        for (int i=1; i<alienPtr->alienDNA->skeletonDNA->Skeletal_BP_Bulkiness_DNA_ForEachBP.value(BP_ID).length(); i+=4)
        {
            alienPtr->alienDNA->skeletonDNA->Skeletal_BP_Bulkiness_DNA_ForEachBP[BP_ID][i].setX(startScaleVal);
            alienPtr->alienDNA->skeletonDNA->Skeletal_BP_Bulkiness_DNA_ForEachBP[BP_ID][i].setY(scalingMultiplier);
        }
    }
}

void AlienModeller::build_BP_DensificationDNA(Alien *alienPtr, QVector<int> &BP_ID_List)
{
    qDebug() << "Starting to build Densification DNA";
    // Go through the BP_GenDensificationDNA and check if the BPs entry is not there
    // ... if not present, copy one directly from the ruleset
    for (int i=0; i<BP_ID_List.length(); i++)
    {
        if (!alienPtr->alienDNA->skeletonDNA->BP_GenDensificationDNA.contains(BP_ID_List[i])) // if not present, add one
            alienPtr->alienDNA->skeletonDNA->BP_GenDensificationDNA.insert(BP_ID_List[i], skeletonRuleSet->BP_Densification_RS);
    }

    for (int i=0; i<BP_ID_List.length(); i++)
    {
        QPolygonF tempRulePoly = alienPtr->alienDNA->skeletonDNA->BP_GenDensificationDNA.value(BP_ID_List[i]);
        QMap<int, QPolygonF> tempMap; tempMap.clear();
        // TODO: Re-build the DNA for an extended BP i.e. BP with joints added to it.
        // Possible solution: Copy the previous entries such as:
        //                                                      movingUpDownPerc
        //                                                      xScaling, yScaling, yScaling
        //                                                      rollAngle, and the rest of the DNA properties
        alienPtr->alienDNA->skeletonDNA->BP_SpecificDensificationDNA.remove(BP_ID_List[i]);
        if (!alienPtr->alienDNA->skeletonDNA->BP_SpecificDensificationDNA.contains(BP_ID_List[i]))
        {
            BodyPart *bpPtr = getBodyPartFromID(alienPtr, BP_ID_List[i]);
            QVector<Bone*> tempBoneList; tempBoneList.clear();
            if (this->getAllBP_BonesInOrder(alienPtr, bpPtr, tempBoneList))
            {
//                if (tempBoneList.length() <= 2)
//                {
//                    qDebug() << "Need atleast 3 bones to build densification DNA. Consider increasing joints or smoothing";
//                    continue;
//                }
                // select its mid bone
                // The numerator the guide the position of bone to act as the pivot bone
                int midInd = floorf((50/100.f)*tempBoneList.length());

                float isThisBonePivot = 1, numOfBonesOnEitherSides = -1;

                bool kIsDecreasing=false;
                float densCnt = tempRulePoly[3].y();
                float movingUpDownPerc = GeometryAlgorithms::getRandomInteger(tempRulePoly[14].x(),tempRulePoly[14].y()); //P13
                // TODO: The following properties must be universal/common for all bones, and not be changed/set for each bone
                // for (int k=midInd; k<tempBoneList.length() && k>=0; )
                // int max = GeometryAlgorithms::getRandomInteger(midInd,tempBoneList.length()-1);
                int min = GeometryAlgorithms::getRandomInteger(0,midInd-1);
                int max = tempBoneList.length();
                for (int k=midInd; k<max && k>=min; )
                {
                    qDebug() << "reached";
                    QPolygonF tempPoly; tempPoly.clear();

                    // If we want decreasing number of densers on the neighbouring bones, then
                    // ... use the following formula.
                    densCnt /= tempRulePoly[3].x();

                    float xScaling = GeometryAlgorithms::getRandomFloat(tempRulePoly[4].x(),tempRulePoly[4].y());
                    float yScaling = GeometryAlgorithms::getRandomFloat(tempRulePoly[5].x(),tempRulePoly[5].y());
                    float zScaling = GeometryAlgorithms::getRandomFloat(tempRulePoly[6].x(),tempRulePoly[6].y());

                    tempPoly    << QPointF(isThisBonePivot,numOfBonesOnEitherSides)     // P0
                                << QPointF(1 /*RIGHT edge - but could be any edge*/ ,(GeometryAlgorithms::getRandomFloat(0,tempRulePoly[2].y()))/100) // P1
                                                           /*number of densers stacked. min=20, max=30*/
                                << QPointF(tempRulePoly[3].x(),ceilf(densCnt)) // P2

                                // definitely need these
                                << QPointF(xScaling,xScaling) //P3
                                << QPointF(yScaling,yScaling) //P4
                                << QPointF(zScaling,zScaling) //P5

                                << QPointF(GeometryAlgorithms::getRandomFloat(tempRulePoly[7].x()-1,tempRulePoly[7].x()),
                                           floorf(GeometryAlgorithms::getRandomFloat(0,tempRulePoly[7].y())))   //P6

                                // definitely need these
                                << QPointF(floorf(GeometryAlgorithms::getRandomFloat(1,tempRulePoly[8].x())),
                                            GeometryAlgorithms::getRandomFloat(0,tempRulePoly[8].y())) //P7

                                << QPointF(GeometryAlgorithms::getRandomFloat(20,tempRulePoly[9].x()),
                                            GeometryAlgorithms::getRandomFloat(20,tempRulePoly[9].y())) //P8

                                << QPointF(floorf(GeometryAlgorithms::getRandomFloat(0,tempRulePoly[10].x())),
                                    GeometryAlgorithms::getRandomFloat(0,tempRulePoly[9].y())) //P9

                                << QPointF(floorf(GeometryAlgorithms::getRandomFloat(0,tempRulePoly[11].x())),
                                    GeometryAlgorithms::getRandomFloat(0,tempRulePoly[11].y())) //P10

                                << QPointF(floorf(GeometryAlgorithms::getRandomFloat(0,tempRulePoly[12].x())),
                                    GeometryAlgorithms::getRandomFloat(0,tempRulePoly[12].y())) //P11

                                << QPointF(GeometryAlgorithms::getRandomFloat(0,tempRulePoly[13].x()),
                                    GeometryAlgorithms::getRandomFloat(0,tempRulePoly[13].y())) //P12

                                << QPointF(movingUpDownPerc,-1) //P13

                                ;

                    tempMap.insert(tempBoneList[k]->BP_ID, tempPoly);

                    isThisBonePivot = 0;

                    if (k == max-1)
                    {
                        k = midInd;
                        kIsDecreasing = true;
                        densCnt = tempRulePoly[3].y(); // reset to the original number of densers
                    }
                    if (kIsDecreasing)
                        k--;
                    else
                        k++;
                }
            }
            else
            {
                qDebug() << "Bones for this BP not extracted in order";
                exit(0);
            }
            alienPtr->alienDNA->skeletonDNA->BP_SpecificDensificationDNA.insert(BP_ID_List[i],tempMap);
        }
    }
    qDebug() << "Densification DNA built successfully";
}

void AlienModeller::buildRebuildUpdate_BP_BulgificationDNA(Alien *alienPtr, int BP_ID, bool rebuildFromScratch)
{
    qDebug() << "commencing building of Bulgification DNA";
    BodyPart *bpPtr = getBodyPartFromID(alienPtr, BP_ID);
    if (bpPtr)
    {
        if (rebuildFromScratch) // for rebuilding for a BP from scratch
        {
            qDebug() << "Rebuilding bulgification DNA from scratch";
            alienPtr->alienDNA->skeletonDNA->BP_BulgificationDNA.remove(BP_ID);
        }
        if (!alienPtr->alienDNA->skeletonDNA->BP_BulgificationDNA.contains(BP_ID)) // for building for a new BP
        {
            qDebug() << "Building bulgification DNA";
            QVector<Bone*> bonesInOrder; bonesInOrder.clear();
            this->getAllBP_BonesInOrder(alienPtr, bpPtr, bonesInOrder);

            QPolygon tempBulgRS = skeletonRuleSet->BP_Bulgification_RS;
            QPolygon tempChoicePoly_DNA;
            QMap<int,QPolygon> tempBoneMap; tempBoneMap.clear();
            for (int i=0; i<bonesInOrder.length(); i++)
            {
                tempChoicePoly_DNA.clear();
                tempChoicePoly_DNA  << QPoint(GeometryAlgorithms::getRandomInteger(tempBulgRS[0].x(),tempBulgRS[0].y()),
                                             GeometryAlgorithms::getRandomInteger(tempBulgRS[1].x(),tempBulgRS[1].y()))
                                   << QPoint(GeometryAlgorithms::getRandomInteger(tempBulgRS[2].x(),tempBulgRS[2].y()),
                                             GeometryAlgorithms::getRandomInteger(tempBulgRS[3].x(),tempBulgRS[3].y()))
                                   << QPoint(GeometryAlgorithms::getRandomInteger(tempBulgRS[4].x(),tempBulgRS[4].y()),
                                             GeometryAlgorithms::getRandomInteger(tempBulgRS[5].x(),tempBulgRS[5].y()))
                                   << QPoint(GeometryAlgorithms::getRandomInteger(tempBulgRS[5].x(),tempBulgRS[5].y()),
                                             GeometryAlgorithms::getRandomInteger(tempBulgRS[5].x(),tempBulgRS[5].y()))
                                   << QPoint(GeometryAlgorithms::getRandomInteger(tempBulgRS[6].x(),tempBulgRS[6].y()),
                                             GeometryAlgorithms::getRandomInteger(tempBulgRS[7].x(),tempBulgRS[7].y()))
                                   << QPoint(GeometryAlgorithms::getRandomInteger(tempBulgRS[8].x(),tempBulgRS[8].y()),
                                             GeometryAlgorithms::getRandomInteger(tempBulgRS[9].x(),tempBulgRS[9].y()));

                tempBoneMap.insert(bonesInOrder[i]->BP_ID, tempChoicePoly_DNA);

                if (i == bonesInOrder.length()-1)
                    alienPtr->alienDNA->skeletonDNA->BP_BulgificationDNA.insert(BP_ID,tempBoneMap);
            }
        }
        else // we might need to update the DNA based on any bones that were added later or removed
        {
            qDebug() << "update the bulginess DNA based on any bones that were added later or removed";
            QVector<Bone*> bonesInOrder; bonesInOrder.clear();
            this->getAllBP_BonesInOrder(alienPtr, bpPtr, bonesInOrder);

            int numOfBonesToBuildFor = bonesInOrder.length() - alienPtr->alienDNA->skeletonDNA->BP_BulgificationDNA.value(BP_ID).size();
            qDebug() << "numOfBonesToBuildFor : " << numOfBonesToBuildFor;
            if (numOfBonesToBuildFor > 0) // we need to add some entries
            {
                qDebug() << "we need to add some entries in the bulginess DNA";
                int startingInd = alienPtr->alienDNA->skeletonDNA->BP_BulgificationDNA.value(BP_ID).size();
                QPolygon tempBulgRS = skeletonRuleSet->BP_Bulgification_RS;
                QPolygon tempChoicePoly_DNA;

                for (int i=startingInd; i<bonesInOrder.length(); i++)
                {
                    tempChoicePoly_DNA.clear();
                    tempChoicePoly_DNA  << QPoint(GeometryAlgorithms::getRandomInteger(tempBulgRS[0].x(),tempBulgRS[0].y()),
                                                 GeometryAlgorithms::getRandomInteger(tempBulgRS[1].x(),tempBulgRS[1].y()))
                                       << QPoint(GeometryAlgorithms::getRandomInteger(tempBulgRS[2].x(),tempBulgRS[2].y()),
                                                 GeometryAlgorithms::getRandomInteger(tempBulgRS[3].x(),tempBulgRS[3].y()))
                                       << QPoint(GeometryAlgorithms::getRandomInteger(tempBulgRS[4].x(),tempBulgRS[4].y()),
                                                 GeometryAlgorithms::getRandomInteger(tempBulgRS[5].x(),tempBulgRS[5].y()))
                                       << QPoint(GeometryAlgorithms::getRandomInteger(tempBulgRS[5].x(),tempBulgRS[5].y()),
                                                 GeometryAlgorithms::getRandomInteger(tempBulgRS[5].x(),tempBulgRS[5].y()))
                                       << QPoint(GeometryAlgorithms::getRandomInteger(tempBulgRS[6].x(),tempBulgRS[6].y()),
                                                 GeometryAlgorithms::getRandomInteger(tempBulgRS[7].x(),tempBulgRS[7].y()))
                                       << QPoint(GeometryAlgorithms::getRandomInteger(tempBulgRS[8].x(),tempBulgRS[8].y()),
                                                 GeometryAlgorithms::getRandomInteger(tempBulgRS[9].x(),tempBulgRS[9].y()));

                    if (!alienPtr->alienDNA->skeletonDNA->BP_BulgificationDNA[BP_ID].contains(bonesInOrder[i]->BP_ID))
                        alienPtr->alienDNA->skeletonDNA->BP_BulgificationDNA[BP_ID].insert(bonesInOrder[i]->BP_ID, tempChoicePoly_DNA);
                }
                if (bonesInOrder.length() != alienPtr->alienDNA->skeletonDNA->BP_BulgificationDNA.value(BP_ID).size())
                {
                    qDebug() << "bonesInOrder.length() and alienPtr->alienDNA->skeletonDNA->BP_BulgificationDNA.value(BP_ID).size()these sizes should be same";
                    exit(0);
                }
            }
            else if (numOfBonesToBuildFor < 0) // we need to remove those entries which are not in the bonesInOrder list
            {
                qDebug() << "we need to remove some entries in the bulginess DNA";
                QVector<int> bonesInOrderIDs; bonesInOrderIDs.clear();
                for (Bone* bone : bonesInOrder)
                    bonesInOrderIDs << bone->BP_ID;

                QMapIterator<int,QPolygon> iter(alienPtr->alienDNA->skeletonDNA->BP_BulgificationDNA.value(BP_ID));
                while (iter.hasNext())
                {
                    iter.next();
                    if (!bonesInOrderIDs.contains(iter.key()))
                        alienPtr->alienDNA->skeletonDNA->BP_BulgificationDNA[BP_ID].remove(iter.key());
                }
            }
        }
    }
    else
    {
        qDebug() << "bpPtr does not exist in alien - buildRebuildUpdate_BP_BulgificationDNA method";
        exit(0);
    }
    qDebug() << "finished building of Bulgification DNA";
}

bool AlienModeller::cloneBP_BulgificationDNA(Alien *srcAlienPtr, Alien *destAlienPtr, BodyPart *srcBP_Ptr, BodyPart *destBP_Ptr)
{
    qDebug() << "Starting to clone Bulgification DNA";
    if (srcAlienPtr->alienDNA->skeletonDNA->BP_BulgificationDNA.contains(srcBP_Ptr->BP_ID))
    {
        QVector<Bone*> tempBoneList_destBP; tempBoneList_destBP.clear();
        this->getAllBP_BonesInOrder(destAlienPtr, destBP_Ptr, tempBoneList_destBP);
        QVector<Bone*> tempBoneList_srcBP; tempBoneList_srcBP.clear();
        this->getAllBP_BonesInOrder(srcAlienPtr, srcBP_Ptr, tempBoneList_srcBP);
        qDebug() << tempBoneList_destBP.length() << " : " << tempBoneList_srcBP.length();
        if (tempBoneList_destBP.length() == tempBoneList_srcBP.length())
        {
            QMap<int, QPolygon> sourceMap = srcAlienPtr->alienDNA->skeletonDNA->BP_BulgificationDNA.value(srcBP_Ptr->BP_ID);
            QMap<int, QPolygon> tempMap; tempMap.clear();
            int b=0;
            while (b<tempBoneList_destBP.length())
            {
                if (sourceMap.contains(tempBoneList_srcBP[b]->BP_ID))
                    tempMap.insert(tempBoneList_destBP[b]->BP_ID, sourceMap.value(tempBoneList_srcBP[b]->BP_ID));
                b++;
            }
            destAlienPtr->alienDNA->skeletonDNA->BP_BulgificationDNA.insert(destBP_Ptr->BP_ID,tempMap);

            qDebug() << "Bulgification DNA cloned successfully";
            return true;
        }
        else
        {
            qDebug() << "Source BP and Dest BP don't have equal number of bones";
            return false;
        }
    }
    else
    {
        qDebug() << "Bulgification DNA could not be cloned as source is not bulgified";
        return false;
    }
}

bool AlienModeller::cloneDensificationDNA(Alien *srcAlienPtr, Alien *destAlienPtr, BodyPart *srcBP_Ptr, BodyPart *destBP_Ptr)
{
    qDebug() << "Starting to clone Densification DNA";
    // Go through the BP_GenDensificationDNA and check if the BPs entry is not there
    // ... if not present, copy one directly from the ruleset
    // TODO: there is no need to have a new entry in BP_GenDensificationDNA as they're all the same
    if (srcAlienPtr->alienDNA->skeletonDNA->BP_GenDensificationDNA.contains(srcBP_Ptr->BP_ID))
    {
        destAlienPtr->alienDNA->skeletonDNA->BP_GenDensificationDNA.insert(destBP_Ptr->BP_ID, skeletonRuleSet->BP_Densification_RS);
        if (srcAlienPtr->alienDNA->skeletonDNA->BP_SpecificDensificationDNA.contains(srcBP_Ptr->BP_ID))
        {
            QVector<Bone*> tempBoneList_destBP; tempBoneList_destBP.clear();
            this->getAllBP_BonesInOrder(destAlienPtr, destBP_Ptr, tempBoneList_destBP);
            QVector<Bone*> tempBoneList_srcBP; tempBoneList_srcBP.clear();
            this->getAllBP_BonesInOrder(srcAlienPtr, srcBP_Ptr, tempBoneList_srcBP);
            qDebug() << tempBoneList_destBP.length() << " : " << tempBoneList_srcBP.length();
            if (tempBoneList_destBP.length() == tempBoneList_srcBP.length())
            {
                QMap<int, QPolygonF> sourceMap = srcAlienPtr->alienDNA->skeletonDNA->BP_SpecificDensificationDNA.value(srcBP_Ptr->BP_ID);
                QMap<int, QPolygonF> tempMap; tempMap.clear();
                int b=0;
                while (b<tempBoneList_destBP.length())
                {
                    if (sourceMap.contains(tempBoneList_srcBP[b]->BP_ID))
                        tempMap.insert(tempBoneList_destBP[b]->BP_ID, sourceMap.value(tempBoneList_srcBP[b]->BP_ID));
                    b++;
                }
                destAlienPtr->alienDNA->skeletonDNA->BP_SpecificDensificationDNA.insert(destBP_Ptr->BP_ID,tempMap);
            }
            qDebug() << "Densification DNA cloned successfully";
            return true;
        }
    }
    else
    {
        qDebug() << "Densification DNA could not be cloned";
        return false;
    }
}

void AlienModeller::modify_BP_DensificationDNA(Alien *alienPtr, QVector<int> &BP_ID_List, int optionNum)
{
    qDebug() << "Modifying Densification DNA";
    for (int i=0; i<BP_ID_List.length(); i++)
    {
        if (alienPtr->alienDNA->skeletonDNA->BP_SpecificDensificationDNA.contains(BP_ID_List[i]))
        {
            if (optionNum == 0) // increase spikiness
                incrDecrDensifierSpikiness(alienPtr,
                                           getBodyPartFromID(alienPtr, BP_ID_List[i]),
                                           true);
            else if (optionNum == 1) // decs spikiness
            {
                incrDecrDensifierSpikiness(alienPtr,
                                           getBodyPartFromID(alienPtr, BP_ID_List[i]),
                                           false);
            }

            else if (optionNum == 2) // incr stacked densers
            {
                incrDecrStackedDensers(alienPtr,
                                       getBodyPartFromID(alienPtr, BP_ID_List[i]),
                                       true);
            }

            else if (optionNum == 3) // decrement stacked densers
            {
                incrDecrStackedDensers(alienPtr,
                                       getBodyPartFromID(alienPtr, BP_ID_List[i]),
                                       false);
            }
            else if (optionNum == 4) // incr curvature
            {
                incrDecrDenserCurvature(alienPtr,
                                       getBodyPartFromID(alienPtr, BP_ID_List[i]),
                                       true);
            }
            else if (optionNum == 5) // decr curvature
            {
                incrDecrDenserCurvature(alienPtr,
                                       getBodyPartFromID(alienPtr, BP_ID_List[i]),
                                       false);
            }
            else if (optionNum == 6) // inc spike moving up/down percentage
            {
                incDecDenserSpikeMovingUpDownPerc(alienPtr,
                                                  getBodyPartFromID(alienPtr, BP_ID_List[i]), true);
            }
            else if (optionNum == 7) // inc spike moving up/down percentage
            {
                incDecDenserSpikeMovingUpDownPerc(alienPtr,
                                                  getBodyPartFromID(alienPtr, BP_ID_List[i]), false);
            }
            else if (optionNum == 8) // inc spike moving up/down percentage
            {
                moveDensersAcrossBP_Bones(alienPtr,
                                                  getBodyPartFromID(alienPtr, BP_ID_List[i]), true);
            }
            else if (optionNum == 9) // inc spike moving up/down percentage
            {
                moveDensersAcrossBP_Bones(alienPtr,
                                                  getBodyPartFromID(alienPtr, BP_ID_List[i]), false);
            }

            // build_BP_DensificationDNA(alienPtr, BP_ID_List);
        }
    }
}

BodyPart* AlienModeller::squashOrStrechBP(Alien *alienPtr, int BP_ID, float squashOrStrechFlag, float squashOrStretchFactor)
{
    qDebug() << "about to squash/stretch";

    alienPtr->alienDNA->skeletonDNA->Skeletal_BP_Bulkiness_DNA_ForEachBP[BP_ID][3].setX(squashOrStrechFlag);
    alienPtr->alienDNA->skeletonDNA->Skeletal_BP_Bulkiness_DNA_ForEachBP[BP_ID][3].setY(squashOrStretchFactor);

    BodyPart *origBP_Ptr = getBodyPartFromID(alienPtr, BP_ID);
    // if this is a parent BP, then detach its subBPs (other than joints and bone) before removing it
    QVector<BodyPart*> tempSubBPs; tempSubBPs.clear();
    detachAllSubBPsFromParentBPs(alienPtr, origBP_Ptr, tempSubBPs);
    QVector<int> subExtremIDs = detachAllExtremBPsFromGivenParentBP(alienPtr, origBP_Ptr); // if there are any sub extremities, these will be detached too

    // Keep a note of BPs position and orientation vector
    QVector3D tempPos = origBP_Ptr->getPivotPoint();
    QVector3D tempVec = origBP_Ptr->getPivotVector();

    // Clone the BP
    BodyPart *mutatedBP_Ptr = cloneSkeletalBP_FromAnotherAlien_viaDNA(alienPtr, alienPtr, origBP_Ptr);

    // Orient and position the BP again to its original place
    GeometryAlgorithms::rotateBodyPartTowardsVecDir(*(mutatedBP_Ptr), tempVec);
    GeometryAlgorithms::translateBP(mutatedBP_Ptr,   tempPos.x() - mutatedBP_Ptr->getPivotPoint().x(),
                                                            tempPos.y() - mutatedBP_Ptr->getPivotPoint().y(),
                                                            tempPos.z() - mutatedBP_Ptr->getPivotPoint().z());


    // Make sure its mirror is also squashed or Stretch, based on user's input
    BodyPart *mirrPtr = getMirrorOrOriginalBP(alienPtr, origBP_Ptr); // before removing originBP, note down its mirror (if any)

    // replace the original BP with its clone
    mutatedBP_Ptr = replaceBP_WithAnotherBP(alienPtr, origBP_Ptr, mutatedBP_Ptr);
    // Attach the extremities that were detached earlier
    attachExtremBP_ToParentBP(alienPtr, mutatedBP_Ptr, subExtremIDs);
    qDebug() << "BP replaced with its Clone";
    // reset the values after the sqwashing has been performed, so it doesn't squash its clone whenever cloning is done
    alienPtr->alienDNA->skeletonDNA->Skeletal_BP_Bulkiness_DNA_ForEachBP[BP_ID][3].setX(-1.0);
    alienPtr->alienDNA->skeletonDNA->Skeletal_BP_Bulkiness_DNA_ForEachBP[BP_ID][3].setY(1.0);

    if (mirrPtr)
    {
        if (!alienPtr->alienDNA->skeletonDNA->BP_SizeComparedToBackBone_DNA[mirrPtr->BP_Type].isEmpty())
            alienPtr->alienDNA->skeletonDNA->BP_SizeComparedToBackBone_DNA[mirrPtr->BP_Type].removeLast();
        if (!alienPtr->alienDNA->skeletonDNA->BP_AngleArrangementsFromBackBone_DNA[mirrPtr->BP_Type].isEmpty())
            alienPtr->alienDNA->skeletonDNA->BP_AngleArrangementsFromBackBone_DNA[mirrPtr->BP_Type].removeLast();

        // TODO: This should be called inside removeAssociatedJATsAndOtherDSsForRemovableJnts
        alienPtr->alienDNA->skeletonDNA->skeletal_JATs_DNA.BP_to_JAT_rootJoint_Map_DNA.remove(mirrPtr->BP_ID);

        QVector<int> tempBP_ID_List_1; tempBP_ID_List_1.clear();
        tempBP_ID_List_1 << mirrPtr->BP_ID;
        removeAlienBodyParts(alienPtr, tempBP_ID_List_1);

        mirrPtr = symmetrifyBP_AroundOppositeAttachmentFace(alienPtr, mutatedBP_Ptr);

        // QPolygon *Jnt2JntRelExtract = new QPolygon(*(alienPtr->alienSkeleton.Jnt2JntRelations_TypeGuided.value(mirrPtr->BP_ID)));
        // Do the smoothing
        // smoothenSkeletalBP(alienPtr, mirrPtr, Jnt2JntRelExtract, smoothLevel);
    }

    // QPolygon *Jnt2JntRelExtract = new QPolygon(*(alienPtr->alienSkeleton.Jnt2JntRelations_TypeGuided.value(inBP_Ptr->BP_ID)));
    // // Do the smoothing
    // smoothenSkeletalBP(alienPtr, inBP_Ptr, Jnt2JntRelExtract, smoothLevel);

    // If the BP has any sub BPs that were detached, these need to be cloned and the clones need to be attached
    // ... to the BP
    // TODO: This code inside the for loop is repeated in other methods as well. Perhaps make a method for it
    QVector<int> tempSubMirrBPsIDs; tempSubMirrBPsIDs.clear();
    if (mutatedBP_Ptr->BP_Type != AlienModeller::BodyPartsEnum::BACK_BONE)
    {
        QMultiMap<int,int> tempMultMap; tempMultMap.clear();
        QVector<BodyPart*> tempSubExtremBPsForAttaching; tempSubExtremBPsForAttaching.clear();
        for (BodyPart *bp : tempSubBPs)
        {
            if (!bp->isSymmetric)
            {
                if (bp->BP_Type == AlienModeller::BodyPartsEnum::ExtremBP_Carpal
                        ||
                    bp->BP_Type == AlienModeller::BodyPartsEnum::ExtremBP_Digit)
                {
                    tempSubExtremBPsForAttaching.append(bp);
                }
                else
                    tempMultMap.insert(bp->BP_Type, bp->BP_ID);
            }
            else // this is a mirror which will be deleted for now
                 // and later resymmetrified
                 // TODO: Inspect this part.
            {
                // keep a note of those BPs that have mirrors so they can be re-symmetrified later
                tempSubMirrBPsIDs << bp->BP_ID;
            }
        }
        qDebug() << "reached 10";
        QVector<int> sameTypeSourceBP_IDs; sameTypeSourceBP_IDs.clear();
        while (!tempMultMap.isEmpty())
        {
            int key = tempMultMap.firstKey();
            sameTypeSourceBP_IDs.append(tempMultMap.values(key));
            tempMultMap.remove(key);
            sameTypeSourceBP_IDs.removeOne(mutatedBP_Ptr->BP_ID);
            if (!sameTypeSourceBP_IDs.contains(mutatedBP_Ptr->BP_ID)) // Sub BPs list must not include parent BP's ID (naturally)
            {
                QVector<QVector3D> attachmentPnts, attachmentVectors;   attachmentPnts.clear(); attachmentVectors.clear();
                QVector<int> boneLocPairList; boneLocPairList.clear();
                // This does 'group attachments'
                attachBodyPartToAnotherBodyPart(alienPtr, sameTypeSourceBP_IDs, mutatedBP_Ptr->BP_ID, true,
                                                attachmentPnts, attachmentVectors, boneLocPairList, false);
                // now re-symmetrify these sameTypeSourceBP_IDs if previously noted
                sameTypeSourceBP_IDs.clear();
            }
            else
            {
                qDebug() << "Found the parent BP's ID in the list of sub BPs, which is a problem";
                exit(0);
            }
        }
        if (!tempSubMirrBPsIDs.isEmpty())
            removeAlienBodyParts(alienPtr, tempSubMirrBPsIDs);

        if (mutatedBP_Ptr->BP_Type == AlienModeller::BodyPartsEnum::ExtremBP_Carpal
                ||
            mutatedBP_Ptr->BP_Type == AlienModeller::BodyPartsEnum::ExtremBP_Digit)
        {
            QVector<BodyPart*> carpals; carpals.clear();
            carpals.append(mutatedBP_Ptr);
            attachDigitsToCarpalsForNewExtremBP(alienPtr, tempSubExtremBPsForAttaching, carpals);
        }
        qDebug() << "reached 11";
        // finally re-attach the detached extremities to this re-JATed BP
        // attachExtremBP_ToParentBP(alienPtr, inBP_Ptr, subExtremIDs);
    }
    else
    {
        QVector<BodyPart*> tempClonedBPs; tempClonedBPs.clear();
        for (int s=0; s<tempSubBPs.length(); s++)
        {
            // clone each BP - NOTE: THis can make the performance really slow. Consider re-attaching these BPs and their mirrors in a clever manner
            qDebug() << "reached SquashStretch - 1";
            if (!tempSubBPs[s]->isSymmetric)
            {
                // TODO: Why are we cloning it? Check if we can do it without having to clone the detached subBP
                tempClonedBPs.append(cloneSkeletalBP_FromAnotherAlien_viaDNA(alienPtr, alienPtr, tempSubBPs[s]));
            }
            // else
            tempSubMirrBPsIDs << tempSubBPs[s]->BP_ID;
            if (s == tempSubBPs.length()-1) // if this is the last loop iteration, then remove all the previous BPs,
                                            // ... assign their IDs to the cloned BPs, and then attach the clones back to
                                            // ... the parent BP
            {
                // replace the detached sub BPs with the new clones
                removeAlienBodyParts(alienPtr, tempSubMirrBPsIDs);

                // TODO: Perhaps use MultiHash here
                // Multimap to store <BP Type, BP ID> relation
                QMultiMap<int,int> tempMultMap; tempMultMap.clear();
                for (BodyPart *bp : tempClonedBPs)
                {
                    alienPtr->alienSkeleton.Jnt2JntRelations_TypeGuided.insert(bp->cloneFromAlienAndBP_Pair.second,
                                            alienPtr->alienSkeleton.Jnt2JntRelations_TypeGuided.value(bp->BP_ID));
                    alienPtr->alienSkeleton.Jnt2JntRelations_TypeGuided.remove(bp->BP_ID);

                    bp->BP_ID = bp->cloneFromAlienAndBP_Pair.second;
                    bp->cloneFromAlienAndBP_Pair = QPair<int,int>(-1,-1);

                    for (int exID : bp->subExtremityBPsIDs)
                    {
                        alienPtr->extremToParentBP_Map.insert(exID,bp->BP_ID);
                    }

                    tempMultMap.insert(bp->BP_Type, bp->BP_ID);
                }
            }
        }
    }


    // TODO(ismail): Fix this as this doesn't work properly.
    // ... The mirrored BPs don't appear. UPDATE: It seems to be working fine lately
    if (mutatedBP_Ptr->BP_Type == AlienModeller::BodyPartsEnum::BACK_BONE)
    {
        attachAllSkeletalBodyPartsToBackBone(alienPtr);
        symmetrifyAlienBodyParts(alienPtr);
    }

    qDebug() << "Squash/stretch done successfully";

    return mutatedBP_Ptr;
}

void AlienModeller::adjustTotalLenghtsOfBPs(Alien *alienPtr, QVector<BodyPart *> &BP_PtrList)
{
    QMap<int, float> sortedTotLengthsMap; sortedTotLengthsMap.clear();
    float avgLen = sortBPsAccordingToTotalLengths(alienPtr, BP_PtrList, sortedTotLengthsMap);
    // DEBUG
    qDebug() << "avgLen : " << avgLen;
    // DEBUG
    for (BodyPart *bpPtr : BP_PtrList)
        qDebug() << "len : " << sortedTotLengthsMap.value(bpPtr->BP_ID) << " / dist from avg len : " << sortedTotLengthsMap.value(bpPtr->BP_ID) - avgLen;

    float avgMargin = 50.f;

    if (!sortedTotLengthsMap.isEmpty())
    {
        QMapIterator<int,float> iter1(sortedTotLengthsMap);
        while (iter1.hasNext())
        {
            iter1.next();
            float bpLen = iter1.value();
            float margin = bpLen - avgLen;
            int bpID = iter1.key();
            BodyPart *bpPtr = getBodyPartFromID(alienPtr, bpID);
            if (margin < 0)
            {
                while (fabs(margin) < avgMargin) // we need to stretch this BP
                {
                    bpPtr = squashOrStrechBP(alienPtr, bpID, 0.0, 1.1);
                    bpLen = getTotalLengthOfSkeletalBP(alienPtr, bpPtr);
                    margin = bpLen - avgLen;
                }
            }
            else if (margin > 0)
            {
                while (margin > avgMargin) // we need to squash this BP
                {
                    bpPtr = squashOrStrechBP(alienPtr, bpID, 0.0, 0.9);
                    bpLen = getTotalLengthOfSkeletalBP(alienPtr, bpPtr);
                    margin = bpLen - avgLen;
                }
            }
        }
    }
    else
    {
        qDebug() << "Something's wrong. sortedTotLengthsMap is empty in adjustTotalLenghtsOfBPs method";
        exit(0);
    }
}

void AlienModeller::initSkeletalBP_BulkinessDNA(Alien *alien)
{
    for (int i=0; i<alien->bodyParts.length(); i++)
    {
        QPolygonF tempPoly; tempPoly.resize(0);
        if (alien->bodyParts[i]->BP_Type != AlienModeller::BodyPartsEnum::JOINT  &&
                alien->bodyParts[i]->BP_Type != AlienModeller::BodyPartsEnum::BONE)
        {
            for (int j=0; j<alien->bodyParts[i]->subBodyParts.length(); j++)
            {
                if (alien->bodyParts[i]->subBodyParts[j]->BP_Type == AlienModeller::BodyPartsEnum::BONE)
                {
                    tempPoly << QPointF(3,1) << QPointF(30,1.1) << QPointF(1,0);
                }
            }
            alien->alienDNA->skeletonDNA->Skeletal_BP_Bulkiness_DNA_ForEachBP.insert(alien->bodyParts[i]->BP_ID,tempPoly);
        }
    }
}

void AlienModeller::updateBoneBendingDNA(Alien *alienPtr)
{
    qDebug() << "updating bend bone DNA";
    // for each bone of this alien, specify the bone bending DNA
    for (int i=0; i<alienPtr->alienBones.length(); i++)
    {
        int bone_id = alienPtr->alienBones[i]->BP_ID;
        float numOfBends = static_cast<float>(GeometryAlgorithms::getRandomInteger(skeletonRuleSet->BoneBending_RS[0].x(), skeletonRuleSet->BoneBending_RS[0].y()));
        QPolygonF tempPoly; tempPoly.resize(0);
        if (numOfBends > 0)
        {
            float firstPercOfBoneForBending = static_cast<float>(GeometryAlgorithms::getRandomInteger(skeletonRuleSet->BoneBending_RS[1].x(), skeletonRuleSet->BoneBending_RS[1].y()));
            float secondPercOfBoneForBending = static_cast<float>(GeometryAlgorithms::getRandomInteger(static_cast<int>(firstPercOfBoneForBending), skeletonRuleSet->BoneBending_RS[1].y()));
            float firstBendSharpness = GeometryAlgorithms::getRandomFloat(static_cast<float>(skeletonRuleSet->BoneBending_RS[2].x()), static_cast<float>(skeletonRuleSet->BoneBending_RS[2].y()));
            float secondBendSharpness = GeometryAlgorithms::getRandomFloat(static_cast<float>(skeletonRuleSet->BoneBending_RS[2].x()), static_cast<float>(skeletonRuleSet->BoneBending_RS[2].y()));
            float smoothnessAmount = static_cast<float>(GeometryAlgorithms::getRandomInteger(skeletonRuleSet->BoneBending_RS[3].x(), skeletonRuleSet->BoneBending_RS[3].y()));
            float smoothnessRequired = 0;
            if (smoothnessAmount > 0)
                smoothnessRequired = 1;

            tempPoly << QPointF(static_cast<float>(bone_id),numOfBends) << QPointF(firstPercOfBoneForBending,secondPercOfBoneForBending)
                     << QPointF(firstBendSharpness,secondBendSharpness) << QPointF(smoothnessRequired,smoothnessAmount);
        }

//        alienPtr->alienDNA->skeletonDNA->BoneBending_DNA.remove(bone_id);
        alienPtr->alienDNA->skeletonDNA->BoneBending_DNA.insert(bone_id, tempPoly);
    }
    qDebug() << "bend bone DNA updated";
}

void AlienModeller::modifySkeletalBP_BulkinessDNA(Alien *alien, QVector<int> &BP_ID_List_toMutate, int incDecBP_Bulkiness,
                                                  int sqwashOrStretchBP, float changeBulkTrend, QPointF XY_ScalingFlags, QPointF scaleValues)
{

    qDebug() << "Modifying Bulkiness DNA";

/*
 * ******** ALGORITHM 1: A heavy weight algorithm that involves deleting all the BPs (which is unnecessary)
 *                       and then remodelling them with new bulkiness DNA entries *********
 *
 * modify the DNA for the incoming BPs
 * Delete all the Alien's BPs
 *
 * trigger the alienmodeller buildAlienSkeleton_TypeGuided() to re-model/re-generate the specified BPs with the modified DNA specs
 *
*/

/*
    // generate a random list of BPs. Next time send a pre-made list of BPs that need to be modified
    BP_ID_List_toMutate.append(alien->alienBackBones.at(0)->BP_ID);

    // modify its Bulkiness DNA
    manipulateSkeletalBulkinessDNA_Flags(alien, false, false, true, false, true, new QVector<int>(0), &BP_ID_List_toMutate);

    alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.VectorEnergyAngleTrends_TypeGuided.clear();

    BP_ToType_Map.clear();
    for (int i=0; i<alien->bodyParts.length(); i++)
    {
        if (alien->bodyParts[i]->BP_Type != AlienModeller::BodyPartsEnum::JOINT  &&
                alien->bodyParts[i]->BP_Type != AlienModeller::BodyPartsEnum::BONE)
        {
            BP_ToType_Map.insert(alien->bodyParts[i]->BP_ID, alien->bodyParts[i]->BP_Type);
        }
    }

    // Remove/delete all Alien's BPs
    QVector<int> BP_ID_List; BP_ID_List.clear();
    getAllBP_ID_List(alien, BP_ID_List);
    removeAlienBodyParts(alien, BP_ID_List);

    // trigger the alienmodeller buildAlienSkeleton_TypeGuided() to re-model/re-generate the specified BPs with the modified DNA specs
    buildAlienSkeleton_TypeGuided(alien);
    attachAllSkeletalBodyPartsToBackBone(alien);
*/


    // *** Algorithm 2: A lightweight algorithm that only works on the BPs whose bulkiness DNA are to be mutated ***
    //
    // generate a random list of BPs. Next time send a pre-made list of BPs that need to be modified
//    if (BP_ID_List_toMutate.isEmpty())
//        BP_ID_List_toMutate.append(alien->alienBackBones.at(0)->BP_ID);

//    BodyPart *bpPtr = getBodyPartFromID(alien, BP_ID_List_toMutate[0]);
//    if (bpPtr->isSymmetric)
//    {
//        bpPtr = getMirrorOrOriginalBP(alien, bpPtr);
//        BP_ID_List_toMutate[0] = bpPtr->BP_ID;
//    }

    if (incDecBP_Bulkiness != -1) // increase or dec bulkiness
    {
        increaseOrReduceBP_Bulkiness(alien, BP_ID_List_toMutate[0], incDecBP_Bulkiness, 2);
    }
    else if (changeBulkTrend != -1  &&  XY_ScalingFlags == QPointF(-1,-1)) // reverse trend
    {
        changeBP_BulkinessTrend(alien, BP_ID_List_toMutate[0], changeBulkTrend);
    }
    else // this means we need to mutate the bulkificaiton completely (randomly) and then modify it based on the input
         // ... values of trend and X-Y scaling
    {
        // indicate the system that we need to modify this BP's Bulkiness DNA
        manipulateSkeletalBulkinessDNA_Flags(alien, false, false, true, false, true, new QVector<int>(0), &BP_ID_List_toMutate);
        // Now rebuild the Bulkification DNA - initially completely randomly
        buildSkeletalBP_BulkinessDNA(alien, BP_ID_List_toMutate);

        if (changeBulkTrend != -1)  // Now specify a new Bulk Trend - as provided by method parameter
        {
            changeBP_BulkinessTrend(alien, BP_ID_List_toMutate[0], changeBulkTrend);
        }
        if (XY_ScalingFlags.x() == 1  ||  XY_ScalingFlags.y() == 1) // Now specify X-Y scaling axes flags - as provided as method parameter
        {
            changeBP_Bulkiness_XY_Scaling(alien, BP_ID_List_toMutate[0], XY_ScalingFlags);
        }
        // Also change the starting scaling value and scaling multiplier if provided by the method input
        if (scaleValues != QPointF(-1,-1))
        {
            changeBP_BulkinessScalingValues(alien, BP_ID_List_toMutate[0], scaleValues.x(), scaleValues.y());
        }
    }

    if (sqwashOrStretchBP != -1)
    {
        if (sqwashOrStretchBP == 0)
        {
            for (int i=3; i<alien->alienDNA->skeletonDNA->Skeletal_BP_Bulkiness_DNA_ForEachBP.value(BP_ID_List_toMutate[0]).length(); i+=4)
            {
                alien->alienDNA->skeletonDNA->Skeletal_BP_Bulkiness_DNA_ForEachBP[BP_ID_List_toMutate[0]][i].setX(0);
                alien->alienDNA->skeletonDNA->Skeletal_BP_Bulkiness_DNA_ForEachBP[BP_ID_List_toMutate[0]][i].setY(0.8);
            }
        }

        else if (sqwashOrStretchBP == 1)
        {
            for (int i=3; i<alien->alienDNA->skeletonDNA->Skeletal_BP_Bulkiness_DNA_ForEachBP.value(BP_ID_List_toMutate[0]).length(); i+=4)
            {
                alien->alienDNA->skeletonDNA->Skeletal_BP_Bulkiness_DNA_ForEachBP[BP_ID_List_toMutate[0]][i].setX(1);
                alien->alienDNA->skeletonDNA->Skeletal_BP_Bulkiness_DNA_ForEachBP[BP_ID_List_toMutate[0]][i].setY(1.2);
            }
        }
    }


    // remove BP bones' cells and remodel cells VS remove BP and remodel it keeping the old BP IDs
    // TODO(ismail): twisted bones problem - Try moving the BP back to the origin and unrotate it using its inverse translation and
    //               ... quaternion matrices, then do the scalings, then move it back to the current location
    //               ... and rotate it back to the its orientation
    //               ... Hopefully this will solve the problem of twisted bones
    for (int i=0; i<BP_ID_List_toMutate.length(); i++)
    {
        BodyPart *bpPtr = getBodyPartFromID(alien, BP_ID_List_toMutate[i]);

        // take the BP back to the origin
        GeometryAlgorithms::translateBP_AccordingToInputTranslationMat(bpPtr, bpPtr->BP_TranslateMatrix.inverted());
        // Unrotate the BP (using inverse quaternion)
        GeometryAlgorithms::rotateBP_AccordingToInputRotationMat(bpPtr, bpPtr->BP_OrientationQuat.inverted());
        // update the cells box geometry
        GeometryAlgorithms::updateBP_CellPrimitiveGeometry(bpPtr);

        // Delete all the cells of the bones of this BP
        // TODO(ismail): Need to make a seperate method
        // TODO(ismail): Need to cater for the case when the BP has been 'Densified' or 'Bulgified'
        // ... as it will have sub-cells
        QVector<Bone *> boneList; boneList.clear();
        getAllBP_Bones(bpPtr, boneList);
        for (int b=0; b<boneList.length(); b++)
        {
            boneList[b]->allStackedDenserLineages.clear();
            boneList[b]->subdivisionCells.clear();
            while (!boneList[b]->cellList.empty()) delete boneList[b]->cellList.last(), boneList[b]->cellList.removeLast();
            boneList[b]->BP_TranslateMatrix.setToIdentity();
            boneList[b]->BP_OrientationQuat = QQuaternion::fromDirection(QVector3D(0,0,1), QVector3D(0,1,0));

            // if (boneList[b]->deleteAllBP_Cells(false))
            // {
                int firJnt = boneList.at(b)->endJoints.first;
                int secJnt = boneList.at(b)->endJoints.second;
                QVector3D firJntLoc = getBodyPartFromID(alien, firJnt)->getPivotPoint();
                QVector3D secJntLoc = getBodyPartFromID(alien, secJnt)->getPivotPoint();

                createSimpleBone(*boneList[b], firJntLoc, secJntLoc);
            // }
            // else
            // {
            //     qDebug() << "all cells not deleted";
            //     exit(0);
            // }
        }
    }

    bulkUpBodyPart(alien, BP_ID_List_toMutate);   

    BodyPart *bpPtr = getBodyPartFromID(alien, BP_ID_List_toMutate[0]);

    // rotate it back to its original orientation
    GeometryAlgorithms::rotateBP_AccordingToInputRotationMat(bpPtr, bpPtr->BP_OrientationQuat);
    // take it back to its actual location
    GeometryAlgorithms::translateBP_AccordingToInputTranslationMat(bpPtr, bpPtr->BP_TranslateMatrix);
    // update the cells box geometry
    GeometryAlgorithms::updateBP_CellPrimitiveGeometry(bpPtr);

    // TODO(ismail): mirror must also be bulkified accordingly
    // Solution 1: Delete its mirror and symmetrify the BP again
    // Solution 2: Delete the entire alien and then clone the alien

    // // Solution 1:
    // // First get its mirror (if any)
    // if (!bpPtr->isSymmetric)
    // {
    //     BodyPart *mirrPtr = getMirrorOrOriginalBP(alien, bpPtr);
    //     if (mirrPtr)
    //     {
    //         // Delete its mirror
    //         QVector<int> tempMirList; tempMirList.clear();
    //         tempMirList << mirrPtr->BP_ID;
    //         removeAlienBodyParts(alien, tempMirList);

    //         // Symmetrify again
    //         BodyPart *tempBP_Ptr = symmetrifyBP_AroundOppositeAttachmentFace(alien, bpPtr);
    //     }
    // }

    qDebug() << "Bulkiness DNA modified and BP(s) remodelled";
}

void AlienModeller::modifyNumOfSkeletalBPsDNA(Alien *alien, int BP_Type)
{
    /*
     * ALGORITHM 1:
     * This algorithm modifies the DNA first, then removes all the BPs from the Alien, and finally calls the
     * buildAlienSkeleton method, just like the strategy we used for modifying Bulkiness DNA
     *
     *  Change the number of BPs (for that BP Type) value in the numOfSkeletalBodyParts_DNA DS for the given BP type
     *
     *  if (the new value is less than the previous num of BPs for that type)
     *      remove the same amount of entries from the following DNAs: numOfJointsPerBP_DNA, skeletal_JATs_DNA,
     *      BP_SizeComparedToBackBone_DNA, BP_AngleArrangementsFromBackBone_DNA, BP_JAT_rootJoint_DNA,
     *
     *      Remove the Joint entries from BP_to_JAT_rootJoint_Map_DNA, VectorEnergyAngleTrends_TypeGuided,
     *      randIntForBookKeeping_EnergyTrend, randFloatForBookKeeping_FinalEnergy, randIntForBookKeeping_AngleTrend,
     *      randIntForBookKeeping_quaterEulerAngleSelectorForFirstChild, randFloatForBookKeeping_quaterEulerAngleSelectorForNotFirstChild
     *
     *      remove any other entries of those BPs and its sub BPs (including joints and bones) from any other DS in the Alien Skeleton
     *      or skeletal modeller
     *
     *  else if (the new value is greater than the previous num of BPs for that type)
     *      // we need to add/create more BPs
     *      // First amend all the relevant DNAs with additional entries
     *
     *  remove all the BPs from the ALien
     *
     *  rebuild the skeleton
     *
     *
    */

    // get a random number between -2 and 3
    // if num of BPs + random num <= 0, then keep the previous num of BPs
    // else new num of BPs = num of BPs + random num
    int prevNumOfBPs = alien->alienDNA->skeletonDNA->numOfSkeletalBodyParts_DNA.value(BP_Type);
    int tempNum1 = /*prevNumOfBPs + */GeometryAlgorithms::getRandomInteger(1,7);
    if (tempNum1 > 0) // if more than 0, then we add it to the previous number, else keep the previous number
        alien->alienDNA->skeletonDNA->numOfSkeletalBodyParts_DNA[BP_Type] = tempNum1;

    int newNumOfBPs = alien->alienDNA->skeletonDNA->numOfSkeletalBodyParts_DNA.value(BP_Type);
    if (newNumOfBPs == prevNumOfBPs)
    {
        alien->alienDNA->skeletonDNA->numOfSkeletalBodyParts_DNA[BP_Type] = ++newNumOfBPs;
    }

    qDebug() << "previous num of BPs : " << prevNumOfBPs << ", New number of BPs : " << newNumOfBPs;
    if (newNumOfBPs < prevNumOfBPs) // delete some BPs DNA entries
    {
        qDebug() << "New num of BPs is less than previous number of BPs";
        // First note the BP IDs
        // TODO(ismail): Perhaps make a method that can remove all these BPs entries from the DNA datastructures.
        //               ... Such a method may be useful later
        QVector<int> BP_ID_listToDelete; BP_ID_listToDelete.clear();
        for (int i=0; i<(prevNumOfBPs-newNumOfBPs); i++)
        {
            // TODO(ismail): we are just removing the last one. Perhaps make it more robust
            // ... by deleting a user given BP
            int tempBP_ID =  alien->alienDNA->skeletonDNA->numOfJointsPerBP_DNA.value(BP_Type).last().x();
            alien->alienDNA->skeletonDNA->numOfJointsPerBP_DNA[BP_Type].removeLast();

            // TODO(ismail): Make the following more robust.
            //               ... For example. The Point of QPolgyon in BP_AngleArrangementsFromBackBone_DNA
            //               ... has the x value as non-BP_ID value at the moment
            alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.BP_JAT_rootJoint_DNA[BP_Type].removeLast();
            alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.BP_JAT_rootJoint_DNA[BP_Type].removeLast();
            alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.BP_to_JAT_rootJoint_Map_DNA.remove(tempBP_ID);
            alien->alienDNA->skeletonDNA->BP_SizeComparedToBackBone_DNA[BP_Type].removeLast();
            alien->alienDNA->skeletonDNA->BP_AngleArrangementsFromBackBone_DNA[BP_Type].removeLast();

//            alien->alienDNA->skeletonDNA->Skeletal_BP_Bulkiness_2_DNA.remove(tempBP_ID);

            // get a list of all the joints of the BPs to be removed and remove the relevant entries from DNA ds
            QVector<Joint *> BP_JntsList; BP_JntsList.clear();
            getAllBP_Joints(getBodyPartFromID(alien, tempBP_ID), BP_JntsList);
            for (int j=0; j<BP_JntsList.length(); j++)
            {
                alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.VectorEnergyAngleTrends_TypeGuided.remove(BP_JntsList[j]->BP_ID);
                alien->alienSkeleton.vectorLocationCoupleForEachJoint_TypeGuided.remove(BP_JntsList[j]->BP_ID);
                alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.randIntForBookKeeping_EnergyTrend.remove(BP_JntsList[j]->BP_ID);
                alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.randFloatForBookKeeping_FinalEnergy.remove(BP_JntsList[j]->BP_ID);
                alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.randIntForBookKeeping_AngleTrend.remove(BP_JntsList[j]->BP_ID);
                alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.randIntForBookKeeping_quaterEulerAngleSelectorForFirstChild.remove(BP_JntsList[j]->BP_ID);
                alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.randQVector3DForBookKeeping_quaterEulerAngleSelectorForNotFirstChild.remove(BP_JntsList[j]->BP_ID);

                if (alien->alienSkeleton.JntToJntMappings_ForBookKeeping.remove(BP_JntsList[j]->BP_ID))
                    qDebug() << "entry for jnt of this BP removed from JntToJntMappings_ForBookKeeping DS";
            }
            BP_ID_listToDelete << tempBP_ID;
        }
        // specify the Bulkiness flags
        manipulateSkeletalBulkinessDNA_Flags(alien, false, false, false, true, false, &BP_ID_listToDelete, new QVector<int>(0));
    }
    else if (newNumOfBPs > prevNumOfBPs) // New BPs need to be created, so add more DNA entries for the new BPs
    {
        qDebug() << "New num of BPs is more than previous number of BPs";
        for (int i=0; i<(newNumOfBPs-prevNumOfBPs); i++)
        {
            // TODO(ismail): The following values should be randomly selected from the ruleset (numOfJointsPerBP_RS)
            alien->alienDNA->skeletonDNA->numOfJointsPerBP_DNA[BP_Type].append(QPoint(-1, GeometryAlgorithms::getRandomInteger(4,7)));
            alien->alienDNA->skeletonDNA->BP_SizeComparedToBackBone_DNA[BP_Type].append(QPoint(-1,GeometryAlgorithms::getRandomInteger(10,300)));
            alien->alienDNA->skeletonDNA->BP_AngleArrangementsFromBackBone_DNA[BP_Type].append(QPoint(-1,GeometryAlgorithms::getRandomInteger(10,60)));

            // Append the same energy and angle trends as the last BP in the BP_JAT_rootJoint_DNA (for its root Jnt)
            QPolygonF tempPoly = alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.BP_JAT_rootJoint_DNA.value(BP_Type);
            QPointF pt1 = tempPoly.last(); // angle trend
            tempPoly.removeLast();
            QPointF pt2 = tempPoly.last(); // energy trend
            alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.BP_JAT_rootJoint_DNA[BP_Type].append(pt2);
            alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.BP_JAT_rootJoint_DNA[BP_Type].append(pt1);
        }
        // specify the Bulkiness flags
        manipulateSkeletalBulkinessDNA_Flags(alien, false, false, false, false, true, new QVector<int>(0), new QVector<int>(0));
    }

    BP_ToType_Map.clear();
    for (int i=0; i<alien->bodyParts.length(); i++)
    {
        if (alien->bodyParts[i]->BP_Type != AlienModeller::BodyPartsEnum::JOINT  &&
                alien->bodyParts[i]->BP_Type != AlienModeller::BodyPartsEnum::BONE)
        {
            BP_ToType_Map.insert(alien->bodyParts[i]->BP_ID, alien->bodyParts[i]->BP_Type);
        }
    }

    // Remove/Delete all Alien's BPs
    QVector<int> BP_ID_List_2; BP_ID_List_2.clear();
    QVector<int> tempSepBP_IDs; tempSepBP_IDs.clear();
    getAllBP_ID_List(alien, BP_ID_List_2, tempSepBP_IDs);
    removeAlienBodyParts(alien, BP_ID_List_2);

    // trigger the alienmodeller buildAlienSkeleton_TypeGuided() to re-model/re-generate the specified BPs with the modified DNA specs
    buildAlienSkeleton_TypeGuided(alien);

    attachAllSkeletalBodyPartsToBackBone(alien);
}

void AlienModeller::modifyNumOfJointsPerBP_DNA(Alien *alien, QVector<int> &BP_ID_List)
{
    /*
     * ALGORITHM 1:
     * This algorithm modifies the DNA first, then removes all the BPs from the Alien, and finally calls the
     * buildAlienSkeleton method, just like the strategy we used for modifying Bulkiness DNA
     *
     *  change the number of Jts for the incoming BPs
     *
     *  remove all the BPs from the ALien
     *
     *  rebuild the skeleton
     *
     *  attach the BPs
    */

    /*  ******* ALGORITHM 2 *********
        // First of all remove all the entries of its joints in:
        //      VectorEnergyAngleTrends_TypeGuided
        //      VectorLocationCoupleForEachJoint_TypeGuided
        //      JntToJntMappings_ForBookKeeping
        //      randIntForBookKeeping_EnergyTrend
        //      randFloatForBookKeeping_FinalEnergy
        //      randIntForBookKeeping_AngleTrend
        //      randIntForBookKeeping_quaterEulerAngleSelectorForFirstChild
        //      randQVector3DForBookKeeping_quaterEulerAngleSelectorForNotFirstChild

        // Specify how to build the Bulkiness DNA via calling manipulateSkeletalBulkinessDNA_Flags() method

        // Remove this BP from the Alien but keep a note of its ID

        // Create a new BP and assign it the same ID as before

        // Instantiate the joints for this BP

        // Call specifyJnt2JntRelations_TypeGuided() to update the Jnt2JntRelations_TypeGuided

        // call initializeJointsArrangementTrendForCurrentAlien() to update the VectorEnergyAngleTrends_TypeGuided DS
        // ... in the JointsArrangementTrend of Skeleton_DNA

        // update vectorLocationCoupleForEachJoint_TypeGuided with dummy values for the joints of this new BP

        // call compute_JAT_AND_VecLocCplForEachJnt_ForBP() and generate it at the origin

        // model all its joints

        // model all its bones

        // bulkup this BP

        // return this BP pointer
    */


    for (int i=0; i<BP_ID_List.length(); i++)
    {
        BodyPart *tempBP_Ptr = getBodyPartFromID(alien, BP_ID_List[i]);
        int tempBP_ID = tempBP_Ptr->BP_ID;
        int tempSourceID = tempBP_Ptr->clonedFrom;
        QPair<int,int> tempSrcClonedFrom = tempBP_Ptr->cloneFromAlienAndBP_Pair;
        int tempBP_Tyep = tempBP_Ptr->BP_Type;
        bool isSymmetric = tempBP_Ptr->isSymmetric;
        int mirroredFrom = tempBP_Ptr->mirrorOf;

        QPolygon tempPoly = alien->alienDNA->skeletonDNA->numOfJointsPerBP_DNA.value(tempBP_Ptr->BP_Type);
        QPolygon tempPoly2; tempPoly2.clear();
        for (int j=0; j<tempPoly.length(); j++)
        {
            if (tempPoly[j].x() == tempBP_Ptr->BP_ID)
            {
                tempPoly[j].setY(GeometryAlgorithms::getRandomInteger(3,10));
//                tempPoly[j].setY(tempPoly[j].y());
                tempPoly2 << tempPoly[j];
                break;
            }
        }
        alien->alienDNA->skeletonDNA->numOfJointsPerBP_DNA.insert(tempBP_Ptr->BP_Type,tempPoly);

        // TODO(ismail): Put the code below inside the removeAlienBodyParts() method
        // We need to remove all the entries in the DNAs / book-keeping DNAs related to these joints
        // First Get all the joints of this BP
        QVector<Joint *> BP_JntsList; BP_JntsList.clear();
        getAllBP_Joints(tempBP_Ptr, BP_JntsList);

        removeAssociatedJATsAndOtherDSsForRemovableJnts(alien, getAllJntIDsOfBP(tempBP_Ptr));

        QVector<int> tempBP_ID_List_1; tempBP_ID_List_1.clear();
        tempBP_ID_List_1 << tempBP_Ptr->BP_ID;
        removeAlienBodyParts(alien, tempBP_ID_List_1);

        instantiateNewBodyParts(alien, tempBP_Tyep, 1);
        BodyPart *newBP_Ptr = alien->bodyParts.last();
        newBP_Ptr->BP_ID = tempBP_ID;
        newBP_Ptr->clonedFrom = tempSourceID; // TODO(ismail): This should be checked that whether the source
                                                   // ... still exists or not
        newBP_Ptr->cloneFromAlienAndBP_Pair = tempSrcClonedFrom;

        newBP_Ptr->isSymmetric = isSymmetric;
        newBP_Ptr->mirrorOf = mirroredFrom;

        QMap<int, QPolygon> tempNumOfJointsPerBP_DNA; tempNumOfJointsPerBP_DNA.clear();
        tempNumOfJointsPerBP_DNA.insert(newBP_Ptr->BP_Type, tempPoly2);
        instantiateNumOfJointsForAllBPs_TypeGuided(alien, tempNumOfJointsPerBP_DNA);

        QVector<BodyPart*> tempBP_Ptr_List_1; tempBP_Ptr_List_1.clear();
        tempBP_Ptr_List_1 << newBP_Ptr;
        specifyJnt2JntRelations_TypeGuided(alien, tempBP_Ptr_List_1, false);

        BP_JntsList.clear();
        getAllBP_Joints(newBP_Ptr, BP_JntsList);
        initializeJointsArrangementTrendForCurrentAlien(alien, BP_JntsList, false);

        QMap<int,int> tempParentJntCompletionCheckList; // checklist for when the all the child joints's positions are computed. Value(int) is 1 for DONE and 0 for NOT_DONE.
        QMap<int,int> tempChildJntPositionCheckList;    // checklist for when the joint's position is computed.

        // Update the bookkeeping data structures in the JAT DNA
        for (Joint *jntPtr : BP_JntsList)
        {
            QPair<QVector3D, QVector3D> tempPair;
            tempPair.first = QVector3D(0.0,0.0,1.0); tempPair.second = QVector3D(0.0,0.0,0.0);
            alien->alienSkeleton.vectorLocationCoupleForEachJoint_TypeGuided.insert(jntPtr->BP_ID, tempPair);

            tempParentJntCompletionCheckList.insert(jntPtr->BP_ID,0);
            tempChildJntPositionCheckList.insert(jntPtr->BP_ID,0);
        }

        QMap<int, QPolygonF> tempCopy_BP_JAT_rootJoint_DNA;
        tempCopy_BP_JAT_rootJoint_DNA.insert(newBP_Ptr->BP_Type,
               alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.BP_to_JAT_rootJoint_Map_DNA.value(newBP_Ptr->BP_ID));

        compute_JAT_AND_VecLocCplForEachJnt_ForBP(alien, newBP_Ptr->BP_ID, tempCopy_BP_JAT_rootJoint_DNA,
                                                  tempParentJntCompletionCheckList, tempChildJntPositionCheckList,
                                                  0.0);
        // model the joints of this BP
        modelAllAlienJoints_TypeGuided(alien, BP_JntsList);

        QMap<int, QPolygon*> tempJnt2JntRelations;
        QPolygon *tempNewPolyPtr = new QPolygon(*(alien->alienSkeleton.Jnt2JntRelations_TypeGuided.value(newBP_Ptr->BP_ID)));
        tempJnt2JntRelations.insert(newBP_Ptr->BP_ID, tempNewPolyPtr);
        // model the bones of this BP
        modelAllAlienBones_TypeGuided(alien, tempJnt2JntRelations);
    }

    manipulateSkeletalBulkinessDNA_Flags(alien, false, false, true, false, false, new QVector<int>(0), &BP_ID_List);

    buildSkeletalBP_BulkinessDNA(alien, BP_ID_List);
    bulkUpBodyPart(alien, BP_ID_List);


/*
    // change the number of Jts for the incoming BPs
    for (int i=0; i<BP_ID_List.length(); i++)
    {
        BodyPart *tempBP_Ptr = getBodyPartFromID(alien, BP_ID_List[i]);
        QPolygon tempPoly = alien->alienDNA->skeletonDNA->numOfJointsPerBP_DNA.value(tempBP_Ptr->BP_Type);
        for (int j=0; j<tempPoly.length(); j++)
        {
            if (tempPoly[j].x() == tempBP_Ptr->BP_ID)
            {
                tempPoly[j].setY(GeometryAlgorithms::getRandomInteger(2,15));
            }
        }
        alien->alienDNA->skeletonDNA->numOfJointsPerBP_DNA.insert(tempBP_Ptr->BP_Type,tempPoly);
    }

    manipulateSkeletalBulkinessDNA_Flags(alien, false, false, true, false, true, new QVector<int>(0), &BP_ID_List);

    BP_ToType_Map.clear();
    for (int i=0; i<alien->bodyParts.length(); i++)
    {
        if (alien->bodyParts[i]->BP_Type != AlienModeller::BodyPartsEnum::JOINT  &&
                alien->bodyParts[i]->BP_Type != AlienModeller::BodyPartsEnum::BONE)
        {
            BP_ToType_Map.insert(alien->bodyParts[i]->BP_ID, alien->bodyParts[i]->BP_Type);
        }
    }

    // Remove/Delete all Alien's BPs
    QVector<int> BP_ID_List_2; BP_ID_List_2.clear();
    getAllBP_ID_List(alien, BP_ID_List_2);
    removeAlienBodyParts(alien, BP_ID_List_2);

    alien->BP_ReplacementMap.clear();

    // trigger the alienmodeller buildAlienSkeleton_TypeGuided() to re-model/re-generate the specified BPs with the modified DNA specs
    buildAlienSkeleton_TypeGuided(alien);

    attachAllSkeletalBodyPartsToBackBone(alien);
*/

}

// incOrDec = true means add new BP. incOrDec = false means remove BPs.
// changeNum is the num of BPs to inc/dec
void AlienModeller::incDecNumOfBP(Alien *alien, int BP_Type, bool incOrDec, ushort changeNum, QVector<int> &newBP_IDs, BodyPart *bpToRemove)
{
    QVector<int> BP_ID_listToAddOrDelete; BP_ID_listToAddOrDelete.clear();
    if (!incOrDec) // remove BPs. delete some BPs DNA entries
    {
        qDebug() << "Removing a BP";

        // First note the BP IDs
        // TODO(ismail): Perhaps make a method that can remove all these BPs entries from the DNA datastructures.
        //               ... Such a method may be useful later
        BP_Type = bpToRemove->BP_Type;
        int tempBP_ID = bpToRemove->BP_ID;
        changeNum = 1;
        for (int i=0; i<changeNum; i++)
        {
            if (alien->alienDNA->skeletonDNA->numOfSkeletalBodyParts_DNA.value(BP_Type) == 0)
            {
                qDebug() << "There are no more BPs left to delete";
                break;
            }
            BP_ID_listToAddOrDelete.clear();
            // int tempBP_ID =  alien->alienDNA->skeletonDNA->numOfJointsPerBP_DNA.value(BP_Type).constLast().x();
            if (!alien->alienDNA->skeletonDNA->numOfJointsPerBP_DNA[BP_Type].isEmpty())
                alien->alienDNA->skeletonDNA->numOfJointsPerBP_DNA[BP_Type].removeLast();

            // TODO(ismail): Make the following more robust.
            //               ... For example. The Point of QPolgyon in BP_AngleArrangementsFromBackBone_DNA
            //               ... has the x value as non-BP_ID value (i.e. x value = -1) at the moment
            QPolygonF tPolyF = alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.BP_to_JAT_rootJoint_Map_DNA.value(tempBP_ID);
            for (int p=0; p<alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.BP_JAT_rootJoint_DNA[BP_Type].length()-1; p++)
            {
                if (tPolyF[0] == alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.BP_JAT_rootJoint_DNA[BP_Type].at(p)
                        &&
                    tPolyF[1] == alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.BP_JAT_rootJoint_DNA[BP_Type].at(p+1))
                {
                    alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.BP_JAT_rootJoint_DNA[BP_Type].remove(p,2);
                }
            }

            // We also need to remove the joints of this BP from the JAT DNA DSs
            // get a list of all the joints of the BPs to be removed and remove the relevant entries from JAT DNA DSs
            removeAssociatedJATsAndOtherDSsForRemovableJnts(alien, getAllJntIDsOfBP(bpToRemove));
            alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.BP_to_JAT_rootJoint_Map_DNA.remove(tempBP_ID);
            if (!alien->alienDNA->skeletonDNA->BP_SizeComparedToBackBone_DNA[BP_Type].isEmpty())
                alien->alienDNA->skeletonDNA->BP_SizeComparedToBackBone_DNA[BP_Type].removeLast();
            if (!alien->alienDNA->skeletonDNA->BP_AngleArrangementsFromBackBone_DNA[BP_Type].isEmpty())
                alien->alienDNA->skeletonDNA->BP_AngleArrangementsFromBackBone_DNA[BP_Type].removeLast();

            // TODO(ismail): We also need to remove this BP from
            // ... and BoneBending_DNA
            alien->alienDNA->skeletonDNA->BP_GenDensificationDNA.remove(tempBP_ID);
            alien->alienDNA->skeletonDNA->BP_SpecificDensificationDNA.remove(tempBP_ID);
            alien->alienDNA->skeletonDNA->BP_AttachmentToOtherBP_withAttachPositionAndVector_DNA.remove(tempBP_ID);

            BP_ID_listToAddOrDelete << tempBP_ID;
            // specify the Bulkiness flags
            manipulateSkeletalBulkinessDNA_Flags(alien, false, false, false, true, false, &BP_ID_listToAddOrDelete, new QVector<int>(0));
            // remove the BP now
            // TODO(ismail): Include all the DNA DSs removals (as above) in the following method: removeAlienBodyParts()
            removeAlienBodyParts(alien, BP_ID_listToAddOrDelete);
            qDebug() << "One BP removed successfully";
        }

/*
        // First note the BP IDs
        // TODO(ismail): Perhaps make a method that can remove all these BPs entries from the DNA datastructures.
        //               ... Such a method may be useful later
        alien->alienDNA->skeletonDNA->numOfSkeletalBodyParts_DNA[BP_Type] -= changeNum;
        for (int i=0; i<changeNum; i++)
        {
            if (alien->alienDNA->skeletonDNA->numOfSkeletalBodyParts_DNA.value(BP_Type) == 0)
            {
                qDebug() << "There are no more BPs left to delete";
                break;
            }
            BP_ID_listToAddOrDelete.clear();
            // TODO(ismail): we are just removing the last one. Perhaps make it more robust
            // ... by deleting a user given BP
            int tempBP_ID =  alien->alienDNA->skeletonDNA->numOfJointsPerBP_DNA.value(BP_Type).constLast().x();
            alien->alienDNA->skeletonDNA->numOfJointsPerBP_DNA[BP_Type].removeLast();

            // TODO(ismail): Make the following more robust.
            //               ... For example. The Point of QPolgyon in BP_AngleArrangementsFromBackBone_DNA
            //               ... has the x value as non-BP_ID value (i.e. x value = -1) at the moment
//            alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.BP_JAT_rootJoint_DNA[BP_Type].removeLast();
//            alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.BP_JAT_rootJoint_DNA[BP_Type].removeLast();
            QPolygonF tPolyF = alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.BP_to_JAT_rootJoint_Map_DNA.value(tempBP_ID);
            for (int p=0; p<alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.BP_JAT_rootJoint_DNA[BP_Type].length()-1; p++)
            {
                if (tPolyF[0] == alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.BP_JAT_rootJoint_DNA[BP_Type].at(p)
                        &&
                    tPolyF[1] == alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.BP_JAT_rootJoint_DNA[BP_Type].at(p+1))
                {
                    alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.BP_JAT_rootJoint_DNA[BP_Type].remove(p,2);
                }
            }
            // We also need to remove the joints of this BP from the JAT DNA DSs
            // get a list of all the joints of the BPs to be removed and remove the relevant entries from JAT DNA DSs
            removeAssociatedJATsAndOtherDSsForRemovableJnts(alien, getAllJntIDsOfBP(getBodyPartFromID(alien, tempBP_ID)));
            alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.BP_to_JAT_rootJoint_Map_DNA.remove(tempBP_ID);
            alien->alienDNA->skeletonDNA->BP_SizeComparedToBackBone_DNA[BP_Type].removeLast();
            alien->alienDNA->skeletonDNA->BP_AngleArrangementsFromBackBone_DNA[BP_Type].removeLast();

//            alien->alienDNA->skeletonDNA->Skeletal_BP_Bulkiness_2_DNA.remove(tempBP_ID);

            // TODO(ismail): We also need to remove this BP from
            // ... BP_GenDensificationDNA; BP_SpecificDensificationDNA;
            // ... and BP_AttachmentToOtherBP_withAttachPositionAndVector_DNA
            // ... and BoneBending_DNA
            alien->alienDNA->skeletonDNA->BP_GenDensificationDNA.remove(tempBP_ID);
            alien->alienDNA->skeletonDNA->BP_SpecificDensificationDNA.remove(tempBP_ID);
            alien->alienDNA->skeletonDNA->BP_AttachmentToOtherBP_withAttachPositionAndVector_DNA.remove(tempBP_ID);

            BP_ID_listToAddOrDelete << tempBP_ID;
            // specify the Bulkiness flags
            manipulateSkeletalBulkinessDNA_Flags(alien, false, false, false, true, false, &BP_ID_listToAddOrDelete, new QVector<int>(0));
            // remove the BP now
            // TODO(ismail): Include all the DNA DSs removals (as above) in the following method: removeAlienBodyParts()
            removeAlienBodyParts(alien, BP_ID_listToAddOrDelete);            

            // Also delete its mirror (if it exists)
*/
    }
    else // New BPs need to be created/added, so add more DNA entries for the new BPs
    {
        if (alien->alienDNA->skeletonDNA->numOfSkeletalBodyParts_DNA.value(BP_Type) <= 700)
        {
            qDebug() << "New num of BPs is more than previous number of BPs";
    //        alien->alienDNA->skeletonDNA->numOfSkeletalBodyParts_DNA[BP_Type] += changeNum;
            BodyPart *bbPtr = !alien->alienBackBones.isEmpty() ? alien->alienBackBones.first() : nullptr;
            QVector<BodyPart *> newBPsPtrsList; newBPsPtrsList.clear();
            for (int i=0; i<changeNum; i++)
            {
                newBPsPtrsList.append(this->modelNewBP_TypeGuided(alien, BP_Type));
                BP_ID_listToAddOrDelete.append(newBPsPtrsList.last()->BP_ID);
            }
            // now bulkify this new BP
            if (!BP_ID_listToAddOrDelete.isEmpty())
            {
                // specify the Bulkiness flags
                manipulateSkeletalBulkinessDNA_Flags(alien, false, false, false, false, true, new QVector<int>(0), new QVector<int>(0));
                this->buildSkeletalBP_BulkinessDNA(alien, BP_ID_listToAddOrDelete);
                this->bulkUpBodyPart(alien, BP_ID_listToAddOrDelete);

                // Now attach this BP to the BB
                attachBodyPartToAnotherBodyPart(alien, BP_ID_listToAddOrDelete, bbPtr->BP_ID, false, QVector<QVector3D>(0), QVector<QVector3D>(0), QVector<int>(0));

                // attachAllSkeletalBodyPartsToBackBone(alien);

                newBP_IDs.append(BP_ID_listToAddOrDelete);
            }

            // Finally symmetrify all the new BPs
            for (BodyPart *bpPtr : newBPsPtrsList)
            {
                if (bpPtr->BP_Type != AlienModeller::BodyPartsEnum::HEAD)
                {
                    BodyPart *mirrPtr = symmetrifyBP_AroundOppositeAttachmentFace(alien, bpPtr);
                    // make sure to store the mirror's ID in newBP_IDs
                    newBP_IDs << mirrPtr->BP_ID;
                }
            }
        }
    }
}

BodyPart *AlienModeller::incDecNumOfJointsForBP(Alien *alienPtr, BodyPart *inBP_Ptr,
                                                int numOfJntsToIncDec, bool changeJAT_forNewJoints)
{
    // TODO(ismail): BUG: This method will crash sometimes if the BP was smoothed prior to increasing its joints
    // The reason is that when the BP is smoothed, it looses its JATs information
    // Need to come up with a few solutions
    // ** Solution 1: (Easy solution) If BP has been smoothed, then switch it with its unsmoothed clone, increase/decrease its joints
    // ... and then smooth it back.

    // Check if this BP has been smoothed or not
    // If it has been smoothed, then swap it with the one in smoothingBackupCloneBPs, and remove it from the Alien
    // smooth it again as many times as it was smoothed before

    // if this is a parent BP, then detach its subBPs other than joints and bone
    // ... before removing it
    qDebug() << "commencing incDecNumOfJointsForBP method";

    // We need to clone this BP first using the modifyJAT, because this BP might have been rotated by the user
    QVector<int> BP_ID_List; BP_ID_List.clear();
    BP_ID_List << inBP_Ptr->BP_ID;
    QPair<int,int> BP_MirrPair = QPair<int,int>(-1,-1);
    modifyBP_JAT_rootJoint_DNA(alienPtr, BP_ID_List, BP_MirrPair,
                                            QPair<bool,bool>(false,false),
                                            QPair<bool,bool>(false,false),
                                            QPointF(-1,-1), QPointF(-1,-1));

    inBP_Ptr = getBodyPartFromID(alienPtr, BP_MirrPair.first);

    if (inBP_Ptr)
    {
        // bring the BP back to origin
        GeometryAlgorithms::translateBP_AccordingToInputTranslationMat(inBP_Ptr, inBP_Ptr->BP_TranslateMatrix.inverted());
        // Unrotate the BP (using inverse quaternion)
        GeometryAlgorithms::rotateBP_AccordingToInputRotationMat(inBP_Ptr, inBP_Ptr->BP_OrientationQuat.inverted());
        // update the cells box geometry
        GeometryAlgorithms::updateBP_CellPrimitiveGeometry(inBP_Ptr);

        QVector<BodyPart*> tempSubBPs; tempSubBPs.clear();
        detachAllSubBPsFromParentBPs(alienPtr, inBP_Ptr, tempSubBPs);
        QVector<int> subExtremIDs = detachAllExtremBPsFromGivenParentBP(alienPtr, inBP_Ptr); // if there are any sub extremities, these will be detached too

        quint8 smoothLevel = inBP_Ptr->smoothnessLevel;
        if (smoothLevel > 0)
        {
            // Basically here we're replacing the BP with its unsmoothed clone
            BodyPart* tempClPtr = alienPtr->smoothingBackupCloneBPs.value(inBP_Ptr->BP_ID);
            alienPtr->smoothingBackupCloneBPs.remove(inBP_Ptr->BP_ID);

            QVector3D tempPos = inBP_Ptr->getPivotPoint();
            QVector3D tempVec = inBP_Ptr->getPivotVector();

            inBP_Ptr = replaceBP_WithAnotherBP(alienPtr, inBP_Ptr, tempClPtr);

            GeometryAlgorithms::rotateBodyPartTowardsVecDir(*(inBP_Ptr), tempVec);
            GeometryAlgorithms::translateBP(inBP_Ptr,   tempPos.x() - inBP_Ptr->getPivotPoint().x(),
                                                                    tempPos.y() - inBP_Ptr->getPivotPoint().y(),
                                                                    tempPos.z() - inBP_Ptr->getPivotPoint().z());

        }

        if (numOfJntsToIncDec > 0) // increase Jnts
        {
            qDebug() << "numOfJntsToIncDec : " << numOfJntsToIncDec;
            // Update the numOfJointsPerBP_DNA by incrementing the number of joints for this BP
            updateNumOfJointsPerBP_DNA(alienPtr, inBP_Ptr, numOfJntsToIncDec);

            // Now instantiate the number of joints needed
            QPolygon tempPoly2; tempPoly2.clear();
            tempPoly2 << QPoint(inBP_Ptr->BP_ID, numOfJntsToIncDec);
            QMap<int, QPolygon> tempNumOfJointsPerBP_DNA; tempNumOfJointsPerBP_DNA.clear();
            tempNumOfJointsPerBP_DNA.insert(inBP_Ptr->BP_Type, tempPoly2);
            instantiateNumOfJointsForAllBPs_TypeGuided(alienPtr, tempNumOfJointsPerBP_DNA);

            qDebug() << "reached 1";

            // Get the existing Jnt2JntRel Poly for this BP. At this time it does not contain the newly added joint IDs
            QVector<int> tempJnt2JntLin; tempJnt2JntLin.clear();
            convertJnt2JntPolyToJointsLineageList(alienPtr->alienSkeleton.Jnt2JntRelations_TypeGuided.value(inBP_Ptr->BP_ID), tempJnt2JntLin);

            // Get a list of all the joints of this BP. This list will contain more joints added in this method
            QVector<Joint*> BP_JntsList; BP_JntsList.clear();
            getAllBP_Joints(inBP_Ptr, BP_JntsList);

            qDebug() << "reached 2";

            QVector<int> tempNewlyAddedJntsList_IDs; tempNewlyAddedJntsList_IDs.clear();
            QVector<Joint*> tempNewlyAddedJntsList_Ptrs; tempNewlyAddedJntsList_Ptrs.clear();
            for (Joint *jPtr : BP_JntsList)
            {
                if (!(tempJnt2JntLin.contains(jPtr->BP_ID)))
                {
                    tempNewlyAddedJntsList_IDs.append(jPtr->BP_ID);
                    tempNewlyAddedJntsList_Ptrs.append(jPtr);
                }
            }

            if (tempNewlyAddedJntsList_IDs.isEmpty())
            {
                qDebug() << "somethings wrong in incDecNumOfJointsForBP - bug 1";
                exit(0);
            }

            qDebug() << "reached 3";

            // Now we need to append these to the Jnt2JntRel list for this BP
            addMoreJointsToJnt2JntRelations_TypeGuided(alienPtr, inBP_Ptr, tempNewlyAddedJntsList_IDs);

            qDebug() << "reached 4";

            initializeJointsArrangementTrendForCurrentAlien(alienPtr, tempNewlyAddedJntsList_Ptrs, false);



            QMap<int,int> tempParentJntCompletionCheckList; // checklist for when the all the child joints's positions are computed. Value(int) is 1 for DONE and 0 for NOT_DONE.
            QMap<int,int> tempChildJntPositionCheckList;    // checklist for when the joint's position is computed.

            // Update the bookkeeping data structures in the JAT DNA
            for (Joint *jntPtr : BP_JntsList)
            {
                QPair<QVector3D, QVector3D> tempPair;
                tempPair.first = QVector3D(0.0,0.0,1.0); tempPair.second = QVector3D(0.0,0.0,0.0);
                if (!alienPtr->alienSkeleton.vectorLocationCoupleForEachJoint_TypeGuided.contains(jntPtr->BP_ID))
                {
                    alienPtr->alienSkeleton.vectorLocationCoupleForEachJoint_TypeGuided.insert(jntPtr->BP_ID, tempPair);

                    tempParentJntCompletionCheckList.insert(jntPtr->BP_ID,0);
                    tempChildJntPositionCheckList.insert(jntPtr->BP_ID,0);
                }
                else
                {
                    tempParentJntCompletionCheckList.insert(jntPtr->BP_ID,1);
                    tempChildJntPositionCheckList.insert(jntPtr->BP_ID,1);
                    // if this joint is the last joint of the original Jnt2Jnt Lineage,
                    // ... then we need to make its flag 0 to indicate that this joint's child
                    // ... must be computed for location. Its child will be first joint in the newly added joints
                    // It means the the last joint of the previous lineage will now act as the parent/root joint for the new lineage
                    if (jntPtr->BP_ID == tempJnt2JntLin.last())
                        tempParentJntCompletionCheckList.insert(jntPtr->BP_ID,0);
                }
            }

            QMap<int, QPolygonF> tempCopy_BP_JAT_rootJoint_DNA;
            tempCopy_BP_JAT_rootJoint_DNA.insert(inBP_Ptr->BP_Type,
                   alienPtr->alienDNA->skeletonDNA->skeletal_JATs_DNA.BP_to_JAT_rootJoint_Map_DNA.value(inBP_Ptr->BP_ID));

            compute_JAT_AND_VecLocCplForEachJnt_ForBP(alienPtr, inBP_Ptr->BP_ID, tempCopy_BP_JAT_rootJoint_DNA,
                                                      tempParentJntCompletionCheckList, tempChildJntPositionCheckList,
                                                      0.0);
            // model the joints of this BP
            modelAllAlienJoints_TypeGuided(alienPtr, tempNewlyAddedJntsList_Ptrs);

            qDebug() << "reached 5";


            QMap<int, QPolygon*> tempJnt2JntRelations; tempJnt2JntRelations.clear();
            QPolygon *tempNewPolyPtr = new QPolygon(*(alienPtr->alienSkeleton.Jnt2JntRelations_TypeGuided.value(inBP_Ptr->BP_ID)));
            QPolygon *tempNewPolyPtr2 = new QPolygon(); tempNewPolyPtr2->clear();
            bool flag1 = false;
            for (int i=0; i<tempNewPolyPtr->length(); i++)
            {
                if (tempNewPolyPtr->at(i).x() == tempJnt2JntLin.last())
                    flag1 = true;
                if (flag1)
                    tempNewPolyPtr2->append(tempNewPolyPtr->at(i));
            }

            tempJnt2JntRelations.insert(inBP_Ptr->BP_ID, tempNewPolyPtr2);
            // model the bones of this BP
            modelAllAlienBones_TypeGuided(alienPtr, tempJnt2JntRelations);

            qDebug() << "reached 6";
        }

        // decrease Jnts - no cloning or construction of new BP, happening here
        else if ( numOfJntsToIncDec < 0    &&    abs(numOfJntsToIncDec) < this->getNumOfJntsForBP(inBP_Ptr)-1 )
        {
            // Update the numOfJointsPerBP_DNA by decrementing the number of joints for this BP
            updateNumOfJointsPerBP_DNA(alienPtr, inBP_Ptr, numOfJntsToIncDec);

            // Remove all the entries in the DNAs / book-keeping DNAs related to these joints
            QVector<int> tempJnt2JntLin; tempJnt2JntLin.clear();
            convertJnt2JntPolyToJointsLineageList(alienPtr->alienSkeleton.Jnt2JntRelations_TypeGuided.value(inBP_Ptr->BP_ID), tempJnt2JntLin);
            qDebug() << tempJnt2JntLin;
            QVector<int> tempJntsIDsToRemove; tempJntsIDsToRemove.clear();
            for (int i=0; i<abs(numOfJntsToIncDec); i++)
            {
                tempJntsIDsToRemove.prepend(tempJnt2JntLin.last());
                tempJnt2JntLin.removeLast();
            }
            qDebug() << tempJntsIDsToRemove;


            // Now remove all these joints from everywhere (including DNA) and the bones associated with these
            QVector<Bone*> tempBonesList; tempBonesList.clear();
            this->getAllBP_BonesInOrder(alienPtr, inBP_Ptr, tempBonesList);
            QPolygon *tPoly = alienPtr->alienSkeleton.Jnt2JntRelations_TypeGuided.value(inBP_Ptr->BP_ID);
            for (int i=tempJntsIDsToRemove.length()-1; i>=0; i--)
            {
                // First get rid of the bones
                // Another solution is to get the bones in order and delete the last bone
                // ... one by one
                for (Bone *bonePtr : tempBonesList)
                {
                    if (tempJntsIDsToRemove[i] == bonePtr->endJoints.second)
                    {
                        alienPtr->alienBones.removeOne(bonePtr);
                        inBP_Ptr->subBodyParts.removeOne(bonePtr);
                        alienPtr->bodyParts.removeOne(bonePtr);

                        delete bonePtr;
                        break;
                    }
                }

                // Update the Jnt2JntRelations_TypeGuided by removing the entry where this joint is the
                // ... child joint
                for (int p=0; p<tPoly->length(); p++)
                {
                    if (tPoly->at(p).y() == tempJntsIDsToRemove[i])
                    {
                        // This removes the entry from Jnt2JntRelations_TypeGuided automatically
                        tPoly->remove(p);
                        break;
                    }
                }
            }

            // Print the updated Jnt2JntRelations_TypeGuided
            qDebug() << *alienPtr->alienSkeleton.Jnt2JntRelations_TypeGuided.value(inBP_Ptr->BP_ID);

            // Remove these joints' entries from the JATs
            removeAssociatedJATsAndOtherDSsForRemovableJnts(alienPtr, tempJntsIDsToRemove);

            // Now get rid of the Joints
            for (int i=0; i<tempJntsIDsToRemove.length(); i++)
            {
                BodyPart *jntPtr = getBodyPartFromID(alienPtr, tempJntsIDsToRemove[i]);
                alienPtr->alienJoints.removeOne(dynamic_cast<Joint*>(jntPtr));
                inBP_Ptr->subBodyParts.removeOne(jntPtr);
                alienPtr->bodyParts.removeOne(jntPtr);
                delete jntPtr;
            }
        }

        QVector<int> tempBP_ID_List; tempBP_ID_List.clear();
        tempBP_ID_List.append(inBP_Ptr->BP_ID);
        manipulateSkeletalBulkinessDNA_Flags(alienPtr, false, false, false, false, false, new QVector<int>(0), new QVector<int>(0), true, &tempBP_ID_List);

        buildSkeletalBP_BulkinessDNA(alienPtr, tempBP_ID_List);
        bulkUpBodyPart(alienPtr, tempBP_ID_List);

        // After the above bulkification, the bulgifiers and densifiers are deleted as well,
        // ... So we need to re-densify and re-bulgify the BP
        if (alienPtr->alienDNA->skeletonDNA->BP_GenDensificationDNA.contains(inBP_Ptr->BP_ID))
        {
            alienPtr->alienDNA->skeletonDNA->BP_GenDensificationDNA.remove(inBP_Ptr->BP_ID);
            // alienPtr->alienDNA->skeletonDNA->BP_SpecificDensificationDNA.remove(inBP_Ptr->BP_ID);
            build_BP_DensificationDNA(alienPtr, tempBP_ID_List);
            densifyAllBodyParts(alienPtr, tempBP_ID_List);
        }

        // Also delete its mirror as the BP will be resymmetrified
        BodyPart *mirrPtr = getMirrorOrOriginalBP(alienPtr, inBP_Ptr);
        if (mirrPtr)
        {
            if (!alienPtr->alienDNA->skeletonDNA->BP_SizeComparedToBackBone_DNA[mirrPtr->BP_Type].isEmpty())
                alienPtr->alienDNA->skeletonDNA->BP_SizeComparedToBackBone_DNA[mirrPtr->BP_Type].removeLast();
            if (!alienPtr->alienDNA->skeletonDNA->BP_AngleArrangementsFromBackBone_DNA[mirrPtr->BP_Type].isEmpty())
                alienPtr->alienDNA->skeletonDNA->BP_AngleArrangementsFromBackBone_DNA[mirrPtr->BP_Type].removeLast();

            // TODO: This should be called inside removeAssociatedJATsAndOtherDSsForRemovableJnts
            alienPtr->alienDNA->skeletonDNA->skeletal_JATs_DNA.BP_to_JAT_rootJoint_Map_DNA.remove(mirrPtr->BP_ID);

            QVector<int> tempBP_ID_List_1; tempBP_ID_List_1.clear();
            tempBP_ID_List_1 << mirrPtr->BP_ID;
            removeAlienBodyParts(alienPtr, tempBP_ID_List_1);
        }

        // Do the smoothing
        smoothenSkeletalBP(alienPtr, inBP_Ptr, smoothLevel);

        // At the end make sure that if this is a parent BP then all its children (subBPs) are re-attached to it
        // If the BP has any sub BPs that were detached, these need to be cloned and the clones need to be attached
        // ... to the BP
        // TODO: This code inside the for loop is repeated in other methods as well. Perhaps make a method for it
        QVector<int> tempSubMirrBPsIDs; tempSubMirrBPsIDs.clear();
        if (inBP_Ptr->BP_Type != AlienModeller::BodyPartsEnum::BACK_BONE)
        {
            QMultiMap<int,int> tempMultMap; tempMultMap.clear();
            QVector<BodyPart*> tempSubExtremBPsForAttaching; tempSubExtremBPsForAttaching.clear();
            for (BodyPart *bp : tempSubBPs)
            {
                if (!bp->isSymmetric)
                {
                    if (bp->BP_Type == AlienModeller::BodyPartsEnum::ExtremBP_Carpal
                            ||
                        bp->BP_Type == AlienModeller::BodyPartsEnum::ExtremBP_Digit)
                    {
                        tempSubExtremBPsForAttaching.append(bp);
                    }
                    else
                        tempMultMap.insert(bp->BP_Type, bp->BP_ID);
                }
                else // this is a mirror which will be deleted for now
                     // and later resymmetrified
                     // TODO: Inspect this part.
                {
                    // keep a note of those BPs that have mirrors so they can be re-symmetrified later
                    tempSubMirrBPsIDs << bp->BP_ID;
                }
            }
            qDebug() << "reached 10";
            QVector<int> sameTypeSourceBP_IDs; sameTypeSourceBP_IDs.clear();
            while (!tempMultMap.isEmpty())
            {
                int key = tempMultMap.firstKey();
                sameTypeSourceBP_IDs.append(tempMultMap.values(key));
                tempMultMap.remove(key);
                sameTypeSourceBP_IDs.removeOne(inBP_Ptr->BP_ID);
                if (!sameTypeSourceBP_IDs.contains(inBP_Ptr->BP_ID)) // Sub BPs list must not include parent BP's ID (naturally)
                {
                    QVector<QVector3D> attachmentPnts, attachmentVectors;   attachmentPnts.clear(); attachmentVectors.clear();
                    QVector<int> boneLocPairList; boneLocPairList.clear();
                    // This does 'group attachments'
                    attachBodyPartToAnotherBodyPart(alienPtr, sameTypeSourceBP_IDs, inBP_Ptr->BP_ID, true,
                                                    attachmentPnts, attachmentVectors, boneLocPairList, false);
                    // now re-symmetrify these sameTypeSourceBP_IDs if previously noted
                    sameTypeSourceBP_IDs.clear();
                }
                else
                {
                    qDebug() << "Found the parent BP's ID in the list of sub BPs, which is a problem";
                    exit(0);
                }
            }
            if (!tempSubMirrBPsIDs.isEmpty())
                removeAlienBodyParts(alienPtr, tempSubMirrBPsIDs);

            if (inBP_Ptr->BP_Type == AlienModeller::BodyPartsEnum::ExtremBP_Carpal
                    ||
                inBP_Ptr->BP_Type == AlienModeller::BodyPartsEnum::ExtremBP_Digit)
            {
                QVector<BodyPart*> carpals; carpals.clear();
                carpals.append(inBP_Ptr);
                attachDigitsToCarpalsForNewExtremBP(alienPtr, tempSubExtremBPsForAttaching, carpals);
            }
            qDebug() << "reached 11";
            // finally re-attach the detached extremities to this re-JATed BP
            attachExtremBP_ToParentBP(alienPtr, inBP_Ptr, subExtremIDs);
        }
        else
        {
            QVector<BodyPart*> tempClonedBPs; tempClonedBPs.clear();
            // for (int s=0; s<tempSubBPs.length(); s++)
            // {
            //     // clone each BP
            //     qDebug() << "reached IncDecJnts - 1";
            //     if (!tempSubBPs[s]->isSymmetric)
            //     {
            //         // TODO: Why are we cloning it? Check it we can do it without having to clone the detached subBP
            //         incDecButtonPressed=false;
            //         tempClonedBPs.append(cloneSkeletalBP_FromAnotherAlien_viaDNA(alienPtr, alienPtr, tempSubBPs[s]));
            //     }
            //     tempSubMirrBPsIDs << tempSubBPs[s]->BP_ID;
            //     if (s == tempSubBPs.length()-1) // if this is the last loop iteration, then remove all the previous BPs,
            //                                     // ... assign their IDs to the cloned BPs, and then attach the clones back to
            //                                     // ... the parent BP
            //     {
            //         // replace the detached sub BPs with the new clones
            //         removeAlienBodyParts(alienPtr, tempSubMirrBPsIDs);

            //         for (BodyPart *bp : tempClonedBPs)
            //         {
            //             alienPtr->alienSkeleton.Jnt2JntRelations_TypeGuided.insert(bp->cloneFromAlienAndBP_Pair.second,
            //                                     alienPtr->alienSkeleton.Jnt2JntRelations_TypeGuided.value(bp->BP_ID));
            //             alienPtr->alienSkeleton.Jnt2JntRelations_TypeGuided.remove(bp->BP_ID);

            //             if (alienPtr->alienDNA->skeletonDNA->BP_SpecificDensificationDNA.contains(bp->BP_ID))
            //             {
            //                 QMap<int, QPolygonF> sourceMap = alienPtr->alienDNA->skeletonDNA->BP_SpecificDensificationDNA.value(bp->BP_ID);
            //                 alienPtr->alienDNA->skeletonDNA->BP_SpecificDensificationDNA.insert(bp->cloneFromAlienAndBP_Pair.second, sourceMap);
            //                 alienPtr->alienDNA->skeletonDNA->BP_SpecificDensificationDNA.remove(bp->BP_ID);
            //             }

            //             if (alienPtr->alienDNA->skeletonDNA->BP_BulgificationDNA.contains(bp->BP_ID))
            //             {
            //                 QMap<int, QPolygon> sourceBulgMap = alienPtr->alienDNA->skeletonDNA->BP_BulgificationDNA.value(bp->BP_ID);
            //                 alienPtr->alienDNA->skeletonDNA->BP_BulgificationDNA.insert(bp->cloneFromAlienAndBP_Pair.second, sourceBulgMap);
            //                 alienPtr->alienDNA->skeletonDNA->BP_BulgificationDNA.remove(bp->BP_ID);
            //             }
            //             bp->BP_ID = bp->cloneFromAlienAndBP_Pair.second;
            //             bp->cloneFromAlienAndBP_Pair = QPair<int,int>(-1,-1);

            //             for (int exID : bp->subExtremityBPsIDs)
            //             {
            //                 alienPtr->extremToParentBP_Map.insert(exID,bp->BP_ID);
            //             }

            //         }
            //     }
            // }
            QVector<int> cloneIDs; cloneIDs.clear();
            for (int s=0; s<tempSubBPs.length(); s++)
            {
                // clone each BP
                qDebug() << "reached IncDecJnts - 1";
                if (!tempSubBPs[s]->isSymmetric)
                {
                    // TODO: Why are we cloning it? Check it we can do it without having to clone the detached subBP
                    // incDecButtonPressed=false;
                    QVector3D tempPos = tempSubBPs[s]->getPivotPoint();
                    QVector3D tempVec = tempSubBPs[s]->getPivotVector();

                    tempClonedBPs.append(cloneSkeletalBP_FromAnotherAlien_viaDNA(alienPtr, alienPtr, tempSubBPs[s]));
                    cloneIDs << tempClonedBPs.last()->BP_ID;

                    GeometryAlgorithms::rotateBodyPartTowardsVecDir(*(tempClonedBPs.last()), tempVec);
                    GeometryAlgorithms::translateBP(tempClonedBPs.last(),   tempPos.x() - tempClonedBPs.last()->getPivotPoint().x(),
                                                                tempPos.y() - tempClonedBPs.last()->getPivotPoint().y(),
                                                                tempPos.z() - tempClonedBPs.last()->getPivotPoint().z());

                }
                else
                    tempSubMirrBPsIDs << tempSubBPs[s]->BP_ID;
            }
            removeAlienBodyParts(alienPtr, tempSubMirrBPsIDs);
            for (int clID : cloneIDs)
            {
                BodyPart *clonePtr = getBodyPartFromID(alienPtr, clID);
                if (clonePtr)
                {
                    BodyPart *origPtr = getBodyPartFromID(alienPtr, clonePtr->cloneFromAlienAndBP_Pair.second);
                    if (clonePtr && origPtr)
                    {
                        // Now replace it
                        clonePtr = replaceBP_WithAnotherBP(alienPtr, origPtr, clonePtr);
                    }
                }
            }

        }

        // TODO(ismail): Fix this as this doesn't work properly.
        // ... The mirrored BPs don't appear. UPDATE: It seems to be working fine lately
        if (inBP_Ptr->BP_Type == AlienModeller::BodyPartsEnum::BACK_BONE)
        {
            // attachAllSkeletalBodyPartsToBackBone(alienPtr);
            symmetrifyAlienBodyParts(alienPtr);
        }

        // take it back to its actual location
        GeometryAlgorithms::rotateBP_AccordingToInputRotationMat(inBP_Ptr, inBP_Ptr->BP_OrientationQuat);
        // take it back to its actual location
        GeometryAlgorithms::translateBP_AccordingToInputTranslationMat(inBP_Ptr, inBP_Ptr->BP_TranslateMatrix);
        // update the cells box geometry
        GeometryAlgorithms::updateBP_CellPrimitiveGeometry(inBP_Ptr);

        if (mirrPtr)
            symmetrifyBP_AroundOppositeAttachmentFace(alienPtr, inBP_Ptr);
    }

    qDebug() << "incDecNumOfJointsForBP method ended";
    return inBP_Ptr;
}

void AlienModeller::removeBonesFromBP(Alien *aPtr, BodyPart *inBP_Ptr, QVector<int> bonesToRemove_IDs)
{
    QVector<Bone*> tempBonesList; tempBonesList.clear();
    getAllBP_BonesInOrder(aPtr, inBP_Ptr, tempBonesList);

    // Get a list of joints and bones that need removing.
    // After the for loop below, tempJntsToRem_IDs will contain exactly those joints that need removing
    QVector<int> tempJntsToRem_IDs; tempJntsToRem_IDs.clear();
    convertJnt2JntPolyToJointsLineageList(aPtr->alienSkeleton.Jnt2JntRelations_TypeGuided.value(inBP_Ptr->BP_ID), tempJntsToRem_IDs);
    QVector<Bone*> bonesToRemove_Ptrs; bonesToRemove_Ptrs.clear();
    QPolygon *tPoly = aPtr->alienSkeleton.Jnt2JntRelations_TypeGuided.value(inBP_Ptr->BP_ID);
    for (int i=0; i<tempBonesList.length(); i++)
    {
        if (bonesToRemove_IDs.contains(tempBonesList[i]->BP_ID))
        {
            bonesToRemove_Ptrs << tempBonesList[i];
            tPoly->remove(i); // this updates the Jnt2JntRelations_TypeGuided for this BP
        }
        else    // Save its end joints from removal/deletion
        {
            tempJntsToRem_IDs.removeOne(tempBonesList[i]->endJoints.first);
            tempJntsToRem_IDs.removeOne(tempBonesList[i]->endJoints.second);
        }
    }
    // update the root Jnt for this BP
    inBP_Ptr->rootJntID = tPoly->first().x();

    // TODO(ismail): Remove any other BPs that were attached to any of the removable bones or joints
    // Get a list of BPs that are attached to the removable bones (in bonesToRemove_Ptrs)
    QVector<int> attachedBPsToDelete_IDs; attachedBPsToDelete_IDs.clear();
    attachedBPsToDelete_IDs.append(tempJntsToRem_IDs);
    attachedBPsToDelete_IDs.append(bonesToRemove_IDs);
    for (Bone *bnPtr : bonesToRemove_Ptrs)
    {
        attachedBPsToDelete_IDs.append(bnPtr->allAttachedBP_IDs);
    }
    // Get a list of BPs that are attached to the removable joints (in tempJntsToRem_IDs)
    for (int j : tempJntsToRem_IDs)
    {
        Joint *jPtr = dynamic_cast<Joint*>(getBodyPartFromID(aPtr, j));
        attachedBPsToDelete_IDs.append(jPtr->allAttachedBP_IDs);
    }

    // Remove these joints' entries (in tempJntsToRem_IDs) from the JATs and other DSs
    removeAssociatedJATsAndOtherDSsForRemovableJnts(aPtr, tempJntsToRem_IDs);

    // Remove all the BPs in attachedBPsToDelete
    removeAlienBodyParts(aPtr, attachedBPsToDelete_IDs);
}

BodyPart *AlienModeller::pruneBP(Alien *aPtr, BodyPart *bpPtr, QVector<int> selectedBones_IDs)
{
    /*
     * AlGO:
     *
     * - Get a list of bones that are not in the selectedBones_IDs list. Call it removableBonesList
     * - Get a list of BPs that are attached to bones in the removableBonesList and remove/delete thse
     * - Clone this BP and all its subBPs (other than Jnts and Bones) except those that
     * ... are attached to the removable bones
     * - Attach the subBPs that need to be cloned to this BP's clone
     * - Remove the bones from this BP's clone that need to be removed
     * - Finally attach this BPs clone to the target Alien's BPs and the specified bone/joint
     *
     *
    */

    BodyPart *clonedBP = nullptr;
    // Get a list of bones that are not in the selectedBones_IDs list. Call it removableBonesList
    QVector<Bone*> bonesPtrList; bonesPtrList.clear();
    if (getAllBP_BonesInOrder(aPtr, bpPtr, bonesPtrList))
    {
        QVector<int> removableChildBPsIDs; removableChildBPsIDs.clear();
        QPolygon jntToChildBP_Poly;jntToChildBP_Poly.clear();
        QVector<int> jntLocPairsList; jntLocPairsList.clear();
        QVector<int> removableBoneIDs; removableBoneIDs.clear();
        QVector<int> removableBoneIndexes = QVector<int>(bonesPtrList.length(),0);
        QVector<int> removableJntsIDs; removableJntsIDs.clear();
        int tempRootJntID = -1;
        for (int bi=0; bi<bonesPtrList.length(); bi++)
        {
            if (!selectedBones_IDs.contains(bonesPtrList[bi]->BP_ID)) // This bone is removable, so
                                                                      // store this bone as this will
                                                                      // be deleted and any subBPs
                                                                      // attached to it will not be cloned
            {
//                removableBoneIDs << bonesPtrList[bi]->BP_ID;
                removableBoneIndexes[bi] = 1;
                // for this bone, we need to get a list of child BPs that are attached to this bone
                // A child BP can be attached directly to this bone or to one of its joints
                // If this bone needs to be deleted, and its next bone also needs to be deleted,
                // ... then we need to remove the child BPs from their common joint
                if (bi == 0) // If this is the first bone, its first joint will be removed along with any child BPs
                {
                    Joint *jPtr = dynamic_cast<Joint*>(getBodyPartFromID(aPtr, bonesPtrList[bi]->endJoints.first));
                    removableJntsIDs << jPtr->BP_ID;
                    removableChildBPsIDs.append(jPtr->allAttachedBP_IDs);
                }
                if ((bi+1) != bonesPtrList.length()) // this is not the last bone so we can safely check for the next bone
                                                     // avoiding vector out of bounds
                {
                    if (!selectedBones_IDs.contains(bonesPtrList[bi+1]->BP_ID)) // next bone is also removable
                                                                                // so find the common joint
                    {
                        // get the common joint and keep a list of child BPs attached to this joint
                        if (bonesPtrList[bi]->endJoints.second == bonesPtrList[bi+1]->endJoints.first)
                        {
                            Joint *jPtr = dynamic_cast<Joint*>(getBodyPartFromID(aPtr, bonesPtrList[bi]->endJoints.second));
                            removableJntsIDs << jPtr->BP_ID;
                            removableChildBPsIDs.append(jPtr->allAttachedBP_IDs);
                        }
                        else
                        {
                            qDebug() << "common joint for adjacent bones not found. Problem with pruneBP() method";
                            exit(0);
                        }
                    }
                }
                else // this is the last bone so its second joint (last joint in the lineage) will
                     // ... also be removed
                {
                    Joint *jPtr = dynamic_cast<Joint*>(getBodyPartFromID(aPtr, bonesPtrList[bi]->endJoints.second));
                    removableJntsIDs << jPtr->BP_ID;
                    removableChildBPsIDs.append(jPtr->allAttachedBP_IDs);
                }
            }
            else if (tempRootJntID == -1)
            {
                tempRootJntID = bonesPtrList[bi]->endJoints.first;
                Joint *jPtr = dynamic_cast<Joint*>(getBodyPartFromID(aPtr, bonesPtrList[bi]->endJoints.first));
                for (int childID : jPtr->allAttachedBP_IDs)
                    jntToChildBP_Poly << QPoint(jPtr->BP_ID, childID);
                if ((bi+1) == bonesPtrList.length())
                {
                    Joint *jPtr = dynamic_cast<Joint*>(getBodyPartFromID(aPtr, bonesPtrList[bi]->endJoints.second));
                    for (int childID : jPtr->allAttachedBP_IDs)
                        jntToChildBP_Poly << QPoint(jPtr->BP_ID, childID);
                }
            }
            else
            {
                Joint *jPtr = dynamic_cast<Joint*>(getBodyPartFromID(aPtr, bonesPtrList[bi]->endJoints.first));
                for (int childID : jPtr->allAttachedBP_IDs)
                    jntToChildBP_Poly << QPoint(jPtr->BP_ID, childID);
                if ((bi+1) == bonesPtrList.length())
                {
                    Joint *jPtr = dynamic_cast<Joint*>(getBodyPartFromID(aPtr, bonesPtrList[bi]->endJoints.second));
                    for (int childID : jPtr->allAttachedBP_IDs)
                        jntToChildBP_Poly << QPoint(jPtr->BP_ID, childID);
                }
            }
        }

        // Now clone this BP
        clonedBP = cloneSkeletalBP_FromAnotherAlien_viaDNA(aPtr, aPtr, bpPtr);
        // Note down the bones to be removed
        bonesPtrList.clear();
        if (getAllBP_BonesInOrder(aPtr, clonedBP, bonesPtrList))
        {
            for (int i=0; i<bonesPtrList.length(); i++)
            {
                if (removableBoneIndexes[i] == 1)
                {
                    removableBoneIDs << bonesPtrList[i]->BP_ID;
                }
            }
        }

        qDebug() << "Removable Bones IDs" << removableBoneIDs;
        qDebug() << "Removable Joints IDs" << removableJntsIDs;

        // Note down the joints to be removed
        // TODO(ismail): To check whether the BP is being pruned properly, check it by generating
        // ... in a seperate panel
        QPolygon *tempPoly = aPtr->alienSkeleton.Jnt2JntRelations_TypeGuided.value(clonedBP->BP_ID);
        QVector<int> Jnt2JntLin; Jnt2JntLin.clear();
        convertJnt2JntPolyToJointsLineageList(tempPoly, Jnt2JntLin);
        // After this loop, removableJntsIDs contains all the joints from the clonedBP that need to be removed
        int tempLen = removableJntsIDs.length();
        for (int i=0; i<tempLen; i++)
        {
            if (aPtr->alienSkeleton.JntToJntMappings_ForBookKeeping.contains(removableJntsIDs[0]))
            {
                removableJntsIDs.append(aPtr->alienSkeleton.JntToJntMappings_ForBookKeeping.value(removableJntsIDs[0]));
                removableJntsIDs.removeFirst();
            }
            else // there seems to be a problem if this get to this point. Please check
            {
                qDebug() << "there seems to be a problem if this get to this point. Please check";
                exit(0);
                break;
            }
        }

        qDebug() << "Removable Joints IDs" << removableJntsIDs;

        // Set the root joint of the clonedBP
        if (!removableJntsIDs.contains(aPtr->alienSkeleton.JntToJntMappings_ForBookKeeping.value(tempRootJntID)))
            clonedBP->rootJntID = aPtr->alienSkeleton.JntToJntMappings_ForBookKeeping.value(tempRootJntID);
        else
        {
            qDebug() << "The root joint is not allocated properly. Problem in PruneBP()";
            exit(0);
        }

        qDebug() << "Root Jnt of Cloned BP" << clonedBP->rootJntID;

        // Now Remove these joints' entries (in tempJntsToRem_IDs) from the JATs and other DSs
        removeAssociatedJATsAndOtherDSsForRemovableJnts(aPtr, removableJntsIDs);

        // include the bones to be removed to removableJntsIDs list as well, so these will
        // ... all be removed together
        removableJntsIDs.append(removableBoneIDs);

        qDebug() << "Removable Joints + bones IDs" << removableJntsIDs;

        // Now Remove all the BPs in attachedBPsToDelete
        removeAlienBodyParts(aPtr, removableJntsIDs);

        qDebug() << "removal done";

        // Now clone only those BPs that are not part of the removableChildBPsPtrs list but are subBPs of the incoming BP
        QVector<BodyPart*> clonedChildBPsPtr; clonedChildBPsPtr.clear();
        QVector<int> clonedChildBPsIDs; clonedChildBPsIDs.clear();
        QVector<QVector3D> attachPnts; attachPnts.clear();
        QVector<QVector3D> attachVectors; attachVectors.clear();
//        for (BodyPart *childPtr : bpPtr->subBodyParts)
//        {
//            if (
//                childPtr->BP_Type != AlienModeller::BodyPartsEnum::JOINT  &&
//                childPtr->BP_Type != AlienModeller::BodyPartsEnum::BONE  &&
//                !removableChildBPsIDs.contains(childPtr->BP_ID)
//               )
//            {
//                clonedChildBPsPtr.append(cloneSkeletalBP_FromAnotherAlien_viaDNA(aPtr, aPtr, childPtr));
//                clonedChildBPsIDs << clonedChildBPsPtr.last()->BP_ID;
//                attachPnts.append(childPtr->getPivotPoint());
//                attachVectors.append(childPtr->getPivotVector());
//            }
//        }
        for (QPoint jntChildPair : jntToChildBP_Poly)
        {
            if (aPtr->alienSkeleton.JntToJntMappings_ForBookKeeping.contains(jntChildPair.x()))
                jntLocPairsList << aPtr->alienSkeleton.JntToJntMappings_ForBookKeeping.value(jntChildPair.x()); // store the joint's ID
            else
            {
                qDebug() << "there is no corresponding joint in JntToJntMappings_ForBookKeeping in PruneBP() method";
                exit(0);
            }
            BodyPart *childPtr = getBodyPartFromID(aPtr, jntChildPair.y());
            if (childPtr)
            {
                clonedChildBPsPtr.append(cloneSkeletalBP_FromAnotherAlien_viaDNA(aPtr, aPtr, childPtr));
                clonedChildBPsIDs << clonedChildBPsPtr.last()->BP_ID;
                attachPnts.append(childPtr->getPivotPoint());
                attachVectors.append(childPtr->getPivotVector());
            }
        }

        qDebug() << "done 2 in prune()";


        // Finally attach these to the clonedBP in exactly the same location and orientation as the original BPs were
        // ... attached to the incoming BP (i.e. bpPtr)
        // computeAttachementParametersForChildBPs(Alien *, BP_List, attachPntsParam, attachVecParam)
        // computeAttachementPntsAndVectorsForChildBPs(Alien *, BP_List, attachPntsParam, attachVecParam, attachPoints, attachVectors)
        if (!clonedChildBPsIDs.isEmpty())
            attachBodyPartToAnotherBodyPart(aPtr, clonedChildBPsIDs, clonedBP->BP_ID, false, attachPnts, attachVectors, jntLocPairsList);

        qDebug() << "done 3 in prune()";
    }
    else
    {
        qDebug() << "No bones present so exiting the system. Problem in PruneBP() method";
        exit(0);
    }

    return clonedBP;

}

// for p1: (change energy trend (T/F), change starting energy (T/F)). For p2: (change angle trend (T/F), change starting angle (T/F))
void AlienModeller::modifyBP_JAT_rootJoint_DNA(Alien *alienPtr, QVector<int> &BP_ID_List,  QPair<int, int> &BP_MirrorPair,
                                               QPair<bool, bool> p1, QPair<bool, bool> p2,
                                               QPointF energyVals, QPointF angleVals)
{
    /*
     * ALGORITHM 1:
     * This algorithm modifies the DNA first, then removes all the BPs from the Alien, and finally calls the
     * buildAlienSkeleton method, just like the strategy we used for modifying Bulkiness DNA
     *
     *  change the JATs for root Jnts for the incoming BPs
     *
     *  remove all the BPs from the ALien
     *
     *  rebuild the skeleton
     *  attach BP
     *
    */

    /*
     * ALGORITHM 2: I think I'm using this one
     *
     * If the input BP is a symmetric BP, find the BP whose mirror it was. Call it mirrorParentBP
     *
     * create a clone of the mirrorParentBP with the modified JATs. Call it clonedMirrorParentBP
     *
     * Generate the clonedMirrorParentBP at the same location and orientation as the mirrorParentBP
     *
     * Delete the input BP, but keep a note of its BP_ID
     *
     * if (input BP is a symmetric BP)
     *      Symmetrify the clonedMirrorParentBP
     *
     * assign clonedMirrorParentBP it the same BP_ID as mirrorParentBP or inputBP
     *
     *
    */
    qDebug() << "commencing Re-JAT-ing of BP";
    for (int i : BP_ID_List)
    {
        QPointF energyPt = QPointF(-1,-1), anglePt = QPointF(-1,-1);
        BodyPart *inBP_Ptr = this->getBodyPartFromID(alienPtr, i);

        // // We need to remove this BP from its parent's subBPs list and then at the end re-assign this BP as a suBP of its parent
        // BodyPart *parentBP_Ptr = getParentOfThisBP(alienPtr, inBP_Ptr);
        // if (parentBP_Ptr)
        //     parentBP_Ptr->subBodyParts.removeOne(inBP_Ptr);

        if (inBP_Ptr->isSymmetric)
        {
            qDebug() << "this BP is a mirror";
//            inBP_Ptr = this->getBodyPartFromID(alien, inBP_Ptr->mirrorOf);
            return;
        }
        else
        {
            qDebug() << "this BP is not a mirror";
        }

        // Check if this BP has been smoothed or not
        // If it has been smoothed, then swap it with its original unsmoothed version in smoothingBackupCloneBPs, and remove it from the Alien
        // smooth it again as many times as it was smoothed before
        quint8 smoothLevel = inBP_Ptr->smoothnessLevel;
        if (smoothLevel > 0)
        {
            // Basically here we're replacing the BP with its unsmoothed clone

            BodyPart* tempClPtr = alienPtr->smoothingBackupCloneBPs.value(inBP_Ptr->BP_ID);
            alienPtr->smoothingBackupCloneBPs.remove(inBP_Ptr->BP_ID);

            QVector3D tempPos = inBP_Ptr->getPivotPoint();
            QVector3D tempVec = inBP_Ptr->getPivotVector();

            inBP_Ptr = replaceBP_WithAnotherBP(alienPtr, inBP_Ptr, tempClPtr);

            GeometryAlgorithms::rotateBodyPartTowardsVecDir(*(inBP_Ptr), tempVec);
            GeometryAlgorithms::translateBP(inBP_Ptr,   tempPos.x() - inBP_Ptr->getPivotPoint().x(),
                                                                    tempPos.y() - inBP_Ptr->getPivotPoint().y(),
                                                                    tempPos.z() - inBP_Ptr->getPivotPoint().z());
        }

        BodyPart *origBP_Ptr = inBP_Ptr;

        // if this is a parent BP, then detach its subBPs (other than joints and bone) before re-JAT-ing it
        // The reason we are doing this is to attach the sub BPs back to the re-JATed parent
        QVector<BodyPart*> tempSubBPs; tempSubBPs.clear();
        detachAllSubBPsFromParentBPs(alienPtr, origBP_Ptr, tempSubBPs);
        QVector<int> subExtremIDs = detachAllExtremBPsFromGivenParentBP(alienPtr, inBP_Ptr); // if there are any sub extremities, these will be detached too

        // If the user HAS selected RANDOM modification of JATS.
        // i.e. energy and angle trends and starting values for the JATs (root Jnts)
        if (energyVals == QPointF(-1,-1) && angleVals == QPointF(-1,-1))
        {
            if (skeletonRuleSet->jointsArrangementTrends_RS.contains(inBP_Ptr->BP_Type))
            {
                QPointF tPt1, tPt2;
                QPolygon tempPoly = skeletonRuleSet->jointsArrangementTrends_RS.value(inBP_Ptr->BP_Type);
                // expand the likelihoods of the 'enerygy trends' and 'angle trends'
                QVector<int> tempLikelihoodExpansionList_1, tempLikelihoodExpansionList_2;
                tempLikelihoodExpansionList_1.resize(0); tempLikelihoodExpansionList_2.resize(0);
                for (int j=0; j<6; j++)
                {
                    for (int k=0; k<tempPoly[j].y(); k++) // expand tempLikelihoodExpansionList_1 for energy trends
                    {
                        tempLikelihoodExpansionList_1.append(tempPoly[j].x());
                    }
                    for (int k=0; k<tempPoly[j+6].y(); k++) // expand tempLikelihoodExpansionList_2 for angle trends
                    {
                        tempLikelihoodExpansionList_2.append(tempPoly[j+6].x());
                    }
                }
                // Select a random energy trend from the expanded energy trends
                // select a starting random energy.
                tPt1 = QPointF(
                                tempLikelihoodExpansionList_1.at(GeometryAlgorithms::getRandomInteger(0, tempLikelihoodExpansionList_1.length()-1)),
                                GeometryAlgorithms::getRandomFloat(tempPoly[12].x(), tempPoly[12].y())
                                );
                // Select a random angle trend from the expanded angle trends
                // select a starting random angle
                tPt2 = QPointF(
                                tempLikelihoodExpansionList_2.at(GeometryAlgorithms::getRandomInteger(0, tempLikelihoodExpansionList_2.length()-1)),
                                GeometryAlgorithms::getRandomFloat(tempPoly[13].x(), tempPoly[13].y())
                                );

                if (p1.first) // energy trend change required
                    energyPt.setX(tPt1.x());
                if (p1.second) // energy of root joint change required
                    energyPt.setY(tPt1.y());
                    // energyPt.setY(tPt1.y()+GeometryAlgorithms::getRandomInteger(30,50));
                if (p2.first) // angle trend change required
                    anglePt.setX(tPt2.x());
                if (p2.second) // angle of root joint change required
                    anglePt.setY(tPt2.y());
            }
        }
        else   // If the user HAS specified the energy and angle trends and starring values for the JATs (root Jnts)
        {
            energyPt = energyVals;
            anglePt = angleVals;
        }

        // Now create a clone with the new JAT values as specified above
        // First backup the attachment position and vectors of this BP
        QVector3D tempPos = inBP_Ptr->getPivotPoint();
        QVector3D tempVec = inBP_Ptr->getPivotVector();

        inBP_Ptr = cloneSkeletalBP_FromAnotherAlien_viaDNA(alienPtr, alienPtr, inBP_Ptr, 1, false, energyPt, anglePt);

        if (inBP_Ptr != nullptr)
        {
            GeometryAlgorithms::rotateBodyPartTowardsVecDir(*(inBP_Ptr), tempVec);
            GeometryAlgorithms::translateBP(inBP_Ptr,   tempPos.x() - inBP_Ptr->getPivotPoint().x(),
                                                        tempPos.y() - inBP_Ptr->getPivotPoint().y(),
                                                        tempPos.z() - inBP_Ptr->getPivotPoint().z());

            BodyPart *mirrPtr = getMirrorOrOriginalBP(alienPtr, origBP_Ptr); // before removing originBP, note down its mirror (if any)


            inBP_Ptr = replaceBP_WithAnotherBP(alienPtr, origBP_Ptr, inBP_Ptr);
            attachExtremBP_ToParentBP(alienPtr, inBP_Ptr, subExtremIDs); // Make sure to re-attach its sub extremities to this re-JAT-ed BP

            // Also symmertify this BP around its parent BP
            if (mirrPtr)
            {
                if (!alienPtr->alienDNA->skeletonDNA->BP_SizeComparedToBackBone_DNA[mirrPtr->BP_Type].isEmpty())
                    alienPtr->alienDNA->skeletonDNA->BP_SizeComparedToBackBone_DNA[mirrPtr->BP_Type].removeLast();
                if (!alienPtr->alienDNA->skeletonDNA->BP_AngleArrangementsFromBackBone_DNA[mirrPtr->BP_Type].isEmpty())
                    alienPtr->alienDNA->skeletonDNA->BP_AngleArrangementsFromBackBone_DNA[mirrPtr->BP_Type].removeLast();

                // removeAssociatedJATsAndOtherDSsForRemovableJnts(alienPtr, getAllJntIDsOfBP(mirrPtr));
                // TODO: This should be called inside removeAssociatedJATsAndOtherDSsForRemovableJnts
                alienPtr->alienDNA->skeletonDNA->skeletal_JATs_DNA.BP_to_JAT_rootJoint_Map_DNA.remove(mirrPtr->BP_ID);

                QVector<int> tempBP_ID_List_1; tempBP_ID_List_1.clear();
                tempBP_ID_List_1 << mirrPtr->BP_ID;
                removeAlienBodyParts(alienPtr, tempBP_ID_List_1);

                mirrPtr = symmetrifyBP_AroundOppositeAttachmentFace(alienPtr, inBP_Ptr);

                // Do the smoothing
                mirrPtr->smoothnessLevel=0;
                smoothenSkeletalBP(alienPtr, mirrPtr, smoothLevel);

                BP_MirrorPair.second = mirrPtr->BP_ID;
            }

            // Do the smoothing
            inBP_Ptr->smoothnessLevel=0;
            smoothenSkeletalBP(alienPtr, inBP_Ptr, smoothLevel);

            BP_MirrorPair.first = inBP_Ptr->BP_ID;
        }

        // If the BP has any sub BPs that were detached, these need to be cloned and the clones need to be attached
        // ... to the BP
        // TODO: This code inside the for loop is repeated in other methods as well. Perhaps make a method for it

        QVector<int> tempSubMirrBPsIDs; tempSubMirrBPsIDs.clear();
        if (inBP_Ptr->BP_Type != AlienModeller::BodyPartsEnum::BACK_BONE)
        {
            QMultiMap<int,int> tempMultMap; tempMultMap.clear();
            QVector<BodyPart*> tempSubExtremBPsForAttaching; tempSubExtremBPsForAttaching.clear();
            for (BodyPart *bp : tempSubBPs)
            {
                if (!bp->isSymmetric)
                {
                    if (bp->BP_Type == AlienModeller::BodyPartsEnum::ExtremBP_Carpal
                            ||
                        bp->BP_Type == AlienModeller::BodyPartsEnum::ExtremBP_Digit)
                    {
                        tempSubExtremBPsForAttaching.append(bp);
                    }
                    else
                        tempMultMap.insert(bp->BP_Type, bp->BP_ID);
                }
                else // this is a mirror which will be deleted for now
                     // and later resymmetrified
                     // TODO: Inspect this part.
                {
                    // keep a note of those BPs that have mirrors so they can be re-symmetrified later
                    tempSubMirrBPsIDs << bp->BP_ID;
                }
            }
            qDebug() << "reached 10";
            QVector<int> sameTypeSourceBP_IDs; sameTypeSourceBP_IDs.clear();
            while (!tempMultMap.isEmpty())
            {
                int key = tempMultMap.firstKey();
                sameTypeSourceBP_IDs.append(tempMultMap.values(key));
                tempMultMap.remove(key);
                sameTypeSourceBP_IDs.removeOne(inBP_Ptr->BP_ID);
                if (!sameTypeSourceBP_IDs.contains(inBP_Ptr->BP_ID)) // Sub BPs list must not include parent BP's ID (naturally)
                {
                    QVector<QVector3D> attachmentPnts, attachmentVectors;   attachmentPnts.clear(); attachmentVectors.clear();
                    QVector<int> boneLocPairList; boneLocPairList.clear();
                    // This does 'group attachments'
                    attachBodyPartToAnotherBodyPart(alienPtr, sameTypeSourceBP_IDs, inBP_Ptr->BP_ID, true,
                                                    attachmentPnts, attachmentVectors, boneLocPairList, false);
                    // now re-symmetrify these sameTypeSourceBP_IDs if previously noted
                    sameTypeSourceBP_IDs.clear();
                }
                else
                {
                    qDebug() << "Found the parent BP's ID in the list of sub BPs, which is a problem";
                    exit(0);
                }
            }
            if (!tempSubMirrBPsIDs.isEmpty())
                removeAlienBodyParts(alienPtr, tempSubMirrBPsIDs);

            if (inBP_Ptr->BP_Type == AlienModeller::BodyPartsEnum::ExtremBP_Carpal
                    ||
                inBP_Ptr->BP_Type == AlienModeller::BodyPartsEnum::ExtremBP_Digit)
            {
                QVector<BodyPart*> carpals; carpals.clear();
                carpals.append(inBP_Ptr);
                attachDigitsToCarpalsForNewExtremBP(alienPtr, tempSubExtremBPsForAttaching, carpals);
            }
            qDebug() << "reached 11";
            // finally re-attach the detached extremities to this re-JATed BP
            // attachExtremBP_ToParentBP(alienPtr, inBP_Ptr, subExtremIDs);
        }
        else
        {
            QVector<BodyPart*> tempClonedBPs; tempClonedBPs.clear();
            for (int s=0; s<tempSubBPs.length(); s++)
            {
                // clone each BP - NOTE: THis can make the performance really slow. Consider re-attaching these BPs and their mirrors in a clever manner
                qDebug() << "reached modJAT - 1";
                if (!tempSubBPs[s]->isSymmetric)
                {
                    // TODO: Why are we cloning it? Check if we can do it without having to clone the detached subBP
                    tempClonedBPs.append(cloneSkeletalBP_FromAnotherAlien_viaDNA(alienPtr, alienPtr, tempSubBPs[s]));
                }
                // else
                tempSubMirrBPsIDs << tempSubBPs[s]->BP_ID;
                if (s == tempSubBPs.length()-1) // if this is the last loop iteration, then remove all the previous BPs,
                                                // ... assign their IDs to the cloned BPs, and then attach the clones back to
                                                // ... the parent BP
                {
                    // replace the detached sub BPs with the new clones
                    removeAlienBodyParts(alienPtr, tempSubMirrBPsIDs);

                    // TODO: Perhaps use MultiHash here
                    // Multimap to store <BP Type, BP ID> relation
                    for (BodyPart *bp : tempClonedBPs)
                    {
                        alienPtr->alienSkeleton.Jnt2JntRelations_TypeGuided.insert(bp->cloneFromAlienAndBP_Pair.second,
                                                alienPtr->alienSkeleton.Jnt2JntRelations_TypeGuided.value(bp->BP_ID));
                        alienPtr->alienSkeleton.Jnt2JntRelations_TypeGuided.remove(bp->BP_ID);

                        if (alienPtr->alienDNA->skeletonDNA->BP_SpecificDensificationDNA.contains(bp->BP_ID))
                        {
                            QMap<int, QPolygonF> sourceMap = alienPtr->alienDNA->skeletonDNA->BP_SpecificDensificationDNA.value(bp->BP_ID);
                            alienPtr->alienDNA->skeletonDNA->BP_SpecificDensificationDNA.insert(bp->cloneFromAlienAndBP_Pair.second, sourceMap);
                            alienPtr->alienDNA->skeletonDNA->BP_SpecificDensificationDNA.remove(bp->BP_ID);
                        }

                        if (alienPtr->alienDNA->skeletonDNA->BP_BulgificationDNA.contains(bp->BP_ID))
                        {
                            QMap<int, QPolygon> sourceBulgMap = alienPtr->alienDNA->skeletonDNA->BP_BulgificationDNA.value(bp->BP_ID);
                            alienPtr->alienDNA->skeletonDNA->BP_BulgificationDNA.insert(bp->cloneFromAlienAndBP_Pair.second, sourceBulgMap);
                            alienPtr->alienDNA->skeletonDNA->BP_BulgificationDNA.remove(bp->BP_ID);
                        }
                        bp->BP_ID = bp->cloneFromAlienAndBP_Pair.second;
                        bp->cloneFromAlienAndBP_Pair = QPair<int,int>(-1,-1);

                        for (int exID : bp->subExtremityBPsIDs)
                        {
                            alienPtr->extremToParentBP_Map.insert(exID,bp->BP_ID);
                        }
                    }
                }
            }
        }

        // TODO(ismail): Fix this as this doesn't work properly.
        // ... The mirrored BPs don't appear. UPDATE: It seems to be working fine lately
        if (inBP_Ptr->BP_Type == AlienModeller::BodyPartsEnum::BACK_BONE)
        {
            attachAllSkeletalBodyPartsToBackBone(alienPtr);
            symmetrifyAlienBodyParts(alienPtr);
        }
    }

    qDebug() << "Re-JAT-ing of BP done successfully";
}

void AlienModeller::manipulateSkeletalBulkinessDNA_Flags(Alien *alienPtr, bool resetAllEntires, bool cloneBulkDNA,
                                                  bool mutateBP_Entries, bool deleteBP_Entries, bool createBP_Entries,
                                                  QVector<int> *BP_listToDeleteFromBulkinessDNA, QVector<int> *BP_listToMutateInBulkinessDNA,
                                                  bool updateBP_Entries, QVector<int> *BP_listToUpdate)
{

    if (resetAllEntires)
        alienPtr->alienDNA->skeletonDNA->bulkinessDNA_ManipulationFlags.fill(false);
    else
    {
        alienPtr->alienDNA->skeletonDNA->bulkinessDNA_ManipulationFlags[0] = cloneBulkDNA;
        alienPtr->alienDNA->skeletonDNA->bulkinessDNA_ManipulationFlags[1] = mutateBP_Entries;
        alienPtr->alienDNA->skeletonDNA->bulkinessDNA_ManipulationFlags[2] = deleteBP_Entries;
        alienPtr->alienDNA->skeletonDNA->bulkinessDNA_ManipulationFlags[3] = createBP_Entries;
        alienPtr->alienDNA->skeletonDNA->bulkinessDNA_ManipulationFlags[4] = updateBP_Entries;

        alienPtr->alienDNA->skeletonDNA->BP_listToDeleteFromBulkinessDNA.resize(0);
        alienPtr->alienDNA->skeletonDNA->BP_listToDeleteFromBulkinessDNA << *BP_listToDeleteFromBulkinessDNA;

        alienPtr->alienDNA->skeletonDNA->BP_listToMutateInBulkinessDNA.resize(0);
        alienPtr->alienDNA->skeletonDNA->BP_listToMutateInBulkinessDNA << *BP_listToMutateInBulkinessDNA;

        alienPtr->alienDNA->skeletonDNA->BP_listToUpdateInBulkinessDNA.resize(0);
        alienPtr->alienDNA->skeletonDNA->BP_listToUpdateInBulkinessDNA << *BP_listToUpdate;
    }
}

void AlienModeller::updateBP_AttachmentToOtherBP_withAttachPositionAndVector_DNA(Alien *alien, int bpToRemove, int parentBP,
                                                                                 int childBP, QVector3D pos, QVector3D vec)
{
    if (bpToRemove != -1)
        alien->alienDNA->skeletonDNA->BP_AttachmentToOtherBP_withAttachPositionAndVector_DNA.remove(bpToRemove);
    else if (childBP != -1)
    {
        QMap<int, QPair<QVector3D, QVector3D>> tempMap; tempMap.clear();
        tempMap.insert(parentBP, QPair<QVector3D, QVector3D>(pos,vec));
        alien->alienDNA->skeletonDNA->BP_AttachmentToOtherBP_withAttachPositionAndVector_DNA.insert(childBP, tempMap);

        for (QPoint &pt : alien->alienDNA->skeletonDNA->BP_AngleArrangementsFromBackBone_DNA[getBodyPartFromID(alien, childBP)->BP_Type])
        {
            if (pt.x() == -1)
            {
                pt.setX(childBP);
                break;
            }
        }
    }
//    if (alien->alienDNA->skeletonDNA->BP_AttachmentToOtherBP_withAttachPositionAndVector_DNA.isEmpty())
    //        exit(0);
}

void AlienModeller::buildHeadStretchingDNA(Alien *alienPtr, int headID)
{
    QPolygon tempPoly; tempPoly.clear();
    int jntInd = 0;
    int percJnts =  GeometryAlgorithms::getRandomInteger(skeletonRuleSet->headStretching_RS[0].x(),
                    skeletonRuleSet->headStretching_RS[0].y());
    int startStretchEgy =   GeometryAlgorithms::getRandomInteger(skeletonRuleSet->headStretching_RS[1].x(),
                            skeletonRuleSet->headStretching_RS[1].y());
    int stretchScalingPerc =     GeometryAlgorithms::getRandomInteger(skeletonRuleSet->headStretching_RS[2].x(),
                                skeletonRuleSet->headStretching_RS[2].y());

    tempPoly << QPoint(jntInd, percJnts)
             << QPoint(startStretchEgy, stretchScalingPerc);

    alienPtr->alienDNA->skeletonDNA->headStretchingDNA.insert(headID, tempPoly);
}

BodyPart *AlienModeller::modelNewBP_TypeGuided(Alien *alien, int BP_Type)
{
    qDebug() << "About to model new BP";
    // If there is no entry in the RS for this BP type, then instead of adding one, we directly add an entry in the DNA
    // ... with 0 number of BP.
    if (!skeletonRuleSet->numOfSkeletalBodyParts_RS.contains(BP_Type)) // it means there is no entry in the numOfSkeletalBodyParts_DNA
    {
        alien->alienDNA->skeletonDNA->numOfSkeletalBodyParts_DNA.insert(BP_Type,0); //++
    }
    // Now increment the BP count for this BP Type in the DNA
    alien->alienDNA->skeletonDNA->numOfSkeletalBodyParts_DNA[BP_Type] += 1;
    // instantiate the BP
    instantiateNewBodyParts(alien, BP_Type, 1);
    BodyPart *bpPtr = alien->bodyParts.last();

    // TODO(ismail): if there is no entry in skeleton DNA's numOfSkeletalBodyParts_DNA for this BP_Type then we need to add one and specify the
    // ... number of joints for this BP in skeletonRuleSet->numOfJointsPerBP_RS

    // First update the DNA DSs by adding an entry for this newly created BP
    QPolygon tempPoly = skeletonRuleSet->numOfJointsPerBP_RS.value(BP_Type);
    addNewEntryforNumOfJointsPerBP_DNA(alien, BP_Type, tempPoly, bpPtr->BP_ID);

    if (BP_Type != AlienModeller::BodyPartsEnum::HEADSubBP_Maxilla
            &&
        BP_Type != AlienModeller::BodyPartsEnum::HEADSubBP_Mandible
            &&
        BP_Type != AlienModeller::BodyPartsEnum::HEADSubBP_Zygomatic)
    {
        if (skeletonRuleSet->BP_SizeComparedToBackBone_RS.contains(BP_Type))
        {
            tempPoly = skeletonRuleSet->BP_SizeComparedToBackBone_RS.value(BP_Type);
            alien->alienDNA->skeletonDNA->BP_SizeComparedToBackBone_DNA[BP_Type].append(QPoint(bpPtr->BP_ID,GeometryAlgorithms::getRandomInteger(tempPoly[alien->Alien_Type_ID].x(),tempPoly[alien->Alien_Type_ID].y())));
        }
        if (skeletonRuleSet->BP_AngleArrangementsFromBackBone_RS.contains(BP_Type))
        {
            tempPoly = skeletonRuleSet->BP_AngleArrangementsFromBackBone_RS.value(BP_Type);
            alien->alienDNA->skeletonDNA->BP_AngleArrangementsFromBackBone_DNA[BP_Type].append(QPoint(bpPtr->BP_ID,GeometryAlgorithms::getRandomInteger(tempPoly[alien->Alien_Type_ID].x(),tempPoly[alien->Alien_Type_ID].y())));
        }
    }

    addNewEntryforBP_JAT_rootJoint_DNA(alien, BP_Type, 1);


    // Now start modelling the new BP by computing its joints and bones
    QMap<int, QPolygon> tempNumOfJointsPerBP_DNA;
    QPolygon tempPoly_2; tempPoly_2.clear();
    tempPoly_2 << alien->alienDNA->skeletonDNA->numOfJointsPerBP_DNA[BP_Type].last(); //++
    tempNumOfJointsPerBP_DNA.insert(BP_Type, tempPoly_2);
    qDebug() << tempNumOfJointsPerBP_DNA;
    instantiateNumOfJointsForAllBPs_TypeGuided(alien, tempNumOfJointsPerBP_DNA);

    QVector<BodyPart*> tempBP_List; tempBP_List.clear();
    tempBP_List << bpPtr;
    specifyJnt2JntRelations_TypeGuided(alien, tempBP_List, false);
    qDebug() << *alien->alienSkeleton.Jnt2JntRelations_TypeGuided.value(bpPtr->BP_ID);

    QVector<Joint*> tempJntList; tempJntList.resize(0);
    getAllBP_Joints(bpPtr, tempJntList);
//    qDebug() << tempJntList.length();
    initializeJointsArrangementTrendForCurrentAlien(alien, tempJntList, false);

    QMap<int,int> tempParentJntCompletionCheckList; // checklist for when the all the child joints's positions are computed. Value(int) is 1 for DONE and 0 for NOT_DONE.
    QMap<int,int> tempChildJntPositionCheckList;    // checklist for when the joint's position is computed.

    for (int i=0; i<tempJntList.length(); i++)
    {
        QPair<QVector3D, QVector3D> tempPair;
        tempPair.first = QVector3D(0.0,0.0,1.0); tempPair.second = QVector3D(0.0,0.0,0.0);
        alien->alienSkeleton.vectorLocationCoupleForEachJoint_TypeGuided.insert(tempJntList[i]->BP_ID, tempPair);

        tempParentJntCompletionCheckList.insert(tempJntList[i]->BP_ID,0);
        tempChildJntPositionCheckList.insert(tempJntList[i]->BP_ID,0);
    }

    QMap<int, QPolygonF> tempCopy_BP_JAT_rootJoint_DNA; tempCopy_BP_JAT_rootJoint_DNA.clear();
    QPolygonF tempJAT_PolyF; tempJAT_PolyF.clear();

    QPolygonF tempPolyF = alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.BP_JAT_rootJoint_DNA.value(BP_Type);
    tempJAT_PolyF << tempPolyF[tempPolyF.length()-2] << tempPolyF[tempPolyF.length()-1];
    tempCopy_BP_JAT_rootJoint_DNA.insert(BP_Type, tempJAT_PolyF);

    compute_JAT_AND_VecLocCplForEachJnt_ForBP(alien, bpPtr->BP_ID, tempCopy_BP_JAT_rootJoint_DNA,
                                              tempParentJntCompletionCheckList, tempChildJntPositionCheckList,
                                              0.0);

    qDebug() << "reached last";

    // model the joints of this BP
    modelAllAlienJoints_TypeGuided(alien, tempJntList);

    qDebug() << *(alien->alienSkeleton.Jnt2JntRelations_TypeGuided.value(bpPtr->BP_ID));

    // If this is Head, then we need to do the stretching of the Head
    // First build the headStretchDNA, and then do the streching
    if (BP_Type == AlienModeller::BodyPartsEnum::HEAD)
    {
        // buildHeadStretchingDNA(alien, bpPtr->BP_ID);
        // stretchHead(alien, bpPtr);
    }

    // TODO(ismail): make a method for extracting a sub map of Jnt2JntRelations_TypeGuided given a set of BP_IDs
    // ... COuld be handy for some purposes
    QMap<int, QPolygon*> tempJnt2JntRelations;
    QPolygon *tempNewPolyPtr = new QPolygon(*(alien->alienSkeleton.Jnt2JntRelations_TypeGuided.value(bpPtr->BP_ID)));
    tempJnt2JntRelations.insert(bpPtr->BP_ID, tempNewPolyPtr);

    // model the bones of this BP
    modelAllAlienBones_TypeGuided(alien, tempJnt2JntRelations);

    qDebug() << "New BP modelled successfully";

    return bpPtr;

}

/* LEGACY METHOD
void AlienModeller::buildSkeletalBP_BulkinessDNA(QVector<int> &BP_ID_List)
{
    // set the Skeletal_BP_Bulkiness_DNA from the Skeletal_BP_Bulkiness_RS
    for (int i=0; i<BP_ID_List.length(); i++)
    {
        // Get hold of this BP_Type_ID as we need the it to look it up in the Skeletal_BP_Bulkiness_RS
        // Also we need to get hold of that BP because
        // TODO(ismail): Perhaps it would be better to pass a list of BP Pointers to this method as parameter, so the following for loop can be avoided
        int tempBP_Type_ID=-1;
        BodyPart *bpPtr=NULL;
        for (int bp=0; bp<alienList.last()->bodyParts.length(); bp++)
        {
            if (alienList.last()->bodyParts[bp]->BP_ID == BP_ID_List[i])
            {
                tempBP_Type_ID = alienList.last()->bodyParts[bp]->BP_Type;
                bpPtr = alienList.last()->bodyParts[bp];
            }
        }

        if (alienList.last()->alienDNA->skeletonDNA->Skeletal_BP_Bulkiness_DNA.contains(BP_ID_List[i])) // remove it first
        {
            // First delete all the QPoints from the QPolygon
            QPolygonF tempPoly_1 = alienList.last()->alienDNA->skeletonDNA->Skeletal_BP_Bulkiness_DNA.value(BP_ID_List[i]);
            // TODO(ismail): Try using the QVector::erase (QVector::iterator QVector::erase(QVector::iterator begin, QVector::iterator end)) to avoid the for loop
            for (int tpEnt=0; tpEnt<tempPoly_1.length(); tpEnt++)
            {
                tempPoly_1.remove(tpEnt);
            }
            int isRemoved = alienList.last()->alienDNA->skeletonDNA->Skeletal_BP_Bulkiness_DNA.remove(BP_ID_List[i]);
            qDebug() << "Old entry removed from BP_AngleArrangementsFromBackBone_DNA";
        }
        // add a new one anyways
        if (skeletonRuleSet->Skeletal_BP_Bulkiness_RS.contains(tempBP_Type_ID))
        {
            QPolygonF tempPoly_2 = skeletonRuleSet->Skeletal_BP_Bulkiness_RS.value(tempBP_Type_ID);
            QPolygonF tempPoly_3;                 tempPoly_3.resize(0);
            QVector<float> tempLikelihoodExpansionList_1;
            tempLikelihoodExpansionList_1.resize(0);
            for (int j=0; j<4; j++)
            {
                for (int k=0; k<static_cast<int>(tempPoly_2[j].y()); k++)
                {
                    tempLikelihoodExpansionList_1.append(tempPoly_2[j].x());
                }
            }

            bool isTrendVarying=true;
            int tempStart = 0;
            float bulkTrend_ID=0;
            int tempRandInd=0, flag_1=0;

            for (int boneInd=0; boneInd<bpPtr->subBodyParts.length(); boneInd++)
            {
                if (bpPtr->subBodyParts[boneInd]->BP_Type == AlienModeller::BodyPartsEnum::BONE)
                {
                    if (isTrendVarying)
                    {
                        tempRandInd = GeometryAlgorithms::getRandomInteger(tempStart, tempLikelihoodExpansionList_1.length());
                        bulkTrend_ID = tempLikelihoodExpansionList_1.at(tempRandInd);
                        qDebug() << "bulkTrend_ID " << bulkTrend_ID;

                        if (bulkTrend_ID == 0 || flag_1 == 1) // trend will be varying. This condition should only happen once if for the first time bulkTrend_ID=0
                        {
                            flag_1=1;
                            tempStart = static_cast<int>(tempPoly_2[0].y());
                            isTrendVarying = true;
                            qDebug() << "trend is varying";
                            if (bulkTrend_ID == 0)
                            {
                                tempRandInd = GeometryAlgorithms::getRandomInteger(tempStart, tempLikelihoodExpansionList_1.length());
                                bulkTrend_ID = tempLikelihoodExpansionList_1.at(tempRandInd);
                            }
                        }
                        else
                        {
                            isTrendVarying = false;
                            qDebug() << "trend is consistent";
                            qDebug() << "bulkTrend_ID " << bulkTrend_ID;
                        }
                    }

                    int tempVal = GeometryAlgorithms::getRandomInteger(tempPoly_2[6].x(),tempPoly_2[6].y());
                    float xVal=0, yVal=0;
                    if (tempVal <= -80)
                    {
                        xVal = 1;
                        yVal = 0;
                    }
                    else if (tempVal > -80 && tempVal <= -60)
                    {
                        xVal = 0;
                        yVal = 1;
                    }
                    else if (tempVal > -60)
                    {
                        xVal = 1;
                        yVal = 1;
                    }

                    if (tempPoly_3.isEmpty()) // This condition should only happen once i.e. for the very first time
                    {
                        tempPoly_3  << QPointF(bulkTrend_ID,GeometryAlgorithms::getRandomFloat(tempPoly_2[4].x(),tempPoly_2[4].y()))
                                    << QPointF(xVal, yVal);
                    }
                    else
                    {
                        tempPoly_3  << QPointF(bulkTrend_ID,GeometryAlgorithms::getRandomFloat(tempPoly_2[5].x(),tempPoly_2[5].y()))
                                    << QPointF(xVal, yVal);
                    }
                }
            }
            alienList.last()->alienDNA->skeletonDNA->Skeletal_BP_Bulkiness_DNA.insert(BP_ID_List[i], tempPoly_3);
            qDebug() << "New entry added to BP_AngleArrangementsFromBackBone_DNA";
        }
        else
        {
            qDebug() << "there is no entry for this BP in the Skeletal_BP_Bulkiness_RS";
            break;
        }
    }
}
*/

void AlienModeller::modelNewAlien()
{
//    // Call TorsoModeller method to model a torso and assign the return pointer to the member variable pointer of UpperTorso
//    uppTorsoPtr = torsoModellerPtr->modelUpTorso();

    // TODO(ismail): REFACTOR: need to have a pointer to the current alien
    Alien *alien = new Alien();
    alienList.append(alien);
    alien->alienID = ++AlienID_Counter;
//    BP_ID_Counter_ForAllAliens.append(0);

    // first initialize all the things related to the skeleton
    initializeAlienSkeleton(alien);

    // call METHOD: generateAlienSkeleton()
    generateAlienSkeleton(alien);
}

// TODO(ismail): Better return the newly created alien' pointer
void AlienModeller::modelNewAlien_TypeGuided(QString alienTypeStr, QMap<int,QPair<int,int>> bpAmntsMap)
{
    // TODO(ismail): REFACTOR: need to have a pointer to the current alien
    // TODO(ismail): Make a method for creating a new Alien that encapsulates the code below
    Alien *newAlien = new Alien();
    alienList.append(newAlien);
    newAlien->alienID = ++AlienID_Counter;
//    BP_ID_Counter_ForAllAliens.append(0);

    int alienType = -1;

//    if (alienTypeStr == "Bipedal")
//        alienType = 0;
//    else if (alienTypeStr == "Aquatic")
//        alienType = 2;
//    else if (alienTypeStr == "Anthropodal")
//        alienType = 5;
//    else
//        alienType = 0; // default is Bipedal
    alienType = alienTypeMap.value(alienTypeStr);
    if (alienType == -1)
        alienType = 0; // default is Bipedal

    newAlien->Alien_Type_ID = alienType;
    // Build the DNA for the alien
    buildCompleteDNA_FromRulesets(newAlien, bpAmntsMap);
    // Build the skeleton of alien
    buildAlienSkeleton_TypeGuided(newAlien);

//    BodyPart *headPtr = modelNewHead(newAlien);

    // The following call should be done within the buildAlienSkeleton_TypeGuided() method towards the end of that method
//    attachAllSkeletalBodyPartsToBackBone(newAlien);

    // Now we can bulk up the BPs according to our choice
    // first initialize the Bulkiness DNA.

//    initSkeletalBP_BulkinessDNA(newAlien);
//    QVector<int> BP_ID_List; BP_ID_List.resize(0);
//    for (int i=0; i<alienList.last()->bodyParts.length(); i++)
//    {
//        if (alienList.last()->bodyParts[i]->BP_Type == AlienModeller::BodyPartsEnum::JOINT  ||
//                alienList.last()->bodyParts[i]->BP_Type == AlienModeller::BodyPartsEnum::BONE)
//            continue;
//        else
//            BP_ID_List.append(alienList.last()->bodyParts[i]->BP_ID);

//    }
//    bulkUpBodyPart(newAlien, BP_ID_List);

//    updateBoneBendingDNA(newAlien);
//    for (int i=0; i<newAlien->bodyParts.length(); i++)
//    {
//        if (newAlien->bodyParts[i]->BP_Type == AlienModeller::BodyPartsEnum::LOWER_LIMB)
//        {
//            for (int j=0; j<newAlien->bodyParts[i]->subBodyParts.length(); j++)
//            {
//                if (newAlien->bodyParts[i]->subBodyParts[j]->BP_Type == AlienModeller::BodyPartsEnum::BONE)
//                {
//                    bendBone(newAlien, dynamic_cast<Bone*>(newAlien->bodyParts[i]->subBodyParts[j]));
////                    break;
//                }
//            }
//        }
//    }

//    // Head Sub BPs alignment
//    // build the Head Sub BP Align DNA and align the Head Sub BPs
//    if (!newAlien->alienHeads.isEmpty())
//    {
//        for (Head *headPtr : newAlien->alienHeads)
//        {
//            buildHeadSubBP_AlignmentDNA(newAlien, headPtr->BP_ID);
//            alignHeadSubBPs(newAlien, headPtr->BP_ID);
//        }
//    }

    // The following call should be made after all the mutations have been applied.

//    buildBoneSubDivCellsDNA_forAllBPs(newAlien);
    attachAllSkeletalBodyPartsToBackBone(newAlien);
    qDebug() << "New Alien Created";

    // Now we can smooth parts of the skeleton if needed AND We can also give different shapes to the skeleton
}

Alien *AlienModeller::getAlienFromID(int alienID)
{
    for (Alien *alienPtr : this->alienList)
    {
        if (alienPtr->alienID == alienID)
            return alienPtr;
    }

    return nullptr;
}

void AlienModeller::specifyBP_TypeColorMapForAlien(Alien *alienPtr)
{

}

void AlienModeller::initializeBP_TypeColorMap()
{
    // first initialize the color map
    colorMap.clear();
//    colorMap.insert(AlienModeller::BodyPartsEnum::LOWER_LIMB, QVector3D(0.0f, 0.5f, 1.0f)); // Blue Green
//    colorMap.insert(AlienModeller::BodyPartsEnum::UPPER_LIMB, QVector3D(0.0f, 0.5f, 1.0f)); // baby blue
//    colorMap.insert(AlienModeller::BodyPartsEnum::BACK_BONE, QVector3D(0.0f, 0.5f, 1.0f)); // Orange

//    colorMap.insert(AlienModeller::BodyPartsEnum::LOWER_LIMB, QVector3D(0.0f, 0.5f, 0.5f)); // Blue Green
//    colorMap.insert(AlienModeller::BodyPartsEnum::UPPER_LIMB, QVector3D(0.0f, 0.5f, 1.0f)); // baby blue
//    colorMap.insert(AlienModeller::BodyPartsEnum::BACK_BONE, QVector3D(1.0f, 0.5f, 0.0f)); // Orange

    colorMap.insert(AlienModeller::BodyPartsEnum::LOWER_LIMB, QVector3D(1.0f, 0.5f, 0.5f)); // Color 1
    colorMap.insert(AlienModeller::BodyPartsEnum::UPPER_LIMB, QVector3D(0.87f, 0.58f, 0.98f)); // Color 2
    colorMap.insert(AlienModeller::BodyPartsEnum::BACK_BONE, QVector3D(0.0f, 0.5f, 1.0f)); // Color 3
    colorMap.insert(AlienModeller::BodyPartsEnum::ExtremBP_Carpal, colorMap_2.value(AlienModeller::ColorEnum::ORANGE));
    colorMap.insert(AlienModeller::BodyPartsEnum::ExtremBP_Digit, colorMap_2.value(AlienModeller::ColorEnum::ORCHID));
    colorMap.insert(AlienModeller::BodyPartsEnum::HEAD, colorMap_2.value(AlienModeller::ColorEnum::STEELBLUE));

    // 0.2f, 0.5f, 1.0f
}

void AlienModeller::setColorMap()
{
    // Corresponds with the color enum
    this->colorMap_2.clear();

    this->colorMap_2.insert(AlienModeller::ColorEnum::RED, QVector3D(1.0, 0.0, 0.0));
    this->colorMap_2.insert(1, QVector3D(0.0, 1.0, 0.0));
    this->colorMap_2.insert(2, QVector3D(0.0, 0.0, 1.0));
    this->colorMap_2.insert(3, QVector3D(1.000, 0.647, 0.000));
    this->colorMap_2.insert(4, QVector3D(0.802, 0.000, 0.802));
    this->colorMap_2.insert(5, QVector3D(0.541, 0.169, 0.886));
    this->colorMap_2.insert(6, QVector3D(0.275, 0.510, 0.906));
    this->colorMap_2.insert(7, QVector3D(0.502, 0.502, 0.000));
    this->colorMap_2.insert(8, QVector3D(1.0f, 0.5f, 0.5f));
    this->colorMap_2.insert(9, QVector3D(0.87f, 0.58f, 0.98f));
    this->colorMap_2.insert(10, QVector3D(0.0f, 0.5f, 1.0f));
    this->colorMap_2.insert(11, QVector3D(0.855, 0.439, 0.839));
    this->colorMap_2.insert(13, QVector3D(0.0, 0.0, 0.0));
}

void AlienModeller::initAlienTypeMap()
{
    alienTypeMap.clear();

    alienTypeMap.insert("Bipedal", 0);
    alienTypeMap.insert("Quadpedal", 1);
    alienTypeMap.insert("Aquatic", 2);
    alienTypeMap.insert("Amphibious", 3);
    alienTypeMap.insert("Octapedal", 4);
    alienTypeMap.insert("Anthropodal", 5);
}

void AlienModeller::initBP_TypeMap()
{
    BP_TypeMap.clear();

    BP_TypeMap.insert("Joint", 0);
    BP_TypeMap.insert("Bone", 1);
    BP_TypeMap.insert("Lower Limb", 2);
    BP_TypeMap.insert("Upper Limb", 3);
    BP_TypeMap.insert("Back Bone", 4);
    BP_TypeMap.insert("Tail", 5);
    BP_TypeMap.insert("Head", 6);
    BP_TypeMap.insert("Hand", 7);
    BP_TypeMap.insert("Foot", 8);
    BP_TypeMap.insert("Neck", 9);
    BP_TypeMap.insert("Rib", 10);
}

void AlienModeller::removeAlienBodyParts(Alien *alienPtr, QVector<int> &BP_ID_List)
{
    for (int i=0; i<BP_ID_List.length(); i++)
    {
        BodyPart *BP_Ptr = getBodyPartFromID(alienPtr, BP_ID_List[i]);
        if (BP_Ptr)
        {
            // Remove and delete all the extremities that are subExtremities of this BP
            while (!BP_Ptr->subExtremityBPsIDs.isEmpty())
            {
                ExtremityBP *tempExBP = getExtremityBP_FromID(alienPtr, BP_Ptr->subExtremityBPsIDs.last());
                removeAndDeleteExtremityBP_FromAlien(alienPtr, tempExBP, BP_Ptr);
            }

            // TODO: if this was a carpal or digit, delete it from its extremity BP lists i.e. ExtBaseBP list and ExtDigits
            if (BP_Ptr->BP_Type == AlienModeller::BodyPartsEnum::ExtremBP_Carpal
                    ||
                BP_Ptr->BP_Type == AlienModeller::BodyPartsEnum::ExtremBP_Digit)
            {
                for (ExtremityBP *extBP : alienPtr->alienExtremities)
                {
                    // check if this extBP has the selected BP (carpal or digit)
                    // Now check if this selected BP belongs to its Carpals or Digits
                    if (extBP->extremBP_basePtr->extremBP_BaseCarpals.removeOne(BP_Ptr))
                        break;
                    else if (extBP->extremBP_Digits.removeOne(BP_Ptr))
                        break;
                }
            }

            // TODO: Also delete any subBPs that are attached to this BP

            // remove it from the cloneBPs list
            if (alienPtr->cloneBPs.contains(BP_Ptr))
                alienPtr->cloneBPs.removeOne(BP_Ptr);

            // Remove all its Bones and Joints (and completely delete them)
            QVector<Joint *> tempJnts; tempJnts.clear();
            QVector<Bone *> tempBones; tempBones.clear();
            getAllBP_Bones(BP_Ptr, tempBones);
            getAllBP_Joints(BP_Ptr, tempJnts);

            // delete it from the Joints/Bones list
            // delete it from the BP list

            // also remove the joints entries from JATs
            removeAssociatedJATsAndOtherDSsForRemovableJnts(alienPtr, getAllJntIDsOfBP(BP_Ptr));

            for (int b=0; b<tempBones.length(); b++)
            {
                alienPtr->alienBones.removeAt(alienPtr->alienBones.indexOf(tempBones[b]));
                alienPtr->bodyParts.removeAt(alienPtr->bodyParts.indexOf(tempBones[b]));
            }
            for (int j=0; j<tempJnts.length(); j++)
            {
                alienPtr->alienJoints.removeAt(alienPtr->alienJoints.indexOf(tempJnts[j]));
                alienPtr->bodyParts.removeAt(alienPtr->bodyParts.indexOf(tempJnts[j]));
            }

            // Get the parent of this BP (if any)
            BodyPart *parentBP_Ptr = nullptr;
            for (BodyPart *bPtr : alienPtr->bodyParts)
            {
                if (bPtr->subBodyParts.contains(BP_Ptr))
                {
                    parentBP_Ptr = bPtr;
                    break;
                }
            }

            if (BP_Ptr->BP_Type == AlienModeller::BodyPartsEnum::BONE)
            {
                if (!removeBone(alienPtr, parentBP_Ptr, dynamic_cast<Bone*>(BP_Ptr)))
                {
                    qDebug() << "Bone couldn't be removed/deleted in the removeAlienBodyParts() method";
                    exit(0);
                }
                else
                    BP_Ptr = nullptr;
            }
            else if (BP_Ptr->BP_Type == AlienModeller::BodyPartsEnum::JOINT)
            {
                if (!removeJoint(alienPtr, parentBP_Ptr, dynamic_cast<Joint*>(BP_Ptr)))
                {
                    qDebug() << "Joint couldn't be removed/deleted in the removeAlienBodyParts() method";
                    exit(0);
                }
                else
                    BP_Ptr = nullptr;
            }
            else if (BP_Ptr->BP_Type == AlienModeller::BodyPartsEnum::LOWER_LIMB)
                alienPtr->alienLowerLimbs.removeAt(alienPtr->alienLowerLimbs.indexOf(dynamic_cast<LowerLimb*>(BP_Ptr)));
            else if (BP_Ptr->BP_Type == AlienModeller::BodyPartsEnum::UPPER_LIMB)
                alienPtr->alienUpperLimbs.removeAt(alienPtr->alienUpperLimbs.indexOf(dynamic_cast<UpperLimb*>(BP_Ptr)));
            else if (BP_Ptr->BP_Type == AlienModeller::BodyPartsEnum::BACK_BONE)
                alienPtr->alienBackBones.removeAt(alienPtr->alienBackBones.indexOf(dynamic_cast<BackBone*>(BP_Ptr)));
            else if (BP_Ptr->BP_Type == AlienModeller::BodyPartsEnum::HEAD)
                alienPtr->alienHeads.removeOne(dynamic_cast<Head*>(BP_Ptr));


            // remove this BP from the backbone's subBPs list
            // TODO(ismail): This shouldn't be too specific like a BB, because a BP
            // ... can have any BP as its parent BP. so this needs to be corrected
            if (parentBP_Ptr)
            {
                if (parentBP_Ptr->subBodyParts.contains(BP_Ptr))
                {
                    qDebug() << "This was a sub-BP of " << parentBP_Ptr->BP_Type << ", so it'll be removed from its subBPs list";
                    parentBP_Ptr->subBodyParts.removeOne(BP_Ptr);
                }
            }

            qDebug() << "done 1";

    //        if (!alienPtr->alienBackBones.isEmpty())
    //        {
    //            if (alienPtr->alienBackBones[0]->subBodyParts.contains(BP_Ptr))
    //            {
    //                qDebug() << "This was a sub-BP of backbone so it'll be deleted";
    //                alienPtr->alienBackBones[0]->subBodyParts.removeOne(BP_Ptr);
    //            }
    //        }

    //        alienPtr->bodyParts.removeAt(alienPtr->bodyParts.indexOf(BP_Ptr));
            if (alienPtr->bodyParts.contains(BP_Ptr))
                alienPtr->bodyParts.removeOne(BP_Ptr);
            else if (alienPtr->seperateCloneBPs.contains(BP_Ptr))
                alienPtr->seperateCloneBPs.removeOne(BP_Ptr);

            if (BP_Ptr)
            {
                if (alienPtr->alienDNA->skeletonDNA->numOfSkeletalBodyParts_DNA.value(BP_Ptr->BP_Type) > 0)
                    alienPtr->alienDNA->skeletonDNA->numOfSkeletalBodyParts_DNA[BP_Ptr->BP_Type]--;

                delete BP_Ptr;
                BP_Ptr = nullptr;
            }
        }
    }

    if (alienPtr->bodyParts.empty())
    {
        qDebug() << "backbones:" << alienPtr->alienBackBones.length()
                 << "Upper Limbs:" << alienPtr->alienUpperLimbs.length()
                 << "Lower Limbs:" << alienPtr->alienLowerLimbs.length()
                 << "Bones:" << alienPtr->alienBones.length()
                 << "Joints:" << alienPtr->alienJoints.length();
        qDebug() << "all BPs removed";
    }
//    else
//    {
//        qDebug() << "BodyParts after removing this BP :" << alienPtr->bodyParts.length();

////        exit(0);
    //    }
}

void AlienModeller::deleteAliens(QVector<int> aliensToDelete_ID_List)
{
    // First delete the BPs
    // Then delete the aliens
    bool delFlg=true;

    for (int i=0; i<aliensToDelete_ID_List.length(); i++)
    {
        qDebug() << "Deleting Alien";
        delFlg=true;
        Alien *tempAlienPtr = getAlienFromID(aliensToDelete_ID_List[i]);
        // TODO: if any of the cloned aliens were cloned from tempAlienPtr, then think about not deleting this alien
        if (tempAlienPtr)
        {
            if (delFlg)
            {
                QVector<int> tempBP_IDs; tempBP_IDs.clear();
                QVector<int> tempSepBP_IDs; tempSepBP_IDs.clear();
                getAllBP_ID_List(tempAlienPtr, tempBP_IDs, tempSepBP_IDs);
                removeAlienBodyParts(tempAlienPtr, tempBP_IDs);
                alienList.removeAt(alienList.indexOf(tempAlienPtr));
                // TODO: Also delete all its extremity BPs.
                delete tempAlienPtr;
            }
        }
    }
}

BodyPart* AlienModeller::replaceBP_WithAnotherBP(Alien *alienPtr, BodyPart *origBP_Ptr, BodyPart *inBP_Ptr)
{

    qDebug() << "About to replace BP with another BP";
    // append it to the bodyparts main list
    // remove it's clone from the smoothingBackupCloneBPs
    // remove the origBP_Ptr from the bodyparts main list

    // Backup the relevant attributes of inBP_Ptr
    int tempBP_ID = origBP_Ptr->BP_ID;
    int tempSourceID = origBP_Ptr->clonedFrom;
    QPair<int,int> tempSrcCloneFrom = origBP_Ptr->cloneFromAlienAndBP_Pair;
    bool isSymmetric = origBP_Ptr->isSymmetric;
    int mirroredFrom = origBP_Ptr->mirrorOf;
    QVector<int> subExtIDs = origBP_Ptr->subExtremityBPsIDs;

    // We need to remove this BP from its parent's subBPs list and then at the end re-assign this BP as a suBP of its parent
    BodyPart *parentBP_Ptr = getParentOfThisBP(alienPtr, origBP_Ptr);
    if (parentBP_Ptr)
        parentBP_Ptr->subBodyParts.removeOne(origBP_Ptr);

    // TODO: Also, if this is an extremity carpal or digit and if sourceAlien is same destAlien, re-assign it to its parent extremityBP (and extremity base)
    // find the extremity that the source carpal or source digit belongs to
    if (inBP_Ptr->BP_Type == AlienModeller::BodyPartsEnum::ExtremBP_Carpal
            ||
        inBP_Ptr->BP_Type == AlienModeller::BodyPartsEnum::ExtremBP_Digit)
    {
        for (ExtremityBP *extBP : alienPtr->alienExtremities)
        {
            // check if this extBP has the selected BP (carpal or digit)
            // Now check if this selected BP belongs to its Carpals or Digits
            if (extBP->extremBP_basePtr->extremBP_BaseCarpals.contains(origBP_Ptr)) // we have found the Ext BP whose carpal or digit has been selected by mouse
            {
                // store it in the extBP->extremBP_basePtr->extremBP_BaseCarpals
                extBP->extremBP_basePtr->extremBP_BaseCarpals.insert(extBP->extremBP_basePtr->extremBP_BaseCarpals.indexOf(origBP_Ptr), inBP_Ptr);
                // extBP->extremBP_basePtr->extremBP_BaseCarpals.append(inBP_Ptr);
                break;
            }
            else if (extBP->extremBP_Digits.contains(origBP_Ptr))
            {
                // store it in the extBP->extremBP_Digits
                extBP->extremBP_Digits.insert(extBP->extremBP_Digits.indexOf(origBP_Ptr), inBP_Ptr);
                // extBP->extremBP_Digits.append(inBP_Ptr);
                break;
            }
        }
    }

    QPolygonF tempJAT_PolyF = alienPtr->alienDNA->skeletonDNA->skeletal_JATs_DNA.BP_to_JAT_rootJoint_Map_DNA.value(inBP_Ptr->BP_ID);
    // Make sure to update all those DNA DS and any other DSs where we have the original BP's ID
    alienPtr->alienDNA->skeletonDNA->skeletal_JATs_DNA.BP_to_JAT_rootJoint_Map_DNA.insert(tempBP_ID, tempJAT_PolyF);
    // TODO: This should be called inside removeAssociatedJATsAndOtherDSsForRemovableJnts
    alienPtr->alienDNA->skeletonDNA->skeletal_JATs_DNA.BP_to_JAT_rootJoint_Map_DNA.remove(inBP_Ptr->BP_ID);

    // If the original BP was densified, the new BP will also be densified inside the clone method.
    // ... but we also need to update the BP_SpecificDensificationDNA so it contains the up to date densification DNA with new Bone IDs
    // TODO: Inspect this as it may cause a BUG, because the bone IDs will not be the same for two different BPs
    if (alienPtr->alienDNA->skeletonDNA->BP_SpecificDensificationDNA.contains(inBP_Ptr->BP_ID))
    {
        QMap<int, QPolygonF> sourceMap = alienPtr->alienDNA->skeletonDNA->BP_SpecificDensificationDNA.value(inBP_Ptr->BP_ID);
        alienPtr->alienDNA->skeletonDNA->BP_SpecificDensificationDNA.insert(tempBP_ID, sourceMap);
        alienPtr->alienDNA->skeletonDNA->BP_SpecificDensificationDNA.remove(inBP_Ptr->BP_ID);
    }

    if (alienPtr->alienDNA->skeletonDNA->BP_BulgificationDNA.contains(inBP_Ptr->BP_ID))
    {
        QMap<int, QPolygon> sourceBulgMap = alienPtr->alienDNA->skeletonDNA->BP_BulgificationDNA.value(inBP_Ptr->BP_ID);
        alienPtr->alienDNA->skeletonDNA->BP_BulgificationDNA.insert(tempBP_ID, sourceBulgMap);
        alienPtr->alienDNA->skeletonDNA->BP_BulgificationDNA.remove(inBP_Ptr->BP_ID);
    }

    if (!alienPtr->alienDNA->skeletonDNA->BP_SizeComparedToBackBone_DNA[origBP_Ptr->BP_Type].isEmpty())
        alienPtr->alienDNA->skeletonDNA->BP_SizeComparedToBackBone_DNA[origBP_Ptr->BP_Type].removeLast();
    if (!alienPtr->alienDNA->skeletonDNA->BP_AngleArrangementsFromBackBone_DNA[origBP_Ptr->BP_Type].isEmpty())
        alienPtr->alienDNA->skeletonDNA->BP_AngleArrangementsFromBackBone_DNA[origBP_Ptr->BP_Type].removeLast();

    // sort out the Jnt2JntRel DS
    QPolygon *newPoly = new QPolygon(*(alienPtr->alienSkeleton.Jnt2JntRelations_TypeGuided.value(inBP_Ptr->BP_ID)));
    alienPtr->alienSkeleton.Jnt2JntRelations_TypeGuided.insert(tempBP_ID, newPoly);
    alienPtr->alienSkeleton.Jnt2JntRelations_TypeGuided.remove(inBP_Ptr->BP_ID);

    // Now remove the Original BP
    QVector<int> tempBP_ID_List_1; tempBP_ID_List_1.clear();
    tempBP_ID_List_1 << origBP_Ptr->BP_ID;
    removeAlienBodyParts(alienPtr, tempBP_ID_List_1); // remove the original BP

    inBP_Ptr->BP_ID = tempBP_ID;
    inBP_Ptr->clonedFrom = tempSourceID;
    inBP_Ptr->cloneFromAlienAndBP_Pair = tempSrcCloneFrom;
    inBP_Ptr->isSymmetric = isSymmetric;
    inBP_Ptr->mirrorOf = mirroredFrom;

    for (int subExID : subExtIDs)
        alienPtr->extremToParentBP_Map.insert(subExID,inBP_Ptr->BP_ID);

    // Finally re-attach the inBP to the parent of Original BP
    if (parentBP_Ptr)
    {
        if (!parentBP_Ptr->subBodyParts.contains(inBP_Ptr))
            parentBP_Ptr->subBodyParts.append(inBP_Ptr);
        updateBP_AttachmentToOtherBP_withAttachPositionAndVector_DNA(alienPtr, -1,
                                                                     parentBP_Ptr->BP_ID,
                                                                     inBP_Ptr->BP_ID, inBP_Ptr->getPivotPoint(),
                                                                     inBP_Ptr->getPivotVector());
    }

    return inBP_Ptr;
}

QPair<int, int> AlienModeller::applySeperateBPChangesToOrigBPInAlien(Alien *alienPtr, BodyPart *sepBP_Ptr, BodyPart *origBP_Ptr)
{
    qDebug() << "about to apply changes from seperate to the orig BP";
    QPair<int, int> bpAndMirrPair = QPair<int,int>(-1,-1);
    // ALGORITHM:
    //  Replace the modified BP in the seperate clones list with the original BP from which it was cloned

    // Important considerations:
    //      If the BP is cloned from the backbone then when replacing, make sure to detach all the sub BPs
    //      ... of the original backbone before deleting it. Then attach those sub BPs to this new
    //      ... backbone BP

    BodyPart *tempSepBP = sepBP_Ptr;
    if (alienPtr->seperateCloneBPs.contains(sepBP_Ptr))
    {
        qDebug() << "reached 1";
        alienPtr->seperateCloneBPs.removeOne(sepBP_Ptr);
        qDebug() << "reached 2";
    }
    else
    {
        qDebug() << "Please select a BP from the seperate BPs panels";
        exit(0);
    }

    // We need to remove this BP from its parent's subBPs list and then at the end re-assign this BP as a suBP of its parent
    BodyPart *parentBP_Ptr = getParentOfThisBP(alienPtr, origBP_Ptr);
    if (parentBP_Ptr)
        parentBP_Ptr->subBodyParts.removeOne(origBP_Ptr);

    QVector3D tempPos = origBP_Ptr->getPivotPoint();
    QVector3D tempVec = origBP_Ptr->getPivotVector();

    QVector<int> tempBP_ID_List_1; tempBP_ID_List_1.clear();
    BodyPart *mirrPtr=nullptr;
    if (!origBP_Ptr->isSymmetric)
        mirrPtr = getMirrorOrOriginalBP(alienPtr, origBP_Ptr);
    if (mirrPtr)
    {
        tempBP_ID_List_1 << mirrPtr->BP_ID;
        removeAlienBodyParts(alienPtr, tempBP_ID_List_1);
    }

    qDebug() << "mirror deleted - reached 3";

    origBP_Ptr = replaceBP_WithAnotherBP(alienPtr, origBP_Ptr, tempSepBP);
    bpAndMirrPair.first = origBP_Ptr->BP_ID;

    qDebug() << "reached 4";

    GeometryAlgorithms::rotateBodyPartTowardsVecDir(*(origBP_Ptr), tempVec);
    GeometryAlgorithms::translateBP(origBP_Ptr,   tempPos.x() - origBP_Ptr->getPivotPoint().x(),
                                                            tempPos.y() - origBP_Ptr->getPivotPoint().y(),
                                                            tempPos.z() - origBP_Ptr->getPivotPoint().z());

    if (parentBP_Ptr)
    {
        if (!parentBP_Ptr->subBodyParts.contains(origBP_Ptr))
            parentBP_Ptr->subBodyParts.append(origBP_Ptr);
        updateBP_AttachmentToOtherBP_withAttachPositionAndVector_DNA(alienPtr, -1,
                                                                     parentBP_Ptr->BP_ID,
                                                                     origBP_Ptr->BP_ID, origBP_Ptr->getPivotPoint(),
                                                                     origBP_Ptr->getPivotVector());
    }

    mirrPtr = symmetrifyBP_AroundOppositeAttachmentFace(alienPtr, origBP_Ptr);
    if (mirrPtr)
    {
        if (parentBP_Ptr)
        {
            if (!parentBP_Ptr->subBodyParts.contains(mirrPtr))
                parentBP_Ptr->subBodyParts.append(mirrPtr);
            updateBP_AttachmentToOtherBP_withAttachPositionAndVector_DNA(alienPtr, -1,
                                                                         parentBP_Ptr->BP_ID,
                                                                         mirrPtr->BP_ID, mirrPtr->getPivotPoint(),
                                                                         mirrPtr->getPivotVector());
        }
        bpAndMirrPair.second = mirrPtr->BP_ID;
    }

    qDebug() << "Changes applied from seperate to the orig BP successfully";

    return bpAndMirrPair;
}

/*
 * +++++ ALGORITHM 1 -- cloneAlien_TypeGuided +++++
 *
 * Alien *newAlien = new Alien();
 * alienList.append(newAlien);
 * BP_ID_Counter_ForAllAliens.append(0);
 *
 * cloneAlienDNA(ID of alien cloned from)
 * --- cloneAlienSkeletonDNA()
 * --- clone JATs DS in AlienModeller
 * --- clone VecLocForEachJnt DS in AlienModeller
 *
 * cloneAlienSkeleton()
 * --- cloneSkeletalBP()
 * --- add all newly cloned BPs to their respective lists in clonedAlien
 * clone Jnt2JntRelations_TypeGuided DS
 *
 *
*/

//void AlienModeller::cloneAlien_TypeGuided(cloned from)
//{
//    // TODO(ismail): REFACTOR: need to have a pointer to the current alien
//    // TODO(ismail): Make a method for creating a new Alien that encapsulates the code below
//    Alien *newAlien = new Alien();
//    alienList.append(newAlien);
//    BP_ID_Counter_ForAllAliens.append(0);

//    cloneAlienDNA(ID of alien cloned from);
//    clone Jnt2Jnt relationships
//    clone JATs DS in AlienModeller
//    clone VecLocForEachJnt DS in AlienModeller // not required when cloning from DNA
//    modifyAlienDNA(); // so that the requested changes can take effect. This method should specify which BPs JATs and VecLocForEachJnt need to be computed
//    buildAlienSkeleton_TypeGuided  // do the necessary changes in JATs and VecLocForEachJnt DS

//    // The following call should be done within the buildAlienSkeleton_TypeGuided() method towards the end of that method
//    attachAllSkeletalBodyPartsToBackBone();

////    // Now we can bulk up the BPs according to our choice
////    QVector<int> BP_ID_List;
////    for (int i=0; i<alienList.last()->alienBackBones.length(); i++)
////    {
////        BP_ID_List.append(alienList.last()->alienBackBones[i]->BP_ID);
////    }
////    bulkUpBodyPart(BP_ID_List);

//    // Now we can smooth parts of the skeleton if needed AND We can also give different shapes to the skeleton
//}


void AlienModeller::initializeAllRulesets()
{
    initializeSkeletonRuleset();
}

void AlienModeller::initializeAlienSkeleton(Alien *alien)
{
    // ***** Random Selection of number of Joints and Bones ******

    // randomly select the number of joints
    int numJoints = (rand() % (skeletonRuleSet->skeletonSpecsBrackets[0].y()-skeletonRuleSet->skeletonSpecsBrackets[0].x())) + skeletonRuleSet->skeletonSpecsBrackets[0].x();

//    int numJoints = GeometryAlgorithms::getRandomInteger(
//                                                            skeletonRuleSet->skeletonSpecsBrackets[0].x(),
//                                                            skeletonRuleSet->skeletonSpecsBrackets[0].y()
//                                                        );

    qDebug() << "Joints: " << numJoints;

    // randomly select the number of bones using the number of joints in the previous step
    // number of bones to lie b/w number of joints selected in code above and thrice the joints
    int numBones = (rand() % ((5*numJoints)-numJoints)) + numJoints; // that number can be a controllable parameter

    qDebug() << "Bones: " << numBones;

    // ***** Generation of Joints and Bones i.e. populating the joints and bones lists for this alien ******

    // instantiate all the joints
    for (int i=0; i<numJoints; i++)
    {
        // TODO(ismail): perhaps make a method inside Alien to instantiate a new BP and call it from whereever I need to
        Joint *newJntPtr = instantiateNewJoint(alien);
    }

    // ***** Update the joint_ruleset and bone_ruleset ******
    // Here we also randomly specify how many bones will be allocated to each joint

    // populate the minMaxBonesPerJoint list in the Joint_RuleSet
    int tempNumBones = numBones;

    // TODO(ismail): FIX THIS. try a random value b/w 1 and 10
    int perc=3; // perc is used to calculate the percentage of bones allocated to each joint.
    bool bonesFinished = false;
    int bonesPerJoint=0;
    int runningSumBones=0;
    for (int i=0,j=perc; i<numJoints; i++,j++) // TODO(ismail): need to use type casting to convert between float and integers.
    {
        if (!bonesFinished)
        {
            // TODO(ismail): perhaps get rid of percentages completely and think of another logic only involving Integers
            int temp = (j*tempNumBones)/100;
            if ( tempNumBones > temp )
            {
                bonesPerJoint = temp;
                tempNumBones -= bonesPerJoint;
                if (bonesPerJoint == 0)
                {
                    bonesPerJoint = 1;
                }
            }
            else
            {
                bonesPerJoint = tempNumBones;
                bonesFinished = true;
            }
        }
        else
        {
            bonesPerJoint = 1;
        }

        if (i == numJoints-1  &&  runningSumBones != numBones)
        {
            bonesPerJoint = numBones - runningSumBones;
            if (bonesPerJoint < 0)
                bonesPerJoint = 0;
        }

        for (int k=0; k<bonesPerJoint; k++)
        {
            tempJntList_1.append(alien->alienJoints[i]->BP_ID);
        }
        jointRuleSet->minMaxBonesPerJoint.insert(i, QPoint(1,bonesPerJoint)); // i is the joint ID
        qDebug() << "max number of bones on joint with ID " << alien->alienJoints[i]->BP_ID << " is: " << bonesPerJoint;

        runningSumBones+=bonesPerJoint;

        tempJntList_2.append(alien->alienJoints[i]->BP_ID);
    }

    // Since tempJntList_1 is larger than tempJntList_2, we need to create new joints and also append their IDs to the tempJntList_2
    for (int i=tempJntList_2.length(); i<GeometryAlgorithms::getRandomInteger(tempJntList_2.length(), tempJntList_1.length()); i++)
    {
        Joint *newJntPtr = instantiateNewJoint(alien);
        tempJntList_2.append(newJntPtr->BP_ID);
    }

//    // Now instantiate all the bones
//    for (int i=0; i<tempJntList_1.length(); i++)
//    {
////        qDebug() << tempJntList_1[i];
//        Bone *bonePtr = new Bone();
//        bonePtr->BP_ID=alienList.last()->bodyParts.length(); // the id is incremented everytime we add a new BP
//        alienList.last()->bodyParts.append(bonePtr);
//        Bone *newBonePtr = bonePtr;
//        alienList.last()->alienBones.append(newBonePtr);
//    }

    // populate the minMaxBonesAllowedForAnyJoint list in the Bone_RuleSet. Not using it at the moment
    // TODO(ismail): FIX THIS.
    for (int i=0; i<alien->alienBones.length(); i++)
    {
        boneRuleSet->minMaxBonesAllowedForAnyJoint.insert(i, QPoint(1,2)); // TODO(ismail): perhaps come up with a better more clever max value
    }
}


// This method uses tempJntList_1 and tempJntList_2 to pair up joints and store their pairs in the Jnt2JntRelations DS
void AlienModeller::createSkeletonSpecs(Alien *alien)
{
//    QVector<int> isJntAChild(alienList.last()->alienJoints.length(), 1);

    for (int i=0; i<alien->alienJoints.length(); i++)
    {
        isJntAChild.insert(alien->alienJoints[i]->BP_ID,QPoint(1,0));
    }

    for (int i=0; i<tempJntList_1.length() && tempJntList_2.length()>0; i++)
    {
        // randomly select an index from tempJntList_1
        int randInd_tempJntList_1 = GeometryAlgorithms::getRandomInteger(0, tempJntList_1.length());

        // delete the entry in tempJntList_2 with the value as ith jnt
        if (tempJntList_2.indexOf(tempJntList_1[randInd_tempJntList_1]) != -1)
            tempJntList_2.remove(tempJntList_2.indexOf(tempJntList_1[randInd_tempJntList_1]));

        // pick up an index at random from tempJntList_2
        int randInd_tempJntList_2 = GeometryAlgorithms::getRandomInteger(0, tempJntList_2.length());

        // pair the jnt from tempJntList_1 with a random jnt from tempJntList_2
        // insert the pair in the Jnt2Jnt relations list
        alien->alienSkeleton.Jnt2JntRelations.append
                (QPoint(tempJntList_1[randInd_tempJntList_1],tempJntList_2[randInd_tempJntList_2]));

//        // DEBUG(ismail)
//        qDebug() << alienList.last()->alienSkeleton.Jnt2JntRelations.last().x()
//                 << " " << alienList.last()->alienSkeleton.Jnt2JntRelations.last().y();

        // mark this randomly selected jnt in parentChild list as a child
        isJntAChild[tempJntList_2[randInd_tempJntList_2]].setX(isJntAChild.value(tempJntList_1[randInd_tempJntList_1]).x() + 1);


        // finally delete the randomly selected jnt in tempJntList_1 and tempJntList_2 respectively
        tempJntList_1.remove(randInd_tempJntList_1);
        tempJntList_2.remove(randInd_tempJntList_2);
    }

    // insert a new joint at the beginning of the joints list
    // go through all the parents in the parentChild list and make these all children to a new parent joint
    // insert (prepend) all the these pairs in the Jnt2Jnt list in at the beginning of the list


    // create a temp list from just those joints that dont have any parent
    QVector<int> tempList;
    tempList.resize(0);
    for (int i=0; i<isJntAChild.size(); i++)
    {
        if (isJntAChild.value(alien->alienJoints[i]->BP_ID).x() == 1)
            tempList.append(alien->alienJoints[i]->BP_ID);
    }

    // pick up a joint from tempList at random and make it the parent of all those who didn't have a parent (selected in the code above)
    int tempRandRootJnt = GeometryAlgorithms::getRandomInteger(0, tempList.length());
    isJntAChild[tempList[tempRandRootJnt]].setX(0);
    // make the parent child relations and prepend it in Jnt2JntRelations
    for (int i=0; i<tempList.length(); i++)
    {
        if (i != tempRandRootJnt)
            alien->alienSkeleton.Jnt2JntRelations.prepend(QPoint(tempList[tempRandRootJnt],tempList[i]));
    }


    // DEBUG(ismail)
    for (int i=0; i<alien->alienSkeleton.Jnt2JntRelations.length(); i++)
    {
        qDebug() << alien->alienSkeleton.Jnt2JntRelations[i].x()
                 << " " << alien->alienSkeleton.Jnt2JntRelations[i].y();
    }

    // DEBUG(ismail)
    for (int i=0; i<isJntAChild.size(); i++)
    {
        qDebug() << alien->alienJoints[i]->BP_ID << " : " << isJntAChild.value(alien->alienJoints[i]->BP_ID).x();
    }
}


// Initialize joints arrangement trends for each joint for the currently modelled alien
// Joint Arrangement Trends mainly pertain to specifying the distance and angle at which a joint can be placed from its parent
// The specifications is defined inside the class JointsArrangementTrend
void AlienModeller::setUpJointArrangementTrend(Alien *alien, int randRootJnt) // TODO(ismail): perhaps need to get rid of the parameter
{
//    JointsArrangementTrend tempObj; // This object is for a single alien

    QVector<float> tempVec;
    QPolygonF tempPolygon;
    for (int i=0; i<alien->alienJoints.length(); i++)
    {
        tempPolygon.resize(0);
        tempVec.resize(0);

        tempVec.append(GeometryAlgorithms::getRandomFloat(100.0, 150.0)); // energy - this is multiplied with the unit vector
        tempVec.append(static_cast<float>(GeometryAlgorithms::getRandomInteger(0, 4))); // energy trend
        tempVec.append(GeometryAlgorithms::getRandomFloat(1.0, 360.0)); // angle of rotation of the vector
        tempVec.append(static_cast<float>(GeometryAlgorithms::getRandomInteger(0, 4))); // angle trend

        tempPolygon << QPointF(tempVec[0], tempVec[1]) << QPointF(tempVec[2], tempVec[3]);
//        qDebug() << tempVec[1] << " <> " << tempVec[3];

        // each joint's arrangement trend is stored in the list
        alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.VectorEnergyAngleTrends.insert(alien->alienJoints[i]->BP_ID, tempPolygon);
    }

//    jntsArrangeTrends.append(tempObj); // append this for the currently modelled alien. The list contains the trends for all the aliens
}

void AlienModeller::computeJointArrangements(Alien *alien)
{

    // pick up a central joint (at random) (** NOT USING RANDOM AT THE MOMENT **)
    // always select the first joint as the root joint
    int randRootJnt = alien->alienSkeleton.Jnt2JntRelations[0].x();

    setUpJointArrangementTrend(alien, randRootJnt);

    QMap<int,int> tempJntCompletionCheckList; // checklist for when the all the child joints's positions are computed
    QMap<int,int> tempJntPositionCheckList; // checklist for when the joint's position is computed
    for (int i=0; i<alien->alienJoints.length(); i++)
    {
        QPair<QVector3D, QVector3D> tempPair;
        tempPair.first = QVector3D(0.0,0.0,1.0); tempPair.second = QVector3D(0.0,0.0,0.0);
        tempVectorLocationCoupleForEachJoint.insert(alien->alienJoints[i]->BP_ID, tempPair);

        tempJntCompletionCheckList.insert(alien->alienJoints[i]->BP_ID,0);

        tempJntPositionCheckList.insert(alien->alienJoints[i]->BP_ID,0);
    }

    alien->alienJoints[randRootJnt]->BP_PivotPt = QVector3D(0.0,0.0,0.0);
    alien->alienJoints[randRootJnt]->makeCell(alien->alienJoints[randRootJnt]->BP_PivotPt, 5);
    tempJntPositionCheckList[randRootJnt] = 1;

    int boneIndex=0;

    for (int i=0; i<alien->alienSkeleton.Jnt2JntRelations.length(); i++)
    {
        bool isThisChildFirst=true;
        int tempJnt = alien->alienSkeleton.Jnt2JntRelations[i].x();
        if (tempJntCompletionCheckList.value(tempJnt) == 1)
            continue;
        for (int j=i; j<alien->alienSkeleton.Jnt2JntRelations.length(); j++)
        {
            int tempJnt2 = alien->alienSkeleton.Jnt2JntRelations[j].y();
            if (alien->alienSkeleton.Jnt2JntRelations[j].x() == tempJnt  &&  tempJntPositionCheckList.value(tempJnt2) == 0) // position not finalized yet..so go ahead?
            {
                QPolygonF tempPolygon = alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.VectorEnergyAngleTrends.value(tempJnt);
                QPolygonF tempPolygon2 = alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.VectorEnergyAngleTrends.value(tempJnt2);
                // compute the energy and angle for this joint using VectorEnergyAngleTrends DS inside JointsArrangementTrend

                if (tempJnt == randRootJnt)
                {
                    // specify the energy and angle trends for this root joint randomly. this changes for each child
                    // so that each child follows a new trend
                    tempPolygon[0].setY(static_cast<float>(GeometryAlgorithms::getRandomInteger(0, 4)));
                    tempPolygon[1].setY(static_cast<float>(GeometryAlgorithms::getRandomInteger(0, 4)));

                    alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.VectorEnergyAngleTrends.insert(tempJnt, tempPolygon);
                }

                //
                // ********* first get the energy sorted *********** // TODO(ismail): can use switch statement here
                //
                if (tempPolygon[0].y() == 0) // if the energy trend of its parent is ASCENDING
                {
                    // compute the energy greater than the parent's energy - randomly selecting between (energy of parent  AND  X times energy of parent)...Select X carefully. big will produce very large values
                    // TODO(ismail): VERY IMPORTANT for generating diverse shapes of aliens. Try different types of calls below.
                    //                                                                          1) remove GeometryAlgorithms::getRandomFloat(0.1,1.0) from first argument.
                    //                                                                          2) only use 2 * 2nd argument.
                    // These can be very useful in generating different shape trends of aliens. Let the user select a particular type of alien and then change these internally
//                    tempPolygon2[0].setX(GeometryAlgorithms::getRandomFloat(
//                                         tempPolygon[0].x(), GeometryAlgorithms::getRandomFloat(1.5,2.5) * tempPolygon[0].x()));

                    //++++ This is also an acceptable solution. Keeps the aliens not extend out too much
                    tempPolygon2[0].setX(GeometryAlgorithms::getRandomFloat(
                                         tempPolygon[0].x(), GeometryAlgorithms::getRandomInteger(50,150) + tempPolygon[0].x()));

                    tempPolygon2[0].setY(0);
                }
                else if (tempPolygon[0].y() == 1) // if the energy trend of its parent is DESCENDING
                {
                    tempPolygon2[0].setX(GeometryAlgorithms::getRandomFloat(0.5*tempPolygon[0].x(),tempPolygon[0].x()));
                    tempPolygon2[0].setY(1);
                }
                else if (tempPolygon[0].y() == 2) // if the energy trend of its parent is WAVY UP
                {
//                    tempPolygon2[0].setX(GeometryAlgorithms::getRandomFloat(
//                                         tempPolygon[0].x(), GeometryAlgorithms::getRandomFloat(1.5,2.5) * tempPolygon[0].x()));

                    //++++ This is also an acceptable solution. Keeps the aliens not extend out too much
                    tempPolygon2[0].setX(GeometryAlgorithms::getRandomFloat(
                                         tempPolygon[0].x(), GeometryAlgorithms::getRandomInteger(50,150) + tempPolygon[0].x()));

                    tempPolygon2[0].setY(3);
                }
                else if (tempPolygon[0].y() == 3) // if the energy trend of its parent is WAVY DOWN
                {
                    tempPolygon2[0].setX(GeometryAlgorithms::getRandomFloat(0.5*tempPolygon[0].x(),tempPolygon[0].x()));
                    tempPolygon2[0].setY(2);
                }

//                tempPolygon2[0].setX(tempPolygon[0].x());

                //
                // ********* Next get the angle sorted ***********
                //
                if (tempPolygon[1].y() == 0) // if the angle trend of its parent is ASCENDING
                {
                    // calculate an angle that is more than the previous angle.
                    // end result: rotate vector at an incremented angle from the previous vector along the pitch
                    tempPolygon2[1].setX(40.0); // can be randomly chosen between two values
                    tempPolygon2[1].setY(0);
                }
                else if (tempPolygon[1].y() == 1) // if the angle trend of its parent is DESCENDING
                {
                    tempPolygon2[1].setX(360.0-40.0);
                    tempPolygon2[1].setY(1);
                }
                else if (tempPolygon[1].y() == 2) // if the angle trend of its parent is WAVY UP
                {
                    tempPolygon2[1].setX(40.0); // make it a larger angle to achieve close curvings
                    tempPolygon2[1].setY(3);
                }
                else if (tempPolygon[1].y() == 3) // if the angle trend of its parent is WAVY DOWN
                {
                    tempPolygon2[1].setX(360.0-40.0);
                    tempPolygon2[1].setY(2);
                }

                // TODO(ismail): perhaps get rid of remove() as the updated Polygon replaces the old one in insert()
                if (alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.VectorEnergyAngleTrends.remove(tempJnt2) == 1)
                    alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.VectorEnergyAngleTrends.insert(tempJnt2, tempPolygon2);
                else
                {
                    qDebug() << "doesn't work";
                    exit(1);
                }

                qDebug() << "energy trend : " << tempPolygon2[0].y()    << "; new energy : " << tempPolygon2[0].x()
                         << " :: angle trend : " << tempPolygon2[1].y() << "; new angle : " << tempPolygon2[1].x();

                // Now compute the position of this joint using the energy and vector angle
                QQuaternion vecOrientQuat;

                //  if its parent joint is the root joint, use random vectors directions for computing its children joints
                //  compute a new random vector
                if (tempJnt == randRootJnt)
                {
                    // compute a vector that is oriented randomly in some direction using Euler angles
                    // later this can be made more aesthetic such as vectors close to each other
                    // ** Set this to (5.0,60) for creatures that look like aquatic creatures **
                    vecOrientQuat = QQuaternion::fromEulerAngles(GeometryAlgorithms::getRandomFloat(5.0,360.0),
                                                                 GeometryAlgorithms::getRandomFloat(5.0,360.0),
                                                                 GeometryAlgorithms::getRandomFloat(5.0,360.0));
                }
                else if (isThisChildFirst)
                {

                    // TODO(ismail): Inspect this. Is this working fine?
                    vecOrientQuat = QQuaternion::rotationTo(QVector3D(0.0,0.0,1.0), tempVectorLocationCoupleForEachJoint.value(tempJnt).first)
                                    * QQuaternion::fromEulerAngles(tempPolygon2[1].x(), 0.0, 0.0);

                }

                else
                {
                    // TODO(ismail): Inspect this. Is this working fine?
//                    vecOrientQuat = QQuaternion::fromEulerAngles((jntsArrangeTrends.last().VectorEnergyAngleTrends[tempJnt2].at(2)), 0.0, 0.0)
//                                    * QQuaternion::rotationTo(QVector3D(0.0,0.0,1.0), tempVectorLocationCoupleForEachJoint[tempJnt].at(0));

                    vecOrientQuat = QQuaternion::rotationTo(QVector3D(0.0,0.0,1.0), tempVectorLocationCoupleForEachJoint.value(tempJnt).first)
                                    * QQuaternion::fromEulerAngles(GeometryAlgorithms::getRandomFloat(5.0,60.0),
                                                                   GeometryAlgorithms::getRandomFloat(5.0,60.0),
                                                                   GeometryAlgorithms::getRandomFloat(5.0,60.0));


                    isJntAChild[(alienList.last()->alienJoints[i]->BP_ID)].setY(1);
//                    vecOrientQuat = QQuaternion::fromEulerAngles((jntsArrangeTrends.last().VectorEnergyAngleTrends[tempJnt2].at(2)), 0.0, 0.0);

                }


                QVector3D tempOrientVector = (vecOrientQuat.rotatedVector(QVector3D(0.0,0.0,1.0))).normalized();
//                QVector3D tempOrientVector = (vecOrientQuat.rotatedVector(tempVectorLocationCoupleForEachJoint[tempJnt].at(0))).normalized();

                // now compute the joint's location using the energy computed in the code above
                QVector3D newLoc =  tempVectorLocationCoupleForEachJoint.value(tempJnt).second
                                    + (tempPolygon2[0].x() * tempOrientVector);

                qDebug() << "joint " << tempJnt2 << " has location : " << newLoc;

                tempVectorLocationCoupleForEachJoint[tempJnt2].first = tempOrientVector;
                tempVectorLocationCoupleForEachJoint[tempJnt2].second = newLoc;

                // Now set this joint's location and create a cell for it
                for (int k=0; k<alienList.last()->alienJoints.length(); k++)
                {
                    if (alien->alienJoints[k]->BP_ID == tempJnt2)
                    {
//                        alienList.last()->alienJoints[k]->BP_PivotPt = newLoc; // set the joint's location
                        alien->alienJoints[k]->makeCell(newLoc, 5); // create the joint's cell
                        // rotate the joint towards the newly computed vector: tempOrientVector
                        //***   TODO(ismail): FIX THIS: This rotation is not working properly. It seems to be rotating around
                        //      the origin rather than spinning about own axis
                        GeometryAlgorithms::rotateCellTowardsVecDir(alien->alienJoints[k]->cellList[0], tempOrientVector);
//                        GeometryAlgorithms::scaleCell(
//                                    &alienList.last()->alienJoints[k]->cellList[0],
//                                    0.5,0.5,
//                                    10*tempOrientVector.length()
//                                    );

                        break;
                    }
                }

                // now specify that this child joint's position has been finalized
                tempJntPositionCheckList[tempJnt2] = 1;

                // This is just to store the parent and child joint's locations to be rendered as opengl lines
                QPair<QVector3D,QVector3D> vec3DPair;
                vec3DPair.first = tempVectorLocationCoupleForEachJoint.value(tempJnt).second;
                vec3DPair.second = tempVectorLocationCoupleForEachJoint.value(tempJnt).second
                        + (tempPolygon2[0].x() * tempOrientVector);
                alien->tempVectorsListForRendering.append(vec3DPair);
            }
            isThisChildFirst=false;
        }
        tempJntCompletionCheckList[tempJnt] = 1;
    }
}

BodyPart *AlienModeller::mirrorBP_AroundPlane(Alien *alienPtr, BodyPart *bpPtr, QVector3D v1, QVector3D v2, QVector3D v3, QVector3D norm)
{
    // Generate a clone of the BP
    BodyPart *clonedBpPtr1 = cloneSkeletalBP_FromAnotherAlien_viaDNA(alienPtr, alienPtr, bpPtr);
    clonedBpPtr1->isSymmetric = true;
//    alienPtr->alienDNA->skeletonDNA->BP_AngleArrangementsFromBackBone_DNA[clonedBpPtr1->BP_Type].removeLast();
    clonedBpPtr1->mirrorOf = bpPtr->BP_ID;

    // Translate and orient the BP at the same position and orientation as original BP
    GeometryAlgorithms::rotateBodyPartTowardsVecDir(*clonedBpPtr1, bpPtr->getPivotVector());
    GeometryAlgorithms::translateBP(clonedBpPtr1,
                                    bpPtr->getPivotPoint().x() - clonedBpPtr1->getPivotPoint().x(),
                                    bpPtr->getPivotPoint().y() - clonedBpPtr1->getPivotPoint().y(),
                                    bpPtr->getPivotPoint().z() - clonedBpPtr1->getPivotPoint().z());

    GeometryAlgorithms::reflectBodyPartAroundPlane(clonedBpPtr1, v1, v2, v3, norm);
    clonedBpPtr1->isSymmetric = true;

    // TODO: Make it more robust by finding the parent BP of the original BP for the following
    if (!alienPtr->alienBackBones[0]->subBodyParts.contains(clonedBpPtr1))
        alienPtr->alienBackBones[0]->subBodyParts.append(clonedBpPtr1);
    this->updateBP_AttachmentToOtherBP_withAttachPositionAndVector_DNA(alienPtr, -1, alienPtr->alienBackBones[0]->BP_ID,
                                                                       clonedBpPtr1->BP_ID,
                                                                       clonedBpPtr1->getPivotPoint(),
                                                                       clonedBpPtr1->getPivotVector());

    return clonedBpPtr1;
}

BodyPart *AlienModeller::symmetrifyBP_AroundOppositeAttachmentFace(Alien *alienPtr, BodyPart *bpPtr)
{
    // First Find its attachment face using a small algorithm that can find the box and its face where this
    // ... BP was attached
    // Then find a parallel plane passing through the center of the box
    // Clone the BP
    // Reflect the BP around this plane
    // Specify the attachment point to be pivot point of this newly reflected BP
    // Do the necessary ammendments in the attachment DNA Datastructures
    // Make this BP a subBP of the BB

    qDebug() << "\nAbout to symmetrify this BP around its attachment face";

    // First Find its attachment face using a small algorithm that can find the box and its face where this
    // ... BP was attached
    QVector3D plVer1, plVer2, plVer3, normal;
    QuadFace *foundFace=nullptr, *oppFace=nullptr;
    // TODO(ismail): Only get the bones of the BP this BP is attached to
    // TODO: make it more generic so it can work on BPs other than BackBone
    BodyPart *bbPtr = alienPtr->alienBackBones[0];
//    QVector<Bone*> boneList; boneList.clear();
    QVector<Joint*> jntList; jntList.clear();
    QVector<Bone*> bonesList; bonesList.clear();
//    getAllBP_Bones(bbPtr, boneList);
    getAllBP_Joints(bbPtr, jntList);
    getAllBP_BonesInOrder(alienPtr, bbPtr, bonesList);
    QVector<BodyPart*> bonesOrJntsList; bonesOrJntsList.clear();
    for (Joint *jnt : jntList)
        bonesOrJntsList.append(jnt);
    for (Bone *bone : bonesList)
        bonesOrJntsList.append(bone);
    // TODO(Ismail): Instead of the joints, go through all the bones of the BP and check which bones it is attached to
    // ... This way its joints will also be checked along the way
    for (BodyPart *bojPtr : bonesOrJntsList)
    {
        QVector<Cell *> tempCellList; tempCellList.clear();
        bojPtr->getBodyPartCellList(tempCellList);

        for (Cell *cellPtr : tempCellList)
        {
            // go through the faces of the box of this cell
            Box *boxPtr = cellPtr->cellBox;
            for (int i=0; i<boxPtr->faces.length(); i++)
            {
                QVector3D vtx1 = boxPtr->vertices[boxPtr->faces[i]->vertNorIndices[QuadFace::FaceVerticesNormalsIndicesEnum::BOTTOM_LEFT]];
                QVector3D vtx2 = boxPtr->vertices[boxPtr->faces[i]->vertNorIndices[QuadFace::FaceVerticesNormalsIndicesEnum::BOTTOM_RIGHT]];
                QVector3D vtx3 = boxPtr->vertices[boxPtr->faces[i]->vertNorIndices[QuadFace::FaceVerticesNormalsIndicesEnum::TOP_RIGHT]];

                // check if the BP's pivot point lies on this face or is too close to the plane (dist <= 0.01)
                qDebug() << "BP's pivot-point distance to plane : " << fabs(bpPtr->getPivotPoint().distanceToPlane(vtx1, vtx2, vtx3));
                if (fabs(bpPtr->getPivotPoint().distanceToPlane(vtx1, vtx2, vtx3)) <= 0.0001)
                {
                    // Found the box so lets get the plane passing through the center of the box
                    // TODO(ismail): write an algorithm that can compute the opposite face by utilizing
                    //               ... line-plane intersection algorithm
                    foundFace = boxPtr->faces[i];

                    if (i == Box::BoxFace::LEFT)
                        oppFace = boxPtr->faces[Box::BoxFace::RIGHT];
                    else if (i == Box::BoxFace::RIGHT)
                        oppFace = boxPtr->faces[Box::BoxFace::LEFT];
                    else if (i == Box::BoxFace::FRONT)
                        oppFace = boxPtr->faces[Box::BoxFace::BACK];
                    else if (i == Box::BoxFace::BACK)
                        oppFace = boxPtr->faces[Box::BoxFace::FRONT];
                    else if (i == Box::BoxFace::TOP)
                        oppFace = boxPtr->faces[Box::BoxFace::BOTTOM];
                    else if (i == Box::BoxFace::BOTTOM)
                        oppFace = boxPtr->faces[Box::BoxFace::TOP];

                    qDebug() << "face found";

                    break; // Don't check for any more faces
                }
            }
            if (foundFace)
            {
                // TODO(ismail): make a method that gets the middle plane between two paralle1 planes
                plVer1 = boxPtr->vertices[foundFace->vertNorIndices[QuadFace::FaceVerticesNormalsIndicesEnum::BOTTOM_LEFT]] +
                        (0.5 * (boxPtr->vertices[oppFace->vertNorIndices[QuadFace::FaceVerticesNormalsIndicesEnum::BOTTOM_RIGHT]]
                        - boxPtr->vertices[foundFace->vertNorIndices[QuadFace::FaceVerticesNormalsIndicesEnum::BOTTOM_LEFT]]));

                plVer2 = boxPtr->vertices[foundFace->vertNorIndices[QuadFace::FaceVerticesNormalsIndicesEnum::BOTTOM_RIGHT]] +
                        (0.5 * (boxPtr->vertices[oppFace->vertNorIndices[QuadFace::FaceVerticesNormalsIndicesEnum::BOTTOM_LEFT]]
                        - boxPtr->vertices[foundFace->vertNorIndices[QuadFace::FaceVerticesNormalsIndicesEnum::BOTTOM_RIGHT]]));

                plVer3 = boxPtr->vertices[foundFace->vertNorIndices[QuadFace::FaceVerticesNormalsIndicesEnum::TOP_RIGHT]] +
                        (0.5 * (boxPtr->vertices[oppFace->vertNorIndices[QuadFace::FaceVerticesNormalsIndicesEnum::TOP_LEFT]]
                        - boxPtr->vertices[foundFace->vertNorIndices[QuadFace::FaceVerticesNormalsIndicesEnum::TOP_RIGHT]]));

                normal = boxPtr->normals[foundFace->vertNorIndices[QuadFace::FaceVerticesNormalsIndicesEnum::NORMAL]];

//                qDebug() << "found face vertices are : " << plVer1 << plVer2 << plVer3;

                break;
            }
        }
        if (foundFace)
            break;
        else
        {
            qDebug() << "face not found";
        }
    }

    // Now we got the plane's 3 vertices around which we need to reflect, lets reflect the BP around it
    BodyPart *mirroredBP_Ptr = mirrorBP_AroundPlane(alienPtr, bpPtr, plVer1, plVer2, plVer3, normal);

    qDebug() << "Symmetrifications done\n";

    return mirroredBP_Ptr;
}

void AlienModeller::symmetrifyBP_AroundOppositeAttachmentFace_Algo2(Alien *alienPtr, BodyPart *bpPtr)
{
    // First Find its attachment face using a small algorithm that can find the box and its face where this
    // ... BP was attached
    // Then find a parallel plane passing through the center of the box
    // Also find a point on the opposite face that is directly opposite the the bpPtr.pivotPoint
    // Clone the BP
    // Reflect the BP around this plane
    // Specify the attachment point to be pivot point of this newly reflected BP
    // Do the necessary ammendments in the attachment DNA Datastructures
    // Make this BP a subBP of the BB

    qDebug() << "\nAbout to symmetrify this BP around its attachment face";

    // First Find its attachment face using a small algorithm that can find the box and its face where this
    // ... BP was attached
    QVector3D plVer1, plVer2, plVer3, attachPnt, normal, normal2;
    QuadFace *foundFace=nullptr, *oppFace=nullptr;
    // TODO(ismail): Only get the bones of the BP this BP is attached to
    BodyPart *bbPtr = alienPtr->alienBackBones[0];
//    QVector<Bone*> boneList; boneList.clear();
    QVector<Joint*> jntList; jntList.clear();
//    getAllBP_Bones(bbPtr, boneList);
    getAllBP_Joints(bbPtr, jntList);
    // TODO(Ismail): Instead of the joints, go through all the bones of the BP and check which bones it is attached to
    // ... This way its joints will also be checked along the way
    for (Joint *jntPtr : jntList)
    {
        QVector<Cell *> tempCellList; tempCellList.clear();
        jntPtr->getBodyPartCellList(tempCellList);

        for (Cell *cellPtr : tempCellList)
        {
            // go through the faces of the box of this cell
            Box *boxPtr = cellPtr->cellBox;
            for (int i=0; i<boxPtr->faces.length(); i++)
            {
                QVector3D vtx1 = boxPtr->vertices[boxPtr->faces[i]->vertNorIndices[QuadFace::FaceVerticesNormalsIndicesEnum::BOTTOM_LEFT]];
                QVector3D vtx2 = boxPtr->vertices[boxPtr->faces[i]->vertNorIndices[QuadFace::FaceVerticesNormalsIndicesEnum::BOTTOM_RIGHT]];
                QVector3D vtx3 = boxPtr->vertices[boxPtr->faces[i]->vertNorIndices[QuadFace::FaceVerticesNormalsIndicesEnum::TOP_RIGHT]];

                // check if the BP's pivot point lies on this face or is too close to the plane (dist <= 0.01)
                qDebug() << "BP's pivot-point distance to plane : " << fabs(bpPtr->getPivotPoint().distanceToPlane(vtx1, vtx2, vtx3));
                if (fabs(bpPtr->getPivotPoint().distanceToPlane(vtx1, vtx2, vtx3)) <= 0.0001)
                {
                    // Found the box so lets get the plane passing through the center of the box
                    // TODO(ismail): write an algorithm that can compute the opposite face by utilizing
                    //               ... line-plane intersection algorithm
                    foundFace = boxPtr->faces[i];

                    if (i == Box::BoxFace::LEFT)
                        oppFace = boxPtr->faces[Box::BoxFace::RIGHT];
                    else if (i == Box::BoxFace::RIGHT)
                        oppFace = boxPtr->faces[Box::BoxFace::LEFT];
                    else if (i == Box::BoxFace::FRONT)
                        oppFace = boxPtr->faces[Box::BoxFace::BACK];
                    else if (i == Box::BoxFace::BACK)
                        oppFace = boxPtr->faces[Box::BoxFace::FRONT];
                    else if (i == Box::BoxFace::TOP)
                        oppFace = boxPtr->faces[Box::BoxFace::BOTTOM];
                    else if (i == Box::BoxFace::BOTTOM)
                        oppFace = boxPtr->faces[Box::BoxFace::TOP];

                    qDebug() << "face found";

                    break; // Don't check for any more faces
                }
            }
            if (foundFace)
            {
                // TODO(ismail): make a method that gets the middle plane between two paralle1 planes
                plVer1 = boxPtr->vertices[foundFace->vertNorIndices[QuadFace::FaceVerticesNormalsIndicesEnum::BOTTOM_LEFT]] +
                        (0.5 * (boxPtr->vertices[oppFace->vertNorIndices[QuadFace::FaceVerticesNormalsIndicesEnum::BOTTOM_RIGHT]]
                        - boxPtr->vertices[foundFace->vertNorIndices[QuadFace::FaceVerticesNormalsIndicesEnum::BOTTOM_LEFT]]));

                plVer2 = boxPtr->vertices[foundFace->vertNorIndices[QuadFace::FaceVerticesNormalsIndicesEnum::BOTTOM_RIGHT]] +
                        (0.5 * (boxPtr->vertices[oppFace->vertNorIndices[QuadFace::FaceVerticesNormalsIndicesEnum::BOTTOM_LEFT]]
                        - boxPtr->vertices[foundFace->vertNorIndices[QuadFace::FaceVerticesNormalsIndicesEnum::BOTTOM_RIGHT]]));

                plVer3 = boxPtr->vertices[foundFace->vertNorIndices[QuadFace::FaceVerticesNormalsIndicesEnum::TOP_RIGHT]] +
                        (0.5 * (boxPtr->vertices[oppFace->vertNorIndices[QuadFace::FaceVerticesNormalsIndicesEnum::TOP_LEFT]]
                        - boxPtr->vertices[foundFace->vertNorIndices[QuadFace::FaceVerticesNormalsIndicesEnum::TOP_RIGHT]]));

                attachPnt = bpPtr->getPivotPoint() +
                            (1.0 * (boxPtr->vertices[oppFace->vertNorIndices[QuadFace::FaceVerticesNormalsIndicesEnum::TOP_LEFT]]
                            - boxPtr->vertices[foundFace->vertNorIndices[QuadFace::FaceVerticesNormalsIndicesEnum::TOP_RIGHT]]));


                normal = boxPtr->normals[foundFace->vertNorIndices[QuadFace::FaceVerticesNormalsIndicesEnum::NORMAL]];
                normal2 = boxPtr->normals[oppFace->vertNorIndices[QuadFace::FaceVerticesNormalsIndicesEnum::NORMAL]];

//                qDebug() << "found face vertices are : " << plVer1 << plVer2 << plVer3;

                break;
            }
        }
        if (foundFace)
            break;
        else
        {
            qDebug() << "face not found";
        }
    }

    BodyPart *clonedBpPtr1 = cloneSkeletalBP_FromAnotherAlien_viaDNA(alienPtr, alienPtr, bpPtr, 1, true);
    QVector3D tempVec = bpPtr->getPivotVector();

    GeometryAlgorithms::reflectVertexAroundPlane(tempVec, plVer1, plVer2, plVer3, normal);
//    GeometryAlgorithms::rotateBodyPartTowardsVecDir(*bpPtr, normal);

    GeometryAlgorithms::rotateBodyPartTowardsVecDir(*clonedBpPtr1, tempVec);
//    GeometryAlgorithms::rotateBodyPartUsingEulerAngles(*clonedBpPtr1, 0, 0, 180);

    GeometryAlgorithms::translateBP(clonedBpPtr1,
                                   attachPnt.x() - clonedBpPtr1->getPivotPoint().x(),
                                    attachPnt.y() - clonedBpPtr1->getPivotPoint().y(),
                                    attachPnt.z() - clonedBpPtr1->getPivotPoint().z());

    alienPtr->alienBackBones[0]->subBodyParts.append(clonedBpPtr1);

    qDebug() << "Symmetrifications done\n";

}

void AlienModeller::symmetrifyAlienBodyParts(Alien *alienPtr)
{
    QVector<BodyPart*> tempBP_List; tempBP_List.clear();
    for (BodyPart *bpPtr : alienPtr->bodyParts)
    {
        if (bpPtr->BP_Type == AlienModeller::BodyPartsEnum::LOWER_LIMB  ||  bpPtr->BP_Type == AlienModeller::BodyPartsEnum::UPPER_LIMB)
        {
            tempBP_List.append(bpPtr);
        }
    }

    for (BodyPart *bpPtr2 : tempBP_List)
    {
        if (!bpPtr2->isSymmetric) // dont mirror the BPs that are already mirrors of other BPs
            BodyPart *tempBP_Ptr = symmetrifyBP_AroundOppositeAttachmentFace(alienPtr, bpPtr2);
//        symmetrifyBP_AroundOppositeAttachmentFace_Algo2(alienPtr, bpPtr2);
    }
}


// This method makes the skeleton more organic
// Main Steps involved are:
//  1 - Extend all the leaf joints and store these as lineages
//  2 - Smooth the lineages
// Idea: skeletonSmoothFactor can be a percentage of the leaf joints to be selected for further smoothing
void AlienModeller::smoothAlienSkeleton(Alien *alien, int skeletonSmoothFactor)
{
    // ++++++ ALGO 1
    // Data structures needed: Jnt2JntRelations, isJntAChild, jointArrangementTrends, tempVectorLocationCoupleForEachJoint,
    // find all the leaf joints
    // randomly decide whether to extend the lineages of some or all of the leaf joints in 'leafJnts' list
        // if yes:
            //  keep adding more decendents to this joint in the Jnt2JntRelations list (based on a random number of desendents)

    // For all of these joints find their ancestors whose child level is random b/w this joint and 0
    // store these newly found joints' lineage from JointToJointRelations list in temporary Jnt2JntRelations lists
    // If lineage was not extended, store the vectors and energies for each of these joints
    // Keep hold of the energy and angle trend for these vectors (Not sure if these are needed)
    // remove these joints pairs from the Joint2Joint list
    // remove all these joints from the alienJoints list and add the new ones
    // compute new joints' locations using either your own smoothness algorithm or Denis Zorin's algorithm
    // now update the Joint2Joint list with the new arrangements/relationships


    // find all the leaf joints and their lineages back to their ancestors
    QVector<QVector<int>> jntLineageList; // list that stores all the lineages
    QVector<QPoint> leafJnts; // x = joint ID, y = amount of further joints to be added in the lineage
    bool isLeafJnt=true;
    for (int i=0; i<alien->alienSkeleton.Jnt2JntRelations.length(); i++)
    {
        QVector<int> tempLineageList;
        tempLineageList.resize(0);
        int secondJnt = alien->alienSkeleton.Jnt2JntRelations[i].y();
        isLeafJnt=true;
        for (int j=0; j<alien->alienSkeleton.Jnt2JntRelations.length(); j++)
        {
            if (secondJnt == alien->alienSkeleton.Jnt2JntRelations[j].x())
            {
                isLeafJnt=false;
                break;
            }
        }
        if (isLeafJnt) // this is a leaf joint so find all its ancenstors and store them in lineage list
        {
            qDebug() << "leaf Joint found";
            leafJnts.append(QPoint(secondJnt,GeometryAlgorithms::getRandomInteger(-7,7)));
            tempLineageList.append(secondJnt);

            if (isJntAChild.value(secondJnt).y() != 1) // i.e. this is not a joint that was formed at a random direction from tis parent
            {
                // Now for this leaf joint, find its ancestor whose child level is random b/w this joint's child level and 0
                // ... (This will become more realistic and interesting)
                int firstJnt = alien->alienSkeleton.Jnt2JntRelations[i].x();
                tempLineageList.prepend(firstJnt);
                for (int j=0; j<alien->alienSkeleton.Jnt2JntRelations.length(); j++)
                {
                    if (isJntAChild.value(firstJnt).y() == 1 || isJntAChild.value(firstJnt).x() == 0)
                        break;

                    else if (firstJnt == alien->alienSkeleton.Jnt2JntRelations[j].y())
                    {
                        firstJnt = alien->alienSkeleton.Jnt2JntRelations[j].x();
                        tempLineageList.prepend(firstJnt);
                        j=-1; // reset j so that the for loop starts from the beginning
                    }
                }
            }
            if (tempLineageList.length() > 1)   // i.e. if there is only one joint in this lineage list,
                                                //  then no need to consider it for smoothing
                jntLineageList.append(tempLineageList);
            else
                leafJnts.removeLast();
            float perc1 = (jntLineageList.length()*100.0)/alien->alienJoints.length();
            if (perc1 >= (skeletonSmoothFactor*10))    // the number of leaf joints to be
                                                                    //  considered must not exceed the smoothness factor
                break;
        }
    }

    // Now extend the lineage based on the random number selected in leafJnts[i].y()
    // ... (some lineages will be extended while some will not. Those with value less than 1 will not be extended)
    // Extending the lineages mean appending fresh new joints to each lineage

    qDebug() << "Extending lineages";
    for (int i=0; i<leafJnts.length(); i++)
    {
        int tempLeafJntID = leafJnts[i].x();
        if (tempLeafJntID == jntLineageList[i].last())
        {
            for (int j=0; j<leafJnts[i].y(); j++)
            {
                Joint *newJntPtr = instantiateNewJoint(alien);
                alien->alienSkeleton.Jnt2JntRelations.append(QPoint(tempLeafJntID,newJntPtr->BP_ID));
                jntLineageList[i].append(newJntPtr->BP_ID);

                // As soon a new joint is added, its position, JointsArrangementTrend and tempVectorLocationCoupleForEachJoint
                // ... are computed
                QPolygonF tempPolygon = alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.VectorEnergyAngleTrends.value(tempLeafJntID);
                QPolygonF tempPolygon2;
                // set default values
                tempPolygon2.resize(0);
                tempPolygon2 << QPointF(1, 0) << QPointF(40, 0);

                if (tempPolygon[0].y() == 0) // if the energy trend of its parent is ASCENDING
                {
                    // compute the energy greater than the parent's energy - randomly selecting between (energy of parent  AND  X times energy of parent)...Select X carefully. big will produce very large values
                    // TODO(ismail): VERY IMPORTANT for generating diverse shapes of aliens. Try different types of calls below.
                    //                                                                          1) remove GeometryAlgorithms::getRandomFloat(0.1,1.0) from first argument.
                    //                                                                          2) only use 2 * 2nd argument.
                    // These can be very useful in generating different shape trends of aliens. Let the user select a particular type of alien and then change these internally
//                    tempPolygon2[0].setX(GeometryAlgorithms::getRandomFloat(
//                                         tempPolygon[0].x(), GeometryAlgorithms::getRandomFloat(1.5,2.5) * tempPolygon[0].x()));

                    //++++ This is also an acceptable solution. Keeps the aliens not extend out too much
                    tempPolygon2[0].setX(GeometryAlgorithms::getRandomFloat(
                                         tempPolygon[0].x(), GeometryAlgorithms::getRandomInteger(50,150) + tempPolygon[0].x()));

                    tempPolygon2[0].setY(0);
                }
                else if (tempPolygon[0].y() == 1) // if the energy trend of its parent is DESCENDING
                {
                    tempPolygon2[0].setX(GeometryAlgorithms::getRandomFloat(0.5*tempPolygon[0].x(),tempPolygon[0].x()));
                    tempPolygon2[0].setY(1);
                }
                else if (tempPolygon[0].y() == 2) // if the energy trend of its parent is WAVY UP
                {
//                    tempPolygon2[0].setX(GeometryAlgorithms::getRandomFloat(
//                                         tempPolygon[0].x(), GeometryAlgorithms::getRandomFloat(1.5,2.5) * tempPolygon[0].x()));

                    //++++ This is also an acceptable solution. Keeps the aliens not extend out too much
                    tempPolygon2[0].setX(GeometryAlgorithms::getRandomFloat(
                                         tempPolygon[0].x(), GeometryAlgorithms::getRandomInteger(50,150) + tempPolygon[0].x()));

                    tempPolygon2[0].setY(3);
                }
                else if (tempPolygon[0].y() == 3) // if the energy trend of its parent is WAVY DOWN
                {
                    tempPolygon2[0].setX(GeometryAlgorithms::getRandomFloat(0.5*tempPolygon[0].x(),tempPolygon[0].x()));
                    tempPolygon2[0].setY(2);
                }

//                tempPolygon2[0].setX(tempPolygon[0].x());

                //
                // ********* Next get the angle sorted ***********
                //
                if (tempPolygon[1].y() == 0) // if the angle trend of its parent is ASCENDING
                {
                    // calculate an angle that is more than the previous angle.
                    // end result: rotate vector at an incremented angle from the previous vector along the pitch
                    tempPolygon2[1].setX(40.0); // can be randomly chosen between two values
                    tempPolygon2[1].setY(0);
                }
                else if (tempPolygon[1].y() == 1) // if the angle trend of its parent is DESCENDING
                {
                    tempPolygon2[1].setX(360.0-40.0);
                    tempPolygon2[1].setY(1);
                }
                else if (tempPolygon[1].y() == 2) // if the angle trend of its parent is WAVY UP
                {
                    tempPolygon2[1].setX(40.0); // make it a larger angle (140 or so) to achieve close curvings
                    tempPolygon2[1].setY(3);
                }
                else if (tempPolygon[1].y() == 3) // if the angle trend of its parent is WAVY DOWN
                {
                    tempPolygon2[1].setX(360.0-40.0);
                    tempPolygon2[1].setY(2);
                }

                // Now store all the energy and angle trends for this joint
                alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.VectorEnergyAngleTrends.insert(newJntPtr->BP_ID, tempPolygon2);

                qDebug() << "energy trend : " << tempPolygon2[0].y()    << "; new energy : " << tempPolygon2[0].x()
                         << " :: angle trend : " << tempPolygon2[1].y() << "; new angle : " << tempPolygon2[1].x();

                // Now compute the position of this joint using the energy and vector angle
                // TODO(ismail): Inspect this. Is this working fine?
                QQuaternion vecOrientQuat = QQuaternion::rotationTo(QVector3D(0.0,0.0,1.0), tempVectorLocationCoupleForEachJoint.value(tempLeafJntID).first)
                                * QQuaternion::fromEulerAngles(tempPolygon2[1].x(), 0.0, 0.0);


                QVector3D tempOrientVector = (vecOrientQuat.rotatedVector(QVector3D(0.0,0.0,1.0))).normalized();
//                QVector3D tempOrientVector = (vecOrientQuat.rotatedVector(tempVectorLocationCoupleForEachJoint[tempJnt].at(0))).normalized();

                // now compute the joint's location using the energy computed in the code above
                QVector3D newLoc =  tempVectorLocationCoupleForEachJoint.value(tempLeafJntID).second
                                    + (tempPolygon2[0].x() * tempOrientVector);

                qDebug() << "joint " << newJntPtr->BP_ID << " has location : " << newLoc;

                QPair<QVector3D, QVector3D> tempPair;
                tempPair.first = tempOrientVector; tempPair.second = newLoc;
                tempVectorLocationCoupleForEachJoint.insert(newJntPtr->BP_ID, tempPair);

                // Now set this joint's location and create a cell for it
                newJntPtr->makeCell(newLoc, 2); // create the joint's cell
                // rotate the joint towards the newly computed vector: tempOrientVector
                GeometryAlgorithms::rotateCellTowardsVecDir(newJntPtr->cellList[0], tempOrientVector);
//                        GeometryAlgorithms::scaleCell(
//                                    &alienList.last()->alienJoints[k]->cellList[0],
//                                    0.5,0.5,
//                                    10*tempOrientVector.length()
//                                    );


                tempLeafJntID = newJntPtr->BP_ID;
            }
        }
        else
        {
            qDebug() << "There's a problem";
            exit(1);
        }
    }

    // ++ SMOOTHING ALGORITHM 2 -- Now subdivide the lineages using your own algorithm. ++
    QVector<QVector<int>> jntLineageList_smoothed_prev;
    QVector<QVector<int>> jntLineageList_smoothed;
    for (int smoothness=0; smoothness<2; smoothness++)
    {
        if (smoothness == 0)
        {
            for (int i=0; i<jntLineageList.length(); i++)
            {
                QVector<int> tempSmoothList;
                tempSmoothList.resize(0);
                for (int j=0; j<jntLineageList[i].length(); j++)
                {
                    tempSmoothList.append(jntLineageList[i][j]);
                }
                jntLineageList_smoothed_prev.append(tempSmoothList);
            }
        }
        else
        {
            for (int i=0; i<jntLineageList_smoothed_prev.length(); i++)
            {
                for (int j=1; j<jntLineageList_smoothed_prev[i].length()-1; j++)
                {
                    int tempJntID = jntLineageList_smoothed_prev[i][j];
                    for (int k=0; k<alien->alienJoints.length(); k++)
                    {
                        if (tempJntID == alien->alienJoints[k]->BP_ID)
                        {
                            // TODO(ismail):    Define a method that with one call to it the system will remove BodyPart, we remove its object and pointer from all Data structures,
                            //                  ... such as BodyParts, Joints / Bones etc
                            alien->alienJoints.remove(k);
                            qDebug() << "Legacy Joint removed while smoothing"; // TODO(ismail): This needs a proper destructor as a complete object is being deleted
                            break;
                        }
                    }
                    for (int k=0; k<alien->bodyParts.length(); k++)
                    {
                        if (tempJntID == alien->bodyParts[k]->BP_ID)
                        {
                            alien->bodyParts.remove(k); // TODO(ismail): This needs a proper destructor as a complete object is being deleted
                            qDebug() << "Legacy Body Part removed while smoothing";
                            break;
                        }
                    }
                }
                jntLineageList_smoothed_prev.resize(0);
            }
            jntLineageList_smoothed_prev.resize(0);

            for (int i=0; i<jntLineageList_smoothed.length(); i++)
            {
                QVector<int> tempSmoothList;
                tempSmoothList.resize(0);
                for (int j=0; j<jntLineageList_smoothed[i].length(); j++)
                {
                    tempSmoothList.append(jntLineageList_smoothed[i][j]);
                }
                jntLineageList_smoothed_prev.append(tempSmoothList);
            }

            for (int i=0; i<jntLineageList_smoothed.length(); i++)
            {
                jntLineageList_smoothed.resize(0);
            }
            jntLineageList_smoothed.resize(0);
        }

        for (int i=0; i<jntLineageList_smoothed_prev.length(); i++)
        {
            // Create a new list like jntLineageList. Call it jntLineageList_smoothed. Store the joint ID at jntLineageList[i][0] as the first element in each inner list.
            QVector<int> tempSmoothedList;
            tempSmoothedList.resize(0);
            tempSmoothedList.append(jntLineageList_smoothed_prev[i][0]);
            int tempJntId_1, tempJntId_2, tempJntId_3; // IDs of the joints taken from the jntLineageList_smoothed_prev
            tempJntId_1 = jntLineageList_smoothed_prev[i][0];
            for (int j=0; j<jntLineageList_smoothed_prev[i].length()-2; j++)
            {
    //            // select a random number for the amount of intermediate points to be computed
    //            int randNumInterPoints = GeometryAlgorithms::getRandomInteger(6,7);
    //            qDebug() << "number of subdivisions: " << randNumInterPoints;

                // Get the locations of the 3 adjacent joints from jntLineageList_smoothed_prev
                // ... i.e. jntLineageList_smoothed_prev[i][j], jntLineageList_smoothed_prev[i][j+1], and jntLineageList_smoothed_prev[i][j+2]
                QVector3D jnt1_Loc, jnt2_Loc, jnt3_Loc;
                for (int jntIter=0; jntIter<alien->alienJoints.length(); jntIter++)
                {
                    int tempJnt_ID = alien->alienJoints[jntIter]->BP_ID;
                    if (jntLineageList_smoothed_prev[i][j] == tempJnt_ID)
                        jnt1_Loc = alien->alienJoints[jntIter]->getPivotPoint();
                    else if (jntLineageList_smoothed_prev[i][j+1] == tempJnt_ID)
                        jnt2_Loc = alien->alienJoints[jntIter]->getPivotPoint();
                    else if (jntLineageList_smoothed_prev[i][j+2] == tempJnt_ID)
                        jnt3_Loc = alien->alienJoints[jntIter]->getPivotPoint();
                }

                // find a new Joint at a point at 75% of the first and second joint. Call it joint_1
                QVector3D newTempIntJnt_1 = jnt1_Loc + (0.75*(jnt2_Loc-jnt1_Loc));
                // create a joint for it and append it to the alienJoints list.
                Joint *newJntPtr = instantiateNewJoint(alien);
                // create a cell for this joint
                newJntPtr->makeCell(newTempIntJnt_1, 1); // create the joint's cell
                tempJntId_2 = newJntPtr->BP_ID;
                // append this joint's IDs in tempSmoothedList.
                tempSmoothedList.append(tempJntId_2);
                // update the Jnt2JntRelations DS
                alien->alienSkeleton.Jnt2JntRelations.append(QPoint(tempJntId_1,tempJntId_2));

                // find a new Joint at a point at 100-75% of the second and third joint. Call it joint_2
                QVector3D newTempIntJnt_2 = jnt2_Loc + (0.25*(jnt3_Loc-jnt2_Loc));
                // create a joint for it and append it to the alienJoints list.
                newJntPtr = instantiateNewJoint(alien);
                // create a cell for this joint
                newJntPtr->makeCell(newTempIntJnt_2, 1); // create the joint's cell
                tempJntId_3 = newJntPtr->BP_ID;
                // append this joint's IDs in tempSmoothedList.
                tempSmoothedList.append(tempJntId_3);
                // update the Jnt2JntRelations DS
                alien->alienSkeleton.Jnt2JntRelations.append(QPoint(tempJntId_2,tempJntId_3));

                tempJntId_1 = tempJntId_3;

                // Remove the previous parent-child relationship from the Jnt2JntRelations DS
                for (int k=0; k<alien->alienSkeleton.Jnt2JntRelations.length(); k++)
                {
                    int firstJntID = alien->alienSkeleton.Jnt2JntRelations[k].x();
                    int secondJntID = alien->alienSkeleton.Jnt2JntRelations[k].y();
                    if (jntLineageList_smoothed_prev[i][j] == firstJntID  &&  jntLineageList_smoothed_prev[i][j+1] == secondJntID)
                    {
                        alien->alienSkeleton.Jnt2JntRelations.remove(k);
                        qDebug() << "Jnt2Jnt Pair removed while smoothing";
                        break;
                    }
                }
            }
            tempSmoothedList.append(jntLineageList_smoothed_prev[i][jntLineageList_smoothed_prev[i].length()-1]);
            alien->alienSkeleton.Jnt2JntRelations.append(QPoint(tempJntId_3,jntLineageList_smoothed_prev[i][jntLineageList_smoothed_prev[i].length()-1]));
            jntLineageList_smoothed.append(tempSmoothedList);
        }

        // Now remove all the joints in jntLineageList_smoothed_prev from alienList.last()->alienJoints and alienList.last()->bodyParts
        // ... except for the first and last joint in each sub list
    }
}

 BodyPart *AlienModeller::smoothenSkeletalBP(Alien *alien, BodyPart *BP_Ptr, quint8 smoothnessAmnt)
{
    if (smoothnessAmnt > 0)
    {
        qDebug() << "Commencing smoothing of BP";

        // if (BP_Ptr->isSymmetric) // If this is a mirror, get its original BP
        //     BP_Ptr = getMirrorOrOriginalBP(alien, BP_Ptr);

        QPolygon *Jnt2JntRelExtract = new QPolygon(*(alien->alienSkeleton.Jnt2JntRelations_TypeGuided.value(BP_Ptr->BP_ID)));
        // First create a clone of the unsmoothed BP. This clone acts as a backup of the original BP
        // This can be useful for several purposes such as recomputing JATs, and cloning an entire Alien
        if (BP_Ptr->smoothnessLevel == 0  &&  !BP_Ptr->isSymmetric)
            createSmoothingBackupCloneBP(alien, BP_Ptr);

        if (BP_Ptr->smoothnessLevel < 2)
        {
            QVector<Joint*> tempJointsList; tempJointsList.clear();
            QVector<Joint*> tempJointsListToRemove; tempJointsListToRemove.clear();
            QVector<int> jntLineageList; jntLineageList.clear();
            convertJnt2JntPolyToJointsLineageList(Jnt2JntRelExtract, jntLineageList);
            qDebug() << jntLineageList;

            if (jntLineageList.length() > 2)
            {
                // We need to remove this BP from its parent's subBPs list and then at the end re-assign this BP as a suBP of its parent
                BodyPart *parentBP_Ptr = getParentOfThisBP(alien, BP_Ptr);
                if (parentBP_Ptr)
                    parentBP_Ptr->subBodyParts.removeOne(BP_Ptr);

                // clone the first and last joints, which will be needed later, because when the entire BP is deleted, the first and last joitns are also deleted
                int frtJntID = getBodyPartFromID(alien, jntLineageList[0])->BP_ID;
                int lstJntID = getBodyPartFromID(alien, jntLineageList.last())->BP_ID;
                Joint *frtJntPtr = instantiateNewJoint(alien);
                Joint *lstJntPtr = instantiateNewJoint(alien);
                // create a cell for this joint
                frtJntPtr->makeCell(getBodyPartFromID(alien, jntLineageList[0])->getPivotPoint(), 2); // create the joint's cell
                lstJntPtr->makeCell(getBodyPartFromID(alien, jntLineageList.last())->getPivotPoint(), 2); // create the joint's cell
                GeometryAlgorithms::rotateBodyPartTowardsVecDir(*frtJntPtr, getBodyPartFromID(alien, jntLineageList[0])->getPivotVector().normalized()); // TODO(ismail): Use a unit vector here
                GeometryAlgorithms::rotateBodyPartTowardsVecDir(*lstJntPtr, getBodyPartFromID(alien, jntLineageList.last())->getPivotVector().normalized()); // TODO(ismail): Use a unit vector here

                tempJointsList.append(dynamic_cast<Joint*>(getBodyPartFromID(alien, jntLineageList[0])));

                // ++ SMOOTHING ALGORITHM 2 -- Now subdivide the lineages using your own algorithm. ++
                QVector<int> jntLineageList_smoothed_prev; jntLineageList_smoothed_prev.clear();
                QVector<int> jntLineageList_smoothed; jntLineageList_smoothed.clear();
                for (int smCntr=0; smCntr<smoothnessAmnt && BP_Ptr->smoothnessLevel<2; smCntr++)
                {
                    // Store the polys for first and last joints in the tempVecEnerAngTrends_TypeGuided copied from VectorEnergyAngleTrends_TypeGuided
                    // The last joint's poly will most probably change in terms of angle, final enery, and energy trend
                    // ... while the angle trend will remain as before

                    // Get all the joints of this BP, so these can be removed from the Bookkeeping DNA DSs
                    // Probably we shouldn't remove the entries for the first and last joint
                    // ... as these will be used when cloning
                    // Now remove all booking DNA DSs
                    removeAssociatedJATsAndOtherDSsForRemovableJnts(alien, getAllJntIDsOfBP(BP_Ptr));

                    while (tempJointsList.length() != 1)
                        tempJointsList.removeLast();

                    Jnt2JntRelExtract->clear();
                    if (smCntr == 0)
                    {
                        for (int i=0; i<jntLineageList.length(); i++)
                        {
                            jntLineageList_smoothed.append(jntLineageList[i]);
                        }
                    }
                    jntLineageList_smoothed_prev.resize(0);

                    // copy the updated smoothed list in the previous one, which will undergo further smoothness
                    for (int i=0; i<jntLineageList_smoothed.length(); i++)
                    {
                        jntLineageList_smoothed_prev.append(jntLineageList_smoothed[i]);
                        if (smCntr>0 && i>0 && i<jntLineageList_smoothed.length()-1)
                        {
                            tempJointsListToRemove.append(dynamic_cast<Joint*>
                                                          (getBodyPartFromID(alien, jntLineageList_smoothed_prev.last())));
        //                    qDebug() << tempJointsListToRemove.last()->BP_ID;
                        }
                    }

                    jntLineageList_smoothed.resize(0);

                    for (int i=0; i<jntLineageList_smoothed_prev.length()-2; i++)
                    {
                        // Create a new list like jntLineageList. Call it jntLineageList_smoothed. Store the joint ID at jntLineageList[i][0] as the first element in each inner list.
        //                jntLineageList_smoothed.append(jntLineageList_smoothed_prev[i]);
                        int tempJntId_2, tempJntId_3; // IDs of the joints taken from the jntLineageList_smoothed_prev

                        // Get the locations of the 3 adjacent joints from jntLineageList_smoothed_prev
                        // ... i.e. jntLineageList_smoothed_prev[i][j], jntLineageList_smoothed_prev[i][j+1], and jntLineageList_smoothed_prev[i][j+2]
                        QVector3D jnt1_Loc = getBodyPartFromID(alien, jntLineageList_smoothed_prev[i])->getPivotPoint();
                        QVector3D jnt2_Loc = getBodyPartFromID(alien, jntLineageList_smoothed_prev[i+1])->getPivotPoint();
                        QVector3D jnt3_Loc = getBodyPartFromID(alien, jntLineageList_smoothed_prev[i+2])->getPivotPoint();

    //                    QPolygonF tempJntEnAngTndPolyF = getEnergyAngleTrendForJoint(alien, jntLineageList_smoothed_prev[i+2]);

                        // find a new Joint at a point at 75% of the first and second joint. Call it joint_1
                        QVector3D newTempInternalJnt1Loc = jnt1_Loc + (0.75*(jnt2_Loc-jnt1_Loc));
                        // create a joint for it and append it to the alienJoints list.
                        Joint *newJntPtr = instantiateNewJoint(alien);
                        tempJointsList.append(newJntPtr);
                        // create a cell for this joint
                        newJntPtr->makeCell(newTempInternalJnt1Loc, 1); // create the joint's cell
                        GeometryAlgorithms::rotateBodyPartTowardsVecDir(*newJntPtr, jnt2_Loc-jnt1_Loc); // TODO(ismail): Use a unit vector here

                        tempJntId_2 = newJntPtr->BP_ID;
                        // update the Jnt2JntRelExtract DS
                        if (i==0)
                        {
                            Jnt2JntRelExtract->append(QPoint(jntLineageList_smoothed_prev[0],tempJntId_2));
                            jntLineageList_smoothed.append(jntLineageList_smoothed_prev[0]);
                        }
                        else
                            Jnt2JntRelExtract->append(QPoint(jntLineageList_smoothed.last(),tempJntId_2));
                        // append this joint's IDs in tempSmoothedList.
                        jntLineageList_smoothed.append(tempJntId_2);


                        // find a new Joint at a point at 100-75%=25% of the second and third joint. Call it joint_2
                        QVector3D newTempInternalJnt2Loc = jnt2_Loc + (0.25*(jnt3_Loc-jnt2_Loc));
                        // create a joint for it and append it to the alienJoints list.
                        newJntPtr = instantiateNewJoint(alien);
                        tempJointsList.append(newJntPtr);
                        // create a cell for this joint
                        newJntPtr->makeCell(newTempInternalJnt2Loc, 1); // create the joint's cell
                        GeometryAlgorithms::rotateBodyPartTowardsVecDir(*newJntPtr, jnt3_Loc-jnt2_Loc); // TODO(ismail): Use a unit vector here

                        tempJntId_3 = newJntPtr->BP_ID;
                        // append this joint's IDs in tempSmoothedList.
                        jntLineageList_smoothed.append(tempJntId_3);
                        // update the Jnt2JntRelExtract DS
                        Jnt2JntRelExtract->append(QPoint(tempJntId_2,tempJntId_3));
                    }

                    Jnt2JntRelExtract->append(QPoint(jntLineageList_smoothed.last(),jntLineageList_smoothed_prev.last()));
                    jntLineageList_smoothed.append(jntLineageList_smoothed_prev.last());
                    tempJointsList.append(dynamic_cast<Joint*>(getBodyPartFromID(alien, jntLineageList_smoothed_prev.last())));

                    BP_Ptr->smoothnessLevel++;
                }

                for (int jt=0; jt<tempJointsListToRemove.length(); jt++)
                {
                    // also remove it from the alien
                    alien->bodyParts.removeAt(alien->bodyParts.indexOf(tempJointsListToRemove[jt]));
                    alien->alienJoints.removeAt(alien->alienJoints.indexOf(tempJointsListToRemove[jt]));
                    delete tempJointsListToRemove[jt];
                }
                tempJointsListToRemove.clear();

                // Note down the ID, Type and Source ID of the BP
                // ... because these will be assigned to the smoothed BP.
                int tempBP_ID = BP_Ptr->BP_ID;
                int tempSourceID = BP_Ptr->clonedFrom;
                QPair<int,int> tempSrcClonedFrom = BP_Ptr->cloneFromAlienAndBP_Pair;
                int tempBP_Type = BP_Ptr->BP_Type;
                int tempSmLvl = BP_Ptr->smoothnessLevel;
                bool isSym = BP_Ptr->isSymmetric;
                int mirrOf = BP_Ptr->mirrorOf;

                // Remove + delete the BP
                QVector<int> tempBP_ID_List_1; tempBP_ID_List_1.clear();
                tempBP_ID_List_1 << BP_Ptr->BP_ID;
                removeAlienBodyParts(alien, tempBP_ID_List_1);

                // Create a new BP and give it the same ID as the input BP
                instantiateNewBodyParts(alien, tempBP_Type, 1);
                BP_Ptr = alien->bodyParts.last();
                BP_Ptr->BP_ID = tempBP_ID;
                BP_Ptr->clonedFrom = tempSourceID; // TODO(ismail): It should be checked that whether the source
                                                      // ... still exists or not
                BP_Ptr->cloneFromAlienAndBP_Pair = tempSrcClonedFrom;
                BP_Ptr->smoothnessLevel = tempSmLvl;
                BP_Ptr->isSymmetric = isSym;
                BP_Ptr->mirrorOf = mirrOf;

                // Since the first and last joints were removed from this BP, we need to give their IDs to their clones below
                frtJntPtr->BP_ID = frtJntID;
                lstJntPtr->BP_ID = lstJntID;
                tempJointsList.removeFirst();
                tempJointsList.prepend(frtJntPtr);
                tempJointsList.removeLast();
                tempJointsList.append(lstJntPtr);

                // append all the newly created joints to this BP's subBPs
                for (Joint *jPtr : tempJointsList)
                {
                    BP_Ptr->subBodyParts.append(jPtr);

                    // Also update the vectorLocationCoupleForEachJoint_TypeGuided by adding the newly created joints in it
                    alien->alienSkeleton.vectorLocationCoupleForEachJoint_TypeGuided.insert(jPtr->BP_ID,
                                     QPair<QVector3D,QVector3D>(jPtr->getPivotVector(),jPtr->getPivotPoint()));
                }
                BP_Ptr->rootJntID = tempJointsList.first()->BP_ID;
                alien->alienSkeleton.Jnt2JntRelations_TypeGuided.insert(BP_Ptr->BP_ID, new QPolygon(*Jnt2JntRelExtract));

                // Update the numOfJointsPerBP_DNA
                QPolygon tempPoly = alien->alienDNA->skeletonDNA->numOfJointsPerBP_DNA.value(BP_Ptr->BP_Type);
                for (int j=0; j<tempPoly.length(); j++)
                {
                    if (tempPoly[j].x() == BP_Ptr->BP_ID)
                    {
                        tempPoly[j].setY(tempJointsList.length());
                        break;
                    }
                }
                alien->alienDNA->skeletonDNA->numOfJointsPerBP_DNA.insert(BP_Ptr->BP_Type,tempPoly);


                // model the bones of this BP
                QMap<int, QPolygon*> tempJnt2JntRelations;
                tempJnt2JntRelations.insert(BP_Ptr->BP_ID, new QPolygon(*Jnt2JntRelExtract));
                modelAllAlienBones_TypeGuided(alien, tempJnt2JntRelations);

                // Get ready for bulkining it up
                // TODO(ismail): This should be the update flag, not the mutation flag
                manipulateSkeletalBulkinessDNA_Flags(alien, false, false, false, false, false, new QVector<int>(0), new QVector<int>(0), true, &tempBP_ID_List_1);

                buildSkeletalBP_BulkinessDNA(alien, tempBP_ID_List_1);
                QVector<Bone*> tempBP_BoneList; tempBP_BoneList.clear();
                getAllBP_Bones(BP_Ptr, tempBP_BoneList);
                qDebug() << "Number of Bones of this BP: " << tempBP_BoneList.length();
                qDebug() << "Number of entries in bulkiness DNA for this BP: " << alien->alienDNA->skeletonDNA->Skeletal_BP_Bulkiness_DNA_ForEachBP.value(BP_Ptr->BP_ID).length();
                if (alien->alienDNA->skeletonDNA->Skeletal_BP_Bulkiness_DNA_ForEachBP.value(BP_Ptr->BP_ID).length() != tempBP_BoneList.length()*4)
                    exit(0);
                // Bulkify the BP
                bulkUpBodyPart(alien, tempBP_ID_List_1);

                // finally re-attach it to its parent BP


                qDebug() << "BP smoothed successfully";

    //            Jnt2JntRelExtract->clear();
                jntLineageList.clear();
                jntLineageList_smoothed.clear();
                jntLineageList_smoothed_prev.clear();
                tempJnt2JntRelations.clear();
    //            tempJnt2JntRelations.first()->clear();
                tempJnt2JntRelations.clear();
                tempJointsList.clear();

                // Finally re-attach the inBP to the parent of Original BP
                if (parentBP_Ptr)
                {
                    if (!parentBP_Ptr->subBodyParts.contains(BP_Ptr))
                        parentBP_Ptr->subBodyParts.append(BP_Ptr);
                    updateBP_AttachmentToOtherBP_withAttachPositionAndVector_DNA(alien, -1,
                                                                                 parentBP_Ptr->BP_ID,
                                                                                 BP_Ptr->BP_ID, BP_Ptr->getPivotPoint(),
                                                                                 BP_Ptr->getPivotVector());
                }
            }
            else
                qDebug() << "Cannot be smoothed as this BP has only two joints";
        }
        else
            qDebug() << "BP cannot be smoothed any further";
    }
    return BP_Ptr;
}

void AlienModeller::smoothenSkeletalBP_Algo2(Alien *alien, BodyPart *BP_Ptr, QPolygon *Jnt2JntRelExtract, quint8 smoothnessAmnt)
{
    if (smoothnessAmnt == 0)
        return;
    // First create a clone of the unsmoothed BP.
    // This can be useful for several purposes such as recomputing JATs.
    if (BP_Ptr->smoothnessLevel == 0)
        createSmoothingBackupCloneBP(alien, BP_Ptr);

    if (BP_Ptr->smoothnessLevel < 3)
    {
        QVector<Joint*> tempJointsList; tempJointsList.clear();
        QVector<Joint*> tempJointsListToRemove; tempJointsListToRemove.clear();
        QVector<int> jntLineageList; jntLineageList.clear();
        convertJnt2JntPolyToJointsLineageList(Jnt2JntRelExtract, jntLineageList);
        qDebug() << jntLineageList;

        if (jntLineageList.length() > 2)
        {
            QMap<int, QPolygonF> tempVecEnerAngTrends_TypeGuided; tempVecEnerAngTrends_TypeGuided.clear();
            QMap<int, float> tempRandFloatForBookKeeping_FinalEnergy; tempRandFloatForBookKeeping_FinalEnergy.clear();
            QPolygonF tempPolyF_forTempJntId_2, tempPolyF_forTempJntId_3, tempPolyF_jntFirst, tempPolyF_jntLast;

            // clone the first and last joints, which will be needed later, because when the entire BP is deleted, the first and last joitns are also deleted
            int frtJntID = getBodyPartFromID(alien, jntLineageList[0])->BP_ID;
            int lstJntID = getBodyPartFromID(alien, jntLineageList.last())->BP_ID;
            Joint *frtJntPtr = instantiateNewJoint(alien);
            Joint *lstJntPtr = instantiateNewJoint(alien);
            // create a cell for this joint
            frtJntPtr->makeCell(getBodyPartFromID(alien, jntLineageList[0])->getPivotPoint(), 2); // create the joint's cell
            lstJntPtr->makeCell(getBodyPartFromID(alien, jntLineageList.last())->getPivotPoint(), 2); // create the joint's cell
            GeometryAlgorithms::rotateBodyPartTowardsVecDir(*frtJntPtr, getBodyPartFromID(alien, jntLineageList[0])->getPivotVector().normalized()); // TODO(ismail): Use a unit vector here
            GeometryAlgorithms::rotateBodyPartTowardsVecDir(*lstJntPtr, getBodyPartFromID(alien, jntLineageList.last())->getPivotVector().normalized()); // TODO(ismail): Use a unit vector here


            tempJointsList.append(dynamic_cast<Joint*>(getBodyPartFromID(alien, jntLineageList[0])));
    //        qDebug() << tempJointsList[0]->BP_ID;

            // ++ SMOOTHING ALGORITHM 2 -- Now subdivide the lineages using your own algorithm. ++
            QVector<int> jntLineageList_smoothed_prev; jntLineageList_smoothed_prev.clear();
            QVector<int> jntLineageList_smoothed; jntLineageList_smoothed.clear();
            for (int smCntr=0; smCntr<smoothnessAmnt && BP_Ptr->smoothnessLevel<3; smCntr++)
            {
                // Store the polys for first and last joints in the tempVecEnerAngTrends_TypeGuided copied from VectorEnergyAngleTrends_TypeGuided
                // The last joint's poly will most probably change in terms of angle, final enery, and energy trend
                // ... while the angle trend will remain as before

                // Get all the joints of this BP, so these can be removed from the Bookkeeping DNA DSs
                // Probably we shouldn't remove the entries for the first and last joint
                // ... as these will be used when cloning
                // First copy all those that will needed during the smoothing loop
//                qDebug() << alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.VectorEnergyAngleTrends_TypeGuided.keys();
                tempVecEnerAngTrends_TypeGuided = QMap<int, QPolygonF>(alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.VectorEnergyAngleTrends_TypeGuided);
    //            QMap<int, int> tempRandIntForBookKeeping_EnergyTrend = alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.randIntForBookKeeping_EnergyTrend;
                tempRandFloatForBookKeeping_FinalEnergy = QMap<int, float>(alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.randFloatForBookKeeping_FinalEnergy);
    //            QMap<int, int> tempRandIntForBookKeeping_AngleTrend = alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.randIntForBookKeeping_AngleTrend;
                // Now remove all booking DNA DSs
                removeAssociatedJATsAndOtherDSsForRemovableJnts(alien, getAllJntIDsOfBP(BP_Ptr));



                while (tempJointsList.length() != 1)
                    tempJointsList.removeLast();

                Jnt2JntRelExtract->clear();
                if (smCntr == 0)
                {
                    for (int i=0; i<jntLineageList.length(); i++)
                    {
                        jntLineageList_smoothed.append(jntLineageList[i]);
                    }
                }
                jntLineageList_smoothed_prev.resize(0);

                // copy the updated smoothed list in the previous one, which will undergo further smoothness
                for (int i=0; i<jntLineageList_smoothed.length(); i++)
                {
                    jntLineageList_smoothed_prev.append(jntLineageList_smoothed[i]);
                    if (smCntr>0 && i>0 && i<jntLineageList_smoothed.length()-1)
                    {
                        tempJointsListToRemove.append(dynamic_cast<Joint*>
                                                      (getBodyPartFromID(alien, jntLineageList_smoothed_prev.last())));
    //                    qDebug() << tempJointsListToRemove.last()->BP_ID;
                    }
                }

                jntLineageList_smoothed.resize(0);

//                qDebug() << jntLineageList_smoothed_prev;
//                qDebug() << tempVecEnerAngTrends_TypeGuided.keys();

                float prevFinalAngle=-1;
                for (int i=0; i<jntLineageList_smoothed_prev.length()-2; i++)
                {

                    // Create a new list like jntLineageList. Call it jntLineageList_smoothed. Store the joint ID at jntLineageList[i][0] as the first element in each inner list.
    //                jntLineageList_smoothed.append(jntLineageList_smoothed_prev[i]);
                    int tempJntId_2, tempJntId_3; // IDs of the joints taken from the jntLineageList_smoothed_prev

                    // Get the locations of the 3 adjacent joints from jntLineageList_smoothed_prev
                    // ... i.e. jntLineageList_smoothed_prev[i][j], jntLineageList_smoothed_prev[i][j+1], and jntLineageList_smoothed_prev[i][j+2]
                    QVector3D jnt1_Loc = getBodyPartFromID(alien, jntLineageList_smoothed_prev[i])->getPivotPoint();
                    QVector3D jnt2_Loc = getBodyPartFromID(alien, jntLineageList_smoothed_prev[i+1])->getPivotPoint();
                    QVector3D jnt3_Loc = getBodyPartFromID(alien, jntLineageList_smoothed_prev[i+2])->getPivotPoint();

//                    QPolygonF tempJntEnAngTndPolyF = getEnergyAngleTrendForJoint(alien, jntLineageList_smoothed_prev[i+2]);

                    // find a new Joint at a point at 75% of the first and second joint. Call it joint_1
                    QVector3D newTempInternalJnt1Loc = jnt1_Loc + (0.75*(jnt2_Loc-jnt1_Loc));
                    // create a joint for it and append it to the alienJoints list.
                    Joint *newJntPtr = instantiateNewJoint(alien);
                    tempJointsList.append(newJntPtr);
                    // create a cell for this joint
                    newJntPtr->makeCell(newTempInternalJnt1Loc, 1); // create the joint's cell
                    GeometryAlgorithms::rotateBodyPartTowardsVecDir(*newJntPtr, jnt2_Loc-jnt1_Loc); // TODO(ismail): Use a unit vector here

                    tempJntId_2 = newJntPtr->BP_ID;
                    // update the Jnt2JntRelExtract DS
                    if (i==0)
                    {
                        Jnt2JntRelExtract->append(QPoint(jntLineageList_smoothed_prev[0],tempJntId_2));
                        jntLineageList_smoothed.append(jntLineageList_smoothed_prev[0]);
                    }
                    else
                        Jnt2JntRelExtract->append(QPoint(jntLineageList_smoothed.last(),tempJntId_2));
                    // append this joint's IDs in tempSmoothedList.
                    jntLineageList_smoothed.append(tempJntId_2);


                    // find a new Joint at a point at 100-75%=25% of the second and third joint. Call it joint_2
                    QVector3D newTempInternalJnt2Loc = jnt2_Loc + (0.25*(jnt3_Loc-jnt2_Loc));
                    // create a joint for it and append it to the alienJoints list.
                    newJntPtr = instantiateNewJoint(alien);
                    tempJointsList.append(newJntPtr);
                    // create a cell for this joint
                    newJntPtr->makeCell(newTempInternalJnt2Loc, 1); // create the joint's cell
                    GeometryAlgorithms::rotateBodyPartTowardsVecDir(*newJntPtr, jnt3_Loc-jnt2_Loc); // TODO(ismail): Use a unit vector here

                    tempJntId_3 = newJntPtr->BP_ID;
                    // append this joint's IDs in tempSmoothedList.
                    jntLineageList_smoothed.append(tempJntId_3);
                    // update the Jnt2JntRelExtract DS
                    Jnt2JntRelExtract->append(QPoint(tempJntId_2,tempJntId_3));


                    // compute final angle, angle trend, final energy, and energy trend
                    // ... Angle Trend of tempJntId_3 = angle trend of jntLineageList_smoothed_prev[i+2] copied from VectorEnergyAngleTrends_TypeGuided
                    // ... Angle Trend of tempJntId_2 = Angle trend of jntLineageList_smoothed_prev[i+1] copied from VectorEnergyAngleTrends_TypeGuided

                    // ... Final Angle of tempJntId_3
                    // ... if angleTrend of jntLineageList_smoothed_prev[i+2] = Des OR WavyDown, then final angle = 360 - angle/2, where angle is taken from VectorEnergyAngleTrends_TypeGuided
                    // ... if angleTrend of jntLineageList_smoothed_prev[i+2] = Asc OR WavyUp, then final angle = angle/2, where angle is taken from VectorEnergyAngleTrends_TypeGuided
                    // ... if (i==0) then ==> Final Angle of tempJntId_2 = final angle of jntLineageList_smoothed_prev[i+1]
                    // ... else Final Angle of tempJntId_2 = previous_final_angle
                    // ... Store final angle of tempJntId_3 in previous_final_angle

                    // ... Final Energy of tempJntId_2 = magnitude of vector(tempJntId_2-jntLineageList_smoothed[last-3])
                    // ... final energy of tempJntId_3 = magnitude of vector(tempJntId_3-tempJntId_2)
                    // ... Store final energy of tempJntId_2 in randFloatForBookKeeping_FinalEnergy
                    // ... Store final energy of tempJntId_3 in randFloatForBookKeeping_FinalEnergy
                    // ... If this is the last iteration, final energy of jntLineageList_smoothed[last-1] = magnitude of vector(tempJntId_3-tempJntId_2)

                    // ... Energy trend of tempJntId_3 = Asc if energy of tempJntId_3 > tempJntId_2
                    // ...                               Des if energy of tempJntId_3 < tempJntId_2
                    // ...                               Constant if energy of tempJntId_3 = energy of tempJntId_2
                    // ... Energy Trend of tempJntId_2 = Asc if energy of tempJntId_2 > energy of jntLineageList_smoothed[last-3]
                    // ...                               Des if energy of tempJntId_2 < energy of jntLineageList_smoothed[last-3]
                    // ...                               Const if energy of tempJntId_2 = energy of jntLineageList_smoothed[last-3]
                    // ... If this is the last iteration, compute final angle, final energy and energy trend. Angle trend will remain as before

                    // Also update the bookkeeping DSs in JATs DNA

//                    alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.VectorEnergyAngleTrends_TypeGuided.insert(tempJntId_3, )

                    tempPolyF_forTempJntId_2.clear(); tempPolyF_forTempJntId_3.clear(); tempPolyF_jntLast.clear(); tempPolyF_jntFirst.clear();
                    tempPolyF_forTempJntId_2 << QPointF(-1,-1) << QPointF(-1,-1);
                    tempPolyF_forTempJntId_3 << QPointF(-1,-1) << QPointF(-1,-1);
                    tempPolyF_jntLast << QPointF(-1,-1) << QPointF(-1,-1);
                    tempPolyF_jntFirst << QPointF(-1,-1) << QPointF(-1,-1);

                    // Angle Trend
                    tempPolyF_forTempJntId_2[1].setX(tempVecEnerAngTrends_TypeGuided.value(jntLineageList_smoothed_prev[i+1]).at(1).x());
                    tempPolyF_forTempJntId_3[1].setX(tempVecEnerAngTrends_TypeGuided.value(jntLineageList_smoothed_prev[i+2]).at(1).x());
                    if (i==0)
                    {
                        tempPolyF_jntFirst[1].setX(tempVecEnerAngTrends_TypeGuided.value(jntLineageList_smoothed_prev[i]).at(1).x());
                    }
                    if (i==jntLineageList_smoothed_prev.length()-3)
                        tempPolyF_jntLast[1].setX(tempVecEnerAngTrends_TypeGuided.value(jntLineageList_smoothed_prev[i+2]).at(1).x());

                    // Final Angle
                    if (tempVecEnerAngTrends_TypeGuided.value(jntLineageList_smoothed_prev[i+2]).at(1).x() == 1
                            ||
                        tempVecEnerAngTrends_TypeGuided.value(jntLineageList_smoothed_prev[i+2]).at(1).x() == 3)
                    {
                        tempPolyF_forTempJntId_3[1].setY( 360.0 - (tempVecEnerAngTrends_TypeGuided.value(jntLineageList_smoothed_prev[i+2]).at(1).y()/2.0) );
                    }
                    else if (tempVecEnerAngTrends_TypeGuided.value(jntLineageList_smoothed_prev[i+2]).at(1).x() == 0
                             ||
                         tempVecEnerAngTrends_TypeGuided.value(jntLineageList_smoothed_prev[i+2]).at(1).x() == 2)
                    {
                        tempPolyF_forTempJntId_3[1].setY( tempVecEnerAngTrends_TypeGuided.value(jntLineageList_smoothed_prev[i+2]).at(1).y()/2.0 );
                    }

                    if (i==0)
                    {
                        tempPolyF_jntFirst[1].setY(tempVecEnerAngTrends_TypeGuided.value(jntLineageList_smoothed_prev[i]).at(1).y());
                        tempPolyF_forTempJntId_2[1].setY(tempVecEnerAngTrends_TypeGuided.value(jntLineageList_smoothed_prev[i+1]).at(1).y());
                    }
                    else
                        tempPolyF_forTempJntId_2[1].setY(prevFinalAngle);
                    if (i==jntLineageList_smoothed_prev.length()-3)
                        tempPolyF_jntLast[1].setY(tempPolyF_forTempJntId_3[1].y());
                    prevFinalAngle = tempPolyF_forTempJntId_3[1].y();

                    // Final Energy
                    tempPolyF_forTempJntId_2[0].setY( (getBodyPartFromID(alien, tempJntId_2)->getPivotPoint() -
                                             getBodyPartFromID(alien, jntLineageList_smoothed[jntLineageList_smoothed.length()-3])->getPivotPoint()).length() );

                    tempPolyF_forTempJntId_3[0].setY( (getBodyPartFromID(alien, tempJntId_3)->getPivotPoint() -
                                             getBodyPartFromID(alien, tempJntId_2)->getPivotPoint()).length() );

                    alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.randFloatForBookKeeping_FinalEnergy.insert(tempJntId_2, tempPolyF_forTempJntId_2[0].y());
                    alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.randFloatForBookKeeping_FinalEnergy.insert(tempJntId_3, tempPolyF_forTempJntId_3[0].y());

                    if (i==0)
                    {
                        tempPolyF_jntFirst[0].setY(tempVecEnerAngTrends_TypeGuided.value(jntLineageList_smoothed_prev[i]).at(0).y());
                        alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.randFloatForBookKeeping_FinalEnergy.insert(jntLineageList_smoothed_prev[i], tempRandFloatForBookKeeping_FinalEnergy.value(jntLineageList_smoothed_prev[i]));
                    }

                    if (i==jntLineageList_smoothed_prev.length()-3)
                    {
                        tempPolyF_jntLast[0].setY( (getBodyPartFromID(alien, jntLineageList_smoothed_prev.last())->getPivotPoint() -
                                                    getBodyPartFromID(alien, tempJntId_3)->getPivotPoint()).length() );

                        alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.randFloatForBookKeeping_FinalEnergy.insert(jntLineageList_smoothed_prev[i+2], tempPolyF_jntLast[0].y());
                    }

                    // Energy Trend
                    if (tempPolyF_forTempJntId_3[0].y() > tempPolyF_forTempJntId_2[0].y()) // Ascending
                        tempPolyF_forTempJntId_3[0].setX(0);
                    else if (tempPolyF_forTempJntId_3[0].y() < tempPolyF_forTempJntId_2[0].y()) // Descending
                        tempPolyF_forTempJntId_3[0].setX(1);
                    else if (tempPolyF_forTempJntId_3[0].y() == tempPolyF_forTempJntId_2[0].y()) // Constant
                        tempPolyF_forTempJntId_3[0].setX(5);

                    if (tempPolyF_forTempJntId_2[0].y() > alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.randFloatForBookKeeping_FinalEnergy.value(jntLineageList_smoothed[jntLineageList_smoothed.length()-3])) // Ascending
                        tempPolyF_forTempJntId_2[0].setX(0);
                    else if (tempPolyF_forTempJntId_2[0].y() < alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.randFloatForBookKeeping_FinalEnergy.value(jntLineageList_smoothed[jntLineageList_smoothed.length()-3])) // Descending
                        tempPolyF_forTempJntId_2[0].setX(1);
                    else if (tempPolyF_forTempJntId_2[0].y() == alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.randFloatForBookKeeping_FinalEnergy.value(jntLineageList_smoothed[jntLineageList_smoothed.length()-3])) // Constant
                        tempPolyF_forTempJntId_2[0].setX(5);

                    if (i==0)
                        tempPolyF_jntFirst[0].setX(tempVecEnerAngTrends_TypeGuided.value(jntLineageList_smoothed_prev[i]).at(0).x());
                    if (i==jntLineageList_smoothed_prev.length()-3)
                    {
                        if (tempPolyF_jntLast[0].y() > tempPolyF_forTempJntId_3[0].y()) // Ascending
                            tempPolyF_jntLast[0].setX(0);
                        else if (tempPolyF_jntLast[0].y() < tempPolyF_forTempJntId_3[0].y()) // Descending
                            tempPolyF_jntLast[0].setX(1);
                        else if (tempPolyF_jntLast[0].y() == tempPolyF_forTempJntId_3[0].y()) // Constant
                            tempPolyF_jntLast[0].setX(5);
                    }

                    // Finally store all the polys in the VectorEnergyAngleTrends_TypeGuided
                    if (i==0)
                    {
                        alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.VectorEnergyAngleTrends_TypeGuided.insert(jntLineageList_smoothed_prev[i], tempPolyF_jntFirst);
                    }
                    if (i==jntLineageList_smoothed_prev.length()-3)
                        alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.VectorEnergyAngleTrends_TypeGuided.insert(jntLineageList_smoothed_prev.last(), tempPolyF_jntLast);
                    alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.VectorEnergyAngleTrends_TypeGuided.insert(tempJntId_2, tempPolyF_forTempJntId_2);
                    alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.VectorEnergyAngleTrends_TypeGuided.insert(tempJntId_3, tempPolyF_forTempJntId_3);

                }

                Jnt2JntRelExtract->append(QPoint(jntLineageList_smoothed.last(),jntLineageList_smoothed_prev.last()));
                jntLineageList_smoothed.append(jntLineageList_smoothed_prev.last());
                tempJointsList.append(dynamic_cast<Joint*>(getBodyPartFromID(alien, jntLineageList_smoothed_prev.last())));

                BP_Ptr->smoothnessLevel++;
            }

//            qDebug() << "jntLineageList_smoothed : " << jntLineageList_smoothed;
//            qDebug() << "Jnt2JntRelExtract : " << Jnt2JntRelExtract;
    //        for (Joint* j : tempJointsList)
    //            qDebug() << " , " << j->BP_ID;

            for (int jt=0; jt<tempJointsListToRemove.length(); jt++)
            {
    //            qDebug() << tempJointsListToRemove[jt]->BP_ID;
                // also remove it from the alien
                alien->bodyParts.removeAt(alien->bodyParts.indexOf(tempJointsListToRemove[jt]));
                alien->alienJoints.removeAt(alien->alienJoints.indexOf(tempJointsListToRemove[jt]));
                delete tempJointsListToRemove[jt];
            }
            tempJointsListToRemove.clear();

            // Note down the ID, Type and Source ID of the BP
            // ... because these will be assigned to the smoothed BP.
            int tempBP_ID = BP_Ptr->BP_ID;
            int tempSourceID = BP_Ptr->clonedFrom;
            QPair<int,int> tempSrcClonedFrom = BP_Ptr->cloneFromAlienAndBP_Pair;
            int tempBP_Type = BP_Ptr->BP_Type;
            int tempSmLvl = BP_Ptr->smoothnessLevel;

            // Remove + delete the BP
            QVector<int> tempBP_ID_List_1; tempBP_ID_List_1.clear();
            tempBP_ID_List_1 << BP_Ptr->BP_ID;
            removeAlienBodyParts(alien, tempBP_ID_List_1);

            // Create a new BP and give it the same ID as the input BP
            instantiateNewBodyParts(alien, tempBP_Type, 1);
            BodyPart *newBP_Ptr = alien->bodyParts.last();
            newBP_Ptr->BP_ID = tempBP_ID;
            newBP_Ptr->clonedFrom = tempSourceID; // TODO(ismail): It should be checked that whether the source
                                                  // ... still exists or not
            newBP_Ptr->cloneFromAlienAndBP_Pair = tempSrcClonedFrom;
            newBP_Ptr->smoothnessLevel = tempSmLvl;

            // Since the first and last joints were removed from this BP, we need to give their IDs to their clones below
            frtJntPtr->BP_ID = frtJntID;
            lstJntPtr->BP_ID = lstJntID;
            tempJointsList.removeFirst();
            tempJointsList.prepend(frtJntPtr);
            tempJointsList.removeLast();
            tempJointsList.append(lstJntPtr);

            // This is only used when a smoothed BP is extended with more joints appended at the end
            // TODO(ismail): Inspect this as this code is buggy
//            for (int jt=0; jt<Jnt2JntRelExtract->length(); jt++)
//            {
//                QPolygonF polyF; polyF.clear();
//                QVector3D firJntPos = getBodyPartFromID(alien, Jnt2JntRelExtract->at(jt).x())->getPivotPoint();
//                QVector3D secJntPos = getBodyPartFromID(alien, Jnt2JntRelExtract->at(jt).y())->getPivotPoint();

//                float energy = (secJntPos - firJntPos).length();
//                polyF << QPoint(0, energy) << QPoint(0, 0);
//                if (!alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.VectorEnergyAngleTrends_TypeGuided.contains(Jnt2JntRelExtract->at(jt).y()))
//                    alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.VectorEnergyAngleTrends_TypeGuided.insert(Jnt2JntRelExtract->at(jt).y(), polyF);
//            }

            // append all the newly created joints to this BP's subBPs
            for (Joint *jPtr : tempJointsList)
            {
                newBP_Ptr->subBodyParts.append(jPtr);

                // Also update the vectorLocationCoupleForEachJoint_TypeGuided by adding the newly created joints in it
                alien->alienSkeleton.vectorLocationCoupleForEachJoint_TypeGuided.insert(jPtr->BP_ID,
                                 QPair<QVector3D,QVector3D>(jPtr->getPivotVector(),jPtr->getPivotPoint()));
            }
            newBP_Ptr->rootJntID = tempJointsList.first()->BP_ID;
            alien->alienSkeleton.Jnt2JntRelations_TypeGuided.insert(newBP_Ptr->BP_ID, new QPolygon(*Jnt2JntRelExtract));

            alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.VectorEnergyAngleTrends_TypeGuided.insert(frtJntPtr->BP_ID, tempPolyF_jntFirst);
            alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.VectorEnergyAngleTrends_TypeGuided.insert(lstJntPtr->BP_ID, tempPolyF_jntLast);

            // Update the numOfJointsPerBP_DNA
            QPolygon tempPoly = alien->alienDNA->skeletonDNA->numOfJointsPerBP_DNA.value(newBP_Ptr->BP_Type);
            for (int j=0; j<tempPoly.length(); j++)
            {
                if (tempPoly[j].x() == newBP_Ptr->BP_ID)
                {
                    tempPoly[j].setY(tempJointsList.length());
                    break;
                }
            }
            alien->alienDNA->skeletonDNA->numOfJointsPerBP_DNA.insert(newBP_Ptr->BP_Type,tempPoly);


            // model the bones of this BP
            QMap<int, QPolygon*> tempJnt2JntRelations;
            tempJnt2JntRelations.insert(newBP_Ptr->BP_ID, new QPolygon(*Jnt2JntRelExtract));
            modelAllAlienBones_TypeGuided(alien, tempJnt2JntRelations);

            // Bulkify the BP
            manipulateSkeletalBulkinessDNA_Flags(alien, false, false, true, false, false, new QVector<int>(0), &tempBP_ID_List_1);

            buildSkeletalBP_BulkinessDNA(alien, tempBP_ID_List_1);
            bulkUpBodyPart(alien, tempBP_ID_List_1);

            qDebug() << "BP smoothed successfully";

//            Jnt2JntRelExtract->clear();
            jntLineageList.clear();
            jntLineageList_smoothed.clear();
            jntLineageList_smoothed_prev.clear();
            tempJnt2JntRelations.clear();
//            tempJnt2JntRelations.first()->clear();
            tempJnt2JntRelations.clear();
            tempJointsList.clear();
        }
        else
            qDebug() << "Cannot be smoothed as this BP has only two joints";
    }
    else
        qDebug() << "BP cannot be smoothed any further";
}

/*
void AlienModeller::smoothLineageFromSkeleton(QPolygon *Jnt2JntRelPtr, quint8 smoothnessAmnt)
{
    // ++ SMOOTHING ALGORITHM 2 ++
    QVector<int> jntLineageList; jntLineageList.resize(0);
    QVector<int> jntLineageList_smoothed_prev;
    QVector<int> jntLineageList_smoothed;

    convertJnt2JntPolyToJointsLineageList(Jnt2JntRelPtr, jntLineageList);

    if (smoothnessAmnt > 3)
        smoothnessAmnt = 3;

    for (int smoothnessIndex=0; smoothnessIndex<smoothnessAmnt; smoothnessIndex++)
    {
        if (smoothnessIndex == 0)
        {
            for (int j=0; j<jntLineageList.length(); j++)
            {
                jntLineageList_smoothed_prev.append(jntLineageList[j]);
            }
        }
        else
        {
            for (int j=1; j<jntLineageList_smoothed_prev.length()-1; j++)
            {
                removeJoint(jntLineageList_smoothed_prev[j]);
            }
            jntLineageList_smoothed_prev.resize(0);

            for (int j=0; j<jntLineageList_smoothed.length(); j++)
            {
                jntLineageList_smoothed_prev.append(jntLineageList_smoothed[j]);
            }
            jntLineageList_smoothed.resize(0);
        }

        // Create a new list like jntLineageList. Call it jntLineageList_smoothed. Store the joint ID at jntLineageList[i][0] as the first element in each inner list.
        int tempJntId_1, tempJntId_2, tempJntId_3; // IDs of the joints taken from the jntLineageList_smoothed_prev
        tempJntId_1 = jntLineageList_smoothed_prev[0];
        jntLineageList_smoothed.append(tempJntId_1);
        for (int j=0; j<jntLineageList_smoothed_prev.length()-2; j++)
        {
//            // select a random number for the amount of intermediate points to be computed
//            int randNumInterPoints = GeometryAlgorithms::getRandomInteger(6,7);
//            qDebug() << "number of subdivisions: " << randNumInterPoints;

            // Get the locations of the 3 adjacent joints from jntLineageList_smoothed_prev
            // ... i.e. jntLineageList_smoothed_prev[j], jntLineageList_smoothed_prev[j+1], and jntLineageList_smoothed_prev[j+2]
            QVector3D jnt1_Loc, jnt2_Loc, jnt3_Loc;
            for (int jntIter=0; jntIter<alienList.last()->alienJoints.length(); jntIter++)
            {
                int tempJnt_ID = alienList.last()->alienJoints[jntIter]->BP_ID;
                if (jntLineageList_smoothed_prev[j] == tempJnt_ID)
                    jnt1_Loc = alienList.last()->alienJoints[jntIter]->getPivotPoint();
                else if (jntLineageList_smoothed_prev[j+1] == tempJnt_ID)
                    jnt2_Loc = alienList.last()->alienJoints[jntIter]->getPivotPoint();
                else if (jntLineageList_smoothed_prev[j+2] == tempJnt_ID)
                    jnt3_Loc = alienList.last()->alienJoints[jntIter]->getPivotPoint();
            }

            // find a new Joint at a point at 75% of the first and second joint. Call it joint_1
            QVector3D newTempIntJnt_1 = jnt1_Loc + (0.75*(jnt2_Loc-jnt1_Loc));
            // create a joint for it and append it to the alienJoints list.
            Joint *newJntPtr = instantiateNewJoint();
            // create a cell for this joint
            newJntPtr->makeCell(newTempIntJnt_1, 1); // create the joint's cell
            tempJntId_2 = newJntPtr->BP_ID;
            // append this joint's IDs in tempSmoothedList.
            jntLineageList_smoothed.append(tempJntId_2);
            // update the Jnt2JntRelations DS
//                Jnt2JntRelPtr->append(QPoint(tempJntId_1,tempJntId_2));
//                alienList.last()->alienSkeleton.Jnt2JntRelations.append(QPoint(tempJntId_1,tempJntId_2));

            // find a new Joint at a point at 100-75% of the second and third joint. Call it joint_2
            QVector3D newTempIntJnt_2 = jnt2_Loc + (0.25*(jnt3_Loc-jnt2_Loc));
            // create a joint for it and append it to the alienJoints list.
            newJntPtr = instantiateNewJoint();
            // create a cell for this joint
            newJntPtr->makeCell(newTempIntJnt_2, 1); // create the joint's cell
            tempJntId_3 = newJntPtr->BP_ID;
            // append this joint's IDs in tempSmoothedList.
            jntLineageList_smoothed.append(tempJntId_3);
            // update the Jnt2JntRelations DS
//                Jnt2JntRelPtr->append(QPoint(tempJntId_2,tempJntId_3));
//                alienList.last()->alienSkeleton.Jnt2JntRelations.append(QPoint(tempJntId_2,tempJntId_3));

            // Remove the previous parent-child relationship from the Jnt2JntRelations DS and insert the new one
            int k=0;
            for (k=0; k<Jnt2JntRelPtr->length(); k++)
            {
                int firstJntID = Jnt2JntRelPtr->at(k).x();
                int secondJntID = Jnt2JntRelPtr->at(k).y();
                if (jntLineageList_smoothed_prev[j] == firstJntID  &&  jntLineageList_smoothed_prev[j+1] == secondJntID)
                {
                    Jnt2JntRelPtr->remove(k);

                    qDebug() << "Jnt2Jnt Pair removed while smoothing";
                    break;
                }
            }

            Jnt2JntRelPtr->insert(k,QPoint(tempJntId_1,tempJntId_2));
            Jnt2JntRelPtr->insert(k+1,QPoint(tempJntId_2,tempJntId_3));

            tempJntId_1 = tempJntId_3;

        }
        jntLineageList_smoothed.append(jntLineageList_smoothed_prev[jntLineageList_smoothed_prev.length()-1]);
        Jnt2JntRelPtr->removeLast();
        Jnt2JntRelPtr->append(QPoint(tempJntId_3,jntLineageList_smoothed_prev[jntLineageList_smoothed_prev.length()-1]));
    }

    // Now create the bones
    for (int i=0; i<Jnt2JntRelPtr->length(); i++)
    {
        Bone *newBonePtr = instantiateNewBone();
        newBonePtr->ContainingBP_ID = ContainingBP_ID;
        createSimpleBone(*newBonePtr, getJointFromID(Jnt2JntRelPtr->at(i).x())->getPivotPoint(), getJointFromID(Jnt2JntRelPtr->at(i).y())->getPivotPoint());
    }

}
*/

void AlienModeller::instantiateSkeletalBPsFromDNA_TypeGuided(Alien *alien)
{
    qDebug() << "\ninstantiating Skeletal BPs from DNA";

    QMapIterator<int, int> i(alien->alienDNA->skeletonDNA->numOfSkeletalBodyParts_DNA);
    while (i.hasNext())
    {
        i.next();
        int tempNumBPs = i.value();
        instantiateNewBodyParts(alien, i.key(), tempNumBPs);
    }

    // Now update the skeletonDNA->numOfJointsPerBP_DNA DS so that each QPoint entry in the QPolygon contains the
    // ... BP_ID in QPoint as (x=BP_ID, y=num of Jnts for this BP).
    // This will be important for cloning in later stages
    QMapIterator<int, QPolygon> iter_2(alien->alienDNA->skeletonDNA->numOfJointsPerBP_DNA);
    while (iter_2.hasNext())
    {
        iter_2.next();
        QPolygon tempPoly = iter_2.value();
        if (iter_2.key() == AlienModeller::BodyPartsEnum::LOWER_LIMB)
        {
            for (int j=0; j<tempPoly.length(); j++)
            {
                if (tempPoly[j].x() == -1)
                    alien->BP_ReplacementMap.insert(alien->alienLowerLimbs[j]->BP_ID, alien->alienLowerLimbs[j]->BP_ID);
                else
                    alien->BP_ReplacementMap.insert(tempPoly[j].x(), alien->alienLowerLimbs[j]->BP_ID);
                tempPoly[j].setX(alien->alienLowerLimbs[j]->BP_ID);
            }
        }
        else if (iter_2.key() == AlienModeller::BodyPartsEnum::UPPER_LIMB)
        {
            for (int j=0; j<tempPoly.length(); j++)
            {
                if (tempPoly[j].x() == -1)
                    alien->BP_ReplacementMap.insert(alien->alienUpperLimbs[j]->BP_ID, alien->alienUpperLimbs[j]->BP_ID);
                else
                    alien->BP_ReplacementMap.insert(tempPoly[j].x(), alien->alienUpperLimbs[j]->BP_ID);
                tempPoly[j].setX(alien->alienUpperLimbs[j]->BP_ID);
            }
        }
        else if (iter_2.key() == AlienModeller::BodyPartsEnum::BACK_BONE)
        {
            for (int j=0; j<tempPoly.length(); j++)
            {
                if (tempPoly[j].x() == -1)
                    alien->BP_ReplacementMap.insert(alien->alienBackBones[j]->BP_ID, alien->alienBackBones[j]->BP_ID);
                else
                    alien->BP_ReplacementMap.insert(tempPoly[j].x(), alien->alienBackBones[j]->BP_ID);
                tempPoly[j].setX(alien->alienBackBones[j]->BP_ID);
            }
        }
        else if (iter_2.key() == AlienModeller::BodyPartsEnum::HEAD)
        {
            if (!alien->alienHeads.isEmpty())
            {
                for (int j=0; j<tempPoly.length(); j++)
                {
                    tempPoly[j].setX(alien->alienHeads[j]->BP_ID);
                }
            }
        }
        else if (iter_2.key() == AlienModeller::BodyPartsEnum::HEADSubBP_Maxilla)
        {
            if (!alien->alienHeads.isEmpty())
            {
                for (int j=0; j<tempPoly.length(); j++)
                {
                    tempPoly[j].setX(alien->alienHeadSubBP_Maxillas[j]->BP_ID);
                }
            }
        }
        else if (iter_2.key() == AlienModeller::BodyPartsEnum::HEADSubBP_Mandible)
        {
            if (!alien->alienHeads.isEmpty())
            {
                for (int j=0; j<tempPoly.length(); j++)
                {
                    tempPoly[j].setX(alien->alienHeadSubBP_Mandibles[j]->BP_ID);
                }
            }
        }
        else if (iter_2.key() == AlienModeller::BodyPartsEnum::HEADSubBP_Zygomatic)
        {
            if (!alien->alienHeads.isEmpty())
            {
                for (int j=0; j<tempPoly.length(); j++)
                {
                    tempPoly[j].setX(alien->alienHeadSubBP_Zygomatics[j]->BP_ID);
                }
            }
        }

//        else if (iter_2.key() == AlienModeller::BodyPartsEnum::TAIL)
//        {
//            for (int j=0; j<tempPoly.length(); j++)
//            {
//                tempPoly[j].setX(alien->alienTails[j]->BP_ID);
//            }
//        }
//        else if (iter_2.key() == AlienModeller::BodyPartsEnum::HEAD)
//        {
//            for (int j=0; j<tempPoly.length(); j++)
//            {
//                tempPoly[j].setX(alien->alienHeads[j]->BP_ID);
//            }
//        }
        alien->alienDNA->skeletonDNA->numOfJointsPerBP_DNA.insert(iter_2.key(),tempPoly);
    }

    qDebug() << "Skeletal BPs from DNA instantiated successfully";
}

void AlienModeller::instantiateNumOfJointsForAllBPs_TypeGuided(Alien *alien, QMap<int, QPolygon> &numOfJointsPerBP_DNA)
{
    // TODO(Ismail): Need to find a better way of doing this. Perhaps using Factory Design Patterns.

    qDebug() << "\nInstantiating Joints for all BPs from DNA";
//    qDebug() << numOfJointsPerBP_DNA;
    QMapIterator<int, QPolygon> i(numOfJointsPerBP_DNA);
    while (i.hasNext())
    {
        i.next();
        QPolygon tempPoly = i.value();
        for (int j=0; j<tempPoly.length(); j++)
        {
            BodyPart *tempBP_Ptr = getBodyPartFromID(alien, tempPoly[j].x());
            if (tempBP_Ptr)
            {
                for (int k=0; k<tempPoly[j].y(); k++)
                {
                    tempBP_Ptr->subBodyParts.append(this->instantiateNewJoint(alien));
                }
            }
            else
            {
                qDebug() << "BP does not exit - instantiateNumOfJointsForAllBPs_TypeGuided method";
                exit(0);
            }
        }
    }

    qDebug() << "Joints for all BPs from DNA instantiated successfully";
}

// +++++++ Specify Jnt2JntRelations -- TODO(ismail): Define rules for what kind of relationships can be done here. Is it linear relationship (linear lineage) or non-linear relationships. At the moment
// ................................... I'm using linear relationships +++++++++
void AlienModeller::specifyJnt2JntRelations_TypeGuided(Alien *alien, QVector<BodyPart *> &bodyParts, bool clearOldJnt2JntRelDS)
{
    qDebug() << "Specifying Jnt 2 Jnt relations";
    if (clearOldJnt2JntRelDS)
        alien->alienSkeleton.Jnt2JntRelations_TypeGuided.clear();

    QVector<QVector<int>> tempJntID_2D_List;
    for (int i=0; i<bodyParts.length(); i++)
    {
        qDebug() << bodyParts[i]->BP_Type;
        qDebug() << "reached 1";
        QVector<int> tempJntID_List;

        if (bodyParts[i]->BP_Type == AlienModeller::BodyPartsEnum::JOINT  ||  bodyParts[i]->BP_Type == AlienModeller::BodyPartsEnum::BONE)
            continue;

        tempJntID_List.resize(0);
        tempJntID_List.append(bodyParts[i]->BP_ID); // the first ID is always the BP_ID for that BP, which is not a joint or a bone
        for (int bp=0; bp<bodyParts[i]->subBodyParts.length(); bp++)
        {
            qDebug() << "reached 2";
            if (bodyParts[i]->subBodyParts[bp]->BP_Type == AlienModeller::BodyPartsEnum::JOINT)
            {
                // get the id of this joint and store it in a temporary list for later arrangement (linear or non-linear lineage). TODO(ismail): The kind of lineage should also be specified inside the DNA
                tempJntID_List.append(bodyParts[i]->subBodyParts[bp]->BP_ID);
                qDebug() << tempJntID_List;
            }
        }
        tempJntID_2D_List.append(tempJntID_List);
    }

    qDebug() << tempJntID_2D_List;

    // Now that we have all the joints IDs stored in the matrix, we need to put these in Jnt2JntRelations_TypeGuided using linear relationships
    for (int i=0; i<tempJntID_2D_List.length(); i++)
    {
        QPolygon *tempPoly = new QPolygon();
        for (int j=1; j<tempJntID_2D_List[i].length()-1; j++) // j starts at 1 because the first element is the BP_ID
        {
            // TODO(ismail): Potential bug here. tempJntID_2D_List[i][j+1] can lead to index out of bound bug. Inspect this
            tempPoly->append(QPoint(tempJntID_2D_List[i][j], tempJntID_2D_List[i][j+1]));
            qDebug() << tempJntID_2D_List[i][j] << " : " << tempJntID_2D_List[i][j+1];
        }
        alien->alienSkeleton.Jnt2JntRelations_TypeGuided.insert(tempJntID_2D_List[i][0], tempPoly);
        qDebug() << "\n";
    }

    // TODO(ismail): MEMORY MANAGEMENT: Need to destroy the tempJntID_2D_List to free up some memory

    qDebug() << "Jnt 2 Jnt relations specified successfully";
}

void AlienModeller::addMoreJointsToJnt2JntRelations_TypeGuided(Alien *alien, BodyPart *bpPtr, QVector<int> jntID_List)
{
    // First create a pair of the last joint in the existing Jnt2JntRel list and the first joint in the newly created joints (jntID_List)
    alien->alienSkeleton.Jnt2JntRelations_TypeGuided[bpPtr->BP_ID]->append(
                QPoint(alien->alienSkeleton.Jnt2JntRelations_TypeGuided.value(bpPtr->BP_ID)->last().y(), jntID_List[0]));
    if (jntID_List.length() > 1)
    {
        for (int j=0; j<jntID_List.length()-1; j++) // j starts at 1 because the first element is the BP_ID
        {
            // TODO(ismail): Potential bug here. tempJntID_2D_List[i][j+1] can lead to index out of bound bug. Inspect this
            alien->alienSkeleton.Jnt2JntRelations_TypeGuided[bpPtr->BP_ID]->append(QPoint(jntID_List[j], jntID_List[j+1]));
    //        qDebug() << tempJntID_2D_List[i][j] << " : " << tempJntID_2D_List[i][j+1];
        }
    }
}

void AlienModeller::initializeJointsArrangementTrendForCurrentAlien(Alien *alien, QVector<Joint *> &jointsList, bool clearOldVectorEnergyAngleTrends_TypeGuided,
                                                                    QPointF energyPt, QPointF anglePt)
{
//    JointsArrangementTrend tempObj; // This object is for a single alien
    if (clearOldVectorEnergyAngleTrends_TypeGuided)
        alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.VectorEnergyAngleTrends_TypeGuided.clear();

    for (int i=0; i<jointsList.length(); i++)
    {
        QPolygonF tempPolygon; tempPolygon.clear();
//        tempPolygon << QPointF(static_cast<float>(GeometryAlgorithms::getRandomInteger(0, 4)), GeometryAlgorithms::getRandomFloat(100.0, 150.0)) /*energy*/
//                    << QPointF(static_cast<float>(GeometryAlgorithms::getRandomInteger(0, 4)), GeometryAlgorithms::getRandomFloat(1.0, 360.0)) /*angle*/;

        if (energyPt == QPointF(-1,-1) && anglePt == QPointF(-1,-1))
        {
            tempPolygon << QPointF(-1.0, 75.0) /*starting energy trend and value ... try QPointF(-1.0, GeometricAlgorithms::getRandomFloat(50.f,100.f)*/
                        << QPointF(-1.0, 0.0) /*starting angle trend and value*/;
        }
        else
        {
            if (energyPt.y() < 10)
                energyPt.setY(75.f);
            tempPolygon << energyPt << anglePt;
        }

        alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.VectorEnergyAngleTrends_TypeGuided.insert(jointsList[i]->BP_ID, tempPolygon); // each joint's arrangement trend is stored in the list.
    }
//    jntsArrangeTrends.append(tempObj); // append this for the currently modelled alien. The list contains the trends for all the aliens

    qDebug() << "JATs initialized successfully";
}

void AlienModeller::compute_JAT_AND_VecLocCplForEachJnt_ForBP(Alien *alien, int BP_ID, QMap<int, QPolygonF> &tempCopy_BP_JAT_rootJoint_DNA,
                                                              QMap<int, int> &tempParentJntCompletionCheckList,
                                                              QMap<int,int> &tempChildJntPositionCheckList,
                                                              float newBP_startingX_Loc)
{
}

QPolygonF AlienModeller::getEnergyAngleTrendForJoint(Alien *alien, int jntID)
{
    QPolygonF tempPolyF; tempPolyF.clear();
    if (getBodyPartFromID(alien, jntID) != nullptr)
    {
        tempPolyF = alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.VectorEnergyAngleTrends_TypeGuided.value(jntID);
    }

    return tempPolyF;
}

void AlienModeller::buildAlienSkeleton_TypeGuided(Alien *alien)
{
    qDebug() << "Commencing the building of Alien Skeleton";
    // +++++++ Instantiate all BPs (from the DNA) +++++++++
    instantiateSkeletalBPsFromDNA_TypeGuided(alien);

    // Specify the BP's IDs in the BP_AngleArrangementsFromBackBone_DNA, cuz at the moment it will have (-1,angle)
    // TODO(ismail): Find a faster way of doing this and include it in a seperate method
    QMapIterator<int, QPolygon> angIter(alien->alienDNA->skeletonDNA->BP_AngleArrangementsFromBackBone_DNA);
    while (angIter.hasNext())
    {
        angIter.next();
        int i = angIter.key();
        QPolygon tempAnglePoly = angIter.value();
        QVector<int> tempBP_IDsList; tempBP_IDsList.clear();
        // get all the BPs of this type
        // TODO(ismail): Make a method that gets all BPs of a given type
        for (BodyPart *bpPtr : alien->bodyParts)
        {
            if (bpPtr->BP_Type != AlienModeller::BodyPartsEnum::JOINT
                    &&
                bpPtr->BP_Type != AlienModeller::BodyPartsEnum::BONE    )
            {
                if (bpPtr->BP_Type == i)
                {
                    tempBP_IDsList << bpPtr->BP_ID;
                }
            }
        }
        // Now do the updations
//        if (tempBP_IDsList.length() == tempAnglePoly.length())
//        {
            for (QPoint &pt : tempAnglePoly)
            {
                if (!tempBP_IDsList.isEmpty())
                {
                    pt.setX(tempBP_IDsList.first());
                    tempBP_IDsList.removeFirst();
                }
            }
            qDebug() << tempAnglePoly;
//        }
//        else
//        {
//            qDebug() << "number of BPs of this type are not equal to the entries in the BP_AngleArrangementsFromBackBone_DNA";
//            exit(1);
//        }
    }
    qDebug() << "BP_AngleArrangementsFromBackBone_DNA is up to date";

    // +++++++ Instantiate the number of joints for all BPs (from the DNA) ++++++++
    instantiateNumOfJointsForAllBPs_TypeGuided(alien, alien->alienDNA->skeletonDNA->numOfJointsPerBP_DNA);

    // +++++++ Specify Jnt2JntRelations for Type Guided +++++++
    // TODO(ismail): Define rules for what kind of relationships can be done here. Is it linear relationship (linear lineage) or non-linear relationships.
    // .................................................... At the moment I'm using linear relationships +++++++++
    specifyJnt2JntRelations_TypeGuided(alien, alien->bodyParts);

    // initialize the JointsArrangementTrend for this alien with dummy values.
    initializeJointsArrangementTrendForCurrentAlien(alien, alien->alienJoints);

    // +++++++ Generate All the joints from the Jnt2JntRelations and arrange these based on JAT DNA. Generate means creating the cells for these joints +++++++
    QMap<int,int> tempParentJntCompletionCheckList; // checklist for when the all the child joints's positions are computed. Value(int) is 1 for DONE and 0 for NOT_DONE.
    QMap<int,int> tempChildJntPositionCheckList;    // checklist for when the joint's position is computed.
                                                    // ... TODO(ismail): Perhaps use enum or macro for these values
    alien->alienSkeleton.vectorLocationCoupleForEachJoint_TypeGuided.clear();
    for (int i=0; i<alien->alienJoints.length(); i++)
    {
        QPair<QVector3D, QVector3D> tempPair;
        tempPair.first = QVector3D(0.0,0.0,1.0); tempPair.second = QVector3D(0.0,0.0,0.0);
        alien->alienSkeleton.vectorLocationCoupleForEachJoint_TypeGuided.insert(alien->alienJoints[i]->BP_ID, tempPair);

        tempParentJntCompletionCheckList.insert(alien->alienJoints[i]->BP_ID,0);
        tempChildJntPositionCheckList.insert(alien->alienJoints[i]->BP_ID,0);
    }

    // create a temporary copy of BP_JAT_rootJoint_DNA
    QMap<int, QPolygonF> tempCopy_BP_JAT_rootJoint_DNA = QMap<int, QPolygonF>(alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.BP_JAT_rootJoint_DNA);

//    float newBP_startingX_Loc = -300.0;
    QMapIterator<int, QPolygon*> tempIter(alien->alienSkeleton.Jnt2JntRelations_TypeGuided);
    while (tempIter.hasNext())
    {
        tempIter.next();
        int tempBP_ID = tempIter.key();

        compute_JAT_AND_VecLocCplForEachJnt_ForBP(alien, tempBP_ID, tempCopy_BP_JAT_rootJoint_DNA, tempParentJntCompletionCheckList, tempChildJntPositionCheckList,
                                                  0.0);

//        newBP_startingX_Loc += 50.0;
    }

    qDebug() << "compute_JAT_AND_VecLocCplForEachJnt_ForBP() done";

    // TODO(ismail): Perhaps create a new method modelAllBPs_TypeGuided() and call it here and within this method, model the
    //              ... joints and bones specific to that BP
    modelAllAlienJoints_TypeGuided(alien, alien->alienJoints);
    modelAllAlienBones_TypeGuided(alien, alien->alienSkeleton.Jnt2JntRelations_TypeGuided);

    // Now apply scalings (relative scalings using the BP_SizeComparedToBackBone_DNA)
//    scaleAllBPsOfGivenType(AlienModeller::BodyPartsEnum::LOWER_LIMB);

    // Bone Bulkification
    // The first step is to construct/update the Skeletal_BP_Bulkiness_DNA from the Skeletal_BP_Bulkiness_RS.
    // ... We have deferred it to this stage as each entry of this DNA DS has the BP_ID as the key(int)
    // Now perform the bulking up effects using the Skeletal_BP_Bulkiness_2_DNA
    qDebug() << "Bone Bulkification";
    QVector<int> BP_ID_List; BP_ID_List.resize(0);
    QVector<int> tempSepBP_IDs; tempSepBP_IDs.clear();
    getAllBP_ID_List(alien, BP_ID_List, tempSepBP_IDs);

    buildSkeletalBP_BulkinessDNA(alien, BP_ID_List);
//    qDebug() << alien->alienDNA->skeletonDNA->Skeletal_BP_Bulkiness_DNA_ForEachBP.keys();
//    qDebug() << BP_ID_List;
    bulkUpBodyPart(alien, BP_ID_List);

    // Now bevel the BPs
    // for (int i=0; i<alien->bodyParts.length(); i++)
    // {
    //     if (alien->bodyParts[i]->BP_Type != AlienModeller::BodyPartsEnum::JOINT  &&
    //             alien->bodyParts[i]->BP_Type != AlienModeller::BodyPartsEnum::BONE)
    //         bevelBP(alien, alien->bodyParts[i]);
    // }

/*
    // Bone Bending
    if (alien->clonedFrom == -1)
        updateBoneBendingDNA(alien);
    else if (alien->alienDNA->skeletonDNA->BoneBending_DNA.size() == 0)// clone the BoneBending_DNA DNA
        alien->alienDNA->skeletonDNA->BoneBending_DNA = QMap<int, QPolygonF>(sourceAlien->alienDNA->skeletonDNA->BoneBending_DNA);

    for (int i=0; i<alien->bodyParts.length(); i++)
    {
        if (alien->bodyParts[i]->BP_Type == AlienModeller::BodyPartsEnum::LOWER_LIMB)
        {
            for (int j=0; j<alien->bodyParts[i]->subBodyParts.length(); j++)
            {
                if (alien->bodyParts[i]->subBodyParts[j]->BP_Type == AlienModeller::BodyPartsEnum::BONE)
                {
                    bendBone(alien, dynamic_cast<Bone*>(alien->bodyParts[i]->subBodyParts[j]));
//                    break;
                }
            }
        }
    }
*/

    // +++++ Test for BP scaling. (Performing successfully) +++++
//    for (int bp=0; bp<this->alienList.last()->bodyParts.length(); bp++)
//    {
//        if (this->alienList.last()->bodyParts[bp]->BP_Type != AlienModeller::BodyPartsEnum::JOINT  &&  this->alienList.last()->bodyParts[bp]->BP_Type != AlienModeller::BodyPartsEnum::BONE)
//        {
//            GeometryAlgorithms::scaleBP(*(this->alienList.last()->bodyParts[bp]), 3, 3, 3);
////            break;
//        }
//    }
    // +++++ Test for BP scaling. (Performing successfully) +++++
//    for (int bp=0; bp<this->alienList.last()->alienBackBones.length(); bp++)
//    {
//        BodyPart &tempBackBone = *(this->alienList.last()->alienBackBones[bp]);
//        GeometryAlgorithms::scaleBP(tempBackBone, 3, 3, 2);
//    }

//    // +++++ Test for BP rotations (Performing okay) +++++
//    for (int bp=0; bp<this->alienList.last()->alienBackBones.length(); bp++)
//    {
//        GeometryAlgorithms::rotateBodyPartTowardsVecDir(*(this->alienList.last()->alienBackBones[bp]), QVector3D(1,1,1));
//    }

//    // +++++ Test for BP tranlation (Performing successfully) +++++
//    for (int bp=0; bp<this->alienList.last()->alienBackBones.length(); bp++)
//    {
//        GeometryAlgorithms::translateBP(*(this->alienList.last()->alienBackBones[bp]), 500, 500, 500);
//    }


    // +++++++ Arrange all BPs so that they are attached to their designated locations of the BackBone. TODO(ismail): Implement more geometric algorithms that can transform an entire BP

    qDebug() << "Alien Skeleton built successfully";
}

void AlienModeller::findPointsAndVectorsOnBackBoneForBP_Attachments(Alien *alien, int Dest_BP_ID,
                                                                      int sourceBP_Type, int pivotZoneID_FromDNA,
                                                                      float scatteringValue_FromDNA, int distributionTrend_FromDNA,
                                                                      int numOfAttachmentPointsToSelect, QVector<QVector3D> &attachmentPnts,
                                                                      QVector<QVector3D> &attachmentVectors /*QMap<int, QVector3D> &attachmentVectors*/, QVector<int> &jntLocPairsList)
{
/*
 * ++++++ ALGORITHM +++++++
 *
 * input:   BP_ID - BackBone
 *          pivot_jnt (zone_jnt) --- taken from DNA
 *          number of points to find where the BPs will be attached --- taken from DNA
 *          scattering value (%age) --- taken from DNA
 *          Boolean: attach some BPs on the same level / not --- taken from DNA
 *          int distributionTrend_FromDNA (either 1 or 2). 1 ==> points to be found on all faces except FRONT and BACK (octapedals, aquatics)
 *                                                     ... 2 ==> points to be found on only LEFT and RIGHT faces (bipeds, quadpeds, ambhis, anthropods)
 *                                                     // TODO(ismail): use an unsigned short for these values or better still use a boolean
 *
 *
 *  Loop_1: from the pivot_jnt, get all the joints that are on either sides of it (store them in a temporary 2D list)
 *
 *  check the scattering %age value
 *
 *  From the pivot_jnt, count the number of joints on either sides
 *
 *  count the number of joints that correspond to %age of the total joints on either sides of the pivot_jnt. These are the attachment joints
 *
 *  store these attachement joints in a temporary list
 *
 *  distribute the attachment points to all the attachement joints. Distribution algorithm...
 *      DISTRIBUTION ALGORITHM:
 *              1 - Run a loop through all the number of attachment points
 *              2 - Allocate each attachment joint to 1 attachement point
 *              3 - if the attachment joints have exhausted, start over again from the first attachment joint and GOTO step 1
 *              4 - After all the attachement points have been a allocated, arrange these in a regular fashion on the appropriate face of the cell box of the attachement joint
 *
 *  For all the stored joints, specify points on either the RIGHT or LEFT face.
 *
 *  Based on the (attach some BPs on the same level / not) value, randomly select some points that are on the same level of an attachement joint.
 *  ... These points can be selected on the different faces i.e. TOP, RIGHT, BACK etc. but not on the FRONT or BACK faces
 *
*/
    // Get the BP_TYPE_ID from the sourceBP_Type
    QPolygon tempAngleArrangementsPoly = alien->alienDNA->skeletonDNA->BP_AngleArrangementsFromBackBone_DNA.value(sourceBP_Type);

    // Get the exact joint id from the backbone which correspond to the zone id specified in tempPair.first from DNA
    QPolygon *tempPolyPtr = alien->alienSkeleton.Jnt2JntRelations_TypeGuided.value(Dest_BP_ID);
    for (int i=0; i<tempPolyPtr->length(); i++)
    {
        qDebug() << "(" << tempPolyPtr->at(i).x() << "," << tempPolyPtr->at(i).y() << ")";
    }

//    qDebug() << *tempPolyPtr; // this works for printing the entire list but its a messy formatting

    // Now its better to lay it out into a list of jnt IDs


    int tempPvtZoneJntID;

    switch (pivotZoneID_FromDNA)
    {
    case 1:
        tempPvtZoneJntID = tempPolyPtr->last().y();
        break;

    case 2:
        tempPvtZoneJntID = tempPolyPtr->at(0).x();
        break;

    case 3:
        tempPvtZoneJntID = tempPolyPtr->at( (tempPolyPtr->length() - (tempPolyPtr->length())/2)/2 + (tempPolyPtr->length())/2 ).x();
        break;

    case 4:
        tempPvtZoneJntID = tempPolyPtr->at( (tempPolyPtr->length())/2 ).x();
        break;

    case 5:
        tempPvtZoneJntID = tempPolyPtr->at( ((tempPolyPtr->length())/2)/2 ).x();
        break;

    default:
        qDebug() << "zone ID incorrect";
        exit(0);
    }

    qDebug() << "pivotZoneID_FromDNA : " << pivotZoneID_FromDNA;
    qDebug() << "tempPvtZoneJntID : " << tempPvtZoneJntID;

    // Lets consider that in Jnt2JntRelations_TypeGuided, the root joint is always the 'end(zone)' joint and the last joint in the lineage is the 'start(zone)' joint
    QVector<QVector<int>> tempBothSidesJointsList(2, QVector<int>(/*size=*/0));  // This is a 2xN list, where N can be the number of joints on either sides of the pivot joint.
                                                    // The first list contains all the joints towards the end zone joint. The second list contains all those joints towards the
                                                    // ... start zone joint.
                                                    // The first element in both of these lists is the pivotJntID.
                                                    // IMPORTANT: I can make use of a 2D Vector DS (tempJntID_2D_List) that was temporarily constructed inside
                                                    // ... specifyJnt2JntRelations_TypeGuided() method. That will get rid of the code below. However having a DS like that
                                                    // ... for so many aliens might be costly for memory

    // Firstly, fill the first list of of tempBothSidesJointsList DS. This list needs to be traversed in reverse order later on
    int i=0;
    for (i=0; i<tempPolyPtr->length(); i++)
    {
        // find the pivot joint
        if (tempPolyPtr->at(i).x() != tempPvtZoneJntID  &&  tempPolyPtr->at(i).y() != tempPvtZoneJntID)
        {
            tempBothSidesJointsList[0].append(tempPolyPtr->at(i).x());
        }
        else if (tempPolyPtr->at(i).y() == tempPvtZoneJntID)
        {
            tempBothSidesJointsList[0].append(tempPolyPtr->at(i).x());
            break;
        }
        else if (tempPolyPtr->at(i).x() == tempPvtZoneJntID)
        {
            tempBothSidesJointsList[1].append(tempPolyPtr->at(i).y());
            break;
        }
    }

    // Now fill the second list of tempBothSidesJointsList DS
    for (int j=i+1; j<tempPolyPtr->length(); j++)
    {
        tempBothSidesJointsList[1].append(tempPolyPtr->at(j).y());
    }

    // count the number of joints that correspond to %age of the total joints on either sides of the pivot_jnt. These are the attachment joints
    qDebug() << "scatteringValue_FromDNA :" << scatteringValue_FromDNA;
    int perc_1 = ceil(scatteringValue_FromDNA/100.0 * static_cast<float>(tempBothSidesJointsList[0].length()));
    int perc_2 = ceil(scatteringValue_FromDNA/100.0 * static_cast<float>(tempBothSidesJointsList[1].length()));

    qDebug() << "perc_1 :" << perc_1 << " and perc_2 : " << perc_2;

    // store these attachement joints in a temporary combined list
    QVector<int> tempAttachmentJnts; tempAttachmentJnts.clear();

    // first store joints from the first list in reverse order
    for (int i=0, j=tempBothSidesJointsList[0].length()-1; i<perc_1 && j>=0; i++)
    {
        tempAttachmentJnts.append(tempBothSidesJointsList[0][j--]);
    }

    // then store joints from the second list in forward order
    for (int i=0; i<perc_2; i++)
    {
        tempAttachmentJnts.append(tempBothSidesJointsList[1][i]);
    }

    qDebug() << tempAttachmentJnts;

    QVector<int> tempFaceIndList; tempFaceIndList.resize(0);
    tempFaceIndList << Box::BoxFace::LEFT << Box::BoxFace::RIGHT << Box::BoxFace::TOP << Box::BoxFace::BOTTOM;
    qDebug() << tempFaceIndList;
    int numOfFacesToConsider = 0;
    if (distributionTrend_FromDNA == 1)
        numOfFacesToConsider = tempFaceIndList.length();
    else
        numOfFacesToConsider = 2;

    qDebug() << "numOfAttachmentPointsToSelect size : " << numOfAttachmentPointsToSelect;

    // distribute the attachment points to all the attachement joints. Distribution algorithm...
    int prevLen = 0; // this will help keep track of how many points are added for each joint
    for (int i=0; i<tempAttachmentJnts.length(); i++)
    {
        int sourceJnt = tempAttachmentJnts[i];
        for (int j=0; j<tempPolyPtr->length(); j++)
        {
            if (tempAttachmentJnts[i] == tempPolyPtr->at(j).y())
            {
                sourceJnt = tempPolyPtr->at(j).x();
                break;
            }
        }

        // get the joint reference from the joint list
        Joint *tempJntPtr=NULL, *tempSourceJntPtr=NULL;

        // What is needed: faces of attachment joints, angles from DNA, the distribution trend (either depth first or breadth first??)
        tempJntPtr = dynamic_cast<Joint*>(getBodyPartFromID(alien, tempAttachmentJnts[i]));
        tempSourceJntPtr = dynamic_cast<Joint*>(getBodyPartFromID(alien, sourceJnt));

        // Find the flow vector
        QVector3D flowVec;
        if (tempAttachmentJnts[i] == sourceJnt)
            flowVec = tempSourceJntPtr->getPivotVector();
        else
            flowVec =  tempSourceJntPtr->getPivotPoint() - tempJntPtr->getPivotPoint();

        // now select its left and right faces and send it to computeRandomPointsOnFace() method for processing
        Box *tempBoxPtr = tempJntPtr->cellList[0]->cellBox;
        // check the distribution trend

        prevLen = attachmentPnts.length();
        // prepare the tempFaceVertices list
        QVector<QVector3D> tempFaceVertices;
        for (int nf=0; nf<numOfFacesToConsider && attachmentPnts.length() < numOfAttachmentPointsToSelect; nf++)
        {
            // Use computeRandomPointsOnFace method to find a point on the face
            tempFaceVertices.resize(0);
            for (int k=0; k<tempBoxPtr->faces[tempFaceIndList[nf]]->vertNorIndices.length()-1 /*4*/; k++) // TODO(ismail): this can me made faster as we already know there are 5 elements
                                                                                                         // ... in the vertNorIndices list
            {
                tempFaceVertices.append(tempBoxPtr->vertices[tempBoxPtr->faces[tempFaceIndList[nf]]->vertNorIndices[k]]);
    //            qDebug () << tempFaceVertices[j];
            }
            computeRandomPointsOnFace(tempFaceVertices, 1, attachmentPnts);    // TODO(ismail): additional improvement. The points on the same face should not be too close to each other.
                                                                               // ... Perhaps include a check if they are and then move them apart
//            // Find the attachment vectors computed as clones of the vector starting from previous joint to this joint
//            attachmentNormalVectors.append(tempBoxPtr->normals[tempBoxPtr->faces[tempFaceIndList[nf]].vertNorIndices[4]]);

            qDebug() << attachmentPnts.last();
            if (i == tempAttachmentJnts.length()-1)
            {
                i=-1; // reset i so it starts from the first attachement joint again
                qDebug() << "i has been resetted";
            }
        }
        for (int pp=prevLen; pp<attachmentPnts.length(); pp++)
            jntLocPairsList << tempJntPtr->BP_ID;

        // Now we need to compute the new attachment vectors which are rotated at an angle (specified in the DNA) from the flow vector (flowVec) above
        for (int attP=0; attP<attachmentPnts.length(); attP++)
        {
            if (!tempAngleArrangementsPoly.isEmpty())
            {
                // TODO(ismail): Make it a method that will rotate any given vector according to given angles,
                // ... or given direction
                QQuaternion tempQuat = QQuaternion::fromEulerAngles(tempAngleArrangementsPoly.first().y(), tempAngleArrangementsPoly.first().y(), 0);
//                attachmentVectors.insert(tempAngleArrangementsPoly.first().x(),tempQuat.rotatedVector(flowVec).normalized());
                attachmentVectors.append(tempQuat.rotatedVector(flowVec));

                tempAngleArrangementsPoly.removeFirst();
                qDebug() << "new attachment vector computed from tempAngleArrangementsPoly";
            }
            else
            {
                qDebug() << "tempAngleArrangementsPoly is empty";
                break;
            }
        }
        if (attachmentPnts.length() == numOfAttachmentPointsToSelect)
        {
            break;
        }
    }
}

void AlienModeller::findPointsAndVectorsOnDestBPForSourceBPsAttachments(Alien *alien, int destBP_ID,
                                                                          int pivotZoneID_FromDNA, float scatteringValue_FromDNA,
                                                                          int groupingTrendFromDNA, int numOfAttachmentPointsToSelect,
                                                                          QVector<QVector3D> &attachmentPnts, QVector<QVector3D> &attachmentVectors,
                                                                          QVector<int> &boneLocPairsList, QVector<int> faceList)
{

    // ** Get the exact bone id from the dest BP which correspond to the zone id specified in tempPair.first from DNA
    BodyPart *destBP_Ptr = this->getBodyPartFromID(alien, destBP_ID);
    QVector<Bone*> tempBoneList; tempBoneList.clear();
    // Get all the Bones in order
    if (!this->getAllBP_BonesInOrder(alien, destBP_Ptr, tempBoneList))
        exit(0);

    // Get all the Bone IDs in order
    QVector<int> tempBoneIDsInOrderList; tempBoneIDsInOrderList.clear();
    for (Bone *bPtr : tempBoneList)
        tempBoneIDsInOrderList << bPtr->BP_ID;

    int tempPvtZoneBoneID;
    switch (pivotZoneID_FromDNA)
    {
    case 0:
        tempPvtZoneBoneID = tempBoneIDsInOrderList.last();
        break;

    case 1:
        tempPvtZoneBoneID = tempBoneIDsInOrderList.at(0);
        break;

    case 2:
        tempPvtZoneBoneID = tempBoneIDsInOrderList.at( (tempBoneIDsInOrderList.length() - (tempBoneIDsInOrderList.length())/2)/2 + (tempBoneIDsInOrderList.length())/2 );
        break;

    case 3:
        tempPvtZoneBoneID = tempBoneIDsInOrderList.at( (tempBoneIDsInOrderList.length())/2 );
        break;

    case 4:
        tempPvtZoneBoneID = tempBoneIDsInOrderList.at( ((tempBoneIDsInOrderList.length())/2)/2 );
        break;

    default:
        qDebug() << "zone ID incorrect";
        exit(0);
    }

    qDebug() << "pivotZoneID_FromDNA : " << pivotZoneID_FromDNA;
    qDebug() << "tempPvtZoneBoneID : " << tempPvtZoneBoneID;

    // ** Now find all the bones that are along the %age (number provided) on both the sides of the pivot bone
    // Lets consider that in tempBoneIDsInOrderList, the first bone is always the 'end(zone)' bone and the last bone in the lineage is the 'start(zone)' bone
    QVector<QVector<int>> tempBothSidesBonesList(2, QVector<int>(/*size=*/0));  // This is a 2xN list, where N can be the number of bones on either sides of the first bone.
                                                    // The first list contains all the bones towards the end zone bone. The second list contains all those bones towards the
                                                    // ... start zone bone.
                                                    // The first element in both of these lists is the pivotJntID.
                                                    // IMPORTANT: I can make use of a 2D Vector DS (tempJntID_2D_List) that was temporarily constructed inside
                                                    // ... specifyJnt2JntRelations_TypeGuided() method. That will get rid of the code below. However having a DS like that
                                                    // ... for so many aliens might be costly for memory

    // Firstly, fill the first list of tempBothSidesBonesList DS. This list needs to be traversed in reverse order later on
    int i=0;
    for (i=0; i<tempBoneIDsInOrderList.length(); i++)
    {
        // find the pivot bone
        if (tempBoneIDsInOrderList.at(i) != tempPvtZoneBoneID)
            tempBothSidesBonesList[0].append(tempBoneIDsInOrderList.at(i));
        else
        {
            tempBothSidesBonesList[0].append(tempBoneIDsInOrderList.at(i));
            break;
        }
    }

    // Now fill the second list of tempBothSidesBonesList DS
    for (int j=i+1; j<tempBoneIDsInOrderList.length(); j++)
    {
        tempBothSidesBonesList[1].append(tempBoneIDsInOrderList.at(j));
    }

    // count the number of bones that correspond to %age of the total bones on either sides of the pivot_jnt. These are the attachment bones
    qDebug() << "scatteringValue_FromDNA :" << scatteringValue_FromDNA;
    int perc_1 = ceil(scatteringValue_FromDNA/100.0 * static_cast<float>(tempBothSidesBonesList[0].length()));
    int perc_2 = ceil(scatteringValue_FromDNA/100.0 * static_cast<float>(tempBothSidesBonesList[1].length()));

    qDebug() << "perc_1 :" << perc_1 << " and perc_2 : " << perc_2;

    // store these attachement joints in a temporary combined list
    QVector<int> tempAttachmentBones;

    // first store bones from the first list in reverse order
    for (int i=0, j=tempBothSidesBonesList[0].length()-1; i<perc_1 && j>=0; i++)
        tempAttachmentBones.append(tempBothSidesBonesList[0][j--]);

    // then store bones from the second list in forward order
    for (int i=0; i<perc_2; i++)
        tempAttachmentBones.append(tempBothSidesBonesList[1][i]);

    qDebug() << tempAttachmentBones;

    // ** Distribute the attachment points to all the attachement bones. Distribution algorithm...
    QVector<int> tempFaceIndList; tempFaceIndList.resize(0);
    // TODO: check if faceList contains any number other than the face IDs
    tempFaceIndList << faceList;
    qDebug() << tempFaceIndList;
    tempFaceIndList.removeAll(-1);
    int numOfFacesToConsider = tempFaceIndList.length();
    if (!tempFaceIndList.isEmpty()) // Shuffle the face indices
    {
        // TODO: make a method for shuffling the elements in a list
        // ... THis is probably a slow algorithm. perhaps use one of the std::algorithms
        for (int i=0; i<tempFaceIndList.length(); i++) // shuffle all the elements
        {
            int randInd = GeometryAlgorithms::getRandomInteger(0, tempFaceIndList.length());
            tempFaceIndList.prepend(tempFaceIndList.at(randInd));
            tempFaceIndList.removeAt(randInd+1);
        }
    }
    else
        tempFaceIndList.append(Box::BoxFace::LEFT);

//    if (distributionTrend_FromDNA == 1)
//        numOfFacesToConsider = tempFaceIndList.length();
//    else
//        numOfFacesToConsider = 1;

    qDebug() << "numOfAttachmentPointsToSelect size : " << numOfAttachmentPointsToSelect;

    // Initialize a list of angles for the attachment vectors.
    // TODO: Angles should come from the DNA
    QPolygon tempAngleArrangementsPoly; tempAngleArrangementsPoly.clear(); // QPoint here is (pitch, yaw) angles
    for (int i=0; i<numOfAttachmentPointsToSelect; i++)
    {
        tempAngleArrangementsPoly << QPoint(GeometryAlgorithms::getRandomInteger(5, 85), GeometryAlgorithms::getRandomInteger(5, 85));
    }
    int prevLen = 0; // this will help keep track of how many points are added to for each bone
    for (int i=0; i<tempAttachmentBones.length(); i++)
    {
        int tempSourceBoneID = tempAttachmentBones[i];
//        for (int j=0; j<tempBoneIDsInOrderList.length(); j++)
//        {
//            if (tempAttachmentBones[i] == tempBoneIDsInOrderList.at(j))
//            {
//                tempSourceBoneID = tempBoneIDsInOrderList.at(j);
//                break;
//            }
//        }
        qDebug() << "tempSourceBoneID : " << tempSourceBoneID;

        // get the Bone reference from the joint list
        Bone *tempBonePtr=NULL, *tempSourceBonePtr=NULL;

        // What is needed: faces of attachment joints, angles from DNA, the distribution trend (either depth first or breadth first??)
        tempBonePtr = dynamic_cast<Bone*>(getBodyPartFromID(alien, tempAttachmentBones[i]));
        tempSourceBonePtr = dynamic_cast<Bone*>(getBodyPartFromID(alien, tempSourceBoneID));

        // Find the flow vector. This flow vector is always the normal vector of the face
        // The flow vector is then rotated according to the Euler angles
        QVector3D flowVec;
//        if (tempAttachmentBones[i] == tempSourceBoneID)
        flowVec = tempSourceBonePtr->getPivotVector();
//        else
//            flowVec =  tempSourceBonePtr->BP_PivotPt - tempBonePtr->BP_PivotPt;

        // now select its left and right faces and send it to computeRandomPointsOnFace() method for processing
        Box *tempBoxPtr = tempBonePtr->cellList[0]->cellBox;

        // check the distribution trend

        // prepare the tempFaceVertices list
        QVector<QVector3D> tempFaceVertices;
        prevLen = attachmentPnts.length();
        for (int nf=0; nf<numOfFacesToConsider && attachmentPnts.length() < numOfAttachmentPointsToSelect; nf++)
        {
            // Use computeRandomPointsOnFace method to find a point on the face
            // TODO(ismail): Make a mehtod that takes in input number of points to find the number of faces of the box
            // ... to consider for finding the faces. This can be a nice method called findPointsOnBox(boxPtr, numOfPntsToFind, numOfFacesToConsider)
            // get all the vertices of the face
            tempFaceVertices.resize(0);
            for (int k=0; k<tempBoxPtr->faces[tempFaceIndList[nf]]->vertNorIndices.length()-1 /*4*/; k++) // TODO(ismail): this can me made faster as we already know there are 5 elements
                                                                                                         // ... in the vertNorIndices list
            {
                if (tempFaceIndList[nf] == -1)
                    continue;
                tempFaceVertices.append(tempBoxPtr->vertices[tempBoxPtr->faces[tempFaceIndList[nf]]->vertNorIndices[k]]);
    //            qDebug () << tempFaceVertices[j];
            }
            // flowVec = tempBoxPtr->normals[tempBoxPtr->faces[tempFaceIndList[0]]->vertNorIndices[4]];
            computeRandomPointsOnFace(tempFaceVertices, 1, attachmentPnts);    // TODO(ismail): additional improvement. The points on the same face should not be too close to each other.
                                                                               // ... Perhaps include a check if they are and then move them apart
//            // Find the attachment vectors computed as clones of the vector starting from previous joint to this joint
//            attachmentNormalVectors.append(tempBoxPtr->normals[tempBoxPtr->faces[tempFaceIndList[nf]].vertNorIndices[4]]);

            qDebug() << attachmentPnts.last();
            if (i == tempAttachmentBones.length()-1)
            {
                i=-1; // reset i so it starts from the first attachement joint again
                qDebug() << "i has been resetted";
            }
        }
        for (int pp=prevLen; pp<attachmentPnts.length(); pp++)
            boneLocPairsList << tempBonePtr->BP_ID;

        // Now we need to compute the new attachment vectors which are rotated at an angle (specified in the DNA) from the flow vector (flowVec) above
        for (int attP=0; attP<attachmentPnts.length(); attP++)
        {
            if (!tempAngleArrangementsPoly.isEmpty())
            {
                // TODO(ismail): Make it a method that will rotate any given vector according to given angles,
                // ... or given direction
                ///////////////
                QQuaternion tempQuat = QQuaternion::fromEulerAngles(tempAngleArrangementsPoly.first().x(), tempAngleArrangementsPoly.first().y(), 0);
                attachmentVectors.append(tempQuat.rotatedVector(flowVec));
                ///////////////

                tempAngleArrangementsPoly.removeFirst();
                qDebug() << "new attachment vector computed from tempAngleArrangementsPoly";
            }
            else
            {
                qDebug() << "tempAngleArrangementsPoly is empty";
                break;
            }
        }
        if (attachmentPnts.length() == numOfAttachmentPointsToSelect)
        {
            break;
        }
    }

    tempFaceIndList.clear();
}

void AlienModeller::attachAllSkeletalBodyPartsToBackBone(Alien *alien)
{
   alien->rootBP = alien->alienBackBones.last();
    /*
     * ++++++ ALGORITHM +++++++
     *
     * input:   attachement points on the joints of the BB
     *          attachement vectors i.e. the vectors at which the attaching BPs will be attached to the BB
     *          List of BPs to be attached
     *
     *
     *  rotate the attaching BP towards the attachement vector
     *
     *  translate the BP to the attachment point on the BB
    */

    // get the backbone ID
    int tempBackBone_ID = alien->alienBackBones.last()->BP_ID;

    // TODO(ismail): ensure all the bodyparts to be attached are placed back at the origin and unrotated
    // This is so that the attachements happen correctly, otherwise attachments will not happen properly

    // First attach the alienLowerLimbs
    QVector<int> sourceBP_ID_List;
    sourceBP_ID_List.resize(0);
    QVector<QVector3D> attPnts; attPnts.clear();
    QVector<QVector3D> attVecs; attVecs.clear();
    QVector<int> housingJntsList; housingJntsList.clear();
    if (!alien->alienLowerLimbs.empty())
    {
        for (int i=0; i<alien->alienLowerLimbs.length(); i++)
        {
            sourceBP_ID_List << alien->alienLowerLimbs[i]->BP_ID;
        }

        attachBodyPartToAnotherBodyPart(alien, sourceBP_ID_List, tempBackBone_ID, false, QVector<QVector3D>(0), QVector<QVector3D>(0), QVector<int>(0));
    }

    // Now attach the alienUpperLimbs
    sourceBP_ID_List.resize(0);
    attPnts.clear();
    attVecs.clear();
    if (!alien->alienUpperLimbs.empty())
    {
        for (int i=0; i<alien->alienUpperLimbs.length(); i++)
        {
            sourceBP_ID_List << alien->alienUpperLimbs[i]->BP_ID;
        }

        attachBodyPartToAnotherBodyPart(alien, sourceBP_ID_List, tempBackBone_ID, false, QVector<QVector3D>(0), QVector<QVector3D>(0), QVector<int>(0));
    }

    // Now attach the Heads
    sourceBP_ID_List.resize(0);
    if (!alien->alienHeads.empty())
    {
        for (int i=0; i<alien->alienHeads.length(); i++)
        {
            sourceBP_ID_List << alien->alienHeads[i]->BP_ID;
        }

        attachBodyPartToAnotherBodyPart(alien, sourceBP_ID_List, tempBackBone_ID, true, QVector<QVector3D>(0), QVector<QVector3D>(0), QVector<int>(0));
    }
}

void AlienModeller::detachAllSubBPsFromParentBPs(Alien *alienPtr, BodyPart *parentBP_Ptr, QVector<BodyPart *> &subBPsList)
{
    for (BodyPart *bp : parentBP_Ptr->subBodyParts)
    {
        if (bp->BP_Type != AlienModeller::BodyPartsEnum::JOINT
                &&
            bp->BP_Type != AlienModeller::BodyPartsEnum::BONE)
        {
            subBPsList.append(bp);
            qDebug() << bp->BP_Type << " appended to sub bps";
           // parentBP_Ptr->subBodyParts.removeOne(bp);
            // also update the BP_AttachmentToOtherBP_withAttachPositionAndVector_DNA
            QVector3D pos(0,0,0); QVector3D vec(0,0,0); // dummy values
            updateBP_AttachmentToOtherBP_withAttachPositionAndVector_DNA(alienPtr, bp->BP_ID, parentBP_Ptr->BP_ID, -1, pos, vec);
        }
    }
}

void AlienModeller::updateGroupBP_AttachmentToAnotherBP_ZoneScatteringTrend_DNA(Alien *alienPtr, int groupBPsTypeID, int destBP_ID,
                                                                                bool toAddOrNot, QPolygon inputDNA_Poly)
{
    // TODO(ismail): Why are we clearing the map when there is a removal below? CHECK!
    alienPtr->alienDNA->skeletonDNA->GroupBP_AttachmentToAnotherBP_ZoneScatteringTrend_DNA.clear(); // TODO: Please remove this
    QPolygon tempRS_Poly = skeletonRuleSet->GroupBP_AttachmentToAnotherBP_ZoneScatteringTrend_RS;
    QPair<int,int> tempKey = QPair<int,int>(groupBPsTypeID,destBP_ID);
    // If we need a new entry, then do the following
    if (toAddOrNot) // build and add a new entry
    {
        if (inputDNA_Poly.isEmpty())
        {
            QPolygon tempPoly; tempPoly.clear();
            // now add 3 points to it
            tempPoly << QPoint(GeometryAlgorithms::getRandomInteger(tempRS_Poly[1].x(), tempRS_Poly[1].y()), GeometryAlgorithms::getRandomInteger(tempRS_Poly[0].x(), tempRS_Poly[0].y()))
                     << QPoint(GeometryAlgorithms::getRandomInteger(tempRS_Poly[2].x(), tempRS_Poly[2].y()), GeometryAlgorithms::getRandomInteger(tempRS_Poly[3].x(), tempRS_Poly[3].y()))
                     << QPoint(GeometryAlgorithms::getRandomInteger(tempRS_Poly[4].x(), tempRS_Poly[4].y()), GeometryAlgorithms::getRandomInteger(tempRS_Poly[4].x(), tempRS_Poly[4].y()))
                     << QPoint(Box::BoxFace::LEFT,Box::BoxFace::RIGHT)
                     << QPoint(Box::BoxFace::TOP,Box::BoxFace::BOTTOM)
                     << QPoint(Box::BoxFace::FRONT,-1) ;

            // now insert this new entry to the DNA DS
            alienPtr->alienDNA->skeletonDNA->GroupBP_AttachmentToAnotherBP_ZoneScatteringTrend_DNA.insert(tempKey,tempPoly);
        }
        else
            alienPtr->alienDNA->skeletonDNA->GroupBP_AttachmentToAnotherBP_ZoneScatteringTrend_DNA.insert(tempKey,inputDNA_Poly);

        qDebug() << "New GroupBP attachment entry added from GroupBP_AttachmentToAnotherBP_ZoneScatteringTrend_DNA";
    }    
    else // remove the entry
    {
        if (alienPtr->alienDNA->skeletonDNA->GroupBP_AttachmentToAnotherBP_ZoneScatteringTrend_DNA.remove(tempKey))
            qDebug() << "GroupBP attachment entry removed from GroupBP_AttachmentToAnotherBP_ZoneScatteringTrend_DNA";
    }
}

QVector<int> AlienModeller::generateAndAttachGroupBPsToAnotherBP(Alien *alienPtr, QMap<QPair<int, int>, QPolygon> &GroupBP_AttachmentToAnotherBP_ZoneScatteringTrend_DNA)
{
    QVector<int> tempGroupBPsIDsList; tempGroupBPsIDsList.clear();
    // The system reviews the DNA and starts generating the group BPs
    QMapIterator<QPair<int, int>, QPolygon> tempIter(GroupBP_AttachmentToAnotherBP_ZoneScatteringTrend_DNA);
    while (tempIter.hasNext())
    {
        tempIter.next();
        QPair<int,int> tempKey = tempIter.key(); // QPair(source BP type, dest BP)
        QPolygon tempPoly = tempIter.value(); // information needed to generate and attach group BPs

        QVector<BodyPart*> tempGroupBPsList; tempGroupBPsList.clear();
        // Now interpret the contents/info of the tempPoly
        // First gererate the group BPs - We'll create a bunch of clones
        int tempNumOfBPs = tempPoly[0].y();
        // The first BP is a newly created BP, and the rest will be its clones
        tempGroupBPsList.append(this->modelNewBP_TypeGuided(alienPtr, tempKey.first));

        QVector<int> tempGroupBP_ID; tempGroupBP_ID.clear();
        manipulateSkeletalBulkinessDNA_Flags(alienPtr, false, false, false, false, true, new QVector<int>(0), new QVector<int>(0));
        tempGroupBP_ID << tempGroupBPsList.last()->BP_ID;
        this->buildSkeletalBP_BulkinessDNA(alienPtr, tempGroupBP_ID);
        this->bulkUpBodyPart(alienPtr, tempGroupBP_ID);

        // The remaining will be its clones
        qDebug() << "tempNumOfBPs: " << tempNumOfBPs;
        qDebug() << "about to create all clones";
        for (int i=1; i<tempNumOfBPs; i++)
        {
            tempGroupBPsList.append(this->cloneSkeletalBP_FromAnotherAlien_viaDNA(alienPtr, alienPtr,
                                                                                  tempGroupBPsList.first()));
        }
        qDebug() << "all clones created";

        for (BodyPart* bpPtr : tempGroupBPsList)
            tempGroupBPsIDsList << bpPtr->BP_ID;

        // Now attach these Group BPs to the dest BP.
        // For this we need to compute the bones and its neighbouring bones
        // We also need to compute the attachement trend

        attachBodyPartToAnotherBodyPart(alienPtr, tempGroupBPsIDsList, tempKey.second, true, QVector<QVector3D>(0), QVector<QVector3D>(0), QVector<int>(0));
    }

    return tempGroupBPsIDsList;
}

QPair<int,int> AlienModeller::generateAndAttachClonedBPNearby(Alien *alienPtr, int BP_ID, uint8_t numOfClones)
{
    QPair<int,int> bpMirrPair = QPair<int,int>(-1,-1);
    BodyPart *bpPtr = getBodyPartFromID(alienPtr, BP_ID);

    if (alienPtr->alienDNA->skeletonDNA->numOfSkeletalBodyParts_DNA.value(bpPtr->BP_Type) <= 30)
    {
        if (bpPtr->isSymmetric)
        {
            qDebug() << "this BP is a mirror, so selecting the original BP";
            bpPtr = this->getBodyPartFromID(alienPtr, bpPtr->mirrorOf);
        }
        qDebug() << "this BP is not a mirror";
        // create clones of the BP
        // Call the updateGroupBP_AttachmentToAnotherBP_ZoneScatteringTrend_DNA to update the
        // ... GroupBP_AttachmentToAnotherBP_ZoneScatteringTrend_DNA.
        // ... This is because we need to cater for the attachment area as set by the user.
        // ... We also need to use the number of clones as specified by the user
        // get the attachment location of the BP
        // attach the clones to this location
        // mirror the clones
        // update all the necessary book-keeping DS

        // Check if this BP has been smoothed or not.
        // ... If it has been smoothed, then swap it with its original unsmoothed version in smoothingBackupCloneBPs, and remove it from the Alien
        // ... Then smooth it again as many times as it was smoothed before
        quint8 smoothLevel = bpPtr->smoothnessLevel;
        if (smoothLevel > 0)
        {
            // Basically here we're replacing the BP with its unsmoothed clone
            BodyPart* tempClPtr = alienPtr->smoothingBackupCloneBPs.value(bpPtr->BP_ID);
            alienPtr->smoothingBackupCloneBPs.remove(bpPtr->BP_ID);

            QVector3D tempPos = bpPtr->getPivotPoint();
            QVector3D tempVec = bpPtr->getPivotVector();

            bpPtr = replaceBP_WithAnotherBP(alienPtr, bpPtr, tempClPtr);

            GeometryAlgorithms::rotateBodyPartTowardsVecDir(*(bpPtr), tempVec);
            GeometryAlgorithms::translateBP(bpPtr,   tempPos.x() - bpPtr->getPivotPoint().x(),
                                                                    tempPos.y() - bpPtr->getPivotPoint().y(),
                                                                    tempPos.z() - bpPtr->getPivotPoint().z());
        }

        QVector<BodyPart*> tempClonedBPsList; tempClonedBPsList.clear();
        QVector<int> bpIDs; bpIDs.clear();
        for (auto i=0; i<numOfClones; i++)
        {
            bpIDs.clear();
            tempClonedBPsList.append(cloneSkeletalBP_FromAnotherAlien_viaDNA(alienPtr, alienPtr, bpPtr));
            bpIDs << tempClonedBPsList.last()->BP_ID;
            bpMirrPair.first = tempClonedBPsList.last()->BP_ID;

            QPolygonF emptyPoly; emptyPoly.clear();
            skeletonRuleSet->modifyBP_AttachmentToBackBone_ZoneAndScattering_RS(tempClonedBPsList.last()->BP_Type, alienPtr->Alien_Type_ID, emptyPoly);
            addEntryForBP_AttachmentToBackBone_ZoneAndScattering_DNA(alienPtr, tempClonedBPsList.last()->BP_Type, alienPtr->Alien_Type_ID);
            BodyPart *bbPtr = !alienPtr->alienBackBones.isEmpty() ? alienPtr->alienBackBones.first() : nullptr;
            // Now attach this BP to the BB
            attachBodyPartToAnotherBodyPart(alienPtr, bpIDs, bbPtr->BP_ID, false, QVector<QVector3D>(0), QVector<QVector3D>(0), QVector<int>(0));
            // addEntryForBP_AttachmentToBackBone_ZoneAndScattering_DNA(alienPtr, tempClonedBPsList.last()->BP_Type, alienPtr->Alien_Type_ID);
            // skeletonRuleSet->initBP_AttachmentToBackBone_ZoneAndScattering_RS();

            BodyPart *mirrPtr = symmetrifyBP_AroundOppositeAttachmentFace(alienPtr, tempClonedBPsList.last());
            // make sure to store the mirror's ID in newBP_IDs
            // newBP_IDs << mirrPtr->BP_ID;

            // Now smooth both the original BP and its mirror
            smoothenSkeletalBP(alienPtr, tempClonedBPsList.last(), smoothLevel);

            if (mirrPtr)
            {
                smoothenSkeletalBP(alienPtr, mirrPtr, smoothLevel);
                bpMirrPair.second = mirrPtr->BP_ID;
            }
        }
    }

    return bpMirrPair;
}

void AlienModeller::removeAssociatedJATsAndOtherDSsForRemovableJnts(Alien *alienPtr, QVector<int> BP_JntsIDsList)
{
    for (int jntID : BP_JntsIDsList)
    {
        // TODO(ismail): Make seperate methods that can remove and add new Joint entries to the following DS
        alienPtr->alienDNA->skeletonDNA->skeletal_JATs_DNA.VectorEnergyAngleTrends_TypeGuided.remove(jntID); // need this for smoothing
        alienPtr->alienSkeleton.vectorLocationCoupleForEachJoint_TypeGuided.remove(jntID); // do not need this smoothing
        alienPtr->alienDNA->skeletonDNA->skeletal_JATs_DNA.randIntForBookKeeping_EnergyTrend.remove(jntID); // do not need ths smooehting
        alienPtr->alienDNA->skeletonDNA->skeletal_JATs_DNA.randFloatForBookKeeping_FinalEnergy.remove(jntID); // need this smoothing
        alienPtr->alienDNA->skeletonDNA->skeletal_JATs_DNA.randIntForBookKeeping_AngleTrend.remove(jntID); // DNN
        alienPtr->alienDNA->skeletonDNA->skeletal_JATs_DNA.randIntForBookKeeping_quaterEulerAngleSelectorForFirstChild.remove(jntID); // DNN
        alienPtr->alienDNA->skeletonDNA->skeletal_JATs_DNA.randQVector3DForBookKeeping_quaterEulerAngleSelectorForNotFirstChild.remove(jntID); // DNN

        // TODO(ismail): What about the case where we need to remove the entry with this joint as the key??
        // ... At the moment we are only removing those entries with this joint as the value
        int tempKey = alienPtr->alienSkeleton.JntToJntMappings_ForBookKeeping.key(jntID, -2);
        if (tempKey != -2)
            alienPtr->alienSkeleton.JntToJntMappings_ForBookKeeping.remove(tempKey);
    }
}

void AlienModeller::attachBodyPartToAnotherBodyPart(Alien *alien, QVector<int> &sourceBP_ID_List, int destBP_ID, bool isItGroupBPsAttachment,
                                                    QVector<QVector3D> attachmentPnts, QVector<QVector3D> attachmentVectors, QVector<int> jntLocPairsList,
                                                    bool isItReAttmt)
{
    qDebug() << "Commencing attachBodyPartToAnotherBodyPart";
    BodyPart *destBP_Ptr = getBodyPartFromID(alien, destBP_ID);

    if (destBP_Ptr)
    {
        // Fetch the source BPs Ptrs
        QVector<BodyPart *> sourceBPsPtrList;
        sourceBPsPtrList.resize(0);
        for (int i=0; i<sourceBP_ID_List.length(); i++)
        {
            BodyPart *tempSrcPtr = getBodyPartFromID(alien, sourceBP_ID_List[i]);
            if (tempSrcPtr)
                sourceBPsPtrList << tempSrcPtr;
            else
            {
                qDebug() << "The child BP does not exist - attachBodyPartToAnotherBodyPart method";
                exit(0);
            }
        }

        if (!sourceBPsPtrList.isEmpty())
        {
            QPair<float, float> tempPair;
            int tempBoneZoneID, tempScatPerc;
            QVector<int> tempFaceList; tempFaceList.clear();
            if (isItGroupBPsAttachment)
            {
                if (!alien->alienDNA->skeletonDNA->GroupBP_AttachmentToAnotherBP_ZoneScatteringTrend_DNA.contains(QPair<int,int>(sourceBPsPtrList.first()->BP_Type, destBP_ID)))
                {
                    // Generate a random group attachment DNA for this (childBP_Type,ParentBP) pair
                    QPolygon tempPoly; tempPoly.clear();
                    updateGroupBP_AttachmentToAnotherBP_ZoneScatteringTrend_DNA(alien, sourceBPsPtrList.first()->BP_Type, destBP_ID, true, tempPoly);
                }
                QPolygon DNA_Poly = alien->alienDNA->skeletonDNA->GroupBP_AttachmentToAnotherBP_ZoneScatteringTrend_DNA.value(QPair<int,int>(sourceBPsPtrList.first()->BP_Type, destBP_ID));
                tempBoneZoneID = DNA_Poly.at(0).x();
                tempScatPerc = DNA_Poly.at(1).x();
                // Also get the faces and the attachement angles from the DNA
                tempFaceList << DNA_Poly.at(3).x() << DNA_Poly.at(3).y() << DNA_Poly.at(4).x() << DNA_Poly.at(4).y() << DNA_Poly.at(5).x() << DNA_Poly.at(5).y();
            }
            else
                tempPair = alien->alienDNA->skeletonDNA->BP_AttachmentToBackBone_ZoneAndScattering_DNA.value(sourceBPsPtrList[0]->BP_Type);

            if (attachmentPnts.isEmpty() && attachmentVectors.isEmpty() && jntLocPairsList.isEmpty())
            {
                int distributionTrend_FromDNA = 2; // TODO(ismail): This needs to be included in a DNA DS
                int groupingTrendFromDNA = 1; // At the moment I'm using hardcoded value
                qDebug() << "source BPs to attach : " << sourceBP_ID_List.length();

                if (isItGroupBPsAttachment)
                {
                    findPointsAndVectorsOnDestBPForSourceBPsAttachments(alien, destBP_ID, tempBoneZoneID, static_cast<float>(tempScatPerc), groupingTrendFromDNA,
                                                                          sourceBPsPtrList.length(), attachmentPnts, attachmentVectors, jntLocPairsList, tempFaceList);
                }
                else
                {
                    findPointsAndVectorsOnBackBoneForBP_Attachments(alien, destBP_ID, sourceBPsPtrList.at(0)->BP_Type, static_cast<int>(tempPair.first), tempPair.second, distributionTrend_FromDNA,
                                                                      sourceBPsPtrList.length(), attachmentPnts, attachmentVectors, jntLocPairsList);
                }
            }

            qDebug() << "attachmentPnts size after source BP attached : " << attachmentPnts.length();
            qDebug() << "attachmentVecs size : " << attachmentVectors.length();
            qDebug() << "jntLocPairsList size : " << jntLocPairsList.length();

            for (int i=0; i<sourceBPsPtrList.length(); i++)
            {
                // if this BP has already been attached, don't attach it
                // To do this, we need to check the BP_AttachmentToOtherBP_withAttachPositionAndVector_DNA DS
                if (alien->alienDNA->skeletonDNA->BP_AttachmentToOtherBP_withAttachPositionAndVector_DNA.contains(sourceBPsPtrList[i]->BP_ID))
                    continue;

                // update the BP_AttachmentToOtherBP_withAttachPositionAndVector_DNA
                // TODO: We need to have u and v parameters as well in this attachment DNA
                this->updateBP_AttachmentToOtherBP_withAttachPositionAndVector_DNA(alien, -1, destBP_ID,
                                                                                   sourceBPsPtrList[i]->BP_ID,
                                                                                   attachmentPnts[i],
                                                                                   attachmentVectors[i]);
                // This arrangement doesn't matter. First rotate then translate OR first translate then rotate
                if (!isItReAttmt)
                    GeometryAlgorithms::rotateBodyPartTowardsVecDir(*(sourceBPsPtrList[i]), attachmentVectors[i]);

                GeometryAlgorithms::translateBP(sourceBPsPtrList.at(i),
                                                attachmentPnts[i].x() - sourceBPsPtrList.at(i)->getPivotPoint().x(),
                                                attachmentPnts[i].y() - sourceBPsPtrList.at(i)->getPivotPoint().y(),
                                                attachmentPnts[i].z() - sourceBPsPtrList.at(i)->getPivotPoint().z());

                GeometryAlgorithms::updateBP_CellPrimitiveGeometry(sourceBPsPtrList.at(i));

        //        GeometryAlgorithms::scaleBP(sourceBPsPtrList.at(i), 1.05, 1.05, 1.05);

                // TODO(ismail): At the moment the body parts are being attached to the joints. Later it would be nice to have them attached to the bones
                // TODO(ismail): compute attachment vectors that are not normal vectors but are inclined towards the pelvis or other anatomical trends
                //               ... and also make these vectors as part of a DNA, so that we can use some sort of vector fields to guide the
                //               ... BP attachement trends

                // *VERY IMPORTANT* - Include the child BP (source BP) as a sub BP of this source BP (dest BP)
                // First check if this is not already a subBP of the parent BP
                if (!destBP_Ptr->subBodyParts.contains(sourceBPsPtrList.at(i)))
                    destBP_Ptr->subBodyParts.append(sourceBPsPtrList.at(i));

                // If any of the parent BP's joints and bones contains this BP in its
                // ... allAttachedBP_IDs list, then first remove it from this list
                QVector<Joint*> allJntPtrs; allJntPtrs.clear();
                getAllBP_Joints(destBP_Ptr,allJntPtrs);
                for (Joint* jnt : allJntPtrs)
                    if (jnt->allAttachedBP_IDs.contains(sourceBPsPtrList.at(i)->BP_ID))
                        jnt->allAttachedBP_IDs.removeOne(sourceBPsPtrList.at(i)->BP_ID);

                QVector<Bone*> allBnsPtrs; allBnsPtrs.clear();
                getAllBP_BonesInOrder(alien, destBP_Ptr,allBnsPtrs);
                for (Bone *bone : allBnsPtrs)
                    if (bone->allAttachedBP_IDs.contains(sourceBPsPtrList.at(i)->BP_ID))
                        bone->allAttachedBP_IDs.removeOne(sourceBPsPtrList.at(i)->BP_ID);

                // Finally add the BP ID to the joint or bone it is attached to
                BodyPart *jntOrBonePtr = getBodyPartFromID(alien, jntLocPairsList[i]);
                if (jntOrBonePtr->BP_Type == AlienModeller::BodyPartsEnum::JOINT)
                {
                    Joint *jntPtr = dynamic_cast<Joint*>(jntOrBonePtr);
                    jntPtr->allAttachedBP_IDs << sourceBPsPtrList[i]->BP_ID;
                }
                else if (jntOrBonePtr->BP_Type == AlienModeller::BodyPartsEnum::BONE)
                {
                    Bone *bonePtr = dynamic_cast<Bone*>(jntOrBonePtr);
                    bonePtr->allAttachedBP_IDs << sourceBPsPtrList[i]->BP_ID;
                }
            }

            qDebug() << "bodyparts attached successfully";
        }
    }
}

void AlienModeller::attachHeadToParentBP(Alien *alienPtr, BodyPart *headPtr, BodyPart *parentBP)
{
    // First add an entry in the GroupBP_AttachmentToAnotherBP_ZoneScatteringTrend_DNA
    QPolygon tempPoly; tempPoly.clear();
    tempPoly << QPoint(0, 1)
             << QPoint(1, 1)
             << QPoint(0, 0)
             << QPoint(-1,-1)
             << QPoint(-1,-1)
             << QPoint(Box::BoxFace::FRONT,-1);
    updateGroupBP_AttachmentToAnotherBP_ZoneScatteringTrend_DNA(alienPtr, headPtr->BP_Type, parentBP->BP_ID, true, tempPoly);
    QVector<int> headIDs; headIDs.clear();
    headIDs << headPtr->BP_ID;
    // Now call the attachBodyPartToAnotherBodyPart()
    // attachBodyPartToAnotherBodyPart(alienPtr, headIDs, alienPtr->alienBackBones.first()->BP_ID,
    //                                 true, QVector<QVector3D>(0), QVector<QVector3D>(0), QVector<int>(0));
    QVector<Bone*> bonesInOrd; bonesInOrd.clear();
    getAllBP_BonesInOrder(alienPtr, parentBP, bonesInOrd);
    GeometryAlgorithms::rotateBodyPartTowardsVecDir(*headPtr, bonesInOrd.last()->cellList.first()->getNormalVectorOfBoxFace(Box::BoxFace::FRONT));
    GeometryAlgorithms::translateBP(headPtr,
                                bonesInOrd.last()->cellList.first()->getMidPointOfFace(Box::BoxFace::FRONT).x() - headPtr->getPivotPoint().x(),
                                bonesInOrd.last()->cellList.first()->getMidPointOfFace(Box::BoxFace::FRONT).y() - headPtr->getPivotPoint().y(),
                                bonesInOrd.last()->cellList.first()->getMidPointOfFace(Box::BoxFace::FRONT).z() - headPtr->getPivotPoint().z());
    if (!parentBP->subBodyParts.contains(headPtr))
        parentBP->subBodyParts.append(headPtr);

    // Now scale this extremity BP according to the size of the parent BP
    float parentBP_Len = getTotalLengthOfSkeletalBP(alienPtr, parentBP);
    float headLength = getTotalLengthOfSkeletalBP(alienPtr, headPtr);
    qDebug() << headLength;
    while (headLength > (GeometryAlgorithms::getRandomInteger(40,100)/100.f)*parentBP_Len)
    {
        QVector3D scaleVec(0.9,0.9,0.9); // scaling Vector
        incDecBP_Size(alienPtr, headPtr->BP_ID, scaleVec);
        headLength = getTotalLengthOfSkeletalBP(alienPtr, headPtr);
        // qDebug() << "reached 0";
    }
}

void AlienModeller::reattachChildBPsToParentBP_DiffLocations(Alien *alien, QVector<int> &childBP_ID_List, int parentBP_ID)
{
    /*  ALGO
     *
     * - Delete the mirrors of the BPs in childBP_ID_List
     * - Detach the BPs in childBP_ID_List from the parent BP (NOT NEEDED)
     * - Recompute the new locations for these BPs...this can be done via manipulating the DNA
     * ... and only using it for the incoming BPs.
     * ... The benefit of utilizing the DNA is that the cloning will happen accurately
     * - Attach them to the newly computed locations on the parent BP...ensure to attach these on the left face
     * ... of bones/joints
     * - Update the necessary DSs
     * ... This includes updating:
     * ... jntPtr->allAttachedBP_IDs, bonePtr->allAttachedBP_IDs, any DNA DSs
     * - Re-symetrify the child BPs
     *
    */

    qDebug() << "About to Reattach incoming child BPs";
    // Find and Delete the mirrors of the BPs in childBP_ID_List
    // ... and reattach these to the parent BP on the newly computed locations
    QVector<int> mirrIDs; mirrIDs.clear();
    for (int childBP_ID : childBP_ID_List)
    {
        BodyPart *mirrPtr = getMirrorOrOriginalBP(alien, getBodyPartFromID(alien, childBP_ID));
        if (mirrPtr)
        {
            mirrIDs << mirrPtr->BP_ID;
        }
        // Ensure to remove this BPs entry from the BP_AttachmentToOtherBP_withAttachPositionAndVector_DNA
        this->updateBP_AttachmentToOtherBP_withAttachPositionAndVector_DNA(alien, childBP_ID, parentBP_ID,
                                                                           childBP_ID,
                                                                           QVector3D(0,0,0),
                                                                           QVector3D(0,0,0));
    }
    // Now delete all the mirrors
    removeAlienBodyParts(alien, mirrIDs);

    qDebug() << "done 1 in reattach method";

    // Reattach these to the parent BP on the newly computed locations
//    updateGroupBP_AttachmentToAnotherBP_ZoneScatteringTrend_DNA(
//                alien, getBodyPartFromID(alien, childBP_ID_List.first())->BP_Type, parentBP_ID, true);
    // First modify the attachement DNA (BP_AttachmentToBackBone_ZoneAndScattering_DNA) entry for this BP_Type
    // For this we will first tweak the Ruleset and then re-assign a new entry to the DNA
    QPolygonF tempRS_PolyF; tempRS_PolyF.clear();
    BodyPart *tempChildPr = getBodyPartFromID(alien, childBP_ID_List[0]);
    skeletonRuleSet->modifyBP_AttachmentToBackBone_ZoneAndScattering_RS(tempChildPr->BP_Type, alien->Alien_Type_ID, tempRS_PolyF);
    addEntryForBP_AttachmentToBackBone_ZoneAndScattering_DNA(alien, tempChildPr->BP_Type, alien->Alien_Type_ID);
    skeletonRuleSet->initBP_AttachmentToBackBone_ZoneAndScattering_RS();
    QVector<QVector3D> attachmentPnts; attachmentPnts.clear();
    QVector<QVector3D> attachmentVectors; attachmentVectors.clear();
    QVector<int> boneLocPairsList; boneLocPairsList.clear();
    attachBodyPartToAnotherBodyPart(alien, childBP_ID_List, parentBP_ID, false, QVector<QVector3D>(0), QVector<QVector3D>(0), QVector<int>(0), true);

    qDebug() << "done 2 in reattach method";

    // Resymmetrify the incoming child BPs
    for (int childBP_ID : childBP_ID_List)
    {
        BodyPart *childPr = getBodyPartFromID(alien, childBP_ID);
        // TODO: first check if a mirror exists or not
        if (!childPr->isSymmetric) // Just to ensure this is not intself a mirror
        {
            BodyPart *mirrPtr = symmetrifyBP_AroundOppositeAttachmentFace(alien, childPr);
        }
    }

    qDebug() << "Reattachments done successfully";
}

void AlienModeller::getAllBP_ID_List(Alien *alienPtr, QVector<int> &allBP_ID_List, QVector<int> &allSepClonedBP_ID_Listm)
{
    for (int i=0; i<alienPtr->bodyParts.length(); i++)
    {
        if (alienPtr->bodyParts[i]->BP_Type != AlienModeller::BodyPartsEnum::JOINT  &&
                alienPtr->bodyParts[i]->BP_Type != AlienModeller::BodyPartsEnum::BONE)
            allBP_ID_List.append(alienPtr->bodyParts[i]->BP_ID);
    }

    for (int i=0; i<alienPtr->seperateCloneBPs.length(); i++)
    {
        if (alienPtr->seperateCloneBPs[i]->BP_Type != AlienModeller::BodyPartsEnum::JOINT  &&
                alienPtr->seperateCloneBPs[i]->BP_Type != AlienModeller::BodyPartsEnum::BONE)
            allSepClonedBP_ID_Listm.append(alienPtr->seperateCloneBPs[i]->BP_ID);
    }
}

bool AlienModeller::getAllRenderableBPs(Alien *alienPtr, QVector<BodyPart *> &allRenderableBPsList, int glwID)
{
    // Check if the GLW, Alien and BP exist in the map or not
    if (GLW_to_Alien_to_BP_to_ColorAndDisplay_Map.contains(glwID))
    {
        if (GLW_to_Alien_to_BP_to_ColorAndDisplay_Map.value(glwID).contains(alienPtr->alienID))
        {
            QVector<int> allBP_ID_List; allBP_ID_List.clear();
            QVector<int> tempSepBP_IDs; tempSepBP_IDs.clear();
            getAllBP_ID_List(alienPtr, allBP_ID_List, tempSepBP_IDs);
            for (int bpID : allBP_ID_List)
            {
                if (GLW_to_Alien_to_BP_to_ColorAndDisplay_Map.value(glwID).value(alienPtr->alienID).value(bpID).first == true)
                {
//                    qDebug() << "*********** TRUE *************";
                    allRenderableBPsList.append(getBodyPartFromID(alienPtr, bpID));
                }
            }
            for (int bpID : tempSepBP_IDs)
            {
                if (GLW_to_Alien_to_BP_to_ColorAndDisplay_Map.value(glwID).value(alienPtr->alienID).value(bpID).first == true)
                {
//                    qDebug() << "*********** TRUE *************";
                    allRenderableBPsList.append(getBodyPartFromID(alienPtr, bpID));
                }
            }
            return true;
        }
        else
            return false;
    }
    else
        return false;
}

void AlienModeller::getAllBPsOfGivenType(Alien *alienPtr, int BP_Tpe, QVector<BodyPart*> &similarBPs, bool includeMirrors)
{
    // Find all the BPs that are of the same type as bpPtr
    for (BodyPart *bp : alienPtr->bodyParts)
    {
        if (bp->BP_Type == BP_Tpe)
        {
            if (includeMirrors)
                similarBPs.append(bp);
            else if (!bp->isSymmetric)
                similarBPs.append(bp);
        }
    }
}

void AlienModeller::buildHeadSubBP_AlignmentDNA(Alien *alienPtr, int headID)
{
    QPolygon tempRS = skeletonRuleSet->HeadSubBP_AlignmentRS;

    QPolygon tempPoly; tempPoly.clear();
    if (!alienPtr->alienDNA->skeletonDNA->headSubBP_AlignmentDNA.contains(headID))
    {
        tempPoly << QPoint(GeometryAlgorithms::getRandomInteger(tempRS[0].x(),tempRS[0].y()), GeometryAlgorithms::getRandomInteger(tempRS[1].x(),tempRS[1].y()))
                 << QPoint(GeometryAlgorithms::getRandomInteger(0,tempRS[2].x()), GeometryAlgorithms::getRandomInteger(0,tempRS[2].y()))
                 << QPoint(GeometryAlgorithms::getRandomInteger(0,tempRS[3].x()), GeometryAlgorithms::getRandomInteger(0,tempRS[3].y()))
                 << QPoint(GeometryAlgorithms::getRandomInteger(0,tempRS[4].x()), GeometryAlgorithms::getRandomInteger(0,tempRS[4].y()))
                 << QPoint(GeometryAlgorithms::getRandomInteger(tempRS[5].x(),tempRS[5].y()), -1);

        if (tempPoly[1].x() == 1)
            tempPoly[1].setY(360 - tempPoly[1].y());
        if (tempPoly[2].x() == 1)
            tempPoly[2].setY(360 - tempPoly[2].y());
        if (tempPoly[3].x() == 1)
            tempPoly[3].setY(360 - tempPoly[3].y());

        alienPtr->alienDNA->skeletonDNA->headSubBP_AlignmentDNA.insert(headID, tempPoly);
    }
}

BodyPart *AlienModeller::modelNewHead(Alien *alienPtr)
{
    // ALGO ***
    // Model a new BP with Head Type
    // Model a new Maxilla, Mandible and Zygomatic
    // Align the sub BPs
    // Attach the Sub BPs to the Head
    // Densify the Sub BPs

    qDebug() << "About to model new Head with its Sub BPs";
    // First create only a basic head bone lineage
    BodyPart *headPtr = modelNewBP_TypeGuided(alienPtr, AlienModeller::BodyPartsEnum::HEAD);

    qDebug() << "Head's basic cell modelled. Now going to densify it";
    // Now densify the head to give it shape and form
    densifyHead(alienPtr, headPtr);

    qDebug() << "New Head modelled successfully";

    return headPtr;
}

BodyPart *AlienModeller::modelNewHead_Algo2(Alien *alienPtr)
{
    // Model a new BP for the main Head Lineage
    // Generate nearby cell list lineages
    // create bridges b/w two lineages keeping account of some eye holes/sockets and nose holes
    // Grow horns/spikes/fangs/shards from the lineages

    qDebug() << "About to model new Head";
    QVector<int> tempBPsID; tempBPsID.clear(); // For Bulkification
    // First create only a basic head bone lineage
    Head *headPtr = dynamic_cast<Head*>(modelNewBP_TypeGuided(alienPtr, AlienModeller::BodyPartsEnum::HEAD));
    tempBPsID << headPtr->BP_ID;
    // Now bulkify the head BP lineage
    manipulateSkeletalBulkinessDNA_Flags(alienPtr, false, false, false, false, true, new QVector<int>(0), new QVector<int>(0));
    this->buildSkeletalBP_BulkinessDNA(alienPtr, tempBPsID);
    this->bulkUpBodyPart(alienPtr, tempBPsID);

    // Now generate a set of cell lineages
    int numOfCellLin = GeometryAlgorithms::getRandomInteger(1,10);
    // Get the existing Jnt2JntRel Poly for this BP. At this time it does not contain the newly added joint IDs
    QVector<int> tempJnt2JntLin; tempJnt2JntLin.clear();
    convertJnt2JntPolyToJointsLineageList(alienPtr->alienSkeleton.Jnt2JntRelations_TypeGuided.value(headPtr->BP_ID), tempJnt2JntLin);
    // // define a large plane
    // QVector3D planeVertex = dynamic_cast<Joint*>(getBodyPartFromID(alienPtr, tempJnt2JntLin[0]))->cellList[0]->getMidPointOfFace(Box::BoxFace::LEFT);
    // QVector3D planeNorm = dynamic_cast<Joint*>(getBodyPartFromID(alienPtr, tempJnt2JntLin[0]))->cellList[0]->getNormalVectorOfBoxFace(Box::BoxFace::LEFT);
    for (int i=0; i<numOfCellLin; i++)
    {
        QVector<Cell*> cellLineage; cellLineage.clear();
        QVector<QVector3D> tempPtsList; tempPtsList.clear();
        float pitchAng = GeometryAlgorithms::getRandomFloat(10,75);
        float normVecScaleFac = GeometryAlgorithms::getRandomInteger(10,200);
        for (int j=0; j<tempJnt2JntLin.length(); j++)
        {
            Joint *jntPtr = dynamic_cast<Joint*>(getBodyPartFromID(alienPtr, tempJnt2JntLin[j]));
            GeometryAlgorithms::rotateCellUsingEulerAngles(jntPtr->cellList[0], 0, 0, 360-pitchAng);
            QVector3D tempPnt = jntPtr->cellList[0]->getMidPointOfFace(Box::BoxFace::LEFT);
            // get the normal vector of left face
            QVector3D normVec = jntPtr->cellList[0]->getNormalVectorOfBoxFace(Box::BoxFace::LEFT);
            normVec = normVecScaleFac * normVec;
            tempPnt = tempPnt + normVec;
            // we can choose to shift this point a bit in all directions
            tempPnt += QVector3D(   GeometryAlgorithms::getRandomFloat(-9.5,9.5),
                                    GeometryAlgorithms::getRandomFloat(-29.5,3.5),
                                    GeometryAlgorithms::getRandomFloat(-9.5,9.5));
            tempPtsList << tempPnt;
            if (j>0) // Now create a cell between the last 2 points and keep updating the cell lineage on the fly
            {
                cellLineage.append(makeCellBetweenTwoPoints(tempPtsList[j-1], tempPtsList[j]));
                GeometryAlgorithms::scaleCell(cellLineage.last(), 3.5, 3.5, 1);
            }

            GeometryAlgorithms::rotateCellUsingEulerAngles(jntPtr->cellList[0], 0, 0, pitchAng);
        }
        // Choose to extend this cell lineage to give interesting formations towards upper jaw area
        // if (GeometryAlgorithms::getRandomInteger(0,100) % 2 == 0)
        // {
        //     for (int j=0; j<6; j++)
        //     {
        //         Cell *newClonedCell = new Cell();
        //         newClonedCell->isDenser = true;
        //         cellLineage.last()->cloneCell(newClonedCell);

        //         newClonedCell->setPivotFace(Box::BoxFace::BACK);

        //         QVector3D attachPoint = cellLineage.last()->getMidPointOfFace(Box::BoxFace::FRONT);
        //         GeometryAlgorithms::scaleCell(newClonedCell, GeometryAlgorithms::getRandomFloat(0.8,1.f),
        //                                       GeometryAlgorithms::getRandomFloat(0.8,1.f), GeometryAlgorithms::getRandomFloat(0.8,1.f));
        //         GeometryAlgorithms::rotateCellUsingEulerAngles(newClonedCell, GeometryAlgorithms::getRandomFloat(-50,50),
        //                                                        GeometryAlgorithms::getRandomFloat(-50,50), 0);
        //         GeometryAlgorithms::translateCell(newClonedCell, attachPoint - newClonedCell->getPivotPoint());

        //         cellLineage.append(newClonedCell);
        //     }
        // }

        // Now bulkify this cell lineage
        headPtr->allCellListLineages.append(cellLineage);
    }

    // OPTIONAL: Perhaps smooth out the cell lineages to make them a bit more organic


    // Now Make bridges b/w any two Lineages
    // TODO: Dont  make a bridge that touches any of the extentions (of celllists)
    int randNumOfBridges = GeometryAlgorithms::getRandomInteger(0,headPtr->allCellListLineages[0].length());
    QVector<QPair<int,int>> cellIndexAvoidancePairs; cellIndexAvoidancePairs.clear();
    if (headPtr->allCellListLineages.length() > 1)
    {
        for (int i=0, b=0; i<headPtr->allCellListLineages.length() && b<randNumOfBridges; i++)
        {

            for (int j=0; j<headPtr->allCellListLineages.at(i).length(); j++)
            {
                int randChoice = GeometryAlgorithms::getRandomInteger(0,100);
                if (randChoice % 2 == 0) // make a bridge b/w this cell and a randomly selected cell of another randomly selected cell list
                {
                    int randCellListInd = 0;
                    do
                    {
                        randCellListInd = GeometryAlgorithms::getRandomInteger(0,headPtr->allCellListLineages.length());
                    } while (randCellListInd == i);
                    do
                    {
                        randChoice = GeometryAlgorithms::getRandomInteger(0,headPtr->allCellListLineages.at(randCellListInd).length());

                    } while ((cellIndexAvoidancePairs.contains(QPair<int,int>(j,randChoice))  ||  cellIndexAvoidancePairs.contains(QPair<int,int>(randChoice,j)))
                                                                                                &&
                             abs(j-randChoice) > 1);
                    qDebug() << "bridge difference : " << abs(j-randChoice);

                    // Now make a bridge b/w these cell by simply creating a cell b/w these two points
                    QVector3D pt1 = headPtr->allCellListLineages.at(i).at(j)->getMidPointOfFace(Box::BoxFace::BACK);
                    QVector3D pt2 = headPtr->allCellListLineages.at(randCellListInd).at(randChoice)->getMidPointOfFace(Box::BoxFace::BACK);

                    Cell *bridgeCell = makeCellBetweenTwoPoints(pt1, pt2);
                    GeometryAlgorithms::scaleCell(bridgeCell, 1.5, 1.5, 1);
                    headPtr->allBridgesCells.append(bridgeCell);
                    cellIndexAvoidancePairs.append(QPair<int,int>(j,randChoice));
                    b++;
                    // TODO: Now subdivide the cell to make it more organic looking. Use 'swellings on ends' bulking trend for the cell subdivisions
                }
            }
        }
    }

    // Now make a bridge on a single Lineage


    // Pick up a cell Lineage at random and attach a new BP (using any JAT) to it at a random point.
    // Rotate the BP so the normal vector of either LEFT or TOP face is YAW'd (-70 to +70) and PITCH'd (-60 to +60)
    // Choose to shrink it
    // Make its Bulkiness as 'Descending' and smooth it if needed
    // TODO: This should be done in the modelNewAlien method since almost all aliens will have extremities
    ExtremityBP *exBP_Ptr = instantiateNewExtremBP(alienPtr);
    // Now add a new entry to the extremity DNA
    addNewEntryForExtremityBP_DNA(alienPtr, exBP_Ptr->BP_ID);
    QVector3D extremDirVec = dynamic_cast<Joint*>(getBodyPartFromID(alienPtr, tempJnt2JntLin[0]))->cellList[0]->getMidPointOfFace(Box::BoxFace::BACK);
    int randNumOfNewHeadSubBPs = GeometryAlgorithms::getRandomInteger(1,headPtr->allCellListLineages.length()*5);
    for (int i=0; i<randNumOfNewHeadSubBPs; i++)
    {
        tempBPsID.clear();
        // For a new BP, at the moment we'll use extremity DIGIT as BP_TYPE
        BodyPart *newHeadSubBP = modelNewBP_TypeGuided(alienPtr, AlienModeller::BodyPartsEnum::ExtremBP_Digit);
        tempBPsID << newHeadSubBP->BP_ID;
        manipulateSkeletalBulkinessDNA_Flags(alienPtr, false, false, false, false, true, new QVector<int>(0), new QVector<int>(0));
        this->buildSkeletalBP_BulkinessDNA(alienPtr, tempBPsID);
        this->bulkUpBodyPart(alienPtr, tempBPsID);

        // headPtr->subBodyParts.append(newHeadSubBP);
        // Now attach it to a Cell Lineage at random
        int randCellLinChoice = GeometryAlgorithms::getRandomInteger(0,headPtr->allCellListLineages.length());
        Cell *randCell = headPtr->allCellListLineages.at(randCellLinChoice).at(GeometryAlgorithms::getRandomInteger(0,headPtr->allCellListLineages.at(randCellLinChoice).length()/2));
        Box *tempBoxPtr = randCell->cellBox;
        QVector<QVector3D> tempFaceVertices; tempFaceVertices.clear();
        QVector<QVector3D> attachmentPnts; attachmentPnts.clear();
        // TODO: re-design the computeRandomPointsOnFace method that accepts the face pointer
        for (int i=0; i<tempBoxPtr->faces[Box::BoxFace::LEFT]->vertNorIndices.length()-1 /*4*/; i++) // TODO(ismail): this can be made faster as we already know there are 5 elements
                                                                                                     // ... in the vertNorIndices list
        {
            tempFaceVertices.append(tempBoxPtr->vertices[tempBoxPtr->faces[Box::BoxFace::LEFT]->vertNorIndices[i]]);
        }
        computeRandomPointsOnFace(tempFaceVertices, 1, attachmentPnts);    // TODO(ismail): additional improvement. The points on the same face should not be too close to each other.
        GeometryAlgorithms::scaleBP(newHeadSubBP, 0.6,0.6,0.6);
        GeometryAlgorithms::rotateBodyPartTowardsVecDir(*(newHeadSubBP), extremDirVec);
        GeometryAlgorithms::rotateBodyPartUsingEulerAngles(*(newHeadSubBP), GeometryAlgorithms::getRandomInteger(10,30),
                                                                            GeometryAlgorithms::getRandomInteger(10,30), 0);
        GeometryAlgorithms::translateBP(newHeadSubBP,
                                    attachmentPnts.first().x() - newHeadSubBP->getPivotPoint().x(),
                                    attachmentPnts.first().y() - newHeadSubBP->getPivotPoint().y(),
                                    attachmentPnts.first().z() - newHeadSubBP->getPivotPoint().z());

        if (!headPtr->subBodyParts.contains(newHeadSubBP))
        {
            headPtr->subBodyParts.append(newHeadSubBP);
            updateBP_AttachmentToOtherBP_withAttachPositionAndVector_DNA(alienPtr, -1, headPtr->BP_ID, newHeadSubBP->BP_ID,
                                                                         attachmentPnts.first(), newHeadSubBP->getPivotVector());
        }
    }



    // Now generate teeth/fangs on a randomly selected cell lineage. Simply create a new BP of 4 to 6 joints, and make its bulkiness as Descending.
    // JAT should be descending. Generate one BP and clone from it to generate the rest
    // find the cell lineage which is farthese away from the main head BP lineage
    qDebug() << "New Head modelled successfully";

    return headPtr;
}

BodyPart *AlienModeller::modelNewHead_UsingStateMachine(Alien *alienPtr)
{
    qDebug() << "About to model new Head";
    Head *headPtr=nullptr;
    QVector<int> tempBPsID; tempBPsID.clear(); // For Bulkification

    // Start the head modelling state machine
    if (alienPtr->alienDNA->skeletonDNA->headStateMachine.isEmpty())
        alienPtr->alienDNA->skeletonDNA->initializeheadStateMachine();

    // THis is the list from which a random cell will be selected throughout the running of this state machine
    // It will be updated in most states where new cells are to added
    QVector<Cell*> tempSelectableCellList; tempSelectableCellList.clear();

    // int randState = GeometryAlgorithms::getRandomInteger(0,alienPtr->alienDNA->skeletonDNA->extremityBP_StateMachine.size());
    int energy = GeometryAlgorithms::getRandomInteger(60,GeometryAlgorithms::getRandomInteger(65,200));
    int currentState = 0;   // start from state 0
    Bone *firstBone = nullptr;

    while (energy > 4)
    {
        // execute the process for this state
        if (currentState == 0)  // Make base head BP
        {
            qDebug() << "Current State : " << currentState;
            // First create only a base head bones lineage
            headPtr = dynamic_cast<Head*>(modelNewBP_TypeGuided(alienPtr, AlienModeller::BodyPartsEnum::HEAD));
            tempBPsID << headPtr->BP_ID;
            // Now bulkify the head BP lineage
            manipulateSkeletalBulkinessDNA_Flags(alienPtr, false, false, false, false, true, new QVector<int>(0), new QVector<int>(0));
            this->buildSkeletalBP_BulkinessDNA(alienPtr, tempBPsID);
            this->bulkUpBodyPart(alienPtr, tempBPsID);
            tempBPsID.clear();
            QVector<Bone*> orderedBones; orderedBones.clear();
            getAllBP_BonesInOrder(alienPtr, headPtr, orderedBones);
            firstBone = orderedBones.first();
            for (Bone *bPtr : orderedBones)
            {
                if (bPtr->rootCell)
                    tempSelectableCellList.append(bPtr->rootCell);
                else
                {
                    qDebug() << "No root cell found for the bones of head in modelNewHead_UsingStateMachine";
                    exit(0);
                }
            }

            // Now decide whether to generate densers from all the bones or not
            // For each bone call the generateStackedDensersFromCell
            int randDecide = GeometryAlgorithms::getRandomInteger(0,100);
            if (randDecide%2 == 0)
            {
                QPolygonF tempGrowthPoly; tempGrowthPoly.clear();
                tempGrowthPoly << QPointF(GeometryAlgorithms::getRandomInteger(5,10), Box::BoxFace::LEFT) // (num of stacked densers, face of base denser on which the stacked denser will be generated)
                               << QPointF(0, 0)     // (pitch angle, yaw angle)
                               << QPointF(GeometryAlgorithms::getRandomInteger(-40,40), 0)    // (roll angle, should it mirror) "NOTE: Keep the roll angle b/w 1 and 2.5" for some interesting results
                               << QPointF(0, 0)     // (%age at which the angle will change, angle change amount)
                               << QPointF(GeometryAlgorithms::getRandomFloat(0.3,0.9), GeometryAlgorithms::getRandomFloat(0.2,0.9))    // (x scaling value, y scaling value)
                               << QPointF(GeometryAlgorithms::getRandomFloat(0.6,1), -1); // (z scaling value, -1)

                QVector<float> rollAngleList = QVector<float>(tempGrowthPoly[0].x(), tempGrowthPoly[2].x());
                Cell *baseCell = nullptr;

                // First generate the stacked densers on all the bones one by one
                for (Bone *bonePtr : orderedBones)
                {
                    if (GeometryAlgorithms::getRandomInteger(0,2) == 1)
                    {
                        baseCell = bonePtr->getLeafDenser();
                        if (baseCell == nullptr)
                            baseCell = bonePtr->rootCell;

                        generateStackedDensersFromCell(bonePtr, baseCell, tempGrowthPoly, rollAngleList);
                    }
                }
            }
        }
        else if (currentState == 1 || currentState == 3 || currentState == 4) // Generate cell lineage from a random point
        {
            qDebug() << "Current State : " << currentState;
            // Select a head BP bone / Lone cell / root cell of cell lineage, at random
            // select a random point keeping checks:    1) The point is not too close to another lone cell or root cell of a cell lineage
            //                                          2) the point is not inside another cell or root cell of a cell lineage
            // If OK to proceed, create a new cloned cell from parent cell

            // Cell Lineage initialization
            QVector<Cell*> tempCellLineage; tempCellLineage.clear(); // TODO: We might not need this DS. Please inspect
            QVector<QVector3D> tempFaceVertices, attachmentPnts;
            qDebug() << "tempSelectableCellList length :" << tempSelectableCellList.length();
            for (int i=0; i<tempSelectableCellList.length() && !tempSelectableCellList.isEmpty(); i++) // shuffle all the elements
            {
                int randInd = GeometryAlgorithms::getRandomInteger(i, tempSelectableCellList.length());
                tempSelectableCellList.prepend(tempSelectableCellList.at(randInd));
                tempSelectableCellList.removeAt(randInd+1);
                qDebug() << randInd;
            }
            QVector<int> shuffledFaceIndices = {0,1,2,3,4,5};
            for (int i=0; i<shuffledFaceIndices.length() && !shuffledFaceIndices.isEmpty(); i++) // shuffle all the elements
            {
                int randInd = GeometryAlgorithms::getRandomInteger(i, shuffledFaceIndices.length());
                shuffledFaceIndices.prepend(shuffledFaceIndices.at(randInd));
                shuffledFaceIndices.removeAt(randInd+1);
            }
            qDebug() << "shuffledFaceIndices : " << shuffledFaceIndices;

            // Now select a random cell from tempCellList
            Cell *selCellPtr=nullptr;
            QVector3D attachPnt = QVector3D(0,0,0);
            bool obstructionFound=false;
            for (int i=0; i<tempSelectableCellList.length(); i++) // TODO: Check if this loop and all its nested loops are performance bottlenecks
                                                                 // ... Perhaps we can use Bounding Volume Heirarchy (BVM) here
            {
                obstructionFound=false;
                selCellPtr = tempSelectableCellList[i];
                for (int faceInd : shuffledFaceIndices)
                {
                    tempFaceVertices.clear();
                    attachmentPnts.clear();
                    // Now find a random point on a random face of this cell
                    for (int k=0; k<selCellPtr->cellBox->faces[faceInd]->vertNorIndices.length()-1 /*4*/; k++) // TODO(ismail): this can me made faster as we already know there are 5 elements
                                                                                                                 // ... in the vertNorIndices list
                    {
                        tempFaceVertices.append(selCellPtr->cellBox->vertices[selCellPtr->cellBox->faces[faceInd]->vertNorIndices[k]]);
            //            qDebug () << tempFaceVertices[j];
                    }
                    computeRandomPointsOnFace(tempFaceVertices, 10, attachmentPnts);    // TODO(ismail): additional improvement. The points on the same face should not be too close to each other.
                                                                                       // ... Perhaps include a check if they are and then move them apart
                    // Now check against each cell in the tempSelectionCellList, if:
                    //                  1) The point is not too close to another lone cell or root cell of a cell lineage or to any face of the head bone cells
                    //                  2) the point is not inside another cell or root cell of a cell lineage
                    for (QVector3D attPnt : attachmentPnts)
                    {
                        for (int c=0; c<tempSelectableCellList.length(); c++)
                        {
                            if (c != i)
                            {
                                for (int f=0; f<6; f++)     // check this point's vicinity against each face of this cell
                                {
                                    QVector3D midPnt = tempSelectableCellList[c]->getMidPointOfFace(f);
                                    QVector3D faceNormal = tempSelectableCellList[c]->getNormalVectorOfBoxFace(f);
                                    float distToFace = attPnt.distanceToPlane(midPnt,faceNormal);
                                    if (distToFace <= 5.f)
                                    {
                                        obstructionFound = true; // red light (so far) - obstruction found
                                    }
                                    else
                                    {
                                        obstructionFound = false; // green light (so far) - no obstruction found
                                        attachPnt = attPnt;
                                        break;
                                    }
                                }
                                if (obstructionFound)
                                    break;      // we've found an obstruction, so no need to check against any more cells
                                // IF no obstruction found, we need to keep checking with other cells
                            }
                        }
                        if (!obstructionFound) // there is no obstruction (GREEN LIGHT) and we exit the loop to proceed with cell lineage generation
                            break;             // This is the point which will serve as the origin point
                        // ELSE otherwise check for the other points
                    }
                    if (!obstructionFound) // we have a point that is not obstructed with any other cell, so we'll use this point as our
                                           // origin for the cell lineage
                    {
                        // NOW GENERATE THE CELL LINEAGE
                        // TODO: Please make a method for this.
                        int numOfStackedCells = GeometryAlgorithms::getRandomInteger(6,40);
                        float xScal=1,yScal=1,zScal=1;
                        QPolygonF x_y_AnglePoly; x_y_AnglePoly.clear();
                        float startXScal = GeometryAlgorithms::getRandomFloat(0.3,0.8);
                        float startYScal = GeometryAlgorithms::getRandomFloat(0.3,0.8);
                        float startZScal = GeometryAlgorithms::getRandomFloat(0.3,0.8);
                        for (int j=0; j<numOfStackedCells; j++)
                        {
                            Cell *newClonedCell = new Cell();
                            newClonedCell->isDenser = true;
                            selCellPtr->cloneCell(newClonedCell);
                            // Make sure we dont clone any of it's child cells
                            while (!newClonedCell->childCells.empty()) delete newClonedCell->childCells.last(), newClonedCell->childCells.removeLast();

                            if (!selCellPtr->childCells.contains(newClonedCell))
                                selCellPtr->childCells.append(newClonedCell);

                            newClonedCell->setPivotFace(Box::BoxFace::BACK);

                            if (j==0)
                            {
                                QVector3D faceNormal = selCellPtr->getNormalVectorOfBoxFace(faceInd);
                                GeometryAlgorithms::rotateCellTowardsVecDir(newClonedCell, faceNormal);
                                GeometryAlgorithms::scaleCell(newClonedCell, startXScal, startYScal, startZScal);
                                int pitch = GeometryAlgorithms::getRandomInteger(-70,70);
                                int yaw = GeometryAlgorithms::getRandomInteger(-30,30);
                                for (int ang=0; ang<numOfStackedCells; ang++)
                                {
                                    int randInt = GeometryAlgorithms::getRandomInteger(0,2);
                                    if (randInt==0)
                                        x_y_AnglePoly << QPointF(pitch,yaw);
                                    else
                                        x_y_AnglePoly << QPointF(360-pitch,360-yaw);
                                    xScal = GeometryAlgorithms::getRandomFloat(0.8,1.f);
                                    yScal = GeometryAlgorithms::getRandomFloat(0.8,1.f);
                                    zScal = GeometryAlgorithms::getRandomFloat(0.8,1.f);
                                }
                            }
                            else
                            {
                                attachPnt = selCellPtr->getMidPointOfFace(Box::BoxFace::FRONT);
                                GeometryAlgorithms::scaleCell(newClonedCell, xScal, yScal, zScal);
                            }

                            GeometryAlgorithms::rotateCellUsingEulerAngles(newClonedCell, x_y_AnglePoly.at(j).x(), x_y_AnglePoly.at(j).y(), 0); // roll angle is 0
                            GeometryAlgorithms::translateCell(newClonedCell, attachPnt - newClonedCell->getPivotPoint());

                            tempCellLineage.append(newClonedCell);
                            selCellPtr = newClonedCell;
                        }
                        // add this cell lineage to the Head's allCellListLineages DS
                        // headPtr->allCellListLineages.append(tempCellLineage);
                        // Update the tempCellList with the first cell of this cell list, so it can be selected in future
                        // tempSelectableCellList.append(tempCellLineage[0]);
                        break;
                    }
                    // else we'll reloop and select a new face where 10 new random candidate points will compete for becoming the origin of cell lineage
                    else
                    {
                        qDebug() << "Obstruction found";
                        // return headPtr;
                    }
                }
                if (!obstructionFound)
                    break;  // we're done
            }
        }
        // Perhaps ignore this state and use the same as 'generate cell lineages' where if there is only one cell, it acts as the 'lone cell'
        else if (currentState == 2 || currentState == 5 || currentState == 6 || currentState == 7) // Make a lone cell
        {
            qDebug() << "Current State : " << currentState;
            QVector<QVector3D> tempFaceVertices, attachmentPnts;
            qDebug() << "tempSelectableCellList length :" << tempSelectableCellList.length();
            for (int i=0; i<tempSelectableCellList.length() && !tempSelectableCellList.isEmpty(); i++) // shuffle all the elements
            {
                int randInd = GeometryAlgorithms::getRandomInteger(i, tempSelectableCellList.length());
                tempSelectableCellList.prepend(tempSelectableCellList.at(randInd));
                tempSelectableCellList.removeAt(randInd+1);
                qDebug() << randInd;
            }
            QVector<int> shuffledFaceIndices = {0,1,2,3,4,5};
            for (int i=0; i<shuffledFaceIndices.length() && !shuffledFaceIndices.isEmpty(); i++) // shuffle all the elements
            {
                int randInd = GeometryAlgorithms::getRandomInteger(i, shuffledFaceIndices.length());
                shuffledFaceIndices.prepend(shuffledFaceIndices.at(randInd));
                shuffledFaceIndices.removeAt(randInd+1);
            }
            qDebug() << "shuffledFaceIndices : " << shuffledFaceIndices;

            // Now select a random cell from tempCellList
            Cell *selCellPtr=nullptr;
            QVector3D attachPnt = QVector3D(0,0,0);
            bool obstructionFound=false;
            for (int i=0; i<tempSelectableCellList.length(); i++) // TODO: Check if this loop and all its nested loops are performance bottlenecks
                                                                 // ... Perhaps we can use Bounding Volume Heirarchy (BVM) here
            {
                obstructionFound=false;
                selCellPtr = tempSelectableCellList[i];
                for (int faceInd : shuffledFaceIndices)
                {
                    tempFaceVertices.clear();
                    attachmentPnts.clear();
                    // Now find a random point on a random face of this cell
                    for (int k=0; k<selCellPtr->cellBox->faces[faceInd]->vertNorIndices.length()-1 /*4*/; k++) // TODO(ismail): this can me made faster as we already know there are 5 elements
                                                                                                                 // ... in the vertNorIndices list
                    {
                        tempFaceVertices.append(selCellPtr->cellBox->vertices[selCellPtr->cellBox->faces[faceInd]->vertNorIndices[k]]);
            //            qDebug () << tempFaceVertices[j];
                    }
                    computeRandomPointsOnFace(tempFaceVertices, 10, attachmentPnts);    // TODO(ismail): additional improvement. The points on the same face should not be too close to each other.
                                                                                       // ... Perhaps include a check if they are and then move them apart
                    // Now check against each cell in the tempSelectionCellList, if:
                    //                  1) The point is not too close to another lone cell or root cell of a cell lineage or to any face of the head bone cells
                    //                  2) the point is not inside another cell or root cell of a cell lineage
                    for (QVector3D attPnt : attachmentPnts)
                    {
                        for (int c=0; c<tempSelectableCellList.length(); c++)
                        {
                            if (c != i)
                            {
                                for (int f=0; f<6; f++)     // check this point's vicinity against each face of this cell
                                {
                                    QVector3D midPnt = tempSelectableCellList[c]->getMidPointOfFace(f);
                                    QVector3D faceNormal = tempSelectableCellList[c]->getNormalVectorOfBoxFace(f);
                                    float distToFace = attPnt.distanceToPlane(midPnt,faceNormal);
                                    if (distToFace <= 5.f)
                                    {
                                        obstructionFound = false; // red light (so far) - obstruction found
                                    }
                                    else
                                    {
                                        obstructionFound = false; // green light (so far) - no obstruction found
                                        attachPnt = attPnt;
                                        break;
                                    }
                                }
                                if (obstructionFound)
                                    break;      // we've found an obstruction, so no need to check against any more cells
                                // IF no obstruction found, we need to keep checking with other cells
                            }
                        }
                        if (!obstructionFound) // there is no obstruction (GREEN LIGHT) and we exit the loop to proceed with cell lineage generation
                            break;             // This is the point which will serve as the origin point
                        // ELSE otherwise check for the other points
                    }
                    if (!obstructionFound) // we have a point that is not obstructed with any other cell, so we'll use this point as our
                                           // origin for the cell lineage
                    {
                        // NOW GENERATE THE LONE CELL
                        float startXScal = GeometryAlgorithms::getRandomFloat(0.3,0.9);
                        float startYScal = GeometryAlgorithms::getRandomFloat(0.3,0.9);
                        float startZScal = GeometryAlgorithms::getRandomFloat(0.3,0.9);
                        Cell *newLoneCell = new Cell();
                        newLoneCell->isDenser = true;
                        selCellPtr->cloneCell(newLoneCell);
                        // Make sure we dont clone any of it's child cells
                        while (!newLoneCell->childCells.empty()) delete newLoneCell->childCells.last(), newLoneCell->childCells.removeLast();

                        if (!selCellPtr->childCells.contains(newLoneCell))
                            selCellPtr->childCells.append(newLoneCell);

                        // if (GeometryAlgorithms::getRandomInteger(0,2) == 1)
                        newLoneCell->setPivotFace(Box::BoxFace::BACK);
                        QVector3D faceNormal = selCellPtr->getNormalVectorOfBoxFace(faceInd);
                        GeometryAlgorithms::rotateCellTowardsVecDir(newLoneCell, faceNormal);
                        GeometryAlgorithms::scaleCell(newLoneCell, startXScal, startYScal, startZScal);
                        int pitch = GeometryAlgorithms::getRandomInteger(-50,50);
                        int yaw = GeometryAlgorithms::getRandomInteger(-50,50);
                        GeometryAlgorithms::rotateCellUsingEulerAngles(newLoneCell, pitch, yaw, 0);
                        GeometryAlgorithms::translateCell(newLoneCell, attachPnt - newLoneCell->getPivotPoint());

                        // // add this lone cell to the head's allLoneCells DS
                        // if (!headPtr->allLoneCells.contains(newLoneCell))
                        //     headPtr->allLoneCells.append(newLoneCell);
                        // Update the tempCellList with this lone cell, so it can be selected in future
                        if (!tempSelectableCellList.contains(newLoneCell))
                            tempSelectableCellList.append(newLoneCell);

                        break;
                    }
                    // else we'll reloop and select a new face where 10 new random candidate points will compete for becoming the origin of cell lineage
                }
                if (!obstructionFound)
                    break;  // we're done
            }
        }
        // else if (currentState == 3) // Mirror Cell Lineage around a plane
        // {
        //     // nothing to do atm
        // }
        // else if (currentState == 4) // Rotate cell
        // {
        //     // nothing to do atm
        // }
        // else if (currentState == 5) // Attach Extremity Digit to a cell/head
        // {
        // }
        // else if (currentState == 6) // smooth cell
        // {

        // }
        // else if (currentState == 7) // Clone/split cell & translate all cells to a corner/side by side
        // {
        // }
        // else if (currentState == 8) // Interwine cell lineage
        // {
        // }
        // else if (currentState == 9) // Make a bridge from one cell to another
        // {
        // }
        else if (currentState == 10 || currentState == 8 || currentState == 9) // Explode Cell
        {
            qDebug() << "Current State : " << currentState;

            int randInd = GeometryAlgorithms::getRandomInteger(0, tempSelectableCellList.length());
            Cell *selCellPtr = tempSelectableCellList[randInd];

            // NOW GENERATE THE EXPLODED CELL
            selCellPtr = explodeCell(selCellPtr, 0.f, QVector3D(0.4,0.4,0.4), GeometryAlgorithms::getRandomInteger(0,9));
            // // This cell can be part of the lone cell because this cell has been exploded (where the corner cells become child cells)
            // if (!headPtr->allLoneCells.contains(selCellPtr))
            //     headPtr->allLoneCells.append(selCellPtr);

            // Add all child cells of this exploded cell to be added to the selectable cells
            QVector<Cell*> allChildCells; allChildCells.clear();
            selCellPtr->getAllChildCellsRecursively(allChildCells);
            for (Cell *cPtr : allChildCells)
            {
                if (!tempSelectableCellList.contains(cPtr))
                    tempSelectableCellList.append(cPtr);
                else if (allChildCells.length() > 1)                // TODO: Inspect this code - its ambigous
                    tempSelectableCellList.removeAll(cPtr);
            }
        }

        // select a random state from the QPolygon of the current state
        QPolygon tempPoly = alienPtr->alienDNA->skeletonDNA->headStateMachine.value(currentState);
        int randPnt = GeometryAlgorithms::getRandomInteger(0,tempPoly.length());
        if (energy % 2 == 0)    // This makes sure we select a random element from the QPoint(x,y) i.e. either x() or y()
            currentState = tempPoly.at(randPnt).x();
        else
            currentState = tempPoly.at(randPnt).y();

        tempPoly.clear();
        energy -= GeometryAlgorithms::getRandomInteger(1,8);

        qDebug() << "Energy : " << energy;
    }

    // GeometryAlgorithms::translateBP(headPtr, 10, 0, 0);
    // Finally mirror/symmetrify the Head
    QVector<Cell*> tempCellList_1; tempCellList_1.clear();
    QVector<Cell*> tempCellListToMirror; tempCellListToMirror.clear();
    headPtr->getBodyPartCellList(tempCellList_1);   // This gets all the cells in this BP including their child cells (if any)
    // Now clone all these cells and mirror it
    for (Cell *cellToClone : tempCellList_1)
    {
        Cell *newCLoneCell = new Cell();
        // newLoneCell->isDenser = true;
        cellToClone->cloneCell(newCLoneCell);
        // Make sure we dont clone any of it's child cells
        while (!newCLoneCell->childCells.empty()) delete newCLoneCell->childCells.last(), newCLoneCell->childCells.removeLast();

        tempCellListToMirror.append(newCLoneCell);
        // firstBone->cellList.append(newCLoneCell);
    }
    qDebug() << tempCellListToMirror.length();
    GeometryAlgorithms::reflectCellListAroundPlane(tempCellListToMirror, QVector3D(0,-100,100), QVector3D(0,-100,-100), QVector3D(0,100,100), QVector3D(1,0,0));
    // firstBone->cellList.append(tempCellListToMirror);
    firstBone->rootCell->childCells.append(tempCellListToMirror);

    qDebug() << "New Head modelled successfully";

    return headPtr;
}

void AlienModeller::stretchHead(Alien *alienPtr, BodyPart *headPtr)
{
    // use the values from headStretchingDNA to stretch the Head
    /*  ALGO
     *
     * - Get all the joints in order
     * - Find the number of joints from the first joint and from the last joint (in reverse)
     *   ... according to the %age of neighboring joints
     * - Translate the joints accordingly
    */

    QPolygon tempStretchDNA_Poly = alienPtr->alienDNA->skeletonDNA->headStretchingDNA.value(headPtr->BP_ID);
    QVector<int> jntLineageList; jntLineageList.clear();
    QPolygon *tempJnt2JntPoly = alienPtr->alienSkeleton.Jnt2JntRelations_TypeGuided.value(headPtr->BP_ID);

    convertJnt2JntPolyToJointsLineageList(tempJnt2JntPoly, jntLineageList);
    qDebug() << jntLineageList;
    qDebug() << tempStretchDNA_Poly;
//    exit(0);
    int numOfNeighbourJnts = static_cast<int>((tempStretchDNA_Poly[0].y() / 100.f)
                                * jntLineageList.length());
    float startStretchEnergy = static_cast<float>(tempStretchDNA_Poly[1].x());

    if (numOfNeighbourJnts == 0)
    {
        qDebug() << "Num of neighbouring Jnts not calculated correctly";
        exit(0);
    }

    BodyPart *tempJntPtr = getBodyPartFromID(alienPtr,jntLineageList[0]);
    // Use the joint's cellbox's top face's normal vector as the stretching vector
//    QVector3D tempStretchVec = tempJntPtr->cellList[0]->getNormalVectorOfBoxFace(Box::BoxFace::TOP);
    BodyPart *tempJnt1 = getBodyPartFromID(alienPtr,jntLineageList[numOfNeighbourJnts/2]);
    BodyPart *tempJnt2 = getBodyPartFromID(alienPtr,jntLineageList[(numOfNeighbourJnts/2) + 2]);
    QVector3D tempJnt1Loc = alienPtr->alienSkeleton.vectorLocationCoupleForEachJoint_TypeGuided.value(jntLineageList[numOfNeighbourJnts/2]).second;
    QVector3D tempJnt2Loc = alienPtr->alienSkeleton.vectorLocationCoupleForEachJoint_TypeGuided.value(jntLineageList[numOfNeighbourJnts/2 + 1]).second;
//    qDebug() << tempJnt1->getPivotPoint();
//    qDebug() << tempJnt1Loc;
//    qDebug() << tempJnt2->getPivotPoint();
//    qDebug() << tempJnt2Loc;
//        exit(0);
    QVector3D tempStretchVec = tempJnt1Loc - tempJnt2Loc;
//    qDebug() << tempStretchVec.normalized();
//    qDebug() << alienPtr->alienSkeleton.vectorLocationCoupleForEachJoint_TypeGuided.value(jntLineageList[numOfNeighbourJnts/2 + 1]).first;
//    exit(0);
//    QVector3D tempStretchVec = tempJnt1->getPivotPoint() - tempJnt2->getPivotPoint();
    tempStretchVec.normalize();

//    GeometryAlgorithms::scaleBP(tempJnt1, 4, 4, 4);
//    GeometryAlgorithms::scaleBP(tempJnt2, 4, 4, 4);


    for (int i=0,j=jntLineageList.length()-1; i<numOfNeighbourJnts/2; i++,j--)
    {
        BodyPart *jnt1 = getBodyPartFromID(alienPtr,jntLineageList[i]);
        BodyPart *jnt2 = getBodyPartFromID(alienPtr,jntLineageList[j]);

        qDebug() << startStretchEnergy;

        QVector3D newJntLoc1 = jnt1->getPivotPoint() + (startStretchEnergy * tempStretchVec);

        QVector3D newJntLoc2 = jnt2->getPivotPoint() + (startStretchEnergy * tempStretchVec);

        // Now translate the Jnts to the new locations
//        GeometryAlgorithms::translateCell(jnt1->cellList[0], newJntLoc1 - jnt1->getPivotPoint());
        GeometryAlgorithms::translateBP(jnt1,
                                        newJntLoc1.x() - jnt1->getPivotPoint().x(),
                                        newJntLoc1.y() - jnt1->getPivotPoint().y(),
                                        newJntLoc1.z() - jnt1->getPivotPoint().z());

//        GeometryAlgorithms::translateCell(jnt2->cellList[0], newJntLoc2 - jnt2->getPivotPoint());

        GeometryAlgorithms::translateBP(jnt2,
                                        newJntLoc2.x() - jnt2->getPivotPoint().x(),
                                        newJntLoc2.y() - jnt2->getPivotPoint().y(),
                                        newJntLoc2.z() - jnt2->getPivotPoint().z());

//        GeometryAlgorithms::scaleBP(jnt1, 4, 4, 4);
//        GeometryAlgorithms::scaleBP(jnt2, 4, 4, 4);

        startStretchEnergy *= tempStretchDNA_Poly[1].y()/100.f;
//        startStretchEnergy /= 2;
    }
}

void AlienModeller::alignHeadSubBPs(Alien *alienPtr, int headID)
{
    /*  ALGORITHM: (uses the Head Sub BP Alignment DNA)
     *
     *  Translate the Maxilla
     *  Rotate the Maxilla
     *  Translate the Mandible
     *  Rotate the Mandible
     *  Rotate the Zygomatic in roll angle 90 degrees
     *  Rotate the Zygomatic in roll angle according to the DNA
     *  Translate the Zygomatic according to the DNA
    */
//    qDebug() << "About to align Head Sub BPs";

//    if (alienPtr->alienDNA->skeletonDNA->headSubBP_AlignmentDNA.contains(headID))
//    {
//        QPolygon tempPoly = alienPtr->alienDNA->skeletonDNA->headSubBP_AlignmentDNA.value(headID);
//        qDebug() << tempPoly;

//        BodyPart *headPtr = getBodyPartFromID(alienPtr, headID);
//        QVector<BodyPart *> maxillaList; maxillaList.clear();
//        QVector<BodyPart *> mandibleList; mandibleList.clear();
//        QVector<BodyPart *> zygomaticList; zygomaticList.clear();
//        getAllHeadSubBPs(alienPtr, headPtr, AlienModeller::BodyPartsEnum::HEADSubBP_Maxilla, maxillaList);
//        qDebug() << maxillaList.length();
//        getAllHeadSubBPs(alienPtr, headPtr, AlienModeller::BodyPartsEnum::HEADSubBP_Mandible, mandibleList);
//        qDebug() << mandibleList.length();
//        getAllHeadSubBPs(alienPtr, headPtr, AlienModeller::BodyPartsEnum::HEADSubBP_Zygomatic, zygomaticList);
//        qDebug() << zygomaticList.length();

//        for (BodyPart *maxPtr : maxillaList)
//        {
//            GeometryAlgorithms::translateBP(maxPtr, 0, tempPoly[0].x(), 0);
//            GeometryAlgorithms::rotateBodyPartUsingEulerAngles(*maxPtr, tempPoly[1].y(), 0, 0);
//        }
//        for (BodyPart *manPtr : mandibleList)
//        {
//            GeometryAlgorithms::translateBP(manPtr, 0, (-1)*tempPoly[0].y(), 0);
//            GeometryAlgorithms::rotateBodyPartUsingEulerAngles(*manPtr, tempPoly[2].y(), 0, 0);
//        }
//        for (BodyPart *zygPtr : zygomaticList)
//        {
//            // TODO(ismail): specify an algorithm and DNA entries that will place the zygomatic close to the
//            // ... maxilla and mandibles.
//            // ... The following are hard coded version of placing the Zygomatic. MAKE IT BETTER!!
//            // Get the main bone of the Head. The Head is composed of only one Bone
//            BodyPart *bonePtr=nullptr;
//            for (BodyPart *bone : headPtr->subBodyParts)
//            {
//                if (bone->BP_Type == AlienModeller::BodyPartsEnum::BONE)
//                {
//                    bonePtr = bone;
//                    break;
//                }
//            }
//            QVector3D zygPlacementPnt = bonePtr->cellList[0]->getMidPointOfFace(Box::BoxFace::LEFT) +
//                                        (3*bonePtr->cellList[0]->getNormalVectorOfBoxFace(Box::BoxFace::LEFT));

//            GeometryAlgorithms::translateBP(zygPtr,
//                                            zygPlacementPnt.x()-zygPtr->getPivotPoint().x(),
//                                            zygPlacementPnt.y()-zygPtr->getPivotPoint().y(),
//                                            zygPlacementPnt.z()-zygPtr->getPivotPoint().z());

//            // yaw angle rotation
//            GeometryAlgorithms::rotateBodyPartUsingEulerAngles(*zygPtr, 0, tempPoly[2].x(), 0);
//        }
//        qDebug() << "Head Sub BPs aligned successfully";
//    }
//    else
//        qDebug() << "No entry for the Head found in Alignment DNA";
}

void AlienModeller::buildCompleteHeadSubBP_DensificationDNA(Alien *alienPtr, int headID)
{
    qDebug() << "Starting to build Head Sub BP Densification DNA";

    // First we need to store this entry to the gen densification DNA
    // Go through the BP_GenDensificationDNA and check if the BPs entry is not there
    // ... if not present, copy one directly from the ruleset
    if (!alienPtr->alienDNA->skeletonDNA->HeadSubBP_GenDensification_DNA.contains(headID)) // if not present, add one
    {
        alienPtr->alienDNA->skeletonDNA->HeadSubBP_GenDensification_DNA.insert(headID, skeletonRuleSet->HeadSubBP_Densification_RS);
    }

    if (!alienPtr->alienDNA->skeletonDNA->HeadSubBP_SpecificDensification_DNA.contains(headID))
    {
        BodyPart *headPtr = getBodyPartFromID(alienPtr, headID);

        QVector<BodyPart *> maxillaList; maxillaList.clear();
        QVector<BodyPart *> mandibleList; mandibleList.clear();
        QVector<BodyPart *> zygomaticList; zygomaticList.clear();
        getAllSubBPsOfGivenType(alienPtr, headPtr, AlienModeller::BodyPartsEnum::HEADSubBP_Maxilla, maxillaList);
        getAllSubBPsOfGivenType(alienPtr, headPtr, AlienModeller::BodyPartsEnum::HEADSubBP_Mandible, mandibleList);
        getAllSubBPsOfGivenType(alienPtr, headPtr, AlienModeller::BodyPartsEnum::HEADSubBP_Zygomatic, zygomaticList);

        for (BodyPart *maxPtr : maxillaList)
        {
            createHeadSubBP_DensificationDNA(alienPtr, headID, maxPtr);
        }
        for (BodyPart *manPtr : mandibleList)
        {
            createHeadSubBP_DensificationDNA(alienPtr, headID, manPtr);
        }
        for (BodyPart *zygPtr : zygomaticList)
        {
            createHeadSubBP_DensificationDNA(alienPtr, headID, zygPtr);
        }
    }
    qDebug() << "Densification DNA built successfully";
}

void AlienModeller::createHeadSubBP_DensificationDNA(Alien *alienPtr, int headID, BodyPart *subPtr)
{
    QPolygonF tempRulePoly = alienPtr->alienDNA->skeletonDNA->HeadSubBP_GenDensification_DNA.value(headID);
    QMap<int, QPolygonF> tempMap; tempMap.clear();

    // store all the bones of the Head Sub BPs in the tempBoneList array
    QVector<Bone*> tempBoneList; tempBoneList.clear();
    // TODO(ismail): make a seperate method that takes care of the for loop below i.e.
    // ... creates the densification for the incoming Head Sub BP
    if (this->getAllBP_BonesInOrder(alienPtr, subPtr, tempBoneList))
    {
//                if (tempBoneList.length() <= 2)
//                {
//                    qDebug() << "Need atleast 3 bones to build densification DNA. Consider increasing joints or smoothing";
//                    continue;
//                }
        // select its mid bone
        int midInd = floorf(tempBoneList.length()/2);
        if (tempBoneList.length() <= 2)
            midInd = 0;

        float isThisBonePivot = 1, numOfBonesOnEitherSides = -1;

        bool kIsDecreasing=false;
        float densCnt = tempRulePoly[3].y();
        for (int k=midInd; k<tempBoneList.length() && k>=0; )
        {
            qDebug() << "reached";
            QPolygonF tempPoly; tempPoly.clear();

            densCnt /= tempRulePoly[3].x();

            float xScaling = GeometryAlgorithms::getRandomFloat(tempRulePoly[4].x(),tempRulePoly[4].y());
            float yScaling = GeometryAlgorithms::getRandomFloat(tempRulePoly[5].x(),tempRulePoly[5].y());
            float zScaling = GeometryAlgorithms::getRandomFloat(tempRulePoly[6].x(),tempRulePoly[6].y());

            tempPoly    << QPointF(isThisBonePivot,numOfBonesOnEitherSides)
                        << QPointF(1 /*RIGHT edge*/ ,(GeometryAlgorithms::getRandomFloat(0,tempRulePoly[2].y()))/100)

                                                                            /*number of densers stacked. min=20, max=30*/
                        << QPointF(tempRulePoly[3].x() /*Protrusion level*/,ceilf(densCnt))

                        // definitely need these
                        << QPointF(xScaling,xScaling)
                        << QPointF(yScaling,yScaling)
                        << QPointF(zScaling,zScaling)

                        // P6 - roll angle for lineage
                        << QPointF(GeometryAlgorithms::getRandomFloat(0,tempRulePoly[7].x()),
                                floorf(GeometryAlgorithms::getRandomFloat(0,tempRulePoly[7].y())))
                        // P7             (should roll angle change after certain num of densers? (Y/N), %age of densers after which the roll angle will increase/change)
                        << QPointF(GeometryAlgorithms::getRandomFloat(tempRulePoly[8].x(),tempRulePoly[8].y()),
                                floorf(GeometryAlgorithms::getRandomFloat(tempRulePoly[9].x(),tempRulePoly[9].y())))
                        // P8             (pitch angle for lineage, yaw angle for lienage)
                        << QPointF(GeometryAlgorithms::getRandomFloat(2,tempRulePoly[10].x()),
                                floorf(GeometryAlgorithms::getRandomFloat(0,1)))
                        // P9             (should pitch angle change after certain num of densers? (Y/N), %age of densers after which the pitch angle will increase/change)
                        << QPointF(GeometryAlgorithms::getRandomFloat(tempRulePoly[11].x(),tempRulePoly[11].y()),
                                floorf(GeometryAlgorithms::getRandomFloat(tempRulePoly[12].x(),tempRulePoly[12].y())))
                        // P10            (should yaw angle change after certain num of densers? (Y/N), %age of densers after which the yaw angle will increase/change)
                        << QPointF(GeometryAlgorithms::getRandomFloat(tempRulePoly[13].x(),tempRulePoly[13].y()),
                                floorf(GeometryAlgorithms::getRandomFloat(tempRulePoly[14].x(),tempRulePoly[14].y())))
                    ;

            if (subPtr->BP_Type == AlienModeller::BodyPartsEnum::HEADSubBP_Mandible)
            {
                tempPoly[8].setX(360-tempPoly[8].x());
            }

            tempMap.insert(tempBoneList[k]->BP_ID, tempPoly);

            isThisBonePivot = 0;

            if (k == tempBoneList.length()-1)
            {
                k = midInd;
                kIsDecreasing = true;
                densCnt = tempRulePoly[3].y(); // reset to the original number of densers
            }
            if (kIsDecreasing)
                k--;
            else
                k++;
        }
    }
    else
    {
        qDebug() << "Bones for this BP not extracted in order";
        exit(0);
    }
    alienPtr->alienDNA->skeletonDNA->HeadSubBP_SpecificDensification_DNA.insert(subPtr->BP_ID,tempMap);
}

void AlienModeller::densifyHeadSubBPs(Alien *alienPtr, int headID)
{
    // **ALGO**
    /*
     *  Phase 1
     *  Phase 2
     *  Phase 3
    */
    QVector<QPair<Cell*,float>> cellAndAnglePairList; cellAndAnglePairList.clear();
//    QPolygonF cellAndAnglePairList; cellAndAnglePairList.clear();

    BodyPart *headPtr = getBodyPartFromID(alienPtr, headID);
    QVector<BodyPart *> maxillaList; maxillaList.clear();
    QVector<BodyPart *> mandibleList; mandibleList.clear();
    QVector<BodyPart *> zygomaticList; zygomaticList.clear();
    getAllSubBPsOfGivenType(alienPtr, headPtr, AlienModeller::BodyPartsEnum::HEADSubBP_Maxilla, maxillaList);
    getAllSubBPsOfGivenType(alienPtr, headPtr, AlienModeller::BodyPartsEnum::HEADSubBP_Mandible, mandibleList);
    getAllSubBPsOfGivenType(alienPtr, headPtr, AlienModeller::BodyPartsEnum::HEADSubBP_Zygomatic, zygomaticList);

    // PHASE 1 - simple densifications from the HeadSubBP_SpecificDensification_DNA
    for (BodyPart* hdSubPtr : maxillaList)
    {
        densifyBodyPart(alienPtr, hdSubPtr);
    }
    for (BodyPart* hdSubPtr : mandibleList)
    {
        densifyBodyPart(alienPtr, hdSubPtr);
    }
    for (BodyPart* hdSubPtr : zygomaticList)
    {
        densifyBodyPart(alienPtr, hdSubPtr);
    }
}

void AlienModeller::densifyBodyPart(Alien *alienPtr, BodyPart *bpPtr)
{
    QMap<int, QPolygonF> tempMap; tempMap.clear();
    if (alienPtr->alienDNA->skeletonDNA->HeadSubBP_SpecificDensification_DNA.contains(bpPtr->BP_ID))
    {
        // Since the BP may have been moved around and rotated, it is important to bring it back to the origin and unrotate it.
        // Bring the BP back to origin
        GeometryAlgorithms::translateBP_AccordingToInputTranslationMat(bpPtr, bpPtr->BP_TranslateMatrix.inverted());
        // Unrotate the BP (using inverse quaternion)
        GeometryAlgorithms::rotateBP_AccordingToInputRotationMat(bpPtr, bpPtr->BP_OrientationQuat.inverted());
        // update the cells box geometry
        GeometryAlgorithms::updateBP_CellPrimitiveGeometry(bpPtr);

        tempMap = alienPtr->alienDNA->skeletonDNA->HeadSubBP_SpecificDensification_DNA.value(bpPtr->BP_ID);
        // go through all the bones in this BP
        QMapIterator<int, QPolygonF> iter(tempMap);
        QVector<float> rollAngleList; rollAngleList.clear();
        while(iter.hasNext())
        {
            iter.next();
            QPolygonF tempPoly = iter.value();
            int numOfStackedDensers = tempPoly[2].y();
            Bone *bonePtr = dynamic_cast<Bone*>(getBodyPartFromID(alienPtr, iter.key()));
            Cell *baseCell = bonePtr->cellList[0];
            // generate all stacked cells (densers) on this Bone
            // First create densers from the LEFT face of the base cell box
            // The two 'for loops' are for generating the densers on the opposite faces (mirrored densers)
            // TODO(ismail): This can be done in a method. This is HARDCODED

            // Find the number of densers after which the pitch/yaw/roll angles will change

            // TODO(ismail): Make a method that will generate a new cell at the given face of the given cell
            // ... Also specify how many densers need to be generated. Also specify the angle of geneation, and
            // ... also the scaling values. Also specify the 'growth flow of the densers' which can be computed
            // ... by a series of angles (and whether it is Ascending or Descending).
            QPolygonF tempGrowthPoly; tempGrowthPoly.clear();
            tempGrowthPoly << QPointF(numOfStackedDensers, Box::BoxFace::LEFT)
                           << QPointF(0, 0)
                           << QPointF(tempPoly[6].x(), 0)
                           << QPointF(0, 0)
                           << QPointF(tempPoly[3].x(), tempPoly[4].x())
                           << QPointF(tempPoly[5].x(), -1);

            float rollAngle = 0;
            if (rollAngleList.isEmpty())
            {
                for (int j=0; j<100; j++)
                {
                    // tweak the roll angle
                    int randNum = GeometryAlgorithms::getRandomInteger(-100,100);
                    if (randNum < -50)
                        rollAngle = 360 - (tempGrowthPoly[2].x() + 20);
                    if (randNum >= -50 && randNum < 0)
                        rollAngle = tempGrowthPoly[2].x() + 20;
                    else if (randNum >= 0 && randNum < 50)
                        rollAngle = 360 - (tempGrowthPoly[2].x() + 0);
                    else if (randNum >= 50)
                        rollAngle = tempGrowthPoly[2].x() + 0;

                    rollAngleList << rollAngle;
                }
            }

            generateStackedDensersFromCell(bonePtr, baseCell, tempGrowthPoly, rollAngleList);

//            tempGrowthPoly.clear();
//            tempGrowthPoly << QPointF(4, Box::BoxFace::LEFT)
//                           << QPointF(0, 0)
//                           << QPointF(tempPoly[6].x(), 0)
//                           << QPointF(0, 0)
//                           << QPointF(tempPoly[3].x(), tempPoly[4].x())
//                           << QPointF(tempPoly[5].x(), -1);

//            generateStackedDensersFromCell(bonePtr, baseCell, tempGrowthPoly);

             // *** HERE I HAVE COMMENTED OUT THE PART THAT GROWS THE MIRRORED DENSERS ***
//                 // Now create densers from the RIGHT face of the base cell box
//                 // This is so that the densers are created in a mirror like fashion
//                 baseCell = bonePtr->cellList[0];
//                 for (int j=0; j<numOfStackedDensers; j++)
//                 {
//                     Cell *newClonedCell = new Cell();
//                     baseCell->cloneCell(newClonedCell);
//                     newClonedCell->setPivotFace(Box::BoxFace::LEFT);

//                     QVector3D attachPoint = baseCell->cellBox->vertices[baseCell->cellBox->faces[Box::BoxFace::RIGHT]->vertNorIndices[1]]
//                               +
//                               (0.5 * (baseCell->cellBox->vertices[baseCell->cellBox->faces[Box::BoxFace::RIGHT]->vertNorIndices[3]]
//                               - baseCell->cellBox->vertices[baseCell->cellBox->faces[Box::BoxFace::RIGHT]->vertNorIndices[1]]));

//                    GeometryAlgorithms::translateCell(newClonedCell, attachPoint - newClonedCell->getPivotPoint());
////                     GeometryAlgorithms::rotateCellTowardsVecDir(newClonedCell, baseCell->getPivotVector());
//                    GeometryAlgorithms::rotateCellUsingEulerAngles(newClonedCell, 10, 355, 360-tempPoly[6].x());
//                    GeometryAlgorithms::scaleCell(newClonedCell, tempPoly[3].x(), tempPoly[4].x(), tempPoly[5].x());

//                     bonePtr->cellList.append(newClonedCell);

//                     baseCell = newClonedCell;
//                 }
         }

         // TODO(ismail): Now the system needs to grow a zygomatic bone from the left face of the
         // ... maxilla base denser. If there are more than one base densers, then the left most denser is
         // ... used to the generate the zygomatic denser.


         // rotate it back to its original orientation
         GeometryAlgorithms::rotateBP_AccordingToInputRotationMat(bpPtr, bpPtr->BP_OrientationQuat);
         // take it back to its actual location
         GeometryAlgorithms::translateBP_AccordingToInputTranslationMat(bpPtr, bpPtr->BP_TranslateMatrix);
         // update the cells box geometry
         GeometryAlgorithms::updateBP_CellPrimitiveGeometry(bpPtr);
    }
    else
        qDebug() << "Entry for this BP is not in BP_SpecificDensificationDNA";
}

void AlienModeller::densifyHead(Alien *alienPtr, BodyPart *headPtr)
{
    QVector<Bone*> bonesPtrList; bonesPtrList.clear();
    if (getAllBP_BonesInOrder(alienPtr, headPtr, bonesPtrList))
    {
        // For each bone call the generateStackedDensersFromCell
        QPolygonF tempGrowthPoly; tempGrowthPoly.clear();
        tempGrowthPoly << QPointF(2, Box::BoxFace::LEFT) // (num of stacked densers, face of base denser on which the stacked denser will be generated)
                       << QPointF(0, 0)     // (pitch angle, yaw angle)
                       << QPointF(1.5, 0)    // (roll angle, should it mirror) "NOTE: Keep the roll angle b/w 1 and 2.5" for some interesting results
                       << QPointF(0, 0)     // (%age at which the angle will change, angle change amount)
                       << QPointF(1.6, 1)    // (x scaling value, y scaling value)
                       << QPointF(1, -1); // (z scaling value, -1)

        QVector<float> rollAngleList = QVector<float>(tempGrowthPoly[0].x(), tempGrowthPoly[2].x());
        Cell *baseCell = nullptr;

        int initLen = bonesPtrList.length();
        QPair<Cell*,Cell*> facingCellsForScaling = QPair<Cell*,Cell*>(nullptr,nullptr); // just for chekcing
        int tempCnt = 0;
        do
        {
            // First generate the stacked densers on all the bones one by one
            for (Bone *bonePtr : bonesPtrList)
            {
//                QVector<Cell*> lDsr = bonePtr->getAllLeafDensers();
//                if (lDsr.isEmpty())
//                {
//                    baseCell = bonePtr->cellList[0];
//                }
//                else
//                    baseCell = lDsr.last();
                baseCell = bonePtr->getLeafDenser();
                if (baseCell == nullptr)
                    baseCell = bonePtr->cellList[0];

                generateStackedDensersFromCell(bonePtr, baseCell, tempGrowthPoly, rollAngleList);
            }

            tempGrowthPoly[4].setX(1);

//            // Now check if the leaf densers are not too close to opposite facing leaf densers
//            // Leaf densers are the densers that are the last in the stacked denser lineage
//            // First find the leaf densers of each bone as follows
//            QVector<QVector<Cell*>> allLeafDensers; allLeafDensers.clear();
//            allLeafDensers.resize(1000);
//            for (Bone *bonePtr : bonesPtrList)
//            {
//                allLeafDensers[bonePtr->BP_ID].append(bonePtr->getAllLeafDensers());
//            }
//            // TODO(ismail): Its a good idea to remove the entries which have empty lists
//            // ... i.e. whose indexes are not the IDs of the bones in bonesPtrList

//            QPair<Bone*,Bone*> facingBones;
//            for (int i=0; i<bonesPtrList.length(); i++)
////            for (Bone *bonePtr1 : bonesPtrList)
//            {
//                float smallAngle = 90.f;
//                float smallDist = 210.f;
//                // At the moment we're only considering there is only one denser lineage on a bone
//                Cell *cell_1_Ptr = allLeafDensers[bonesPtrList[i]->BP_ID][0];
//                // We again go through each bone and check whether the leaf denser of bonePtr1 is not too
//                // ... close to bonePtr1
//                QVector3D tempNor = cell_1_Ptr->getNormalVectorOfBoxFace(Box::BoxFace::LEFT);
//                qDebug() << "reached 1";
//                for (int j=0; j<bonesPtrList.length(); j++)
////                for (Bone *bonePtr2 : bonesPtrList)
//                {
//                    if (bonesPtrList[i] != bonesPtrList[j])
//                    {
//                        Cell *cell_2_Ptr = allLeafDensers[bonesPtrList[j]->BP_ID][0];
//                        // Now check if the angle between the normal vector of cell_1_Ptr left
//                        // and the vector (cell_2_Ptr.leftFace_centerPt - cell_1_Ptr.leftFace_centerPt)
//                        // is less than the previous vector
//                        // Also check of the distance is not more than certain amount
//                        QVector3D tempVecBetweenPivPnts = cell_2_Ptr->getPivotPoint() - cell_1_Ptr->getPivotPoint();
//                        qDebug() << "reached 2";
//                        tempNor.normalize();
//                        tempVecBetweenPivPnts.normalize();
//                        float angle = acos(QVector3D::dotProduct(tempNor, tempVecBetweenPivPnts)) * 57.2957795;
//                        if (angle < smallAngle)
//                        {
//                            qDebug() << "Angle : " << angle;
//                            smallAngle = angle;
//                            // Also check the distance between their left faces
//                            QVector3D pt1 = cell_1_Ptr->getMidPointOfFace(Box::BoxFace::LEFT);
//                            QVector3D pt2 = cell_2_Ptr->getMidPointOfFace(Box::BoxFace::LEFT);

//                            qDebug() << "reached 3";

//                            float distBetweenFacingBones = pt1.Point(pt2);
//                            qDebug() << "Distance B/W facing Bones : " << distBetweenFacingBones;
//                            if (distBetweenFacingBones < smallDist)
//                            {
//                                qDebug() << "reached 4";
//                                facingBones.first = bonesPtrList[i];
//                                facingBones.second = bonesPtrList[j];

//                                facingCellsForScaling.first = cell_1_Ptr;
//                                facingCellsForScaling.second = cell_2_Ptr;
//                                qDebug() << "leaf Densers are facing and too close so we'll stop here";
//                            }
//                            else
//                                qDebug() << "leaf Densers are facing but not too close";
//                        }
//                    }
//                }
//            }

//            qDebug() << "facingBones : " << facingBones.first->BP_ID << "," << facingBones.second->BP_ID;


//            // Now that we have found the facing bone, we need to remove these bones from the
//            // ... bonePtrList so these are not considered for growing more densers
//            bonesPtrList.removeOne(facingBones.first);
//            bonesPtrList.removeOne(facingBones.second);

//            allLeafDensers.removeAt(facingBones.first->BP_ID);
//            allLeafDensers.removeAt(facingBones.second->BP_ID);

//            qDebug() << "New length of bonesPtrList is : " << bonesPtrList.length();
            tempCnt++;
        } while(tempCnt < 20);


        tempGrowthPoly.clear();
        tempGrowthPoly << QPointF(2, Box::BoxFace::RIGHT) // (num of stacked densers, face of base denser on which the stacked denser will be generated)
                       << QPointF(0, 0)     // (pitch angle, yaw angle)
                       << QPointF(360-1.5, 0)    // (roll angle, should it mirror)
                       << QPointF(0, 0)     // (%age at which the angle will change, angle change amount)
                       << QPointF(1.6, 1)    // (x scaling value, y scaling value)
                       << QPointF(1, -1); // (z scaling value, -1)

        rollAngleList.clear();
        rollAngleList = QVector<float>(tempGrowthPoly[0].x(), tempGrowthPoly[2].x());
        baseCell = nullptr;

        tempCnt = 0;
        do
        {
            // First generate the stacked densers on all the bones one by one
            for (Bone *bonePtr : bonesPtrList)
            {
                if (tempCnt == 0)
                    baseCell = bonePtr->cellList[0];
                else
                    baseCell = bonePtr->getLeafDenser();
                generateStackedDensersFromCell(bonePtr, baseCell, tempGrowthPoly, rollAngleList);

            }
            tempGrowthPoly[4].setX(1);
            tempCnt++;
        } while(tempCnt < 20);

//        while((bonesPtrList.length()*100)/initLen > 30);

        // Now we need to create the sub joints lineages, which will hopefully enhance the shape of the Head
        // ... These will be densified as well.



        // Now generate the shapes for zygomatic curves, nasal bones, eye sockets, horns,
        // ... jaw/mandible extensions etc
//        QPolygon dentSpecs; dentSpecs.clear();
//        dentSpecs << QPoint(30,10)  // (%age of boneList length to select the pivot bone, %age of bones on either side of pivot bone)
//                  << QPoint(40,20)  // (%age of denser lineage length to select the pivot cell, %age of cells on either side of pivot cell)
//                  << QPoint(40,-1);  // (amount of bend i.e. scaling factor for unit vector, -1)
//                  // QPoint(1,10) // min and max number of dents on the Head
//                  // QPoint(1,4) // min and max amount of adjacent densers that act as pivot densers

//        createDentInDensers(headPtr, bonesPtrList, dentSpecs);

//        dentSpecs.clear();
//        dentSpecs << QPoint(50,95)  // (%age of boneList length to select the pivot bone, %age of bones on either side of pivot bone)
//                  << QPoint(80,60)  // (%age of denser lineage length to select the pivot cell, %age of cells on either side of pivot cell)
//                  << QPoint(90,-1);  // (amount of bend i.e. scaling factor for unit vector, -1)

//        createDentInDensers(headPtr, bonesPtrList, dentSpecs);


        // Now generate the protrusions (if any), based on the following DNA
        // These can also be called 'Horned Protrusions'
        QPolygonF protSpecs; protSpecs.clear();
        protSpecs << QPointF(5,2)  // (%age of boneList length to select the pivot bone, make a curvy formation or not i.e. 0=do not make curve formation; 1=make a curve formation)
                  << QPointF(10,10) // (%age of bones on upper side of pivot bone, %age of bones on lower side of pivot bone)
                  << QPointF(20,20)  // (%age of denser lineage length to select the pivot cell, %age of cells on either side of pivot cell)
                  << QPointF(1,10)   // (minimum num of densers stacked on top of the pivot denser, max num of densers stacked on top of the pivot denser)
                  << QPointF(10,2)  // (%age of densers to reduce on either side of the pivit denser on each subsequent bone, -1)
                  << QPointF(5,0)    // (pitch angle of stacked densers, roll angle of the stacked densers) These angles can make the formation of a bridge
                  << QPointF(2,1.5); // (Y scaling up/down trend i.e. 0 or 1, Y scaling value...value should be between 0.9 and 1.7)
                                      // Here scaling trend is: 0=descending, 1=ascending)
                    // Additional parameters...
                    // QPointF(min and max number of protrusions)
                    // QPointF(10, 50) --> min and max of stacked densers that can be grown on top of any of the peak densers
                    // QPointF(min and max z scaling factor)
                    // QPointF(min and max y scaling factor)
                    // QPointF(min and max roll angle)
                    // QPointF(min and max pitch angle)
                    // QPointF(min and max 1s that will happen for the changes to the pitch and roll angles for the stacked peak densers)


        for (int i=0; i<16; i++)
            createDenserProtrusions(headPtr, bonesPtrList, protSpecs);

        // CRATERS generation
//        createCratersInDensers(bonesPtrList, protSpecs);

        // *** Now generate some craters, some thorns, some ridges, and some horns...FUN STUFF !!
        // Curvy Ridge generation
        QPolygonF ridgeSpecs; ridgeSpecs.clear();
        ridgeSpecs << QPointF(15,15)  // (%age of boneList length to select the pivot bone, min and max number of times the densers are selected i.e. 4 to 9)
                  << QPointF(60,4)  // (%age of denser lineage length to select the pivot cell, 0 to 3 ==> code for selecting the denser. i.e. 0 = up, 1 = down, 2 = left, 3 = right)
                  << QPointF(40,100)   // (minimum num of densers stacked on top of the pivot denser, max num of densers stacked on top of the pivot denser)
                  << QPointF(6,12)    // (min and max roll angle of stacked densers)
                  << QPointF(2, 1.0) // (Y scaling up/down trend i.e. 0 or 1, Y scaling value)
                  << QPointF(2, 0.95); // (X scaling up/down trend i.e. 0 or 1, X scaling value)
                                      // Here scaling trend is: 0=descending, 1=ascending

        createCurvyRidgeOnDensers(headPtr, bonesPtrList, ridgeSpecs);

//        // JAW DEPRESSIONS
//        QPolygonF jawDepSpecs; jawDepSpecs.clear();
//        jawDepSpecs  << QPointF(65,80)  // (min and max %age of boneList length to select the pivot bone, min and max number of times the densers are selected i.e. 4 to 9)
//                  << QPointF(45,70)  // (min and max %age of denser lineage length to select the pivot cell)
//                  << QPointF(5,30)    // (min and max roll angle for jaw depression)
//                  << QPointF(2,-1) // (should the angle remain the same or change?...0=remain same, 1=angle changes, -1)
//                  << QPointF(60,100); // (min and max %age of 0s then 1s...0 represents a roll angle in ascending trend and 1 represents roll angle in descending trend)
//                                    // ... for this we'll create a list of 0s and 1s, where the 0 will appear for the %age of the remaining densers in the lineage of densers
//                                    // ... if the angle is not supposed to change, then do not find different 0s and 1s. Only use 0s

//        // ENDING LINEAGE THICKNESS
    }
}

QVector<QPolygon> AlienModeller::generateSubJntsLineage(Alien *alienPtr, int headID, QVector<Cell *> &cellList)
{
//    // first get the top face normals of the cells in the celllist
//    QVector<Joint*> jntList; jntList.clear();
//    QVector<QPair<QVector3D,QVector3D>> jntLocAnglePairList; jntLocAnglePairList.clear();
//    for (Cell *cellPtr : cellList)
//    {
//        QVector3D tempNorVec = cellPtr->getNormalVectorOfBoxFace(Box::BoxFace::TOP);
//        tempNorVec.normalize();

//        // Find a point at a certain scaling distance of the unit vector tempNorVec...say 50 or 300
//        QVector3D tempJntLoc = cellPtr->getMidPointOfFace(Box::BoxFace::TOP) + (50.f * tempNorVec);
//        QVector3D tempJntDir = cellPtr->getNormalVectorOfBoxFace(Box::BoxFace::BACK).normalized();
//        jntLocAnglePairList.append(QPair<QVector3D,QVector3D>(tempJntLoc,tempJntDir));

//        // Now create a joint at this location
//        jntList.append(instantiateNewJoint(alienPtr));
//    }

//    // Now get these joints lineage in the subHeadJnt2JntLineages of the head
//    // ... and assign it a unique key.
//    // ... This should be pretty straight forward as we just need to make parentJnt,ChildJnt pairs

//    // Now model the joints
//    qDebug() << "modelling all incoming joints";
//    // Now set this joint's location and create a cell for it
//    for (int i=0; i<jntList.length(); i++)
//    {
//        jntList[i]->makeCell(jntLocAnglePairList[i].first, 5); // create the joint's cell
//        // rotate the joint towards the newly computed vector: tempOrientVector
//        GeometryAlgorithms::rotateBodyPartTowardsVecDir(*jntList[i], jntLocAnglePairList[i].second);
//    }
//    qDebug() << "all joints modelled";

//    // Finally create and model the bones b/w the joints
}

void AlienModeller::createDentInDensers(BodyPart *bpPtr, QVector<Bone *> &boneOrderedList, QPolygon dentSpecs)
{
    // Find a bone at random
    int indexOfPivotBone = static_cast<int>((dentSpecs[0].x() / 100.f)
                                            * boneOrderedList.length());
//    qDebug() << indexOfPivotBone;

    int numOfBnsOnUpperSideOfPivBone = static_cast<int>((dentSpecs[0].y() / 100.f)
                                            * boneOrderedList.length()); // should be done from the remaining bones
    int numOfBnsOnLowerSideOfPivBone = static_cast<int>((dentSpecs[0].y() / 100.f)
                                            * boneOrderedList.length()); // should be done from the remaining bones

    QPair<QVector3D,QVector3D> ptPair;
    float scalingVal = static_cast<float>(dentSpecs.at(2).x());
    qDebug() << "scaling value : " << scalingVal;
    // Go through the bones on the upper side of the pivot bone
    int numDensOnEitherSidesOfPivDenser = static_cast<int>((dentSpecs[1].y() / 100.f)
                                          * boneOrderedList.length()); // TODO(ismail): get this from specs
    bool upperPartInProg = true;
    for (int nb=indexOfPivotBone; nb>=0 && nb<boneOrderedList.length(); )
    {
        if (nb == (indexOfPivotBone+numOfBnsOnUpperSideOfPivBone)  &&  upperPartInProg)
        {
            upperPartInProg = false;
            nb = indexOfPivotBone-1;
            numDensOnEitherSidesOfPivDenser = 6;
        }
        else if (nb == (indexOfPivotBone-numOfBnsOnLowerSideOfPivBone)-1)
            break;

        // Now find the pivot cell/denser for the pivot bone based on the dentSpecs[1].x()
        QVector<Cell*> denserLin_1 = QVector<Cell*>(boneOrderedList.at(nb)->allStackedDenserLineages);
//        denserLin_1 = boneOrderedList.at(nb)->allStackedDenserLineages.at(0);
        qDebug() << denserLin_1.length();
        int indexOfPivotDenser = static_cast<int>((dentSpecs[1].x() / 100.f)
                                                * denserLin_1.length());

        scalingVal = static_cast<float>(dentSpecs.at(2).x());
//        qDebug() << scalingVal;
//        exit(0);
        // First work on the densers on the right of pivot denser
        for (int i=indexOfPivotDenser; i<indexOfPivotDenser+numDensOnEitherSidesOfPivDenser && i<denserLin_1.length(); i++)
        {
            // Get the normal vector of this denser's top face
            QVector3D tempNorVec = denserLin_1.at(i)->getNormalVectorOfBoxFace(Box::BoxFace::TOP);
            tempNorVec.normalize();

            if (i == indexOfPivotDenser) // For the first bone, both left and right face will be used for finding points
            {
                // Find two points, one from right face and one from left face
                ptPair.first = denserLin_1.at(i)->getMidPointOfFace(Box::BoxFace::RIGHT);
                ptPair.second = denserLin_1.at(i)->getMidPointOfFace(Box::BoxFace::LEFT);
                ptPair.first += (scalingVal * tempNorVec);
                ptPair.second += (scalingVal * tempNorVec);
            }
            else
            {
                ptPair.first = ptPair.second;
                ptPair.second = denserLin_1.at(i)->getMidPointOfFace(Box::BoxFace::LEFT);
                ptPair.second += (scalingVal * tempNorVec);
            }

            // Now translate and rotate this cell accordingly
            QVector3D tempVec = ptPair.first - ptPair.second;
//            QVector3D tempMidPnt = ptPair.first + (0.5*tempVec);
//            QVector3D transVec = tempMidPnt - denserLin_1.at(i)->getCellCenterLocation();
//            QVector3D transVec = ptPair.first - denserLin_1.at(i)->getMidPointOfFace(Box::BoxFace::RIGHT);
            GeometryAlgorithms::translateCell(denserLin_1.at(i), ptPair.first - denserLin_1.at(i)->getPivotPoint());
//            GeometryAlgorithms::rotateCellTowardsVecDir(denserLin_1.at(i), tempVec);
            // Keep rotating it slighlty until the denser/cell's left and right faces aligns with both the
            // ... points
            if (i > indexOfPivotDenser)
            {
                GeometryAlgorithms::rotateCellUsingEulerAngles(denserLin_1.at(i), 0, 0, -20);
            }

            float xScal = 1.f;
            do
            {
                xScal = (denserLin_1.at(i)->getMidPointOfFace(Box::BoxFace::RIGHT)
                                  - denserLin_1.at(i)->getMidPointOfFace(Box::BoxFace::LEFT)).length();
                GeometryAlgorithms::scaleCell(denserLin_1.at(i), 1.1, 1, 1);
            } while (tempVec.length() > xScal);

            // store the last affected denser in the boundaryDnsrs DS of this BP
            if (i == (indexOfPivotDenser+numDensOnEitherSidesOfPivDenser)-1
                    ||
                i == denserLin_1.length()-1)
            {
                bpPtr->boundaryDnsrs.append(denserLin_1.at(i));
            }

            // make the scaling val smaller
            scalingVal /= 1.2f; // this should also be part of the DNA

        }
        // Now work on the densers on left of pivot denser
        scalingVal = static_cast<float>(dentSpecs.at(2).x());
        for (int i=indexOfPivotDenser; i>indexOfPivotDenser-numDensOnEitherSidesOfPivDenser && i>=0; i--)
        {
            // Get the normal vector of this denser's top face
            QVector3D tempNorVec = denserLin_1.at(i)->getNormalVectorOfBoxFace(Box::BoxFace::TOP);
            tempNorVec.normalize();

            if (i == indexOfPivotDenser) // For the first bone, both left and right face will be used for finding points
            {
                // Find two points, one from right face and one from left face
                ptPair.first = denserLin_1.at(i)->getMidPointOfFace(Box::BoxFace::RIGHT);
                ptPair.second = denserLin_1.at(i)->getMidPointOfFace(Box::BoxFace::LEFT);
                ptPair.first += (scalingVal * tempNorVec);
                ptPair.second += (scalingVal * tempNorVec);
            }
            else
            {
                ptPair.first = ptPair.second;
                ptPair.second = denserLin_1.at(i)->getMidPointOfFace(Box::BoxFace::RIGHT);
                ptPair.second += (scalingVal * tempNorVec);
                // Now translate and rotate this cell accordingly
                QVector3D tempVec = ptPair.second - ptPair.first;
    //            QVector3D tempMidPnt = ptPair.first + (0.5*tempVec);
    //            QVector3D transVec = tempMidPnt - denserLin_1.at(i)->getCellCenterLocation();
    //            QVector3D transVec = ptPair.first - denserLin_1.at(i)->getMidPointOfFace(Box::BoxFace::RIGHT);
                GeometryAlgorithms::translateCell(denserLin_1.at(i), ptPair.second - denserLin_1.at(i)->getPivotPoint());
    //            GeometryAlgorithms::rotateCellTowardsVecDir(denserLin_1.at(i), tempVec);
                // Keep rotating it slighlty until the denser/cell's left and right faces aligns with both the
                // ... points

                GeometryAlgorithms::rotateCellUsingEulerAngles(denserLin_1.at(i), 0, 0, 20);

                float xScal = 1.f;
                do
                {
                    xScal = (denserLin_1.at(i)->getMidPointOfFace(Box::BoxFace::LEFT)
                                      - denserLin_1.at(i)->getMidPointOfFace(Box::BoxFace::RIGHT)).length();
                    GeometryAlgorithms::scaleCell(denserLin_1.at(i), 1.1, 1, 1);
                } while (tempVec.length() > xScal);

                // store the last affected denser in the boundaryDnsrs DS of this BP
                if (i == (indexOfPivotDenser-numDensOnEitherSidesOfPivDenser)
                        ||
                    i == 0)
                {
                    bpPtr->boundaryDnsrs.append(denserLin_1.at(i));
                }

                // make the scaling val smaller
                scalingVal /= 1.2f; // this should also be part of the DNA

            }
        }

        numDensOnEitherSidesOfPivDenser -= 1;
        if (upperPartInProg)
            nb++;
        else
            nb--;
    }
}

void AlienModeller::createDenserProtrusions(BodyPart *bpPtr, QVector<Bone *> &boneOrderedList, QPolygonF protSpecs)
{
    // Find a bone at random
    int indexOfPivotBone = static_cast<int>((protSpecs[0].x() / 100.f)
                                            * boneOrderedList.length());
//    qDebug() << indexOfPivotBone;

    int numOfBnsOnUpperSideOfPivBone = static_cast<int>((protSpecs[1].x() / 100.f)
                                            * boneOrderedList.length());
    qDebug() << numOfBnsOnUpperSideOfPivBone;
    int numOfBnsOnLowerSideOfPivBone = static_cast<int>((protSpecs[1].y() / 100.f)
                                            * boneOrderedList.length());

    int numOfDensersToStackOnTop = GeometryAlgorithms::getRandomInteger(protSpecs[3].x(), protSpecs[3].y());

    // Go through the bones on the upper side of the pivot bone
    int numDensOnEitherSidesOfPivDenser = 5;
    int indexOfPivotDenser = 0;
    QVector<Cell*> denserLin_1; denserLin_1.clear();
    QMap<QPair<int,int>, Cell*> tempStckDnsrIdentifierMap; tempStckDnsrIdentifierMap.clear();
    Cell *baseCellPtr = nullptr;

    // First check whether the final denser formation will overlap any of the previous formations' boundary densers
    // If yes, find another spot
    bool thereIsOverlap = false;
    if (!bpPtr->boundaryDnsrs.isEmpty())
    {
        int cnt=0;
        do // keep checking as long as there is an overlap
        {
            thereIsOverlap = false;
            indexOfPivotBone = static_cast<int>((protSpecs[0].x() / 100.f)
                                                    * boneOrderedList.length());
            numOfBnsOnUpperSideOfPivBone = static_cast<int>((protSpecs[1].x() / 100.f)
                                                    * boneOrderedList.length());

            for (int nb=indexOfPivotBone; nb<indexOfPivotBone+numOfBnsOnUpperSideOfPivBone && nb<boneOrderedList.length(); nb++)
            {
                // Now find the pivot cell/denser for the pivot bone based on the dentSpecs[1].x()
                denserLin_1 = QVector<Cell*>(boneOrderedList.at(nb)->allStackedDenserLineages);
                indexOfPivotDenser = static_cast<int>((protSpecs[2].x() / 100.f)
                                                        * denserLin_1.length());
                int tempRemainingDensers = denserLin_1.length() - indexOfPivotDenser;
                numDensOnEitherSidesOfPivDenser = static_cast<int>((protSpecs[2].y() / 100.f)
                                                        * tempRemainingDensers);

                Cell *cell_1 = denserLin_1.at((indexOfPivotDenser+numDensOnEitherSidesOfPivDenser)-1);
                Cell *cell_2 = denserLin_1.at(indexOfPivotDenser-numDensOnEitherSidesOfPivDenser);
                if (bpPtr->boundaryDnsrs.contains(cell_1) || bpPtr->boundaryDnsrs.contains(cell_2))
                {
                    thereIsOverlap = true;
                    break;
                }
            }

            if (thereIsOverlap) // Change the ProtSpec with a new value for
                                // -    %age of boneList length to select the pivot bone
                                // -    %age of bones on upper side of pivot bone
                                // -    %age of denser lineage length to select the pivot cell
            {
                // fix it by using some other values for protSpecs
                if (protSpecs[0].x() >= 95)
                    protSpecs[0].setX(5);
                else
                    protSpecs[0].setX(protSpecs[0].x() + 10);
//                protSpecs[1] = QPointF(fabs(protSpecs[1].x() - 1), fabs(protSpecs[1].y() - 2));

                if (protSpecs[2].x() >= 80)
                    protSpecs[2].setX(20);
                else
                    protSpecs[2].setX(protSpecs[2].x() + 20);
//                protSpecs[2].setY(protSpecs[2].y() - 2);
            } // go back and check again
            cnt++;
        } while (thereIsOverlap || cnt<20); // keep finding a new spot(pivot denser) for formation generation
    }

    int tempCnt = 0;
    Cell *topMostDenser=nullptr;
    for (int d=0; d<=numOfDensersToStackOnTop; d++)
    {
        for (int nb=indexOfPivotBone; nb<indexOfPivotBone+numOfBnsOnUpperSideOfPivBone && nb<boneOrderedList.length(); nb++)
        {
            // Now find the pivot cell/denser for the pivot bone based on the dentSpecs[1].x()
                denserLin_1 = QVector<Cell*>(boneOrderedList.at(nb)->allStackedDenserLineages);
        //        denserLin_1 = boneOrderedList.at(nb)->allStackedDenserLineages.at(0);
    //            qDebug() << denserLin_1.length();
                indexOfPivotDenser = static_cast<int>((protSpecs[2].x() / 100.f)
                                                        * denserLin_1.length());
            if (nb == indexOfPivotBone)
            {
                int tempRemainingDensers = denserLin_1.length() - indexOfPivotDenser;
//                if (tempRemainingDensers <= 10)
//                    protSpecs[2].setY(protSpecs[2].y()*1.3);
//                else
//                    protSpecs[2].setY(20);
                numDensOnEitherSidesOfPivDenser = static_cast<int>((protSpecs[2].y() / 100.f)
                                                        * tempRemainingDensers);

//                if (d % 2 == 0)
                    numDensOnEitherSidesOfPivDenser -= d;
            }

            // First work on the densers on the right of pivot denser
            // We need to create a cloned denser on top of this denser, i.e. just stack it.
            for (int i=indexOfPivotDenser; i<indexOfPivotDenser+numDensOnEitherSidesOfPivDenser && i<denserLin_1.length(); i++)
            {
                QPolygonF tempGrowthPoly; tempGrowthPoly.clear();
                tempGrowthPoly << QPointF(1, Box::BoxFace::BOTTOM)
                               << QPointF(protSpecs[5].x(), 0)     // (pitch angle, yaw angle)
                               << QPointF(protSpecs[5].y(), 0)     // (roll angle, should it mirror)
                               << QPointF(0, 0)     // (%age at which the angle will change, angle change amount)
                               << QPointF(1, protSpecs[6].y())     // (x scaling value, y scaling value)
                               << QPointF(1, -1);   // (z scaling value, -1)

                QVector<float> rollAngleList = QVector<float>(tempGrowthPoly[0].x(), tempGrowthPoly[2].x());
                if (tempStckDnsrIdentifierMap.contains(QPair<int,int>(nb,i)))
                    baseCellPtr = tempStckDnsrIdentifierMap.value(QPair<int,int>(nb,i));
                else
                    baseCellPtr = denserLin_1.at(i);
                generateStackedDensersFromCell(boneOrderedList.at(nb), baseCellPtr, tempGrowthPoly, rollAngleList);

                topMostDenser = boneOrderedList.at(nb)->allStackedDenserLineages.last();
                tempStckDnsrIdentifierMap.insert(QPair<int,int>(nb,i), boneOrderedList.at(nb)->allStackedDenserLineages.last());

//                if (i == (indexOfPivotDenser+numDensOnEitherSidesOfPivDenser)-1
//                        ||
//                    i == denserLin_1.length()-1)
//                {
                if (d == 0)
                    bpPtr->boundaryDnsrs.append(baseCellPtr);
//                }

                boneOrderedList.at(nb)->allStackedDenserLineages.removeLast();
            }
            // Now work on the densers on the left of pivot denser
            // We need to create a cloned denser on top of this denser, i.e. just stack it.
            for (int i=indexOfPivotDenser-1; i>indexOfPivotDenser-numDensOnEitherSidesOfPivDenser && i>0; i--)
            {
                QPolygonF tempGrowthPoly; tempGrowthPoly.clear();
                tempGrowthPoly << QPointF(1, Box::BoxFace::BOTTOM)
                               << QPointF(protSpecs[5].x(), 0)     // (pitch angle, yaw angle)
                               << QPointF(protSpecs[5].y(), 0)     // (roll angle, should it mirror)
                               << QPointF(0, 0)     // (%age at which the angle will change, angle change amount)
                               << QPointF(1, protSpecs[6].y())     // (x scaling value, y scaling value)
                               << QPointF(1, -1);   // (z scaling value, -1)

                QVector<float> rollAngleList = QVector<float>(tempGrowthPoly[0].x(), tempGrowthPoly[2].x());
                if (tempStckDnsrIdentifierMap.contains(QPair<int,int>(nb,i)))
                    baseCellPtr = tempStckDnsrIdentifierMap.value(QPair<int,int>(nb,i));
                else
                    baseCellPtr = denserLin_1.at(i);
                generateStackedDensersFromCell(boneOrderedList.at(nb), baseCellPtr, tempGrowthPoly, rollAngleList);

                topMostDenser = boneOrderedList.at(nb)->allStackedDenserLineages.last();
                tempStckDnsrIdentifierMap.insert(QPair<int,int>(nb,i), boneOrderedList.at(nb)->allStackedDenserLineages.last());

                if (i == (indexOfPivotDenser+numDensOnEitherSidesOfPivDenser)-1
                        ||
                    i == denserLin_1.length()-1)
                {
                    bpPtr->boundaryDnsrs.append(denserLin_1.at(i));
                }

                boneOrderedList.at(nb)->allStackedDenserLineages.removeLast();
            }
            tempCnt++;

//            if (d % 3 == 0) // TODO(ismail): variable qualifiable for protrusion DNA
                numDensOnEitherSidesOfPivDenser -= 1; // TODO(ismail): variable qualifiable for protrusion DNA
        }
        // Now do the bones on the lower side of the pivot bone
    }

//    // Now generate the spikes
//    for (int spike=0; spike<10; spike++)
//    {
//        QPolygonF tempGrowthPoly; tempGrowthPoly.clear();
//        tempGrowthPoly << QPointF(1, Box::BoxFace::BOTTOM)
//                       << QPointF(-7, 0)     // (pitch angle, yaw angle)
//                       << QPointF(0, 0)     // (roll angle, should it mirror)
//                       << QPointF(0, 0)     // (%age at which the angle will change, angle change amount)
//                       << QPointF(1, 0.9)     // (x scaling value, y scaling value)
//                       << QPointF(0.85, -1);   // (z scaling value, -1)

//        QVector<float> rollAngleList = QVector<float>(tempGrowthPoly[0].x(), tempGrowthPoly[2].x());
//        generateStackedDensersFromCell(boneOrderedList.at(indexOfPivotBone), topMostDenser, tempGrowthPoly, rollAngleList);
//        topMostDenser = boneOrderedList.at(indexOfPivotBone)->allStackedDenserLineages.last();
//        boneOrderedList.at(indexOfPivotBone)->allStackedDenserLineages.removeLast();
//    }

}

void AlienModeller::createCratersInDensers(QVector<Bone *> &boneOrderedList, QPolygonF craterSpecs)
{
    // Find a bone at random
    int indexOfPivotBone = static_cast<int>((craterSpecs[0].x() / 100.f)
                                            * boneOrderedList.length());
//    qDebug() << indexOfPivotBone;

    int numOfBnsOnUpperSideOfPivBone = static_cast<int>((craterSpecs[1].x() / 100.f)
                                            * boneOrderedList.length());
    qDebug() << numOfBnsOnUpperSideOfPivBone;
    int numOfBnsOnLowerSideOfPivBone = static_cast<int>((craterSpecs[1].y() / 100.f)
                                            * boneOrderedList.length());

    int numOfDensersToStackOnTop = GeometryAlgorithms::getRandomInteger(craterSpecs[3].x(), craterSpecs[3].y());

    // Go through the bones on the upper side of the pivot bone
    int numDensOnEitherSidesOfPivDenser = 5;
    int indexOfPivotDenser = 0;
    QVector<Cell*> denserLin_1; denserLin_1.clear();
    QVector<QVector<Cell*>> stckDenserLin2DList; stckDenserLin2DList.clear();
    QMap<QPair<int,int>, Cell*> tempStckDnsrIdentifierMap; tempStckDnsrIdentifierMap.clear();
    Cell *baseCellPtr = nullptr;

    for (int d=0; d<=numOfDensersToStackOnTop; d++)
    {
        for (int nb=indexOfPivotBone; nb<indexOfPivotBone+numOfBnsOnUpperSideOfPivBone && nb<boneOrderedList.length(); nb++)
        {
            // Now find the pivot cell/denser for the pivot bone based on the dentSpecs[1].x()
                denserLin_1 = QVector<Cell*>(boneOrderedList.at(nb)->allStackedDenserLineages);
        //        denserLin_1 = boneOrderedList.at(nb)->allStackedDenserLineages.at(0);
    //            qDebug() << denserLin_1.length();
                indexOfPivotDenser = static_cast<int>((craterSpecs[2].x() / 100.f)
                                                        * denserLin_1.length());
            if (nb == indexOfPivotBone)
            {
                int tempRemainingDensers = denserLin_1.length() - indexOfPivotDenser;
                numDensOnEitherSidesOfPivDenser = static_cast<int>((craterSpecs[2].y() / 100.f)
                                                        * tempRemainingDensers);
//                if (d % 2 == 0)
                    numDensOnEitherSidesOfPivDenser -= d;
            }

            // First work on the densers on the right of pivot denser
            // We need to create a cloned denser on top of this denser, i.e. just stack it.
            for (int i=indexOfPivotDenser; i<indexOfPivotDenser+numDensOnEitherSidesOfPivDenser && i<denserLin_1.length(); i++)
            {
                QPolygonF tempGrowthPoly; tempGrowthPoly.clear();
                tempGrowthPoly << QPointF(1, Box::BoxFace::TOP)
                               << QPointF(0, 0)     // (pitch angle, yaw angle)
                               << QPointF(0, 0)     // (roll angle, should it mirror)
                               << QPointF(0, 0)     // (%age at which the angle will change, angle change amount)
                               << QPointF(1, 1.1)     // (x scaling value, y scaling value)
                               << QPointF(1, -1);   // (z scaling value, -1)

                QVector<float> rollAngleList = QVector<float>(tempGrowthPoly[0].x(), tempGrowthPoly[2].x());
                if (tempStckDnsrIdentifierMap.contains(QPair<int,int>(nb,i)))
                    baseCellPtr = tempStckDnsrIdentifierMap.value(QPair<int,int>(nb,i));
                else
                    baseCellPtr = denserLin_1.at(i);
                generateStackedDensersFromCell(boneOrderedList.at(nb), baseCellPtr, tempGrowthPoly, rollAngleList);

                tempStckDnsrIdentifierMap.insert(QPair<int,int>(nb,i), boneOrderedList.at(nb)->allStackedDenserLineages.last());

                // We need to store parent child densers as a lineage in the 2D list i.e. stckDenserLin2DList
                int j=0;
                for (j=0; j<stckDenserLin2DList.length(); j++)
                {
                    if (stckDenserLin2DList[j].last() == baseCellPtr)
                    {
                        stckDenserLin2DList[j].append(boneOrderedList.at(nb)->allStackedDenserLineages.last());
                        break;
                    }
                }
                if (j == stckDenserLin2DList.length())
                {
                    QVector<Cell*> tempDnsrList; tempDnsrList.clear();
                    tempDnsrList.append(baseCellPtr);
                    tempDnsrList.append(boneOrderedList.at(nb)->allStackedDenserLineages.last());
                    stckDenserLin2DList.append(tempDnsrList);
                }

                boneOrderedList.at(nb)->allStackedDenserLineages.removeLast();
            }
            // Now work on the densers on the left of pivot denser
            // We need to create a cloned denser on top of this denser, i.e. just stack it.
            for (int i=indexOfPivotDenser-1; i>indexOfPivotDenser-numDensOnEitherSidesOfPivDenser && i>0; i--)
            {
                QPolygonF tempGrowthPoly; tempGrowthPoly.clear();
                tempGrowthPoly << QPointF(1, Box::BoxFace::TOP)
                               << QPointF(0, 0)     // (pitch angle, yaw angle)
                               << QPointF(0, 0)     // (roll angle, should it mirror)
                               << QPointF(0, 0)     // (%age at which the angle will change, angle change amount)
                               << QPointF(1, 1.1)     // (x scaling value, y scaling value)
                               << QPointF(1, -1);   // (z scaling value, -1)

                QVector<float> rollAngleList = QVector<float>(tempGrowthPoly[0].x(), tempGrowthPoly[2].x());
                if (tempStckDnsrIdentifierMap.contains(QPair<int,int>(nb,i)))
                    baseCellPtr = tempStckDnsrIdentifierMap.value(QPair<int,int>(nb,i));
                else
                    baseCellPtr = denserLin_1.at(i);
                generateStackedDensersFromCell(boneOrderedList.at(nb), baseCellPtr, tempGrowthPoly, rollAngleList);

                tempStckDnsrIdentifierMap.insert(QPair<int,int>(nb,i), boneOrderedList.at(nb)->allStackedDenserLineages.last());

                // We need to store parent child densers as a lineage in the 2D list i.e. stckDenserLin2DList
                int j=0;
                for (j=0; j<stckDenserLin2DList.length(); j++)
                {
                    if (stckDenserLin2DList[j].last() == baseCellPtr)
                    {
                        stckDenserLin2DList[j].append(boneOrderedList.at(nb)->allStackedDenserLineages.last());
                        break;
                    }
                }
                if (j == stckDenserLin2DList.length())
                {
                    QVector<Cell*> tempDnsrList; tempDnsrList.clear();
                    tempDnsrList.append(baseCellPtr);
                    tempDnsrList.append(boneOrderedList.at(nb)->allStackedDenserLineages.last());
                    stckDenserLin2DList.append(tempDnsrList);
                }

                boneOrderedList.at(nb)->allStackedDenserLineages.removeLast();
            }
            numDensOnEitherSidesOfPivDenser--;
        }
    }

    // Now translate the densers in the stckDenserLin2DList so that they give the illusion of a crater/depression
    float scalVal = 100.f;
    for (int i=0; i<stckDenserLin2DList.length(); i++)
    {
        scalVal = 100;
        QVector3D pnt1 = stckDenserLin2DList[i].last()->getMidPointOfFace(Box::BoxFace::TOP);
        QVector3D pnt2 = stckDenserLin2DList[i].first()->getMidPointOfFace(Box::BoxFace::BOTTOM);
        QVector3D tempVec = pnt1 - pnt2;
        for (int j=stckDenserLin2DList[i].length()-1; j>=0; j--)
        {
            scalVal /= 1.001f;
            QVector3D tempPnt = pnt2 + ((scalVal/100.f)*tempVec);
            QVector3D tempVec2 = tempPnt - stckDenserLin2DList[i][j]->getMidPointOfFace(Box::BoxFace::TOP);
//            QVector3D tempVec2 = tempPnt - stckDenserLin2DList[i][j]->getCellCenterLocation();
            GeometryAlgorithms::translateCell(stckDenserLin2DList[i][j], tempVec2);
        }
    }
}

/*
ridgeSpecs << QPointF(75,2)  // (%age of boneList length to select the pivot bone, make a curvy formation or not i.e. 0=do not make curve formation; 1=make a curve formation)
          << QPointF(60,4)  // (%age of denser lineage length to select the pivot cell, 0 to 3 ==> code for selecting the denser. i.e. 0 = up, 1 = down, 2 = left, 3 = right)
          << QPointF(2,5)   // (minimum num of densers stacked on top of the pivot denser, max num of densers stacked on top of the pivot denser)
          << QPointF(3,10)    // (min and max roll angle of stacked densers)
          << QPointF(2, 1.0); // (Y scaling up/down trend i.e. 0 or 1, Y scaling value)
          << QPointF(2, 0.9); // (X scaling up/down trend i.e. 0 or 1, X scaling value)
                              // Here scaling trend is: 0=descending, 1=ascending
*/
void AlienModeller::createCurvyRidgeOnDensers(BodyPart *bpPtr, QVector<Bone *> &boneOrderedList, QPolygonF ridgeSpecs)
{
    int indexOfPivotBone = static_cast<int>((ridgeSpecs[0].x() / 100.f)
                                            * boneOrderedList.length());
//    qDebug() << indexOfPivotBone;

    int numOfIter = GeometryAlgorithms::getRandomInteger(4, ridgeSpecs[0].y());
    float rollAng = GeometryAlgorithms::getRandomFloat(ridgeSpecs[3].x(), ridgeSpecs[3].y());

    // Go through the bones on the upper side of the pivot bone
    int indexOfPivotDenser = 0;
    QVector<Cell*> denserLin_1; denserLin_1.clear();
    QVector<Cell*> usedDensersList; usedDensersList.clear();
    Cell *baseCellPtr = nullptr;
    QVector<float> rollAngleList; rollAngleList.clear();
    for (int i=0; i<numOfIter; i++)
    {
        int numOfStckdDnsrs = GeometryAlgorithms::getRandomInteger(ridgeSpecs[2].x(), ridgeSpecs[2].y());
        // Now find the pivot cell/denser for the pivot bone based on the dentSpecs[1].x()
        if (i==0)
        {
            denserLin_1 = QVector<Cell*>(boneOrderedList.at(indexOfPivotBone)->allStackedDenserLineages);
            indexOfPivotDenser = static_cast<int>((ridgeSpecs[1].x() / 100.f)
                                                * denserLin_1.length());
        }

        // generate the stacked densers according to ridgeSpecs
//        ridgeSpecs[5].setY(GeometryAlgorithms::getRandomFloat(0.8, 1.2));
        QPolygonF tempGrowthPoly; tempGrowthPoly.clear();
        tempGrowthPoly << QPointF(numOfStckdDnsrs, Box::BoxFace::BOTTOM) // (num of stacked densers, face of base denser on which the stacked denser will be generated)
                       << QPointF(0, 0)     // (pitch angle, yaw angle)
                       << QPointF(rollAng, 0)    // (roll angle, should it mirror)
                       << QPointF(0, 0)     // (%age at which the angle will change, angle change amount)
                       << QPointF(ridgeSpecs[5].y(), ridgeSpecs[4].y())    // (x scaling value, y scaling value)
                       << QPointF(1, -1); // (z scaling value, -1)

        if (i==0)
        {
//            rollAngleList = QVector<float>(tempGrowthPoly[0].x(), tempGrowthPoly[2].x());
            for (int r=0; r<tempGrowthPoly[0].x(); r++)
            {
                int rollAngFlag = GeometryAlgorithms::getRandomInteger(-10, 10);
                if (rollAngFlag < 0)
                    rollAngleList.append(tempGrowthPoly[2].x() * -1.f);
                else
                    rollAngleList.append(tempGrowthPoly[2].x());
            }
        }
        else
        {
            int rem = tempGrowthPoly[0].x() - rollAngleList.length();
            int toDel=0, toAdd=0;
            if (rem < 0)
            {
                toDel = abs(rem);
                for (int r=0; r<toDel; r++)
                {
                    rollAngleList.removeLast();
                }
            }
            else
            {
                toAdd = rem;
                for (int r=0; r<toAdd; r++)
                {
                    int rollAngFlag = GeometryAlgorithms::getRandomInteger(0, 2);
                    if (rollAngFlag == 1)
                        rollAngleList.append(rollAngleList.last() * -1.f);
                    else
                        rollAngleList.append(rollAngleList.last());
                }
            }

        }
        qDebug() << rollAngleList;
        baseCellPtr = denserLin_1[indexOfPivotDenser];
        generateStackedDensersFromCell(boneOrderedList.at(indexOfPivotBone), baseCellPtr, tempGrowthPoly, rollAngleList);
        usedDensersList.append(baseCellPtr);

        // We need to store these densers
        while (numOfStckdDnsrs > 0  &&  !boneOrderedList.at(indexOfPivotBone)->allStackedDenserLineages.isEmpty())
        {
//            usedDensersList.append(boneOrderedList.at(indexOfPivotBone)->allStackedDenserLineages.last());
            boneOrderedList.at(indexOfPivotBone)->allStackedDenserLineages.removeLast();
            numOfStckdDnsrs--;
        }

        do
        {
            int codeForGen = GeometryAlgorithms::getRandomInteger(0, ridgeSpecs[1].y());
//            qDebug() << codeForGen;
            if (codeForGen == 0)
            {
                indexOfPivotBone--;
                indexOfPivotDenser++;
            }
            else if (codeForGen == 1)
            {
                indexOfPivotBone++;
                indexOfPivotDenser--;
            }
            else if (codeForGen == 2)
            {
                indexOfPivotBone--;
                indexOfPivotDenser--;
            }
            else if (codeForGen == 3)
            {
                indexOfPivotBone++;
                indexOfPivotDenser++;
            }

            // TODO(Ismail): ensure index out of range bug doesn't occur
            denserLin_1 = QVector<Cell*>(boneOrderedList.at(indexOfPivotBone)->allStackedDenserLineages);
        }   // TODO(Ismail): ensure index out of range bug doesn't occur
        while (usedDensersList.contains(denserLin_1[indexOfPivotDenser]));


    }

}

void AlienModeller::generateStackedDensersFromCell(Bone *bonePtr, Cell *cellPtr, QPolygonF growthPoly, QVector<float> rollAngleList)
{
    QVector<Cell*> tempStkDnsrsLineage; tempStkDnsrsLineage.clear();
    // The first cell is always the base cell
    tempStkDnsrsLineage.append(cellPtr);

    for (int j=0; j<growthPoly[0].x(); j++)
    {
        // *** ALGO ***
        // Find/Compute the attachment point for this denser using the DNA specs
        // rotate the cell according to the roll angle in the DNA specs
        // mirror it according to the DNA specs
        // Assign this denser cell to the BP celllist
        // ************

        // Generate a clone of the cell where this denser will be created, and make it rotate, scale and translate
        // ... exactly like the source

        Cell *newClonedCell = new Cell();
        newClonedCell->isDenser = true;
        cellPtr->cloneCell(newClonedCell);
        int pivotFace=-1;
        // Get the opposite face.
        // TODO(ismail): Implement an algorithm that can find the opposite face via line-plane intersection algorithm
        // OR find the face with the opposite NORMAL vector
        if (growthPoly[0].y() == Box::BoxFace::FRONT)
            pivotFace = Box::BoxFace::BACK;
        else if (growthPoly[0].y() == Box::BoxFace::BACK)
            pivotFace = Box::BoxFace::FRONT;
        else if (growthPoly[0].y() == Box::BoxFace::LEFT)
            pivotFace = Box::BoxFace::RIGHT;
        else if (growthPoly[0].y() == Box::BoxFace::RIGHT)
            pivotFace = Box::BoxFace::LEFT;
        else if (growthPoly[0].y() == Box::BoxFace::TOP)
            pivotFace = Box::BoxFace::BOTTOM;
        else if (growthPoly[0].y() == Box::BoxFace::BOTTOM)
            pivotFace = Box::BoxFace::TOP;

        newClonedCell->setPivotFace(pivotFace);


        QVector3D attachPoint = cellPtr->getMidPointOfFace(growthPoly[0].y());
//        QVector3D attachPoint = cellPtr->cellBox->vertices[cellPtr->cellBox->faces[growthPoly[0].y()]->vertNorIndices[1]]
//                   +
//                   (0.5 * (cellPtr->cellBox->vertices[cellPtr->cellBox->faces[growthPoly[0].y()]->vertNorIndices[3]]
//                   - cellPtr->cellBox->vertices[cellPtr->cellBox->faces[growthPoly[0].y()]->vertNorIndices[1]]));

        //                     GeometryAlgorithms::rotateCellTowardsVecDir(newClonedCell, baseCell->getPivotVector());
        GeometryAlgorithms::rotateCellUsingEulerAngles(newClonedCell, growthPoly[1].x(), growthPoly[1].y(), rollAngleList[j]);
        GeometryAlgorithms::scaleCell(newClonedCell, growthPoly[4].x(), growthPoly[4].y(), growthPoly[5].x());
        GeometryAlgorithms::translateCell(newClonedCell, attachPoint - newClonedCell->getPivotPoint());

        bonePtr->cellList.append(newClonedCell);
        tempStkDnsrsLineage.append(newClonedCell);

        cellPtr = newClonedCell;
    }

    tempStkDnsrsLineage.removeFirst();
    bonePtr->allStackedDenserLineages.append(tempStkDnsrsLineage);
    qDebug() << bonePtr->allStackedDenserLineages.length();

//    bool lineageContinued=false;
//    for (int i=0; i<bonePtr->allStackedDenserLineages.length(); i++)
//    {
//        if (bonePtr->allStackedDenserLineages[i].last() == cellPtr)
//        {
//            tempStkDnsrsLineage.removeFirst();
//            bonePtr->allStackedDenserLineages[i].append(tempStkDnsrsLineage);
//            lineageContinued = true;
//            break;
//        }
//    }

//    if (lineageContinued == false)  // Lineage is not continued. A new lineage will be added as a new list
//                                    // ... to the allStackedDenserLineages
//        bonePtr->allStackedDenserLineages.append(tempStkDnsrsLineage);
}

void AlienModeller::squeezeStackedDensers(QPair<Bone*,Bone*> adjBones, QVector<Cell *> listOfFirstCellOfStkDnsr, float sqzAmnt)
{
    /*  *** ALGORITHM 1 ***
     *
     *  Go through the main lineage (i.e. first lineage) of this bone's stacked densers
     *
     *  If the listOfFirstCellOfStkDnsr contains any entries, then go through that lineage for which the first cell is
     *  ... present in the listOfFirstCellOfStkDnsr
     *
     *  Select a cell at random and apply scaling to its z scaling
     *
     *  Scale the the neighbouring cells in a decreasing manner, so it looks like a smooth squeezing effect
     *
     *  Translate the squeezed cells away from the 2nd lineage (i.e) main lineage of the adjacent bone in the list adjBoneIDs
     *
     *  Depending on the requirement, bring the squeezed densers closer to the adjacent bone's lineage
     *
    */
    QVector<Cell*> tempStackDensers1; tempStackDensers1.clear();
    QVector<Cell*> tempStackDensers2; tempStackDensers2.clear();
    // Get the first stacked densers lineage of both adjacent bones
    for (Cell *cellPtr : adjBones.first->allStackedDenserLineages)
    {
        tempStackDensers1.append(cellPtr);
    }
    for (Cell *cellPtr : adjBones.second->allStackedDenserLineages)
    {
        tempStackDensers2.append(cellPtr);
    }
    qDebug() << tempStackDensers1.length();
    qDebug() << tempStackDensers2.length();

    // Now squeeze the densers (cells)
    int midInd = tempStackDensers1.length()/2;
    bool kIsDecreasing = false;
    float tempSqzAmnt = sqzAmnt;
    for (int k=midInd; k<tempStackDensers1.length(); k++)
    {
        // squeeze
        GeometryAlgorithms::scaleCell(tempStackDensers1[k], 1, 1, tempSqzAmnt);
        tempSqzAmnt = tempSqzAmnt/1.2;
    }

//    for (int k=midInd; k<tempStackDensers1.length() && k>=0; )
//    {
//        // squeeze
//        GeometryAlgorithms::scaleCell(tempStackDensers1[k], 1, 1, tempSqzAmnt);
//        tempSqzAmnt = 1.0 - tempSqzAmnt/1.5;
//        if (k == tempStackDensers1.length()-1)
//        {
//            k = midInd;
//            kIsDecreasing = true;
//            tempSqzAmnt = sqzAmnt;
//        }
//        if (kIsDecreasing)
//            k--;
//        else
//            k++;
//    }

    // Now squeeze the densers (cells) on the adjacent bone
    midInd = tempStackDensers2.length()/2;
    kIsDecreasing = false;
    tempSqzAmnt = sqzAmnt;
    for (int k=midInd; k<tempStackDensers2.length() && k>=0; )
    {
        // squeeze
        GeometryAlgorithms::scaleCell(tempStackDensers2[k], 1, 1, tempSqzAmnt);
        tempSqzAmnt = tempSqzAmnt + 0.1;
        if (k == tempStackDensers2.length()-1)
        {
            k = midInd;
            kIsDecreasing = true;
            tempSqzAmnt = sqzAmnt;
        }
        if (kIsDecreasing)
            k--;
        else
            k++;
    }

}

void AlienModeller::getAllSubBPsOfGivenType(Alien *alienPtr, BodyPart *parentBP_Ptr, int subBP_TypeID, QVector<BodyPart *> &subBPsList)
{
    for (BodyPart *bpPtr : parentBP_Ptr->subBodyParts)
    {
        if (bpPtr->BP_Type == subBP_TypeID)
            subBPsList.append(bpPtr);
    }
}

// Clone the alien via DNA
// NOT BEING USED, SO PERHAPS NEED TO REMOVE IT AND ANY METHOD called by only this method
Alien *AlienModeller::cloneAlien(Alien *sourceAlien)
{
    // TODO(Ismail): BUG: This method might fail when the source Alien's BP IDs are not in a sequence starting from 0
    // ... This needs to be resolved so that whenever we wish to clone the ALien (via DNA), the clone is created perfectly
    // ... regardless of the BP IDs
    Alien *newAlien = new Alien();
    alienList.append(newAlien);
    newAlien->alienID = ++AlienID_Counter;
    newAlien->clonedFrom = sourceAlien->alienID;
//    BP_ID_Counter_ForAllAliens.append(0);

    cloneAlienDNA(sourceAlien, newAlien);

    // make the BP_ToType_Map list
    BP_ToType_Map.clear();
    for (int i=0; i<sourceAlien->bodyParts.length(); i++)
    {
        if (sourceAlien->bodyParts[i]->BP_Type != AlienModeller::BodyPartsEnum::JOINT  &&
                sourceAlien->bodyParts[i]->BP_Type != AlienModeller::BodyPartsEnum::BONE)
        {
            BP_ToType_Map.insert(sourceAlien->bodyParts[i]->BP_ID, sourceAlien->bodyParts[i]->BP_Type);
        }
    }

    buildAlienSkeleton_TypeGuided(newAlien);
    attachAllSkeletalBodyPartsToBackBone(newAlien);

    return newAlien;
}

Alien *AlienModeller::cloneAlien_Algo2(Alien *sourceAlien)
{
    /*
     * Algorithm: A more robust/reliable algorithm than cloneAlien()
     * This method will clone each body part of the source alien and fill up the cloned Alien with these BPs
     * In other words it will generate the clones BPs for the new alien based on the source Alien.
    */

    // // First remove and delete all the extremities.
    // // TODO: we need to clone these extremities and attach these to the corresponding cloned BP in the cloned Alien
    // for (ExtremityBP *extPtr : sourceAlien->alienExtremities)
    // {
    //     if (detachExtremBP_FromParentBP(sourceAlien, extPtr->BP_ID))
    //     {
    //         removeAndDeleteExtremityBP_FromAlien(sourceAlien, extPtr);
    //     }
    // }

    Alien *newClonedAlien = new Alien(); // The Alien Skeleton is also created inside the Alien
    alienList.append(newClonedAlien);
    newClonedAlien->alienID = ++AlienID_Counter;
    newClonedAlien->clonedFrom = sourceAlien->alienID;

    newClonedAlien->Alien_Type_ID = sourceAlien->Alien_Type_ID;

    newClonedAlien->alienDNA = new Alien_DNA();
    newClonedAlien->alienDNA->skeletonDNA = new Skeleton_DNA();

    // newClonedAlien->alienDNA->skeletonDNA->numOfSkeletalBodyParts_DNA = QMap<int,int>(sourceAlien->alienDNA->skeletonDNA->numOfSkeletalBodyParts_DNA);
    //++
    newClonedAlien->alienDNA->skeletonDNA->BP_SizeComparedToBackBone_DNA = QMap<int, QPolygon>(sourceAlien->alienDNA->skeletonDNA->BP_SizeComparedToBackBone_DNA);

    // clone the Attachment DNAs from source to new alien
    //++
    newClonedAlien->alienDNA->skeletonDNA->BP_AttachmentToBackBone_ZoneAndScattering_DNA =
                                        QMap<int, QPair<float, float>>(sourceAlien->alienDNA->skeletonDNA->BP_AttachmentToBackBone_ZoneAndScattering_DNA);
    newClonedAlien->alienDNA->skeletonDNA->BP_AngleArrangementsFromBackBone_DNA =
            QMap<int, QPolygon>(sourceAlien->alienDNA->skeletonDNA->BP_AngleArrangementsFromBackBone_DNA);

    // QList<int> tempKeys = newClonedAlien->alienDNA->skeletonDNA->BP_AngleArrangementsFromBackBone_DNA.keys();
    // for (int key : tempKeys)
    //     newClonedAlien->alienDNA->skeletonDNA->BP_AngleArrangementsFromBackBone_DNA[key].clear();
    // qDebug() << sourceAlien->alienDNA->skeletonDNA->BP_AngleArrangementsFromBackBone_DNA;
    // exit(0);


    // First construct a list of BPs that should include all the ones which were smoothed
    QVector<BodyPart*> toBeSmoothBPsSourceAlien; toBeSmoothBPsSourceAlien.clear();
    for (int i=0; i<sourceAlien->bodyParts.length(); i++)
    {
        int tempBP_ID = sourceAlien->bodyParts[i]->BP_ID;
        if (sourceAlien->smoothingBackupCloneBPs.contains(tempBP_ID))
        {
            qDebug() << "reached 1";
            int tempBP_ID_2 = sourceAlien->smoothingBackupCloneBPs.value(tempBP_ID)->BP_ID;
            // Basically here we're replacing the BP with its unsmoothed clone

            // append it to the bodyparts main list
            // remove it's clone from the smoothingBackupCloneBPs
            // remove the inBP_Ptr from the bodyparts main list
            sourceAlien->bodyParts.append(sourceAlien->smoothingBackupCloneBPs.value(tempBP_ID));
            sourceAlien->smoothingBackupCloneBPs.remove(tempBP_ID);

            // Remove + delete the BP
            QVector<int> tempBP_ID_List_1; tempBP_ID_List_1.clear();
            tempBP_ID_List_1 << tempBP_ID;
            removeAlienBodyParts(sourceAlien, tempBP_ID_List_1);

    //            alien->alienSkeleton.Jnt2JntRelations_TypeGuided.remove(inBP_Ptr->BP_ID);
            sourceAlien->alienSkeleton.Jnt2JntRelations_TypeGuided.insert(tempBP_ID,
                                    sourceAlien->alienSkeleton.Jnt2JntRelations_TypeGuided.value(tempBP_ID_2));
            sourceAlien->alienSkeleton.Jnt2JntRelations_TypeGuided.remove(tempBP_ID_2);

            // assign the inBP_Ptr ID to the newly appended clone
            sourceAlien->bodyParts.last()->BP_ID = tempBP_ID;
            toBeSmoothBPsSourceAlien.append(sourceAlien->bodyParts.last());
        }
        qDebug() << "reached 2";
    }

    // Now start cloning all the BPs of the source Alien
    // NOTE: We dont build the skeleton as we do for a new alien. Here we clone every BP and attach it to the BackBone
    // QVector<int> toBeDensifiedBPsNewAlien; toBeDensifiedBPsNewAlien.clear();
    QVector<BodyPart *> allClonedBPsPtrs; allClonedBPsPtrs.clear();
    QVector<int> BP_TypesToIgnore; BP_TypesToIgnore.clear();
    BP_TypesToIgnore    << AlienModeller::BodyPartsEnum::JOINT
                        << AlienModeller::BodyPartsEnum::BONE
                        << AlienModeller::BodyPartsEnum::ExtremBP_Carpal
                        << AlienModeller::BodyPartsEnum::ExtremBP_Digit;
    for (int i=0; i<sourceAlien->bodyParts.length(); i++)
    {
        // TODO(ismail):
        //              If this BP is one of the keys in the smoothingBackupCloneBPs DS in Alien,
        //              then replace it with the one in smoothingBackupCloneBPs whose key is this BP_ID.
        //              Replacing will delete this BP, move the cloned BP from the smoothingBackupCloneBPs
        //              and add it to the alien's BP_List. Also maintain a list of BPs, which will be smoothed later
        //              on, i.e. add this BP to this list
        if (!BP_TypesToIgnore.contains(sourceAlien->bodyParts[i]->BP_Type))
        {
            if (!sourceAlien->bodyParts[i]->isSymmetric) // only clone it if its not a mirror. We'll symmetrify it later
            {
                allClonedBPsPtrs.append(cloneSkeletalBP_FromAnotherAlien_viaDNA(newClonedAlien, sourceAlien, sourceAlien->bodyParts[i]));
                // if (toBeSmoothBPsSourceAlien.contains(sourceAlien->bodyParts[i]))
                // {
                //     toBeSmoothBPsSourceAlien.append(cloneBpPtr);
                //     toBeSmoothBPsSourceAlien.removeOne(sourceAlien->bodyParts[i]);
                // }
                // if (cloneDensificationDNA(sourceAlien, newAlien, sourceAlien->bodyParts[i], cloneBpPtr))
                //     toBeDensifiedBPsNewAlien << cloneBpPtr->BP_ID;
            }
        }
    }

    if (sourceAlien->alienJustLoadedFlag)
    {
        // attach all BPs to the Backbone of the newly cloned alien
        attachAllSkeletalBodyPartsToBackBone(newClonedAlien);
        symmetrifyAlienBodyParts(newClonedAlien);
        sourceAlien->alienJustLoadedFlag=false;
    }
    else
    {
        // Now attach the cloned BPs to their parents - mostly the BPs are attached to the BB
        // So we first rotate the cloned BB to the original BB
        newClonedAlien->rootBP = newClonedAlien->alienBackBones.last();
        GeometryAlgorithms::rotateBodyPartTowardsVecDir(*(newClonedAlien->rootBP), sourceAlien->rootBP->getPivotVector());
        // GeometryAlgorithms::rotateBP_AccordingToInputRotationMat(newClonedAlien->rootBP, sourceAlien->rootBP->BP_OrientationQuat);
        // update the cells box geometry
        GeometryAlgorithms::updateBP_CellPrimitiveGeometry(newClonedAlien->rootBP);


        attachAllClonedChildBPsToTheirClonedParentBPs_afterAlienIsCloned(newClonedAlien, sourceAlien, allClonedBPsPtrs, sourceAlien->rootBP);
        symmetrifyAlienBodyParts(newClonedAlien);
        // GeometryAlgorithms::rotateBodyPartTowardsVecDir(*(newClonedAlien->rootBP), QVector3D(0,0,1));

        // Unrotate the BP (using inverse quaternion)
        GeometryAlgorithms::rotateBP_AccordingToInputRotationMat(newClonedAlien->rootBP, newClonedAlien->rootBP->BP_OrientationQuat.inverted());
        // update the cells box geometry
        GeometryAlgorithms::updateBP_CellPrimitiveGeometry(newClonedAlien->rootBP);
    }

    // Now smooth the BPs that need to be smoothed
    for (BodyPart *bpPtr : toBeSmoothBPsSourceAlien)
    {
        // Do the smoothing
        smoothenSkeletalBP(newClonedAlien, bpPtr, 2);
    }

    // qDebug() <<  newClonedAlien->alienDNA->skeletonDNA->BP_AngleArrangementsFromBackBone_DNA;
    // exit(0);

    return newClonedAlien;
}

bool AlienModeller::saveAlienToFile(Alien *alienPtr)
{
    // First prepare all the data structures to be stored to file
    //
    // Store Alien_Type_ID
    // Store alienID
    // store BP_TypeColorMap_2
    // store extremToParentBP_Map
    // prepare and store parentBP to childBP map
    // prepare and store parentBP to ExtBP to Carpals and Digits map
    // store BP_SizeComparedToBackBone_DNA
    // store BP_AngleArrangementsFromBackBone_DNA
    // prepare and store numOfJointsPerBP_DNA
    // store BP_to_JAT_rootJoint_Map_DNA
    // prepare and store BP_JAT_rootJoint_DNA - perhaps not needed as it can be generated by BP_to_JAT_rootJoint_Map_DNA
    // store Jnt2JntRelations_TypeGuided
    // store randIntForBookKeeping_EnergyTrend
    // store randFloatForBookKeeping_FinalEnergy
    // store randIntForBookKeeping_AngleTrend
    // store randIntForBookKeeping_quaterEulerAngleSelectorForFirstChild
    // store randQVector3DForBookKeeping_quaterEulerAngleSelectorForNotFirstChild
    // store Skeletal_BP_Bulkiness_DNA_ForEachBP --> This does not include bone IDs
    // store BP_GenDensificationDNA
    // prepare and store a map for BP_ID > BP_OrientationQuat for all BPs
    // prepare and store a map for BP_ID > BP_TranslateMatrix for all BPs
    // prepare and store a map for BP_ID > BP_ScalingMatrix for all BPs
    // store the following for all BPs:
    // *  clonedFrom
    // *  cloneFromAlienAndBP_Pair
    // *  isSymmetric
    // *  mirrorOf
    // *  smoothnessLevel
    // store BP_BulgificationDNA --> This includes Bones IDs, so need to be cloned using a method, when loading the creature from file
    // BP_SpecificDensificationDNA --> This includes Bones IDs, so need to be cloned using a method, when loading the creature from file




    QString fileName = "/Users/ismailkazmi/Qt/Examples/Qt-6.6.1/opengl/idea1_QtProject/test_file.dat";

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly))
    {
        qDebug() << "Failed to open file for writing";
        return false;
    }
    QDataStream out(&file);

    // Store Alien_Type_ID and Alien ID
    out << alienPtr->Alien_Type_ID;
    qDebug() << alienPtr->Alien_Type_ID;
    // out << a->alienID; // Perhaps not store this as it will be assigned by the system when loading
    // store BP_TypeColorMap_2
    out << alienPtr->extremToParentBP_Map;
    qDebug() << alienPtr->extremToParentBP_Map;
    // prepare and store '(parentBP, ExtBP) to Carpals and Digits map'.
    QMap<QPair<int, int>, QList<int> > parentExtremMap = getExtremityAndParentToCarpalsAndDigitsMap(alienPtr);
    // if (!parentExtremMap.isEmpty())
    out << parentExtremMap;
    qDebug() << parentExtremMap;
    // prepare and store 'childBP to parentBP map'.
    // ... This map also allows us to know which BPs need to be generated/constructed/modelled
    QMap<int,int> parentChildMap = getChildParentRelationsInMap(alienPtr);
    // if (!parentChildMap.isEmpty())
    out << parentChildMap;
    qDebug() << parentChildMap;

    // store BP_SizeComparedToBackBone_DNA
    out << alienPtr->alienDNA->skeletonDNA->BP_SizeComparedToBackBone_DNA;
    qDebug() << alienPtr->alienDNA->skeletonDNA->BP_SizeComparedToBackBone_DNA;
    // store BP_AngleArrangementsFromBackBone_DNA
    out << alienPtr->alienDNA->skeletonDNA->BP_AngleArrangementsFromBackBone_DNA;
    qDebug() << alienPtr->alienDNA->skeletonDNA->BP_AngleArrangementsFromBackBone_DNA;
    // store BP_AttachmentToBackBone_ZoneAndScattering_DNA
    out << alienPtr->alienDNA->skeletonDNA->BP_AttachmentToBackBone_ZoneAndScattering_DNA;
    qDebug() << alienPtr->alienDNA->skeletonDNA->BP_AttachmentToBackBone_ZoneAndScattering_DNA;
    // store GroupBP_AttachmentToAnotherBP_ZoneScatteringTrend_DNA
    out << alienPtr->alienDNA->skeletonDNA->GroupBP_AttachmentToAnotherBP_ZoneScatteringTrend_DNA;
    qDebug() << alienPtr->alienDNA->skeletonDNA->GroupBP_AttachmentToAnotherBP_ZoneScatteringTrend_DNA;


    // prepare and store numOfJointsPerBP_DNA
    // prepare and store a map for BP_ID > BP_OrientationQuat for all BPs
    // prepare and store a map for BP_ID > BP_TranslateMatrix for all BPs
    // prepare and store a map for BP_ID > BP_ScalingMatrix for all BPs
    // store the following for all BPs:
    // *  clonedFrom
    // *  cloneFromAlienAndBP_Pair
    // *  isSymmetric
    // *  mirrorOf
    // *  smoothnessLevel
    QMap<int,int> BP_To_bpTypeMap; BP_To_bpTypeMap.clear();
    QMap<int, int> numOfJntsMap; numOfJntsMap.clear();
    QMap<int, QQuaternion> BP_OrientationQuatMap; BP_OrientationQuatMap.clear();
    QMap<int, QMatrix4x4> BP_TranslateMatrixMap; BP_TranslateMatrixMap.clear();
    QMap<int, QMatrix4x4> BP_ScaleMatrixMap; BP_ScaleMatrixMap.clear();
    // key(int) is BP_ID. QPOly is (cloneFromAlienAndBP_Pair) (cloneFrom, MirrorOF) (isSymmetric, smoothnessLevel)
    QMap<int,QPolygon> miscelleneousBP_Properties; miscelleneousBP_Properties.clear();
    QMap<int, QPolygon> Jnt2JntRelations_TypeGuided_QDataStreamFriendly; Jnt2JntRelations_TypeGuided_QDataStreamFriendly.clear();
    QMap<int, QVector<int>> bonesIDsInOrder_ForEachBP; bonesIDsInOrder_ForEachBP.clear();
    QVector<int> BP_TypesToIgnore; BP_TypesToIgnore.clear();
    BP_TypesToIgnore    << AlienModeller::BodyPartsEnum::JOINT
                        << AlienModeller::BodyPartsEnum::BONE;
    for (BodyPart *bp : alienPtr->bodyParts)
        if (!BP_TypesToIgnore.contains(bp->BP_Type))
            if (!bp->isSymmetric)
            {
                BP_To_bpTypeMap.insert(bp->BP_ID, bp->BP_Type);
                numOfJntsMap.insert(bp->BP_ID, getNumOfJntsForBP(bp));
                BP_OrientationQuatMap.insert(bp->BP_ID, bp->BP_OrientationQuat);
                BP_TranslateMatrixMap.insert(bp->BP_ID, bp->BP_TranslateMatrix);
                BP_ScaleMatrixMap.insert(bp->BP_ID, bp->BP_ScalingMatrix);

                QPolygon tempPoly; tempPoly.clear();
                tempPoly    << QPoint(bp->cloneFromAlienAndBP_Pair.first, bp->cloneFromAlienAndBP_Pair.second) // might not need this
                            << QPoint(bp->clonedFrom, bp->mirrorOf) // might not need this
                            << QPoint(bp->isSymmetric, bp->smoothnessLevel); // might not need bp->isSymmetric. May need smoothnessLevel
                miscelleneousBP_Properties.insert(bp->BP_ID, tempPoly);

                tempPoly.clear();
                tempPoly << *(alienPtr->alienSkeleton.Jnt2JntRelations_TypeGuided.value(bp->BP_ID));
                Jnt2JntRelations_TypeGuided_QDataStreamFriendly.insert(bp->BP_ID, tempPoly);
                QVector<Bone*> bonesInOrder; bonesInOrder.clear();
                QVector<int> bonesIDsInOrder; bonesIDsInOrder.clear();
                getAllBP_BonesInOrder(alienPtr, bp, bonesInOrder);
                for (Bone *bone : bonesInOrder)
                    bonesIDsInOrder << bone->BP_ID;
                bonesIDsInOrder_ForEachBP.insert(bp->BP_ID, bonesIDsInOrder);
            }
    out << BP_To_bpTypeMap;
    qDebug() << "BP_To_bpTypeMap specs : " << BP_To_bpTypeMap;
    out << numOfJntsMap;
    out << BP_OrientationQuatMap; out << BP_TranslateMatrixMap; out << BP_ScaleMatrixMap;
    out << miscelleneousBP_Properties;
    // store Jnt2JntRelations_TypeGuided
    out << Jnt2JntRelations_TypeGuided_QDataStreamFriendly;
    qDebug() << Jnt2JntRelations_TypeGuided_QDataStreamFriendly;
    out << bonesIDsInOrder_ForEachBP;

    // store BP_to_JAT_rootJoint_Map_DNA
    // if (!alienPtr->alienDNA->skeletonDNA->skeletal_JATs_DNA.BP_to_JAT_rootJoint_Map_DNA.isEmpty())
    out << alienPtr->alienDNA->skeletonDNA->skeletal_JATs_DNA.BP_to_JAT_rootJoint_Map_DNA;
    // store randIntForBookKeeping_EnergyTrend
    out << alienPtr->alienDNA->skeletonDNA->skeletal_JATs_DNA.randIntForBookKeeping_EnergyTrend;
    qDebug() << "randIntForBookKeeping_EnergyTrend : " << alienPtr->alienDNA->skeletonDNA->skeletal_JATs_DNA.randIntForBookKeeping_EnergyTrend;
    // store randFloatForBookKeeping_FinalEnergy
    out << alienPtr->alienDNA->skeletonDNA->skeletal_JATs_DNA.randFloatForBookKeeping_FinalEnergy;
    // store randIntForBookKeeping_AngleTrend
    out << alienPtr->alienDNA->skeletonDNA->skeletal_JATs_DNA.randIntForBookKeeping_AngleTrend;
    // store randIntForBookKeeping_quaterEulerAngleSelectorForFirstChild
    out << alienPtr->alienDNA->skeletonDNA->skeletal_JATs_DNA.randIntForBookKeeping_quaterEulerAngleSelectorForFirstChild;
    // store randQVector3DForBookKeeping_quaterEulerAngleSelectorForNotFirstChild
    out << alienPtr->alienDNA->skeletonDNA->skeletal_JATs_DNA.randQVector3DForBookKeeping_quaterEulerAngleSelectorForNotFirstChild;
    // store Skeletal_BP_Bulkiness_DNA_ForEachBP --> This does not include bone IDs
    out << alienPtr->alienDNA->skeletonDNA->Skeletal_BP_Bulkiness_DNA_ForEachBP;
    // store BP_GenDensificationDNA
    // if (!alienPtr->alienDNA->skeletonDNA->BP_GenDensificationDNA.isEmpty())
    out << alienPtr->alienDNA->skeletonDNA->BP_GenDensificationDNA;
    // BP_SpecificDensificationDNA --> This includes Bones IDs, so it needs to be cloned using a method, when loading the creature from file
    // if (!alienPtr->alienDNA->skeletonDNA->BP_SpecificDensificationDNA.isEmpty())
    out << alienPtr->alienDNA->skeletonDNA->BP_SpecificDensificationDNA;
    // store BP_BulgificationDNA --> This includes Bones IDs, so it needs to be cloned using a method, when loading the creature from file
    // if (!alienPtr->alienDNA->skeletonDNA->BP_BulgificationDNA.isEmpty())
    out << alienPtr->alienDNA->skeletonDNA->BP_BulgificationDNA;
    // store the extremity BP DNA
    out << alienPtr->alienDNA->skeletonDNA->extremityBP_DNA;

    file.close();

    return true;
}

Alien *AlienModeller::loadAlienFromFile(QString filename)
{
    QFile file(filename);

    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "Error opening file for reading:" << file.errorString();
        return nullptr;
    }

    QDataStream in(&file);

    // Starting to Deserialize data from file
    int alienTypeID;
    in >> alienTypeID;

    QMap<int,int> extremToParentBP_Map;
    in >> extremToParentBP_Map;
    QMap<QPair<int, int>, QList<int> > parentExtremMap;
    in >> parentExtremMap;
    QMap<int,int> parentChildMap;
    in >> parentChildMap;
    QMap<int, QPolygon> BP_SizeComparedToBackBone_DNA;
    in >> BP_SizeComparedToBackBone_DNA;
    QMap<int, QPolygon> BP_AngleArrangementsFromBackBone_DNA;
    in >> BP_AngleArrangementsFromBackBone_DNA;
    QMap<int, QPair<float, float>> BP_AttachmentToBackBone_ZoneAndScattering_DNA;
    in >> BP_AttachmentToBackBone_ZoneAndScattering_DNA;
    QMap<QPair<int,int>, QPolygon> GroupBP_AttachmentToAnotherBP_ZoneScatteringTrend_DNA;
    in >> GroupBP_AttachmentToAnotherBP_ZoneScatteringTrend_DNA;

    QMap<int, int> BP_To_bpTypeMap;
    in >> BP_To_bpTypeMap;
    QMap<int, int> numOfJntsMap;
    in >> numOfJntsMap;
    QMap<int, QQuaternion> BP_OrientationQuatMap;
    in >> BP_OrientationQuatMap;
    QMap<int, QMatrix4x4> BP_TranslateMatrixMap;
    in >> BP_TranslateMatrixMap;
    QMap<int, QMatrix4x4> BP_ScaleMatrixMap;
    in >> BP_ScaleMatrixMap;
    QMap<int,QPolygon> miscelleneousBP_Properties;
    in >> miscelleneousBP_Properties;
    QMap<int, QPolygon> Jnt2JntRelations_TypeGuided;
    in >> Jnt2JntRelations_TypeGuided;
    QMap<int, QVector<int>> bonesIDsInOrder_ForEachBP;
    in >> bonesIDsInOrder_ForEachBP;

    QMap<int, QPolygonF> BP_to_JAT_rootJoint_Map_DNA;
    in >> BP_to_JAT_rootJoint_Map_DNA;
    QMap<int, int> randIntForBookKeeping_EnergyTrend;
    in >> randIntForBookKeeping_EnergyTrend;
    QMap<int, float> randFloatForBookKeeping_FinalEnergy;
    in >> randFloatForBookKeeping_FinalEnergy;
    QMap<int, int> randIntForBookKeeping_AngleTrend;
    in >> randIntForBookKeeping_AngleTrend;
    QMap<int, int> randIntForBookKeeping_quaterEulerAngleSelectorForFirstChild;
    in >> randIntForBookKeeping_quaterEulerAngleSelectorForFirstChild;
    QMap<int, QVector3D> randQVector3DForBookKeeping_quaterEulerAngleSelectorForNotFirstChild;
    in >> randQVector3DForBookKeeping_quaterEulerAngleSelectorForNotFirstChild;

    QMap<int, QPolygonF> Skeletal_BP_Bulkiness_DNA_ForEachBP;
    in >> Skeletal_BP_Bulkiness_DNA_ForEachBP;
    QMap<int, QPolygonF> BP_GenDensificationDNA;
    in >> BP_GenDensificationDNA;
    QMap<int, QMap<int, QPolygonF>> BP_SpecificDensificationDNA;
    in >> BP_SpecificDensificationDNA;
    QMap<int,QMap<int,QPolygon>> BP_BulgificationDNA;
    in >> BP_BulgificationDNA;
    QMap<int, QPolygon> extremityBP_DNA;
    in >> extremityBP_DNA;
    // --- All data deserialized

    file.close();

    qDebug() << alienTypeID;
    qDebug() << extremToParentBP_Map;
    qDebug() << parentExtremMap;
    qDebug() << parentChildMap;
    qDebug() << BP_SizeComparedToBackBone_DNA;
    qDebug() << "Angle arrangements : " << BP_AngleArrangementsFromBackBone_DNA;
    qDebug() << "Attachments specs : " << BP_AttachmentToBackBone_ZoneAndScattering_DNA;
    qDebug() << "BP_To_bpTypeMap specs : " << BP_To_bpTypeMap;
    qDebug() << numOfJntsMap;
    // qDebug() << BP_OrientationQuatMap;
    // qDebug() << BP_TranslateMatrixMap;
    qDebug() << Jnt2JntRelations_TypeGuided;
    qDebug() << "randIntForBookKeeping_EnergyTrend : " << randIntForBookKeeping_EnergyTrend;


    //  --- Generate the Alien from data
    Alien *newAlienPtr = new Alien(); // The Alien Skeleton is also created inside the Alien
    alienList.append(newAlienPtr);
    newAlienPtr->alienID = ++AlienID_Counter;
    newAlienPtr->Alien_Type_ID = alienTypeID;
    newAlienPtr->alienDNA = new Alien_DNA();
    newAlienPtr->alienDNA->skeletonDNA = new Skeleton_DNA();

    newAlienPtr->alienDNA->skeletonDNA->BP_SizeComparedToBackBone_DNA = BP_SizeComparedToBackBone_DNA;
    newAlienPtr->alienDNA->skeletonDNA->BP_AngleArrangementsFromBackBone_DNA = BP_AngleArrangementsFromBackBone_DNA;
    newAlienPtr->alienDNA->skeletonDNA->BP_AttachmentToBackBone_ZoneAndScattering_DNA = BP_AttachmentToBackBone_ZoneAndScattering_DNA;
    newAlienPtr->alienDNA->skeletonDNA->GroupBP_AttachmentToAnotherBP_ZoneScatteringTrend_DNA = GroupBP_AttachmentToAnotherBP_ZoneScatteringTrend_DNA;


    QMap<int,int> origBP_ToGeneratedBP_Map; origBP_ToGeneratedBP_Map.clear();

    QList<int> bpsToGenerateFromFile = BP_To_bpTypeMap.keys();
    QList<BodyPart*> bpsGeneratedFromFile; bpsGeneratedFromFile.clear();
    for (int origID : bpsToGenerateFromFile)
    {
        BodyPart *generatedBP_Ptr = generateBP_FromFile(newAlienPtr, origID, miscelleneousBP_Properties.value(origID).at(1).x(),
                                                        QPair<int,int>(miscelleneousBP_Properties.value(origID).at(0).x(), miscelleneousBP_Properties.value(origID).at(0).y()),
                                                        miscelleneousBP_Properties.value(origID).at(2).x(), miscelleneousBP_Properties.value(origID).at(1).y(),
                                                        miscelleneousBP_Properties.value(origID).at(2).y(), BP_To_bpTypeMap.value(origID), numOfJntsMap.value(origID),
                                                        Jnt2JntRelations_TypeGuided.value(origID), randIntForBookKeeping_EnergyTrend, randFloatForBookKeeping_FinalEnergy,
                                                        randIntForBookKeeping_AngleTrend, randIntForBookKeeping_quaterEulerAngleSelectorForFirstChild,
                                                        randQVector3DForBookKeeping_quaterEulerAngleSelectorForNotFirstChild,
                                                        BP_to_JAT_rootJoint_Map_DNA.value(origID), Skeletal_BP_Bulkiness_DNA_ForEachBP.value(origID),
                                                        BP_GenDensificationDNA.value(origID), BP_SpecificDensificationDNA.value(origID),
                                                        BP_BulgificationDNA.value(origID), bonesIDsInOrder_ForEachBP.value(origID), origBP_ToGeneratedBP_Map);

        if (generatedBP_Ptr)
        {
            bpsGeneratedFromFile.append(generatedBP_Ptr);
            qDebug() << "reached -1";
        }
        else
        {
            qDebug() << "BP not generated from file - loadAlienFromFile method";
            exit(0);
        }
    }

    qDebug() << "all BPs generated from file";

    qDebug() << origBP_ToGeneratedBP_Map;
    qDebug() << newAlienPtr->alienDNA->skeletonDNA->BP_AngleArrangementsFromBackBone_DNA;
    qDebug() << newAlienPtr->alienDNA->skeletonDNA->GroupBP_AttachmentToAnotherBP_ZoneScatteringTrend_DNA;

    // +++ Now generate the Extremities. Their carpals and digits are already present in the generatedBP_Ptr
    // ... We just need to put them togehter and attach to their respect parent BPs
    qDebug() << parentExtremMap;
    QVector<QPair<int,int>> extremParentPairsList = parentExtremMap.keys();
    for (int i=0; i<extremParentPairsList.length(); i++)
    {
        qDebug() << "about to model Extremity BP";

        int loadedExtID = extremParentPairsList[i].first;
        int loadedParentID = extremParentPairsList[i].second;
        // Now get the carpals and digits that belong to this extremity from the parentExtremMap list
        QList<int> carDigIDsList = parentExtremMap.value(extremParentPairsList[i]);
        // change the original carpals and digits with the generated ones
        for (int cd=0; cd<carDigIDsList.length(); cd++)
        {
            if (origBP_ToGeneratedBP_Map.contains(carDigIDsList[cd]))
                carDigIDsList[cd] = origBP_ToGeneratedBP_Map.value(carDigIDsList[cd]);
            else
                exit(0);
        }

        // Instantiate a new extremity BP and assign an ID to it
        ExtremityBP *exBP_Ptr = instantiateNewExtremBP(newAlienPtr); // this method does not require DNA

        if (extremityBP_DNA.contains(loadedExtID))
            newAlienPtr->alienDNA->skeletonDNA->extremityBP_DNA.insert(exBP_Ptr->BP_ID, extremityBP_DNA.value(loadedExtID));
        else
        {
            qDebug() << "There is no entry for the loaded Extremity in the loaded extremity DNA";
            exit(1);
        }
        // Now instantiate the base and its carpals and digits
        ExtremityBP_Base *extBasePtr = instantiateNewExtemBase(newAlienPtr, exBP_Ptr); // this method does not require DNA

        // Now we need to add the carpals and digits to this extremity.
        // ... These should already been generated in the previous steps.
        for (int carOrDigID : carDigIDsList)
        {
            // It is already generated in the earlier steps, so we expect it to be inside the alien's BPs list, else there's a problem
            BodyPart *carOrDigPtr = getBodyPartFromID(newAlienPtr, carOrDigID);
            if (carOrDigPtr)
            {
                if (carOrDigPtr->BP_Type == AlienModeller::BodyPartsEnum::ExtremBP_Carpal)
                {
                    if (!extBasePtr->extremBP_BaseCarpals.contains(carOrDigPtr))
                        extBasePtr->extremBP_BaseCarpals.append(carOrDigPtr);
                }
                else if (carOrDigPtr->BP_Type == AlienModeller::BodyPartsEnum::ExtremBP_Digit)
                {
                    if (!exBP_Ptr->extremBP_Digits.contains(carOrDigPtr))
                        exBP_Ptr->extremBP_Digits.append(carOrDigPtr);
                }
            }
            else
            {
                qDebug() << "This carpal or digit was not generated and not present in the Aliens BP list";
                exit(2);
            }
        }
        qDebug() << "reached 1";

        // Now Align the Carpals
        QPoint alignSpec = QPoint(newAlienPtr->alienDNA->skeletonDNA->extremityBP_DNA.value(exBP_Ptr->BP_ID).at(1).y(), 0);
        alignBPsInVicinity(newAlienPtr, extBasePtr->extremBP_BaseCarpals, alignSpec); // Uses DNA

        // Attach the digits to the carpals
        attachDigitsToCarpalsForNewExtremBP(newAlienPtr, exBP_Ptr->extremBP_Digits, extBasePtr->extremBP_BaseCarpals); // Does not Use DNA

        // Now attach this Extremity to its parent BP
        if (exBP_Ptr->attachSubCarpalsToRootCarpal())
        {
        }
        BodyPart *parentBP_Ptr = getBodyPartFromID(newAlienPtr, origBP_ToGeneratedBP_Map.value(loadedParentID));
        if (parentBP_Ptr)
        {
            QVector<int> extremBP_IDs; extremBP_IDs.clear();
            extremBP_IDs << exBP_Ptr->BP_ID;
            if (attachExtremBP_ToParentBP(newAlienPtr, parentBP_Ptr, extremBP_IDs))
            {
            }
            else
            {
                qDebug() << "extremity BP couldnt be attached to parent BP";
                exit(3);
            }
        }
        else
        {
            qDebug() << "Generated Parent BP is not present in Alien - loadAlienFromFile method";
            exit(0);
        }
        qDebug() << "Extremity BP modelled successfully";
    }

    qDebug() << "reached 0";


    // Now attach the generated BPs to the Backbone and symmetrify them
    // newAlienPtr->rootBP = newAlienPtr->alienBackBones.last();
    attachAllSkeletalBodyPartsToBackBone(newAlienPtr);
    symmetrifyAlienBodyParts(newAlienPtr);


    // Now get the extremities sorted
    // QMapIterator<QPair<int, int>, QList<int>> iter2(parentExtremMap);
    // while (iter2.hasNext())
    // {
    //     QPair<int,int> key = iter2.key();
    //     int GenerBP_ID = origBP_ToGeneratedBP_Map.value(key.first);
    //     // Now this extremity and all its children carpals and digits will be attached to this generated BP
    //     // Create an extremity BP and assign the carpals and digists to it
    //     // ALso attach the digits to carpals and other carpals to root carpal
    //     ExtremityBP *exBP_Ptr = instantiateNewExtremBP(newAlienPtr); // this method does not require DNA

    //     // Now add a new entry to the extremity DNA
    //     // For cloning an ExtBP, simply copy the DNA
    //     // newAlienPtr->alienDNA->skeletonDNA->extremityBP_DNA.insert(exBP_Ptr->BP_ID, sourceAlien->alienDNA->skeletonDNA->extremityBP_DNA.value(srcExtBP_Ptr->BP_ID));


    // }


    return newAlienPtr;
}

BodyPart *AlienModeller::generateBP_FromFile(Alien *alienPtr, int origBP_ID, int clonedFrom, QPair<int, int> cloneFromAlienAndBP_Pair,
                                             bool isSymmetric, int mirrorOf, int smoothnessLevel, int BP_Type, int numOfJnts, QPolygon Jnt2JntRelations,
                                             QMap<int, int> randIntForBookKeeping_EnergyTrend, QMap<int, float> randFloatForBookKeeping_FinalEnergy,
                                             QMap<int, int> randIntForBookKeeping_AngleTrend, QMap<int, int> randIntForBookKeeping_quaterEulerAngleSelectorForFirstChild,
                                             QMap<int, QVector3D> randQVector3DForBookKeeping_quaterEulerAngleSelectorForNotFirstChild,
                                             QPolygonF BP_to_JAT_rootJoint, QPolygonF Skeletal_BP_Bulkiness_DNA, QPolygonF BP_GenDensificationDNA, QMap<int, QPolygonF> BP_SpecificDensificationDNA,
                                             QMap<int, QPolygon> BP_BulgificationDNA, QVector<int> bonesIDsInOrder, QMap<int,int> &origBP_ToGeneratedBP_Map)
{
    qDebug() << "Commencing generation of BP from file";

    BodyPart *newBP_Ptr = nullptr;
    instantiateNewBodyParts(alienPtr, BP_Type, 1);
    newBP_Ptr = alienPtr->bodyParts.last();
    origBP_ToGeneratedBP_Map.insert(origBP_ID,newBP_Ptr->BP_ID);
    newBP_Ptr->smoothnessLevel = smoothnessLevel;
    newBP_Ptr->BP_Type = BP_Type;

    QPolygon &sizeCompPoly = alienPtr->alienDNA->skeletonDNA->BP_SizeComparedToBackBone_DNA[BP_Type];
    for (int i=0; i<sizeCompPoly.length(); i++)
        if (sizeCompPoly[i].x() == origBP_ID)
        {
            sizeCompPoly[i].setX(newBP_Ptr->BP_ID);
            break;
        }

    QPolygon &angleArrPoly = alienPtr->alienDNA->skeletonDNA->BP_AngleArrangementsFromBackBone_DNA[BP_Type];
    for (int i=0; i<angleArrPoly.length(); i++)
        if (angleArrPoly[i].x() == origBP_ID)
        {
            angleArrPoly[i].setX(newBP_Ptr->BP_ID);
            break;
        }

    if (alienPtr->alienDNA->skeletonDNA->GroupBP_AttachmentToAnotherBP_ZoneScatteringTrend_DNA.contains(QPair<int,int>(BP_Type, origBP_ID)))
    {
        QPolygon grpAttPoly = alienPtr->alienDNA->skeletonDNA->GroupBP_AttachmentToAnotherBP_ZoneScatteringTrend_DNA.value(QPair<int,int>(BP_Type, origBP_ID));
        alienPtr->alienDNA->skeletonDNA->GroupBP_AttachmentToAnotherBP_ZoneScatteringTrend_DNA.insert(QPair<int,int>(BP_Type, newBP_Ptr->BP_ID), grpAttPoly);
        alienPtr->alienDNA->skeletonDNA->GroupBP_AttachmentToAnotherBP_ZoneScatteringTrend_DNA.remove(QPair<int,int>(BP_Type, origBP_ID));
    }


    if (alienPtr->alienDNA->skeletonDNA->numOfSkeletalBodyParts_DNA.contains(BP_Type))
        alienPtr->alienDNA->skeletonDNA->numOfSkeletalBodyParts_DNA[BP_Type]++; //++
    else
        alienPtr->alienDNA->skeletonDNA->numOfSkeletalBodyParts_DNA.insert(BP_Type, 1);
    qDebug() << alienPtr->alienDNA->skeletonDNA->numOfSkeletalBodyParts_DNA;

    // Copy the number of Joints
    // TODO: Make sure these are not redundant entries added to numOfJointsPerBP_DNA after we add a new entry for the clonedBP
    QPolygon tempPoly; tempPoly.clear();
    if (alienPtr->alienDNA->skeletonDNA->numOfJointsPerBP_DNA.contains(BP_Type))
    {
        tempPoly = alienPtr->alienDNA->skeletonDNA->numOfJointsPerBP_DNA.value(BP_Type);
    }
    tempPoly << QPoint(newBP_Ptr->BP_ID, numOfJnts);
    qDebug() << BP_Type;
    alienPtr->alienDNA->skeletonDNA->numOfJointsPerBP_DNA.insert(BP_Type,tempPoly);
    qDebug() << alienPtr->alienDNA->skeletonDNA->numOfJointsPerBP_DNA;

    QMap<int, QPolygon> tempNumOfJointsPerBP_DNA;
    QPolygon tempPoly_2; tempPoly_2.clear();
    tempPoly_2 << tempPoly.last();
    tempNumOfJointsPerBP_DNA.insert(BP_Type, tempPoly_2);
    instantiateNumOfJointsForAllBPs_TypeGuided(alienPtr, tempNumOfJointsPerBP_DNA);


    // Specify Jnt2Jnt Relations
    QVector<BodyPart*> tempBP_List; tempBP_List.clear();
    tempBP_List << newBP_Ptr;
    specifyJnt2JntRelations_TypeGuided(alienPtr, tempBP_List, false);

    // add the pairs(mappings) to the JntToJntMappingList_forCloningSourceToDest DS in the alien's skeleton
    updateJntToJntMappingList_forBookKeeping(alienPtr, &Jnt2JntRelations,
                                                       alienPtr->alienSkeleton.Jnt2JntRelations_TypeGuided.value(newBP_Ptr->BP_ID));

    QVector<Joint*> tempJntList; tempJntList.resize(0);
    getAllBP_Joints(newBP_Ptr, tempJntList);
    initializeJointsArrangementTrendForCurrentAlien(alienPtr, tempJntList, false);

    QMap<int,int> tempParentJntCompletionCheckList; // checklist for when the all the child joints's positions are computed. Value(int) is 1 for DONE and 0 for NOT_DONE.
    QMap<int,int> tempChildJntPositionCheckList;    // checklist for when the joint's position is computed.

    // Update the bookkeeping data structures in the JAT DNA
    for (int i=0; i<tempJntList.length(); i++)
    {
        QPair<QVector3D, QVector3D> tempPair;
        tempPair.first = QVector3D(0.0,0.0,1.0); tempPair.second = QVector3D(0.0,0.0,0.0);
        alienPtr->alienSkeleton.vectorLocationCoupleForEachJoint_TypeGuided.insert(tempJntList[i]->BP_ID, tempPair);

        tempParentJntCompletionCheckList.insert(tempJntList[i]->BP_ID,0);
        tempChildJntPositionCheckList.insert(tempJntList[i]->BP_ID,0);

        int tempSrcJnt_ID = alienPtr->alienSkeleton.JntToJntMappings_ForBookKeeping.key(tempJntList[i]->BP_ID);
        qDebug() << "source joint : " << tempSrcJnt_ID << "   cloned joint : " << tempJntList[i]->BP_ID;

        qDebug() << "randIntForBookKeeping_EnergyTrend : " << randIntForBookKeeping_EnergyTrend;
        if (randIntForBookKeeping_EnergyTrend.contains(tempSrcJnt_ID))
        {
            int tempEngyTrd = randIntForBookKeeping_EnergyTrend.value(tempSrcJnt_ID);
            alienPtr->alienDNA->skeletonDNA->skeletal_JATs_DNA.randIntForBookKeeping_EnergyTrend.insert(tempJntList[i]->BP_ID, tempEngyTrd);
            qDebug() << "source joint energy trend : " << tempEngyTrd;
        }
        // else
        //     exit(0);

        if (randFloatForBookKeeping_FinalEnergy.contains(tempSrcJnt_ID)) // ++
        {
            float tempFinEngy = randFloatForBookKeeping_FinalEnergy.value(tempSrcJnt_ID); // ++
            alienPtr->alienDNA->skeletonDNA->skeletal_JATs_DNA.randFloatForBookKeeping_FinalEnergy.insert(tempJntList[i]->BP_ID, tempFinEngy);
            qDebug() << "source joint final energy : " << tempFinEngy;
        }
        // else
        //     exit(1);

        if (randIntForBookKeeping_AngleTrend.contains(tempSrcJnt_ID))
        {
            int tempAngTrd = randIntForBookKeeping_AngleTrend.value(tempSrcJnt_ID);
            alienPtr->alienDNA->skeletonDNA->skeletal_JATs_DNA.randIntForBookKeeping_AngleTrend.insert(tempJntList[i]->BP_ID, tempAngTrd);
            qDebug() << "source joint angle trend : " << tempAngTrd;
        }
        // else
        //     exit(2);

        if (randIntForBookKeeping_quaterEulerAngleSelectorForFirstChild.contains(tempSrcJnt_ID))
        {
            int tempQuatEulAngForFirChd = randIntForBookKeeping_quaterEulerAngleSelectorForFirstChild.value(tempSrcJnt_ID);
            alienPtr->alienDNA->skeletonDNA->skeletal_JATs_DNA.randIntForBookKeeping_quaterEulerAngleSelectorForFirstChild.insert(tempJntList[i]->BP_ID, tempQuatEulAngForFirChd);
            qDebug() << "source joint Euler Angle For First Jnt : " << tempQuatEulAngForFirChd;
        }
        // else
        //     exit(3);

        if (randQVector3DForBookKeeping_quaterEulerAngleSelectorForNotFirstChild.contains(tempSrcJnt_ID))
        {
            QVector3D tempQuatEulAngForNotFirChd = randQVector3DForBookKeeping_quaterEulerAngleSelectorForNotFirstChild.value(tempSrcJnt_ID); // ++
            alienPtr->alienDNA->skeletonDNA->skeletal_JATs_DNA.randQVector3DForBookKeeping_quaterEulerAngleSelectorForNotFirstChild.insert(tempJntList[i]->BP_ID, tempQuatEulAngForNotFirChd);
            qDebug() << "source joint Euler Angle NOT For First Jnt : " << tempQuatEulAngForNotFirChd;
        }
        // else
        //     exit(4);
    }

    QPolygonF tempPolyF; tempPolyF.clear();
    if (alienPtr->alienDNA->skeletonDNA->skeletal_JATs_DNA.BP_JAT_rootJoint_DNA.contains(newBP_Ptr->BP_Type))
    {
        tempPolyF = alienPtr->alienDNA->skeletonDNA->skeletal_JATs_DNA.BP_JAT_rootJoint_DNA.value(newBP_Ptr->BP_Type);
    }
    tempPolyF.append(BP_to_JAT_rootJoint);
    alienPtr->alienDNA->skeletonDNA->skeletal_JATs_DNA.BP_JAT_rootJoint_DNA.insert(newBP_Ptr->BP_Type, tempPolyF);
    alienPtr->alienDNA->skeletonDNA->skeletal_JATs_DNA.BP_to_JAT_rootJoint_Map_DNA.insert(newBP_Ptr->BP_ID, BP_to_JAT_rootJoint);

    QMap<int, QPolygonF> tempCopy_BP_JAT_rootJoint_DNA; tempCopy_BP_JAT_rootJoint_DNA.clear();
    QPolygonF polyF; polyF.clear();
    polyF << tempPolyF.at(tempPolyF.length()-2) << tempPolyF.at(tempPolyF.length()-1);
    tempCopy_BP_JAT_rootJoint_DNA.insert(newBP_Ptr->BP_Type, polyF);

    compute_JAT_AND_VecLocCplForEachJnt_ForBP(alienPtr, newBP_Ptr->BP_ID, tempCopy_BP_JAT_rootJoint_DNA,
                                              tempParentJntCompletionCheckList, tempChildJntPositionCheckList,
                                              0.0);
    // model all the joints of this BP
    modelAllAlienJoints_TypeGuided(alienPtr, tempJntList);

    // Model all the bones
    // TODO(ismail): make a method for extracting a sub map of Jnt2JntRelations_TypeGuided given a set of BP_IDs
    // ... COuld be handy for some purposes
    QMap<int, QPolygon*> tempJnt2JntRelations; tempJnt2JntRelations.clear();
    if (alienPtr->alienSkeleton.Jnt2JntRelations_TypeGuided.contains(newBP_Ptr->BP_ID))
    {
        QPolygon *tempNewPolyPtr = new QPolygon(*(alienPtr->alienSkeleton.Jnt2JntRelations_TypeGuided.value(newBP_Ptr->BP_ID)));
        tempJnt2JntRelations.insert(newBP_Ptr->BP_ID, tempNewPolyPtr);
        // model the bones of this BP
        modelAllAlienBones_TypeGuided(alienPtr, tempJnt2JntRelations);

        // clone its bulkiness DNA
        alienPtr->alienDNA->skeletonDNA->Skeletal_BP_Bulkiness_DNA_ForEachBP.insert(newBP_Ptr->BP_ID, Skeletal_BP_Bulkiness_DNA);

        // bulk it up
        QVector<int> tempBP_ListToBulk; tempBP_ListToBulk.clear();
        tempBP_ListToBulk << newBP_Ptr->BP_ID;
        bulkUpBodyPart(alienPtr,tempBP_ListToBulk);

        // Now do the densifications
        if (!BP_GenDensificationDNA.isEmpty())
        {
            alienPtr->alienDNA->skeletonDNA->BP_GenDensificationDNA.insert(newBP_Ptr->BP_ID, BP_GenDensificationDNA);
            if (!BP_SpecificDensificationDNA.isEmpty())
            {
                QVector<Bone*> bonesInOrderGenBP; bonesInOrderGenBP.clear();
                getAllBP_BonesInOrder(alienPtr, newBP_Ptr, bonesInOrderGenBP);
                if (bonesIDsInOrder.length() == bonesInOrderGenBP.length())
                {
                    QMap<int, QPolygonF> specDensDNA_Map; specDensDNA_Map.clear();
                    for (int b=0; b<bonesInOrderGenBP.length(); b++)
                    {
                        specDensDNA_Map.insert(bonesInOrderGenBP[b]->BP_ID, BP_SpecificDensificationDNA.value(bonesIDsInOrder.at(b)));
                    }
                    alienPtr->alienDNA->skeletonDNA->BP_SpecificDensificationDNA.insert(newBP_Ptr->BP_ID, specDensDNA_Map);
                    QVector<int> toBeDensifiedBPsNewAlien; toBeDensifiedBPsNewAlien.clear();
                    toBeDensifiedBPsNewAlien << newBP_Ptr->BP_ID;
                    densifyAllBodyParts(alienPtr, toBeDensifiedBPsNewAlien);
                }
                else
                {
                    qDebug() << "bonesIDsInOrder.length() != bonesInOrderGenBP.length()";
                    exit(0);
                }
            }
        }

        // Now do the bulgifications
        if (!BP_BulgificationDNA.isEmpty())
        {
            qDebug() << BP_BulgificationDNA;
            qDebug() << bonesIDsInOrder;
            QList<int> bonesIDs; bonesIDs.clear();
            if (!BP_BulgificationDNA.contains(bonesIDsInOrder.first()))
            {
                bonesIDs = BP_BulgificationDNA.keys();
            }
            else
                bonesIDs = bonesIDsInOrder;
            QVector<Bone*> bonesInOrderGenBP; bonesInOrderGenBP.clear();
            getAllBP_BonesInOrder(alienPtr, newBP_Ptr, bonesInOrderGenBP);
            QMap<int, QPolygon> bulgDNA_Map; bulgDNA_Map.clear();
            for (int b=0; b<bonesInOrderGenBP.length(); b++)
            {
                bulgDNA_Map.insert(bonesInOrderGenBP[b]->BP_ID, BP_BulgificationDNA.value(bonesIDs.at(b)));
            }
            alienPtr->alienDNA->skeletonDNA->BP_BulgificationDNA.insert(newBP_Ptr->BP_ID, bulgDNA_Map);
            bulgifyBodyPart(alienPtr, newBP_Ptr);
        }

        tempCopy_BP_JAT_rootJoint_DNA.clear();
        tempParentJntCompletionCheckList.clear();
        tempChildJntPositionCheckList.clear();
        tempJnt2JntRelations.clear();
        tempJntList.clear();
        tempBP_List.clear();
        tempPoly_2.clear();
        tempPoly.clear();
    }
    else
    {
        qDebug() << "no entry found for this BP in Jnt2JntRelations_TypeGuided - generateBP_FromFile method";
        exit(0);
    }

    qDebug() << "BP generated from file successfully";

    return newBP_Ptr;
}

/*
 * Things to store for saving and loading the model
 *
 *  +++ Alien Specific DSs
 *  Alien_Type_ID
 *  BP_TypeColorMap_2 - Might not be needed
 *  GLW_to_Alien_to_BP_to_ColorAndDisplay_Map - Might not be needed
 *  numOfSkeletalBodyParts_DNA - might not be needed
 *  BP_SizeComparedToBackBone_DNA
 *  BP_AngleArrangementsFromBackBone_DNA
 *  numOfJointsPerBP_DNA
 *  BP_JAT_rootJoint_DNA
 *  JntToJntMappings_ForBookKeeping - perhaps not needed as it will be constructed at runtime
 *  randIntForBookKeeping_EnergyTrend
 *  randFloatForBookKeeping_FinalEnergy
 *  randIntForBookKeeping_AngleTrend
 *  randIntForBookKeeping_quaterEulerAngleSelectorForFirstChild
 *  randQVector3DForBookKeeping_quaterEulerAngleSelectorForNotFirstChild
 *  Skeletal_BP_Bulkiness_DNA_ForEachBP
 *  BP_GenDensificationDNA
 *  headStateMachine
 *  QMap<int,QPOlygon> ==> QMap<Parent BP_ID, QPolygon containing all the child BP IDs>
 *  QMap<int,QMap<int,QPOlygon>> ==> Outer map: Key(int) is the Parent BP_ID. Inner Map: Key(int) is the ExtID.
 *  .................................           QPolygon contains all the carpals and digits IDs. First Carpal is the root Carpal
 *
 *  +++ BP specific DSs +++
 *
 *  Jnt2JntRelations_TypeGuided
 *  BP_to_JAT_rootJoint_Map_DNA
 *  BP_OrientationQuat of the BP
 *  BP_TranslateMatrix of the BP
 *  BP_ScalingMatrix of the BP
 *  clonedFrom
 *  cloneFromAlienAndBP_Pair
 *  isSymmetric
 *  mirrorOf
 *  smoothnessLevel
 *  BP_BulgificationDNA
 *  BP_SpecificDensificationDNA
 *
*/


void AlienModeller::cloneAlienSkeleton(Alien *sourceAlien, Alien *clonedAlien)
{
/*
    QMap<int, bool> tempClonedBP_Checklist;
    for (int i=0; i<sourceAlien->bodyParts.length(); i++)
    {
        tempClonedBP_Checklist.insert(sourceAlien->bodyParts[i]->BP_ID, false);
    }

    for (int i=0; i<sourceAlien->bodyParts.length(); i++)
    {
        cloneSkeletalBP_viaGeometry(sourceAlien->bodyParts[i], clonedAlien, tempClonedBP_Checklist);
    }
*/
}

void AlienModeller::cloneAlienDNA(Alien *sourceAlien, Alien *clonedAlien)
{
    clonedAlien->alienDNA = new Alien_DNA();
    cloneSkeletonDNA(sourceAlien, clonedAlien);
}

void AlienModeller::cloneSkeletonDNA(Alien *sourceAlien, Alien *clonedAlien)
{
    // IMPORTANT(ismail): This method can lead to potential bugs.
    //                    The JAT DNAs being copied are taking into account the Joint IDs.
    //                    If the cloned aliens generates joints with different IDs, this method will break the code.

    clonedAlien->alienDNA->skeletonDNA = new Skeleton_DNA();

    clonedAlien->alienDNA->skeletonDNA->numOfSkeletalBodyParts_DNA = QMap<int,int>(sourceAlien->alienDNA->skeletonDNA->numOfSkeletalBodyParts_DNA);
    clonedAlien->alienDNA->skeletonDNA->numOfJointsPerBP_DNA = QMap<int, QPolygon>(sourceAlien->alienDNA->skeletonDNA->numOfJointsPerBP_DNA);
    clonedAlien->alienDNA->skeletonDNA->BP_AttachmentToBackBone_ZoneAndScattering_DNA =
                                        QMap<int, QPair<float, float>>(sourceAlien->alienDNA->skeletonDNA->BP_AttachmentToBackBone_ZoneAndScattering_DNA);
    clonedAlien->alienDNA->skeletonDNA->BP_SizeComparedToBackBone_DNA = QMap<int, QPolygon>(sourceAlien->alienDNA->skeletonDNA->BP_SizeComparedToBackBone_DNA);
    clonedAlien->alienDNA->skeletonDNA->BP_AngleArrangementsFromBackBone_DNA = QMap<int, QPolygon>(sourceAlien->alienDNA->skeletonDNA->BP_AngleArrangementsFromBackBone_DNA);
    clonedAlien->alienDNA->skeletonDNA->OtherBPsToGrow_DNA = QMap<int, QPolygon>(sourceAlien->alienDNA->skeletonDNA->OtherBPsToGrow_DNA);
    // set the bulkiness DNA manipulation flags
    manipulateSkeletalBulkinessDNA_Flags(clonedAlien, false, true, false, false, false, new QVector<int>(0), new QVector<int>(0));

//    clonedAlien->alienDNA->skeletonDNA->Skeletal_BP_Bulkiness_2_DNA = QMap<int, QPolygonF>(sourceAlien->alienDNA->skeletonDNA->Skeletal_BP_Bulkiness_2_DNA);
//    clonedAlien->alienDNA->skeletonDNA->BoneBending_DNA = QMap<int, QPolygonF>(sourceAlien->alienDNA->skeletonDNA->BoneBending_DNA);

    // +++ Now clone the JointsArrangementTrend
    clonedAlien->alienDNA->skeletonDNA->skeletal_JATs_DNA.BP_JAT_rootJoint_DNA =
                                            QMap<int, QPolygonF>(sourceAlien->alienDNA->skeletonDNA->skeletal_JATs_DNA.BP_JAT_rootJoint_DNA);
    clonedAlien->alienDNA->skeletonDNA->skeletal_JATs_DNA.randIntForBookKeeping_EnergyTrend =
                                QMap<int, int>(sourceAlien->alienDNA->skeletonDNA->skeletal_JATs_DNA.randIntForBookKeeping_EnergyTrend);
    clonedAlien->alienDNA->skeletonDNA->skeletal_JATs_DNA.randFloatForBookKeeping_FinalEnergy =
                                QMap<int, float>(sourceAlien->alienDNA->skeletonDNA->skeletal_JATs_DNA.randFloatForBookKeeping_FinalEnergy);
    clonedAlien->alienDNA->skeletonDNA->skeletal_JATs_DNA.randIntForBookKeeping_AngleTrend =
                                QMap<int, int>(sourceAlien->alienDNA->skeletonDNA->skeletal_JATs_DNA.randIntForBookKeeping_AngleTrend);
    clonedAlien->alienDNA->skeletonDNA->skeletal_JATs_DNA.randIntForBookKeeping_quaterEulerAngleSelectorForFirstChild =
            QMap<int, int>(sourceAlien->alienDNA->skeletonDNA->skeletal_JATs_DNA.randIntForBookKeeping_quaterEulerAngleSelectorForFirstChild);
    clonedAlien->alienDNA->skeletonDNA->skeletal_JATs_DNA.randQVector3DForBookKeeping_quaterEulerAngleSelectorForNotFirstChild =
            QMap<int, QVector3D>(sourceAlien->alienDNA->skeletonDNA->skeletal_JATs_DNA.randQVector3DForBookKeeping_quaterEulerAngleSelectorForNotFirstChild);
    clonedAlien->alienDNA->skeletonDNA->skeletal_JATs_DNA.BP_JAT_needRecomputingIndicator =
            QMap<int, bool>(sourceAlien->alienDNA->skeletonDNA->skeletal_JATs_DNA.BP_JAT_needRecomputingIndicator);

    // clone the numOfSkeletalBodyParts_DNA
//    QMapIterator<int, int> numBP_Iter(sourceAlien->alienDNA->skeletonDNA->numOfSkeletalBodyParts_DNA);
//    while (numBP_Iter.hasNext())
//    {
//        numBP_Iter.next();
//        clonedAlien->alienDNA->skeletonDNA->numOfSkeletalBodyParts_DNA.insert(numBP_Iter.key(), numBP_Iter.value());
//    }

    // clone the numOfJointsPerBP_DNA
//    QMapIterator<int, QPolygon> numJntsIter(sourceAlien->alienDNA->skeletonDNA->numOfJointsPerBP_DNA);
//    while (numJntsIter.hasNext())
//    {
//        numJntsIter.next();
//        clonedAlien->alienDNA->skeletonDNA->numOfJointsPerBP_DNA.insert(numJntsIter.key(), QPolygon(numJntsIter.value()));
//    }

    // clone the BP_AttachmentToBackBone_ZoneAndScattering_DNA
//    QMapIterator<int, QPair<float,float>> BP_attachIter(sourceAlien->alienDNA->skeletonDNA->BP_AttachmentToBackBone_ZoneAndScattering_DNA);
//    while (BP_attachIter.hasNext())
//    {
//        BP_attachIter.next();
//        clonedAlien->alienDNA->skeletonDNA->BP_AttachmentToBackBone_ZoneAndScattering_DNA.insert(BP_attachIter.key(),
//                                                                    QPair<float,float>(BP_attachIter.value().first, BP_attachIter.value().second));
//    }

    // clone the BP_SizeComparedToBackBone_DNA
//    QMapIterator<int, QPolygon> BP_sizeIter(sourceAlien->alienDNA->skeletonDNA->BP_SizeComparedToBackBone_DNA);
//    while (BP_sizeIter.hasNext())
//    {
//        BP_sizeIter.next();
//        clonedAlien->alienDNA->skeletonDNA->BP_SizeComparedToBackBone_DNA.insert(BP_sizeIter.key(), QPolygon(BP_sizeIter.value()));
//    }

    // clone the BP_AngleArrangementsFromBackBone_DNA
//    QMapIterator<int, QPolygon> BP_angleIter(sourceAlien->alienDNA->skeletonDNA->BP_AngleArrangementsFromBackBone_DNA);
//    while (BP_angleIter.hasNext())
//    {
//        BP_angleIter.next();
//        clonedAlien->alienDNA->skeletonDNA->BP_AngleArrangementsFromBackBone_DNA.insert(BP_angleIter.key(), QPolygon(BP_angleIter.value()));
//    }

    // clone the OtherBPsToGrow_DNA
//    QMapIterator<int, QPolygon> otherBPToGrowIter(sourceAlien->alienDNA->skeletonDNA->OtherBPsToGrow_DNA);
//    while (otherBPToGrowIter.hasNext())
//    {
//        otherBPToGrowIter.next();
//        clonedAlien->alienDNA->skeletonDNA->OtherBPsToGrow_DNA.insert(otherBPToGrowIter.key(), QPolygon(otherBPToGrowIter.value()));
//    }

    // clone the Skeletal_BP_Bulkiness_2_DNA
//    QMapIterator<int, QPolygonF> BP_BulkinessIter(sourceAlien->alienDNA->skeletonDNA->Skeletal_BP_Bulkiness_2_DNA);
//    while (BP_BulkinessIter.hasNext())
//    {
//        BP_BulkinessIter.next();
//        clonedAlien->alienDNA->skeletonDNA->Skeletal_BP_Bulkiness_2_DNA.insert(BP_BulkinessIter.key(), QPolygonF(BP_BulkinessIter.value()));
//    }

    //
    // +++ Now clone the JointsArrangementTrend
    //

    // clone the BP_JAT_rootJoint_DNA
//    QMapIterator<int, QPolygonF> JAT_1(sourceAlien->alienDNA->skeletonDNA->skeletal_JATs_DNA.BP_JAT_rootJoint_DNA);
//    while (JAT_1.hasNext())
//    {
//        JAT_1.next();
//        clonedAlien->alienDNA->skeletonDNA->skeletal_JATs_DNA.BP_JAT_rootJoint_DNA.insert(JAT_1.key(), QPolygonF(JAT_1.value()));
//    }

//    QMapIterator<int, QPolygonF> JAT_2(sourceAlien->alienDNA->skeletonDNA->skeletal_JATs_DNA.VectorEnergyAngleTrends_TypeGuided);
//    while (JAT_2.hasNext())
//    {
//        JAT_2.next();
//        clonedAlien->alienDNA->skeletonDNA->skeletal_JATs_DNA.VectorEnergyAngleTrends_TypeGuided.insert(JAT_2.key(), QPolygonF(JAT_2.value()));
//    }

//    QMapIterator<int, int> JAT_3(sourceAlien->alienDNA->skeletonDNA->skeletal_JATs_DNA.randIntForBookKeeping_EnergyTrend);
//    while (JAT_3.hasNext())
//    {
//        JAT_3.next();
//        clonedAlien->alienDNA->skeletonDNA->skeletal_JATs_DNA.randIntForBookKeeping_EnergyTrend.insert(JAT_3.key(), JAT_3.value());
//    }

//    QMapIterator<int, float> JAT_4(sourceAlien->alienDNA->skeletonDNA->skeletal_JATs_DNA.randFloatForBookKeeping_FinalEnergy);
//    while (JAT_4.hasNext())
//    {
//        JAT_4.next();
//        clonedAlien->alienDNA->skeletonDNA->skeletal_JATs_DNA.randFloatForBookKeeping_FinalEnergy.insert(JAT_4.key(), JAT_4.value());
//    }

//    QMapIterator<int, int> JAT_5(sourceAlien->alienDNA->skeletonDNA->skeletal_JATs_DNA.randIntForBookKeeping_AngleTrend);
//    while (JAT_5.hasNext())
//    {
//        JAT_5.next();
//        clonedAlien->alienDNA->skeletonDNA->skeletal_JATs_DNA.randIntForBookKeeping_AngleTrend.insert(JAT_5.key(), JAT_5.value());
//    }

//    QMapIterator<int, int> JAT_6(sourceAlien->alienDNA->skeletonDNA->skeletal_JATs_DNA.randIntForBookKeeping_quaterEulerAngleSelectorForFirstChild);
//    while (JAT_6.hasNext())
//    {
//        JAT_6.next();
//        clonedAlien->alienDNA->skeletonDNA->skeletal_JATs_DNA.randIntForBookKeeping_quaterEulerAngleSelectorForFirstChild.insert(JAT_6.key(), JAT_6.value());
//    }

//    QMapIterator<int, QVector3D> JAT_7(sourceAlien->alienDNA->skeletonDNA->skeletal_JATs_DNA.randFloatForBookKeeping_quaterEulerAngleSelectorForNotFirstChild);
//    while (JAT_7.hasNext())
//    {
//        JAT_7.next();
//        clonedAlien->alienDNA->skeletonDNA->skeletal_JATs_DNA.randFloatForBookKeeping_quaterEulerAngleSelectorForNotFirstChild.insert(JAT_7.key(),
//                                                                                     QVector3D(JAT_7.value().x(),JAT_7.value().y(),JAT_7.value().z()));
//    }

}

BodyPart *AlienModeller::cloneSkeletalBP_viaGeometry(Alien *sourceAlien, BodyPart *sourceBP_Ptr, Alien *alienPtr)
{
//    BodyPart *tempNewBR_Ptr = SourceBP_Ptr->duplicateBP(clonedAlien, tempClonedBP_Checklist);

//    // Make sure it is added to the BP list in Alien
////    addBodyPartToCurrentAlienSkeleton(tempNewBR_Ptr);
//    addAllSubBPs_ToAlienRecursive(clonedAlien, tempNewBR_Ptr);

    /*
     * ALGORITHM ********
     *
     * Instantiate a new BP for the source alien
     *
     * Update the relevant DNA DSs such as numOfBPs, numOfJnts, etc
     *
     * Instantiate the joints for the clonedBP
     *
     * Model the joints at the same location as source joints
     *
     * Create the Jnt2JntRel DS for the clonedBP
     *
     * If this BP has not been smoothed, then update all the bookkeeping DNA DSs
     *
     * else
     *      model all the bones for this clonedBP
     *
     * make the bulkiness DNA
     *
     * bulk it up
     *
     */

    qDebug() << "Commencing cloning of BP via Geometry";

    BodyPart *newCloneBP_Ptr = nullptr;
    instantiateNewBodyParts(alienPtr, sourceBP_Ptr->BP_Type, 1);
    newCloneBP_Ptr = alienPtr->bodyParts.last();
    newCloneBP_Ptr->clonedFrom = sourceBP_Ptr->BP_ID;
    newCloneBP_Ptr->cloneFromAlienAndBP_Pair = QPair<int,int>(sourceAlien->alienID, sourceBP_Ptr->BP_ID);
    newCloneBP_Ptr->isSymmetric = sourceBP_Ptr->isSymmetric;

    if (alienPtr->alienID == sourceAlien->alienID)
    {
        alienPtr->cloneBPs.append(newCloneBP_Ptr);

        // Also adjust the number of BPs in the DNA
        int tempNumOfBPs = alienPtr->alienDNA->skeletonDNA->numOfSkeletalBodyParts_DNA.value(newCloneBP_Ptr->BP_Type);
        alienPtr->alienDNA->skeletonDNA->numOfSkeletalBodyParts_DNA.insert(newCloneBP_Ptr->BP_Type, ++tempNumOfBPs);

        // Also adjust the BP_AngleArrangementsFromBackBone_DNA by adding one more entry to it
        if (newCloneBP_Ptr->BP_Type != AlienModeller::BodyPartsEnum::BACK_BONE) // Backbone cannot be attached to itself
        {
            QPolygon tempPoly = skeletonRuleSet->BP_AngleArrangementsFromBackBone_RS.value(newCloneBP_Ptr->BP_Type);

            alienPtr->alienDNA->skeletonDNA->BP_AngleArrangementsFromBackBone_DNA[newCloneBP_Ptr->BP_Type].append(
                        QPoint(-1,GeometryAlgorithms::getRandomInteger(tempPoly[alienPtr->Alien_Type_ID].x(),tempPoly[alienPtr->Alien_Type_ID].y())));
        }
    }

    // Copy the relevant DNA from the source to the dest BP

    QPolygon tempPoly; tempPoly.clear();
    if (alienPtr->alienDNA->skeletonDNA->numOfJointsPerBP_DNA.contains(newCloneBP_Ptr->BP_Type))
    {
        tempPoly = alienPtr->alienDNA->skeletonDNA->numOfJointsPerBP_DNA.value(newCloneBP_Ptr->BP_Type);
    }
    tempPoly << QPoint(newCloneBP_Ptr->BP_ID, getNumOfJntsForBP(sourceBP_Ptr));
    alienPtr->alienDNA->skeletonDNA->numOfJointsPerBP_DNA.insert(newCloneBP_Ptr->BP_Type,tempPoly);


    QMap<int, QPolygon> tempNumOfJointsPerBP_DNA;
    QPolygon tempPoly_2; tempPoly_2.clear();
    tempPoly_2 << tempPoly.last();
    tempNumOfJointsPerBP_DNA.insert(newCloneBP_Ptr->BP_Type, tempPoly_2);
    instantiateNumOfJointsForAllBPs_TypeGuided(alienPtr, tempNumOfJointsPerBP_DNA);
    qDebug() << tempNumOfJointsPerBP_DNA;
    qDebug() << newCloneBP_Ptr->subBodyParts.length();

    QVector<BodyPart*> tempBP_List; tempBP_List.clear();
    tempBP_List << newCloneBP_Ptr;
    specifyJnt2JntRelations_TypeGuided(alienPtr, tempBP_List, false);

    // add the pairs(mappings) to the JntToJntMappingList_forCloningSourceToDest DS in the alien's skeleton
    updateJntToJntMappingList_forBookKeeping(alienPtr, sourceAlien->alienSkeleton.Jnt2JntRelations_TypeGuided.value(sourceBP_Ptr->BP_ID),
                                                       alienPtr->alienSkeleton.Jnt2JntRelations_TypeGuided.value(newCloneBP_Ptr->BP_ID));

    // Now go through the Jnt2Jnt mapping and model the joints at the same location as
    QMapIterator<int, int> iter(alienPtr->alienSkeleton.JntToJntMappings_ForBookKeeping);
    while (iter.hasNext())
    {
        iter.next();

        alienPtr->alienSkeleton.vectorLocationCoupleForEachJoint_TypeGuided.insert(iter.value(),
                  QPair<QVector3D,QVector3D>(alienPtr->alienSkeleton.vectorLocationCoupleForEachJoint_TypeGuided.value(iter.key())));
    }

    QVector<Joint*> tempJntList; tempJntList.resize(0);
    getAllBP_Joints(newCloneBP_Ptr, tempJntList);
    // model the joints of this BP
    modelAllAlienJoints_TypeGuided(alienPtr, tempJntList);

    // TODO(ismail): make a method for extracting a sub map of Jnt2JntRelations_TypeGuided given a set of BP_IDs
    // ... COuld be handy for some purposes
    QMap<int, QPolygon*> tempJnt2JntRelations;
    QPolygon *tempNewPolyPtr = new QPolygon(*(alienPtr->alienSkeleton.Jnt2JntRelations_TypeGuided.value(newCloneBP_Ptr->BP_ID)));
    tempJnt2JntRelations.insert(newCloneBP_Ptr->BP_ID, tempNewPolyPtr);
    // model the bones of this BP
    modelAllAlienBones_TypeGuided(alienPtr, tempJnt2JntRelations);

    // clone its bulkiness DNA
    // TODO(ismail): Make a method cloneBulkinessDNA_FromBP_To_BP() which will copy one BP's bulkiness DNA
    // ... to another
    QPolygonF tempBulkPoly = sourceAlien->alienDNA->skeletonDNA->Skeletal_BP_Bulkiness_DNA_ForEachBP.value(sourceBP_Ptr->BP_ID);
    alienPtr->alienDNA->skeletonDNA->Skeletal_BP_Bulkiness_DNA_ForEachBP.insert(newCloneBP_Ptr->BP_ID, tempBulkPoly);

    // bulk it up
    QVector<int> tempBP_ListToBulk; tempBP_ListToBulk.clear();
    tempBP_ListToBulk << newCloneBP_Ptr->BP_ID;
    bulkUpBodyPart(alienPtr,tempBP_ListToBulk);

    // finally copy the smoothness level as well
    newCloneBP_Ptr->smoothnessLevel = sourceBP_Ptr->smoothnessLevel;

    // TODO(ismail): Scale the cloned BP by taking into account the scaling of the source BP as well

    // Generate the cloned BP at the same location as the source BP, and also rotate it accordingly

    tempNumOfJointsPerBP_DNA.clear();
    tempJnt2JntRelations.clear();
    tempJntList.clear();
    tempBP_List.clear();
    tempPoly_2.clear();
    tempPoly.clear();

    qDebug() << "BP cloned successfully";

    return newCloneBP_Ptr;
}

BodyPart *AlienModeller::cloneSkeletalBP_FromAnotherAlien_viaDNA(Alien *alienPtr, Alien *sourceAlien, BodyPart *sourceBP_Ptr,
                                                                 int numOfClones, bool isMirror,
                                                                 QPointF JAT_EnergyPt, QPointF JAT_AnglePt)
{
    qDebug() << "Commencing cloning of BP via DNA";
    // Create a new BP and add it to the Alien. Set its cloneFrom to the appropriate value
    BodyPart *newCloneBP_Ptr = nullptr;
    for (int bCnt=0; bCnt<numOfClones; bCnt++)
    {
        // Instantiate a new BP
        instantiateNewBodyParts(alienPtr, sourceBP_Ptr->BP_Type, 1);
        newCloneBP_Ptr = alienPtr->bodyParts.last();
        newCloneBP_Ptr->clonedFrom = sourceBP_Ptr->BP_ID;
        newCloneBP_Ptr->cloneFromAlienAndBP_Pair = QPair<int,int>(sourceAlien->alienID, sourceBP_Ptr->BP_ID);
        newCloneBP_Ptr->isSymmetric = sourceBP_Ptr->isSymmetric;
        newCloneBP_Ptr->mirrorOf = sourceBP_Ptr->mirrorOf;

        // Copy the relevant DNA from the source to the dest BP
        if (alienPtr->alienDNA->skeletonDNA->numOfSkeletalBodyParts_DNA.contains(newCloneBP_Ptr->BP_Type))
            alienPtr->alienDNA->skeletonDNA->numOfSkeletalBodyParts_DNA[newCloneBP_Ptr->BP_Type]++; //++
        else
            alienPtr->alienDNA->skeletonDNA->numOfSkeletalBodyParts_DNA.insert(newCloneBP_Ptr->BP_Type, 1);

        if (alienPtr->alienID == sourceAlien->alienID)
        {
            alienPtr->cloneBPs.append(newCloneBP_Ptr);

            // Also adjust the number of BPs in the DNA
            // int tempNumOfBPs = alienPtr->alienDNA->skeletonDNA->numOfSkeletalBodyParts_DNA.value(newCloneBP_Ptr->BP_Type);
            // alienPtr->alienDNA->skeletonDNA->numOfSkeletalBodyParts_DNA.insert(newCloneBP_Ptr->BP_Type, ++tempNumOfBPs);

            // Also adjust the BP_AngleArrangementsFromBackBone_DNA by adding one more entry to it
            if (newCloneBP_Ptr->BP_Type != AlienModeller::BodyPartsEnum::BACK_BONE) // Backbone cannot be attached to itself
            {
                if (skeletonRuleSet->BP_AngleArrangementsFromBackBone_RS.contains(newCloneBP_Ptr->BP_Type))
                {
                    QPolygon tempPoly = skeletonRuleSet->BP_AngleArrangementsFromBackBone_RS.value(newCloneBP_Ptr->BP_Type);

                    alienPtr->alienDNA->skeletonDNA->BP_AngleArrangementsFromBackBone_DNA[newCloneBP_Ptr->BP_Type].append(
                                QPoint(-1,GeometryAlgorithms::getRandomInteger(tempPoly[alienPtr->Alien_Type_ID].x(),tempPoly[alienPtr->Alien_Type_ID].y()))); //++
                }
            }
        }


        // Copy the number of Joints
        // TODO: Make sure there are not redundant entries added to numOfJointsPerBP_DNA after we add a new entry for the clonedBP
        QPolygon tempPoly; tempPoly.clear();
        if (alienPtr->alienDNA->skeletonDNA->numOfJointsPerBP_DNA.contains(newCloneBP_Ptr->BP_Type)) //++
        {
            tempPoly = alienPtr->alienDNA->skeletonDNA->numOfJointsPerBP_DNA.value(newCloneBP_Ptr->BP_Type);
        }
        tempPoly << QPoint(newCloneBP_Ptr->BP_ID, getNumOfJntsForBP(sourceBP_Ptr));
        alienPtr->alienDNA->skeletonDNA->numOfJointsPerBP_DNA.insert(newCloneBP_Ptr->BP_Type,tempPoly);

        // instantiate joints
        QMap<int, QPolygon> tempNumOfJointsPerBP_DNA;
        QPolygon tempPoly_2; tempPoly_2.clear();
        tempPoly_2 << tempPoly.last();
        tempNumOfJointsPerBP_DNA.insert(newCloneBP_Ptr->BP_Type, tempPoly_2);
        instantiateNumOfJointsForAllBPs_TypeGuided(alienPtr, tempNumOfJointsPerBP_DNA);
        qDebug() << tempNumOfJointsPerBP_DNA;
        qDebug() << newCloneBP_Ptr->subBodyParts.length();

        // Specify Jnt2Jnt Relations
        QVector<BodyPart*> tempBP_List; tempBP_List.clear();
        tempBP_List << newCloneBP_Ptr;
        specifyJnt2JntRelations_TypeGuided(alienPtr, tempBP_List, false);

        // add the pairs(mappings) to the JntToJntMappingList_forCloningSourceToDest DS in the alien's skeleton
        updateJntToJntMappingList_forBookKeeping(alienPtr, sourceAlien->alienSkeleton.Jnt2JntRelations_TypeGuided.value(sourceBP_Ptr->BP_ID),
                                                           alienPtr->alienSkeleton.Jnt2JntRelations_TypeGuided.value(newCloneBP_Ptr->BP_ID)); //++

    //    alienPtr->alienDNA->skeletonDNA->skeletal_JATs_DNA.BP_to_JAT_rootJoint_Map_DNA.insert(newCloneBP_Ptr->BP_ID,
    //         alienPtr->alienDNA->skeletonDNA->skeletal_JATs_DNA.BP_to_JAT_rootJoint_Map_DNA.value(sourceBP_Ptr->BP_ID));

        QVector<Joint*> tempJntList; tempJntList.resize(0);
        getAllBP_Joints(newCloneBP_Ptr, tempJntList);
        initializeJointsArrangementTrendForCurrentAlien(alienPtr, tempJntList, false, JAT_EnergyPt, JAT_AnglePt);

        QMap<int,int> tempParentJntCompletionCheckList; // checklist for when the all the child joints's positions are computed. Value(int) is 1 for DONE and 0 for NOT_DONE.
        QMap<int,int> tempChildJntPositionCheckList;    // checklist for when the joint's position is computed.

        // Update the bookkeeping data structures in the JAT DNA
        for (int i=0; i<tempJntList.length(); i++)
        {
            QPair<QVector3D, QVector3D> tempPair;
            tempPair.first = QVector3D(0.0,0.0,1.0); tempPair.second = QVector3D(0.0,0.0,0.0);
            alienPtr->alienSkeleton.vectorLocationCoupleForEachJoint_TypeGuided.insert(tempJntList[i]->BP_ID, tempPair);

            tempParentJntCompletionCheckList.insert(tempJntList[i]->BP_ID,0);
            tempChildJntPositionCheckList.insert(tempJntList[i]->BP_ID,0);

            int tempSrcJnt_ID = alienPtr->alienSkeleton.JntToJntMappings_ForBookKeeping.key(tempJntList[i]->BP_ID); //++
            qDebug() << "source joint : " << tempSrcJnt_ID << "   cloned joint : " << tempJntList[i]->BP_ID;

            if (JAT_EnergyPt.x() == -1)
            {
                int tempEngyTrd = sourceAlien->alienDNA->skeletonDNA->skeletal_JATs_DNA.randIntForBookKeeping_EnergyTrend.value(tempSrcJnt_ID); // ++
                alienPtr->alienDNA->skeletonDNA->skeletal_JATs_DNA.randIntForBookKeeping_EnergyTrend.insert(tempJntList[i]->BP_ID, tempEngyTrd);
                qDebug() << "source joint energy trend : " << tempEngyTrd;

                if (JAT_EnergyPt.y() == -1)
                {
                    float tempFinEngy = sourceAlien->alienDNA->skeletonDNA->skeletal_JATs_DNA.randFloatForBookKeeping_FinalEnergy.value(tempSrcJnt_ID); // ++
                    // If the BP needs to be squashed/stretched, then we simply multiply the energy with the z scaling value of the bulkiness DNA.
                    // This neat trick ensures that the joint will automatically be generated at the end of the squashed bone.
                    tempFinEngy *= sourceAlien->getZScalingFactorFromBulkinessDNA(sourceBP_Ptr);
                    alienPtr->alienDNA->skeletonDNA->skeletal_JATs_DNA.randFloatForBookKeeping_FinalEnergy.insert(tempJntList[i]->BP_ID, tempFinEngy);
                    qDebug() << "source joint final energy : " << tempFinEngy;
                }
            }

//            else
//            {
//                float tempFinEngy = sourceAlien->alienDNA->skeletonDNA->skeletal_JATs_DNA.randFloatForBookKeeping_FinalEnergy.value(tempSrcJnt_ID);
//                alienPtr->alienDNA->skeletonDNA->skeletal_JATs_DNA.randFloatForBookKeeping_FinalEnergy.insert(tempJntList[i]->BP_ID, 10+tempFinEngy);
//            }

            if (JAT_AnglePt.x() == -1)
            {
                if (sourceAlien->alienDNA->skeletonDNA->skeletal_JATs_DNA.randIntForBookKeeping_AngleTrend.contains(tempSrcJnt_ID)) // ++
                {
                    int tempAngTrd = sourceAlien->alienDNA->skeletonDNA->skeletal_JATs_DNA.randIntForBookKeeping_AngleTrend.value(tempSrcJnt_ID);
        //            if (isMirror)
        //                tempAngTrd *= -1;
                    alienPtr->alienDNA->skeletonDNA->skeletal_JATs_DNA.randIntForBookKeeping_AngleTrend.insert(tempJntList[i]->BP_ID, tempAngTrd);
                    qDebug() << "source joint angle trend : " << tempAngTrd;
                }

                int tempQuatEulAngForFirChd = sourceAlien->alienDNA->skeletonDNA->skeletal_JATs_DNA.randIntForBookKeeping_quaterEulerAngleSelectorForFirstChild.value(tempSrcJnt_ID); // ++
                alienPtr->alienDNA->skeletonDNA->skeletal_JATs_DNA.randIntForBookKeeping_quaterEulerAngleSelectorForFirstChild.insert(tempJntList[i]->BP_ID, tempQuatEulAngForFirChd);
                qDebug() << "source joint Euler Angle For First Jnt : " << tempQuatEulAngForFirChd;

                QVector3D tempQuatEulAngForNotFirChd = sourceAlien->alienDNA->skeletonDNA->skeletal_JATs_DNA.randQVector3DForBookKeeping_quaterEulerAngleSelectorForNotFirstChild.value(tempSrcJnt_ID); // ++
                alienPtr->alienDNA->skeletonDNA->skeletal_JATs_DNA.randQVector3DForBookKeeping_quaterEulerAngleSelectorForNotFirstChild.insert(tempJntList[i]->BP_ID, tempQuatEulAngForNotFirChd);
                qDebug() << "source joint Euler Angle NOT For First Jnt : " << tempQuatEulAngForNotFirChd;
            }
        }


        QPolygonF tempPolyF; tempPolyF.clear();
        if (alienPtr->alienDNA->skeletonDNA->skeletal_JATs_DNA.BP_JAT_rootJoint_DNA.contains(newCloneBP_Ptr->BP_Type)) //++
        {
            tempPolyF = alienPtr->alienDNA->skeletonDNA->skeletal_JATs_DNA.BP_JAT_rootJoint_DNA.value(newCloneBP_Ptr->BP_Type);
        }
        tempPolyF.append(sourceAlien->alienDNA->skeletonDNA->skeletal_JATs_DNA.BP_to_JAT_rootJoint_Map_DNA.value(sourceBP_Ptr->BP_ID)); //++

        // For modification of JATs DNA for this BP, change the last two QPointF accordingly
        // ... This can be sent as an input
        if (JAT_EnergyPt.x() != -1)
            tempPolyF[tempPolyF.length()-2].setX(JAT_EnergyPt.x());
        if (JAT_EnergyPt.y() != -1)
            tempPolyF[tempPolyF.length()-2].setY(JAT_EnergyPt.y());
        if (JAT_AnglePt.x() != -1)
            tempPolyF[tempPolyF.length()-1].setX(JAT_AnglePt.x());
        if (JAT_AnglePt.y() != -1)
            tempPolyF[tempPolyF.length()-1].setY(JAT_AnglePt.y());

        qDebug() << "BP_to_JAT_rootJoint_Map_DNA for source BP : " << sourceAlien->alienDNA->skeletonDNA->skeletal_JATs_DNA.BP_to_JAT_rootJoint_Map_DNA.value(sourceBP_Ptr->BP_ID);

        alienPtr->alienDNA->skeletonDNA->skeletal_JATs_DNA.BP_JAT_rootJoint_DNA.insert(newCloneBP_Ptr->BP_Type, tempPolyF);

        QMap<int, QPolygonF> tempCopy_BP_JAT_rootJoint_DNA; tempCopy_BP_JAT_rootJoint_DNA.clear();

        QPolygonF polyF; polyF.clear();
        polyF << tempPolyF.at(tempPolyF.length()-2) << tempPolyF.at(tempPolyF.length()-1);
        tempCopy_BP_JAT_rootJoint_DNA.insert(newCloneBP_Ptr->BP_Type, polyF);

        alienPtr->alienDNA->skeletonDNA->skeletal_JATs_DNA.BP_to_JAT_rootJoint_Map_DNA.insert(newCloneBP_Ptr->BP_ID, polyF);

        // tempCopy_BP_JAT_rootJoint_DNA.insert(newCloneBP_Ptr->BP_Type,
        //        sourceAlien->alienDNA->skeletonDNA->skeletal_JATs_DNA.BP_to_JAT_rootJoint_Map_DNA.value(sourceBP_Ptr->BP_ID));

        compute_JAT_AND_VecLocCplForEachJnt_ForBP(alienPtr, newCloneBP_Ptr->BP_ID, tempCopy_BP_JAT_rootJoint_DNA,
                                                  tempParentJntCompletionCheckList, tempChildJntPositionCheckList,
                                                  0.0);
        // model all the joints of this BP
        modelAllAlienJoints_TypeGuided(alienPtr, tempJntList);

        // if (sourceAlien != alienPtr)
        // {
        //     for (int i=0; i<tempJntList.length(); i++)
        //     {
        //         int tempSrcJnt_ID = alienPtr->alienSkeleton.JntToJntMappings_ForBookKeeping.key(tempJntList[i]->BP_ID); //++
        //         Joint *srcJnt = getJointFromID(sourceAlien, tempSrcJnt_ID);

        //         if (srcJnt)
        //         {
        //             // move the new Joint to the same location as the tempSrcJnt
        //             // GeometryAlgorithms::translateBP(tempJntList[i],
        //             //                                 srcJnt->getPivotPoint().x() - tempJntList[i]->getPivotPoint().x(),
        //             //                                 srcJnt->getPivotPoint().y() - tempJntList[i]->getPivotPoint().y(),
        //             //                                 srcJnt->getPivotPoint().z() - tempJntList[i]->getPivotPoint().z());

        //             Cell *newClonedCell = new Cell();
        //             srcJnt->cellList.first()->cloneCell(newClonedCell);
        //             tempJntList[i]->cellList.prepend(newClonedCell);
        //             tempJntList[i]->cellList.removeLast();
        //         }
        //         else
        //             exit(0);
        //     }
        // }

        // Model all the bones
        // TODO(ismail): make a method for extracting a sub map of Jnt2JntRelations_TypeGuided given a set of BP_IDs
        // ... COuld be handy for some purposes
        QMap<int, QPolygon*> tempJnt2JntRelations;
        QPolygon *tempNewPolyPtr = new QPolygon(*(alienPtr->alienSkeleton.Jnt2JntRelations_TypeGuided.value(newCloneBP_Ptr->BP_ID)));
        tempJnt2JntRelations.insert(newCloneBP_Ptr->BP_ID, tempNewPolyPtr);
        // model the bones of this BP
        modelAllAlienBones_TypeGuided(alienPtr, tempJnt2JntRelations);

        // clone its bulkiness DNA
        // TODO(ismail): Make a method cloneBulkinessDNA_FromSrcToDestBP() which will copy one BP's bulkiness DNA
        // ... to another
        QPolygonF tempBulkPoly = sourceAlien->alienDNA->skeletonDNA->Skeletal_BP_Bulkiness_DNA_ForEachBP.value(sourceBP_Ptr->BP_ID); // ++
        alienPtr->alienDNA->skeletonDNA->Skeletal_BP_Bulkiness_DNA_ForEachBP.insert(newCloneBP_Ptr->BP_ID, tempBulkPoly);

        // bulk it up
        QVector<int> tempBP_ListToBulk; tempBP_ListToBulk.clear();
        tempBP_ListToBulk << newCloneBP_Ptr->BP_ID;
        bulkUpBodyPart(alienPtr,tempBP_ListToBulk);

        // Now densify the cloned BP by cloning the densification DNA
        QVector<int> toBeDensifiedBPsNewAlien; toBeDensifiedBPsNewAlien.clear(); //++
        if (cloneDensificationDNA(sourceAlien, alienPtr, sourceBP_Ptr, newCloneBP_Ptr))
        {
            toBeDensifiedBPsNewAlien << newCloneBP_Ptr->BP_ID;
            densifyAllBodyParts(alienPtr, toBeDensifiedBPsNewAlien);
        }

        // Now bulgify the cloned BP by cloning the bulgification DNA
        QVector<int> toBeBulgifiedBPsNewAlien; toBeBulgifiedBPsNewAlien.clear(); //++
        if (cloneBP_BulgificationDNA(sourceAlien, alienPtr, sourceBP_Ptr, newCloneBP_Ptr))
        {
            if (!incDecButtonPressed) // JUGAAR!!
                bulgifyBodyPart(alienPtr, newCloneBP_Ptr);
        }

        // ** Extremities cloning and transfering ** Also need to transfer the extremities to the cloned BP using cloning
        for (int extID : sourceBP_Ptr->subExtremityBPsIDs)
        {
            ExtremityBP *tempSubExtBP = getExtremityBP_FromID(sourceAlien, extID);
            if (tempSubExtBP)
            {
                tempSubExtBP = transferExtBP_FromSourceAlienToDestAlienWithCloning(sourceAlien, alienPtr, tempSubExtBP, newCloneBP_Ptr->BP_ID);
            }
        }

        // TODO(ismail): Scale the cloned BP by taking into account the scaling of the source BP as well

        // Generate the cloned BP at the same location as the source BP, and also rotate it accordingly

        toBeDensifiedBPsNewAlien.clear();
        tempCopy_BP_JAT_rootJoint_DNA.clear();
        tempParentJntCompletionCheckList.clear();
        tempChildJntPositionCheckList.clear();
        tempJnt2JntRelations.clear();
        tempJntList.clear();
        tempBP_List.clear();
        tempPoly_2.clear();
        tempPoly.clear();
    }

    return newCloneBP_Ptr;
}

void AlienModeller::attachAllClonedChildBPsToTheirClonedParentBPs_afterAlienIsCloned(Alien *clonedAlienPtr, Alien *origAlienPtr,
                                                                                     QVector<BodyPart *> &allClonedBPsPtrs, BodyPart *origParentBP)
{
    // // make sure that this alien is cloned from another one
    // if (clonedAlienPtr->alienID != clonedAlienPtr->clonedFrom)
    // {
    //     // Get the original alien from which this alien is cloned from
    //     Alien *origAlien = getAlienFromID(clonedAlienPtr->clonedFrom);
    //     // find the root BP of the original Alien and the cloned Alien
    //     BodyPart *rootBPofClonedAlien = nullptr;
    //     for (BodyPart *bp : allClonedBPsPtrs)
    //         if (bp->cloneFromAlienAndBP_Pair.second == origAlien->rootBP->BP_ID)
    //         {
    //             rootBPofClonedAlien = bp;
    //             break;
    //         }

    //     if (rootBPofClonedAlien)
    //     {
    //         // Now go through the BP heirarchy of the original alien and attach the cloned BPs (in the Cloned Alien)

    //     }
    // }

    // first find the parent of all the BPs that cloned the allClonedBPsPtrs

    // Get the cloned Parent BP that was cloned from parentBP
    BodyPart *clonedParentBP = nullptr;
    for (BodyPart *bp : allClonedBPsPtrs)
        if (bp->cloneFromAlienAndBP_Pair.second == origParentBP->BP_ID)
        {
            clonedParentBP = bp; // we have found the parent that cloned this BP
            break;
        }

    // We need all the Joints and Bones in order for this parentBP, to figure out the joint/bone a subBP is attached to
    QVector<int> jntsInOrderOrigParentBP; jntsInOrderOrigParentBP.clear();
    convertJnt2JntPolyToJointsLineageList(origAlienPtr->alienSkeleton.Jnt2JntRelations_TypeGuided.value(origParentBP->BP_ID), jntsInOrderOrigParentBP);
    QVector<Bone*> origPparentBP_BonesInOrder; origPparentBP_BonesInOrder.clear();
    getAllBP_BonesInOrder(origAlienPtr, origParentBP, origPparentBP_BonesInOrder);

    if (clonedParentBP)
    {
        // get all the joints and bones of this clonedParentBP in order. We need these in order to attach the clonedSubBP to the right Jnt or Bone
        QVector<int> jntsInOrderClonedParentBP; jntsInOrderClonedParentBP.clear();
        convertJnt2JntPolyToJointsLineageList(clonedAlienPtr->alienSkeleton.Jnt2JntRelations_TypeGuided.value(clonedParentBP->BP_ID), jntsInOrderClonedParentBP);
        QVector<Bone*> clonedParentBP_BonesInOrder; clonedParentBP_BonesInOrder.clear();
        getAllBP_BonesInOrder(clonedAlienPtr, clonedParentBP, clonedParentBP_BonesInOrder);

        if (jntsInOrderClonedParentBP.length() == jntsInOrderOrigParentBP.length()
                &&
            clonedParentBP_BonesInOrder.length() == origPparentBP_BonesInOrder.length())
        {
            QVector<int> SubBP_TypesToIgnore; SubBP_TypesToIgnore.clear();
            SubBP_TypesToIgnore     << AlienModeller::BodyPartsEnum::JOINT
                                    << AlienModeller::BodyPartsEnum::BONE
                                    << AlienModeller::BodyPartsEnum::ExtremBP_Carpal
                                    << AlienModeller::BodyPartsEnum::ExtremBP_Digit;
            for (BodyPart *subBP : origParentBP->subBodyParts)
            {
                if (!SubBP_TypesToIgnore.contains(subBP->BP_Type)
                        &&
                    !subBP->isSymmetric)
                {
                    // Now get the cloned BP that was cloned from subBP.
                    BodyPart *clonedSubBP = nullptr;
                    for (BodyPart *bp : allClonedBPsPtrs)
                        if (bp->cloneFromAlienAndBP_Pair.second == subBP->BP_ID)
                        {
                            clonedSubBP = bp;
                            break;
                        }

                    if (clonedSubBP)
                    {
                        // // Also get the attachment loc and vector from the BP_AttachmentToOtherBP_withAttachPositionAndVector_DNA
                        // updateBP_AttachmentToOtherBP_withAttachPositionAndVector_DNA(origAlienPtr, -1, origParentBP->BP_ID,
                        //                                                              subBP->BP_ID, subBP->getPivotPoint(), subBP->getPivotVector());
                        QPair<QVector3D,QVector3D> attLocAndVec = QPair<QVector3D,QVector3D>(subBP->getPivotPoint(), subBP->getPivotVector());
                        // QPair<QVector3D,QVector3D> attLocAndVec = QPair<QVector3D,QVector3D>(subBP->getPivotPoint(), endJntsPos.second-endJntsPos.first);

                        // Now attach the clonedSubBP to the clonedParentBP using the attLocAndVec
                        // Also do the following:
                        //  - make clonedSubBP subBP of clonedParentBP
                        //  - update the Jnts and Bones allAttachedBP_IDs
                        //  - insert the entry for the clones in the BP_AttachmentToOtherBP_withAttachPositionAndVector_DNA
                        // update the BP_AttachmentToOtherBP_withAttachPositionAndVector_DNA for clones
                        this->updateBP_AttachmentToOtherBP_withAttachPositionAndVector_DNA(clonedAlienPtr, -1, clonedParentBP->BP_ID, clonedSubBP->BP_ID,
                                                                                           attLocAndVec.first, attLocAndVec.second);

                        GeometryAlgorithms::rotateBodyPartTowardsVecDir(*(clonedSubBP), attLocAndVec.second);
                        GeometryAlgorithms::translateBP(clonedSubBP,
                                                        attLocAndVec.first.x() - clonedSubBP->getPivotPoint().x(),
                                                        attLocAndVec.first.y() - clonedSubBP->getPivotPoint().y(),
                                                        attLocAndVec.first.z() - clonedSubBP->getPivotPoint().z());

                        GeometryAlgorithms::updateBP_CellPrimitiveGeometry(clonedSubBP);

                        // *VERY IMPORTANT* - Include the child BP (source BP) as a sub BP of this source BP (dest BP)
                        // First check if this is not already a subBP of the parent BP
                        if (!clonedParentBP->subBodyParts.contains(clonedSubBP))
                            clonedParentBP->subBodyParts.append(clonedSubBP);

                        // Also add the BP ID to the joint or bone it is attached to in the origParentBP
                        bool jntSuccess=false;
                        for (int j=0; j<jntsInOrderOrigParentBP.length(); j++)
                        {
                            if (dynamic_cast<Joint*>(getBodyPartFromID(origAlienPtr, jntsInOrderOrigParentBP[j]))->allAttachedBP_IDs.contains(subBP->BP_ID))
                            {
                                // we have found the index of the joint so thats the joint index we'll be using in the cloneParentBP
                                if (!dynamic_cast<Joint*>(getBodyPartFromID(clonedAlienPtr, jntsInOrderClonedParentBP[j]))->allAttachedBP_IDs.contains(clonedSubBP->BP_ID))
                                    dynamic_cast<Joint*>(getBodyPartFromID(clonedAlienPtr, jntsInOrderClonedParentBP[j]))->allAttachedBP_IDs.append(clonedSubBP->BP_ID);
                                jntSuccess=true;
                                break;
                            }
                        }
                        if (!jntSuccess) // check and attach it to the bone
                        {
                            for (int b=0; b<origPparentBP_BonesInOrder.length(); b++)
                            {
                                if (origPparentBP_BonesInOrder[b]->allAttachedBP_IDs.contains(subBP->BP_ID))
                                {
                                    // we have found the index of the joint so thats the joint index we'll be using in the cloneParentBP
                                    if (!clonedParentBP_BonesInOrder[b]->allAttachedBP_IDs.contains(clonedSubBP->BP_ID))
                                        clonedParentBP_BonesInOrder[b]->allAttachedBP_IDs.append(clonedSubBP->BP_ID);
                                    break;
                                }
                            }
                        }

                        // Finally call it recursively for this subBP
                        attachAllClonedChildBPsToTheirClonedParentBPs_afterAlienIsCloned(clonedAlienPtr,origAlienPtr,allClonedBPsPtrs,subBP);

                        // allClonedBPsPtrs.removeOne(clonedSubBP); // once all its subBPs have been attached remove it - just to improve tiny performance
                    }
                    else
                    {
                        qDebug() << "There is something wrong. The clonedSubBP is not found in the allClonedBPsPtrs";
                        exit(0);
                    }
                }
            }
        }
        else
        {
            qDebug() << "There is something wrong. jntsInOrderClonedParentBP/origPparentBP_BonesInOrder is not equal "
                        "size to jntsInOrderOrigParentBP/clonedParentBP_BonesInOrder respectively";
            exit(0);
        }
    }
    else
    {
        qDebug() << "There is something wrong. The clonedParentBP is not found in the allClonedBPsPtrs";
        exit(0);
    }

}

// void AlienModeller::visitBPsRecursivelyForClonedBPsAttachment(Alien *alienPtr, BodyPart *parentBP)
// {
//     for (BodyPart *subBP : parentBP->subBodyParts)
//     {
//         if (subBP->BP_Type != AlienModeller::BodyPartsEnum::JOINT
//                 &&
//             subBP->BP_Type != AlienModeller::BodyPartsEnum::BONE)
//         {

//         }
//     }
// }

void AlienModeller::createSmoothingBackupCloneBP(Alien *alienPtr, BodyPart *sourceBP_Ptr)
{
    // first clone the BP and store it in smoothingBackupCloneBPs
    //      Clone BP
    //      remove the entries from the numOfSkeletalBodyParts_DNA and BP_AngleArrangementsFromBackBone_DNA
    //      ... because these were added during the cloning (in cloneSkeletalBP_FromAnotherAlien_viaDNA)
    //      Also remove its pointer from bodyParts and cloneBPs

    if (!alienPtr->smoothingBackupCloneBPs.contains(sourceBP_Ptr->BP_ID))
    {
        BodyPart *tempCloneBP_Ptr = cloneSkeletalBP_FromAnotherAlien_viaDNA(alienPtr, alienPtr, sourceBP_Ptr);
        tempCloneBP_Ptr->clonedFrom = sourceBP_Ptr->BP_ID;
        tempCloneBP_Ptr->cloneFromAlienAndBP_Pair = QPair<int,int>(alienPtr->alienID,sourceBP_Ptr->BP_ID);

        int tempNumOfBPs = alienPtr->alienDNA->skeletonDNA->numOfSkeletalBodyParts_DNA.value(sourceBP_Ptr->BP_Type);
        alienPtr->alienDNA->skeletonDNA->numOfSkeletalBodyParts_DNA.insert(sourceBP_Ptr->BP_Type, --tempNumOfBPs);

        if (sourceBP_Ptr->BP_Type != AlienModeller::BodyPartsEnum::BACK_BONE)
        {
            if (!alienPtr->alienDNA->skeletonDNA->BP_AngleArrangementsFromBackBone_DNA.value(sourceBP_Ptr->BP_Type).isEmpty())
                alienPtr->alienDNA->skeletonDNA->BP_AngleArrangementsFromBackBone_DNA[sourceBP_Ptr->BP_Type].removeLast();
        }

        // add this tempCloneBP_Ptr to smoothingBackupCloneBPs list in the alien
        alienPtr->smoothingBackupCloneBPs.insert(sourceBP_Ptr->BP_ID, tempCloneBP_Ptr);
        if (alienPtr->bodyParts.removeOne(tempCloneBP_Ptr))
            if (alienPtr->cloneBPs.removeOne(tempCloneBP_Ptr))
                qDebug() << "New clone created and appeneded to the smoothingBackupCloneBPs list."
                         << "Also removed from the bodyParts list and cloneBPs list";

        // TODO(Ismail): Also, perhaps we need to remove the joints entries added to the JAT DSs when cloning
        // ... because this clone is not part of the main BodyPart List, but is only for backup purposes
    }
}

BodyPart *AlienModeller::createSeperateCloneBP(Alien *alienPtr, BodyPart *sourceBP_Ptr)
{
    // first clone the BP and store it in seperateCloneBPs
    //      Clone BP
    //      remove the entries from the numOfSkeletalBodyParts_DNA and BP_AngleArrangementsFromBackBone_DNA
    //      ... because these were added during the cloning (in cloneSkeletalBP_FromAnotherAlien_viaDNA)
    //      Also remove its pointer from bodyParts and cloneBPs

    BodyPart *tempCloneBP_Ptr = cloneSkeletalBP_FromAnotherAlien_viaDNA(alienPtr, alienPtr, sourceBP_Ptr);
    tempCloneBP_Ptr->clonedFrom = sourceBP_Ptr->BP_ID;
    tempCloneBP_Ptr->cloneFromAlienAndBP_Pair = QPair<int,int>(alienPtr->alienID,sourceBP_Ptr->BP_ID);

    int tempNumOfBPs = alienPtr->alienDNA->skeletonDNA->numOfSkeletalBodyParts_DNA.value(sourceBP_Ptr->BP_Type);
    alienPtr->alienDNA->skeletonDNA->numOfSkeletalBodyParts_DNA.insert(sourceBP_Ptr->BP_Type, --tempNumOfBPs);

    if (sourceBP_Ptr->BP_Type != AlienModeller::BodyPartsEnum::BACK_BONE)
    {
        if (!alienPtr->alienDNA->skeletonDNA->BP_AngleArrangementsFromBackBone_DNA.value(sourceBP_Ptr->BP_Type).isEmpty())
            alienPtr->alienDNA->skeletonDNA->BP_AngleArrangementsFromBackBone_DNA[sourceBP_Ptr->BP_Type].removeLast();
    }

    // add this tempCloneBP_Ptr to seperateCloneBPs list in the alien
    alienPtr->seperateCloneBPs.append(tempCloneBP_Ptr);
    if (alienPtr->bodyParts.removeOne(tempCloneBP_Ptr))
        if (alienPtr->cloneBPs.removeOne(tempCloneBP_Ptr))
            qDebug() << "New clone created and appeneded to the seperateCloneBPs list."
                     << "Also removed from the bodyParts list and cloneBPs list";

    // TODO(Ismail): Also, perhaps we need to remove the joints entries added to the JAT DSs when cloning
    // ... because this clone is not part of the main BodyPart List, but is only for backup purposes.
    // ... Otherwise the JATs DSs will get really huge. Perhaps need to think about a solution

    return tempCloneBP_Ptr;
}

void AlienModeller::addBP_ToItsRespectiveList(Alien *inputAlien, BodyPart *BP_Ptr)
{
    // Now add all the body parts of this Alien to their respective lists and also to the BodyParts list
    if (BP_Ptr->BP_Type == AlienModeller::BodyPartsEnum::BACK_BONE)
        inputAlien->alienBackBones.append(dynamic_cast<BackBone*>(BP_Ptr));
    else if (BP_Ptr->BP_Type == AlienModeller::BodyPartsEnum::LOWER_LIMB)
        inputAlien->alienLowerLimbs.append(dynamic_cast<LowerLimb*>(BP_Ptr));
    else if (BP_Ptr->BP_Type == AlienModeller::BodyPartsEnum::UPPER_LIMB)
        inputAlien->alienUpperLimbs.append(dynamic_cast<UpperLimb*>(BP_Ptr));
//    else if (BP_Ptr->BP_Type == AlienModeller::BodyPartsEnum::TAIL)
//        inputAlien->alienTails.append(dynamic_cast<Tail*>(BP_Ptr));
//    else if (BP_Ptr->BP_Type == AlienModeller::BodyPartsEnum::HEAD)
//        inputAlien->alienHeads.append(dynamic_cast<Head*>(BP_Ptr));
    else if (BP_Ptr->BP_Type == AlienModeller::BodyPartsEnum::JOINT)
        inputAlien->alienJoints.append(dynamic_cast<Joint*>(BP_Ptr));
    else if (BP_Ptr->BP_Type == AlienModeller::BodyPartsEnum::BONE)
        inputAlien->alienBones.append(dynamic_cast<Bone*>(BP_Ptr));
}

void AlienModeller::addAllSubBPs_ToAlienRecursive(Alien *inputAlien, BodyPart *BP_Ptr)
{
    inputAlien->bodyParts.append(BP_Ptr);
    addBP_ToItsRespectiveList(inputAlien, BP_Ptr);
    for (int i=0; i<BP_Ptr->subBodyParts.length(); i++)
    {
        addAllSubBPs_ToAlienRecursive(inputAlien, BP_Ptr->subBodyParts[i]);
    }
}

void AlienModeller::cloneJnt2JntRelations_TypeGuided_DS(Alien *sourceAlien, Alien *clonedAlien)
{
    clonedAlien->alienSkeleton.Jnt2JntRelations_TypeGuided.clear();
    QMapIterator<int, QPolygon*> j2jIter(sourceAlien->alienSkeleton.Jnt2JntRelations_TypeGuided);
    while (j2jIter.hasNext())
    {
        j2jIter.next();
        QPolygon *tempPoly = new QPolygon(*(j2jIter.value()));
        clonedAlien->alienSkeleton.Jnt2JntRelations_TypeGuided.insert(j2jIter.key(), tempPoly);
    }
    qDebug() << "J2JRel cloned successfully";
}

void AlienModeller::getAllBP_Joints(BodyPart *bpPtr, QVector<Joint *> &BP_JntsList)
{
    if (bpPtr->BP_Type != AlienModeller::BodyPartsEnum::BONE
            &&
        bpPtr->BP_Type != AlienModeller::BodyPartsEnum::JOINT)
    {
        for (int i=0; i<bpPtr->subBodyParts.length(); i++)
        {
            if (bpPtr->subBodyParts[i]->BP_Type == AlienModeller::BodyPartsEnum::JOINT)
                BP_JntsList.append(dynamic_cast<Joint*>(bpPtr->subBodyParts[i]));
        }
    }
}

QVector<int> AlienModeller::getAllJntIDsOfBP(BodyPart *bpPtr)
{
    QVector<int> BP_JntsIDsList; BP_JntsIDsList.clear();
    if (bpPtr->BP_Type != AlienModeller::BodyPartsEnum::BONE
            &&
        bpPtr->BP_Type != AlienModeller::BodyPartsEnum::JOINT)
    {
        for (int i=0; i<bpPtr->subBodyParts.length(); i++)
        {
            if (bpPtr->subBodyParts[i]->BP_Type == AlienModeller::BodyPartsEnum::JOINT)
                BP_JntsIDsList << bpPtr->subBodyParts[i]->BP_ID;
        }
    }

    return BP_JntsIDsList;
}

void AlienModeller::getAllBP_Bones(BodyPart *bpPtr, QVector<Bone *> &BP_BonesList)
{
    if (bpPtr)
    {
        if (bpPtr->BP_Type != AlienModeller::BodyPartsEnum::BONE
                &&
            bpPtr->BP_Type != AlienModeller::BodyPartsEnum::JOINT)
        {
            for (int i=0; i<bpPtr->subBodyParts.length(); i++)
            {
                if (bpPtr->subBodyParts[i]->BP_Type == AlienModeller::BodyPartsEnum::BONE)
                    BP_BonesList.append(dynamic_cast<Bone*>(bpPtr->subBodyParts[i]));
            }
        }
    }
    else
    {
        qDebug() << "BP doesnt exist - method getAllBP_Bones";
        exit(0);
    }
}

int AlienModeller::getNumOfJntsForBP(BodyPart *bpPtr)
{
    int jntCnt = 0;
    if (bpPtr)
    {
        for (int i=0; i<bpPtr->subBodyParts.length(); i++)
        {
            if (bpPtr->subBodyParts[i]->BP_Type == AlienModeller::BodyPartsEnum::JOINT)
                jntCnt++;
        }
    }
    return jntCnt;
}

bool AlienModeller::getAllBP_BonesInOrder(Alien *alienPtr, BodyPart *bpPtr, QVector<Bone *> &BP_BonesList)
{
    if (bpPtr)
    {
        if (alienPtr->alienSkeleton.Jnt2JntRelations_TypeGuided.contains(bpPtr->BP_ID))
        {
            QVector<Bone*> tempBoneList; tempBoneList.clear();
            getAllBP_Bones(bpPtr, tempBoneList);
            qDebug() << tempBoneList.length() << " bones extracted";

            QPolygon *polyPtr = alienPtr->alienSkeleton.Jnt2JntRelations_TypeGuided.value(bpPtr->BP_ID);
            // TODO(ismail): Put this inside a utility method to be used by other methods
            for (int i=0; i<polyPtr->length(); i++)
            {
                for (int j=0; j<tempBoneList.length(); j++)
                {
                    if (polyPtr->at(i).x() == tempBoneList[j]->endJoints.first  &&  polyPtr->at(i).y() == tempBoneList[j]->endJoints.second)
                    {
                        BP_BonesList.append(tempBoneList[j]);
                        break;
                    }
                }
            }
            qDebug() << BP_BonesList.length() << " bones arranged in order";
            return true;
        }
    }
    else
        qDebug() << "BP doesnt exist - method getAllBP_BonesInOrder";
    return false;
}

void AlienModeller::applyRelativeScalingsToBodyParts(Alien *alienPtr, QMap<int, quint8> BP_ComparisonList)
{
    /* +++++++ ALGORITHM ++++++++
     *
     * compute the Total Length of the back bone of the Alien
     * compute the Total Length of any given BP of the Alien
     * scale the BP so that its Total Length is equal to that of BB
     * now scale the BP according to its scaling amount. This scaling amount can be taken from the DNA
    */

    float totalLengthRefBP=0,totalLengthScaledBP=0;
    BodyPart *refBP_Ptr=nullptr, *scaledBP_Ptr=nullptr;
    QMapIterator<int, quint8> i(BP_ComparisonList);
    while (i.hasNext()) // first compute the Total Length of the reference BP
    {
        i.next();

        // Check the value whether these BP need to be scaled or not
        if (i.value() != 0)
        {
            if (i.value() == 2) // This is the reference BP
            {
                // compute the Total Length of this BP
                for (int j=0; j<alienPtr->bodyParts.length(); j++)
                {
                    if (i.key() == alienPtr->bodyParts[j]->BP_Type)
                    {
                        refBP_Ptr = alienPtr->bodyParts[j];
                        totalLengthRefBP = getTotalLengthOfSkeletalBP(alienPtr, refBP_Ptr);
                        break;
                    }
                }
            }
        }
        i.toBack();
    }

    i.toFront();
    while (i.hasNext())
    {
        i.next();

        if (i.value() == 1) // This is the BP to be scaled
        {
            // compute the Total Length of this BP
            for (int j=0; j<alienPtr->bodyParts.length(); j++)
            {
                if (i.key() == alienPtr->bodyParts[j]->BP_Type)
                {
                    scaledBP_Ptr = alienPtr->bodyParts[j];
                    totalLengthScaledBP = getTotalLengthOfSkeletalBP(alienPtr, scaledBP_Ptr);
                    break;
                }
            }
        }
        i.toBack();
    }

    // scale all the BPs according to the BP_SizeComparedToBackBone_DNA
    // ... TO BE CONTINUED

}

float AlienModeller::getTotalLengthOfSkeletalBP(Alien *alien, BodyPart *BP_Ptr)
{
    QVector<Bone *> BP_BonesList; BP_BonesList.resize(0);
    if (getAllBP_BonesInOrder(alien, BP_Ptr, BP_BonesList))
    {
        float totalLength=0;
        // Add all the lengths of the bone vectors
        for (Bone *bn : BP_BonesList)
        {
            // find the vector from mid point of back face to mid point of front face
            QVector3D tempBoneVec = getJointFromID(alien, bn->endJoints.second)->getPivotPoint() - getJointFromID(alien, bn->endJoints.first)->getPivotPoint();

            totalLength += tempBoneVec.length();
        }

        return totalLength;
    }
    return 0;
}

float AlienModeller::sortBPsAccordingToTotalLengths(Alien *alien, QVector<BodyPart *> BP_PtrList, QMap<int, float> &sortedTotLengthsMap)
{
    // sort these
    for (BodyPart *bp : BP_PtrList)
        sortedTotLengthsMap.insert(bp->BP_ID, getTotalLengthOfSkeletalBP(alien, bp));

    QList<float> allLengths = sortedTotLengthsMap.values();
    // Find the average total length
    float sum = 0;
    for (float l : allLengths)
        sum += l;

    return sum/BP_PtrList.length(); // return the average total length
}

void AlienModeller::scaleAllBPsOfGivenType(Alien *alien, int BP_Type)
{
    QPolygon tempPoly = alien->alienDNA->skeletonDNA->BP_SizeComparedToBackBone_DNA.value(BP_Type);

    for (int j=0, k=0; j<alien->bodyParts.length(); j++)
    {
        if (BP_Type == alien->bodyParts[j]->BP_Type)
        {
            // Now scale this BP according to the BP_SizeComparedToBackBone_DNA
            GeometryAlgorithms::scaleBP(alien->bodyParts[j], static_cast<float>(tempPoly[k].y())/100.0, static_cast<float>(tempPoly[k].y())/100.0, static_cast<float>(tempPoly[k].y())/100.0);
            k++;
        }
    }
}

void AlienModeller::incDecBP_Size(Alien *alien, int BP_ID, QVector3D scalingFactorVec)
{
    BodyPart *bpPtr = getBodyPartFromID(alien, BP_ID);
    GeometryAlgorithms::scaleBP(bpPtr, scalingFactorVec.x(), scalingFactorVec.y(), scalingFactorVec.z());
}

void AlienModeller::updateJntToJntMappingList_forBookKeeping(Alien *alienPtr, QPolygon *sourcePoly, QPolygon *destPoly)
{
    qDebug() << "Source poly = " << *sourcePoly;
    qDebug() << "Destination poly = " << *destPoly;

    QVector<int> tempJntChecklist; tempJntChecklist.clear();
    for (int i=0; i<sourcePoly->length(); i++)
    {
        // TODO: Do we really need the tempJntChecklist?
        int thisVal = sourcePoly->at(i).x();
        if (!tempJntChecklist.contains(thisVal))
        {
            tempJntChecklist.append(thisVal);
            alienPtr->alienSkeleton.JntToJntMappings_ForBookKeeping.insert(sourcePoly->at(i).x(),destPoly->at(i).x());
        }
        thisVal = sourcePoly->at(i).y();
        if (!tempJntChecklist.contains(thisVal))
        {
            tempJntChecklist.append(thisVal);
            alienPtr->alienSkeleton.JntToJntMappings_ForBookKeeping.insert(sourcePoly->at(i).y(),destPoly->at(i).y());
        }
    }
    tempJntChecklist.clear();
}

void AlienModeller::createExtremityBP_DNA_FromRS(Alien *alienPtr, QVector<int> extremIDs)
{
    for (int id : extremIDs)
    {
        addNewEntryForExtremityBP_DNA(alienPtr, id);
    }

}

void AlienModeller::addNewEntryForExtremityBP_DNA(Alien *alienPtr, int extremBP_ID)
{
    if (!alienPtr->alienDNA->skeletonDNA->extremityBP_DNA.contains(extremBP_ID))
    {
        QPolygon tempPoly; tempPoly.clear();
        int numOfDigits = GeometryAlgorithms::getRandomInteger(skeletonRuleSet->extremity_RS.at(0).x(),skeletonRuleSet->extremity_RS.at(0).y());
        int numOfCarpals = GeometryAlgorithms::getRandomInteger(skeletonRuleSet->extremity_RS.at(1).x(),skeletonRuleSet->extremity_RS.at(1).y());

        int digitAngle = GeometryAlgorithms::getRandomInteger(skeletonRuleSet->extremity_RS.at(2).x(),skeletonRuleSet->extremity_RS.at(2).y());
        int carpalDist = GeometryAlgorithms::getRandomInteger(skeletonRuleSet->extremity_RS.at(3).x(),skeletonRuleSet->extremity_RS.at(3).y());

        int relativeSize = GeometryAlgorithms::getRandomInteger(skeletonRuleSet->extremity_RS.at(4).x(),skeletonRuleSet->extremity_RS.at(4).y());
        int sizeIncOrDec = GeometryAlgorithms::getRandomInteger(skeletonRuleSet->extremity_RS.at(5).x(),skeletonRuleSet->extremity_RS.at(5).y());

        int alignTrend = GeometryAlgorithms::getRandomInteger(skeletonRuleSet->extremity_RS.at(6).x(),skeletonRuleSet->extremity_RS.at(6).y());

        tempPoly << QPoint(numOfDigits,numOfCarpals) << QPoint(digitAngle,carpalDist) << QPoint(relativeSize,sizeIncOrDec) << QPoint(alignTrend,-1);

        alienPtr->alienDNA->skeletonDNA->extremityBP_DNA.insert(extremBP_ID, tempPoly);
    }

    // qDebug() << "Extremity BP DNA : " << alienPtr->alienDNA->skeletonDNA->extremityBP_DNA;
}

ExtremityBP* AlienModeller::modelNewExtermityBP(Alien *alienPtr, int parentBP_ID)
{
    qDebug() << "about to model Extremity BP";

    // Instantiate a new extremity BP and assign an ID to it
    ExtremityBP *exBP_Ptr = instantiateNewExtremBP(alienPtr); // this method does not require DNA

    // Now add a new entry to the extremity DNA
    addNewEntryForExtremityBP_DNA(alienPtr, exBP_Ptr->BP_ID); // For cloning an ExtBP, simply copy the DNA

    // Now instantiate the base and its carpals and digits
    ExtremityBP_Base *extBasePtr = instantiateNewExtemBase(alienPtr, exBP_Ptr); // this method does not require DNA

    // First model the carpals for the base (extBase). For cloning these will be cloned
    int numOfCarpals = alienPtr->alienDNA->skeletonDNA->extremityBP_DNA.value(exBP_Ptr->BP_ID).at(0).y();
    QVector<int> tempBPsID; tempBPsID.clear();
    for (int i=0; i<numOfCarpals; i++)
    {
        extBasePtr->extremBP_BaseCarpals.append(modelNewBP_TypeGuided(alienPtr, AlienModeller::BodyPartsEnum::ExtremBP_Carpal));
        tempBPsID << extBasePtr->extremBP_BaseCarpals.last()->BP_ID;
    }

    // Now model the digits
    int numOfDigits = alienPtr->alienDNA->skeletonDNA->extremityBP_DNA.value(exBP_Ptr->BP_ID).at(0).x();
    for (int i=0; i<numOfDigits; i++)
    {
        exBP_Ptr->extremBP_Digits.append(modelNewBP_TypeGuided(alienPtr, AlienModeller::BodyPartsEnum::ExtremBP_Digit));
        tempBPsID << exBP_Ptr->extremBP_Digits.last()->BP_ID;
    }

    // Now we need to align these, bulkify these and sculpt (including bulgification) these

    // First bulkify the Extremity BPs. When cloning a carpal/digit, this bulkification happens within cloning method
    manipulateSkeletalBulkinessDNA_Flags(alienPtr, false, false, false, false, true, new QVector<int>(0), new QVector<int>(0));
    this->buildSkeletalBP_BulkinessDNA(alienPtr, tempBPsID);
    this->bulkUpBodyPart(alienPtr, tempBPsID);

    // Align the Carpals
    QPoint alignSpec = QPoint(alienPtr->alienDNA->skeletonDNA->extremityBP_DNA.value(exBP_Ptr->BP_ID).at(1).y(), 0);
    alignBPsInVicinity(alienPtr, extBasePtr->extremBP_BaseCarpals, alignSpec); // Uses DNA

    // Attach the digits to the carpals
    attachDigitsToCarpalsForNewExtremBP(alienPtr, exBP_Ptr->extremBP_Digits, extBasePtr->extremBP_BaseCarpals); // Deos not Uses DNA

    // // Finally attach this Extremity BP to the parent BP
    // BodyPart *parentBP_Ptr = getBodyPartFromID(alienPtr, parentBP_ID);
    // QVector<int> extIDs;
    // extIDs << exBP_Ptr->BP_ID;
    // if (!attachExtremBP_ToParentBP(alienPtr, parentBP_Ptr, extIDs))
    //     return nullptr;

    // extIDs.clear();
    qDebug() << "Extremity BP modelled successfully";

    return exBP_Ptr;
}

ExtremityBP *AlienModeller::modelNewExtermityBP_UsingStateMachine(Alien *alienPtr, int parentBP_ID)
{
    qDebug() << "about to model Extremity BP using State Machine";

    // Instantiate a new extremity BP and assign an ID to it
    ExtremityBP *exBP_Ptr = instantiateNewExtremBP(alienPtr);
    // Now add a new entry to the extremity DNA
    addNewEntryForExtremityBP_DNA(alienPtr, exBP_Ptr->BP_ID);
    QPoint alignSpec = QPoint(alienPtr->alienDNA->skeletonDNA->extremityBP_DNA.value(exBP_Ptr->BP_ID).at(1).y(), 0);

    // Now instantiate the base and its carpals and digits
    ExtremityBP_Base *extBasePtr = instantiateNewExtemBase(alienPtr, exBP_Ptr);
    // float newXPos = 30.f;

    if (alienPtr->alienDNA->skeletonDNA->extremityBP_StateMachine.isEmpty())
        alienPtr->alienDNA->skeletonDNA->initializeExtremityBP_StateMachine();

    // int randState = GeometryAlgorithms::getRandomInteger(0,alienPtr->alienDNA->skeletonDNA->extremityBP_StateMachine.size());
    int energy = GeometryAlgorithms::getRandomInteger(4,GeometryAlgorithms::getRandomInteger(10,200));
    int currentState = 0;
    while (energy > 4)
    {
        qDebug() << "Energy : " << energy;
        QVector<int> tempBPsID; tempBPsID.clear();
        // execute the process for this state
        if (currentState == 0)  // Create Carpal BP
        {
            qDebug() << "State: Create Carpal BP";
            // TODO: Create a method ModelNewCarpalBP()
            extBasePtr->extremBP_BaseCarpals.append(modelNewBP_TypeGuided(alienPtr, AlienModeller::BodyPartsEnum::ExtremBP_Carpal));
            tempBPsID << extBasePtr->extremBP_BaseCarpals.last()->BP_ID;

            // First bulkify the Extremity BPs
            manipulateSkeletalBulkinessDNA_Flags(alienPtr, false, false, false, false, true, new QVector<int>(0), new QVector<int>(0));
            this->buildSkeletalBP_BulkinessDNA(alienPtr, tempBPsID);
            this->bulkUpBodyPart(alienPtr, tempBPsID);

            if (extBasePtr->extremBP_BaseCarpals.length() > 1)
                alignBPsInVicinity(alienPtr, extBasePtr->extremBP_BaseCarpals, alignSpec);

            // // If this is not the first carpal, then attach it to the root carpal (i.e. make it a sub BP of the root carpal)
            // if (extBasePtr->extremBP_BaseCarpals.length() == 1)
            //     rootCarpalPtr = extBasePtr->extremBP_BaseCarpals.last();
            // else if (extBasePtr->extremBP_BaseCarpals.length() > 1)
            // {

            // }

        }
        else if (currentState == 1) // Create & Attach Digit to Carpal
        {
            qDebug() << "State: Create & Attach Digit to Carpal";
            // TODO: Create a method ModelNewDigitBP()
            exBP_Ptr->extremBP_Digits.append(modelNewBP_TypeGuided(alienPtr, AlienModeller::BodyPartsEnum::ExtremBP_Digit));
            tempBPsID << exBP_Ptr->extremBP_Digits.last()->BP_ID;

            // First bulkify the Extremity BPs
            manipulateSkeletalBulkinessDNA_Flags(alienPtr, false, false, false, false, true, new QVector<int>(0), new QVector<int>(0));
            this->buildSkeletalBP_BulkinessDNA(alienPtr, tempBPsID);
            this->bulkUpBodyPart(alienPtr, tempBPsID);

            // Now attach the Digit to a Carpal. Select a carpal at random (or via a criteria) and ensure a carpal exists
            // make sure we have an entry in the RS as well as the DNA
            // Select a random carpal
            QPolygon tempPoly; tempPoly.clear();
            updateGroupBP_AttachmentToAnotherBP_ZoneScatteringTrend_DNA(
                        alienPtr, AlienModeller::BodyPartsEnum::ExtremBP_Digit, extBasePtr->extremBP_BaseCarpals.last()->BP_ID, true, tempPoly);
            // alienPtr->alienDNA->skeletonDNA->GroupBP_AttachmentToAnotherBP_ZoneScatteringTrend_DNA[QPair<int,int>(AlienModeller::BodyPartsEnum::ExtremBP_Digit, extBasePtr->extremBP_BaseCarpals.last()->BP_ID)][0].setX(2);
            // alienPtr->alienDNA->skeletonDNA->GroupBP_AttachmentToAnotherBP_ZoneScatteringTrend_DNA[QPair<int,int>(AlienModeller::BodyPartsEnum::ExtremBP_Digit, extBasePtr->extremBP_BaseCarpals.last()->BP_ID)][0].setY(1);
            // alienPtr->alienDNA->skeletonDNA->GroupBP_AttachmentToAnotherBP_ZoneScatteringTrend_DNA[QPair<int,int>(AlienModeller::BodyPartsEnum::ExtremBP_Digit, extBasePtr->extremBP_BaseCarpals.last()->BP_ID)][3].setX(-1);
            // alienPtr->alienDNA->skeletonDNA->GroupBP_AttachmentToAnotherBP_ZoneScatteringTrend_DNA[QPair<int,int>(AlienModeller::BodyPartsEnum::ExtremBP_Digit, extBasePtr->extremBP_BaseCarpals.last()->BP_ID)][3].setY(-1);
            // alienPtr->alienDNA->skeletonDNA->GroupBP_AttachmentToAnotherBP_ZoneScatteringTrend_DNA[QPair<int,int>(AlienModeller::BodyPartsEnum::ExtremBP_Digit, extBasePtr->extremBP_BaseCarpals.last()->BP_ID)][4].setX(-1);
            // alienPtr->alienDNA->skeletonDNA->GroupBP_AttachmentToAnotherBP_ZoneScatteringTrend_DNA[QPair<int,int>(AlienModeller::BodyPartsEnum::ExtremBP_Digit, extBasePtr->extremBP_BaseCarpals.last()->BP_ID)][4].setY(-1);
            // alienPtr->alienDNA->skeletonDNA->GroupBP_AttachmentToAnotherBP_ZoneScatteringTrend_DNA[QPair<int,int>(AlienModeller::BodyPartsEnum::ExtremBP_Digit, extBasePtr->extremBP_BaseCarpals.last()->BP_ID)][5].setY(-1);

            QVector<QVector3D> attachmentPnts, attachmentVectors;   attachmentPnts.clear(); attachmentVectors.clear();
            QVector<int> boneLocPairList; boneLocPairList.clear();
            attachBodyPartToAnotherBodyPart(alienPtr, tempBPsID, extBasePtr->extremBP_BaseCarpals.last()->BP_ID, true,
                                            attachmentPnts, attachmentVectors, boneLocPairList, false);
        }
        else if (currentState == 2) // Create & Attach Digit to Digit
        {
            qDebug() << "State: Create & Attach Digit to Digit";
            // TODO: Create a method ModelNewDigitBP()
            exBP_Ptr->extremBP_Digits.append(modelNewBP_TypeGuided(alienPtr, AlienModeller::BodyPartsEnum::ExtremBP_Digit));
            tempBPsID << exBP_Ptr->extremBP_Digits.last()->BP_ID;

            // First bulkify the Extremity BPs
            manipulateSkeletalBulkinessDNA_Flags(alienPtr, false, false, false, false, true, new QVector<int>(0), new QVector<int>(0));
            this->buildSkeletalBP_BulkinessDNA(alienPtr, tempBPsID);
            this->bulkUpBodyPart(alienPtr, tempBPsID);

            // Now attach the Digit to another Digit selected randomly
            int randDestDigitInd = GeometryAlgorithms::getRandomInteger(0, exBP_Ptr->extremBP_Digits.length()-1);

            // make sure we have an entry in the RS as well as the DNA, before we call the attachBodyPartToAnotherBodyPart method
            QPolygon tempPoly; tempPoly.clear();
            updateGroupBP_AttachmentToAnotherBP_ZoneScatteringTrend_DNA(
                        alienPtr, AlienModeller::BodyPartsEnum::ExtremBP_Digit, exBP_Ptr->extremBP_Digits[randDestDigitInd]->BP_ID, true, tempPoly);
            // alienPtr->alienDNA->skeletonDNA->GroupBP_AttachmentToAnotherBP_ZoneScatteringTrend_DNA[QPair<int,int>(AlienModeller::BodyPartsEnum::ExtremBP_Digit, exBP_Ptr->extremBP_Digits[randDestDigitInd]->BP_ID)][0].setX(2);
            // alienPtr->alienDNA->skeletonDNA->GroupBP_AttachmentToAnotherBP_ZoneScatteringTrend_DNA[QPair<int,int>(AlienModeller::BodyPartsEnum::ExtremBP_Digit, exBP_Ptr->extremBP_Digits[randDestDigitInd]->BP_ID)][0].setY(1);
            // alienPtr->alienDNA->skeletonDNA->GroupBP_AttachmentToAnotherBP_ZoneScatteringTrend_DNA[QPair<int,int>(AlienModeller::BodyPartsEnum::ExtremBP_Digit, exBP_Ptr->extremBP_Digits[randDestDigitInd]->BP_ID)][5].setY(-1);

            QVector<QVector3D> attachmentPnts, attachmentVectors;   attachmentPnts.clear(); attachmentVectors.clear();
            QVector<int> boneLocPairList; boneLocPairList.clear();
            attachBodyPartToAnotherBodyPart(alienPtr, tempBPsID, exBP_Ptr->extremBP_Digits[randDestDigitInd]->BP_ID, true,
                                            attachmentPnts, attachmentVectors, boneLocPairList, false);
        }
        else if (currentState == 3) // Increase Jnts for Carpal
        {
            // nothing to do atm
        }
        else if (currentState == 4) // Increase Jnts for Digit
        {
            // nothing to do atm
        }
        else if (currentState == 5) // equalize total length of BPs
        {

        }

        // select a random state from the QPolygon of the current state
        QPolygon tempPoly = alienPtr->alienDNA->skeletonDNA->extremityBP_StateMachine.value(currentState);
        int randPnt = GeometryAlgorithms::getRandomInteger(0,tempPoly.length());
        if (energy % 2 == 0)    // This makes sure we select a random element from the QPoint(x,y) i.e. either x() or y()
            currentState = tempPoly.at(randPnt).x();
        else
            currentState = tempPoly.at(randPnt).y();

        tempPoly.clear();
        energy -= GeometryAlgorithms::getRandomInteger(1,8);
    }

    // // Also use the first carpal as the root carpal and attach the remaining carpals to this root carpal
    // BodyPart *rootCarpal = extBasePtr->extremBP_BaseCarpals.first();
    // if (extBasePtr->extremBP_BaseCarpals.length() > 1)
    // {
    //     for (int ca=1; ca<extBasePtr->extremBP_BaseCarpals.length(); ca++)
    //     {
    //         if (!rootCarpal->subBodyParts.contains(extBasePtr->extremBP_BaseCarpals[ca]))
    //             rootCarpal->subBodyParts.append(extBasePtr->extremBP_BaseCarpals[ca]);

    //         updateBP_AttachmentToOtherBP_withAttachPositionAndVector_DNA(alienPtr, -1, rootCarpal->BP_ID, extBasePtr->extremBP_BaseCarpals[ca]->BP_ID,
    //                                                                      extBasePtr->extremBP_BaseCarpals[ca]->getPivotPoint(),
    //                                                                      extBasePtr->extremBP_BaseCarpals[ca]->getPivotVector());
    //     }
    // }

    // // Finally attach this Extremity BP to the parent BP
    // BodyPart *parentBP_Ptr = getBodyPartFromID(alienPtr, parentBP_ID);
    // QVector<int> extIDs;
    // extIDs << exBP_Ptr->BP_ID;
    // if (!attachExtremBP_ToParentBP(alienPtr, parentBP_Ptr, extIDs))
    //     return nullptr;
    // extIDs.clear();

    // // Now detach the carpals from the root carpal
    // // The reason we are doing this is to avoid bugs later.
    // rootCarpal = extBasePtr->extremBP_BaseCarpals.first();
    // QVector<BodyPart *> subCarpalsList; subCarpalsList.clear();
    // getAllSubBPsOfGivenType(alienPtr, rootCarpal, AlienModeller::BodyPartsEnum::ExtremBP_Carpal, subCarpalsList);

    // for (BodyPart *subCarpal : subCarpalsList)
    // {
    //     if (!rootCarpal->subBodyParts.isEmpty())
    //     {
    //         rootCarpal->subBodyParts.removeOne(subCarpal);
    //         updateBP_AttachmentToOtherBP_withAttachPositionAndVector_DNA(alienPtr, subCarpal->BP_ID, rootCarpal->BP_ID, -1, QVector3D(0,0,0), QVector3D(0,0,0));
    //     }
    // }


    qDebug() << "Extremity BP - from State Machine - modelled successfully";
    return exBP_Ptr;
}

void AlienModeller::modelManyExtremityBPs(Alien *alienPtr, int numOfExtBPs, QVector<ExtremityBP*> &newExtBPsList, int parentBP_ID)
{
    qDebug() << "about to model several Extremities";

    // QVector<BodyPart*> digits, carpals; digits.clear(); carpals.clear();
    // QVector<int> tempBPsID; tempBPsID.clear();
    for (int i=0; i<numOfExtBPs; i++)
    {
        newExtBPsList.append(modelNewExtermityBP(alienPtr,parentBP_ID));
        // newExtBPsList.append(modelNewExtermityBP_UsingStateMachine(alienPtr,parentBP_ID));
        // newExtBPsList.last()->getAllCarpalsAndDigits(digits, carpals);
        // for (int j=0; j<carpals.length(); j++)
        //     tempBPsID << carpals[j]->BP_ID;
    }

    // manipulateSkeletalBulkinessDNA_Flags(alienPtr, false, false, false, false, true, new QVector<int>(0), new QVector<int>(0));
    // this->buildSkeletalBP_BulkinessDNA(alienPtr, tempBPsID);
    // this->bulkUpBodyPart(alienPtr, tempBPsID);

    qDebug() << "Several Extremities modelled successfully : " << newExtBPsList.length();
}

void AlienModeller::modelAndAttachSeveralExtremBPsOnSeveralRandomParentBPs(Alien *alienPtr, int numOfExtBPs,
                                                                           QVector<ExtremityBP *> &newExtBPsList, QVector<BodyPart *> &allParents)
{
    // TODO: Use a RS and DNA for Extremity attachment
    numOfExtBPs = GeometryAlgorithms::getRandomInteger(4,5);
    for (float i=0; i<numOfExtBPs;)
    {
        // randomly select a BP of Alien. At the moment we are only using BB, UpperLimb and LowerLimb as the parent BPs
        BodyPart *tempBP = allParents.at(GeometryAlgorithms::getRandomInteger(0,allParents.length()));
        int parentBP_ID = tempBP->BP_ID;
        int parentType = tempBP->BP_Type;
        if (parentType == AlienModeller::BodyPartsEnum::BACK_BONE)
        {
            newExtBPsList.append(modelNewExtermityBP_UsingStateMachine(alienPtr,parentBP_ID));
            i++;
        }
        else if (parentType == AlienModeller::BodyPartsEnum::UPPER_LIMB  ||  parentType == AlienModeller::BodyPartsEnum::LOWER_LIMB)
        {
            newExtBPsList.append(modelNewExtermityBP(alienPtr,parentBP_ID));
            i += 0.5;
        }
    }

    qDebug() << "Several Extremities modelled and attached to Random Parent BPs successfully : " << newExtBPsList.length();
}

ExtremityBP *AlienModeller::instantiateNewExtremBP(Alien *alienPtr)
{
    ExtremityBP *exBP_Ptr = new ExtremityBP();
    exBP_Ptr->BP_ID = ++(alienPtr->BP_ID_Counter);
    alienPtr->alienExtremities.append(exBP_Ptr);
    return exBP_Ptr;
}

ExtremityBP_Base *AlienModeller::instantiateNewExtemBase(Alien *alienPtr, ExtremityBP *exBP_Ptr)
{
    ExtremityBP_Base *extBasePtr = new ExtremityBP_Base();
    extBasePtr->BP_ID = ++(alienPtr->BP_ID_Counter);
    exBP_Ptr->extremBP_basePtr = extBasePtr;
    return extBasePtr;
}


void AlienModeller::alignBPsInVicinity(Alien *alienPtr, const QVector<BodyPart *> &origBP_List, QPoint alignmentSpecs)
{
    QVector<BodyPart *> bpList1; bpList1.clear();
    for (BodyPart *inBP : origBP_List) // TODO: use std::algorithms to do this
        bpList1.append(inBP);

    BodyPart* pivotBP_Ptr = bpList1.first();
    bpList1.removeFirst();
    QVector<BodyPart *> bpList2; bpList2.clear();
    bpList2.append(pivotBP_Ptr);
    float rollAng = 0.f;

    if (alignmentSpecs.y() == 0)    // IF the alignment is 'side-ways' i.e 0
    {
        while (!bpList1.isEmpty())
        {
            BodyPart *movBP1, *movBP2;  movBP1=nullptr; movBP2=nullptr;
            BodyPart *bp1 = bpList2.last();     // source to move movBP1 to the RIGHT of it
            BodyPart *bp2 = bpList2.first();    // source move movBP2 to the LEFT of it

            movBP1 = bpList1.first();     // move this to the RIGHT of bp1
            bpList1.removeFirst();
            if (!bpList1.isEmpty())
            {
                movBP2 = bpList1.first();     // move this one to the LEFT of bp2
                bpList1.removeFirst();
            }

            // First get the right face of the root joint of this BP
            // Then get normal of the right face
            // normalize the normal. *** TODO: Make sure all normals are stored as unit vectors in the cell box ***
            // Get all of the bones of this BP in order.

            // TODO: Check if the following repetitive code can be done using a Lambda or another function
            Joint *tempJntPtr = dynamic_cast<Joint*>(getBodyPartFromID(alienPtr, bp1->rootJntID));
            // get the bone which is the first bone of this BP
            QVector<Bone*> bones; bones.clear();
            getAllBP_BonesInOrder(alienPtr, bp1, bones);
            QVector3D pt1 = bones.first()->rootCell->getCellCenterLocation();
            QVector3D pt2 = bones.first()->rootCell->getMidPointOfFace(Box::BoxFace::RIGHT);
            float vecMag = (pt2 - pt1).length();
            float vecScaleParam = (float(alignmentSpecs.x()/100.f))*vecMag;
            QVector3D normVec = tempJntPtr->cellList.first()->getNormalVectorOfBoxFace(Box::BoxFace::RIGHT).normalized();

            // find a new point at a distance (n units) along the normalized normal vector
            QVector3D newPt =   tempJntPtr->cellList.first()->getMidPointOfFace(Box::BoxFace::RIGHT) +
                                ((vecScaleParam)*normVec);

            // Rotate this BP along the pivot vector of the bp1
            GeometryAlgorithms::rotateBodyPartTowardsVecDir(*movBP1, bp1->getPivotVector());
            // Translate this BP to this new point
            GeometryAlgorithms::translateBP(movBP1, newPt.x() - movBP1->getPivotPoint().x(),
                                                    newPt.y() - movBP1->getPivotPoint().y(),
                                                    newPt.z() - movBP1->getPivotPoint().z());

            bpList2.append(movBP1);


            if (movBP2)
            {
                // DO the same for the movBP2 but move it to the left
                tempJntPtr = dynamic_cast<Joint*>(getBodyPartFromID(alienPtr, bp2->rootJntID));
                bones.clear();
                getAllBP_BonesInOrder(alienPtr, bp2, bones);
                pt1 = bones.first()->rootCell->getCellCenterLocation();
                pt2 = bones.first()->rootCell->getMidPointOfFace(Box::BoxFace::LEFT);
                vecMag = (pt2 - pt1).length();
                vecScaleParam = (float(alignmentSpecs.x()/100.f))*vecMag;
                normVec = tempJntPtr->cellList.first()->getNormalVectorOfBoxFace(Box::BoxFace::LEFT).normalized();

                // find a new point at a distance (n units) along the normalized normal vector
                newPt = tempJntPtr->cellList.first()->getMidPointOfFace(Box::BoxFace::LEFT) +
                        ((vecScaleParam)*normVec);

                // Rotate this BP along the pivot vector of the bp2
                GeometryAlgorithms::rotateBodyPartTowardsVecDir(*movBP2, bp2->getPivotVector());
                // Translate this BP to this new point
                GeometryAlgorithms::translateBP(movBP2,  newPt.x() - movBP2->getPivotPoint().x(),
                                                        newPt.y() - movBP2->getPivotPoint().y(),
                                                        newPt.z() - movBP2->getPivotPoint().z());

                bpList2.prepend(movBP1);
            }
            else // we have run out of BPs so break out of the loop
                break;
        }
    }

    else if (alignmentSpecs.y() == 1) // circular alignment
    {
        // We need to roll the vector made from (centerPnt_of_top_face - centerPnt_of_cell)
        // Each roll will give us a new vector which we will use to find a new point of placement
        while (!bpList1.isEmpty())
        {
            rollAng += 10.f;

            BodyPart *movBP1, *movBP2;  movBP1=nullptr; movBP2=nullptr;
            BodyPart *bp1 = bpList2.last();     // source to move movBP1 to the RIGHT of it
            BodyPart *bp2 = bpList2.first();    // source move movBP2 to the LEFT of it

            movBP1 = bpList1.first();     // move this to the RIGHT of bp1
            bpList1.removeFirst();
            if (!bpList1.isEmpty())
            {
                movBP2 = bpList1.first();     // move this one to the LEFT of bp2
                bpList1.removeFirst();
            }

            // First get the right face of the root joint of this BP
            // Then get normal of the right face
            // normalize the normal. *** TODO: Make sure all normals are stored as unit vectors in the cell box ***
            // Get all of the bones of this BP in order.

            // TODO: Check if the following repetitive code can be done using a Lambda or another function
            Joint *tempJntPtr = dynamic_cast<Joint*>(getBodyPartFromID(alienPtr, bp1->rootJntID));
            // get the bone which is the first bone of this BP
            QVector<Bone*> bones; bones.clear();
            getAllBP_BonesInOrder(alienPtr, bp1, bones);
            QVector3D pt1 = bones.first()->rootCell->getCellCenterLocation();
            QVector3D pt2 = bones.first()->rootCell->getMidPointOfFace(Box::BoxFace::RIGHT);
            QVector3D rollingVec = pt2 - pt1;
            float vecMag = rollingVec.length();
            float vecScaleParam = (float(alignmentSpecs.x()/100.f))*vecMag;
            rollingVec = QQuaternion::fromEulerAngles(0,0,rollAng).rotatedVector(rollingVec).normalized();

            // find a new point at a distance (n units) along the normalized normal vector
            QVector3D newPt =   tempJntPtr->cellList.first()->getMidPointOfFace(Box::BoxFace::RIGHT) +
                                ((vecScaleParam)*rollingVec);

            // Rotate this BP along the pivot vector of the bp1
            GeometryAlgorithms::rotateBodyPartTowardsVecDir(*movBP1, bp1->getPivotVector());
            // Translate this BP to this new point
            GeometryAlgorithms::translateBP(movBP1, newPt.x() - movBP1->getPivotPoint().x(),
                                                    newPt.y() - movBP1->getPivotPoint().y(),
                                                    newPt.z() - movBP1->getPivotPoint().z());

            bpList2.append(movBP1);


            if (movBP2)
            {
                // DO the same for the movBP2 but move it to the left
                tempJntPtr = dynamic_cast<Joint*>(getBodyPartFromID(alienPtr, bp2->rootJntID));
                bones.clear();
                getAllBP_BonesInOrder(alienPtr, bp2, bones);
                pt1 = bones.first()->rootCell->getCellCenterLocation();
                pt2 = bones.first()->rootCell->getMidPointOfFace(Box::BoxFace::LEFT);
                rollingVec = pt2 - pt1;
                vecMag = rollingVec.length();
                vecScaleParam = (float(alignmentSpecs.x()/100.f))*vecMag;
                rollingVec = QQuaternion::fromEulerAngles(0,0,rollAng).rotatedVector(rollingVec).normalized();

                // find a new point at a distance (n units) along the normalized normal vector
                newPt = tempJntPtr->cellList.first()->getMidPointOfFace(Box::BoxFace::LEFT) +
                        ((vecScaleParam)*rollingVec);

                // Rotate this BP along the pivot vector of the bp2
                GeometryAlgorithms::rotateBodyPartTowardsVecDir(*movBP2, bp2->getPivotVector());
                // Translate this BP to this new point
                GeometryAlgorithms::translateBP(movBP2, newPt.x() - movBP2->getPivotPoint().x(),
                                                        newPt.y() - movBP2->getPivotPoint().y(),
                                                        newPt.z() - movBP2->getPivotPoint().z());

                bpList2.prepend(movBP1);
            }
            else // we have run out of BPs so break out of the loop
                break;
        }
    }

    else if (alignmentSpecs.y() == 2) // wavy alignment
    {

    }

    bpList1.clear();
    bpList2.clear();
}

bool AlienModeller::attachExtremBP_ToParentBP(Alien *alienPtr, BodyPart *parentPtr, QVector<int> extremBP_IDs)
{
    qDebug() << "Commencing attaching of extremity BP to Parent BP";
    if (parentPtr)
    {
        QVector<Bone*> bonesInOrd; bonesInOrd.clear();
        getAllBP_BonesInOrder(alienPtr, parentPtr, bonesInOrd);
        float parentBP_Len = getTotalLengthOfSkeletalBP(alienPtr, parentPtr);
        for (int extID : extremBP_IDs)
        {
            // detachExtremBP_FromParentBP(alienPtr, extID);
            ExtremityBP *extPtr = getExtremityBP_FromID(alienPtr, extID);
            if (extPtr)
            {
                QVector<BodyPart*> digits, carpals; digits.clear(); carpals.clear();
                extPtr->getAllCarpalsAndDigits(digits, carpals);
                if (!carpals.isEmpty())
                {
                    BodyPart *rootCarpalPtr = carpals.first();
                    QVector<QVector3D> tempFaceVertices; tempFaceVertices.clear();
                    QVector<QVector3D> attachmentPnts; attachmentPnts.clear();

                    // randomly select a bone, its face and a random point on that face where this Extrem BP needs to be attached
                    // Get the front face of the last bone and find random points on this face
                    // TODO: Make this method more flexible by selecting a random Bone
                    // int randInd = GeometryAlgorithms::getRandomInteger(0,bonesInOrd.length());
                    Bone *randBonePtr = nullptr;
                    int randBoxFaceInd = 0;
                    if (parentPtr->BP_Type == AlienModeller::BodyPartsEnum::BACK_BONE)
                    {
                        randBonePtr = bonesInOrd[GeometryAlgorithms::getRandomInteger(0,bonesInOrd.length())];
                        do
                        {
                            randBoxFaceInd = GeometryAlgorithms::getRandomInteger(0,Box::BoxFace::BoxFace_Length);
                        }
                        while (randBoxFaceInd == Box::BoxFace::FRONT  ||  randBoxFaceInd == Box::BoxFace::BACK);

                        Box *tempBoxPtr = randBonePtr->rootCell->cellBox;

                        // TODO: re-design the computeRandomPointsOnFace method that accepts the face pointer
                        for (int i=0; i<tempBoxPtr->faces[randBoxFaceInd]->vertNorIndices.length()-1 /*4*/; i++) // TODO(ismail): this can be made faster as we already know there are 5 elements
                                                                                                                     // ... in the vertNorIndices list
                        {
                            tempFaceVertices.append(tempBoxPtr->vertices[tempBoxPtr->faces[randBoxFaceInd]->vertNorIndices[i]]);
                        }

                        computeRandomPointsOnFace(tempFaceVertices, 1, attachmentPnts);    // TODO(ismail): additional improvement. The points on the same face should not be too close to each other.
                    }
                    else // if its not the BB, then attach it to the last bone's front face
                    {
                        randBonePtr = bonesInOrd.last();
                        randBoxFaceInd = Box::BoxFace::FRONT;

                        // Check if its densification DNA is not present, then attach it at the last allStackedDenserLineages
                        // Otherwise attach it at the randBonePtr->rootCell->getMidPointOfFace(randBoxFaceInd)
                        if (!alienPtr->alienDNA->skeletonDNA->BP_GenDensificationDNA.contains(parentPtr->BP_ID)
                                &&
                            !randBonePtr->allStackedDenserLineages.isEmpty())
                            attachmentPnts.append(randBonePtr->allStackedDenserLineages.last()->getMidPointOfFace(randBoxFaceInd));
                        else
                            attachmentPnts.append(randBonePtr->rootCell->getMidPointOfFace(randBoxFaceInd));
                    }

                    float pitchAng = GeometryAlgorithms::getRandomInteger(-10,10);
                    float yawAng = GeometryAlgorithms::getRandomInteger(-10,10);
                    // each carpal will have one digit attached to it
                    QVector3D normVec = randBonePtr->rootCell->getNormalVectorOfBoxFace(randBoxFaceInd);
                    GeometryAlgorithms::rotateBodyPartTowardsVecDir(*(rootCarpalPtr), normVec);
                    // if (parentPtr->BP_Type != AlienModeller::BodyPartsEnum::BACK_BONE)
                    //     pitchAng = 90;
                    GeometryAlgorithms::rotateBodyPartUsingEulerAngles(*(rootCarpalPtr), pitchAng, yawAng, 0);
                    GeometryAlgorithms::translateBP(rootCarpalPtr,
                                                attachmentPnts.first().x() - rootCarpalPtr->getPivotPoint().x(),
                                                attachmentPnts.first().y() - rootCarpalPtr->getPivotPoint().y(),
                                                attachmentPnts.first().z() - rootCarpalPtr->getPivotPoint().z());

                    if (!parentPtr->subExtremityBPsIDs.contains(extID))
                    {
                        parentPtr->subExtremityBPsIDs << extID;
                        // updateBP_AttachmentToOtherBP_withAttachPositionAndVector_DNA(alienPtr, -1, parentPtr->BP_ID, extID,
                        //                                                              attachmentPnts.first(), rootCarpalPtr->getPivotVector());
                        // map/assign this extremity BP to its parent BP
                        alienPtr->mapExtremToParentBP(extPtr->BP_ID, parentPtr->BP_ID);

                        // now make the root carpal as the subBP of this parent
                        if (!parentPtr->subBodyParts.contains(rootCarpalPtr))
                            parentPtr->subBodyParts.append(rootCarpalPtr);
                    }

                    // Now scale this extremity BP according to the size of the parent BP
                    float totalCarpalsLength=0;
                    for (BodyPart *carpal : carpals)
                        totalCarpalsLength += getTotalLengthOfSkeletalBP(alienPtr, carpal);
                    qDebug() << totalCarpalsLength;
                    // exit(0);
                    // int cnt=0;
                    while (totalCarpalsLength > (GeometryAlgorithms::getRandomInteger(70,130)/100.f)*parentBP_Len)
                    {
                        QVector3D scaleVec(0.9,0.9,0.9); // scaling Vector
                        incDecBP_Size(alienPtr, rootCarpalPtr->BP_ID, scaleVec);
                        totalCarpalsLength=0;
                        for (BodyPart *carpal : carpals)
                            totalCarpalsLength += getTotalLengthOfSkeletalBP(alienPtr, carpal);
                        qDebug() << "reached 0";
                        // if (++cnt == 7)
                        //     break;
                    }
                    qDebug() << "reached 1";
                    // exit(0);
                }
                else
                {
                    qDebug() << "There are no carpals";
                    return false;
                }
            }
            else
            {
                qDebug() << "Extremity cannot be attached to the same parent. Please choose a different parent";
                return false;
            }
        }
        qDebug() << "Extremity BP attached to parent BP successfully";
        return true;
    }
    else
        return false;
}

bool AlienModeller::detachExtremBP_FromParentBP(Alien *alienPtr, int extID)
{
    qDebug() << "Commencing detaching of extremity BP from its Parent BP";

    ExtremityBP *extPtr = getExtremityBP_FromID(alienPtr, extID);
    // First find its parent
    BodyPart *parentPtr = getBodyPartFromID(alienPtr, alienPtr->extremToParentBP_Map.value(extID,-1));
    if (extPtr && parentPtr)
    {
        if (parentPtr->subExtremityBPsIDs.removeOne(extID))
        {
            for (BodyPart *sub : parentPtr->subBodyParts)
                if (extPtr->extremBP_basePtr->extremBP_BaseCarpals.contains(sub))
                {
                    parentPtr->subBodyParts.removeOne(sub); // we have removed the carpal
                    // Also remove it from the extremToParentBP_Map of Alien
                    alienPtr->extremToParentBP_Map.remove(extID);
                    qDebug() << "sub extremity of this BP detached successfully";
                    return true;
                }
        }
        else
        {
            qDebug() << "Extermity ID doesn't exist in the subExtremityBPsIDs of parent";
            return false;
        }
    }
    else
        qDebug() << "This extermity or its parent doesn't exist";

    return false;
}

QVector<int> AlienModeller::detachAllExtremBPsFromGivenParentBP(Alien *alienPtr, BodyPart *parentPtr)
{
    QVector<int> subExtremIDs; subExtremIDs.clear();
    // Also detach any sub extremity BPs
    for (int subExtID : parentPtr->subExtremityBPsIDs)
    {
        if (detachExtremBP_FromParentBP(alienPtr, subExtID))
        {
            subExtremIDs << subExtID;
            qDebug() << "Sub extremities of this BP detached";
        }
    }
    return subExtremIDs;
}

void AlienModeller::attachDigitsToCarpalsForNewExtremBP(Alien *alienPtr, QVector<BodyPart *> &digits, QVector<BodyPart *> &carpals)
{
    // There are two trends of attachments: 1) Even Distribution 2) Un-even Distribution
    // Find points on the front faces on the last bones on all the carpals
    // The number of points must be equal to number of digits
    int totDigits = digits.length();
    int portion=0, randNum=0;
    // prepare the tempFaceVertices list
    QVector<QVector3D> tempFaceVertices; tempFaceVertices.clear();
    QVector<QVector3D> attachmentPnts; attachmentPnts.clear();
    bool lastIteration=false;
    // uchar distTrend = GeometryAlgorithms::getRandomInteger(0,2);
    uchar distTrend = (digits.length() + carpals.length()) % 2;
    if (carpals.length() == 1) // If there's only one carpal, go straight to the UN-EVEN distribution
        distTrend = 1;
    for (int c=0, d=0, yawD=0; c<carpals.length(); c++)
    {
        QVector<Bone*> bonesInOrd; bonesInOrd.clear();
        getAllBP_BonesInOrder(alienPtr, carpals[c], bonesInOrd);
        // Get the front face of the last bone and find random points on this face
        // TODO: Make this method more flexible by selecting a random Bone
        Bone *lastBnPtr = bonesInOrd.last();
        Box *tempBoxPtr = lastBnPtr->rootCell->cellBox;
        tempFaceVertices.clear();
        attachmentPnts.clear();
        // TODO: re-design the computeRandomPointsOnFace method that accepts the face pointer
        for (int i=0; i<tempBoxPtr->faces[Box::BoxFace::FRONT]->vertNorIndices.length()-1 /*4*/; i++) // TODO(ismail): this can be made faster as we already know there are 5 elements
                                                                                                     // ... in the vertNorIndices list
        {
            tempFaceVertices.append(tempBoxPtr->vertices[tempBoxPtr->faces[Box::BoxFace::FRONT]->vertNorIndices[i]]);
        }

        if (distTrend == 0) // *** EVEN DISTRIBUTION ***
        {
            qDebug() << "even dist";
            yawD *= -1;
            // each carpal will have one digit attached to it
            computeRandomPointsOnFace(tempFaceVertices, 1, attachmentPnts);    // TODO(ismail): additional improvement. The points on the same face should not be too close to each other.
            GeometryAlgorithms::rotateBodyPartTowardsVecDir(*(digits[d]), carpals[c]->getPivotVector());
            GeometryAlgorithms::rotateBodyPartUsingEulerAngles(*(digits[d]), 0, d*yawD, 0);
            GeometryAlgorithms::translateBP(digits.at(d),
                                        attachmentPnts.first().x() - digits.at(d)->getPivotPoint().x(),
                                        attachmentPnts.first().y() - digits.at(d)->getPivotPoint().y(),
                                        attachmentPnts.first().z() - digits.at(d)->getPivotPoint().z());

            if (!carpals[c]->subBodyParts.contains(digits.at(d)))
            {
                carpals[c]->subBodyParts.append(digits.at(d));
                updateBP_AttachmentToOtherBP_withAttachPositionAndVector_DNA(alienPtr, -1, carpals[c]->BP_ID, digits.at(d)->BP_ID,
                                                                             attachmentPnts.first(), digits.at(d)->getPivotVector());
            }

            // attachToCarpal(carpalPtr, digitPtr, pivotVector, QVector3D(patch,yaw,roll), atmntPoint)

            if (d<digits.length()-1)
                d++;
            else    // no more digits to attach
                break;

            if (c == carpals.length()-1) // go back to the beginning
                c = 0;
        }
        else    // *** UN-EVEN DISTRIBUTION ***     (disTrend = 1)
        {
            qDebug() << "un-even dist";

            if ((totDigits - portion) == 1  ||  c == carpals.length()-1)
            {
                randNum = totDigits;
                lastIteration = true;
            }
            else
                // randNum = GeometryAlgorithms::getRandomInteger(portion,totDigits);
                randNum = totDigits/2;

            if (carpals.length() == 1) // if there is only one carpal, then attach all the digits to that carpal
            {
                randNum = totDigits;
                portion = 0;
            }
            int digitsToAttach = randNum - portion;
            // We need to attach digitsToAttach num of Digits to this carpal's last bone
            // So get all the bones in order, cuz we only need the last bone
            computeRandomPointsOnFace(tempFaceVertices, digitsToAttach, attachmentPnts);    // TODO(ismail): additional improvement. The points on the same face should not be too close to each other.
            if (attachmentPnts.length() != digitsToAttach)
            {
                qDebug() << "something's wrong in attachDigitsToCarpalsForExtremBP";
                exit(0);
            }
            // Finally Attach the Digits to attachmentPnts
            for (int i=portion, yaw=5; i<randNum; i++, yaw*=i)
            {
                GeometryAlgorithms::rotateBodyPartTowardsVecDir(*(digits[i]), carpals[c]->getPivotVector());
                GeometryAlgorithms::rotateBodyPartUsingEulerAngles(*(digits[i]), 0, yaw, 0);
                GeometryAlgorithms::translateBP(digits.at(i),
                                            attachmentPnts.first().x() - digits.at(i)->getPivotPoint().x(),
                                            attachmentPnts.first().y() - digits.at(i)->getPivotPoint().y(),
                                            attachmentPnts.first().z() - digits.at(i)->getPivotPoint().z());

                if (!carpals[c]->subBodyParts.contains(digits.at(i)))
                {
                    carpals[c]->subBodyParts.append(digits.at(i));
                    updateBP_AttachmentToOtherBP_withAttachPositionAndVector_DNA(alienPtr, -1, carpals[c]->BP_ID, digits.at(i)->BP_ID,
                                                                                 attachmentPnts.first(), digits.at(i)->getPivotVector());
                }

                attachmentPnts.removeFirst();
            }

            if (lastIteration)
                break;

            portion = randNum;
        }
    }
}

ExtremityBP *AlienModeller::transferExtBP_FromSourceAlienToDestAlienWithCloning(Alien *sourceAlien, Alien *destAlien, ExtremityBP *srcExtBP_Ptr, int parentBP_ID)
{
    qDebug() << "about to transfer Extremity BP from one alien to another";

    ExtremityBP *destExBP_Ptr = cloneExtremityBP(sourceAlien, destAlien, srcExtBP_Ptr);

    if (destExBP_Ptr)
    {
        // Now we need to attach cloned Extremity (destExBP_Ptr) to the BP selected by the user
        // But first we need to parent/attach all the carpals to the root carpal (the first carpal in the extremBP_BaseCarpals list)
        if (destExBP_Ptr->attachSubCarpalsToRootCarpal())
        {
        }

        BodyPart *parentBP_Ptr=nullptr;
        parentBP_Ptr = getBodyPartFromID(destAlien, parentBP_ID);
        QVector<int> extremBP_IDs; extremBP_IDs.clear();
        if (parentBP_Ptr)
        {
            extremBP_IDs << destExBP_Ptr->BP_ID;
            if (attachExtremBP_ToParentBP(destAlien, parentBP_Ptr, extremBP_IDs))
            {
                // // Now detach/unparent all the carpals to the root carpal (the first carpal in the extremBP_BaseCarpals list)
                // // Reason for detachment is to allow the BP
                // if (destExBP_Ptr->detachSubCarpalsFromRootCarpal())
                // {
                // }
            }
        }

        qDebug() << "Extremity BP transfered successfully from one alien to another";

        return destExBP_Ptr;
    }
    else
        return nullptr;
}

ExtremityBP *AlienModeller::cloneExtremityBP(Alien *sourceAlien, Alien *destAlien, ExtremityBP *srcExtBP_Ptr)
{
    if (srcExtBP_Ptr)
    {
        // First clone the extremity BP
        qDebug() << "about to clone Extremity BP";

        // Instantiate a new extremity BP and assign an ID to it
        ExtremityBP *destExBP_Ptr = instantiateNewExtremBP(destAlien); // this method does not require DNA

        // Now add a new entry to the extremity DNA
        // For cloning an ExtBP, simply copy the DNA
        destAlien->alienDNA->skeletonDNA->extremityBP_DNA.insert(destExBP_Ptr->BP_ID, sourceAlien->alienDNA->skeletonDNA->extremityBP_DNA.value(srcExtBP_Ptr->BP_ID));

        // Now instantiate the base and its carpals and digits
        ExtremityBP_Base *destExtBasePtr = instantiateNewExtemBase(destAlien, destExBP_Ptr); // this method does not require DNA

        // Now extract all the carpals and digits (from srcExtBP_Ptr) which will be cloned
        // Clone all the carpals and digits. Bulkification happens inside the cloneSkeletalBP_FromAnotherAlien_viaDNA() methods
        qDebug() << "reached 0";
        for (BodyPart *srcCarpal : srcExtBP_Ptr->extremBP_basePtr->extremBP_BaseCarpals)
        {
            destExtBasePtr->extremBP_BaseCarpals.append(cloneSkeletalBP_FromAnotherAlien_viaDNA(destAlien, sourceAlien, srcCarpal));
        }
        for (BodyPart *srcDigit : srcExtBP_Ptr->extremBP_Digits)
        {
            destExBP_Ptr->extremBP_Digits.append(cloneSkeletalBP_FromAnotherAlien_viaDNA(destAlien, sourceAlien, srcDigit));
        }
        qDebug() << "reached 1";

        // Now Align the Carpals
        QPoint alignSpec = QPoint(destAlien->alienDNA->skeletonDNA->extremityBP_DNA.value(destExBP_Ptr->BP_ID).at(1).y(), 0);
        alignBPsInVicinity(destAlien, destExtBasePtr->extremBP_BaseCarpals, alignSpec); // Uses DNA

        // Attach the digits to the carpals
        attachDigitsToCarpalsForNewExtremBP(destAlien, destExBP_Ptr->extremBP_Digits, destExtBasePtr->extremBP_BaseCarpals);

        qDebug() << "Extremity BP cloned successfully";

        return destExBP_Ptr;
    }
    else
    {
        qDebug() << "source Ext BP does not exists";
        return nullptr;
    }
}

void AlienModeller::removeAndDeleteExtremityBP_FromAlien(Alien *alienPtr, ExtremityBP *tempExBP, BodyPart *parentPtr)
{
    if (tempExBP    &&      alienPtr->alienExtremities.contains(tempExBP))
    {
        // TODO: Make a method here for deleting the entire extremity along with its digits and carpals
        QVector<BodyPart*> digits, carpals; digits.clear(); carpals.clear();
        tempExBP->getAllCarpalsAndDigits(digits, carpals);
        QVector<int> digitsAndCarpalsToRemove; digitsAndCarpalsToRemove.clear();
        if (!digits.isEmpty())
            for (BodyPart *dig : digits)
                if (dig)
                    digitsAndCarpalsToRemove << dig->BP_ID;
        if (!carpals.isEmpty())
            for (BodyPart *carpal : carpals)
                if (carpal)
                    digitsAndCarpalsToRemove << carpal->BP_ID;
        removeAlienBodyParts(alienPtr, digitsAndCarpalsToRemove);
        qDebug() << "all carpas and digits removed from this extermity BP";
        if (parentPtr)
        {
                parentPtr->subExtremityBPsIDs.removeOne(tempExBP->BP_ID);
        }
        alienPtr->extremToParentBP_Map.remove(tempExBP->BP_ID);
        alienPtr->alienExtremities.removeOne(tempExBP);
        // also remove its entry from the extremityBP_DNA
        alienPtr->alienDNA->skeletonDNA->extremityBP_DNA.remove(tempExBP->BP_ID);
        delete tempExBP;
        tempExBP = nullptr;
    }
}

void AlienModeller::createSimpleBone(Bone &bone, QVector3D pnt1, QVector3D pnt2)
{
/*
    // +++++++ ALGO 1

    since the bone is created between two joints, we can use the front face of the first joint and the back face of the
    second joint to create the back and front of the bone respectively

    // +++++++ ALGO 2

    Take the distance of the vector formed by the locations of the first and second joints

    Use this distance to create the bone at the origin (either create the unit cube and then scale it according to the distance
    OR create an elongated box)

    Rotate the cell along the vector computed from the locations of first and second joints

    Translate the cell to a location wich is at the middle of both the joints
*/

    QVector3D tempVec = pnt2 - pnt1;
    QVector3D tempMidPnt = pnt1 + (0.5)*tempVec;
    float tempVecMagnitude = sqrtf((tempVec.x()*tempVec.x()) + (tempVec.y()*tempVec.y()) + (tempVec.z()*tempVec.z()));

    bone.makeCell(tempMidPnt, 1); // Here energy is 1 because the default box is always created as unit cube
    bone.rootCell = bone.cellList.first();

    GeometryAlgorithms::scaleBP(&bone, 3, 3, tempVecMagnitude/2.0); // TODO(ismail): this should be goverened by the DNA
    GeometryAlgorithms::rotateBodyPartTowardsVecDir(bone, tempVec);
}

void AlienModeller::modelAllAlienBones(Alien *alien)
{
//    for (int i=0; i<alienList.last()->alienBones.length(); i++)
//    {
//        QVector3D pt1 = alienList.last()->alienJoints[alienList.last()->alienSkeleton.Jnt2JntRelations[i].x()]->BP_PivotPt;
//        QVector3D pt2 = alienList.last()->alienJoints[alienList.last()->alienSkeleton.Jnt2JntRelations[i].y()]->BP_PivotPt;
//        createSimpleBone(*(alienList.last()->alienBones[i]), pt1, pt2);
//    }

    // Now instantiate all the bones
    for (int i=0; i<alien->alienSkeleton.Jnt2JntRelations.length(); i++)
    {
//        Bone *bonePtr = new Bone();
//        bonePtr->BP_ID=alienList.last()->bodyParts.length(); // the id is incremented everytime we add a new BP
//        alienList.last()->bodyParts.append(bonePtr);
//        Bone *newBonePtr = bonePtr;
//        alienList.last()->alienBones.append(newBonePtr);
        Bone *newBonePtr = instantiateNewBone(alien);
    }


    for (int i=0; i<alien->alienBones.length(); i++)
    {
        int firJnt = alien->alienSkeleton.Jnt2JntRelations[i].x();
        int secJnt = alien->alienSkeleton.Jnt2JntRelations[i].y();
        QVector3D firJntLoc, secJntLoc;
        for (int j=0; j<alien->alienJoints.length(); j++)
        {
            if(firJnt == alien->alienJoints[j]->BP_ID)
            {
                firJntLoc = alien->alienJoints[j]->getPivotPoint();
                break;
            }
        }
        for (int j=0; j<alien->alienJoints.length(); j++)
        {
            if(secJnt == alien->alienJoints[j]->BP_ID)
            {
                secJntLoc = alien->alienJoints[j]->getPivotPoint();
                break;
            }
        }
        createSimpleBone(*(alien->alienBones[i]), firJntLoc, secJntLoc);
    }

    // createSimpleBone
    // squash bone
    // elongate bone
    // bend bone
    // compute further growth points on bone
    // generate Staircase Formations For Bone
    // model bone bulgings
    // model bone dents
    // bevel bone - TODO(Ismail): create a general bevel algorith that can bevel a box and potentially other models as well
    // generate further branching growth
    // generate mirrored growth

    // generate surrounding growth formations b/w joints. Some joints will not be attached via bones. Instead they will be
    // ... attached with bulging flesh (using the method surroundingGrowthFormations() )
}

void AlienModeller::modelAllAlienBones_TypeGuided(Alien *alien, QMap<int, QPolygon *> &tempJnt2JntRelations)
{
    qDebug() << "Modelling All bones";
    // TODO(Ismail): First delete all bones because this method can be called when smoothing or subdividing bones

    // Now instantiate all the bones
//    qDebug() << *tempJnt2JntRelations.first();
    QMapIterator<int, QPolygon*> jnt2jntIter(tempJnt2JntRelations);
    while (jnt2jntIter.hasNext()) // Traversing through each BP, which is not a joint or bone
    {
        jnt2jntIter.next();
        QPolygon *tempPoly = jnt2jntIter.value();
        for (int j=0; j<tempPoly->length(); j++)
        {
            int firJnt = tempPoly->at(j).x();
            int secJnt = tempPoly->at(j).y();
            QVector3D firJntLoc, secJntLoc;

            firJntLoc = getBodyPartFromID(alien, firJnt)->getPivotPoint();
            secJntLoc = getBodyPartFromID(alien, secJnt)->getPivotPoint();

            Bone *newBonePtr = instantiateNewBone(alien);
//            newBonePtr->ContainingBP_ID = i.key();
            createSimpleBone(*newBonePtr, firJntLoc, secJntLoc);

            newBonePtr->endJoints.first = firJnt;
            newBonePtr->endJoints.second = secJnt;

            // Add this bone to the subBPs of this BP
            getBodyPartFromID(alien, jnt2jntIter.key())->subBodyParts.append(newBonePtr);
        }
    }

    qDebug() << "All bones modelled successfully";
}

// This method only removes the bones. The joints remain there
void AlienModeller::removeAllAlienBones()
{
// Remove all the bones from the alienBones
// Remove all the bones from bodyParts


}

bool AlienModeller::removeBone(Alien *aPtr, BodyPart *bpPtr, Bone *bonePtr)
{
    if (aPtr != nullptr && bonePtr != nullptr)
    {
        if (aPtr->alienBones.removeOne(bonePtr) && aPtr->bodyParts.removeOne(bonePtr))
        {
            if (bpPtr)
                bpPtr->subBodyParts.removeOne(bonePtr);

            qDebug() << "Bone removed successfully";
            delete bonePtr;
            bonePtr = nullptr;
            return true;
        }
    }
    // TODO(ismail): Also need to delete the bone's entry from any other DS dwelling in DNA, Skeleton, Alien etc
    // TODO(ismail): Also update the DNA DSs

    qDebug() << "Bone could not be removed";
    return false;
}

void AlienModeller::modelAllAlienJoints_TypeGuided(Alien *alien, QVector<Joint *> &jntList)
{
    qDebug() << "modelling all incoming joints";
    // Now set this joint's location and create a cell for it
    for (int i=0; i<jntList.length(); i++)
    {
        int jntID = jntList[i]->BP_ID;
        jntList[i]->makeCell(alien->alienSkeleton.vectorLocationCoupleForEachJoint_TypeGuided.value(jntID).second, 5); // create the joint's cell
        // rotate the joint towards the newly computed vector: tempOrientVector
        GeometryAlgorithms::rotateBodyPartTowardsVecDir(*jntList[i],
                                                    alien->alienSkeleton.vectorLocationCoupleForEachJoint_TypeGuided.value(jntID).first);
    }
    qDebug() << "all incoming joints modelled";
}

Joint *AlienModeller::instantiateNewJoint(Alien *alien)
{
    Joint *jntPtr = new Joint();
    // TODO(ismail): Perhaps use a static member wihthin the ALienModeller class that acts a global counter.
    jntPtr->BP_ID = ++(alien->BP_ID_Counter); // the id is incremented everytime we add a new BP
    alien->bodyParts.append(jntPtr); // TODO(ismail): Better use a QMap for joints list
    alien->alienJoints.append(jntPtr);

    return jntPtr;
}

bool AlienModeller::removeJoint(Alien *alien, BodyPart *bpPtr, Joint *jPtr)
{
    if (alien != nullptr && jPtr != nullptr)
    {
        if (alien->alienJoints.removeOne(jPtr) && alien->bodyParts.removeOne(jPtr))
        {
            if (bpPtr)
                bpPtr->subBodyParts.removeOne(jPtr);

            qDebug() << "Joint removed successfully";
            delete jPtr;
            jPtr = nullptr;
            return true;
        }
    }

    // TODO(ismail): Also update the DNA DSs

    qDebug() << "Joint could not be removed";
    return false;
}

bool AlienModeller::updateNumOfJointsPerBP_DNA(Alien *alien, BodyPart *inBP_Ptr, int numOfJntsToIncDec)
{
    qDebug() << "About to update NumOfJointsPerBP_DNA";
    qDebug() << "Existing number of joints of this BP : " << this->getNumOfJntsForBP(inBP_Ptr);
    if (abs(numOfJntsToIncDec) < this->getNumOfJntsForBP(inBP_Ptr)-1)
    {
        QPolygon tempPoly = alien->alienDNA->skeletonDNA->numOfJointsPerBP_DNA.value(inBP_Ptr->BP_Type);
        for (int j=0; j<tempPoly.length(); j++)
        {
            if (tempPoly[j].x() == inBP_Ptr->BP_ID)
            {
                tempPoly[j].setY(tempPoly[j].y() + numOfJntsToIncDec);
                break;
            }
        }
        alien->alienDNA->skeletonDNA->numOfJointsPerBP_DNA.insert(inBP_Ptr->BP_Type,tempPoly);
        qDebug() << "NumOfJointsPerBP_DNA updated successfully";
        return true;
    }
    else
    {
        qDebug() << "absolute value of numOfJntsToIncDec is more than the number of joints of the BP";
        return false;
    }
}

void AlienModeller::instantiateNewBodyParts(Alien *alien, int BP_Type, int numBPs)
{
    // TODO(ismail): Create a generic/factory method to instantiate these BPs. Learn about factory methods, templates, design patterns etc
    for (int i=0; i<numBPs; i++)
    {
        if (BP_Type == AlienModeller::BodyPartsEnum::LOWER_LIMB)
        {           
            LowerLimb *lowerLimbPtr = new LowerLimb();
            // TODO(ismail): Perhaps use a static member wihthin the ALienModeller class that acts a global counter.
            addBodyPartToCurrentAlienSkeleton(alien, lowerLimbPtr);
            alien->alienLowerLimbs.append(lowerLimbPtr);
            lowerLimbPtr->parentAlienID = alien->alienID;
        }
        else if (BP_Type == AlienModeller::BodyPartsEnum::UPPER_LIMB)
        {
            UpperLimb *upperLimbPtr = new UpperLimb();
            upperLimbPtr->BP_ID = ++(alien->BP_ID_Counter); // the id is incremented everytime we add a new BP
            alien->bodyParts.append(upperLimbPtr); // TODO(ismail): Better use a QMap for joints list
            alien->alienUpperLimbs.append(upperLimbPtr);
            upperLimbPtr->parentAlienID = alien->alienID;
        }
        else if (BP_Type == AlienModeller::BodyPartsEnum::BACK_BONE)
        {
            BackBone *backBonePtr = new BackBone();
            backBonePtr->BP_ID = ++(alien->BP_ID_Counter); // the id is incremented everytime we add a new BP
            alien->bodyParts.append(backBonePtr); // TODO(ismail): Better use a QMap for joints list
            alien->alienBackBones.append(backBonePtr);
            backBonePtr->parentAlienID = alien->alienID;
        }
//        else if (BP_Type == AlienModeller::BodyPartsEnum::TAIL)
//        {
//            Tail *tailPtr = new Tail();
//            tailPtr->BP_ID = ++(alien->BP_ID_Counter); // the id is incremented everytime we add a new BP
//            alien->bodyParts.append(tailPtr); // TODO(ismail): Better use a QMap for joints list
//            alien->alienTails.append(tailPtr);
//        }
        else if (BP_Type == AlienModeller::BodyPartsEnum::HEAD)
        {
            Head *headPtr = new Head();
            headPtr->BP_ID = ++(alien->BP_ID_Counter); // the id is incremented everytime we add a new BP
            alien->bodyParts.append(headPtr); // TODO(ismail): Better use a QMap for joints list
            alien->alienHeads.append(headPtr);
            headPtr->parentAlienID = alien->alienID;
            // TODO(ismail): If the Head is instantiated then its Sub BPs should also be
            // ... instantiated
        }
        else if (BP_Type == AlienModeller::BodyPartsEnum::RIB)
        {
            Rib *ribPtr = new Rib();
            ribPtr->BP_ID = ++(alien->BP_ID_Counter); // the id is incremented everytime we add a new BP
            alien->bodyParts.append(ribPtr); // TODO(ismail): Better use a QMap for joints list
            alien->alienRibs.append(ribPtr);
            ribPtr->parentAlienID = alien->alienID;
        }

        // Instantiate the HEAD Sub BPs
        else if (BP_Type == AlienModeller::BodyPartsEnum::HEADSubBP_Maxilla)
        {
            if (!alien->alienHeads.isEmpty())
            {
                HeadSubBP_Maxilla *maxillaPtr = new HeadSubBP_Maxilla();
                maxillaPtr->BP_ID = ++(alien->BP_ID_Counter); // the id is incremented everytime we add a new BP
                alien->bodyParts.append(maxillaPtr); // TODO(ismail): Better use a QMap for joints list
                alien->alienHeadSubBP_Maxillas.append(maxillaPtr);
                maxillaPtr->parentAlienID = alien->alienID;
                alien->alienHeads.last()->subBodyParts.append(maxillaPtr);
            }
        }
        else if (BP_Type == AlienModeller::BodyPartsEnum::HEADSubBP_Mandible)
        {
            if (!alien->alienHeads.isEmpty())
            {
                HeadSubBP_Mandible *mandiblePtr = new HeadSubBP_Mandible();
                mandiblePtr->BP_ID = ++(alien->BP_ID_Counter); // the id is incremented everytime we add a new BP
                alien->bodyParts.append(mandiblePtr); // TODO(ismail): Better use a QMap for joints list
                alien->alienHeadSubBP_Mandibles.append(mandiblePtr);
                mandiblePtr->parentAlienID = alien->alienID;
                alien->alienHeads.last()->subBodyParts.append(mandiblePtr);
            }
        }
        else if (BP_Type == AlienModeller::BodyPartsEnum::HEADSubBP_Zygomatic)
        {
            if (!alien->alienHeads.isEmpty())
            {
                HeadSubBP_Zygomatic *zygPtr = new HeadSubBP_Zygomatic();
                zygPtr->BP_ID = ++(alien->BP_ID_Counter); // the id is incremented everytime we add a new BP
                alien->bodyParts.append(zygPtr); // TODO(ismail): Better use a QMap for joints list
                alien->alienHeadSubBP_Zygomatics.append(zygPtr);
                zygPtr->parentAlienID = alien->alienID;
                alien->alienHeads.last()->subBodyParts.append(zygPtr);
            }
        }
        else if (BP_Type == AlienModeller::BodyPartsEnum::ExtremBP_Carpal)
        {
            if (!alien->alienExtremities.isEmpty())
            {
                ExtremityBP_Base_Carpal *extCarpalPtr = new ExtremityBP_Base_Carpal();
                extCarpalPtr->BP_ID = ++(alien->BP_ID_Counter); // the id is incremented everytime we add a new BP
                alien->bodyParts.append(extCarpalPtr); // TODO(ismail): Better use a QMap
                extCarpalPtr->parentAlienID = alien->alienID;
            }
        }
        else if (BP_Type == AlienModeller::BodyPartsEnum::ExtremBP_Digit)
        {
            if (!alien->alienExtremities.isEmpty())
            {
                ExtremityBP_Digit *extDigitPtr = new ExtremityBP_Digit();
                extDigitPtr->BP_ID = ++(alien->BP_ID_Counter); // the id is incremented everytime we add a new BP
                alien->bodyParts.append(extDigitPtr);
                extDigitPtr->parentAlienID = alien->alienID;
            }
        }
    }
}

void AlienModeller::addBodyPartToCurrentAlienSkeleton(Alien *alien, BodyPart *bpPtr)
{
    if (bpPtr->BP_ID == -1)
        bpPtr->BP_ID = ++(alien->BP_ID_Counter); // the id is incremented everytime we add a new BP
    alien->bodyParts.append(bpPtr); // TODO(ismail): Better use a QMap for joints list
}


void AlienModeller::generateAlienSkeleton(Alien *alien)
{
/*
 * GENERATE ALIEN SKELETON:
 *
 *  call METHOD: createSkeletonSpecs() (member of Skeleton)
 *
 *
 *  call METHOD: jointsArrangements(): attach joints with bones (make a rough skeleton made out of boxes) - This step is perhaps the most important thing in
 *               the entire alien modelling system, becasue this step actually defines the overall shape of the alien.
 *      call METHOD: specify the originating point and the direction of orientation for each joint and bone
 *
 *  call METHOD: magnify/shrink some bones and joints according to others (random scalings) - this method is part of the arrangements in the above step
 *
 *
 *  call METHOD: model all alien bones
 *
 *      call METHOD: Make bone squashed or elongated
 *
 *      if Bone Growth Levels > 0
 *          call METHOD: "Create Bone growth levels" with the "curvature specified"
 *
 *      if curvature amount is less than 20
 *          call METHOD: Create parallel bones
 *
 *      if HasMirroredBranchings = false
 *          call METHOD: Create Bone Branchings (irregular version)
 *      else
 *          call METHOD: Create Bone Branchings (Mirrored version) // mirrored and symmetrical branchings
 *
 *
 *  call METHOD: model all alien joints
 *      call METHOD: model joint with the specified sub cells
 *
*/
    createSkeletonSpecs(alien);

    computeJointArrangements(alien);

    smoothAlienSkeleton(alien, GeometryAlgorithms::getRandomInteger(1,5));

    modelAllAlienBones(alien);

//    sculptBones();

    // Just make a simple bone (or sequence of bones) and test the ridges
}

void AlienModeller::bulkUpBodyPart(Alien *alien, QVector<int> &BP_ID_List)
{
/*
 * ++++++++ Algorithm ++++++++

    - Go through all the bones of this BP
    - For each Bone, select the main cell
    - Scale the main cell according to the specications in the DNA
*/
    // After the DNA has been updated, start bulking out the BPs
    qDebug() << "commencing bulking up of BP";
    for (int bpID : BP_ID_List)
    {
        if (alien->alienDNA->skeletonDNA->Skeletal_BP_Bulkiness_DNA_ForEachBP.contains(bpID))
        {
            QPolygonF tempPoly = alien->alienDNA->skeletonDNA->Skeletal_BP_Bulkiness_DNA_ForEachBP.value(bpID);
            qDebug() << tempPoly;

            // Get hold of the BodyPart pointer
            BodyPart *bpPtr = getBodyPartFromID(alien, bpID);
            // Now go through all the bones of this BP and start bulking these up
            int bulkinessTrend = static_cast<int>(tempPoly[0].x());
            int numOfSubCells = static_cast<int>(tempPoly[0].y());
            float newScaleValue = 1;
            float X_newScaleValue = 1, Y_newScaleValue = 1;
            QVector<Bone*> bones; bones.clear();
            getAllBP_BonesInOrder(alien, bpPtr, bones);
            if (tempPoly.length() != 4*bones.length())
            {
                qDebug() << "tempPoly.length() != 4*bones.length()";
                exit(0);
            }
            for (int i=0, j=0; i<bones.length(); i++)
            {
                // if (bones[i]->BP_Type == AlienModeller::BodyPartsEnum::BONE)
                // {
                    qDebug() << "reached 1";

                    qDebug() << "reached 2";

                    // IMPROVEMENT(ismail): Come up with an algorithm that can automatically identify how much the startScalingValue and scalingMultiplier,
                    // ... by assessing the area of the bone / BP. Perhaps using Machine Learning
                    float startScalingValue = tempPoly[j+1].x();
                    float scalingMultiplier = tempPoly[j+1].y();

                    int X_ScalingFlag = static_cast<int>(tempPoly[j+2].x()); // qDebug() << "X scaling flag : " << X_ScalingFlag;
                    int Y_ScalingFlag = static_cast<int>(tempPoly[j+2].y()); // qDebug() << "Y scaling flag : " << Y_ScalingFlag;

                    float zScalingFlag = tempPoly[j+3].x();
                    float zScalingFactor = tempPoly[j+3].y();

                    qDebug() << "reached 2.5";


                    // TODO (ismail): Divide the cell of this bone into subcells
                    // Scale each subcell accordingly
                    // If the bone need not be divided into further subcells, then apply the scaling on the bone itself

                    for (int sc=0; sc<numOfSubCells; sc++)
                    {
                        if (zScalingFlag == 0)
                            zScalingFactor = 1.0;
                        if ((X_ScalingFlag == 1 && Y_ScalingFlag == 1)/* || (X_ScalingFlag == 0 && Y_ScalingFlag == 0)*/) //    Apply scaling on both X and Y
                        {
                            if (j == 0 && bulkinessTrend != 3)
                            {
                                newScaleValue = startScalingValue;
                            }
                            else
                            {
                                if (bulkinessTrend == 1) // Ascending i.e. scale up
                                    newScaleValue *= scalingMultiplier;
                                else if (bulkinessTrend == 2) // Descending i.e. scale down
                                    newScaleValue /= scalingMultiplier;
                                else if (bulkinessTrend == 3) // Constant, so no scaling up or down
                                    newScaleValue = newScaleValue;
                                else if (bulkinessTrend == 4 &&  i<ceil(bones.length()/2))
                                    newScaleValue *= scalingMultiplier;
                                else if (bulkinessTrend == 4 &&  i>=ceil(bones.length()/2))
                                    newScaleValue /= scalingMultiplier;
                            }
                            if (zScalingFactor <= 1)
                                newScaleValue += (1.0 - zScalingFactor);
                            else
                                newScaleValue -= (zScalingFactor - 1.0);
                            qDebug() << "newScaleValue : " << newScaleValue;
                            GeometryAlgorithms::scaleBP(bones[i], newScaleValue, newScaleValue, zScalingFactor);

                            qDebug() << "reached 3";

                            // copy it back to the DNA, so that mitigating bulkiness would be easy next time
                        }
                        else if (X_ScalingFlag == 1 && Y_ScalingFlag == 0)
                        {
//                            qDebug() << "scaling on X only";
                            Y_newScaleValue = 1; // TODO(ismail): This should be also be selected from the DNA.
                                                 // ... Think about a Ruleset item that can change this value
                            if (j == 0)
                            {
                                X_newScaleValue = startScalingValue;
                            }
                            else
                            {
                                if (bulkinessTrend == 1) // Ascending, then scale up
                                {
                                    X_newScaleValue *= scalingMultiplier;
                                }
                                else if (bulkinessTrend == 2) // Descending, then scale down
                                {
                                    X_newScaleValue /= scalingMultiplier;
                                }
                                // TODO(ismail): This doesn't seem to have any effect, so perhaps remove it
                                else if (bulkinessTrend == 3) // Constant, so no scaling up or down.
                                {
                                    X_newScaleValue = X_newScaleValue;
                                }
                            }
                            if (zScalingFactor <= 1)
                                newScaleValue += (1.0 - zScalingFactor);
                            else
                                newScaleValue -= (zScalingFactor - 1.0);
                            qDebug() << "newScaleValue : " << newScaleValue;
                            GeometryAlgorithms::scaleBP(bones[i], X_newScaleValue, Y_newScaleValue, zScalingFactor);

                            qDebug() << "reached 4";
                        }
                        else if (X_ScalingFlag == 0 && Y_ScalingFlag == 1)
                        {
//                            qDebug() << "scaling on Y only";
                            X_newScaleValue = 1;
                            if (j == 0)
                            {
                                Y_newScaleValue = startScalingValue;
                            }
                            else
                            {
                                if (bulkinessTrend == 1) // Ascending, then scale up
                                {
                                    Y_newScaleValue *= scalingMultiplier;
                                }
                                else if (bulkinessTrend == 2) // Descending, then scale down
                                {
                                    Y_newScaleValue /= scalingMultiplier;
                                }
                                // TODO(ismail): This doesn't seem to have any effect, so perhaps remove it
                                else if (bulkinessTrend == 3) // Constant, so no scaling up or down
                                {
                                    Y_newScaleValue = Y_newScaleValue;
                                }
                            }
                            if (zScalingFactor <= 1)
                                newScaleValue += (1.0 - zScalingFactor);
                            else
                                newScaleValue -= (zScalingFactor - 1.0);
                            qDebug() << "newScaleValue : " << newScaleValue;
                            GeometryAlgorithms::scaleBP(bones[i], X_newScaleValue, Y_newScaleValue, zScalingFactor);
                        }
                    }

                    j+=4; // iterate to the next bone
                // }
            }
            qDebug() << "***** BP bulked up";
        }
        else
            qDebug() << "BP entry not found in the Skeletal_BP_Bulkiness_2_DNA";
    }

    qDebug() << "All BPs bulked up successfully";
}

void AlienModeller::bevelBP(Alien *alien, BodyPart *bpPtr)
{
    // go through all its bones
    QVector<Bone*> allBonesInOrder; allBonesInOrder.clear();
    getAllBP_BonesInOrder(alien, bpPtr, allBonesInOrder);
    for (int b=0; b<allBonesInOrder.length(); b++)
    {
        allBonesInOrder[b]->bevelBone();
    }
}

void AlienModeller::bulkUpBP_BoneSubdivCells(Alien *alien, BodyPart *bpPtr, QVector<Bone *> &boneList)
{
    // Read the values from the DNA
    // Just like the bulkification of the bone, we'll get rid of subdiv cells and bulk them according to the DNA

    QPolygonF subDivDNA_Poly = alien->alienDNA->skeletonDNA->Skeletal_BP_SubDivCells_DNA.value(bpPtr->BP_ID);
    QVector<Bone*> bonesInOrder; bonesInOrder.clear();
    this->getAllBP_BonesInOrder(alien, bpPtr, bonesInOrder);
    for (int i=0, j=0; i<bonesInOrder.length(); i++)
    {
        int sdInd = 0;
        float newScaleValue = 1;
        if (bonesInOrder[i] == boneList[j])
        {
            if (boneList[j]->subdivisionCells.isEmpty())
            {
                qDebug() << "Bone need to be subdivided first";
                return;
            }
            // Now bulk up this bone based on the DNA parameters
            sdInd = i*8;
            QPointF BT_StartScale = subDivDNA_Poly[sdInd+2];
            float startScale = BT_StartScale.y();
            QPointF bulkScaleMult = subDivDNA_Poly[sdInd+3];
            float scalMult = bulkScaleMult.x();
            QPointF bulkPercAscDes = subDivDNA_Poly[sdInd+4];
            float percAscThenDesc = bulkPercAscDes.x();

            Bone *bnPtr = boneList[j];
            // Go through its subdiv cells
            // Delete the cells
            // Recreate the cells
            // Scale them according to the DNA
            float magicResForVaryingScalMult = bulkScaleMult.y();
            float magicResForVaryingTrend = bulkPercAscDes.y();
            QVector<int> trendVec; trendVec.clear();
            int indexOfPivot = static_cast<int>((percAscThenDesc / 100.f)
                                                    * bnPtr->subdivisionCells.length());
            for (int k=0; k<bnPtr->subdivisionCells.length(); k++)
            {
                if (BT_StartScale.x() == 0) // Asc
                    trendVec << 0;
                else if (BT_StartScale.x() == 1) // Desc
                    trendVec << 1;
                else if (BT_StartScale.x() == 2) // Varying
                {
                    if (k % 2 == 0)
                        magicResForVaryingTrend /= 1.3;
                    else
                        magicResForVaryingTrend *= 2.3;

                    if ((int)magicResForVaryingTrend % 2 == 0)
                        trendVec << 0;
                    else
                        trendVec << 1;
                }
                else if (BT_StartScale.x() == 3) // Des->Asc
                {
                    if (k <= indexOfPivot)
                        trendVec << 1;
                    else
                        trendVec << 0;
                }
                else if (BT_StartScale.x() == 4) // Asc->Des
                {
                    if (k <= indexOfPivot)
                        trendVec << 0;
                    else
                        trendVec << 1;
                }

                // Take the ending points and make another cell
                // delete this cell.
                QVector3D p1 = bnPtr->subdivisionCells[k]->getMidPointOfFace(Box::BoxFace::BACK);
                QVector3D p2 = bnPtr->subdivisionCells[k]->getMidPointOfFace(Box::BoxFace::FRONT);
                Cell *cellToDel = bnPtr->subdivisionCells[k];
                int cellIndInCellList = bnPtr->cellList.indexOf(cellToDel);
                bnPtr->cellList.removeAt(cellIndInCellList);
                bnPtr->subdivisionCells.removeAt(k);
                delete cellToDel;
                Cell *newCell = makeCellBetweenTwoPoints(p1,p2);
                bnPtr->cellList.insert(cellIndInCellList,newCell);
                bnPtr->subdivisionCells.insert(k,newCell);
            }

            qDebug() << "Trend Vector : " << trendVec;
            if (trendVec.length() != bnPtr->subdivisionCells.length())
            {
                qDebug() << "trend Vec and subdivcells lengths don't match in bulkUpBP_BoneSubdivCells method";
                exit(0);
            }

            for (int k=0; k<bnPtr->subdivisionCells.length(); k++)
            {
                if (k == 0)
                    newScaleValue = startScale;
                else
                {
                    int tempRand = GeometryAlgorithms::getRandomInteger(0,2);

                    if (trendVec[k] == 0) // Asc
                    {
                        if (newScaleValue + scalMult >= 60.5)
                            newScaleValue = 60.5;
                        else
                        {
//                            if (tempRand == 0)
                                newScaleValue += scalMult;
//                            else
//                                newScaleValue *= scalMult;
                        }
                    }
                    else if (trendVec[k] == 1) // Desc
                    {
                        if (newScaleValue + scalMult <= 3.5)
                            newScaleValue = 3.5;
                        else
                        {
//                            if (tempRand == 0)
                                newScaleValue -= scalMult;
//                            else
//                                newScaleValue /= scalMult;
                        }
                    }
                }
                qDebug() << "newScaleValue : " << newScaleValue;
                GeometryAlgorithms::scaleCell(bnPtr->subdivisionCells[k], newScaleValue, newScaleValue, 1);
                // TODO: Inspect rotateCellUsingEulerAngles() as it does not do ROLL rotation propoerly!!
//                GeometryAlgorithms::rotateCellUsingEulerAngles(bnPtr->subdivisionCells[k], 0, 0, k+35.f);
            }

            j++;
        }
    }
}

QVector<QVector3D> AlienModeller::getNormalVectorsOfAdjacentBonesForGivenFace(Alien *alien, QVector<int> &bonesIDsList, int faceID)
{
    // For each bone, get the face
    // Get its normal vector and append to the list

    QVector<QVector3D> normalList; normalList.clear();
    for (int boneID : bonesIDsList)
    {
        BodyPart *bonePtr = getBodyPartFromID(alien, boneID);
        // get this bones normal vector
        normalList << bonePtr->cellList[0]->getNormalVectorOfBoxFace(faceID);
    }

    return normalList;
}

QVector<float> AlienModeller::getAnglesBetweenAdjacentVectors(QVector<QVector3D> listOfAdjVectors)
{
    QVector<float> angleList; angleList.clear();
    for (int i=0; i<listOfAdjVectors.length()-1; i++)
    {
//        QVector3D vec1 = listOfAdjVectors[i].normalized();
//        QVector3D vec2 = listOfAdjVectors[i+1].normalized();
        QVector3D vec1 = listOfAdjVectors[i].normalized();
        QVector3D vec2 = listOfAdjVectors[i+1].normalized();
        // get the angle between this and the next vector
        angleList << acos(QVector3D::dotProduct(vec1, vec2)) * 57.2957795;
    }

    return angleList;
}

QVector<int> AlienModeller::getAngleTrendsOfJntsOfBP(Alien *alienPtr, BodyPart *bpPtr)
{
    QVector<int> angleTrendList; angleTrendList.clear();
    // Get the joints of the BP in order
    QVector<int> jntIDsInOrder; jntIDsInOrder.clear();
    convertJnt2JntPolyToJointsLineageList(alienPtr->alienSkeleton.Jnt2JntRelations_TypeGuided.value(bpPtr->BP_ID),
                                          jntIDsInOrder);

    for (int jntID : jntIDsInOrder)
    {
        // For each joint, get its angle trend from the VectorEnergyAngleTrends_TypeGuided
        angleTrendList << alienPtr->alienDNA->skeletonDNA->skeletal_JATs_DNA.VectorEnergyAngleTrends_TypeGuided.value(jntID).at(1).x();
    }

    return angleTrendList;
}

void AlienModeller::densifyAllBodyParts(Alien *alien, QVector<int> &BP_ID_List)
{
    /*
     * ****** Algorithm 1 ****
     *
     *  Go through all the BPs
     *  Get the BP from the BP_DensificationDNA
     *  Go through all the bones in the map
     *  Start generating densers for each Bone base on the DNA values
     *      Generate a clone of the cell where this denser will be created, and make it rotate, scale and translate
            ... exactly like the source
            Find/Compute the attachment point for this denser using the DNA specs
            rotate the cell according to the roll angle in the DNA specs
            mirror it according to the DNA specs
            Assign this denser cell to the BP celllist
     *
    */
    qDebug() << "commencing densificaiton of all BPs";

    for (int id : BP_ID_List)
    {
        BodyPart *bpPtr = getBodyPartFromID(alien, id);
        if (bpPtr)
        {
            if (bpPtr->BP_Type != AlienModeller::BodyPartsEnum::HEAD)
            {
                alien->alienDNA->skeletonDNA->BP_BulgificationDNA.remove(bpPtr->BP_ID);
                if (!bpPtr->isSymmetric)
                {
                    BodyPart *mirrPtr = getMirrorOrOriginalBP(alien, bpPtr);
                    if (mirrPtr)
                        alien->alienDNA->skeletonDNA->BP_BulgificationDNA.remove(mirrPtr->BP_ID);

                    QVector<Bone*> bonesInOrder; bonesInOrder.clear();
                    getAllBP_BonesInOrder(alien, bpPtr, bonesInOrder);
                    for (Bone *bn : bonesInOrder)
                    {
                            qDebug() << bn->subdivisionCells.length();
                            qDebug() << bn->allStackedDenserLineages.length();
                            // exit(0);
                        if (bn->deleteAllBP_Cells(true))
                        {
                            bn->subdivisionCells.clear();
                            bn->allStackedDenserLineages.clear();
                        }
                        // while (!bn->subdivisionCells.empty()) delete bn->subdivisionCells.last(), bn->subdivisionCells.removeLast();
                    }
                }

                QMap<int, QPolygonF> tempMap; tempMap.clear();
                if (alien->alienDNA->skeletonDNA->BP_SpecificDensificationDNA.contains(id))
                {
                     tempMap = alien->alienDNA->skeletonDNA->BP_SpecificDensificationDNA.value(id);
                     // go through all the bones in this BP
                     QMapIterator<int, QPolygonF> iter(tempMap);
                     while(iter.hasNext())
                     {
                         iter.next();
                         QPolygonF tempPoly = iter.value();
                         qDebug() << tempPoly;
                         if (tempPoly.isEmpty())
                             continue;
                         int numOfStackedDensers = tempPoly[2].y();
                         Bone *bonePtr = dynamic_cast<Bone*>(getBodyPartFromID(alien, iter.key()));
                         Cell *baseCell = bonePtr->cellList[0];
                         // generate all stacked cells (densers) on this Bone
                         // First create densers from the LEFT face of the base cell box
                         // The two 'for loops' are for generating the densers on the opposite faces
                         // TODO(ismail): This can be done in a method. This is HARDCODED

                        QVector<Cell*> tempStackDnsrsLineage; tempStackDnsrsLineage.clear();
                        // tempStackDnsrsLineage.append(baseCell);
                        float rollAngle = tempPoly[6].x();
                        bool rollIncrease = tempPoly[7].x();
                        float rollIncrementAmnt = 0;
                        int startInd = 0;
                        int endInd = 1;
                        qDebug() << "reached 2";
                        if (rollIncrease)
                        {
                            rollIncrementAmnt = tempPoly[7].y();
                            startInd = ceil((tempPoly[8].x()/100.f) * numOfStackedDensers);
                            int numOfIndicesForRollIncrease = ceil((tempPoly[8].y()/100.f) * (numOfStackedDensers-startInd));
                            endInd = startInd + numOfIndicesForRollIncrease;
                        }
                        for (int j=0; j<numOfStackedDensers; j++)
                        {
                            // *** ALGO ***
                            // Find/Compute the attachment point for this denser using the DNA specs
                            // rotate the cell according to the roll angle in the DNA specs
                            // mirror it according to the DNA specs
                            // Assign this denser cell to the BP celllist
                            // ************

                            // Generate a clone of the cell where this denser will be created, and make it rotate, scale and translate
                            // ... exactly like the source

                            Cell *newClonedCell = new Cell();
                            baseCell->cloneCell(newClonedCell);
                            newClonedCell->setPivotFace(Box::BoxFace::RIGHT);

                            if (j>=startInd  &&  j<=endInd)
                            {
                                rollAngle += rollIncrementAmnt;
                                rollIncrementAmnt = 0;
                            }
                            else
                                rollAngle = tempPoly[6].x();

                            // QVector3D attachPoint = baseCell->getMidPointOfFace(Box::BoxFace::LEFT);
                            QVector3D attachPoint = baseCell->getLocationFrom_u_v_parametersOnBoxFace(Box::BoxFace::LEFT, tempPoly[13].x(), 50);

                            //                     GeometryAlgorithms::rotateCellTowardsVecDir(newClonedCell, baseCell->getPivotVector());
                            GeometryAlgorithms::rotateCellUsingEulerAngles(newClonedCell, 0, 0, rollAngle); // for more regular formations
                            // GeometryAlgorithms::rotateCellUsingEulerAngles(newClonedCell, 10, 5, rollAngle); // for more curly formations
                            // GeometryAlgorithms::scaleCell(newClonedCell, tempPoly[3].x(), tempPoly[4].x(), tempPoly[5].x()-(j/10.f));
                            GeometryAlgorithms::scaleCell(newClonedCell, tempPoly[3].x(), tempPoly[4].x(), tempPoly[5].x());
                            GeometryAlgorithms::translateCell(newClonedCell, attachPoint - newClonedCell->getPivotPoint());

                            bonePtr->cellList.append(newClonedCell);
                            tempStackDnsrsLineage.append(newClonedCell);

                            baseCell = newClonedCell;
                        }
                        qDebug() << "reached 3";

                        // Now create densers from the RIGHT face of the base cell box
                        // This is so that the densers are created in a mirror like fashion
                        baseCell = bonePtr->cellList[0];
                        rollAngle = tempPoly[6].x();
                        if (rollIncrease)
                        {
                            rollIncrementAmnt = tempPoly[7].y();
                            startInd = ceil((tempPoly[8].x()/100.f) * numOfStackedDensers);
                            int numOfIndicesForRollIncrease = ceil((tempPoly[8].y()/100.f) * (numOfStackedDensers-startInd));
                            endInd = startInd + numOfIndicesForRollIncrease;
                        }
                        for (int j=0; j<numOfStackedDensers; j++)
                        {
                            Cell *newClonedCell = new Cell();
                            baseCell->cloneCell(newClonedCell);
                            newClonedCell->setPivotFace(Box::BoxFace::LEFT);

                            if (j>=startInd  &&  j<=endInd)
                            {
                                rollAngle += rollIncrementAmnt;
                                rollIncrementAmnt = 0;
                            }
                            else
                                rollAngle = tempPoly[6].x();

                            // QVector3D attachPoint = baseCell->getMidPointOfFace(Box::BoxFace::RIGHT);
                            QVector3D attachPoint = baseCell->getLocationFrom_u_v_parametersOnBoxFace(Box::BoxFace::RIGHT, 100-tempPoly[13].x(), 50);

                            //                     GeometryAlgorithms::rotateCellTowardsVecDir(newClonedCell, baseCell->getPivotVector());
                            GeometryAlgorithms::rotateCellUsingEulerAngles(newClonedCell, 0, 0, 360-rollAngle);
                            // GeometryAlgorithms::rotateCellUsingEulerAngles(newClonedCell, 10, 355, 360-rollAngle);
                            // GeometryAlgorithms::scaleCell(newClonedCell, tempPoly[3].x(), tempPoly[4].x(), tempPoly[5].x()-(j/10.f));
                            GeometryAlgorithms::scaleCell(newClonedCell, tempPoly[3].x(), tempPoly[4].x(), tempPoly[5].x());
                            GeometryAlgorithms::translateCell(newClonedCell, attachPoint - newClonedCell->getPivotPoint());

                            bonePtr->cellList.append(newClonedCell);
                            tempStackDnsrsLineage.prepend(newClonedCell);
                            baseCell = newClonedCell;
                        }
                        bonePtr->allStackedDenserLineages.append(tempStackDnsrsLineage);
                        qDebug() << "reached 4";
                     }
                }
                else
                    qDebug() << "Entry for this BP is not in BP_SpecificDensificationDNA";
            }
            else // this BP is a Head so we need to use a different algorithm
            {

            }
            qDebug() << "reached 5";
        }
    }
    qDebug() << "all BPs densified successfully";
}

void AlienModeller::incrDecrDensifierSpikiness(Alien *alien, BodyPart *bpPtr, bool incOrDec)
{
    // for the BP, go through all the bones
    // for each bone,
    //      delete all the cells from the end until only the first cell remains
    // rebuild its densification DNA, this time with a change in the z-scaling on lineage
    // re-densify the BP

    if (alien->alienDNA->skeletonDNA->BP_SpecificDensificationDNA.contains(bpPtr->BP_ID))
    {
        QVector<Bone*> tempBoneList; tempBoneList.clear();
        getAllBP_Bones(bpPtr, tempBoneList);
        for (Bone *bonePtr : tempBoneList)
        {
            if (alien->alienDNA->skeletonDNA->BP_SpecificDensificationDNA.value(bpPtr->BP_ID).contains(bonePtr->BP_ID))
            {
                if (!bonePtr->allStackedDenserLineages.isEmpty())
                {
                    if (bonePtr->deleteAllBP_Cells(true))
                    {
                        bonePtr->allStackedDenserLineages.clear();
                        bonePtr->subdivisionCells.clear();
                        qDebug() << "cells deleted successfully except for the first cell";
                        QPolygonF &tempPoly = alien->alienDNA->skeletonDNA->BP_SpecificDensificationDNA[bpPtr->BP_ID][bonePtr->BP_ID];
                        if (incOrDec == true) // increase spikiness i.e. make it more pointy
                        {
                            if (tempPoly[5].x() >= 0.4)
                            {
                                tempPoly[5].setX(tempPoly[5].x()-0.02);
                            }
                        }
                        else // decrease spikiness i.e. make it flatened
                        {
                            if (tempPoly[5].x() <= 0.99)
                            {
                                tempPoly[5].setX(tempPoly[5].x()+0.02);
                            }
                        }
                    }
                    else
                    {
                        qDebug() << "cells not deleted properly";
                        exit(0);
                    }
                }
            }
            else
            {
                qDebug() << "DNA does not contains entry for this bone - incrDecrDensifierSpikiness method";
            }
        }
    }
}

void AlienModeller::incrDecrStackedDensers(Alien *alien, BodyPart *bpPtr, bool incOrDecr)
{
    if (alien->alienDNA->skeletonDNA->BP_SpecificDensificationDNA.contains(bpPtr->BP_ID))
    {
        QVector<Bone*> tempBoneList; tempBoneList.clear();
        getAllBP_Bones(bpPtr, tempBoneList);
        for (Bone *bonePtr : tempBoneList)
        {
            if (alien->alienDNA->skeletonDNA->BP_SpecificDensificationDNA.value(bpPtr->BP_ID).contains(bonePtr->BP_ID))
            {
                if (!bonePtr->allStackedDenserLineages.isEmpty())
                {
                    if (bonePtr->deleteAllBP_Cells(true))
                    {
                        bonePtr->allStackedDenserLineages.clear();
                        qDebug() << "cells deleted successfully except for the first cell";
                        QPolygonF &tempPoly = alien->alienDNA->skeletonDNA->BP_SpecificDensificationDNA[bpPtr->BP_ID][bonePtr->BP_ID];
                        if (incOrDecr == true) // increase spikiness i.e. make it more pointy
                        {
                            if (tempPoly[2].y() <= 100)
                            {
                                tempPoly[2].setY(tempPoly[2].y()+1);
                            }
                        }
                        else // decrease spikiness i.e. make it flatened
                        {
                            if (tempPoly[2].y() >= 2)
                            {
                                tempPoly[2].setY(tempPoly[2].y()-1);
                            }
                        }
                    }
                    else
                    {
                        qDebug() << "cells not deleted properly";
                        exit(0);
                    }
                }
            }
            else
            {
                qDebug() << "DNA does not contains entry for this bone - incrDecrDensifierSpikiness method";
            }
        }
    }
}

void AlienModeller::incrDecrDenserCurvature(Alien *alien, BodyPart *bpPtr, bool incOrDec)
{
    if (alien->alienDNA->skeletonDNA->BP_SpecificDensificationDNA.contains(bpPtr->BP_ID))
    {
        QVector<Bone*> tempBoneList; tempBoneList.clear();
        getAllBP_Bones(bpPtr, tempBoneList);
        for (Bone *bonePtr : tempBoneList)
        {
            if (alien->alienDNA->skeletonDNA->BP_SpecificDensificationDNA.value(bpPtr->BP_ID).contains(bonePtr->BP_ID))
            {
                if (!bonePtr->allStackedDenserLineages.isEmpty())
                {
                    if (bonePtr->deleteAllBP_Cells(true))
                    {
                        bonePtr->allStackedDenserLineages.clear();
                        qDebug() << "cells deleted successfully except for the first cell";
                        QPolygonF &tempPoly = alien->alienDNA->skeletonDNA->BP_SpecificDensificationDNA[bpPtr->BP_ID][bonePtr->BP_ID];
                        if (incOrDec == true) // increase spikiness i.e. make it more pointy
                        {
                            if (tempPoly[6].x() <= 90)
                                tempPoly[6].setX(tempPoly[6].x()+2);
                        }
                        else // decrease spikiness i.e. make it flatened
                        {
                            if (tempPoly[6].x() >= 0)
                                tempPoly[6].setX(tempPoly[6].x()-2);
                        }
                    }
                    else
                    {
                        qDebug() << "cells not deleted properly";
                        exit(0);
                    }
                }
            }
            else
            {
                qDebug() << "DNA does not contains entry for this bone - incrDecrDensifierSpikiness method";
            }
        }
    }
}

void AlienModeller::incDecDenserSpikeMovingUpDownPerc(Alien *alien, BodyPart *bpPtr, bool incOrDec)
{
    if (alien->alienDNA->skeletonDNA->BP_SpecificDensificationDNA.contains(bpPtr->BP_ID))
    {
        QVector<Bone*> tempBoneList; tempBoneList.clear();
        getAllBP_Bones(bpPtr, tempBoneList);
        for (Bone *bonePtr : tempBoneList)
        {
            if (alien->alienDNA->skeletonDNA->BP_SpecificDensificationDNA.value(bpPtr->BP_ID).contains(bonePtr->BP_ID))
            {
                if (!bonePtr->allStackedDenserLineages.isEmpty())
                {
                    if (bonePtr->deleteAllBP_Cells(true))
                    {
                        bonePtr->allStackedDenserLineages.clear();
                        qDebug() << "cells deleted successfully except for the first cell";
                        QPolygonF &tempPoly = alien->alienDNA->skeletonDNA->BP_SpecificDensificationDNA[bpPtr->BP_ID][bonePtr->BP_ID];
                        if (incOrDec == true) // increase spikiness i.e. make it more pointy
                        {
                            if (tempPoly[13].x() <= 75)
                                tempPoly[13].setX(tempPoly[13].x()+1);
                        }
                        else // decrease spikiness i.e. make it flatened
                        {
                            if (tempPoly[13].x() >= 25)
                                tempPoly[13].setX(tempPoly[13].x()-1);
                        }
                    }
                    else
                    {
                        qDebug() << "cells not deleted properly";
                        exit(0);
                    }
                }
            }
            else
            {
                qDebug() << "DNA does not contains entry for this bone - incrDecrDensifierSpikiness method";
            }
        }
    }
}


void AlienModeller::moveDensersAcrossBP_Bones(Alien *alien, BodyPart *bpPtr, bool upOrDown)
{
    if (alien->alienDNA->skeletonDNA->BP_SpecificDensificationDNA.contains(bpPtr->BP_ID))
    {
        QVector<Bone*> tempBoneList; tempBoneList.clear();
        getAllBP_Bones(bpPtr, tempBoneList);
        for (Bone *bnPtr : tempBoneList)
        {
            if (!bnPtr->allStackedDenserLineages.isEmpty())
            {
                if (bnPtr->deleteAllBP_Cells(true))
                {
                    bnPtr->allStackedDenserLineages.clear();
                    qDebug() << "cells deleted successfully except for the first cell";
                }
                else
                {
                    qDebug() << "cells not deleted properly";
                    exit(0);
                }
            }
        }

        QMap<int, QPolygonF> tempMap; tempMap.clear();
        if (upOrDown)
        {
            for (int i=0; i<tempBoneList.length()-1; i++)
            {
                if (alien->alienDNA->skeletonDNA->BP_SpecificDensificationDNA.value(bpPtr->BP_ID).contains(tempBoneList[i+1]->BP_ID))
                    tempMap.insert(tempBoneList[i]->BP_ID, alien->alienDNA->skeletonDNA->BP_SpecificDensificationDNA.value(bpPtr->BP_ID).value(tempBoneList[i+1]->BP_ID));
                else
                    qDebug() << "DNA does not contains entry for this bone - incrDecrDensifierSpikiness method";
            }
        }
        else
        {
            for (int i=1; i<tempBoneList.length(); i++)
            {
                if (alien->alienDNA->skeletonDNA->BP_SpecificDensificationDNA.value(bpPtr->BP_ID).contains(tempBoneList[i-1]->BP_ID))
                    tempMap.insert(tempBoneList[i]->BP_ID, alien->alienDNA->skeletonDNA->BP_SpecificDensificationDNA.value(bpPtr->BP_ID).value(tempBoneList[i-1]->BP_ID));
                else
                    qDebug() << "DNA does not contains entry for this bone - incrDecrDensifierSpikiness method";
            }
        }
        alien->alienDNA->skeletonDNA->BP_SpecificDensificationDNA.insert(bpPtr->BP_ID,tempMap);
    }
}

// LEGACY
//void AlienModeller::bulkUpBodyPart(QVector<int> &BP_ID_List)
//{
///*
// * ++++++++ Algorithm ++++++++

//    - Go through all the bones of this BP
//    - For each Bone, select the main cell
//    - Scale the main cell according to the specications in the DNA

//*/
//    // The first step is to construct/update the Skeletal_BP_Bulkiness_DNA from the Skeletal_BP_Bulkiness_RS.
//    // ... We have deferred it to this stage as each entry of the DNA has the BP_ID as the key(int)

//    buildSkeletalBP_BulkinessDNA(BP_ID_List);

//    // After the DNA has been updated, start bulking out the BPs
//    for (int bp=0; bp<BP_ID_List.length(); bp++)
//    {
//        QPolygonF tempPoly = alienList.last()->alienDNA->skeletonDNA->Skeletal_BP_Bulkiness_DNA.value(BP_ID_List[bp]);

//        // Get hold of the BodyPart
//        BodyPart *bpPtr=NULL;
//        for (int i=0; i<alienList.last()->bodyParts.length(); i++)
//        {
//            if (alienList.last()->bodyParts[i]->BP_ID == BP_ID_List[bp])
//            {
//                bpPtr = alienList.last()->bodyParts[i];
//                break;
//            }
//        }

//        // Now go through all the bones of the BP and start bulking these up
//        float newScaleValue = 1;
//        float X_newScaleValue = 1, Y_newScaleValue = 1;
//        for (int i=0, j=0; i<bpPtr->subBodyParts.length(); i++)
//        {
//            if (bpPtr->subBodyParts[i]->BP_Type == AlienModeller::BodyPartsEnum::BONE)
//            {
//                int bulkinessTrend = static_cast<int>(tempPoly[j].x());
//                float scalingFactor = tempPoly[j].y();
//                int X_ScalingFlag = static_cast<int>(tempPoly[j+1].x());
//                int Y_ScalingFlag = static_cast<int>(tempPoly[j+1].y());

//                if ((X_ScalingFlag == 1 && Y_ScalingFlag == 1) || (X_ScalingFlag == 0 && Y_ScalingFlag == 0)) //    Apply scaling on both X and Y
//                {
//                    if (j == 0)
//                    {
//                        newScaleValue = scalingFactor;
//                    }
//                    else
//                    {
//                        if (bulkinessTrend == 1) // Ascending, then scale up
//                        {
//                            newScaleValue *= scalingFactor;
//                        }
//                        else if (bulkinessTrend == 2) // Descending, then scale down
//                        {
//                            newScaleValue /= scalingFactor;
//                        }
//                        else if (bulkinessTrend == 3) // Constant, so no scaling up or down
//                        {
//                            newScaleValue = newScaleValue;
//                        }
//                    }
//                    // TODO(ismail): Don't use scaleCell. Use ScaleBP
//                    GeometryAlgorithms::scaleCell(&(bpPtr->subBodyParts[i]->cellList[0]), newScaleValue, newScaleValue, 1);
//                }
//                else if (X_ScalingFlag == 1 && Y_ScalingFlag == 0)
//                {
//                    Y_newScaleValue = 1;
//                    if (j == 0)
//                    {
//                        X_newScaleValue = scalingFactor;
//                    }
//                    else
//                    {
//                        if (bulkinessTrend == 1) // Ascending, then scale up
//                        {
//                            X_newScaleValue *= scalingFactor;
//                        }
//                        else if (bulkinessTrend == 2) // Descending, then scale down
//                        {
//                            X_newScaleValue /= scalingFactor;
//                        }
//                        // TODO(ismail): This doesn't seem to have any effect, so perhaps remove it
//                        else if (bulkinessTrend == 3) // Constant, so no scaling up or down.
//                        {
//                            X_newScaleValue = X_newScaleValue;
//                        }
//                    }
//                    // TODO(ismail): Don't use scaleCell. Use ScaleBP
//                    GeometryAlgorithms::scaleCell(&(bpPtr->subBodyParts[i]->cellList[0]), X_newScaleValue, Y_newScaleValue, 1);
//                }
//                else if (X_ScalingFlag == 0 && Y_ScalingFlag == 1)
//                {
//                    X_newScaleValue = 1;
//                    if (j == 0)
//                    {
//                        Y_newScaleValue = scalingFactor;
//                    }
//                    else
//                    {
//                        if (bulkinessTrend == 1) // Ascending, then scale up
//                        {
//                            Y_newScaleValue *= scalingFactor;
//                        }
//                        else if (bulkinessTrend == 2) // Descending, then scale down
//                        {
//                            Y_newScaleValue /= scalingFactor;
//                        }
//                        // TODO(ismail): This doesn't seem to have any effect, so perhaps remove it
//                        else if (bulkinessTrend == 3) // Constant, so no scaling up or down
//                        {
//                            Y_newScaleValue = Y_newScaleValue;
//                        }
//                    }
//                    // TODO(ismail): Don't use scaleCell. Use ScaleBP
//                    GeometryAlgorithms::scaleCell(&(bpPtr->subBodyParts[i]->cellList[0]), X_newScaleValue, Y_newScaleValue, 1);
//                }

//                j+=2; // iterate to the next bone
//            }
//        }
//    }
//}


void AlienModeller::setBoneRidgesSpecs_global()
{
    // use the bone ridge ruleset brackets to initialize the bone ridges specs
    for (int i=0; i<alienList.last()->alienBones.length(); i++)
    {
        Bone *tempBonePtr = alienList.last()->alienBones[i];
        tempBonePtr->boneRidgeSpecs_global.resize(0);
        for (int j=0; j<ridgeRuleSetPtr->ridgeRulesetBrackets_global.length(); j++)
        {
            // j can be useful to identify which particular rule has been randomly specified.
            // ... TODO (ismail): Each rule can be identified by an enum entry
            tempBonePtr->boneRidgeSpecs_global.append(
                        GeometryAlgorithms::getRandomInteger(ridgeRuleSetPtr->ridgeRulesetBrackets_global[j].x(),
                                                             ridgeRuleSetPtr->ridgeRulesetBrackets_global[j].y()));

//            qDebug() << "Global Spec" << tempBonePtr->boneRidgeSpecs_global[j];
        }
    }
}

void AlienModeller::setBoneRidgesSpecs_local()
{
    // use the bone ridge ruleset brackets to initialize the bone ridges specs
    for (int i=0; i<alienList.last()->alienBones.length(); i++)
    {
        Bone *tempBonePtr = alienList.last()->alienBones[i];
        // Now create all the ridges for this bone as specified in the boneRidgeSpecs_global[0]
        for (int j=0; j<tempBonePtr->boneRidgeSpecs_global[0]; j++)
        {
            // Instantiate all the ridges for this bone
            Ridge *ridgePtr = new Ridge();
            ridgePtr->BP_ID=alienList.last()->bodyParts.length(); // the id is incremented everytime we add a new BP
            alienList.last()->bodyParts.append(ridgePtr);
            tempBonePtr->subBodyParts.append(ridgePtr);

            // At the moment all ridges are created adjacent to each other i.e. regular alignment

            QPolygon *tempRidgeSpecPolygon_local = new QPolygon();
            tempRidgeSpecPolygon_local->resize(0);
            // randomly specify the ridge local specs and store them in the above polygon
            for (int k=0; k<ridgeRuleSetPtr->ridgeRulesetBrackets_local.length(); k++)
            {
                tempRidgeSpecPolygon_local->append(QPoint(k,
                             GeometryAlgorithms::getRandomInteger(ridgeRuleSetPtr->ridgeRulesetBrackets_local[k].x(),
                                                                  ridgeRuleSetPtr->ridgeRulesetBrackets_local[k].y())));


            }
            // store the ridge ID and its local specs in the bone's boneRidgeSpecs_local DS
            tempBonePtr->boneRidgeSpecs_local.insert(ridgePtr->BP_ID,tempRidgeSpecPolygon_local);
        }
    }
}

Bone *AlienModeller::instantiateNewBone(Alien *alien)
{
    Bone *bonePtr = new Bone();
    bonePtr->BP_ID = ++(alien->BP_ID_Counter); // the id is incremented everytime we add a new BP
    alien->bodyParts.append(bonePtr);
    Bone *newBonePtr = bonePtr;
    alien->alienBones.append(newBonePtr);

    return newBonePtr;
}

void AlienModeller::createSimpleJoint(Joint *jnt, QVector3D loc, QVector3D dir)
{
    jnt->makeCell(loc, 5); // create the joint's cell
    // rotate the joint towards the newly computed vector: tempOrientVector
    // TODO(ismail): use rotateBodyPartTowardsVecDir here
    GeometryAlgorithms::rotateCellTowardsVecDir(jnt->cellList[0], dir);
}

void AlienModeller::sculptBones()
{
/*
    // ====== ALGORITHM: Bone Bulgings ======

    Based on the 'Bone scaling factor along its z-axis (roll vector)', scale the bone

    Based on which area the bulgings will occur, select random points in those areas

    Create box...

    Based on whether the bulgings are bilobed or not, select two random point(s) in that end where the bulgings will be created

    (SECONDARY) Now use the rule 'nature of protrusion of the ridges/bulging' to decide how much bigger or smaller
    the bulgings will be



    // ====== ALGORITHM: Bone Ridges ======

    read the total number of ridges to be created from the ruleset

    read the number of faces that the ridge should cover

    create the ridge lines on all the faces
        SUB_ALGORITHM (SubAlgo_1): This algorithm takes in a number of cells for a BP, and creates ridges on the cells.
        Input: number of faces, cell ptr, BP id, bone_ridge_ruleset
            * use computeRandomPointsOnFace method to find a point on the face
            ** create a ridge line
            * Randomly select an edge of this face that will serve as the ridge vector
            * compute a vector in the direction of this edge. This will be our ridge vector
            * (secondary option) : Perhaps rotate slightly along the same face to give a slightly angled ridge direction
            * find the point of intersection of this vector and one of the edges that are perpendicular to this vector
            * find the face that is sharing this edge
            * rotate this vector so that it lies on the face which is adjacent to this 'common edge'. Lets call this 'attaching face'
            * ... This can be computed as: 1) use an edge of the 'attaching face' which is perpendicular to the 'common edge'
            * ... 2) find another point on the vector starting from this point. This finds a new 'line'
            * ... 3) Find the 'point of intersection' between this 'line' and the edge on the 'attaching face', which is
            * ... perpendicular to the 'common edge'
            * after we have the lines and their vectors, find the distorted points along these lines
            * ... (Distorted points are those points that distract away from these ridge lines. These distorted points will
            * ... serve as the origin points for the tiny boxes that are the building blocks of the ridges.
            * ... These tiny boxes will flow in wavy fashion in terms of their protrusion from the face i.e. in the direction
            * ... of the face's normal vector)
            *
            * Using the above approach, create all the ridge lines specified in the ruleset
            * ... i.e. create all the distorted points and store them in a list
            *
            * If the next ridge line is going out of the face, then create this ridge line on the opposite direction of the
            * ... first ridge line, and then start adding more ridges on that direction


    for (total number of ridges to create)
    {
        for (total number of distorted points for this ridge)
            create a box at each distorted point so that its front face's normal points towards normal of the
            ... face it is created on

            Now create additional boxes on top of this box according to the ridge protrusion amount
            ... (This can be saved in additional ruleset for ridge aesthetics)
    }

    // ====== ALGORITHM: Bone GROWTH FORMATIONS ======

    // ====== ALGORITHM: Bone Scalings ======

    // ====== ALGORITHM: Bone Branchings ======

*/

    // ====== ALGORITHM IMPLEMENTATION: Bone Ridges ======
    // specify the global ridge specs for any given bone
    setBoneRidgesSpecs_global();

    // Now instantiate all the ridges for the bones according to the values selected in boneRidgeSpecs_global DS
    // ... and set their local specs
    setBoneRidgesSpecs_local();

    alienList.last()->tempVectorsListForRendering.resize(0);

    // Now start generating the ridges on all the bones' surface according to the bone ridge specs
    for (int i=0; i<alienList.last()->alienBones.length(); i++)
    {
        Bone *tempBonePtr = alienList.last()->alienBones[i];

//        qDebug() << "Bone: " << i;

        // Iterate through all the ridges that need to be generated on this bone
        QMapIterator<int, QPolygon*> tempIter(tempBonePtr->boneRidgeSpecs_local);
//        for (tempIter = tempBonePtr->boneRidgeSpecs_local.constBegin(); tempIter != tempBonePtr->boneRidgeSpecs_local.constEnd(); ++tempIter)
        while (tempIter.hasNext())
        {
            tempIter.next();
            Ridge *ridgePtr;
            int ridge_ID = tempIter.key();
            QPolygon *tempPoly = tempIter.value();

//            qDebug() << "local specs for this ridge: ";
            for (int k=0; k<tempPoly->length(); k++)
            {
//                 qDebug() << tempPoly->at(k).x() << " " << tempPoly->at(k).y();
            }
            // Get the ridge's pointer from the subBodyParts matching the ridge_ID
            for (int k=0; k<tempBonePtr->subBodyParts.length(); k++)
            {
                if (ridge_ID == tempBonePtr->subBodyParts[k]->BP_ID)
                {
                    ridgePtr = static_cast<Ridge*>(tempBonePtr->subBodyParts[k]);
                    break;
                }
            }

            QVector<int> facesAlreadyVisited;
            facesAlreadyVisited.resize(0);
            // Loop through the amount of faces (stored in boneRidgeSpecs_local) that the ridge must occupy. This was randomly selected using setBoneRidgesSpecs_local() method
            QVector3D StartPtOnFace;
            for (int f=0; f<tempPoly->at(0).y(); f++)
            {
                Edge *tempEdgeNotAdjacToFrontOrBackFace;
                int currentFaceIndex;
                if (f == 0)     // If this is the first face, we need to find a random face and a point on this face
                                // This random face must not be the FRONT or BACK face
                                // ... This random face acts as the starting face
                                // This happens for every ridge

                {
                    currentFaceIndex = GeometryAlgorithms::getRandomInteger(0,5);
                    if (currentFaceIndex == Box::BoxFace::FRONT || currentFaceIndex == Box::BoxFace::BACK)
                        currentFaceIndex++;
//                    qDebug() << "starting face is: " << currentFaceIndex;
                    facesAlreadyVisited.append(currentFaceIndex);

                    // prepare the tempFaceVertices list
                    // TODO(ismail): make a method for it as it is being used often
                    QVector<QVector3D> points;
                    points.resize(0);
                    QVector<QVector3D> tempFaceVertices;
                    tempFaceVertices.resize(0);
                    // Use computeRandomPointsOnFace method to find a point on the face
                    for (int k=0; k<tempBonePtr->cellList[0]->cellBox->faces[currentFaceIndex]->vertNorIndices.length()-1; k++)
                    {
                        tempFaceVertices.append(tempBonePtr->cellList[0]->cellBox->vertices[tempBonePtr->cellList[0]->cellBox->faces[currentFaceIndex]->vertNorIndices[k]]);
            //            qDebug () << tempFaceVertices[j];
                    }
                    computeRandomPointsOnFace(tempFaceVertices, 1, points);
                    StartPtOnFace = 1*points[0];
                }

                // else we need to find the face that this edge is adjacent to. This edge being the one where the intersection point was found
                else
                {
//                    while (facesAlreadyVisited.contains(faceIndex))
//                    {

//                    }
                    currentFaceIndex = tempBonePtr->cellList[0]->cellBox->getAdjacentFaceOfEdge(tempEdgeNotAdjacToFrontOrBackFace, currentFaceIndex); // this seems to work fine
//                    qDebug() << "adjacent face to edge is: " << currentFaceIndex;
                    if (currentFaceIndex == -1)
                    {
                        qDebug() << "No face adjacent to this edge...so there is a problem...aborting program";
                        exit(1);
                    }

                }


                // Now start generating the ridge lines (straight lines) travelling through adjacent faces
                // ... Should we find the adjacent faces beforehand or on the fly??

                // First select the edge of this face that shares the FRONT or BACK faces.
                // We need this edge to get a direction in which the ridge will be made
                int tempAdjFaceInd;
                Edge *tempEdgeAdjacToFrontOrBackFace;
                for (int k=0; k<tempBonePtr->cellList[0]->cellBox->faces[currentFaceIndex]->edgeList.length(); k++)
                {
                    tempAdjFaceInd = tempBonePtr->cellList[0]->cellBox->getAdjacentFaceOfEdge(tempBonePtr->cellList[0]->cellBox->faces[currentFaceIndex]->edgeList[k], currentFaceIndex);
                    if (tempAdjFaceInd == Box::BoxFace::FRONT || tempAdjFaceInd == Box::BoxFace::BACK)
                    {
                        tempEdgeAdjacToFrontOrBackFace = tempBonePtr->cellList[0]->cellBox->faces[currentFaceIndex]->edgeList[k];
//                        qDebug() << "front or back face detected";
//                        break;
                    }
                    else
                    {
                        if (f == 0) // if this is starting face, first detect a non-front or non-back face, and then detect a front or back face
                        {
                            tempEdgeNotAdjacToFrontOrBackFace = tempBonePtr->cellList[0]->cellBox->faces[currentFaceIndex]->edgeList[k];
//                            qDebug() << "This is the first face so non-front or non-back face detected";
                        }
                    }
                }

                // Now that we got the edge (adjacent to Front or Back face) we need to compute the vector in the direction of this edge which can be used for the ridge
                // Also, Find the common point on tempEdgeAdjacToFrontOrBackFace edge and tempEdgeNotAdjacToFrontOrBackFace, and then take the vector as ==> vec = other point - common point
                // this ensures we don't flow in the opposite direction
                QVector3D tempCommonVx, tempEdgeVec;
                if (tempBonePtr->cellList[0]->cellBox->getCommonVertexBetweenTwoEdges(tempEdgeAdjacToFrontOrBackFace, tempEdgeNotAdjacToFrontOrBackFace,tempCommonVx))
                {
//                    qDebug() << "Common vertex detected";
                    if (tempBonePtr->cellList[0]->cellBox->vertices[tempEdgeAdjacToFrontOrBackFace->vertexInd_1] != tempCommonVx)
//                        tempEdgeVec = (tempBonePtr->cellList[0].cellBox->vertices[tempEdgeAdjacToFrontOrBackFace->vertexInd_1] - tempCommonVx).normalized();
                        tempEdgeVec = tempBonePtr->cellList[0]->cellBox->vertices[tempEdgeAdjacToFrontOrBackFace->vertexInd_1] - tempCommonVx;
                    else
//                        tempEdgeVec = (tempBonePtr->cellList[0].cellBox->vertices[tempEdgeAdjacToFrontOrBackFace->vertexInd_2] - tempCommonVx).normalized();
                        tempEdgeVec = tempBonePtr->cellList[0]->cellBox->vertices[tempEdgeAdjacToFrontOrBackFace->vertexInd_2] - tempCommonVx;
                }
                else
                {
                    qDebug() << "There is no common vertex between the edges so there is a problem...Aborting program";
                    exit(1);
                }


                // Find a point along this vector which is far away and then find the point of intersection of this vector line with the edge that is perpendicular to this vector line

                QVector3D tempPt1 = StartPtOnFace + (tempEdgeVec);
                QVector3D tPt1, tPt2, finalIntPt;
                float tempDisBtwPts=10000;

//                Edge *duplicateEdgePtr = tempBonePtr->cellList[0].cellBox->duplicateEdge(tempEdgeNotAdjacToFrontOrBackFace);
//                for (int k=0; k<tempBonePtr->cellList[0].cellBox->faces[currentFaceIndex].edgeList.length(); k++)
//                {
//                    // We need to exclude the edge pointed to by tempEdgeNotAdjacToFrontOrBackFace since we dont want to find the intersection point on the same edge we starting from in this face
//                    if (!(tempBonePtr->cellList[0].cellBox->areEdgesSame(&(tempBonePtr->cellList[0].cellBox->faces[currentFaceIndex].edgeList[k]), duplicateEdgePtr)))
//                    {
//                        tPt1 = tempBonePtr->cellList[0].cellBox->vertices[tempBonePtr->cellList[0].cellBox->faces[currentFaceIndex].edgeList[k].vertexInd_1];
//                        tPt2 = tempBonePtr->cellList[0].cellBox->vertices[tempBonePtr->cellList[0].cellBox->faces[currentFaceIndex].edgeList[k].vertexInd_2];
//                        QVector3D tIntPt;
//                        if (GeometryAlgorithms::getLinesIntersection(StartPtOnFace, tempPt1, tPt1, tPt2, tIntPt));
//                        {
//                            if (tIntPt.distanceToPoint(StartPtOnFace) < tempDisBtwPts)
//                            {
//                                tempDisBtwPts = tIntPt.distanceToPoint(StartPtOnFace);
//                                finalIntPt = tIntPt;
//                                tempEdgeNotAdjacToFrontOrBackFace = &(tempBonePtr->cellList[0].cellBox->faces[currentFaceIndex].edgeList[k]);
//                            }
//                        }
//                    }
//                }

                Edge *duplicateEdgePtr = tempBonePtr->cellList[0]->cellBox->duplicateEdge(tempEdgeNotAdjacToFrontOrBackFace);
                for (int k=0; k<tempBonePtr->cellList[0]->cellBox->faces[currentFaceIndex]->edgeList.length(); k++)
                {
                    // We need to exclude the edge pointed to by tempEdgeNotAdjacToFrontOrBackFace since we dont want to find the intersection point on the same edge we starting from in this face
                    tempAdjFaceInd = tempBonePtr->cellList[0]->cellBox->getAdjacentFaceOfEdge(tempBonePtr->cellList[0]->cellBox->faces[currentFaceIndex]->edgeList[k], currentFaceIndex);
                    if (!(tempBonePtr->cellList[0]->cellBox->areEdgesSame(tempBonePtr->cellList[0]->cellBox->faces[currentFaceIndex]->edgeList[k], duplicateEdgePtr))

                            &&
                        (tempAdjFaceInd != Box::BoxFace::FRONT && tempAdjFaceInd != Box::BoxFace::BACK)
                       )
                    {
                        tempEdgeNotAdjacToFrontOrBackFace = tempBonePtr->cellList[0]->cellBox->faces[currentFaceIndex]->edgeList[k];
                    }
                }


                // Now that we have the final intersection point, the ridge will be made between tempNewPt and finalIntPt
                // So NOW FINALLY CREATE THE RIDGE !!!

                // For now lets just test using GL Lines
                // This is just to store the parent and child joint's locations to be rendered as opengl lines
                QPair<QVector3D,QVector3D> vec3DPair;
                vec3DPair.first = StartPtOnFace;
                vec3DPair.second = tempPt1;
                alienList.last()->tempVectorsListForRendering.append(vec3DPair);

                StartPtOnFace = tempPt1;

            }

        }

    }
}


//// This algorithm divides a bone into several cells and bends them accordingly
//void AlienModeller::bendBone(Bone *bonePtr)
//{
//    /*
//     * ++++++++ Algorithm - Create sub cells of the bone instead of creating new bones ++++++++
//     *
//     * Get the two joints of this bone
//     * Get a vector between these two joints (j1 and j2)
//     * Dividing_distance = 100/number of sub cells
//     * For the number of sub cells to be created
//     *      Find all points at a distance of 'Dividing_distance' from the end of the previous sub cell
//     *      if (bendType == 0)
//     *      {
//     *          go through all the points
//     *          {
//     *              create a new cell and add it to this bone's cellList
//     *          }
//     *      }
//     *      else if (bendType == 1)
//     *      {
//     *          go through all the points
//     *          {
//     *              Rotate the vector (made of this point and the next point) 90 degrees in the pitch angle
//     *              find new points in this vector's direction based on an algorithm to create C shape
//     *              create a new cell and add it to this bone's cellList
//     *          }
//     *      }
//     *      else if (bendType == 2)
//     *      {
//     *              Rotate the vector (made of this point and the next point) 90 degrees in the pitch angle
//     *              find new points in this vector's direction based on an algorithm to create S shape
//     *              create a new cell and add it to this bone's cellList
//     *      }
//     *
//     * Make these set of cells smooth (tweak the smoothing algorithm to smooth out the bone)
//     *
//    */

//}

// This algorithm could be useful for dividing a bone into more bones

void AlienModeller::bendBone(Alien *alienPtr, Bone *bonePtr)
{
    /*
     * ++++++++ Algorithm ++++++++
     *
     * This method can create a 'C' shaped bend or an 'S' shaped bend
     * Get the two joints of this bone
     * Get a vector between these two joints (j1 and j2)
     * Find a point at a random location on this bone line/vector. preferrably the middle point
     * Rotate the vector 90 degrees in the pitch angle or the yaw angle (choose between these angles randomly)
     * Randomly select a scaling factor between (1% and 50%)
     * Get a scaled vector
     * Get the new point at the end of the line defined by the scaled vector
     * Create a new joint j3 at this new point
     * Create two bones between (j1 and j3) and between (j3 and j2).
     * Get rid of the bone in the alien that was between j1 and j2, as this no longer exists
     * Make these set of bones smooth
     *
    */
    QPolygonF thisBoneDNA_Poly = alienPtr->alienDNA->skeletonDNA->BoneBending_DNA.value(bonePtr->BP_ID);

    if (thisBoneDNA_Poly.length() > 0)
    {
        float bendType = thisBoneDNA_Poly.at(0).y(); // 0 = no bending; 1 = C shaped bend; 2 = S shaped bend
        if (bendType != 0)
        {
            QVector<QVector3D> tempPtsList;

            Joint *tempJntPtr1 = dynamic_cast<Joint*>(getBodyPartFromID(alienPtr, bonePtr->endJoints.first));
            Joint *tempJntPtr2 = dynamic_cast<Joint*>(getBodyPartFromID(alienPtr, bonePtr->endJoints.second));

            QVector3D tempVec1, tempVec2;
            QVector3D temPnt1, temPnt2;

            // TODO: Make a method for converting a straight line into a curve.
            // ... Inputs: line points, bendType, perc to travel on line to find the points
            tempPtsList << tempJntPtr1->getPivotPoint();
            // Get a vector between these two joints (j1 and j2)
            tempVec1 = tempJntPtr2->getPivotPoint() - tempJntPtr1->getPivotPoint();
            // get the two breakage points using the %ages of bends from the DNA
            // First point
            temPnt1 = tempPtsList[0] + ((thisBoneDNA_Poly[1].x()/100) * tempVec1);
            // rotate vector 90 degrees along a desired face's normal
            tempVec1 = QQuaternion::fromEulerAngles(90, 0, 0).rotatedVector(tempVec1).normalized();
            temPnt1 += (thisBoneDNA_Poly[2].x() * tempVec1);
            tempPtsList << temPnt1;

            if (bendType == 2) // we need to add another point
            {
                // 2nd point
                tempVec2 = tempJntPtr2->getPivotPoint() - tempJntPtr1->getPivotPoint();
                temPnt2 = tempPtsList[0] + ((thisBoneDNA_Poly[1].y()/100) * tempVec2);
                // get the vector flipped in opposite direction
                temPnt2 += (thisBoneDNA_Poly[2].y() * ((-1)*tempVec1));
                tempPtsList << temPnt2;
            }

            tempPtsList << tempJntPtr2->getPivotPoint();

            QVector<QVector3D> bulkXYZ_List = QVector<QVector3D>(tempPtsList.length()-1,QVector3D(1,1,1));
            // Now that the 2 points have been found, it is time to generate the two cells and add them to the bone
            QVector<Cell*> subDivCells = subdivideBoneIntoSubcells(bonePtr, tempPtsList, bulkXYZ_List);
            // Now apply smoothing as specified in the DNA
        }
    }
}

QVector<QVector3D> AlienModeller::convertLineToCurve(QPair<QVector3D,QVector3D> line, bool isThisS_Bend, QPolygonF bendDNA_Poly)
{
    QVector<QVector3D> curvPnts; curvPnts.clear();

    QVector3D tempVec1, tempVec2; // vectors
    QVector3D temPnt1, temPnt2; // points/vertices

    curvPnts << line.first;
    // Get a vector between these two joints (j1 and j2)
    tempVec1 = line.second - line.first;
    // get the two breakage points using the %ages of bends from the DNA
    // First point
    temPnt1 = curvPnts[0] + ((bendDNA_Poly[0].x()/100.f) * tempVec1);
    // rotate vector 90 degrees along a desired face's normal
    tempVec1 = QQuaternion::fromEulerAngles(0, 90, 0).rotatedVector(tempVec1).normalized();
    temPnt1 += (bendDNA_Poly[1].x() * tempVec1); // sharpness of bend. The higher the sharper
    curvPnts << temPnt1;

    // 2nd point
    tempVec2 = line.second - line.first;
    temPnt2 = curvPnts[0] + ((bendDNA_Poly[0].y()/100.f) * tempVec2);
    if (isThisS_Bend) // we need to add another point to create an S type bend - get the vector flipped in opposite direction
        temPnt2 += (bendDNA_Poly[1].y() * ((-1.f)*tempVec1)); // sharpness of bend. The higher the sharper
    else
        temPnt2 += (bendDNA_Poly[1].y() * ((1.f)*tempVec1)); // shrink the vector to get a smoother appearance
    curvPnts << temPnt2 << line.second;

    return curvPnts;
}

QPair<QVector3D, QVector3D> AlienModeller::getPositionOfEndJointsOfBone(Alien *alienPtr, Bone *bonePtr)
{
    QVector3D firstJointPos = getBodyPartFromID(alienPtr, bonePtr->endJoints.first)->getPivotPoint();
    QVector3D secondJointPos = getBodyPartFromID(alienPtr, bonePtr->endJoints.second)->getPivotPoint();

    return QPair<QVector3D,QVector3D>(firstJointPos,secondJointPos);
}

void AlienModeller::bulgifyBodyPart(Alien *alienPtr, BodyPart *bpPtr)
{
    // // First remove its densification DNA if it has one
    alienPtr->alienDNA->skeletonDNA->BP_GenDensificationDNA.remove(bpPtr->BP_ID);
    //     alienPtr->alienDNA->skeletonDNA->BP_SpecificDensificationDNA.remove(bpPtr->BP_ID);

    subDivideBP_BonesIntoSubdivCells(alienPtr, bpPtr);

    // finally ensure that the extremity (rootCarpal) is re-attached at the end of the allStackedDenserLineages
    if (!bpPtr->subExtremityBPsIDs.isEmpty())
    {
        if (!attachExtremBP_ToParentBP(alienPtr, bpPtr, bpPtr->subExtremityBPsIDs))
        {
            qDebug() << "something went wrong with re-attaching the extremities to this BP - bulgifyBodyPart method";
            exit(0);
        }
    }
}

// TODO(ismail): Possibly Buggy Code, because what if the bone already has more than one cells??
// ... For instance, in the case of Densers??
// ... So first we need to remove all the previous cells of the Bone
// For a single cell bone, it works alright
QVector<Cell*> AlienModeller::subdivideBoneIntoSubcells(Bone *bonePtr, QVector<QVector3D> &inputPts, QVector<QVector3D> &bulkVec)
{
    // TODO(ismail): First remove all the cells of the Bone
    int randChoice = GeometryAlgorithms::getRandomInteger(0,1);
    // we can find the width of the bone and use as the starting x-scale value for the subdiv cells
    QVector<Cell*> subDivCellsList; subDivCellsList.clear();

    if (randChoice == 0)
    {
        qDebug() << "reached 3";
        for (int i=0; i<inputPts.length()-1; i++)
        {
            Cell *newCellPtr = makeCellBetweenTwoPoints(inputPts[i], inputPts[i+1]);           
            // We can smooth the cell according to the input params
            newCellPtr = smoothCell(newCellPtr, 1);
            // TODO: What about Strafing?
            GeometryAlgorithms::scaleCell(newCellPtr, bulkVec[i].x(), bulkVec[i].y(), bulkVec[i].z());

            subDivCellsList.append(newCellPtr);

/*
            // NOW GENERATE THE CELL LINEAGE
            // TODO: Please make a method for this.
            // QVector<Cell*> tempCellLineage; tempCellLineage.clear();
            Cell *tempNewCellPtr = newCellPtr;
            int numOfStackedCells = GeometryAlgorithms::getRandomInteger(6,10);
            float xScal=1,yScal=1,zScal=1;
            QPolygonF x_y_AnglePoly; x_y_AnglePoly.clear();
            float startXScal = GeometryAlgorithms::getRandomFloat(0.3,0.8);
            float startYScal = GeometryAlgorithms::getRandomFloat(0.3,0.8);
            float startZScal = GeometryAlgorithms::getRandomFloat(0.3,0.8);
            // find a random face, making sure its not a front or back face
            int randBoxFaceInd = 0;
            do
            {
                randBoxFaceInd = GeometryAlgorithms::getRandomInteger(0,Box::BoxFace::BoxFace_Length);
            }
            while (randBoxFaceInd == Box::BoxFace::FRONT  ||  randBoxFaceInd == Box::BoxFace::BACK);
            QVector3D attachPnt = tempNewCellPtr->getMidPointOfFace(randBoxFaceInd);
            for (int j=0; j<numOfStackedCells; j++)
            {
                Cell *newClonedCell = new Cell();
                newClonedCell->isDenser = true;
                tempNewCellPtr->cloneCell(newClonedCell);
                // Make sure we dont clone any of it's child cells
                while (!newClonedCell->childCells.empty()) delete newClonedCell->childCells.last(), newClonedCell->childCells.removeLast();

                if (!tempNewCellPtr->childCells.contains(newClonedCell))
                    tempNewCellPtr->childCells.append(newClonedCell);

                newClonedCell->setPivotFace(Box::BoxFace::BACK);
                if (j==0)
                {
                    QVector3D faceNormal = tempNewCellPtr->getNormalVectorOfBoxFace(randBoxFaceInd);
                    GeometryAlgorithms::rotateCellTowardsVecDir(newClonedCell, faceNormal);
                    GeometryAlgorithms::scaleCell(newClonedCell, startXScal, startYScal, startZScal);
                    int pitch = GeometryAlgorithms::getRandomInteger(-70,70);
                    int yaw = GeometryAlgorithms::getRandomInteger(-30,30);
                    for (int ang=0; ang<numOfStackedCells; ang++)
                    {
                        int randInt = GeometryAlgorithms::getRandomInteger(0,2);
                        if (randInt==0)
                            x_y_AnglePoly << QPointF(pitch,yaw);
                        else
                            x_y_AnglePoly << QPointF(360-pitch,360-yaw);
                        xScal = GeometryAlgorithms::getRandomFloat(0.8,1.f);
                        yScal = GeometryAlgorithms::getRandomFloat(0.8,1.f);
                        zScal = GeometryAlgorithms::getRandomFloat(0.8,1.f);
                    }
                }
                else
                {
                    attachPnt = tempNewCellPtr->getMidPointOfFace(Box::BoxFace::FRONT);
                    GeometryAlgorithms::scaleCell(newClonedCell, xScal, yScal, zScal);
                }

                GeometryAlgorithms::rotateCellUsingEulerAngles(newClonedCell, x_y_AnglePoly.at(j).x(), x_y_AnglePoly.at(j).y(), 0); // roll angle is 0
                GeometryAlgorithms::translateCell(newClonedCell, attachPnt - newClonedCell->getPivotPoint());

                // tempCellLineage.append(newClonedCell);
                tempNewCellPtr = newClonedCell;
            }
*/
            qDebug() << "reached 4";
        }

        // // --- Mound Generation Algorithm on Cell --------------------------------------------------------
        // // -----------------------------------------------------------------------------------------------
        // // Now decide whether to create a mound or not
        // int numOfMounds=5;
        // QVector<Cell*> tempCells; tempCells.append(bonePtr->subdivisionCells);
        // QVector<QVector3D> tempFaceVertices; tempFaceVertices.clear();
        // QVector<QVector3D> attachmentPnts; attachmentPnts.clear();
        // qDebug() << "reached 5";
        // for (int i=0; i<numOfMounds; i++)
        // {
        //     // create a mound on this root cell
        //     // Get a random face of the cell
        //     // find a random point on the random face
        //     // generate some surrounding points
        //     // Generate cells on these points and rotate the cells towards the normal vector of this face (this is the first layer of cells)
        //     // scale the above cells so they are much smaller than the original/root cell
        //     // find more points between the mid points of the front faces of first layer cells
        //     // repeat the above process to add more layers of cells

        //     Cell *tempCell =  tempCells.at(GeometryAlgorithms::getRandomInteger(0,tempCells.length()));
        //     Box *tempBoxPtr = tempCell->cellBox;
        //     tempFaceVertices.clear();
        //     attachmentPnts.clear();
        //     int randBoxFaceInd = Box::BoxFace::LEFT;
        //     int numOfCellsInLayer = 10;
        //     qDebug() << "reached 6";
        //     do
        //     {
        //         randBoxFaceInd = GeometryAlgorithms::getRandomInteger(0,Box::BoxFace::BoxFace_Length);
        //     }
        //     while (randBoxFaceInd == Box::BoxFace::FRONT  ||  randBoxFaceInd == Box::BoxFace::BACK);
        //     qDebug() << "reached 7";

        //     // TODO: re-design the computeRandomPointsOnFace method that accepts the face pointer
        //     for (int i=0; i<tempBoxPtr->faces[randBoxFaceInd]->vertNorIndices.length()-1 /*4*/; i++) // TODO(ismail): this can be made faster as we already know there are 5 elements
        //                                                                                                  // ... in the vertNorIndices list
        //     {
        //         tempFaceVertices.append(tempBoxPtr->vertices[tempBoxPtr->faces[randBoxFaceInd]->vertNorIndices[i]]);
        //     }
        //     computeRandomPointsOnFace(tempFaceVertices, numOfCellsInLayer, attachmentPnts);    // TODO(ismail): additional improvement. The points on the same face should not be too close to each other.
        //     QVector3D faceNormal = tempCell->getNormalVectorOfBoxFace(randBoxFaceInd);
        //     int numOfLayers = 4;
        //     qDebug() << "reached 8";
        //     for (int l=0; l<numOfLayers; l++)
        //     {
        //         QVector<Cell*> cLayer; cLayer.clear();
        //         for (int j=0; j<numOfCellsInLayer; j++)
        //         {
        //             Cell *newClonedCell = new Cell();
        //             // newClonedCell->isDenser = true;
        //             tempCell->cloneCell(newClonedCell);
        //             // Make sure we dont clone any of it's child cells
        //             while (!newClonedCell->childCells.empty()) delete newClonedCell->childCells.last(), newClonedCell->childCells.removeLast();

        //             float vecMag1 = (newClonedCell->getMidPointOfFace(Box::BoxFace::TOP) - newClonedCell->getMidPointOfFace(Box::BoxFace::BOTTOM)).length();
        //             float vecMag2 = (newClonedCell->getMidPointOfFace(Box::BoxFace::FRONT) - newClonedCell->getMidPointOfFace(Box::BoxFace::BACK)).length();
        //             // if (vecMag2>vecMag1)
        //             //     GeometryAlgorithms::scaleCell(newClonedCell, 1, 1, vecMag2/vecMag1);
        //             do
        //             {
        //                 GeometryAlgorithms::scaleCell(newClonedCell, 1, 1, 0.9);
        //                 vecMag2 = (newClonedCell->getMidPointOfFace(Box::BoxFace::FRONT) - newClonedCell->getMidPointOfFace(Box::BoxFace::BACK)).length();
        //             }
        //             while (vecMag2 > vecMag1);

        //             GeometryAlgorithms::rotateCellTowardsVecDir(newClonedCell, faceNormal);
        //             // newClonedCell->setPivotFace(Box::BoxFace::BACK);
        //             GeometryAlgorithms::scaleCell(newClonedCell, 0.6, 0.6, 0.6);
        //             GeometryAlgorithms::translateCell(newClonedCell, attachmentPnts[j] - newClonedCell->getPivotPoint());

        //             newClonedCell = smoothCell(newClonedCell, 1);

        //             cLayer.append(newClonedCell);
        //         }
        //         qDebug() << "reached 9";
        //         attachmentPnts.clear();
        //         for (int j=0; j<cLayer.length()-1; j++)
        //         {
        //             int randDecide = GeometryAlgorithms::getRandomInteger(0,2);
        //             if (randDecide) // lets find a point mid way between this cells front face's midpoint and the next cell
        //             {
        //                 attachmentPnts <<   cLayer[j]->getMidPointOfFace(Box::BoxFace::FRONT) +
        //                                     (0.5)*(cLayer[j+1]->getMidPointOfFace(Box::BoxFace::FRONT) - cLayer[j]->getMidPointOfFace(Box::BoxFace::FRONT));
        //             }
        //         }
        //         qDebug() << "reached 10";
        //         numOfCellsInLayer = attachmentPnts.length();

        //         bonePtr->cellList.append(cLayer);
        //         bonePtr->subdivisionCells.append(cLayer);

        //         if (!cLayer.isEmpty())
        //             tempCell = cLayer.first();
        //     }

        //     // tempCells.removeOne(tempCell);
        // }
        //         qDebug() << "reached 11";
        // // --- Mound Generation Algorithm on Cell --------------------------------------------------------
        // // -----------------------------------------------------------------------------------------------





    }
    else if (randChoice == 1)
    {
    //     for (int i=0,j=inputPts.length()-1; i<j; i++,j--)
    //     {
    //         Cell *newCellPtr = makeCellBetweenTwoPoints(inputPts[i], inputPts[i+1]);
    //         if (i == j)
    //             GeometryAlgorithms::scaleCell(newCellPtr,startScaleVal,scaleMult,1);
    //         else
    //         {
    //             GeometryAlgorithms::scaleCell(newCellPtr,startScaleVal,scaleMult,1);
    //             GeometryAlgorithms::scaleCell(newCellPtr,startScaleVal,scaleMult,1);

    //             // Now rotate the cells (roll angle) according to DNA (OPTIONAL STEP)
    // //            GeometryAlgorithms::rotateCellUsingEulerAngles(bonePtr->cellList[i],0,0,rollAng);
    // //            GeometryAlgorithms::rotateCellUsingEulerAngles(bonePtr->cellList[j],0,0,rollAng);
    //         }
    //         startScaleVal *= scaleMult;
    // //        rollAng += 30;
    //     }
    }

    return subDivCellsList;
}

void AlienModeller::subDivideBP_BonesIntoSubdivCells(Alien *alienPtr, BodyPart *bpPtr)
{
    buildRebuildUpdate_BP_BulgificationDNA(alienPtr, bpPtr->BP_ID, false);
    // get its mirror as the mirror will also be bulgified
    BodyPart *mirrorPtr=nullptr;
    if (!bpPtr->isSymmetric)
        mirrorPtr = getMirrorOrOriginalBP(alienPtr, bpPtr);

    QVector<Bone*> bonesInOrder_forOrig; bonesInOrder_forOrig.clear();
    this->getAllBP_BonesInOrder(alienPtr, bpPtr, bonesInOrder_forOrig);

    QVector<Bone*> bonesInOrder_forMirr; bonesInOrder_forMirr.clear();
    if (mirrorPtr)
    {
        this->getAllBP_BonesInOrder(alienPtr, mirrorPtr, bonesInOrder_forMirr);
        // number of bones in both original and its mirror must be same
        if (bonesInOrder_forOrig.length() != bonesInOrder_forMirr.length())
        {
            qDebug() << "bone lengths are not same for orig and mirror BP";
            exit(0);
        }
    }
    QVector<Cell*> cellsToDelete;
    for (int i=0; i<bonesInOrder_forOrig.length(); i++)
    {
        // cellsToDelete.clear();
        // int numOfSubDivsForEachBone = static_cast<int>(tPolyF[p].x());
        Bone *origBonePtr = bonesInOrder_forOrig[i];
        Bone *mirrBonePtr=nullptr;
        if (mirrorPtr)
            mirrBonePtr = bonesInOrder_forMirr[i];
        // Every time the user subdives the BP (bulgify the BP), then we need to first remove the previous subdiv cells and re-subdivide the bone
        if (!origBonePtr->allStackedDenserLineages.isEmpty())
        {
            if (origBonePtr->cellList.length() > 1)
                origBonePtr->cellList.remove(1,origBonePtr->cellList.length()-1);
            // // Also delete the allStackedDenserLineages cells, which will automatically delete any child cells if that cell was smoothed
            while (!origBonePtr->allStackedDenserLineages.empty()) delete origBonePtr->allStackedDenserLineages.last(), origBonePtr->allStackedDenserLineages.removeLast();
        }
        if (!origBonePtr->subdivisionCells.isEmpty())
        {
            if (origBonePtr->cellList.length() > 1)
                origBonePtr->cellList.remove(1,origBonePtr->cellList.length()-1);
            // Also delete the subdiv cells, which will automatically delete any child cells if that cell was smoothed
            while (!origBonePtr->subdivisionCells.empty()) delete origBonePtr->subdivisionCells.last(), origBonePtr->subdivisionCells.removeLast();
        }
        if (mirrBonePtr  &&  !mirrBonePtr->allStackedDenserLineages.isEmpty())
        {
            if (mirrBonePtr->cellList.length() > 1)
                mirrBonePtr->cellList.remove(1,mirrBonePtr->cellList.length()-1);
            // // Also delete the allStackedDenserLineages cells, which will automatically delete any child cells if that cell was smoothed
            while (!mirrBonePtr->allStackedDenserLineages.empty()) delete mirrBonePtr->allStackedDenserLineages.last(), mirrBonePtr->allStackedDenserLineages.removeLast();
        }
        if (mirrBonePtr  &&  !mirrBonePtr->subdivisionCells.isEmpty())
        {
            if (mirrBonePtr->cellList.length() > 1)
                mirrBonePtr->cellList.remove(1,mirrBonePtr->cellList.length()-1);
            // Also delete the subdiv cells, which will automatically delete any child cells if that cell was smoothed
            while (!mirrBonePtr->subdivisionCells.empty()) delete mirrBonePtr->subdivisionCells.last(), mirrBonePtr->subdivisionCells.removeLast();
        }

        qDebug() << "reached -3";
        if (alienPtr->alienDNA->skeletonDNA->BP_BulgificationDNA.value(bpPtr->BP_ID).value(origBonePtr->BP_ID).isEmpty())
            continue;
        int numOfSubDivs = alienPtr->alienDNA->skeletonDNA->BP_BulgificationDNA.value(bpPtr->BP_ID).value(origBonePtr->BP_ID).at(0).x();
        int curvingOnLastBone = alienPtr->alienDNA->skeletonDNA->BP_BulgificationDNA.value(bpPtr->BP_ID).value(origBonePtr->BP_ID).at(0).y();
        int bulkingTrend = alienPtr->alienDNA->skeletonDNA->BP_BulgificationDNA.value(bpPtr->BP_ID).value(origBonePtr->BP_ID).at(1).x();
        int extendLastBoneTip = alienPtr->alienDNA->skeletonDNA->BP_BulgificationDNA.value(bpPtr->BP_ID).value(origBonePtr->BP_ID).at(1).y();
        int cellSmoothAllowed = alienPtr->alienDNA->skeletonDNA->BP_BulgificationDNA.value(bpPtr->BP_ID).value(origBonePtr->BP_ID).at(2).y();
        QVector3D XYZ_SmoothFactor = QVector3D  (alienPtr->alienDNA->skeletonDNA->BP_BulgificationDNA.value(bpPtr->BP_ID).value(origBonePtr->BP_ID).at(2).y(),
                                                 alienPtr->alienDNA->skeletonDNA->BP_BulgificationDNA.value(bpPtr->BP_ID).value(origBonePtr->BP_ID).at(3).x(),
                                                 alienPtr->alienDNA->skeletonDNA->BP_BulgificationDNA.value(bpPtr->BP_ID).value(origBonePtr->BP_ID).at(3).y());
        float curvingAngle_forOrig = (float)alienPtr->alienDNA->skeletonDNA->BP_BulgificationDNA.value(bpPtr->BP_ID).value(origBonePtr->BP_ID).at(4).x();
        int shouldCurvAngleChange = alienPtr->alienDNA->skeletonDNA->BP_BulgificationDNA.value(bpPtr->BP_ID).value(origBonePtr->BP_ID).at(4).y();

        qDebug() << "reached -2";
        // Get the ending joints of this bone
        // ... and find points on this bone according to the numOfSubDivsForEachBone
        Joint *tempJntPtr1 = dynamic_cast<Joint*>(getBodyPartFromID(alienPtr, origBonePtr->endJoints.first));
        Joint *tempJntPtr2 = dynamic_cast<Joint*>(getBodyPartFromID(alienPtr, origBonePtr->endJoints.second));
        QVector3D tempVec_orig = tempJntPtr2->getPivotPoint() - tempJntPtr1->getPivotPoint();
        QPair<QVector3D, QVector3D> endJntsPosForMirrBone; QVector3D tempVec_mirr;
        if (mirrBonePtr)
        {
            endJntsPosForMirrBone = getPositionOfEndJointsOfBone(alienPtr, mirrBonePtr);
            tempVec_mirr = endJntsPosForMirrBone.second - endJntsPosForMirrBone.first;
        }
        qDebug() << "reached -1";
        QVector<QVector3D> tempPtsList_forOrig; tempPtsList_forOrig.clear();
        QVector<QVector3D> tempPtsList_forMirror; tempPtsList_forMirror.clear();
        float perc = 100.f / numOfSubDivs;
        float tempTotal = perc;
        tempPtsList_forOrig << tempJntPtr1->getPivotPoint();
        if (mirrBonePtr)
            tempPtsList_forMirror << endJntsPosForMirrBone.first;
        // We need to find 1 point less than the number of subdiv cells
        qDebug() << "reached 0";
        bool ignoreLastPnt=false;
        // This for loop computes all the points between the bone. These points will act as the subdiv cells points
        qDebug() << "numOfSubDivs : " << numOfSubDivs;
        for (int j=1; j<numOfSubDivs && tempTotal<100; j++)
        {
            if (i<bonesInOrder_forOrig.length()-1)
            {
                curvingAngle_forOrig = 0.f;
            }
            else // curving to be done on the last bone only
            {
                ignoreLastPnt=true;
                if (curvingOnLastBone==1)
                {
                    int randNum = 0;
                    if (shouldCurvAngleChange == 1)
                    {
                        randNum = GeometryAlgorithms::getRandomInteger(0,100);
                        if (randNum % 2 == 0)
                        {
                            curvingAngle_forOrig += 30.f;
                        }
                        // else
                        //     curvingAngle = (float)SubDivCells_DNA.value(bpPtr->BP_ID).value(bonePtr->BP_ID).at(4).x();
                    }
                    randNum = GeometryAlgorithms::getRandomInteger(0,100);
                    if (randNum % 2 == 0)
                    {
                        curvingAngle_forOrig = (float)alienPtr->alienDNA->skeletonDNA->BP_BulgificationDNA.value(bpPtr->BP_ID).value(origBonePtr->BP_ID).at(4).x();
                    }
                    else
                    {
                        curvingAngle_forOrig = 360.f - curvingAngle_forOrig;
                    }
                }
            }
            qDebug() << "curvingAngle for orig: " << curvingAngle_forOrig;
            tempVec_orig = GeometryAlgorithms::rotateVec_EulerAngles(tempVec_orig, 0.f,curvingAngle_forOrig,0.f);
            tempPtsList_forOrig << tempPtsList_forOrig.last() + ((perc/100.f) * tempVec_orig);

            if (mirrBonePtr)
            {
                tempVec_mirr = GeometryAlgorithms::rotateVec_EulerAngles(tempVec_mirr, 0.f,360-curvingAngle_forOrig,0.f);
                tempPtsList_forMirror << tempPtsList_forMirror.last() + ((perc/100.f) * tempVec_mirr);
            }

            tempTotal += perc;
        }

        // Set up the Bulkiness of subdiv cells
        QVector<QVector3D> bulkXYZ_List = QVector<QVector3D>(numOfSubDivs,QVector3D(1,1,1));
        // origBonePtr->rootCell = origBonePtr->cellList.first();
        QVector3D tempVec = origBonePtr->rootCell->getMidPointOfFace(Box::BoxFace::LEFT) - origBonePtr->rootCell->getMidPointOfFace(Box::BoxFace::RIGHT);
        float startingX_Scale = tempVec.length()/2.f;
        tempVec = origBonePtr->rootCell->getMidPointOfFace(Box::BoxFace::TOP) - origBonePtr->rootCell->getMidPointOfFace(Box::BoxFace::BOTTOM);
        float startingY_Scale = tempVec.length()/2.f;

        // if (GeometryAlgorithms::getRandomInteger(0,100) % 2 == 0)
        // {
            startingX_Scale *= 1.5;
            startingY_Scale *= 1.5;
        // }

        if (bulkingTrend == 2   ||   bulkingTrend == 3) // 2=swellingOnEnds,3=swelling in middle
        {
            // TODO: Make a method that takes in the number of subDivs and returns a vector 'QVector<QVector3D> bulkXYZ_List' of scalings
            for (int m=0,n=numOfSubDivs-1; m<=n /*&& startingX_Scale>0.3 && startingY_Scale>0.3 && startingX_Scale<10.5 && startingY_Scale<10.5*/; m++,n--)
            {
                if (bulkingTrend == 2)
                {
                    startingX_Scale*=0.9;
                    startingY_Scale*=0.9;
                }
                else if (bulkingTrend == 3)
                {
                    if (m==0)
                    {
                        startingX_Scale*=0.4;
                        startingY_Scale*=0.4;
                    }
                    startingX_Scale/=0.9;
                    startingY_Scale/=0.9;
                }
                if (startingX_Scale<1.3)
                    startingX_Scale=1.4;
                if (startingY_Scale<1.3)
                    startingY_Scale=1.4;
                if (startingX_Scale>25.5)
                    startingX_Scale=25.5;
                if (startingY_Scale>25.5)
                    startingY_Scale=25.5;

                bulkXYZ_List[m] = QVector3D(startingX_Scale,startingY_Scale,1);
                bulkXYZ_List[n] = QVector3D(startingX_Scale,startingY_Scale,1);
            }
        }
        if (bulkingTrend == 0   ||   bulkingTrend == 1) // 0=decreasing,1=increasing
        {
            // TODO: Make a method for this
            for (int m=0; m<numOfSubDivs; m++)
            {                
                if (bulkingTrend == 0)
                {
                    startingX_Scale*=0.9;
                    startingY_Scale*=0.9;
                }
                else if (bulkingTrend == 1)
                {
                    if (m==0)
                    {
                        startingX_Scale*=1.f;
                        startingY_Scale*=1.f;
                    }
                    startingX_Scale/=0.9;
                    startingY_Scale/=0.9;
                }
                if (startingX_Scale<1.3)
                    startingX_Scale=1.4;
                if (startingY_Scale<1.3)
                    startingY_Scale=1.4;
                if (startingX_Scale>25.5)
                    startingX_Scale=25.5;
                if (startingY_Scale>25.5)
                    startingY_Scale=25.5;

                bulkXYZ_List[m] = QVector3D(startingX_Scale,startingY_Scale,1);
            }
        }

        qDebug() << "reached 1";

        bool extend=false;
        if (!ignoreLastPnt)
        {
            tempPtsList_forOrig << tempJntPtr2->getPivotPoint();
            if (mirrBonePtr)
                tempPtsList_forMirror << endJntsPosForMirrBone.second;
        }
        else
        {
            tempPtsList_forOrig << tempPtsList_forOrig.last() + ((perc/100.f) * (tempPtsList_forOrig.last()-tempPtsList_forOrig[tempPtsList_forOrig.length()-2]));
            if (mirrBonePtr)
                tempPtsList_forMirror << tempPtsList_forMirror.last() + ((perc/100.f) * (tempPtsList_forMirror.last()-tempPtsList_forMirror[tempPtsList_forMirror.length()-2]));
            extend=true;
        }

        // Now we need to actually subdivide the bone into subdivision cells

        // if (i==bonesInOrder.length()-1    &&      extendLastBoneTip==1)
        //     extend = true;
        QVector<Cell*> subDivCellsList_origBone = subdivideBoneIntoSubcells(origBonePtr, tempPtsList_forOrig, bulkXYZ_List);
        origBonePtr->cellList.append(subDivCellsList_origBone);
        origBonePtr->subdivisionCells.append(subDivCellsList_origBone);

        QVector<Cell*> subDivCellsList_mirrBone; subDivCellsList_mirrBone.clear();
        if (mirrBonePtr)
        {
            subDivCellsList_mirrBone = subdivideBoneIntoSubcells(mirrBonePtr, tempPtsList_forMirror, bulkXYZ_List);
            mirrBonePtr->cellList.append(subDivCellsList_mirrBone);
            mirrBonePtr->subdivisionCells.append(subDivCellsList_mirrBone);
        }

        // ... The above method makeCellBetweenTwoPoints automatically creates the cell in a rotated manner

        if (i==bonesInOrder_forOrig.length()-1  &&  extend) // extend it
        {
            // TODO: Make this part of the Bulgification DNA i.e. BP_BulgLastBoneGrowth_DNA
            QPolygonF tempGrowthPoly; tempGrowthPoly.clear();
            tempGrowthPoly << QPointF(GeometryAlgorithms::getRandomInteger(7,20), Box::BoxFace::FRONT)
                           << QPointF(2, 0)    // (yaw,pitch)
                           << QPointF(0, 0)     // (roll,should it mirror?)
                           << QPointF(0, 0)
                           << QPointF(GeometryAlgorithms::getRandomFloat(0.9,1.1), GeometryAlgorithms::getRandomFloat(0.95,1.05))
                           << QPointF(GeometryAlgorithms::getRandomFloat(0.95,1.05), -1);
            float rollAngle = 0;
            QVector<float> rollAngleList = QVector<float>(tempGrowthPoly.at(0).x(),rollAngle);



            generateStackedDensersFromCell(origBonePtr, subDivCellsList_origBone.last(), tempGrowthPoly, rollAngleList);

            // tempGrowthPoly[1].setX(360-tempGrowthPoly.at(1).x());
            if (mirrBonePtr)
                generateStackedDensersFromCell(mirrBonePtr, subDivCellsList_mirrBone.last(), tempGrowthPoly, rollAngleList);
        }
    }
}

Cell *AlienModeller::makeCellBetweenTwoPoints(QVector3D firstPnt, QVector3D secondPnt)
{
    QVector3D tempVec = secondPnt - firstPnt;
    // float tempVecMagnitude = sqrtf((tempVec.x()*tempVec.x()) + (tempVec.y()*tempVec.y()) + (tempVec.z()*tempVec.z()));
    float tempVecMagnitude = tempVec.length();
    QVector3D tempMidPnt = firstPnt + (0.5*tempVec);

    Cell *newCellPtr = new Cell(tempMidPnt, 1, -1);
    newCellPtr->makeBox();

    GeometryAlgorithms::rotateCellTowardsVecDir(newCellPtr, tempVec);
    int x_y_scalFac = GeometryAlgorithms::getRandomInteger(1,2);
    GeometryAlgorithms::scaleCell(newCellPtr, x_y_scalFac, x_y_scalFac, tempVecMagnitude/2.0);
    return newCellPtr;
}

Cell *AlienModeller::smoothCell(Cell *cellPtr, int smoothlevels)
{
    // create a clone of this cell
    // create a ball for cell
    // return the cell

    // Now add more subcells to it to make it like a ball
    // Cell *newClonedCell = new Cell();
    // cellPtr->cloneCell(newClonedCell);
    // while (!newClonedCell->childCells.empty()) delete newClonedCell->childCells.last(), newClonedCell->childCells.removeLast();
    Cell *origCell = cellPtr;
    for (int i=0; i<smoothlevels; i++) // generate the y-smoothing-box
    {
        Cell *newClonedCell = new Cell();
        origCell->cloneCell(newClonedCell);
        while (!newClonedCell->childCells.empty()) delete newClonedCell->childCells.last(), newClonedCell->childCells.removeLast();
        cellPtr->childCells.append(newClonedCell);
        GeometryAlgorithms::scaleCell(newClonedCell, 0.6, 1.4, 1);
        origCell = newClonedCell;
    }
    origCell = cellPtr;
    for (int i=0; i<smoothlevels; i++) // generate the x-smoothing-box
    {
        Cell *newClonedCell = new Cell();
        origCell->cloneCell(newClonedCell);
        while (!newClonedCell->childCells.empty()) delete newClonedCell->childCells.last(), newClonedCell->childCells.removeLast();
        cellPtr->childCells.append(newClonedCell);
        GeometryAlgorithms::scaleCell(newClonedCell, 1.4, 0.6, 1);
        origCell = newClonedCell;
    }
    // origCell = cellPtr;
    // for (int i=0; i<smoothlevels; i++) // generate the z-smoothing-box
    // {
    //     Cell *newClonedCell = new Cell();
    //     origCell->cloneCell(newClonedCell);
    //     while (!newClonedCell->childCells.empty()) delete newClonedCell->childCells.last(), newClonedCell->childCells.removeLast();
    //     cellPtr->childCells.append(newClonedCell);
    //     GeometryAlgorithms::scaleCell(newClonedCell, 0.6, 0.6, 1.4);
    //     origCell = newClonedCell;
    // }

    return cellPtr;
}

Cell *AlienModeller::explodeCell(Cell *cellPtr, float distanceFromCorner, QVector3D xyzScaleVals, int numOfExplosions)
{
    QVector3D cellCntrPnt = cellPtr->getCellCenterLocation();
    int tempCnt = 0;
    if (numOfExplosions < 0  ||  numOfExplosions > cellPtr->cellBox->vertices.length())
        numOfExplosions = cellPtr->cellBox->vertices.length();
    QVector<QVector3D> copyOfVertices = QVector<QVector3D>(cellPtr->cellBox->vertices);
    for (int i=0; i<copyOfVertices.length() && !copyOfVertices.isEmpty(); i++) // shuffle all the elements
    {
        int randInd = GeometryAlgorithms::getRandomInteger(i, copyOfVertices.length());
        copyOfVertices.prepend(copyOfVertices.at(randInd));
        copyOfVertices.removeAt(randInd+1);
        qDebug() << randInd;
    }
    for (QVector3D vertex : copyOfVertices)
    {
        if (tempCnt < numOfExplosions)
        {
            QVector3D tempVec = cellCntrPnt - vertex;
            QVector3D newPnt = vertex + distanceFromCorner*tempVec;

            Cell *newClonedCell = new Cell();
            cellPtr->cloneCell(newClonedCell);
            // make sure to not clone any of it's child cells
            while (!newClonedCell->childCells.empty()) delete newClonedCell->childCells.last(), newClonedCell->childCells.removeLast();
            cellPtr->childCells.append(newClonedCell);
            GeometryAlgorithms::scaleCell(newClonedCell, xyzScaleVals.x(), xyzScaleVals.y(), xyzScaleVals.z());
            GeometryAlgorithms::translateCell(newClonedCell, newPnt - newClonedCell->getCellCenterLocation());
            GeometryAlgorithms::rotateCellUsingEulerAngles(newClonedCell, GeometryAlgorithms::getRandomInteger(-30,30),
                                                           GeometryAlgorithms::getRandomInteger(-30,30), 0);
            tempCnt++;
        }
    }

    return cellPtr;
}

void AlienModeller::embedMoreJointsInBP(Alien *alienPtr, BodyPart *bpPtr, int numOfJntsToEmbed, bool regularOrIrregularSpacing)
{
/*
 *  ****** ALGORITHM *******
 *
 *  Go through all the bones of the BP
 *
 *  call subdivideBoneIntoSmallerBones()
 *
 *  update all the relevant DNAs and other datastructures of skeleton (Jnt2JntRel) and Alien that contains references, IDs
 *  ... or pointers to Bones and Jnts
 *
 *  Remodel all the Jnts and Bones
 *
 *  Rebulify and redensify
 *
 *  reattach to BB
 *
 *
*/
}

QPolygon *AlienModeller::subdivideBoneIntoSmallerBones(Alien *alienPtr, Bone *bonePtr, int numOfJntsToEmbed, QVector<float> percOfVecForNewJntsLoc)
{
/*
 *  *********** ALGORITHM ***********
 *
 *  Store the first joint of this Bone in a 'tempList' of joint IDs
 *  Go through the percOfVecForNewJntsLoc list
 *      Find a new point according to the percentage value in percOfVecForNewJntsLoc
 *      Instantiate a new Jnt at this location and store its location and vector in the
 *      ... tempVectorLocationCoupleForEachJoint_TypeGuided DS of Skeleton
 *      Update the JATs according to the energy and angle of this newly created Joint
 *      Update other DNA DS accordingly
 *
 *      append this joint ID in the tempList
 *
 *
 *  Append the second joint of the Bone to the tempList
 *  create a Jnt2JntRel list and store it in the QPolygon to be returned
 *  Remove the bone from all over its usage in the code including DS
 *
 *
 *
 *
*/
//    BodyPart *parentBP_Ptr = getBP_containingThisBone(alienPtr, bonePtr->BP_ID);
//    int firstJntID = bonePtr->endJoints.first;
//    int seconfJntID = bonePtr->endJoints.second;

//    BodyPart *firstJntPtr = getBodyPartFromID(alienPtr, firstJntID);
//    BodyPart *secondJntPtr = getBodyPartFromID(alienPtr, firstJntID);
//    QVector3D tempVec = secondJntPtr->getPivotPoint() - firstJntPtr->getPivotPoint();
//    QVector3D tempLoc = firstJntPtr->getPivotPoint();

//    if (percOfVecForNewJntsLoc.length() == numOfJntsToEmbed)
//    {
//        for (int i=0; i<percOfVecForNewJntsLoc.length(); i++)
//        {
//            // Instantiate a new Joint and make it a subBP of the parentBP
//            parentBP_Ptr->subBodyParts.append(this->instantiateNewJoint(alienPtr));
//            QVector3D newJntLoc = tempLoc + ((percOfVecForNewJntsLoc[i]/100.0) * tempVec);


//        }
//    }






/*
    QPolygon *newJnt2JntLineageExtractPtr=nullptr;

    // Remove the legacy parent-child relationship from the Jnt2JntRelations DS
    // ... and insert the new joint relations in the list
    // Also remove the bone
    // TODO(Ismail): Also remove the bone from the AlienBones and bodyParts
    int insertionIndex=0;
    for (insertionIndex=0; insertionIndex<alienList.last()->alienSkeleton.Jnt2JntRelations_TypeGuided.value(bonePtr->BP_ID)->length(); insertionIndex++)
    {
        int firstJntID = alienList.last()->alienSkeleton.Jnt2JntRelations_TypeGuided.value(bonePtr->ContainingBP_ID)->at(insertionIndex).x();
        int secondJntID = alienList.last()->alienSkeleton.Jnt2JntRelations_TypeGuided.value(bonePtr->ContainingBP_ID)->at(insertionIndex).y();
        if (bonePtr->endJoints.first == firstJntID  &&  bonePtr->endJoints.second == secondJntID)
        {
            // first remove the old joint relation
            alienList.last()->alienSkeleton.Jnt2JntRelations_TypeGuided.value(bonePtr->ContainingBP_ID)->remove(insertionIndex);
            break;
        }
    }

    int ContainingBP_ID = bonePtr->ContainingBP_ID;
    int tempJnt1 = bonePtr->endJoints.first;
    int tempJnt2 = bonePtr->endJoints.second;

    // Now remove the bone and insert new bones
    if (removeBone(bonePtr))
    {
        int firstJnt, secJnt;
        // insert the new joints' relations in Jnt2Jnt relations DS
        for (int i=0; i<inputPts.length()-1; i++, insertionIndex++)
        {
            // create a new Joint at this location and insert it in the bone
            Joint *newJnt = instantiateNewJoint();
            newJnt->makeCell(inputPts[i+1],1);

            if (i==0)
            {
                firstJnt = tempJnt1;
                secJnt = newJnt->BP_ID;
            }
            else if (i==inputPts.length()-2)
            {
                firstJnt = newJnt->BP_ID;
                secJnt = tempJnt2;
            }
            else
            {
                firstJnt = secJnt;
                secJnt = newJnt->BP_ID;
            }

            // insert the new joint relations in Jnt2Jnt relations DS
            this->alienList.last()->alienSkeleton.Jnt2JntRelations_TypeGuided.value(ContainingBP_ID)->insert(insertionIndex,QPoint(firstJnt,secJnt));
            newJnt2JntLineageExtractPtr->append(QPoint(firstJnt,secJnt));

            Bone *newBonePtr = instantiateNewBone();
            newBonePtr->ContainingBP_ID = ContainingBP_ID;
            createSimpleBone(*newBonePtr, inputPts[i], inputPts[i+1]);
        }

        qDebug() << "Jnt2Jnt Pair removed new joints' relations inserted";
    }

    smoothLineageFromSkeleton(newJnt2JntLineageExtractPtr, 2);

    return newJnt2JntLineageExtractPtr;
*/
}


// All seed joints are modelled using a box.
void AlienModeller::generateSeedJoints()
{
/*
     algorithm:
        - loop goes from 0 to 4
            - create a new joint and create a new cell
            - select a face at random which has not been used before
            - make a sub cell which is somewhere on that face
            - append the seedJoints list with the new Joint
*/

    for (int i=0; i<4; i++) // i goes from 0th to 4th face
    {
        QVector<QVector3D> points;
        points.resize(0);
        QVector<QVector3D> tempFaceVertices;
        tempFaceVertices.resize(0);

        Joint tempJnt;
        tempJnt.makeCell(QVector3D(0,0,0), 50);

        // prepare the tempFaceVertices list
        for (int j=0; j<tempJnt.cellList[0]->cellBox->faces[i]->vertNorIndices.length()-1; j++)
        {
            tempFaceVertices.append(tempJnt.cellList[0]->cellBox->vertices[tempJnt.cellList[0]->cellBox->faces[i]->vertNorIndices[j]]);
//            qDebug () << tempFaceVertices[j];
        }
        computeRandomPointsOnFace(tempFaceVertices, 1, points);

        // Now generate the cells on all the random points..in this case only 1 cell
        for (int k=0; k<points.length(); k++)
        {
            tempJnt.makeCell(points[k], 20);
        }
        seedJoints.append(tempJnt);
    }
}


// void AlienModeller::modelNewAlienUsingCells()
// {
//     Alien *newAlien = new Alien();

//     // Level 1 modelling ==> model the upper torso, lower torso, arms spawn locations, fins spawn locations, head spawn locations, legs spawn locations
//     // Level 2 modelling ==> model arms, fins, legs, backbone, head, exoskeletal features
//     // Level 3 modelling ==> model thorns, horns, mushrooms, spikes, fungal growth, tentacles, boils, cavities, other abnormalities.

//     // ** Idea: the transforms of the children cells are parented to their parent's cell's transform so when the
//     // parent cell rotates or translates, the children rotate or translate with it.


//     // ****** Leve 1 modelling ********

//     // create the root cell, assign its location=origin, assign it an energy (high value), and make its box
//     Cell *rootCellPtr = new Cell();
//     rootCellPtr->cellEnergy=100.0f; // Starting energy: Can be altered based on requirements. Experiment with different energies eg 100
//     rootCellPtr->cellCenterLocation=QVector3D(0.0,0.0,0.0);
//     rootCellPtr->makeBox(); // perhaps think of a better value for distance

//     int depth=0; // recursive tree depth

//     // Now create all the 6 children of the cell
//     createChildCells(rootCellPtr,depth);

//     newAlien->rootCellPtr = rootCellPtr;

//     alienList.append(newAlien);
// }

// recursive method
void AlienModeller::createChildCells(Cell *cellPtr, int depth)
{
    // check if we need to create any more children
    if (depth < 5)
    {
        depth++;
        qDebug() << "depth : " << depth;

        // Now create all the 6 children of the cell

        /* initialize random seed: */
//        srand (time(NULL));
        int tempRandParam;
        GLint tempEnergy = cellPtr->cellEnergy;

        for (int i=0; i<6; i++)
        {
            Cell *childCellPtr = new Cell();

            // Assign each cell a location (in the direction of the parent's normal vectors one by one) and energy.
            // Here the normal vector is a unit vector.
            // How far to move in the direction of normal vector is randomly done using the random parameter
            // use parametric equation of line to find the new point (this can be easily achieved using QVector3D)

            // find the value of param using randomization. The param should depend upon the energy
            /* generate a random number between 0 and 100, representing the value of parameter: */
            GLint tempdist = 200/100 * tempEnergy;
            tempRandParam = (rand() % tempdist)+tempdist; // *** analyse this ... whether it will do the trick.
                                                // This distance should depend on the distance this cell is from its parent cell.
                                                // try 250% of the cell's energy

            childCellPtr->cellCenterLocation = cellPtr->cellCenterLocation + (tempRandParam * cellPtr->cellBox->normals[i]);


            // Calculate the child's energy according to some formula (energy = E_rootCell - SUM(E_childrenCells)). This can be any other viable formula
            // Each child cell will have a percentage of energy of the parent cell. Thus, all the children cells' energies
            // will add up to form the parent's energy

    // This portion needs fixing because some boxes are too small/large and perhaps the energy hasn't been distributed properly

//            tempRandParam = rand() % tempEnergy;         // generate a random number between 0 and 80 representing percentage
//            if (i<5)
//                childCellPtr->cellEnergy = (tempRandParam/100.0f)*tempEnergy;
//            else
//                childCellPtr->cellEnergy = tempEnergy;

//            tempEnergy = tempEnergy - childCellPtr->cellEnergy;
//            qDebug() << tempEnergy;


                childCellPtr->cellEnergy = tempEnergy/2;

                        qDebug() << tempEnergy;

    // The above portion needs fixing

            childCellPtr->makeBox();

            cellPtr->childCells.append(childCellPtr);
        }

        // now undergo recursion for each child to divide it further. Looks like depth first traversal?
        for (int i=0; i<6; i++)
        {
            createChildCells(cellPtr->childCells[i], depth);
        }
    }
    else
        return;
}

void AlienModeller::Stage1_Modelling()
{
/*
 *   ++++ Rotation tests using quaternions ++++
 *
 *  helpful cues:
 *          - different color for each face
 *          - render the normal vectors
 *          - render the X,Y,Z axes
*/

    Alien *newAlien = new Alien();

    makeAlienRuleSets(newAlien);

//    modelBP_ThighBone(newAlien);
    // model other level 0 features
    // model level 1 features
    // model level 2 features
    // model head(s)

    alienList.append(newAlien);
}

/*
void AlienModeller::modelBP_ThighBone(Alien *alien)
{
    BodyPart thighBoneBP;

//    thighBoneBP.BP_Name = "THIGHBONE";

    // generate a cell for this bone
    thighBoneBP.makeCell(QVector3D(0.0,0.0,0.0), 100.0f);
    // scale the cell so it looks like a femur bone (according to the ruleset).
    // Secondary objective: Design algorithms for sculpting the different bones/level_0 features so they look more realisitc
    GeometryAlgorithms::scaleCell(&thighBoneBP.cellList[0], 1, 1, 4);
    thighBoneBP.cellList[0].setPivotPtLocation(0);

    // Run one or more of the following algorithms (randomly depending on some criteria)
    generateCellBranchings(thighBoneBP.cellList[0], alien);

    // specify placeholders
    // generate placeholders
    // rearrange placeholders
    // attach level 1 bp to placeholders

    qDebug () << "branches: " << thighBoneBP.cellList[0].childCells.length();

    alien->bodyParts.append(thighBoneBP);
}
*/

void AlienModeller::generateCellBranchings(Cell *cell, Alien *alien)
{
    int ptsCnt =    (rand() % (alien->ruleSetList[0].maxBranchings - alien->ruleSetList[0].minBranchings))
                    + alien->ruleSetList[0].minBranchings;

    int branchPerFace=0;
    QVector<QVector3D> points;
    // First go through all the faces, and pick up random points
    for (int i=0; i<cell->cellBox->faces.length(); i++)
    {
            // Decide which faces to choose for branching (based on some criteria or ruleset)
            if (!(alien->ruleSetList[0].facesToIgnore.contains(i)))
            {
                if (i == cell->cellBox->faces.length()-1)
                    branchPerFace = ptsCnt;
                else
                    branchPerFace = (rand() % ptsCnt);

//                qDebug () << branchPerFace;

                // prepare the tempFaceVertices list
                QVector<QVector3D> tempFaceVertices;
                tempFaceVertices.resize(0);
                for (int j=0; j<cell->cellBox->faces[i]->vertNorIndices.length()-1; j++)
                {
                    tempFaceVertices.append(cell->cellBox->vertices[cell->cellBox->faces[i]->vertNorIndices[j]]);
//                    qDebug () << tempFaceVertices[j];
                }
                points.resize(0);
                computeRandomPointsOnFace(tempFaceVertices, branchPerFace, points);


                // Now generate the cells on all the random points
                for (int k=0; k<points.length(); k++)
                {
                    // TODO(Ismail):
                    //                  1 - the size (scale) of the branches should be decided according to the size of its parent cell/BP

                    qDebug () << "random point: " << points[k];
                    Cell *tempCell = new Cell();
                    tempCell->cellEnergy=20;
            //        tempCell->cellLocation.setX(points[i].x()); tempCell->cellLocation.setY(points[i].y()); tempCell->cellLocation.setZ(points[i].z());
                    tempCell->cellCenterLocation.setX(0.0); tempCell->cellCenterLocation.setY(0.0); tempCell->cellCenterLocation.setZ(0.0);
                    tempCell->makeBox();
                    tempCell->setPivotFace(Box::BoxFace::BACK);

                    //
                    // OBSERVATION(ismail): The following order of transformations is producing the desired result (scale > rotate > translate)
                    //
                    GeometryAlgorithms::scaleCell(tempCell, 1, 1, 10);
                    // Need to rotate the cell according to a random protruding vector from the given point on the face
                    GeometryAlgorithms::rotateCellTowardsVecDir(tempCell, QVector3D((rand() % 90),(rand() % 90),(rand() % 90)));
//                        GeometryAlgorithms::rotateCellTowardsVecDir(tempCell, cell.cellBox->normals[i]);
                    GeometryAlgorithms::translateCell(tempCell, points[k]);

                    // TODO(Ismail):
                    //                  1 - branches coming out of RIGHT face (and maybe some other faces) are rotated inwards into the box rather than outwards, so fix this
                    //                  2 - need to create more methods that encapsulate the transfomation code in order to make the code more readable
                    //                      and follow the algorithm

                    cell->childCells.append(tempCell);
                }

            ptsCnt -= branchPerFace;
        }
    }
}

// ++++ The following Algorithm finds random points on a face.
/*  Input: A face (a list of its vertices), minimum number of Pts
 *  Output: array of random points on face
*/
void AlienModeller::computeRandomPointsOnFace(QVector<QVector3D> &tempFaceVertices,
                                              int numOfBranch, QVector<QVector3D> &points)
{
    if (numOfBranch != 0)
    {
        // Get the midpoint of the face diagonal. It will serve as the face center point
        QVector3D centerPt = tempFaceVertices[0] + (0.5 * (tempFaceVertices[2] - tempFaceVertices[0]));

        // k goes through face vertices in cc-wise fashion. k starts at a random vertex index
        for (int j=0,k=(rand() % 2); j<numOfBranch; j++,k++)
        {
            if (k==3) // go back to the first vertex
                k=0;

            QVector3D tempPt = tempFaceVertices[k] +
                    ( ((rand() % 100)/100.0) * (tempFaceVertices[k+1] - tempFaceVertices[k]) );

            // finally calculate the random point
            QVector3D randomPt = centerPt + (((rand() % 100)/100.0) * (tempPt - centerPt));
            points.append(randomPt);

            // cycle to the next edge
        }
    }
}

BodyPart *AlienModeller::getClosestBPFromPoint(QVector3D inputPt, QVector<BodyPart *> *inputBPsList)
{
    for (int i=0; i<inputBPsList->length(); i++)
    {
        for (int j=0; j<inputBPsList->at(i)->subBodyParts.length(); j++)
        {
            if (inputBPsList->at(j)->BP_Type == AlienModeller::BodyPartsEnum::JOINT  ||  inputBPsList->at(j)->BP_Type == AlienModeller::BodyPartsEnum::BONE)
            {
                // go through all its cells and check which ones inputPt intersects
                for (int j=0; j<inputBPsList->at(i)->cellList.length(); j++)
                {
                    if (inputBPsList->at(i)->cellList[j]->cellBox->doesMouseHitThisBox(inputPt))
                    {
                        qDebug() << "BP detected for mouse picking";
                        return inputBPsList->at(i);
                    }
                }
            }
        }
    }
    qDebug() << "No BP detected for mouse picking";
    return nullptr;

//    for (int i=0; i<inputBPsList->length(); i++)
//    {
//        for (int j=0; j<inputBPsList->at(i)->subBodyParts.length(); j++)
//        {
//            if (inputBPsList->at(j)->BP_Type == AlienModeller::BodyPartsEnum::JOINT  ||  inputBPsList->at(j)->BP_Type == AlienModeller::BodyPartsEnum::BONE)
//            {
//                // go through all its cells and check which ones inputPt intersects
//                for (int j=0; j<inputBPsList->at(i)->cellList.length(); j++)
//                {
//                    if (inputBPsList->at(i)->cellList[j].cellBox->doesMouseHitThisBox(inputPt))
//                    {
//                        qDebug() << "BP detected for mouse picking";
//                        return inputBPsList->at(i);
//                    }
//                }
//            }
//        }
//    }
//    qDebug() << "No BP detected for mouse picking";
    //    return nullptr;
}

BodyPart *AlienModeller::getBP_PickedByMouse(QVector<BodyPart *> &bpList, QVector3D inputPt, QPair<BodyPart *, Cell *> &BP_Cell_Pair)
{
    for (BodyPart *bpPtr : bpList)
    {
        if (bpPtr->BP_Type != AlienModeller::BodyPartsEnum::JOINT  &&  bpPtr->BP_Type != AlienModeller::BodyPartsEnum::BONE)
        {
            for (BodyPart *subBP_Ptr : bpPtr->subBodyParts)
            {
                if (subBP_Ptr->BP_Type == AlienModeller::BodyPartsEnum::JOINT  ||  subBP_Ptr->BP_Type == AlienModeller::BodyPartsEnum::BONE)
                {
                    QVector<Cell *> tempCellList; tempCellList.clear();
                    subBP_Ptr->getBodyPartCellList(tempCellList);
                    if (subBP_Ptr->BP_Type == AlienModeller::BodyPartsEnum::BONE)
                    {
                        Bone *bnPtr = dynamic_cast<Bone*>(subBP_Ptr);
                        if (!bnPtr->subdivisionCells.isEmpty())
                            tempCellList.removeFirst();
                    }
                    for (Cell *cellPtr : tempCellList)
                    {
                        if (checkCellForMousePicking(inputPt, cellPtr))
                        {
                            BP_Cell_Pair.first = bpPtr;
                            BP_Cell_Pair.second = cellPtr;
                            return bpPtr;
                        }
                    }
                }
            }
        }
    }

    return NULL;


/*
    for (int i=0; i<rootBP->cellList.length(); i++) // checkCellForHit() recursive method
    {
        if (checkCellForMousePicking(inputPt, rootBP->cellList[i]))
        {
            qDebug() << "picked";
            return rootBP;
        }
    }

    for (int i=0; i<rootBP->subBodyParts.length(); i++)
    {

        if (getBP_PickedByMouse(inputPt, rootBP->subBodyParts[i]))
            return rootBP->subBodyParts[i];

    }

    return NULL;
*/
}

bool AlienModeller::checkCellForMousePicking(QVector3D inputPt, Cell *cellPtr)
{
    if (cellPtr->cellBox->doesMouseHitThisBox(inputPt))
    {
        return true;
    }

    for (int i=0; i<cellPtr->childCells.length(); i++)
    {
        if (checkCellForMousePicking(inputPt, cellPtr->childCells[i]))
            return true;
    }

    return false;
}

Joint *AlienModeller::getJointFromID(Alien *alien, int BP_ID)
{
    for (int k=0; k<alien->alienJoints.length(); k++)
    {
        if (BP_ID == alien->alienJoints[k]->BP_ID)
        {
            return alien->alienJoints[k];
        }
    }
    return nullptr;
}

Bone *AlienModeller::getBoneFromID(Alien *alien, int BP_ID)
{
    for (int k=0; k<alien->alienBones.length(); k++)
    {
        if (BP_ID == alien->alienBones[k]->BP_ID)
        {
            return alien->alienBones[k];
        }
    }
    return nullptr;
}

void AlienModeller::convertJnt2JntPolyToJointsLineageList(QPolygon *Jnt2JntRelPtr, QVector<int> &jntLineageList)
{
    for (int i=0; i<Jnt2JntRelPtr->length(); i++)
    {
        if (i==0)
        {
            jntLineageList << Jnt2JntRelPtr->at(i).x();
            jntLineageList << Jnt2JntRelPtr->at(i).y();
        }
        else
        {
            jntLineageList << Jnt2JntRelPtr->at(i).y();
        }
    }
}

BodyPart *AlienModeller::getBodyPartFromID(Alien *alien, int BP_ID)
{
    BodyPart *bpPtr = nullptr;
    for (int k=0; k<alien->bodyParts.length(); k++)
    {
        if (BP_ID == alien->bodyParts[k]->BP_ID)
        {
            bpPtr = alien->bodyParts[k];
            return bpPtr;
        }
    }
    for (int k=0; k<alien->seperateCloneBPs.length(); k++)
    {
        if (BP_ID == alien->seperateCloneBPs[k]->BP_ID)
        {
            bpPtr = alien->seperateCloneBPs[k];
            return bpPtr;
        }
    }
    qDebug() << "BodyPart not found in the Alien";
    return nullptr;
}

ExtremityBP *AlienModeller::getExtremityBP_FromID(Alien *alienPtr, int extremID)
{
    for (ExtremityBP *ePtr : alienPtr->alienExtremities)
        if (extremID == ePtr->BP_ID)
            return ePtr;

    qDebug() << "Extremity BodyPart not found in the Alien";
    return nullptr;
}

void AlienModeller::getAllCarpalsAndDigitsIDsOfBP(Alien *alienPtr, int BP_ID, QVector<int> &carpalDigitsIDs)
{
    BodyPart *mainBP = getBodyPartFromID(alienPtr, BP_ID);
    if (mainBP)
    {
        for (int subExtID : mainBP->subExtremityBPsIDs)
        {
            ExtremityBP *subExtPtr = getExtremityBP_FromID(alienPtr, subExtID);
            if (subExtPtr)
            {
                for (BodyPart *carpal : subExtPtr->extremBP_basePtr->extremBP_BaseCarpals)
                {
                    if (!carpalDigitsIDs.contains(carpal->BP_ID))
                        carpalDigitsIDs << carpal->BP_ID;
                }
                for (BodyPart *digit : subExtPtr->extremBP_Digits)
                {
                    if (!carpalDigitsIDs.contains(digit->BP_ID))
                        carpalDigitsIDs << digit->BP_ID;
                }
            }
        }
    }
}

BodyPart *AlienModeller::getParentOfThisBP(Alien *alien, BodyPart *inBP_Ptr, ExtremityBP *exBP_Ptr)
{
    if (exBP_Ptr)
    {
        for (BodyPart *bpPtr : alien->bodyParts) // TODO: Alternatively, Use extremToParentBP_Map defined inside Alien class
        {
            if (bpPtr)
                if (bpPtr->subExtremityBPsIDs.contains(exBP_Ptr->BP_ID))
                    return bpPtr;
        }
    }
    else if (inBP_Ptr)
    {
        for (BodyPart *bpPtr : alien->bodyParts)
        {
            if (bpPtr != nullptr  &&    bpPtr != inBP_Ptr)
                if (bpPtr->subBodyParts.contains(inBP_Ptr))
                    return bpPtr;
        }
    }
    qDebug() << "Parent BodyPart of this BP not found";
    return nullptr;
}

QMap<int,int> AlienModeller::getChildParentRelationsInMap(Alien *alienPtr, bool includeMirror)
{
    QMap<int,int> childParentMap; childParentMap.clear();
    QVector<int> BP_TypesToIgnore; BP_TypesToIgnore.clear();
    BP_TypesToIgnore    << AlienModeller::BodyPartsEnum::JOINT
                        << AlienModeller::BodyPartsEnum::BONE;
    for (BodyPart *bp : alienPtr->bodyParts)
    {
        if (!BP_TypesToIgnore.contains(bp->BP_Type))
        {
            if (!includeMirror)
            {
                if (bp->isSymmetric) // Exclude any mirrored BPs
                    continue;
            }
            BodyPart *parent = getParentOfThisBP(alienPtr, bp, nullptr);
            if (parent)
                childParentMap.insert(bp->BP_ID, parent->BP_ID);
            else
                childParentMap.insert(bp->BP_ID, -1);
        }
    }
    return childParentMap;
}

QMap<QPair<int, int>, QList<int> > AlienModeller::getExtremityAndParentToCarpalsAndDigitsMap(Alien *alienPtr, bool includeMirrorAndTheirExtremities)
{
    QMap<QPair<int, int>, QList<int> > parentExtremMap; parentExtremMap.clear();
    QVector<int> BP_TypesToIgnore; BP_TypesToIgnore.clear();
    BP_TypesToIgnore    << AlienModeller::BodyPartsEnum::JOINT
                        << AlienModeller::BodyPartsEnum::BONE
                        << AlienModeller::BodyPartsEnum::ExtremBP_Carpal
                        << AlienModeller::BodyPartsEnum::ExtremBP_Digit;

    for (BodyPart *bp : alienPtr->bodyParts)
    {
        if (!BP_TypesToIgnore.contains(bp->BP_Type))
        {
            if (!includeMirrorAndTheirExtremities)
            {
                if (bp->isSymmetric) // Ignore mirrored BPs. This will automatically ignore all the extremities
                    continue;        // .. attached to the mirrored BPs
            }
            // find its extremities...
            for (int extID : bp->subExtremityBPsIDs)
            {
                ExtremityBP *extPtr = getExtremityBP_FromID(alienPtr, extID);
                if (extPtr)
                {
                    // Optional: First we need to detach the child carpals from the root carpal
                    extPtr->detachSubCarpalsFromRootCarpal();
                    QList<int> subCarpDigitsIDs; subCarpDigitsIDs.clear();
                    QVector<BodyPart*> digits, carpals; digits.clear(); carpals.clear();
                    extPtr->getAllCarpalsAndDigits(digits, carpals);
                    for (BodyPart *car : carpals)
                        if (!subCarpDigitsIDs.contains(car->BP_ID))
                            subCarpDigitsIDs << car->BP_ID;
                    for (BodyPart *dig : digits)
                        if (!subCarpDigitsIDs.contains(dig->BP_ID))
                            subCarpDigitsIDs << dig->BP_ID;
                    // getAllCarpalsAndDigitsIDsOfBP(alienPtr, bp->BP_ID, subCarpDigitsIDs);
                    parentExtremMap.insert(QPair<int,int>(extID,bp->BP_ID), subCarpDigitsIDs);
                }
            }
        }
    }

    return parentExtremMap;
}

BodyPart *AlienModeller::getBP_containingThisBone(Alien *alien, int boneID)
{
    for (BodyPart *bpPtr : alien->bodyParts)
    {
        for (BodyPart *subBP_Ptr : bpPtr->subBodyParts)
        {
            if (subBP_Ptr->BP_ID == boneID)
            {
                return bpPtr;
            }
        }
    }
    qDebug() << "BodyPart containing this bone not found";
    return nullptr;
}

void AlienModeller::updateGLW_to_Alien_to_BP_to_ColorDisplay_Map(Alien *alienPtr, int GLW_ID, QVector<int> BP_ListToDisplay, QVector<int> BP_ListToDisplayInDiffColor, int diffColor)
{

    // TODO(ismail): Some house cleaning:
    // First remove any aliens that are not present in the system at all. Eg, those that were deleted
    // TODO(ismail): Maintain a set of DSs in AlienModeller/or a seperate class that will keep track of
    // ... which aliens and BPs are no more in the system so they need to be deleted from the DNAs
    // ... and other relevant DSs

    QMap<int,QPair<bool, int>> tempMap_1; tempMap_1.clear();
    // First insert all the BP entries
    for (int bpID: BP_ListToDisplay)
    {
        BodyPart *tempBP = getBodyPartFromID(alienPtr, bpID);
        if (tempBP)
        {
            int bpTypeID = tempBP->BP_Type;
            if (!alienPtr->BP_TypeColorMap_2.contains(bpTypeID))
                alienPtr->BP_TypeColorMap_2.insert(bpTypeID,colorMap_2.key(colorMap.value(bpTypeID)));
            tempMap_1.insert(bpID, QPair<bool,int>(true,alienPtr->BP_TypeColorMap_2.value(bpTypeID)));
        }
    }

    if (GLW_to_Alien_to_BP_to_ColorAndDisplay_Map.contains(GLW_ID))
    {
        // We need to ensure that only those BPs should be displayed
        // ... that have the displayable value as true
        if (!tempMap_1.isEmpty())
            GLW_to_Alien_to_BP_to_ColorAndDisplay_Map[GLW_ID].insert(alienPtr->alienID, tempMap_1);
    }
    else
    {
        QMap<int, QMap<int,QPair<bool, int>>> tempMap_2; tempMap_2.clear();
        tempMap_2.insert(alienPtr->alienID, tempMap_1);
        GLW_to_Alien_to_BP_to_ColorAndDisplay_Map.insert(GLW_ID, tempMap_2);
//        tempMap_2.clear();
    }

    // Update the BP entries whose color need to be changed
    for (int i=0; i<BP_ListToDisplayInDiffColor.length(); i++)
    {
        if (GLW_to_Alien_to_BP_to_ColorAndDisplay_Map.value(GLW_ID).value(alienPtr->alienID).contains(BP_ListToDisplayInDiffColor[i])) // Now change its color
            GLW_to_Alien_to_BP_to_ColorAndDisplay_Map[GLW_ID][alienPtr->alienID][BP_ListToDisplayInDiffColor[i]].second = diffColor;
        else
            GLW_to_Alien_to_BP_to_ColorAndDisplay_Map[GLW_ID][alienPtr->alienID].insert(BP_ListToDisplayInDiffColor[i],QPair<bool,int>(true, diffColor));
    }

    tempMap_1.clear();

    qDebug() << "updating of GLW_to_Alien_to_BP_to_ColorAndDisplay_Map done";
}

int AlienModeller::getGLW_ID_ThatDisplaysThisAlienAndBP(int alienID, int BP_ID, QVector<int> &GLW_List)
{
    while (!GLW_List.isEmpty())
    {
        if (GLW_to_Alien_to_BP_to_ColorAndDisplay_Map.value(GLW_List.first()).value(alienID).contains(BP_ID))
        {
            qDebug() << "found 1";
            if (GLW_to_Alien_to_BP_to_ColorAndDisplay_Map.value(GLW_List.first()).value(alienID).value(BP_ID).first) // found to be rendered in this GLW
            {
                qDebug() << "found 2";
                return GLW_List.first();
            }
        }

        GLW_List.removeFirst();
    }
    return -1;
}

QVector3D AlienModeller::getBP_Color(int GLW_ID, Alien *alienPtr, int BP_ID)
{
    int finalColorIndex = 12; // if entry not found in map, then a default color is returned
    if (this->GLW_to_Alien_to_BP_to_ColorAndDisplay_Map.contains(GLW_ID))
    {
        if (GLW_to_Alien_to_BP_to_ColorAndDisplay_Map.value(GLW_ID).contains(alienPtr->alienID))
        {
            if (GLW_to_Alien_to_BP_to_ColorAndDisplay_Map.value(GLW_ID).value(alienPtr->alienID).contains(BP_ID))
            {
                finalColorIndex = GLW_to_Alien_to_BP_to_ColorAndDisplay_Map.value(GLW_ID).value(alienPtr->alienID).value(BP_ID).second;
                return this->colorMap_2.value(finalColorIndex);
            }
        }
    }
    qDebug() << "BP not found in the GLW_to_Alien_to_BP_to_ColorDisplay_Map";
    return this->colorMap_2.value(finalColorIndex);
}

BodyPart *AlienModeller::getBP_ThatWasClonedFromGivenBP(Alien *clonedAlien, BodyPart *srcBP_Ptr)
{
    for (BodyPart *bp : clonedAlien->bodyParts)
    {
        if (bp->cloneFromAlienAndBP_Pair == QPair<int,int>(srcBP_Ptr->parentAlienID, srcBP_Ptr->BP_ID))
            return bp;
    }

    for (BodyPart *bp : clonedAlien->seperateCloneBPs)
    {
        if (bp->cloneFromAlienAndBP_Pair == QPair<int,int>(srcBP_Ptr->parentAlienID, srcBP_Ptr->BP_ID))
            return bp;
    }

    return nullptr;
}

BodyPart *AlienModeller::getOriginalBP_FromClonedBP(BodyPart *clonedBP_Ptr)
{
    return getBodyPartFromID(getAlienFromID(clonedBP_Ptr->cloneFromAlienAndBP_Pair.first), clonedBP_Ptr->cloneFromAlienAndBP_Pair.second);
}

// This mehtod returns the orginal BP if the input BP is a mirror OR the mirror of the input BP
BodyPart *AlienModeller::getMirrorOrOriginalBP(Alien *aPtr, BodyPart *inputBP_Ptr)
{
    if (!inputBP_Ptr->isSymmetric) // return its mirror
    {
        // TODO(ismail): Ensure that there is only one mirror of a BP. A BP can have either 0 or exactly 1 mirror
        for (BodyPart *bp : aPtr->bodyParts)
        {
            if (bp->mirrorOf == inputBP_Ptr->BP_ID)
            {
                qDebug() << "mirror found so returning the mirror";
                return bp;
            }
        }
        return nullptr; // if the inputBP doesn't have a mirror, then simply return a nullptr
    }
    else // return its original BP
    {
        qDebug() << "This is a mirror so returning its original BP";
        return getBodyPartFromID(aPtr, inputBP_Ptr->mirrorOf);
    }
}

void AlienModeller::computeFinalEnergyAndAngleForBodyPartJointsFromJnt2JntRelations(Alien *alien, BodyPart *bpPtr)
{

}

void AlienModeller::getAllBPsCellListForRendering(QVector<BodyPart *> &bpList, QVector<QVector<Cell*>> &tempCellMatrix, QMap<int, int> &tempCellListToBP_Map__forColoring)
{
    tempCellMatrix.resize(0);

    Alien *alienPtr = getAlienFromID(bpList.first()->parentAlienID);
    if (!alienPtr)
        exit(0);
    for (BodyPart *bpPtr : bpList)
    {
        QVector<Bone*> bonesInOrder; bonesInOrder.clear();
        getAllBP_BonesInOrder(alienPtr, bpPtr, bonesInOrder);
        BodyPart *endJntPtr = nullptr;
        if (!bonesInOrder.last()->subdivisionCells.isEmpty())
        {
            endJntPtr = getBodyPartFromID(alienPtr, bonesInOrder.last()->endJoints.second);
            if (!endJntPtr)
                exit(1);
        }
        if (bpPtr->BP_Type != AlienModeller::BodyPartsEnum::JOINT  &&  bpPtr->BP_Type != AlienModeller::BodyPartsEnum::BONE)
        {
            QVector<Cell*> tempCellList; tempCellList.resize(0);
            if (bpPtr->BP_Type == AlienModeller::BodyPartsEnum::HEAD)
            {
                Head *headPtr = dynamic_cast<Head*>(bpPtr);
                for (QVector<Cell*> clPtr : headPtr->allCellListLineages)
                {
                    if (!clPtr.isEmpty())
                        tempCellList.append(clPtr);
                }
                if (!headPtr->allBridgesCells.isEmpty())
                    tempCellList.append(headPtr->allBridgesCells);
                if (!headPtr->allLoneCells.isEmpty())
                    for (Cell *lCell : headPtr->allLoneCells)
                        lCell->getAllChildCellsRecursively(tempCellList);
            }

            for (BodyPart *subBP : bpPtr->subBodyParts)
            {
                if (endJntPtr  &&  subBP==endJntPtr)
                    continue;
                if (subBP->BP_Type == AlienModeller::BodyPartsEnum::JOINT
                        ||  subBP->BP_Type == AlienModeller::BodyPartsEnum::BONE)
                {
                    int prevLen = tempCellList.length();
                    subBP->getBodyPartCellList(tempCellList);
                    if (subBP->BP_Type == AlienModeller::BodyPartsEnum::BONE)
                    {
                        Bone *bnPtr = dynamic_cast<Bone*>(subBP);
                        if (!bnPtr->subdivisionCells.isEmpty()) // If this bone was subdivided, remove its first cell
                        {
                            tempCellList.remove(prevLen);         // ...  i.e. the root cell from cellList
                        }
                    }
                }
            }
            tempCellMatrix.append(tempCellList);
            // Do the color mapping here
            tempCellListToBP_Map__forColoring.insert(tempCellMatrix.length()-1, bpPtr->BP_ID);
        }
    }
}

//void AlienModeller::getBodyPartCellList(BodyPart *thisBP, QVector<Cell *> &tempCellList)
//{
//    if (thisBP->BP_Type != AlienModeller::BodyPartsEnum::JOINT  &&  thisBP->BP_Type != AlienModeller::BodyPartsEnum::BONE)
//    {
////        tempCellList.resize(0);
//        for (int j=0; j<thisBP->subBodyParts.length(); j++)
//        {
//            // Look for joints and bones. Good to have the below check but not really needed for now. Better keep it
//            if (thisBP->subBodyParts[j]->BP_Type == AlienModeller::BodyPartsEnum::JOINT
//                    ||  thisBP->subBodyParts[j]->BP_Type == AlienModeller::BodyPartsEnum::BONE)
//            {
//                for (int k=0; k<thisBP->subBodyParts[j]->cellList.length(); k++)
//                {
//                    Cell *tempCellPtr = &(thisBP->subBodyParts[j]->cellList[k]);
//                    visitCellsRecursively(tempCellPtr, tempCellList);
//                }
//            }
//            else // recursively look for any subBPs that are not joints or bones, e.g. upper limbs attached to lower limbs
//                getBodyPartCellList(thisBP->subBodyParts[j], tempCellList);
//        }
//    }
//}

//void AlienModeller::visitCellsRecursively(Cell *rootCell, QVector<Cell *> &tempCellList)
//{
//    tempCellList.append(rootCell);

//    // go through all the children of this cell and render them recursively
//    for (int i=0; i<rootCell->childCells.length(); i++)
//    {
//        visitCellsRecursively(rootCell->childCells[i], tempCellList);
//    }
//}


void AlienModeller::printAllJointLocations(Joint &rootJnt)
{

}


void AlienModeller::initBP_IDs()
{
    originLevel_0_BP_ID_List.append("FEMUR");

    level_1_BP_ID_List.append("TENSOR_FASCIA_LATA");
    level_1_BP_ID_List.append("SARTORIOUS");
    level_1_BP_ID_List.append("RECTUS_FEMORIS");
    level_1_BP_ID_List.append("VASTUS_EXTERNUS");
    level_1_BP_ID_List.append("VASTUS_INTERNUS");
    level_1_BP_ID_List.append("TIBIALIS_ANTICUS");
    level_1_BP_ID_List.append("PERONEOUS_LONGUS");
    level_1_BP_ID_List.append("EXTENSOR_LONGUS_DIGITORUM");

    BP_Level_List.append(0);
    BP_Level_List.append(1);
    BP_Level_List.append(2);
//    BP_Level;

}

void AlienModeller::specifyLevel_1_2_FeaturePlaceHolders()
{
//    placeholderSpecs

    // -- BP: RECTUS_FEMORIS
    Level_1_2_FeaturePlaceholderSpecs *PH_0_Specs = new Level_1_2_FeaturePlaceholderSpecs();

    PH_0_Specs->originLevel_0_BP_ID = originLevel_0_BP_ID_List[0];
    PH_0_Specs->originLevel_0_BP_SelectionStrength = 80;

    PH_0_Specs->BP_Candidates.append(level_1_BP_ID_List[2]); // first priority PB
    PH_0_Specs->BP_SelectionStrength.append(80);
    PH_0_Specs->BP_Candidates.append(level_1_BP_ID_List[3]);
    PH_0_Specs->BP_SelectionStrength.append(60);
    PH_0_Specs->BP_Candidates.append(level_1_BP_ID_List[0]);
    PH_0_Specs->BP_SelectionStrength.append(40);

    PH_0_Specs->BP_LevelCandidates.append(1);
    PH_0_Specs->BP_LevelSelectionStrength.append(60);
    PH_0_Specs->BP_LevelCandidates.append(2);
    PH_0_Specs->BP_LevelSelectionStrength.append(40);

    PH_0_Specs->faceCandidates.append(Box::BoxFace::FRONT);
    PH_0_Specs->faceSelectionStrength.append(80);
    PH_0_Specs->faceCandidates.append(Box::BoxFace::RIGHT);
    PH_0_Specs->faceSelectionStrength.append(60);
    PH_0_Specs->faceCandidates.append(Box::BoxFace::LEFT);
    PH_0_Specs->faceSelectionStrength.append(40);

    PH_0_Specs->startEdge.append(QuadFace::FaceEdge::TOP);
    PH_0_Specs->percentDistFromStartEdge.append(100.0);
    PH_0_Specs->startEdge.append(QuadFace::FaceEdge::BOTTOM);
    PH_0_Specs->percentDistFromStartEdge.append(100.0);
    PH_0_Specs->startEdge.append(QuadFace::FaceEdge::LEFT);
    PH_0_Specs->percentDistFromStartEdge.append(100.0);

    PH_0_Specs->isTiltable = false;

    placeholderSpecs.append(PH_0_Specs);


    // -- BP: SARTORIOUS
    Level_1_2_FeaturePlaceholderSpecs *PH_1_Specs = new Level_1_2_FeaturePlaceholderSpecs();

    PH_1_Specs->originLevel_0_BP_ID = originLevel_0_BP_ID_List[0];
    PH_1_Specs->originLevel_0_BP_SelectionStrength = 80;

    PH_1_Specs->BP_Candidates.append(level_1_BP_ID_List[1]); // first priority PB
    PH_1_Specs->BP_SelectionStrength.append(80);
    PH_1_Specs->BP_Candidates.append(level_1_BP_ID_List[0]);
    PH_1_Specs->BP_SelectionStrength.append(60);
    PH_1_Specs->BP_Candidates.append(level_1_BP_ID_List[2]);
    PH_1_Specs->BP_SelectionStrength.append(40);

    PH_1_Specs->BP_LevelCandidates.append(1);
    PH_1_Specs->BP_LevelSelectionStrength.append(60);
    PH_1_Specs->BP_LevelCandidates.append(2);
    PH_1_Specs->BP_LevelSelectionStrength.append(40);

    PH_1_Specs->faceCandidates.append(Box::BoxFace::FRONT);
    PH_1_Specs->faceSelectionStrength.append(80);
    PH_1_Specs->faceCandidates.append(Box::BoxFace::LEFT);
    PH_1_Specs->faceSelectionStrength.append(60);
    PH_1_Specs->faceCandidates.append(Box::BoxFace::RIGHT);
    PH_1_Specs->faceSelectionStrength.append(40);

    PH_1_Specs->startEdge.append(QuadFace::FaceEdge::TOP);
    PH_1_Specs->percentDistFromStartEdge.append(80);
    PH_1_Specs->startEdge.append(QuadFace::FaceEdge::BOTTOM);
    PH_1_Specs->percentDistFromStartEdge.append(80);
    PH_1_Specs->startEdge.append(QuadFace::FaceEdge::LEFT);
    PH_1_Specs->percentDistFromStartEdge.append(80);

    PH_1_Specs->isTiltable = true;

    placeholderSpecs.append(PH_1_Specs);


    // -- BP: TENSOR_FASCIA_LATA
    Level_1_2_FeaturePlaceholderSpecs *PH_2_Specs = new Level_1_2_FeaturePlaceholderSpecs();

    PH_2_Specs->originLevel_0_BP_ID = originLevel_0_BP_ID_List[0];
    PH_2_Specs->originLevel_0_BP_SelectionStrength = 80;

    PH_2_Specs->BP_Candidates.append(level_1_BP_ID_List[0]); // first priority PB
    PH_2_Specs->BP_SelectionStrength.append(80);
    PH_2_Specs->BP_Candidates.append(level_1_BP_ID_List[4]);
    PH_2_Specs->BP_SelectionStrength.append(60);
    PH_2_Specs->BP_Candidates.append(level_1_BP_ID_List[3]);
    PH_2_Specs->BP_SelectionStrength.append(40);

    PH_2_Specs->BP_LevelCandidates.append(1);
    PH_2_Specs->BP_LevelSelectionStrength.append(60);
    PH_2_Specs->BP_LevelCandidates.append(2);
    PH_2_Specs->BP_LevelSelectionStrength.append(40);

    PH_2_Specs->faceCandidates.append(Box::BoxFace::RIGHT);
    PH_2_Specs->faceSelectionStrength.append(80);
    PH_2_Specs->faceCandidates.append(Box::BoxFace::LEFT);
    PH_2_Specs->faceSelectionStrength.append(60);
    PH_2_Specs->faceCandidates.append(Box::BoxFace::FRONT);
    PH_2_Specs->faceSelectionStrength.append(40);

    PH_2_Specs->startEdge.append(QuadFace::FaceEdge::TOP);
    PH_2_Specs->percentDistFromStartEdge.append(45);
    PH_2_Specs->startEdge.append(QuadFace::FaceEdge::BOTTOM);
    PH_2_Specs->percentDistFromStartEdge.append(45);
    PH_2_Specs->startEdge.append(QuadFace::FaceEdge::LEFT);
    PH_2_Specs->percentDistFromStartEdge.append(45);

    PH_2_Specs->isTiltable = true;

    placeholderSpecs.append(PH_2_Specs);


    // -- BP: VASTUS_EXTERNUS
    Level_1_2_FeaturePlaceholderSpecs *PH_3_Specs = new Level_1_2_FeaturePlaceholderSpecs();

    PH_3_Specs->originLevel_0_BP_ID = originLevel_0_BP_ID_List[0];
    PH_3_Specs->originLevel_0_BP_SelectionStrength = 80;

    PH_3_Specs->BP_Candidates.append(level_1_BP_ID_List[3]); // first priority PB
    PH_3_Specs->BP_SelectionStrength.append(80);
    PH_3_Specs->BP_Candidates.append(level_1_BP_ID_List[4]);
    PH_3_Specs->BP_SelectionStrength.append(60);
    PH_3_Specs->BP_Candidates.append(level_1_BP_ID_List[0]);
    PH_3_Specs->BP_SelectionStrength.append(40);

    PH_3_Specs->BP_LevelCandidates.append(1);
    PH_3_Specs->BP_LevelSelectionStrength.append(60);
    PH_3_Specs->BP_LevelCandidates.append(2);
    PH_3_Specs->BP_LevelSelectionStrength.append(40);

    PH_3_Specs->faceCandidates.append(Box::BoxFace::RIGHT);
    PH_3_Specs->faceSelectionStrength.append(80);
    PH_3_Specs->faceCandidates.append(Box::BoxFace::LEFT);
    PH_3_Specs->faceSelectionStrength.append(60);
    PH_3_Specs->faceCandidates.append(Box::BoxFace::BACK);
    PH_3_Specs->faceSelectionStrength.append(40);

    PH_3_Specs->startEdge.append(QuadFace::FaceEdge::BOTTOM);
    PH_3_Specs->percentDistFromStartEdge.append(55);
    PH_3_Specs->startEdge.append(QuadFace::FaceEdge::TOP);
    PH_3_Specs->percentDistFromStartEdge.append(55);
    PH_3_Specs->startEdge.append(QuadFace::FaceEdge::LEFT);
    PH_3_Specs->percentDistFromStartEdge.append(55);

    PH_3_Specs->isTiltable = true;

    placeholderSpecs.append(PH_3_Specs);

}


void AlienModeller::makeAlienRuleSets(Alien *alien)
{
    alien->makeAllRuleSets();
}

// recursive method
// tempFaceList stores a point on the face and its normal vector.
//      thus each face covers two adjacent objects in the list. First being any point on the face
//      and the other being the normal vector of the face
void AlienModeller::surroundingGrowthFormations(Cell *inputCell,
                                                int depth, QVector<QVector3D> &tempFaceList)
{
    if (depth >= 3)
        return;

    // Randomly specify how many surrounding boxes are to be created (between 10 and 20)

    int surrBoxCnt=0; // the total surrounding boxes (total children) for this cell box
    int childrenPerFace=0;

    surrBoxCnt = (rand() % 10)+10; // surrounding boxes are always greater than 1
    qDebug() << surrBoxCnt;

    // select the growth trend (1: Biased trend; 2: Unbiased trend).
    // Biased trend means that most of the growth will happen in a single direction
    // Let's start off with unbiased

    // go through all the faces of the cell box
    int tempCnt = surrBoxCnt;
    for (int i=0; i<inputCell->cellBox->faces.length(); i++)
    {
        // randomly determine how many child cells to be generated on this face
        // (out of the total surrounding boxes allocated for this cell i.e. surrBoxCnt)

        if (i == inputCell->cellBox->faces.length()-1)
            childrenPerFace = tempCnt;
        else
            childrenPerFace = (rand() % tempCnt);

        tempCnt -= childrenPerFace;

        qDebug() << "assigned children: " << childrenPerFace;
        qDebug() << "remaining children: " << tempCnt;

        // now randomly determine the locations for these child cells to be generated
        // now randomly determine the size of each child box.
        // At the moment assign same energy to each child
        // Formula: child cell energy = energy of this cell/(childrenPerFace+1)
        // create the first child in the centre of the face
        // offset temp1stPt to half the energy along the normal vector

        if (childrenPerFace > 0)
        {
            // First check whether this face is facing any of the faces of the boxes created before
            //      according to the following rule
            // RULE: Do not generate the box on the face whose normal vector intersects any face
            // of any of the previously generated boxes (set its probability high in Biased Trend)

            // Use the face normal to find any intersections
            QVector3D temp1stPt(0,0,0);

            if (!GeometryAlgorithms::getLinesIntersection(
                        inputCell->cellBox->vertices[inputCell->cellBox->faces[i]->vertNorIndices[0]],
                        inputCell->cellBox->vertices[inputCell->cellBox->faces[i]->vertNorIndices[2]],
                        inputCell->cellBox->vertices[inputCell->cellBox->faces[i]->vertNorIndices[1]],
                        inputCell->cellBox->vertices[inputCell->cellBox->faces[i]->vertNorIndices[3]],
                        temp1stPt)) // intersection point not found by geometric algorithm
            {
                // find a mid point on the line connecting the opposite vertices of the face
                temp1stPt =
                        inputCell->cellBox->vertices[inputCell->cellBox->faces[i]->vertNorIndices[0]]
                        +
                        (0.5 *
                        (inputCell->cellBox->vertices[inputCell->cellBox->faces[i]->vertNorIndices[2]] -
                        inputCell->cellBox->vertices[inputCell->cellBox->faces[i]->vertNorIndices[0]]));
            }

            QVector3D temp2ndPt(0,0,0);
            temp2ndPt = temp1stPt +
                   (5000)*inputCell->cellBox->normals[inputCell->cellBox->faces[i]->vertNorIndices[4]];


            int tempFlag=0;
            for (int j=0; j<tempFaceList.length(); j+=2)
            {
                QVector3D tempIntersPt(0,0,0);
                tempFlag = GeometryAlgorithms::getLinePlaneIntersectionPoint(
                                            temp1stPt, temp2ndPt, tempFaceList[j],
                                            tempFaceList[j+1], tempIntersPt);
                if (tempFlag==1)
                {
                    qDebug() << "face inelligible";
                    break; // Intersect found so no child boxes for this face
                }
            }
            if (tempFlag != 1) // No intersection found so this face can have child boxes
            {
                GLint childEnergy = inputCell->cellEnergy/3; // come up with better size approach here
//                GLint childEnergy = inputCell->cellEnergy/(childrenPerFace+1);
                QVector3D tempOffsetPt = temp1stPt +
                     (100*inputCell->cellBox->normals[inputCell->cellBox->faces[i]->vertNorIndices[4]]);

                Cell *childCellPtr = new Cell();
                childCellPtr->cellEnergy=childEnergy;
                childCellPtr->cellCenterLocation=tempOffsetPt;
                childCellPtr->makeBox();

                inputCell->childCells.append(childCellPtr);

                // create the remaining child cells on this face
                // initialize an edge

    // *************** POSSIBLE ALGORITHM *******************************************

    // ++++ The following Algorithm finds random points on a face.
    // ++++ Consider making a method for the following Algorithm in the AlienModeller class

                // k goes through face vertices in cc-wise fashion
                for (int j=0,k=0; j<childrenPerFace-1; j++,k++)
                {
                    // Get a random point on the edge i.e. randPtOnLine

                    // Get a random point on the line connecting temp1stPt and randPtOnLine
                    // and assign it to randPtOnLine
                    if (k==3) // go back to the first vertex
                        k=0;

                    QVector3D temp3rdPt =
                    inputCell->cellBox->vertices[inputCell->cellBox->faces[i]->vertNorIndices[k]]
                    +
                    ( ((rand() % 100)/100.0) *
                    (inputCell->cellBox->vertices[inputCell->cellBox->faces[i]->vertNorIndices[k+1]] -
                    inputCell->cellBox->vertices[inputCell->cellBox->faces[i]->vertNorIndices[k]]) );

                    QVector3D temp4thPt = temp1stPt + (((rand() % 100)/100.0) * (temp3rdPt - temp1stPt));

                    tempOffsetPt = temp4thPt +
                    (100*inputCell->cellBox->normals[inputCell->cellBox->faces[i]->vertNorIndices[4]]);

                    Cell *childCellPtr = new Cell();
                    childCellPtr->cellEnergy=childEnergy;
                    childCellPtr->cellCenterLocation=tempOffsetPt;
                    childCellPtr->makeBox();

                    inputCell->childCells.append(childCellPtr);

                    // Offset this randPtOnLine to specify the location of new child cell
                    // generate the child cell with the energy = childEnergy
                    // cycle to the next edge
                }
// ***************************************************************
            }
        }
        // store all the faces of this box in the input tempFaceList as a (point,normal) pair
        tempFaceList.append(inputCell->cellBox->vertices[inputCell->cellBox->faces[i]->vertNorIndices[0]]);
        tempFaceList.append(inputCell->cellBox->normals[inputCell->cellBox->faces[i]->vertNorIndices[4]]);

    }

    // Finally undergo recursion for each child to divide it further. Looks like depth first traversal?
    for (int i=0; i<inputCell->childCells.length(); i++)
    {
        surroundingGrowthFormations(inputCell->childCells[i], ++depth, tempFaceList);
    }

    // Randomly assign a fraction of the above chosen boxes to each face
    // For each face, generate the boxes in a random location on the face
    // Resize and rotate pass
}

void AlienModeller::gridBasedFormations(Cell *inputCell)
{
/*
 *  ALGORITHM
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
    *           TO DO...
    *      }
*/
    // Extract the top face of the input cell box to make an imaginary plane
    QVector<QVector3D> tempImagPlane; // will have 4 vertices and 1 normal vector
    // store all the vertices of the face in the imaginary plane
    for (int i=0; i<4; i++)
    {
        tempImagPlane.append(inputCell->cellBox->vertices
                             [inputCell->cellBox->faces[4]->vertNorIndices[i]]);
    }
    // store the face normal at the end of the imaginary plane
    tempImagPlane.append(inputCell->cellBox->vertices
                         [inputCell->cellBox->faces[4]->vertNorIndices[4]]);

    // scale up the imagPlane 3 times. Perhaps need to add a new method in GeometryAlgos Class
    GeometryAlgorithms::scalePlane(tempImagPlane, 3, 3, 3);

    // create imaginary grid style lines (horizontal and vertical) - at the moment just 10x10 grid
    QVector3D tempVec1 = tempImagPlane[1] - tempImagPlane[0];
    QVector3D tempVec2 = tempImagPlane[2] - tempImagPlane[1];
    // specify 10x10=100 points
    for (float i=0; i<10; i++)
    {
        QVector3D tempPt1 = tempImagPlane[0] + (i/10.0f)*tempVec1;
        // store the tempPt1 into a list of grid boxes
        for (float j=0; j<10; j++)
        {
            QVector3D tempPt2 = tempPt1 + (j/10.0f)*tempVec2;
            // store the tempPt1 into the list of grid boxes
        }
    }

    // .... TO BE CONTINUED ....
}

void AlienModeller::generateBGF(QVector<Cell*> cells, BodyPart *bPart)
{

}



/**
 * @brief AlienModeller::initializeAlienSkeleton
 */
/*
void AlienModeller::initializeAlienSkeleton()
{
    // ***** Random Selection of number of Joints and Bones ******

    // randomly select the number of joints
    int numJoints = (rand() % (skeletonRuleSet->skeletonSpecsBrackets[0].y()-skeletonRuleSet->skeletonSpecsBrackets[0].x()))
                                + skeletonRuleSet->skeletonSpecsBrackets[0].x();

//    int numJoints = GeometryAlgorithms::getRandomInteger(
//                                                            skeletonRuleSet->skeletonSpecsBrackets[0].x(),
//                                                            skeletonRuleSet->skeletonSpecsBrackets[0].y()
//                                                        );

    qDebug() << "Joints: " << numJoints;

    // randomly select the number of bones using the number of joints in the previous step
    // number of bones to lie b/w number of joints selected in code above and thrice the joints
    int numBones = (rand() % ((3*numJoints)-numJoints)) + numJoints;

    qDebug() << "Bones: " << numBones;

    // ***** Generation of Joints and Bones i.e. populating the joints and bones lists for this alien ******

    // instantiate all the joints
    for (int i=0; i<numJoints; i++)
    {
        Joint jnt;
        jnt.BP_ID=i;
        alienList.last()->alienJoints.append(jnt); // TODO(ismail): Better use a QMap for joints list
    }

    // instantiate all the bones
    for (int i=0; i<numBones; i++)
    {
        Bone bone;
        bone.BP_ID=i;
        alienList.last()->alienBones.append(bone); // TODO(ismail): Better use a QMap for bones list
    }

    // ***** Update the joint_ruleset and bone_ruleset ******
    // Here we also randomly specify how many bones will be allocated to each joint

    // populate the minMaxBonesPerJoint list in the Joint_RuleSet
    int tempNumBones = numBones;

    // TODO(ismail): FIX THIS. try a random value b/w 1 and 10
    int perc=3; // perc is used to calculate the percentage of bones allocated to each joint.
    for (int i=0,j=perc; i<numJoints; i++,j++) // TODO(ismail): need to use type casting to convert between float and integers.
    {
        // TODO(ismail): get rid of percentages completely and think of another logic only involving Integers
        if ( tempNumBones > ceil((perc*numBones)/100) )
        {
            int bonesPerJoint = ceil((j*tempNumBones)/100);
            jointRuleSet->minMaxBonesPerJoint.insert(i, QPoint(1,bonesPerJoint)); // i is the joint ID
            qDebug() << "max number of bones on joint " << i << " is: " << bonesPerJoint;
            tempNumBones -= bonesPerJoint;
        }
        else
        {
            jointRuleSet->minMaxBonesPerJoint.insert(i, QPoint(1,tempNumBones));
            qDebug() << "max number of bones on joint " << i << " is: " << tempNumBones;
            break;
        }
    }
    // TODO(ismail): for the joints that are left boneless, POSSIBLE SOLUTIONS are:
    //  Solution 1: I need to allocate unallocated bones to these joints
    //  Solution 2 (currently using) : get rid of all the boneless joints
    //  Solution 3 (worth exploring) : Assign other level 0 (cartiledges / bones) or level 1 (blobby features) or level 2 features to boneless joints

    // populate the minMaxBonesAllowedForAnyJoint list in the Bone_RuleSet
    for (int i=0; i<numBones; i++)
    {
        boneRuleSet->minMaxBonesAllowedForAnyJoint.insert(i, QPoint(1,2)); // TODO(ismail): perhaps come up with a better more clever max value
    }
}
*/


/*
// This method creates/populates the skeleton_specs data structure inside Skeleton i.e. it specifies which bones (and how many of those) will be allocated to each joint
void AlienModeller::createSkeletonSpecs()
{
    // Each entry refers to each bone. Initialize the Jnt2JntRelations DS so that all the joints indices are negative.
    // This is becuase a negative value will tell us that there is no other joint connected to this joint.
    alienList.last()->alienSkeleton.Jnt2JntRelations.resize(0);
    for (int i=0; i<alienList.last()->alienBones.length(); i++)
    {
        alienList.last()->alienSkeleton.Jnt2JntRelations.append(QPoint(-1,-1)); // (-1,-1) means that x is the parent joint and y is the child joint i.e. joint attached to the parent joint
    }

    QVector<int> tempBoneAllocationAmount(alienList.last()->alienBones.length(),0); //  This is a counter array that ensures that a bone must not appear in more than
                                                                                    //  two joints. We initialize it to same size as bones and fill all values with 0

    QPolygon bonesWithAmnts;    //  a set of pairs of format (BoneID, AmountChosenPerJoint). This is to be associated with each joint to specify which bone and how many of it
                                //  is attached to this joint

    // Now assign bones to each joint
    for (int i=0; i<alienList.last()->alienJoints.length(); i++)
    {
        int bonesForThisJoint = jointRuleSet->minMaxBonesPerJoint[i].y();
        qDebug() << "for joint " << i << ", bone allocations are...";
        bonesWithAmnts.resize(0);

        // TODO(ismail): VERY IMPORTANT: traverse through the bones in a random fashion (use a while loop)
        for (int j=0; j<alienList.last()->alienBones.length(); j++)
        {
            if (tempBoneAllocationAmount[j] < 2) // as soon as we detect that this bone has been allocated to more than 2 joints,
                                                 // we leave it and move to the next one
            {
                int temp1 = 0;
                temp1 = (rand() % (boneRuleSet->minMaxBonesAllowedForAnyJoint[j].y() - boneRuleSet->minMaxBonesAllowedForAnyJoint[j].x()))
                        + boneRuleSet->minMaxBonesAllowedForAnyJoint[j].x();
                tempBoneAllocationAmount[j]++;

                if (bonesForThisJoint > boneRuleSet->minMaxBonesAllowedForAnyJoint[j].y()   &&   j < alienList.last()->alienBones.length()-1)
                {
                        bonesWithAmnts << QPoint(j,temp1);
                        qDebug() << "bone: " << j << " allocated amount: " << temp1;

                        // Now populate the Jnt2JntRelations DS with the actual joint indices
                        if (alienList.last()->alienSkeleton.Jnt2JntRelations[j].x() == -1)
                            alienList.last()->alienSkeleton.Jnt2JntRelations[j].setX(i);
                        else
                            alienList.last()->alienSkeleton.Jnt2JntRelations[j].setY(i);
                }
                else
                {
                    bonesWithAmnts << QPoint(j,bonesForThisJoint); // TODO(ismail): perhaps do maximumOf( bonesForThisJoint , boneRuleSet->minMaxBonesAllowedForAnyJoint[j].y() )
                    qDebug() << "bone: " << j << " allocated amount: " << bonesForThisJoint;

                    // Now populate the Jnt2JntRelations DS with the actual joint indices
                    if (alienList.last()->alienSkeleton.Jnt2JntRelations[j].x() == -1)
                        alienList.last()->alienSkeleton.Jnt2JntRelations[j].setX(i);
                    else
                        alienList.last()->alienSkeleton.Jnt2JntRelations[j].setY(i);

                    break;
                }
                bonesForThisJoint -= temp1;
            }
        }
        alienList.last()->alienSkeleton.skeleton_specs.append(bonesWithAmnts);
    }

    for (int i=0; i<alienList.last()->alienSkeleton.Jnt2JntRelations.length(); i++)
    {
        qDebug() << "parent joint: " << alienList.last()->alienSkeleton.Jnt2JntRelations[i].x()
                 << ":: child joint: " << alienList.last()->alienSkeleton.Jnt2JntRelations[i].y();
    }


}
*/

// This method creates/populates the skeleton_specs data structure inside Skeleton i.e. it specifies which bones (and how many of those) will be allocated to each joint
//void AlienModeller::createSkeletonSpecs()
//{
//    QVector<int> isJntAChild(alienList.last()->alienJoints.length(), 0);

//    for (int i=0; i<tempJntList_1.length() && tempJntList_2.length()>0; i++)
//    {

//        // delete the entry in tempJntList_2 with the value as ith jnt
//        if (tempJntList_2.indexOf(tempJntList_1[i]) != -1)
//            tempJntList_2.remove(tempJntList_2.indexOf(tempJntList_1[i]));

//        // pick up an index at random from tempJntList_2
//        int randInd_tempJntList_2 = GeometryAlgorithms::getRandomInteger(0, tempJntList_2.length());

//        // pair the ith jnt from tempJntList_1 with the random indexth jnt from tempJntList_2
//        // insert the pair in the Jnt2Jnt relations list
//        alienList.last()->alienSkeleton.Jnt2JntRelations.append
//                (QPoint(tempJntList_1[i],tempJntList_2[randInd_tempJntList_2]));

//        // DEBUG(ismail)
//        qDebug() << alienList.last()->alienSkeleton.Jnt2JntRelations[i].x()
//                 << " " << alienList.last()->alienSkeleton.Jnt2JntRelations[i].y();

//        // mark this randomly selected jnt in parentChild list as a child
//        isJntAChild[tempJntList_2[randInd_tempJntList_2]] = 1;

//        // finally delete the randomly selected jnt in tempJntList_2
//        tempJntList_2.remove(randInd_tempJntList_2);
//    }

//    // insert a new joint at the beginning of the joints list
//    // go through all the parents in the parentChild list and make these all children to a new parent joint
//    // insert (prepend) all the these pairs in the Jnt2Jnt list in at the beginning of the list

//    // DEBUG(ismail)
//    for (int i=0; i<isJntAChild.length(); i++)
//    {
//        qDebug() << i << " : " << isJntAChild[i];
//    }

//}


/*
//++++++++ ANOTHER SIMILAR ALGORITHM THAT WORKS WELL +++++++//
void AlienModeller::computeJointArrangements()
{
    // declare a vector of pairs/couples, where the first element will store the vector for guiding the orientation
    // and the second vector is the position/location of the joint
    QVector<QVector<QVector3D>> tempVectorLocationCoupleForEachJoint(
                                                                        alienList.last()->alienJoints.length(),
                                                                        QVector<QVector3D>(2, QVector3D(0.0,0.0,1.0))
                                                                     );

    // pick up a central joint (a joint chosen at random)
    int randRootJnt = alienList.last()->alienSkeleton.Jnt2JntRelations[0].x(); // always select the first joint as the root joint

    setUpJointArrangementTrend(randRootJnt);

    // TODO(ismail): fix the two below so they contain a key, value pair. Key is the joint ID and value is the 0=false or 1=true
    QVector<int> tempJntCompletionCheckList(alienList.last()->alienJoints.length(),0); // as soon as all the neighbouring joints positions are computed, its box is checked
    QVector<int> tempJntPositionCheckList(alienList.last()->alienJoints.length(),0); // as soon as a joint's position is computed, its box is checked

    alienList.last()->alienJoints[randRootJnt]->BP_PivotPt = QVector3D(0.0,0.0,0.0);
    alienList.last()->alienJoints[randRootJnt]->makeCell(alienList.last()->alienJoints[randRootJnt]->BP_PivotPt, 5);
    tempJntPositionCheckList[randRootJnt] = 1;

    for (int i=0; i<alienList.last()->alienSkeleton.Jnt2JntRelations.length(); i++)
    {
        bool isThisChildFirst=true;
        int tempJnt = alienList.last()->alienSkeleton.Jnt2JntRelations[i].x();
        if (tempJntCompletionCheckList[tempJnt] == 1)
            continue;
        for (int j=i; j<alienList.last()->alienSkeleton.Jnt2JntRelations.length(); j++)
        {
            int tempJnt2 = alienList.last()->alienSkeleton.Jnt2JntRelations[j].y();
            if (alienList.last()->alienSkeleton.Jnt2JntRelations[j].x() == tempJnt  &&  tempJntPositionCheckList[tempJnt2] == 0) // position not finalized yet..so go ahead?
            {
                // compute the energy and angle for this joint using VectorEnergyAngleTrends DS inside JointsArrangementTrend

                if (tempJnt == randRootJnt)
                {
                    // specify the energy and angle trends for this root joint randomly. this changes for each child
                    // so that each child follows a new trend
                    jntsArrangeTrends.last().VectorEnergyAngleTrends[tempJnt][1] = static_cast<float>(GeometryAlgorithms::getRandomInteger(0, 4));
                    jntsArrangeTrends.last().VectorEnergyAngleTrends[tempJnt][3] = static_cast<float>(GeometryAlgorithms::getRandomInteger(0, 4));
                }

                //
                // ********* first get the energy sorted *********** // TODO(ismail): can use switch statement here
                //
                if (jntsArrangeTrends.last().VectorEnergyAngleTrends[tempJnt].at(1) == 0) // if the energy trend of its parent is ASCENDING
                {
                    // compute the energy greater than the parent's energy - randomly selecting between (energy of parent  AND  X times energy of parent)...Select X carefully. big will produce very large values
                    // TODO(ismail): VERY IMPORTANT for generating diverse shapes of aliens. Try different types of calls below.
                    //                                                                          1) remove GeometryAlgorithms::getRandomFloat(0.1,1.0) from first argument.
                    //                                                                          2) only use 2 * 2nd argument.
                    // These can be very useful in generating different shape trends of aliens. Let the use select a particular type of alien and then change these internally
                    jntsArrangeTrends.last().VectorEnergyAngleTrends[tempJnt2][0] = GeometryAlgorithms::getRandomFloat(
                                                                                                                        GeometryAlgorithms::getRandomFloat(0.1,1.0) * jntsArrangeTrends.last().VectorEnergyAngleTrends[tempJnt].at(0),
                                                                                                                        GeometryAlgorithms::getRandomFloat(2.0,5.0) * jntsArrangeTrends.last().VectorEnergyAngleTrends[tempJnt].at(0)
                                                                                                                       );

                    jntsArrangeTrends.last().VectorEnergyAngleTrends[tempJnt2][1] = 0; // keep the trend ASCENDING
                }
                else if (jntsArrangeTrends.last().VectorEnergyAngleTrends[tempJnt].at(1) == 1) // if the energy trend of its parent is DESCENDING
                {
                    jntsArrangeTrends.last().VectorEnergyAngleTrends[tempJnt2][0] = GeometryAlgorithms::getRandomFloat(
                                                                                                                        0.1,
                                                                                                                        jntsArrangeTrends.last().VectorEnergyAngleTrends[tempJnt].at(0)
                                                                                                                      );

                    jntsArrangeTrends.last().VectorEnergyAngleTrends[tempJnt2][1] = 1; // keep the trend DESCENDING
                }
                else if (jntsArrangeTrends.last().VectorEnergyAngleTrends[tempJnt].at(1) == 2) // if the energy trend of its parent is WAVY UP
                {
                    //
                    jntsArrangeTrends.last().VectorEnergyAngleTrends[tempJnt2][0] = GeometryAlgorithms::getRandomFloat(
                                                                                                                            GeometryAlgorithms::getRandomFloat(0.1,1.0) * jntsArrangeTrends.last().VectorEnergyAngleTrends[tempJnt].at(0),
                                                                                                                            GeometryAlgorithms::getRandomFloat(2.0,5.0) * jntsArrangeTrends.last().VectorEnergyAngleTrends[tempJnt].at(0)
                                                                                                                      );
                    jntsArrangeTrends.last().VectorEnergyAngleTrends[tempJnt2][1] = 3; // flip the trend to WAVY DOWN
                }
                else if (jntsArrangeTrends.last().VectorEnergyAngleTrends[tempJnt].at(1) == 3) // if the energy trend of its parent is WAVY UP
                {
                    //
                    jntsArrangeTrends.last().VectorEnergyAngleTrends[tempJnt2][0] = GeometryAlgorithms::getRandomFloat(
                                                                                                                        0.1,
                                                                                                                        jntsArrangeTrends.last().VectorEnergyAngleTrends[tempJnt].at(0)
                                                                                                                      );
                    jntsArrangeTrends.last().VectorEnergyAngleTrends[tempJnt2][1] = 2; // flip the trend to WAVY UP
                }
                else if (jntsArrangeTrends.last().VectorEnergyAngleTrends[tempJnt].at(1) == 4) // if the energy trend of its parent is STABLE
                {
                    //
                    jntsArrangeTrends.last().VectorEnergyAngleTrends[tempJnt2][0] = jntsArrangeTrends.last().VectorEnergyAngleTrends[tempJnt].at(0);
                    jntsArrangeTrends.last().VectorEnergyAngleTrends[tempJnt2][1] = 4; // keep the trend STABLE
                }

//                jntsArrangeTrends.last().VectorEnergyAngleTrends[tempJnt2][0] = jntsArrangeTrends.last().VectorEnergyAngleTrends[tempJnt].at(0);

                //
                // ********* Next get the angle sorted ***********
                //
                if (jntsArrangeTrends.last().VectorEnergyAngleTrends[tempJnt].at(3) == 0) // if the angle trend of its parent is ASCENDING
                {
                    // calculate an angle that is more than the previous angle.
                    // end result: rotate vector at an incremented angle from the previous vector along the pitch
//                    jntsArrangeTrends.last().VectorEnergyAngleTrends[tempJnt2].at(2) = jntsArrangeTrends.last().VectorEnergyAngleTrends[tempJnt].at(2) + 10;
                    jntsArrangeTrends.last().VectorEnergyAngleTrends[tempJnt2][2] = 10; // when rotating vector do: pitch = 10
                    jntsArrangeTrends.last().VectorEnergyAngleTrends[tempJnt2][3] = 0; // keep the angle trend ASCENDING
                }
                else if (jntsArrangeTrends.last().VectorEnergyAngleTrends[tempJnt].at(3) == 1) // if the angle trend of its parent is DESCENDING
                {
//                    jntsArrangeTrends.last().VectorEnergyAngleTrends[tempJnt2].at(2) = jntsArrangeTrends.last().VectorEnergyAngleTrends[tempJnt].at(2) + 10;
                    jntsArrangeTrends.last().VectorEnergyAngleTrends[tempJnt2][2] = 360-10; // when rotating vector do: pitch = 360 - 10
                    jntsArrangeTrends.last().VectorEnergyAngleTrends[tempJnt2][3] = 1; // keep the angle trend DESCENDING
                }
                else if (jntsArrangeTrends.last().VectorEnergyAngleTrends[tempJnt].at(3) == 2) // if the angle trend of its parent is WAVY UP
                {
//                    jntsArrangeTrends.last().VectorEnergyAngleTrends[tempJnt2][2] = jntsArrangeTrends.last().VectorEnergyAngleTrends[tempJnt].at(2) + 10;
                    jntsArrangeTrends.last().VectorEnergyAngleTrends[tempJnt2][2] = 10;
                    jntsArrangeTrends.last().VectorEnergyAngleTrends[tempJnt2][3] = 3; // flip the trend to WAVY DOWN
                }
                else if (jntsArrangeTrends.last().VectorEnergyAngleTrends[tempJnt].at(3) == 3) // if the angle trend of its parent is WAVY DOWN
                {
//                    jntsArrangeTrends.last().VectorEnergyAngleTrends[tempJnt2][2] = jntsArrangeTrends.last().VectorEnergyAngleTrends[tempJnt].at(2) - 10;
                    jntsArrangeTrends.last().VectorEnergyAngleTrends[tempJnt2][2] = 360 - 10;
                    jntsArrangeTrends.last().VectorEnergyAngleTrends[tempJnt2][3] = 2; // flip the trend to WAVY UP
                }
                else if (jntsArrangeTrends.last().VectorEnergyAngleTrends[tempJnt].at(3) == 4) // if the angle trend of its parent is STABLE
                {
//                    jntsArrangeTrends.last().VectorEnergyAngleTrends[tempJnt2][2] = jntsArrangeTrends.last().VectorEnergyAngleTrends[tempJnt].at(2);
                    jntsArrangeTrends.last().VectorEnergyAngleTrends[tempJnt2][2] = 0;
                    jntsArrangeTrends.last().VectorEnergyAngleTrends[tempJnt2][3] = 4; // keep the trend STABLE
                }

                qDebug() << "energy trend : " << jntsArrangeTrends.last().VectorEnergyAngleTrends[tempJnt2][1]
                         << "; new energy : " << jntsArrangeTrends.last().VectorEnergyAngleTrends[tempJnt2][0]
                         << " :: angle trend : " << jntsArrangeTrends.last().VectorEnergyAngleTrends[tempJnt2][3]
                         << "; new angle : " << jntsArrangeTrends.last().VectorEnergyAngleTrends[tempJnt2][2];


                // Now compute the position of this joint using the energy and vector angle
                QQuaternion vecOrientQuat;

                //  if its parent joint is the root joint, use random vectors directions for computing its children joints
                //  compute a new random vector
                if (tempJnt == randRootJnt)
                {
                    // compute a vector that is oriented randomly in some direction using Euler angles
                    // later this can be made more easthetic such as vectors close to each other
                    vecOrientQuat = QQuaternion::fromEulerAngles(GeometryAlgorithms::getRandomFloat(0.0,360.0),
                                                                 GeometryAlgorithms::getRandomFloat(0.0,360.0),
                                                                 GeometryAlgorithms::getRandomFloat(0.0,360.0));
                }
                else if (isThisChildFirst)
                {
                    // TODO(ismail): Inspect this. Is this working fine?
//                    vecOrientQuat = QQuaternion::fromEulerAngles((jntsArrangeTrends.last().VectorEnergyAngleTrends[tempJnt2].at(2)), 0.0, 0.0)
//                                    * QQuaternion::rotationTo(QVector3D(0.0,0.0,1.0), tempVectorLocationCoupleForEachJoint[tempJnt].at(0));

                    vecOrientQuat = QQuaternion::rotationTo(QVector3D(0.0,0.0,1.0), tempVectorLocationCoupleForEachJoint[tempJnt].at(0))
                                    * QQuaternion::fromEulerAngles((jntsArrangeTrends.last().VectorEnergyAngleTrends[tempJnt2].at(2)), 0.0, 0.0);

//                    vecOrientQuat = QQuaternion::fromEulerAngles((jntsArrangeTrends.last().VectorEnergyAngleTrends[tempJnt2].at(2)), 0.0, 0.0);

                }

                else
                {
                    // TODO(ismail): Inspect this. Is this working fine?
//                    vecOrientQuat = QQuaternion::fromEulerAngles((jntsArrangeTrends.last().VectorEnergyAngleTrends[tempJnt2].at(2)), 0.0, 0.0)
//                                    * QQuaternion::rotationTo(QVector3D(0.0,0.0,1.0), tempVectorLocationCoupleForEachJoint[tempJnt].at(0));

                    vecOrientQuat = QQuaternion::rotationTo(QVector3D(0.0,0.0,1.0), tempVectorLocationCoupleForEachJoint[tempJnt].at(0))
                                    * QQuaternion::fromEulerAngles(GeometryAlgorithms::getRandomFloat(5.0,60.0),
                                                                   GeometryAlgorithms::getRandomFloat(5.0,60.0),
                                                                   GeometryAlgorithms::getRandomFloat(5.0,60.0));

//                    vecOrientQuat = QQuaternion::fromEulerAngles((jntsArrangeTrends.last().VectorEnergyAngleTrends[tempJnt2].at(2)), 0.0, 0.0);

                }


                QVector3D tempOrientVector = (vecOrientQuat.rotatedVector(QVector3D(0.0,0.0,1.0))).normalized();
//                QVector3D tempOrientVector = (vecOrientQuat.rotatedVector(tempVectorLocationCoupleForEachJoint[tempJnt].at(0))).normalized();

                // now compute the joint's location using the energy computed in the code above
                QVector3D newLoc = tempVectorLocationCoupleForEachJoint[tempJnt].at(1) + (jntsArrangeTrends.last().VectorEnergyAngleTrends[tempJnt].at(0)
                                                              * tempOrientVector);

                qDebug() << "joint " << tempJnt2 << " has location : " << newLoc;

                tempVectorLocationCoupleForEachJoint[tempJnt2][0] = tempOrientVector;
                tempVectorLocationCoupleForEachJoint[tempJnt2][1] = newLoc;

                alienList.last()->alienJoints[tempJnt2]->BP_PivotPt = tempVectorLocationCoupleForEachJoint[tempJnt2].at(1);
                alienList.last()->alienJoints[tempJnt2]->makeCell(alienList.last()->alienJoints[tempJnt2]->BP_PivotPt, 5);
                GeometryAlgorithms::rotateCellTowardsVecDir(&alienList.last()->alienJoints[tempJnt2]->cellList[0],
                                                            tempOrientVector);

//                alienList.last()->alienJoints[tempJnt].subBodyParts.append(alienList.last()->alienJoints[tempJnt2]);

                // now specify that this child joints position has been finalized
                tempJntPositionCheckList[tempJnt2] = 1;
            }
            isThisChildFirst=false;
        }
        tempJntCompletionCheckList[tempJnt] = 1;
    }
}


void AlienModeller::buildAlienSkeleton_TypeGuided()
{
    // +++++++ Instantiate all BPs (from the DNA) +++++++++
    instantiateSkeletalBPsFromDNA_TypeGuided();

    // +++++++ Instantiate the number of joints for all BPs (from the DNA) ++++++++
    instantiateNumOfJointsForAllBPs_TypeGuided();

    // +++++++ Specify Jnt2JntRelations for Type Guided +++++++
    // TODO(ismail): Define rules for what kind of relationships can be done here. Is it linear relationship (linear lineage) or non-linear relationships.
    // .................................................... At the moment I'm using linear relationships +++++++++
    specifyJnt2JntRelations_TypeGuided();

    // initialize the JointsArrangementTrend for this alien with dummy values. this method is only for aliens that are not cloned from another aliens
    // TODO(ismail): If this alien is to be cloned from another alien, then call cloneJointsArrangementTrendForCurrentAlien()
    // ... and copy the entire JointsArrangementTrend
    initializeJointsArrangementTrendForCurrentAlien();

    bool alienBeingCloned=false;


    // +++++++ Generate All the joints from the Jnt2JntRelations and arrange these based on JAT DNA. Generate means creating the cells for these joints +++++++

    QMap<int, int> temp_BP_JAT_DNA_counter; // this DS stores a 'counter' for each BP_TYPE_ID as present in the BP_JAT_DNA in Skeleton_DNA. We need this because we need to give a new JAT for each BP
    QMapIterator<int, QPolygonF> i(alienList.last()->alienDNA->skeletonDNA->BP_JAT_rootJoint_DNA);
    while (i.hasNext())
    {
        i.next();
        temp_BP_JAT_DNA_counter.insert(i.key(), 0);
    }


    QMap<int,int> tempParentJntCompletionCheckList; // checklist for when the all the child joints's positions are computed. Value(int) is 1 for DONE and 0 for NOT_DONE.
                                              // ... TODO(ismail): Perhaps use enum or macro for these values
    QMap<int,int> tempChildJntPositionCheckList; // checklist for when the joint's position is computed
    for (int i=0; i<alienList.last()->alienJoints.length(); i++)
    {
        QPair<QVector3D, QVector3D> tempPair;
        tempPair.first = QVector3D(0.0,0.0,1.0); tempPair.second = QVector3D(0.0,0.0,0.0);
        tempVectorLocationCoupleForEachJoint_TypeGuided.insert(alienList.last()->alienJoints[i]->BP_ID, tempPair);

        tempParentJntCompletionCheckList.insert(alienList.last()->alienJoints[i]->BP_ID,0);
        tempChildJntPositionCheckList.insert(alienList.last()->alienJoints[i]->BP_ID,0);
    }

    float newBP_startingX_Loc = -500.0;
    QMapIterator<int, QPolygon*> tempIter(alienList.last()->alienSkeleton.Jnt2JntRelations_TypeGuided);
    while (tempIter.hasNext())
    {
        tempIter.next();
        int tempBP_ID = tempIter.key();
        QPolygon *tempJnt2JntPoly = tempIter.value();

        BodyPart *tempThisBP_Ptr = getBodyPartFromID(tempBP_ID); // TODO(ismail): check if a nullptr is being returned or not

        // pick up a root joint (at random) (** NOT USING RANDOM AT THE MOMENT **)
        // always select the first joint as the root joint
        int randRootJnt = tempJnt2JntPoly->at(0).x();

        tempThisBP_Ptr->rootJntID = randRootJnt; // store the root joints' ID in this BP object
        tempThisBP_Ptr->BP_PivotVector = QVector3D(0.0,0.0,1.0);

        // Create the cell for the root joint
        for (int k=0; k<alienList.last()->alienJoints.length(); k++)
        {
            if (alienList.last()->alienJoints[k]->BP_ID == randRootJnt)
            {
                alienList.last()->alienJoints[k]->BP_PivotPt = QVector3D(newBP_startingX_Loc,0.0,0.0);
                alienList.last()->alienJoints[k]->makeCell(alienList.last()->alienJoints[k]->BP_PivotPt, 5);
                tempVectorLocationCoupleForEachJoint_TypeGuided[randRootJnt].second = alienList.last()->alienJoints[k]->BP_PivotPt;

                // Update the tranlation matrix of this BP. This will be useful when transformations are performed on
                // TODO(ismail): Need to make a method for this perhaps in the GeometricAlgoriths class or inside the BP class
                QMatrix4x4 tempTransMat;
                tempTransMat.setToIdentity();
                tempTransMat.translate(alienList.last()->alienJoints[k]->BP_PivotPt);
                tempThisBP_Ptr->BP_TranslateMatrix *= tempTransMat;
                tempThisBP_Ptr->BP_PivotPt = alienList.last()->alienJoints[k]->BP_PivotPt;

                break;
            }
        }
        newBP_startingX_Loc += 50.0;
        tempChildJntPositionCheckList[randRootJnt] = 1;


        float randomEnergyMultipleFromDNA=0, randomAngleFromDNA=0;
        int tempBP_Type_ID = tempThisBP_Ptr->BP_Type;
        QPolygonF temp_BP_JAT_DNA_Poly = this->alienList.last()->alienDNA->skeletonDNA->BP_JAT_rootJoint_DNA.value(tempBP_Type_ID);
        int cnt = temp_BP_JAT_DNA_counter.value(tempBP_Type_ID);
//        randomEnergyMultipleFromDNA /= 2;
        jntsArrangeTrends.last().VectorEnergyAngleTrends_TypeGuided.value(randRootJnt).at(0).setX(temp_BP_JAT_DNA_Poly.at(cnt).x());
        jntsArrangeTrends.last().VectorEnergyAngleTrends_TypeGuided.value(randRootJnt).at(1).setX(temp_BP_JAT_DNA_Poly.at(cnt+1).x());

        randomEnergyMultipleFromDNA = temp_BP_JAT_DNA_Poly.at(cnt).y();
        randomAngleFromDNA = temp_BP_JAT_DNA_Poly.at(cnt+1).y();

        cnt += 2;
        temp_BP_JAT_DNA_counter.insert(tempBP_Type_ID, cnt);


        QPolygonF tempRootJntPolygon = jntsArrangeTrends.last().VectorEnergyAngleTrends_TypeGuided.value(randRootJnt);

        for (int i=0; i<tempJnt2JntPoly->length(); i++)
        {
            bool isThisChildFirst=true;
            int parentJnt = tempJnt2JntPoly->at(i).x();

            if (tempParentJntCompletionCheckList.value(parentJnt) == 1)
                continue;

            QPolygonF tempPolygon = jntsArrangeTrends.last().VectorEnergyAngleTrends_TypeGuided.value(parentJnt);
            for (int j=i; j<tempJnt2JntPoly->length(); j++)
            {
                int childJnt = tempJnt2JntPoly->at(j).y();
                if (tempJnt2JntPoly->at(j).x() == parentJnt  &&  tempChildJntPositionCheckList.value(childJnt) == 0) // position not finalized yet..so go ahead
                {
                    // compute the energy and angle for this joint using VectorEnergyAngleTrends_TypeGuided DS inside JointsArrangementTrend
                    QPolygonF tempPolygon2 = jntsArrangeTrends.last().VectorEnergyAngleTrends_TypeGuided.value(childJnt);
                    //
                    // ********* first get the energy sorted *********** // TODO(ismail): can use switch statement here
                    //
                    if (tempRootJntPolygon[0].x() == 4.0) // if the trend at the root joint is irregular
                    {
                        int tempRandInt = GeometryAlgorithms::getRandomInteger(-100, 100);
                        if (tempRandInt < 0)
                        {
                            tempPolygon[0].setX(1.0);
                            jntsArrangeTrends.last().VectorEnergyAngleTrends_TypeGuided.value(parentJnt).at(0).setX(1.0);
                        }
                        else
                        {
                            tempPolygon[0].setX(0.0);
                            jntsArrangeTrends.last().VectorEnergyAngleTrends_TypeGuided.value(parentJnt).at(0).setX(0.0);
                        }
                    }

                    if (tempPolygon[0].x() == 0.0) // if the energy trend of its parent is ASCENDING
                    {
                        //++++ This is also an acceptable solution. Keeps the aliens not extend out too much
//                        tempPolygon2[0].setY(GeometryAlgorithms::getRandomFloat(tempPolygon[0].y(), GeometryAlgorithms::getRandomInteger(50,150) + tempPolygon[0].y()));
                        tempPolygon2[0].setY(GeometryAlgorithms::getRandomFloat( tempPolygon[0].y(), randomEnergyMultipleFromDNA * tempPolygon[0].y()) );
                        tempPolygon2[0].setX(0);
                        qDebug() << "ENERGY ASCENDING";
                    }
                    else if (tempPolygon[0].x() == 1.0) // if the energy trend of its parent is DESCENDING
                    {
                        tempPolygon2[0].setY(GeometryAlgorithms::getRandomFloat( (1/randomEnergyMultipleFromDNA)*tempPolygon[0].y(),tempPolygon[0].y() ));
                        tempPolygon2[0].setX(1);
                        qDebug() << "ENERGY DESCENDING";
                    }
                    else if (tempPolygon[0].x() == 2.0) // if the energy trend of its parent is WAVY UP
                    {
                        //++++ This is also an acceptable solution. Keeps the aliens not extend out too much
//                        tempPolygon2[0].setY(GeometryAlgorithms::getRandomFloat(tempPolygon[0].y(), GeometryAlgorithms::getRandomInteger(50,150) + tempPolygon[0].y()));
                        tempPolygon2[0].setY(GeometryAlgorithms::getRandomFloat( tempPolygon[0].y(), randomEnergyMultipleFromDNA * tempPolygon[0].y()) );
                        tempPolygon2[0].setX(3);
                        qDebug() << "ENERGY WAVY UP";
                    }
                    else if (tempPolygon[0].x() == 3.0) // if the energy trend of its parent is WAVY DOWN
                    {
                        tempPolygon2[0].setY(GeometryAlgorithms::getRandomFloat( (1/randomEnergyMultipleFromDNA)*tempPolygon[0].y(),tempPolygon[0].y() ));
                        tempPolygon2[0].setX(2);
                        qDebug() << "ENERGY WAVY DOWN";
                    }

                    //
                    // ********* Next get the angle sorted ***********
                    //
                    if (tempRootJntPolygon[1].x() == 4.0) // if the trend at the root joint is irregular
                    {
                        qDebug() << "ANGLE TREND IS IRREGULAR";
                        int tempRandInt = GeometryAlgorithms::getRandomInteger(-100, 100);
                        if (tempRandInt < 0)
                        {
                            tempPolygon[1].setX(1.0);
                            jntsArrangeTrends.last().VectorEnergyAngleTrends_TypeGuided.value(parentJnt).at(1).setX(1.0);
                        }
                        else
                        {
                            tempPolygon[1].setX(0.0);
                            jntsArrangeTrends.last().VectorEnergyAngleTrends_TypeGuided.value(parentJnt).at(1).setX(1.0);
                        }
                    }

                    if (tempPolygon[1].x() == 0.0) // if the angle trend of its parent is ASCENDING
                    {
                        // calculate an angle that is more than the previous angle.
                        // end result: rotate vector at an incremented angle from the previous vector along the pitch
                        tempPolygon2[1].setY(randomAngleFromDNA); // can be randomly chosen between two values
                        tempPolygon2[1].setX(0); // keep the trend ASCENDING
                        qDebug() << "ANGLE ASCENDING";
                    }
                    else if (tempPolygon[1].x() == 1.0) // if the angle trend of its parent is DESCENDING
                    {
                        tempPolygon2[1].setY(360.0-randomAngleFromDNA);
                        tempPolygon2[1].setX(1); // keep the trend DESCENDING
                        qDebug() << "ANGLE DESCENDING";
                    }
                    else if (tempPolygon[1].x() == 2.0) // if the angle trend of its parent is WAVY UP
                    {
                        tempPolygon2[1].setY(randomAngleFromDNA); // make it a larger angle to achieve close curvings
                        tempPolygon2[1].setX(3); // make the trend WAVY DOWN
                        qDebug() << "ANGLE WAVY DOWN";
                    }
                    else if (tempPolygon[1].x() == 3.0) // if the angle trend of its parent is WAVY DOWN
                    {
                        tempPolygon2[1].setY(360.0-randomAngleFromDNA);
                        tempPolygon2[1].setX(2); // make the trend WAVY UP
                        qDebug() << "ANGLE WAVY UP";
                    }

                    // TODO(ismail): perhaps get rid of remove() as the updated Polygon replaces the old one in insert()
                    if (jntsArrangeTrends.last().VectorEnergyAngleTrends_TypeGuided.remove(childJnt) == 1)
                        jntsArrangeTrends.last().VectorEnergyAngleTrends_TypeGuided.insert(childJnt, tempPolygon2);
                    else
                    {
                        qDebug() << "doesn't work";
                        exit(1);
                    }


//                    qDebug() << "energy trend : " << tempPolygon2[0].x()    << "; new energy : " << tempPolygon2[0].y()
//                             << " :: angle trend : " << tempPolygon2[1].x() << "; new angle : " << tempPolygon2[1].y();



                    // Now compute the position of this joint using the energy and vector angle.
                    // In other words, we are computing the tempVectorLocationCoupleForEachJoint_TypeGuided
                    QQuaternion vecOrientQuat;

                    //  if its parent joint is the root joint, use random vectors directions for computing its children joints
                    //  compute a new random vector
                    if (parentJnt == randRootJnt)
                    {
                        // compute a vector that is oriented randomly in some direction using Euler angles
                        // later this can be made more aesthetic such as vectors close to each other
                        // ** Set this to (5.0,60) for creatures that look like aquatic creatures **
                        vecOrientQuat = QQuaternion::fromEulerAngles(0.0,0.0,0.0);
                    }
                    else if (isThisChildFirst)
                    {
                        qDebug() << "isThisFirstChild reached";
                        // TODO(ismail): Inspect this. Is this working fine?
                        int tempRandInt = GeometryAlgorithms::getRandomInteger(-100, 100);
                        if (tempRandInt < 0)
                            vecOrientQuat = QQuaternion::rotationTo(QVector3D(0.0,0.0,1.0), tempVectorLocationCoupleForEachJoint_TypeGuided.value(parentJnt).first)
                                            * QQuaternion::fromEulerAngles(tempPolygon2[1].y(), 5.0, 0.0);
                        else
                            vecOrientQuat = QQuaternion::rotationTo(QVector3D(0.0,0.0,1.0), tempVectorLocationCoupleForEachJoint_TypeGuided.value(parentJnt).first)
                                        * QQuaternion::fromEulerAngles(tempPolygon2[1].y(), 0.0, 0.0);
                    }

                    else
                    {
                        // TODO(ismail): Inspect this. Is this working fine?
    //                    vecOrientQuat = QQuaternion::fromEulerAngles((jntsArrangeTrends.last().VectorEnergyAngleTrends[tempJnt2].at(2)), 0.0, 0.0)
    //                                    * QQuaternion::rotationTo(QVector3D(0.0,0.0,1.0), tempVectorLocationCoupleForEachJoint[tempJnt].at(0));
                        vecOrientQuat = QQuaternion::rotationTo(QVector3D(0.0,0.0,1.0), tempVectorLocationCoupleForEachJoint_TypeGuided.value(parentJnt).first)
                                        * QQuaternion::fromEulerAngles(GeometryAlgorithms::getRandomFloat(5.0,60.0),
                                                                       GeometryAlgorithms::getRandomFloat(5.0,60.0),
                                                                       GeometryAlgorithms::getRandomFloat(5.0,60.0));

                        isJntAChild[(alienList.last()->alienJoints[i]->BP_ID)].setY(1);
    //                    vecOrientQuat = QQuaternion::fromEulerAngles((jntsArrangeTrends.last().VectorEnergyAngleTrends[tempJnt2].at(2)), 0.0, 0.0);
                    }

                    QVector3D tempOrientVector = (vecOrientQuat.rotatedVector(QVector3D(0.0,0.0,1.0))).normalized();
    //                QVector3D tempOrientVector = (vecOrientQuat.rotatedVector(tempVectorLocationCoupleForEachJoint[tempJnt].at(0))).normalized();

                    // now compute the joint's location using the energy computed in the code above
                    QVector3D newLoc =  tempVectorLocationCoupleForEachJoint_TypeGuided.value(parentJnt).second + (tempPolygon2[0].y() * tempOrientVector);

//                    qDebug() << "joint " << tempJnt2 << " has location : " << newLoc;

                    tempVectorLocationCoupleForEachJoint_TypeGuided[childJnt].first = tempOrientVector;
                    tempVectorLocationCoupleForEachJoint_TypeGuided[childJnt].second = newLoc;

//                    // Now set this joint's location and create a cell for it
//                    for (int k=0; k<alienList.last()->alienJoints.length(); k++)
//                    {
//                        if (alienList.last()->alienJoints[k]->BP_ID == childJnt)
//                        {
//    //                        alienList.last()->alienJoints[k]->BP_PivotPt = newLoc; // set the joint's location
//                            alienList.last()->alienJoints[k]->makeCell(tempVectorLocationCoupleForEachJoint_TypeGuided[childJnt].second, 5); // create the joint's cell
//                            // rotate the joint towards the newly computed vector: tempOrientVector
//                            GeometryAlgorithms::rotateCellTowardsVecDir(&alienList.last()->alienJoints[k]->cellList[0],
//                                                                        tempVectorLocationCoupleForEachJoint_TypeGuided[childJnt].first);
//    //                        GeometryAlgorithms::scaleCell(&alienList.last()->alienJoints[k]->cellList[0], 0.5,0.5,10*tempOrientVector.length());
//                            break;
//                        }
//                    }

                    // now specify that this child joint's position has been finalized
                    tempChildJntPositionCheckList[childJnt] = 1;

                    // This is just to store the parent and child joint's locations to be rendered as opengl lines
                    QPair<QVector3D,QVector3D> vec3DPair;
                    vec3DPair.first = tempVectorLocationCoupleForEachJoint_TypeGuided.value(parentJnt).second;
                    vec3DPair.second = tempVectorLocationCoupleForEachJoint_TypeGuided.value(parentJnt).second + (tempPolygon2[0].y() * tempOrientVector);
                    alienList.last()->tempVectorsListForRendering.append(vec3DPair);
                }
                isThisChildFirst=false;
            }
            tempParentJntCompletionCheckList[parentJnt] = 1;
        }
    }

    modelAllAlienJoints_TypeGuided();
    modelAllAlienBones_TypeGuided();

    // Now apply the relative scalings using the BP_SizeComparedToBackBone_DNA

    // Now perform the bulking up effects using the Skeletal_BP_Bulkiness_2_DNA



    // +++++ Test for BP scaling. (Performing successfully) +++++
//    for (int bp=0; bp<this->alienList.last()->bodyParts.length(); bp++)
//    {
//        if (this->alienList.last()->bodyParts[bp]->BP_Type != AlienModeller::BodyPartsEnum::JOINT  &&  this->alienList.last()->bodyParts[bp]->BP_Type != AlienModeller::BodyPartsEnum::BONE)
//        {
//            GeometryAlgorithms::scaleBP(*(this->alienList.last()->bodyParts[bp]), 3, 3, 3);
////            break;
//        }
//    }
//    // +++++ Test for BP scaling. (Performing successfully) +++++
//    for (int bp=0; bp<this->alienList.last()->alienBackBones.length(); bp++)
//    {
//        QVector<Cell*> tempCellList;
//        tempCellList.resize(0);
//        BodyPart &tempBackBone = *(this->alienList.last()->alienBackBones[bp]);
//        getBodyPartCellList(&tempBackBone, tempCellList);

//        GeometryAlgorithms::scaleBP(tempCellList, tempBackBone, 1, 1, 1);
//    }

//    // +++++ Test for BP rotations (Performing okay) +++++
//    for (int bp=0; bp<this->alienList.last()->alienBackBones.length(); bp++)
//    {
//        GeometryAlgorithms::rotateBodyPartTowardsVecDir(*(this->alienList.last()->alienBackBones[bp]), QVector3D(1,1,1));
//    }

//    // +++++ Test for BP tranlation (Performing successfully) +++++
//    for (int bp=0; bp<this->alienList.last()->alienBackBones.length(); bp++)
//    {
//        GeometryAlgorithms::translateBP(*(this->alienList.last()->alienBackBones[bp]), 500, 500, 500);
//    }


    // +++++++ Arrange all BPs so that they are attached to their designated locations of the BackBone. TODO(ismail): Implement more geometric algorithms that can transform an entire BP
}

*/

/*
// Might need to deprecate this method
void AlienModeller::initializeAlienModeller()
{
    // initialize the Skeleton_RuleSet
    QVector<QPoint> tempList;
    int minJnts=10, maxJnts=50; // choose a range of joints
    int minBns=minJnts, maxBns=200; // TODO(ismail): seems irrelavant because later the min bones are the max joints and max are 3 times that of joints number selected
    tempList.append(QPoint(minJnts, maxJnts));
    tempList.append(QPoint(minBns, maxBns)); // TODO(ismail): seems irrelavant because later the min bones are the max joints and max are 3 times that of joints number selected
    skeletonRuleSet->setSkeletonRuleSet(tempList);

    // initialize the Joint_RuleSet
    tempList.resize(0);
    int minSubCells=0, maxSubCells=5;
    tempList.append(QPoint(minSubCells, maxSubCells));
    jointRuleSet->setJointRuleSet(tempList);

    // initialize the Bone_RuleSet
    tempList.resize(0);
    int minBrPnts=0, maxBrPnts=100;
    int minCurvAmnt=0, maxCurvAmnt=90;
    int minLvlOfGrth=0, maxLvlOfGrth=10;
    int minParBns=0, maxParBns=2;
    int minSquashOrElongAmnt=10, maxSquashOrElongAmnt=600;
    int notGotMirrBranches=0, gotMirrBranches=1;
    tempList.append(QPoint(minBrPnts, maxBrPnts));
    tempList.append(QPoint(minCurvAmnt, maxCurvAmnt));
    tempList.append(QPoint(minLvlOfGrth, maxLvlOfGrth));
    tempList.append(QPoint(minParBns, maxParBns));
    tempList.append(QPoint(minSquashOrElongAmnt, maxSquashOrElongAmnt));
    tempList.append(QPoint(notGotMirrBranches, gotMirrBranches));
    boneRuleSet->setBoneRuleSet(tempList);

    // initialize the Ridges Ruleset (GLOBAL)
    tempList.resize(0);
    int minRidges=3, maxRidges=20;
    int ridgeAlignmentIrregular= 0, ridgeAlignmentRegular= 5; // if -ve number then alignment is irregular, otherwise its regular
    tempList.append(QPoint(minRidges, maxRidges));
    tempList.append(QPoint(ridgeAlignmentIrregular, ridgeAlignmentRegular));
    ridgeRuleSetPtr->setRidgeRulesetBrackets_global(tempList);

    // initialize the Ridges Ruleset (LOCAL)
    tempList.resize(0);
    int minNumFacesForRidge=1, maxNumFacesForRidge=5;
    int minRidgeThickness=5, maxRidgeThickness=20;  // thickness of ridge (5% to 20% of the thickness of the face that this
                                                    // ... ridge appears on)
    int minDistanceBetweenRidges=10, maxDistanceBetweenRidges=50; // distance b/w each ridge (10% to 50% of the average
                                                                  // ... thickness of the previous ridge)
    int minRidgeDisturbance=0, maxRidgeDisturbance=50; // (b/w 0% and 50% of the thickness of the ridge)
    tempList.append(QPoint(minNumFacesForRidge, maxNumFacesForRidge));
    tempList.append(QPoint(minDistanceBetweenRidges, maxDistanceBetweenRidges));
    tempList.append(QPoint(minRidgeThickness, maxRidgeThickness));
    tempList.append(QPoint(minRidgeDisturbance, maxRidgeDisturbance));
    ridgeRuleSetPtr->setRidgeRulesetBrackets_local(tempList);
}
*/

/*
void AlienModeller::smoothenSkeletalBP(Alien *alien, BodyPart *BP_Ptr, QPolygon &Jnt2JntRelExtract, quint8 smoothnessAmnt)
{
    qDebug() << "BP_to_JAT_rootJoint_Map_DNA for source BP : " << alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.BP_to_JAT_rootJoint_Map_DNA.value(BP_Ptr->BP_ID);

    QVector<Joint*> tempJointsList; tempJointsList.clear();
    QVector<Joint*> tempJointsListToRemove; tempJointsListToRemove.clear();
    QVector<int> jntLineageList; jntLineageList.clear();
    convertJnt2JntPolyToJointsLineageList(&Jnt2JntRelExtract, jntLineageList);
    qDebug() << jntLineageList;

    if (jntLineageList.length() > 2)
    {
        // Get all the joints of this BP, so these can be removed from the Bookkeeping DNA DSs
        QVector<Joint *> BP_JntsList; BP_JntsList.clear();
        getAllBP_Joints(BP_Ptr, BP_JntsList);

        // clone the first and last joints, which will be needed later, because when the entire BP is deleted, the first and last joitns are also deleted
        int frtJntID = getBodyPartFromID(alien, jntLineageList[0])->BP_ID;
        int lstJntID = getBodyPartFromID(alien, jntLineageList.last())->BP_ID;
        Joint *frtJntPtr = instantiateNewJoint(alien);
        Joint *lstJntPtr = instantiateNewJoint(alien);
        // create a cell for this joint
        frtJntPtr->makeCell(getBodyPartFromID(alien, jntLineageList[0])->getPivotPoint(), 2); // create the joint's cell
        lstJntPtr->makeCell(getBodyPartFromID(alien, jntLineageList.last())->getPivotPoint(), 2); // create the joint's cell
        GeometryAlgorithms::rotateBodyPartTowardsVecDir(*frtJntPtr, getBodyPartFromID(alien, jntLineageList[0])->getPivotVector().normalized()); // TODO(ismail): Use a unit vector here
        GeometryAlgorithms::rotateBodyPartTowardsVecDir(*lstJntPtr, getBodyPartFromID(alien, jntLineageList.last())->getPivotVector().normalized()); // TODO(ismail): Use a unit vector here

        for (Joint *jntPtr : BP_JntsList)
        {
            // TODO(ismail): Make seperate methods that can remove and add new Joint entries to the following DS
            alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.VectorEnergyAngleTrends_TypeGuided.remove(jntPtr->BP_ID);
            alien->alienSkeleton.vectorLocationCoupleForEachJoint_TypeGuided.remove(jntPtr->BP_ID);
            alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.randIntForBookKeeping_EnergyTrend.remove(jntPtr->BP_ID);
            alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.randFloatForBookKeeping_FinalEnergy.remove(jntPtr->BP_ID);
//            alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.randIntForBookKeeping_AngleTrend.remove(jntPtr->BP_ID);
            alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.randIntForBookKeeping_quaterEulerAngleSelectorForFirstChild.remove(jntPtr->BP_ID);
            alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.randQVector3DForBookKeeping_quaterEulerAngleSelectorForNotFirstChild.remove(jntPtr->BP_ID);

            int tempKey = alien->alienSkeleton.JntToJntMappings_ForBookKeeping.key(jntPtr->BP_ID, -2);
            if (tempKey != -2)
                alien->alienSkeleton.JntToJntMappings_ForBookKeeping.remove(tempKey);
        }

        tempJointsList.append(dynamic_cast<Joint*>(getBodyPartFromID(alien, jntLineageList[0])));
//        qDebug() << tempJointsList[0]->BP_ID;

        // ++ SMOOTHING ALGORITHM 2 -- Now subdivide the lineages using your own algorithm. ++
        QVector<int> jntLineageList_smoothed_prev; jntLineageList_smoothed_prev.clear();
        QVector<int> jntLineageList_smoothed; jntLineageList_smoothed.clear();
        for (int smCntr=0; smCntr<smoothnessAmnt; smCntr++)
        {
//            for (int jt=1; jt<tempJointsList.length(); jt++)
//            {
//                // also remove it from the alien
////                alien->bodyParts.removeAt(alien->bodyParts.indexOf(tempJointsList[jt]));
////                alien->alienJoints.removeAt(alien->alienJoints.indexOf(tempJointsList[jt]));
////                delete tempJointsList[jt];
//                tempJointsList.removeAt(jt);
////                if (jt == tempJointsList.length()-2)
////                    tempJointsList.removeLast();
//            }
            while (tempJointsList.length() != 1)
                tempJointsList.removeLast();

            Jnt2JntRelExtract.clear();
            if (smCntr == 0)
            {
                for (int i=0; i<jntLineageList.length(); i++)
                {
                    jntLineageList_smoothed.append(jntLineageList[i]);
                }
            }
            jntLineageList_smoothed_prev.resize(0);

            // copy the updated smoothed list in the previous one, which will undergo further smoothness
            for (int i=0; i<jntLineageList_smoothed.length(); i++)
            {
                jntLineageList_smoothed_prev.append(jntLineageList_smoothed[i]);
                if (smCntr>0 && i>0 && i<jntLineageList_smoothed.length()-1)
                {
                    tempJointsListToRemove.append(dynamic_cast<Joint*>
                                                  (getBodyPartFromID(alien, jntLineageList_smoothed_prev.last())));
//                    qDebug() << tempJointsListToRemove.last()->BP_ID;
                }
            }

            jntLineageList_smoothed.resize(0);

//            qDebug() << jntLineageList_smoothed_prev;
            for (int i=0; i<jntLineageList_smoothed_prev.length()-2; i++)
            {

                // Create a new list like jntLineageList. Call it jntLineageList_smoothed. Store the joint ID at jntLineageList[i][0] as the first element in each inner list.
//                jntLineageList_smoothed.append(jntLineageList_smoothed_prev[i]);
                int tempJntId_2, tempJntId_3; // IDs of the joints taken from the jntLineageList_smoothed_prev
    //            // select a random number for the amount of intermediate points to be computed
    //            int randNumInterPoints = GeometryAlgorithms::getRandomInteger(6,7);
    //            qDebug() << "number of subdivisions: " << randNumInterPoints;

                // Get the locations of the 3 adjacent joints from jntLineageList_smoothed_prev
                // ... i.e. jntLineageList_smoothed_prev[i][j], jntLineageList_smoothed_prev[i][j+1], and jntLineageList_smoothed_prev[i][j+2]
                QVector3D jnt1_Loc = getBodyPartFromID(alien, jntLineageList_smoothed_prev[i])->getPivotPoint();
                QVector3D jnt2_Loc = getBodyPartFromID(alien, jntLineageList_smoothed_prev[i+1])->getPivotPoint();
                QVector3D jnt3_Loc = getBodyPartFromID(alien, jntLineageList_smoothed_prev[i+2])->getPivotPoint();

                // find a new Joint at a point at 75% of the first and second joint. Call it joint_1
                QVector3D newTempInternalJnt_1 = jnt1_Loc + (0.75*(jnt2_Loc-jnt1_Loc));
                // create a joint for it and append it to the alienJoints list.
                Joint *newJntPtr = instantiateNewJoint(alien);
                tempJointsList.append(newJntPtr);
                // create a cell for this joint
                newJntPtr->makeCell(newTempInternalJnt_1, 1); // create the joint's cell
                GeometryAlgorithms::rotateBodyPartTowardsVecDir(*newJntPtr, jnt2_Loc-jnt1_Loc); // TODO(ismail): Use a unit vector here

                // Add the new joint's angle trend
                alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.randIntForBookKeeping_AngleTrend.insert(newJntPtr->BP_ID,
                      alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.randIntForBookKeeping_AngleTrend.value(jntLineageList_smoothed_prev[i+2]));
                // remove the angle trend of the old joint
                alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.randIntForBookKeeping_AngleTrend.remove(jntLineageList_smoothed_prev[i+1]);

                tempJntId_2 = newJntPtr->BP_ID;
                // update the Jnt2JntRelExtract DS
                if (i==0)
                {
                    Jnt2JntRelExtract.append(QPoint(jntLineageList_smoothed_prev[0],tempJntId_2));
                    jntLineageList_smoothed.append(jntLineageList_smoothed_prev[0]);
                }
                else
                    Jnt2JntRelExtract.append(QPoint(jntLineageList_smoothed.last(),tempJntId_2));
                // append this joint's IDs in tempSmoothedList.
                jntLineageList_smoothed.append(tempJntId_2);


                // find a new Joint at a point at 100-75% of the second and third joint. Call it joint_2
                QVector3D newTempInternalJnt_2 = jnt2_Loc + (0.25*(jnt3_Loc-jnt2_Loc));
                // create a joint for it and append it to the alienJoints list.
                newJntPtr = instantiateNewJoint(alien);
                tempJointsList.append(newJntPtr);
                // create a cell for this joint
                newJntPtr->makeCell(newTempInternalJnt_2, 1); // create the joint's cell
                GeometryAlgorithms::rotateBodyPartTowardsVecDir(*newJntPtr, jnt3_Loc-jnt2_Loc); // TODO(ismail): Use a unit vector here

                // add the new joint's angle trend
                alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.randIntForBookKeeping_AngleTrend.insert(newJntPtr->BP_ID,
                      alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.randIntForBookKeeping_AngleTrend.value(jntLineageList_smoothed_prev[i+2]));
                // remove the old joint's angle trend
//                alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.randIntForBookKeeping_AngleTrend.remove(jntLineageList_smoothed_prev[i+2]);

                tempJntId_3 = newJntPtr->BP_ID;
                // append this joint's IDs in tempSmoothedList.
                jntLineageList_smoothed.append(tempJntId_3);
                // update the Jnt2JntRelExtract DS
                Jnt2JntRelExtract.append(QPoint(tempJntId_2,tempJntId_3));
            }

            Jnt2JntRelExtract.append(QPoint(jntLineageList_smoothed.last(),jntLineageList_smoothed_prev.last()));
            jntLineageList_smoothed.append(jntLineageList_smoothed_prev.last());
            tempJointsList.append(dynamic_cast<Joint*>(getBodyPartFromID(alien, jntLineageList_smoothed_prev.last())));
        }

        qDebug() << "jntLineageList_smoothed : " << jntLineageList_smoothed;
        qDebug() << "Jnt2JntRelExtract : " << Jnt2JntRelExtract;
//        for (Joint* j : tempJointsList)
//            qDebug() << " , " << j->BP_ID;

        for (int jt=0; jt<tempJointsListToRemove.length(); jt++)
        {
//            qDebug() << tempJointsListToRemove[jt]->BP_ID;
            // also remove it from the alien
            alien->bodyParts.removeAt(alien->bodyParts.indexOf(tempJointsListToRemove[jt]));
            alien->alienJoints.removeAt(alien->alienJoints.indexOf(tempJointsListToRemove[jt]));
            delete tempJointsListToRemove[jt];
        }
        tempJointsListToRemove.clear();



        // Note down the ID, Type and Source ID of the BP
        int tempBP_ID = BP_Ptr->BP_ID;
        int tempSourceID = BP_Ptr->clonedFrom;
        int tempBP_Tyep = BP_Ptr->BP_Type;

        // Remove the BP from memory
        QVector<int> tempBP_ID_List_1; tempBP_ID_List_1.clear();
        tempBP_ID_List_1 << BP_Ptr->BP_ID;
        removeAlienBodyParts(alien, tempBP_ID_List_1);

        // Create a new BP and give it the same ID as the input BP
        instantiateNewBodyParts(alien, tempBP_Tyep, 1);
        BodyPart *newBP_Ptr = alien->bodyParts.last();
        newBP_Ptr->BP_ID = tempBP_ID;
        newBP_Ptr->clonedFrom = tempSourceID; // TODO(ismail): It should be checked that whether the source
                                              // ... still exists or not

        // Since the first and last joints were removed from this BP, we need to give their IDs to their clones below
        frtJntPtr->BP_ID = frtJntID;
        lstJntPtr->BP_ID = lstJntID;
        tempJointsList.removeFirst();
        tempJointsList.prepend(frtJntPtr);
        tempJointsList.removeLast();
        tempJointsList.append(lstJntPtr);

        // append all the newly created joints to this BP's subBPs
        for (Joint *jPtr : tempJointsList)
        {
//            qDebug() << jPtr->BP_ID;
//            if (!alien->bodyParts.contains(jPtr))
//                alien->bodyParts.append(jPtr);
//            if (!alien->alienJoints.contains(jPtr))
//                alien->alienJoints.append(jPtr);

            newBP_Ptr->subBodyParts.append(jPtr);
        }
        alien->alienSkeleton.Jnt2JntRelations_TypeGuided.insert(newBP_Ptr->BP_ID, new QPolygon(Jnt2JntRelExtract));

        // Update the energy trends in the randIntForBookKeeping_EnergyTrend DS because the energy trend
        QVector<int> patternReg; // 0=alternating; 1=irregular
        float prevEnergy = alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.randFloatForBookKeeping_FinalEnergy.value(Jnt2JntRelExtract[0].x());
        for (int jt=0; jt<Jnt2JntRelExtract.length(); jt++)
        {
            QVector3D firJntPos = getBodyPartFromID(alien, Jnt2JntRelExtract[jt].x())->getPivotPoint();
            QVector3D secJntPos = getBodyPartFromID(alien, Jnt2JntRelExtract[jt].y())->getPivotPoint();

            float energy = (secJntPos - firJntPos).length();
            alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.randFloatForBookKeeping_FinalEnergy.insert(Jnt2JntRelExtract[jt].y(), energy);
            if (energy > prevEnergy)
                alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.randIntForBookKeeping_EnergyTrend.insert(Jnt2JntRelExtract[jt].y(), 1);
            else
                alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.randIntForBookKeeping_EnergyTrend.insert(Jnt2JntRelExtract[jt].y(), -1);

            patternReg << alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.randIntForBookKeeping_EnergyTrend.value(Jnt2JntRelExtract[jt].y());

            if (jt > 0)
                prevEnergy = energy;
            else
                alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.BP_to_JAT_rootJoint_Map_DNA[newBP_Ptr->BP_ID][0].setY(energy);
        }

        float whichPatt = 0; // 0=ascending; 1=descending; 2=wavy up; 3=wavy down; 4=irregular
        for (int i=0; i<patternReg.length()-1; i++)
        {
            if (patternReg[i] == patternReg[i+1])
            {
                whichPatt = 4;
                break;
            }
            else
                whichPatt = 2;
        }

        if (whichPatt == 4)
        {
            if (patternReg[0] == 1 && !patternReg.contains(-1))
            {
                whichPatt = 0; // should be ascending
            }
            else if (patternReg[0] == -1 && !patternReg.contains(1))
            {
                whichPatt = 1; // should be descending
            }
        }

        alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.BP_to_JAT_rootJoint_Map_DNA[newBP_Ptr->BP_ID][0].setX(whichPatt);

        if (alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.BP_to_JAT_rootJoint_Map_DNA[newBP_Ptr->BP_ID][1].x() > 1)
            alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.BP_to_JAT_rootJoint_Map_DNA[newBP_Ptr->BP_ID][1].setX(4.0);


        // Now it is most likely that the energy trend pattern of the entire jnt2jnt lineage has been changed,
        // ... because the energies of the intermediate newly created joints would be lesser or greater than their parent
        // ... joints.
        // ... So we need to check using our boolean/binary logic algorithm what kind of energy pattern should be assigned
        // ... to the root node of the BP, which will be stored in the BP_JAT_rootJoint_DNA



        // Update the numOfJointsPerBP_DNA
        QPolygon tempPoly = alien->alienDNA->skeletonDNA->numOfJointsPerBP_DNA.value(newBP_Ptr->BP_Type);
        for (int j=0; j<tempPoly.length(); j++)
        {
            if (tempPoly[j].x() == newBP_Ptr->BP_ID)
            {
                tempPoly[j].setY(tempJointsList.length());
                break;
            }
        }
        alien->alienDNA->skeletonDNA->numOfJointsPerBP_DNA.insert(newBP_Ptr->BP_Type,tempPoly);


        // model the bones of this BP
        QMap<int, QPolygon*> tempJnt2JntRelations; tempJnt2JntRelations.clear();
        tempJnt2JntRelations.insert(newBP_Ptr->BP_ID, new QPolygon(Jnt2JntRelExtract));
        modelAllAlienBones_TypeGuided(alien, tempJnt2JntRelations);

        // Bulkify the BP
        manipulateSkeletalBulkinessDNA_Flags(alien, false, false, true, false, false, new QVector<int>(0), &tempBP_ID_List_1);

        buildSkeletalBP_BulkinessDNA(alien, tempBP_ID_List_1);
//        bulkUpBodyPart(alien, tempBP_ID_List_1);

        qDebug() << "BP smoothed successfully";
    }
    else
        qDebug() << "Cannot be smoothed as this BP has only two joints";
}

Old modify_JAT method
//    for (int i : BP_ID_List)
//    {
//        QPointF energyPt = QPointF(-1,-1), anglePt = QPointF(-1,-1);
//        BodyPart *inBP_Ptr = this->getBodyPartFromID(alien, i);

////        // remove this BP from the backbone's subBPs list
////        bool isThisBP_SubBP_OfBackBone = false;
////        if (!alien->alienBackBones.isEmpty())
////            if (alien->alienBackBones.last()->subBodyParts.contains(inBP_Ptr))
////                isThisBP_SubBP_OfBackBone = true;

//        if (inBP_Ptr->isSymmetric)
//        {
//            qDebug() << "this BP is a mirror";
////            inBP_Ptr = this->getBodyPartFromID(alien, inBP_Ptr->mirrorOf);
//            return;
//        }
//        else
//        {
//            qDebug() << "this BP is not a mirror";
//        }

//        // Check if this BP has been smoothed or not
//        // If it has been smoothed, then swap it with its original unsmoothed version in smoothingBackupCloneBPs, and remove it from the Alien
//        // smooth it again as many times as it was smoothed before
//        quint8 smoothLevel = inBP_Ptr->smoothnessLevel;
//        if (smoothLevel > 0)
//        {
//            // Basically here we're replacing the BP with its unsmoothed clone

//            BodyPart* tempClPtr = alien->smoothingBackupCloneBPs.value(inBP_Ptr->BP_ID);
//            alien->smoothingBackupCloneBPs.remove(inBP_Ptr->BP_ID);

//            QVector3D tempPos = inBP_Ptr->getPivotPoint();
//            QVector3D tempVec = inBP_Ptr->getPivotVector();

//            inBP_Ptr = replaceBP_WithAnotherBP(alien, inBP_Ptr, tempClPtr);

//            GeometryAlgorithms::rotateBodyPartTowardsVecDir(*(inBP_Ptr), tempVec);
//            GeometryAlgorithms::translateBP(inBP_Ptr,   tempPos.x() - inBP_Ptr->getPivotPoint().x(),
//                                                                    tempPos.y() - inBP_Ptr->getPivotPoint().y(),
//                                                                    tempPos.z() - inBP_Ptr->getPivotPoint().z());
//        }

//        int tempBP_ID = inBP_Ptr->BP_ID;
//        int tempSourceID = inBP_Ptr->clonedFrom;
//        QPair<int,int> tempSrcCloneFrom = inBP_Ptr->cloneFromAlienAndBP_Pair;
//        bool isSymmetric = inBP_Ptr->isSymmetric;
//        int mirroredFrom = inBP_Ptr->mirrorOf;

//        BodyPart *origBP_Ptr = inBP_Ptr;


////        if (eneryVals == QPointF(-1,-1) && angleVals == QPointF(-1,-1))
////        {
//            if (skeletonRuleSet->jointsArrangementTrends_RS.contains(inBP_Ptr->BP_Type))
//            {
//                QPointF tPt1, tPt2;
//                QPolygon tempPoly = skeletonRuleSet->jointsArrangementTrends_RS.value(inBP_Ptr->BP_Type);
//                // expand the likelihoods of the 'enerygy trends' and 'angle trends'
//                QVector<int> tempLikelihoodExpansionList_1, tempLikelihoodExpansionList_2;
//                tempLikelihoodExpansionList_1.resize(0); tempLikelihoodExpansionList_2.resize(0);
//                for (int j=0; j<6; j++)
//                {
//                    for (int k=0; k<tempPoly[j].y(); k++) // expand tempLikelihoodExpansionList_1 for energy trends
//                    {
//                        tempLikelihoodExpansionList_1.append(tempPoly[j].x());
//                    }
//                    for (int k=0; k<tempPoly[j+6].y(); k++) // expand tempLikelihoodExpansionList_2 for angle trends
//                    {
//                        tempLikelihoodExpansionList_2.append(tempPoly[j+6].x());
//                    }
//                }
//                // Select a random energy trend from the expanded energy trends
//                // select a starting random energy.
//                int tempRandInd_1 = GeometryAlgorithms::getRandomInteger(0, tempLikelihoodExpansionList_1.length()-1);
//                tPt1 = QPointF(
//                                static_cast<float>(tempLikelihoodExpansionList_1.at(tempRandInd_1)),
//                                GeometryAlgorithms::getRandomFloat(static_cast<float>(tempPoly[12].x()), static_cast<float>(tempPoly[12].y()))
//                                );
//                // Select a random angle trend from the expanded angle trends
//                // select a starting random angle
//                int tempRandInd_2 = GeometryAlgorithms::getRandomInteger(0, tempLikelihoodExpansionList_2.length()-1);
//                tPt2 = QPointF(
//                                static_cast<float>(tempLikelihoodExpansionList_2.at(tempRandInd_2)),
//                                GeometryAlgorithms::getRandomFloat(static_cast<float>(tempPoly[13].x()), static_cast<float>(tempPoly[13].y()))
//                                );

//                if (p1.first) // energy trend change required
//                    energyPt.setX(tPt1.x());
//                if (p1.second) // final energy of root joint change required
//                    energyPt.setY(tPt1.y());
//                if (p2.first) // angle trend change required
//                    anglePt.setX(tPt2.x());
//                if (p2.second) // final angle of root joint change required
//                    anglePt.setY(tPt2.y());
//            }
////        }
////        else
////        {
////            energyPt = eneryVals;
////            anglePt = angleVals;
////        }

//        // Now create a clone with the above JAT values
//        QVector3D tempPos = inBP_Ptr->getPivotPoint();
//        QVector3D tempVec = inBP_Ptr->getPivotVector();

//        inBP_Ptr = cloneSkeletalBP_FromAnotherAlien_viaDNA(alien, alien, inBP_Ptr, 1, false, energyPt, anglePt);

//        GeometryAlgorithms::rotateBodyPartTowardsVecDir(*(inBP_Ptr), tempVec);
//        GeometryAlgorithms::translateBP(inBP_Ptr,   tempPos.x() - inBP_Ptr->getPivotPoint().x(),
//                                                    tempPos.y() - inBP_Ptr->getPivotPoint().y(),
//                                                    tempPos.z() - inBP_Ptr->getPivotPoint().z());


//        BodyPart *tempMirrorBP_Ptr = nullptr;
//        if (isSymmetric) // symmetrify it
//        {
//            tempMirrorBP_Ptr = this->symmetrifyBP_AroundOppositeAttachmentFace(alien, inBP_Ptr);

//            // TODO(ismail): Put the code below inside the removeAlienBodyParts() method or make a seperate method for it
//            // We need to remove all the entries in the DNAs / book-keeping DNAs related to these joints
//            // First Get all the joints of this BP
////            QPolygonF tPolyF = alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.BP_to_JAT_rootJoint_Map_DNA.value(inBP_Ptr->BP_ID);
////            for (int p=0; p<alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.BP_JAT_rootJoint_DNA[inBP_Ptr->BP_Type].length()-1; p++)
////            {
////                if (tPolyF[0] == alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.BP_JAT_rootJoint_DNA[inBP_Ptr->BP_Type].at(p)
////                        &&
////                    tPolyF[1] == alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.BP_JAT_rootJoint_DNA[inBP_Ptr->BP_Type].at(p+1))
////                {
////                    alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.BP_JAT_rootJoint_DNA[inBP_Ptr->BP_Type].remove(p,2);
////                }
////            }
////            alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.BP_to_JAT_rootJoint_Map_DNA.remove(inBP_Ptr->BP_ID);
//            alien->alienDNA->skeletonDNA->BP_SizeComparedToBackBone_DNA[inBP_Ptr->BP_Type].removeLast();
//            alien->alienDNA->skeletonDNA->BP_AngleArrangementsFromBackBone_DNA[inBP_Ptr->BP_Type].removeLast();

//            removeAssociatedJATsAndOtherDSsForRemovableJnts(alien, getAllJntIDsOfBP(inBP_Ptr));

//            QVector<int> tempBP_ID_List_1; tempBP_ID_List_1.clear();
//            tempBP_ID_List_1 << inBP_Ptr->BP_ID;
//            removeAlienBodyParts(alien, tempBP_ID_List_1);
//        }

//        // TODO(ismail): Put the code below inside the removeAlienBodyParts() method or make a seperate method for it
//        // We need to remove all the entries in the DNAs / book-keeping DNAs related to these joints
//        // First Get all the joints of this BP
////        QPolygonF tPolyF = alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.BP_to_JAT_rootJoint_Map_DNA.value(origBP_Ptr->BP_ID);
////        for (int p=0; p<alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.BP_JAT_rootJoint_DNA[origBP_Ptr->BP_Type].length()-1; p++)
////        {
////            if (tPolyF[0] == alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.BP_JAT_rootJoint_DNA[origBP_Ptr->BP_Type].at(p)
////                    &&
////                tPolyF[1] == alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.BP_JAT_rootJoint_DNA[origBP_Ptr->BP_Type].at(p+1))
////            {
////                alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.BP_JAT_rootJoint_DNA[origBP_Ptr->BP_Type].remove(p,2);
////            }
////        }
////        alien->alienDNA->skeletonDNA->skeletal_JATs_DNA.BP_to_JAT_rootJoint_Map_DNA.remove(origBP_Ptr->BP_ID);
//        if (!alien->alienDNA->skeletonDNA->BP_SizeComparedToBackBone_DNA[origBP_Ptr->BP_Type].isEmpty())
//            alien->alienDNA->skeletonDNA->BP_SizeComparedToBackBone_DNA[origBP_Ptr->BP_Type].removeLast();
//        if (!alien->alienDNA->skeletonDNA->BP_AngleArrangementsFromBackBone_DNA[origBP_Ptr->BP_Type].isEmpty())
//            alien->alienDNA->skeletonDNA->BP_AngleArrangementsFromBackBone_DNA[origBP_Ptr->BP_Type].removeLast();

//        removeAssociatedJATsAndOtherDSsForRemovableJnts(alien, getAllJntIDsOfBP(origBP_Ptr));

//        // if this is a backbone, then detach its subBPs other than joints and bone
//        // ... before removing it
//        QVector<BodyPart*> tempSubBPs; tempSubBPs.clear();
//        detachSubBPsFromParentBPs(alien, origBP_Ptr, tempSubBPs);

//        QVector<int> tempBP_ID_List_1; tempBP_ID_List_1.clear();
//        tempBP_ID_List_1 << origBP_Ptr->BP_ID;
//        removeAlienBodyParts(alien, tempBP_ID_List_1);

//        if (inBP_Ptr != nullptr)
//        {
//            // sort out the Jnt2JntRel DS
//            QPolygon *newPoly = new QPolygon(*(alien->alienSkeleton.Jnt2JntRelations_TypeGuided.value(inBP_Ptr->BP_ID)));
//            alien->alienSkeleton.Jnt2JntRelations_TypeGuided.insert(tempBP_ID, newPoly);
//            alien->alienSkeleton.Jnt2JntRelations_TypeGuided.remove(inBP_Ptr->BP_ID);

//            inBP_Ptr->BP_ID = tempBP_ID;
//            inBP_Ptr->clonedFrom = tempSourceID;
//            inBP_Ptr->cloneFromAlienAndBP_Pair = tempSrcCloneFrom;
//            inBP_Ptr->isSymmetric = isSymmetric;
//            inBP_Ptr->mirrorOf = mirroredFrom;

//            QPolygon *Jnt2JntRelExtract = new QPolygon(*(alien->alienSkeleton.Jnt2JntRelations_TypeGuided.value(inBP_Ptr->BP_ID)));
//            // Do the smoothing
//            smoothenSkeletalBP(alien, inBP_Ptr, Jnt2JntRelExtract, smoothLevel);
//        }
//        else if (tempMirrorBP_Ptr != nullptr)
//        {
//            QPolygon *newPoly = new QPolygon(*(alien->alienSkeleton.Jnt2JntRelations_TypeGuided.value(tempMirrorBP_Ptr->BP_ID)));
//            alien->alienSkeleton.Jnt2JntRelations_TypeGuided.insert(tempBP_ID, newPoly);
//            alien->alienSkeleton.Jnt2JntRelations_TypeGuided.remove(tempMirrorBP_Ptr->BP_ID);

//            tempMirrorBP_Ptr->BP_ID = tempBP_ID;
//            tempMirrorBP_Ptr->clonedFrom = tempSourceID;
//            tempMirrorBP_Ptr->cloneFromAlienAndBP_Pair = tempSrcCloneFrom;
//            tempMirrorBP_Ptr->isSymmetric = isSymmetric;
//            tempMirrorBP_Ptr->mirrorOf = mirroredFrom;

//            QPolygon *Jnt2JntRelExtract = new QPolygon(*(alien->alienSkeleton.Jnt2JntRelations_TypeGuided.value(tempMirrorBP_Ptr->BP_ID)));
//            // Do the smoothing
//            smoothenSkeletalBP(alien, tempMirrorBP_Ptr, Jnt2JntRelExtract, smoothLevel);
//        }

//        attachAllSkeletalBodyPartsToBackBone(alien);

////        // If the BP has any sub BPs that were detached, these need to be cloned and the clones need to be attached
////        // ... to the BP
////        QVector<int> tempSubBPsIDs; tempSubBPsIDs.clear();
////        QVector<BodyPart*> tempClonedBPs; tempClonedBPs.clear();
////        for (int s=0; s<tempSubBPs.length(); s++)
////        {
////            // clone each BP
////            if (tempSubBPs[s]->BP_Type != AlienModeller::BodyPartsEnum::JOINT  &&
////                    tempSubBPs[s]->BP_Type != AlienModeller::BodyPartsEnum::BONE)
////            {
////                qDebug() << "reached modJAT - 1";
////                if (!tempSubBPs[s]->isSymmetric)
////                {
////                    tempSubBPsIDs << tempSubBPs[s]->BP_ID;
////                    tempClonedBPs.append(cloneSkeletalBP_FromAnotherAlien_viaDNA(alien, alien, tempSubBPs[s]));
////                }
////                else // we need to symmertify this BP around the BB
////                {
////                    tempSubBPsIDs << tempSubBPs[s]->BP_ID;
////                }
////            }
////            else
////                exit(0);
////            if (s == tempSubBPs.length()-1) // if this is the last loop iteration, then remove all the previous BPs,
////                                            // ... assign their IDs to the cloned BPs, and then attach the clones back to
////                                            // ... the backbone
////            {
////                // replace the detached sub BPs with the new clones

////                removeAlienBodyParts(alien, tempSubBPsIDs);

////                attachAllSkeletalBodyPartsToBackBone(alien);

////                for (BodyPart *bp : tempClonedBPs)
////                {
////                    alien->alienSkeleton.Jnt2JntRelations_TypeGuided.insert(bp->cloneFromAlienAndBP_Pair.second,
////                                            alien->alienSkeleton.Jnt2JntRelations_TypeGuided.value(bp->BP_ID));
////                    alien->alienSkeleton.Jnt2JntRelations_TypeGuided.remove(bp->BP_ID);

////                    bp->BP_ID = bp->cloneFromAlienAndBP_Pair.second;
////                    bp->cloneFromAlienAndBP_Pair = QPair<int,int>(-1,-1);
////                }
////            }
////        }

////        if (isThisBP_SubBP_OfBackBone)
////        {
////            alien->alienBackBones.last()->subBodyParts.append(inBP_Ptr);
////            updateBP_AttachmentToOtherBP_withAttachPositionAndVector_DNA(alien, -1,
////                                                                         alien->alienBackBones.last()->BP_ID,
////                                                                         inBP_Ptr->BP_ID, inBP_Ptr->getPivotPoint(),
////                                                                         inBP_Ptr->getPivotVector());
////        }
////        // TODO(ismail): Fix this as this doesn't work properly.
////        // ... The mirrored BPs don't appear
//////        if (inBP_Ptr->BP_Type == AlienModeller::BodyPartsEnum::BACK_BONE)
//////            symmetrifyAlienBodyParts(alien);
//    }


*/
