#include "skeleton_ruleset.h"

Skeleton_RuleSet::Skeleton_RuleSet()
{
    // Initialize the ruleset here using defaut values

    initJATsTrendsMap();
}

void Skeleton_RuleSet::setSkeletonRuleSet(QVector<QPoint> inBrackets)
{
    for (int i=0; i<inBrackets.length(); i++)
    {
        skeletonSpecsBrackets.append(QPoint(inBrackets[i].x(),inBrackets[i].y()));
    }
}

void Skeleton_RuleSet::initNumOfSkeletalBodyParts_RS()
{
    // initialize the numOfSkeletalBodyParts_RS
    numOfSkeletalBodyParts_RS.clear();
    QPolygon tempPolygon; tempPolygon.clear();
    // LOWER LIMB = 2 (BP_TYPE_ID)
    tempPolygon << QPoint(2,4)/*Biped*/ << QPoint(2,4)/*Quadped*/ << QPoint(3,8)/*Aquatic*/
                << QPoint(2,5)/*Amphibious*/ << QPoint(4,10)/*Octapedal*/ << QPoint(6,10)/*Anthropodal*/;
    numOfSkeletalBodyParts_RS.insert(2, tempPolygon);

    // UPPER LIMB = 3
    tempPolygon.resize(0);
    tempPolygon << QPoint(1,6)/*Biped*/ << QPoint(2,8)/*Quadped*/ << QPoint(0,3)/*Aquatic*/
                << QPoint(0,6)/*Amphibious*/ << QPoint(0,6)/*Octapedal*/ << QPoint(12,14)/*Anthropodal*/;
    numOfSkeletalBodyParts_RS.insert(3, tempPolygon);

    // BACK BONE = 4
    tempPolygon.resize(0);
    tempPolygon << QPoint(1,2)/*Biped*/ << QPoint(1,2)/*Quadped*/ << QPoint(1,2)/*Aquatic*/
                << QPoint(1,2)/*Amphibious*/ << QPoint(1,2)/*Octapedal*/ << QPoint(1,2)/*Anthropodal*/;
    numOfSkeletalBodyParts_RS.insert(4, tempPolygon);

    // TAIL = 5
    tempPolygon.resize(0);
    tempPolygon << QPoint(0,3)/*Biped*/ << QPoint(1,3)/*Quadped*/ << QPoint(0,3)/*Aquatic*/
                << QPoint(1,3)/*Amphibious*/ << QPoint(0,2)/*Octapedal*/ << QPoint(1,2)/*Anthropodal*/;
    numOfSkeletalBodyParts_RS.insert(5, tempPolygon);

    // HEAD = 6
    tempPolygon.resize(0);
    tempPolygon << QPoint(0,1)/*Biped*/ << QPoint(0,1)/*Quadped*/ << QPoint(0,1)/*Aquatic*/
                << QPoint(0,1)/*Amphibious*/ << QPoint(0,1)/*Octapedal*/ << QPoint(0,1)/*Anthropodal*/;
//    tempPolygon << QPoint(1,4)/*Biped*/ << QPoint(1,4)/*Quadped*/ << QPoint(0,4)/*Aquatic*/
//                << QPoint(1,4)/*Amphibious*/ << QPoint(1,6)/*Octapedal*/ << QPoint(1,6)/*Anthropodal*/;
    numOfSkeletalBodyParts_RS.insert(6, tempPolygon);

    // NECK = 9
    tempPolygon.resize(0);
    tempPolygon << QPoint(1,3)/*Biped*/ << QPoint(1,4)/*Quadped*/ << QPoint(0,4)/*Aquatic*/
                << QPoint(0,4)/*Amphibious*/ << QPoint(1,4)/*Octapedal*/ << QPoint(1,4)/*Anthropodal*/;
    numOfSkeletalBodyParts_RS.insert(9, tempPolygon);

    // RIB = 10
    tempPolygon.resize(0);
    tempPolygon << QPoint(0,1)/*Biped*/ << QPoint(0,1)/*Quadped*/ << QPoint(0,1)/*Aquatic*/
                << QPoint(0,1)/*Amphibious*/ << QPoint(0,1)/*Octapedal*/ << QPoint(0,1)/*Anthropodal*/;
    numOfSkeletalBodyParts_RS.insert(10, tempPolygon);


//    // HAND = 7
//    tempPolygon.resize(0);
//    tempPolygon << QPoint(2,4)/*Biped*/ << QPoint(2,4)/*Quadped*/ << QPoint(0,8)/*Aquatic*/
//                << QPoint(4,6)/*Amphibious*/ << QPoint(4,10)/*Octapedal*/ << QPoint(6,10)/*Anthropodal*/;
    //    numOfSkeletalBodyParts_RS.insert(7, tempPolygon);
}

void Skeleton_RuleSet::addNewBP_TypeEntryToNumOfSkeletalBodyParts_RS(int BP_TypeID, const QPolygon &newEntryPoly)
{
    numOfSkeletalBodyParts_RS.insert(BP_TypeID, newEntryPoly);
}

void Skeleton_RuleSet::setNumOfSkeletalBodyParts_RS(QMap<int, QPolygon> numOfSkeletalBodyParts_RS)
{
    numOfSkeletalBodyParts_RS = QMap<int, QPolygon>(numOfSkeletalBodyParts_RS);
}

void Skeleton_RuleSet::modifyNumOfSkeletalBodyParts_RS(int alienType, QMap<int, QPair<int, int> > tempBP_AmntMap)
{
    QMapIterator<int, QPair<int, int> > iter(tempBP_AmntMap);
    // go through the incoming user specified min and max values
    while (iter.hasNext())
    {
        iter.next();
        // if the user specified a greater value for min than max, then make min one less than max
        QPair<int,int> tempPair = iter.value();
        if (tempPair.first >= tempPair.second)
            tempPair.first = tempPair.second - 1;
        // Make sure the min num of BPs must not be less than 0
        if (tempPair.first < 1)
            tempPair.first = 0;
        // Now store it in the ruleset
        numOfSkeletalBodyParts_RS[iter.key()].setPoint(alienType, QPoint(tempPair.first, tempPair.second));
    }
}

void Skeleton_RuleSet::modifyMinMaxJntsForBodyParts_RS(int alienType, QMap<int, QPoint> tempJnts_AmntMap)
{
    QMapIterator<int, QPoint> iter(tempJnts_AmntMap);
    // go through the incoming user specified min and max values
    while (iter.hasNext())
    {
        iter.next();
        // if the user specified a greater value for min than max, then make min one less than max
        // Ensure the min num of jnts must not be below 3
        QPoint tempPnt = iter.value();
        if (tempPnt.x() >= tempPnt.y())
        {
            tempPnt.setX(tempPnt.y() - 1);
            if (tempPnt.x() < 3)
                tempPnt.setX(3);
        }
        // Now store it in the ruleset
        numOfJointsPerBP_RS[iter.key()].setPoint(alienType, tempPnt);
    }
}

QMap<int, QPolygon> Skeleton_RuleSet::getNumOfSkeletalBodyParts_RS()
{
    return numOfSkeletalBodyParts_RS;
}

void Skeleton_RuleSet::addNewEntryToNumOfJointsPerBP_RS(int BP_TypeID, const QPolygon &newEntryPoly)
{
    numOfJointsPerBP_RS.insert(BP_TypeID, newEntryPoly);
}

void Skeleton_RuleSet::initJointsArrangementTrends_RS()
{
    // initialize the JAT_RS ... Joints Arrangements Trends Ruleset

    jointsArrangementTrends_RS.clear();
    // LOWER LIMB = 2
    QPolygon tempPolygon;
    tempPolygon.resize(0);
    tempPolygon /*Energy trends (First 6 pairs) ... Each pair is the (trend specifier, likelihood) */
        << QPoint(0,4)/*Ascending*/ << QPoint(1,4)/*Descending*/ << QPoint(2,4)/*Wavy up*/ << QPoint(3,4)/*Wavy down*/ << QPoint(4,4)/*irregular (ascending or descending)*/ << QPoint(5,4)/*constant*/
        /*Angle trends (next 6 pairs) ... Each pair is the (trend specifier, likelihood) */
        << QPoint(0,4)/*Ascending*/ << QPoint(1,4)/*Descending*/ << QPoint(2,1)/*Wavy up*/ << QPoint(3,1)/*Wavy down*/ << QPoint(4,4)/*irregular (ascending or descending)*/ << QPoint(5,0)/*constant*/
        /*Energy range (13th pair) ... This is the multiplier of the previous energy*/
        << QPoint(1,5)/*Energy range*/
        /*Angle range (14th pair) */
        << QPoint(10,90)/*Angle range ... try (10, 170)*/;
    jointsArrangementTrends_RS.insert(2, tempPolygon);

    // UPPER LIMB = 3
    tempPolygon.resize(0);
    tempPolygon /*Energy trends (First 6 pairs) ... Each pair is the (trend specifier, likelihood) */
        << QPoint(0,4)/*Ascending*/ << QPoint(1,4)/*Descending*/ << QPoint(2,4)/*Wavy up*/ << QPoint(3,4)/*Wavy down*/ << QPoint(4,4)/*irregular (ascending or descending)*/ << QPoint(5,2)/*constant*/
        /*Angle trends (next 6 pairs) ... Each pair is the (trend specifier, likelihood) */
        << QPoint(0,4)/*Ascending*/ << QPoint(1,4)/*Descending*/ << QPoint(2,1)/*Wavy up*/ << QPoint(3,1)/*Wavy down*/ << QPoint(4,4)/*irregular (ascending or descending)*/ << QPoint(5,0)/*constant*/
        /*Energy range (13th pair) ... This is the multiplier of the previous energy*/
        << QPoint(1,3)/*Energy range*/
        /*Angle range (14th pair) */
        << QPoint(20,90)/*Angle range ... try (10, 170)*/;
    jointsArrangementTrends_RS.insert(3, tempPolygon);

    // BACK BONE = 4
    tempPolygon.resize(0);
    tempPolygon /*Energy trends (First 6 pairs) ... Each pair is the (trend specifier, likelihood) */
        << QPoint(0,4)/*Ascending*/ << QPoint(1,4)/*Descending*/ << QPoint(2,4)/*Wavy up*/ << QPoint(3,4)/*Wavy down*/ << QPoint(4,4)/*irregular (ascending or descending)*/ << QPoint(5,2)/*constant*/
        /*Angle trends (next 6 pairs) ... Each pair is the (trend specifier, likelihood) */
        << QPoint(0,4)/*Ascending*/ << QPoint(1,4)/*Descending*/ << QPoint(2,1)/*Wavy up*/ << QPoint(3,1)/*Wavy down*/ << QPoint(4,4)/*irregular (ascending or descending)*/ << QPoint(5,0)/*constant*/
        /*Energy range (13th pair) ... This is the multiplier of the previous energy*/
        << QPoint(1,3)/*Energy range.. try (1,3)*/
        /*Angle range (last pair) */
        << QPoint(10,50)/*Angle range ... try (10, 170)*/;
    jointsArrangementTrends_RS.insert(4, tempPolygon);

    // TAIL = 5
    tempPolygon.resize(0);
    tempPolygon /*Energy trends (First 5 pairs) ... Each pair is the (trend specifier, likelihood) */
        << QPoint(0,4)/*Ascending*/ << QPoint(1,4)/*Descending*/ << QPoint(2,4)/*Wavy up*/ << QPoint(3,4)/*Wavy down*/ << QPoint(4,4)/*irregular (ascending or descending)*/ << QPoint(5,2)/*constant*/
        /*Angle trends (next 6 pairs) ... Each pair is the (trend specifier, likelihood) */
        << QPoint(0,4)/*Ascending*/ << QPoint(1,4)/*Descending*/ << QPoint(2,0)/*Wavy up*/ << QPoint(3,0)/*Wavy down*/ << QPoint(4,4)/*irregular (ascending or descending)*/ << QPoint(5,0)/*constant*/
        /*Energy range (2nd last pair) ... This is the multiplier of the previous energy*/
        << QPoint(1,3)/*Energy range*/
        /*Angle range (last pair) */
        << QPoint(30,50)/*Angle range ... try (10, 170)*/;
    jointsArrangementTrends_RS.insert(5, tempPolygon);

    // HEAD = 6
    tempPolygon.resize(0);
    tempPolygon /*Energy trends (First 6 pairs) ... Each pair is the (trend specifier, likelihood) */
        << QPoint(0,0)/*Ascending*/ << QPoint(1,1)/*Descending*/ << QPoint(2,0)/*Wavy up*/ << QPoint(3,0)/*Wavy down*/ << QPoint(4,0)/*irregular (ascending or descending)*/ << QPoint(5,3)/*constant*/
        /*Angle trends (next 6 pairs) ... Each pair is the (trend specifier, likelihood) */
        << QPoint(0,2)/*Ascending*/ << QPoint(1,2)/*Descending*/ << QPoint(2,0)/*Wavy up*/ << QPoint(3,0)/*Wavy down*/ << QPoint(4,2)/*irregular (ascending or descending)*/ << QPoint(5,0)/*constant*/
        /*Energy range (13th pair) ... This is the multiplier of the previous energy*/
        << QPoint(1,3)/*Energy range.. try (1,3)*/
        /*Angle range (last pair) */
        << QPoint(5,30)/*Angle range ... try (10, 170)*/;
    // tempPolygon /*Energy trends (First 5 pairs) ... Each pair is the (trend specifier, likelihood) */
    //     << QPoint(0,0)/*Ascending*/ << QPoint(1,0)/*Descending*/ << QPoint(2,0)/*Wavy up*/ << QPoint(3,0)/*Wavy down*/ << QPoint(4,0)/*irregular (ascending or descending)*/ << QPoint(5,4)/*constant*/
    //     /*Angle trends (next 5 pairs) ... Each pair is the (trend specifier, likelihood) */
    //     << QPoint(0,0)/*Ascending*/ << QPoint(1,4)/*Descending*/ << QPoint(2,0)/*Wavy up*/ << QPoint(3,0)/*Wavy down*/ << QPoint(4,0)/*irregular (ascending or descending)*/ << QPoint(5,0)/*constant*/
    //     /*Energy range (2nd last pair) ... This is the multiplier of the previous energy*/
    //     << QPoint(1,3)/*Energy range*/
    //     /*Angle range (last pair) */
    //     << QPoint(9,11)/*Angle range ... try (10, 170)*/;
    jointsArrangementTrends_RS.insert(6, tempPolygon);

    // HAND = 7
    tempPolygon.resize(0);
    tempPolygon /*Energy trends (First 5 pairs) ... Each pair is the (trend specifier, likelihood) */
        << QPoint(0,4)/*Ascending*/ << QPoint(1,4)/*Descending*/ << QPoint(2,3)/*Wavy up*/ << QPoint(3,2)/*Wavy down*/ << QPoint(4,100)/*irregular (ascending or descending)*/ << QPoint(5,1)/*constant*/
        /*Angle trends (next 5 pairs) ... Each pair is the (trend specifier, likelihood) */
        << QPoint(0,4)/*Ascending*/ << QPoint(1,4)/*Descending*/ << QPoint(2,3)/*Wavy up*/ << QPoint(3,2)/*Wavy down*/ << QPoint(4,100)/*irregular (ascending or descending)*/ << QPoint(5,1)/*constant*/
        /*Energy range (2nd last pair) ... This is the multiplier of the previous energy*/
        << QPoint(1,3)/*Energy range*/
        /*Angle range (last pair) */
        << QPoint(30,50)/*Angle range ... try (10, 170)*/;
    jointsArrangementTrends_RS.insert(7, tempPolygon);

    // FOOT = 8
    tempPolygon.resize(0);
    tempPolygon /*Energy trends (First 5 pairs) ... Each pair is the (trend specifier, likelihood) */
        << QPoint(0,4)/*Ascending*/ << QPoint(1,4)/*Descending*/ << QPoint(2,3)/*Wavy up*/ << QPoint(3,2)/*Wavy down*/ << QPoint(4,100)/*irregular (ascending or descending)*/ << QPoint(5,1)/*constant*/
        /*Angle trends (next 5 pairs) ... Each pair is the (trend specifier, likelihood) */
        << QPoint(0,4)/*Ascending*/ << QPoint(1,4)/*Descending*/ << QPoint(2,3)/*Wavy up*/ << QPoint(3,2)/*Wavy down*/ << QPoint(4,100)/*irregular (ascending or descending)*/ << QPoint(5,1)/*constant*/
        /*Energy range (2nd last pair) ... This is the multiplier of the previous energy*/
        << QPoint(1,3)/*Energy range*/
        /*Angle range (last pair) */
        << QPoint(30,50)/*Angle range ... try (10, 170)*/;
    jointsArrangementTrends_RS.insert(8, tempPolygon);

    // NECK = 9
    tempPolygon.resize(0);
    tempPolygon /*Energy trends (First 5 pairs) ... Each pair is the (trend specifier, likelihood) */
        << QPoint(0,4)/*Ascending*/ << QPoint(1,4)/*Descending*/ << QPoint(2,1)/*Wavy up*/ << QPoint(3,1)/*Wavy down*/ << QPoint(4,100)/*irregular (ascending or descending)*/ << QPoint(5,0)/*constant*/
        /*Angle trends (next 5 pairs) ... Each pair is the (trend specifier, likelihood) */
        << QPoint(0,4)/*Ascending*/ << QPoint(1,4)/*Descending*/ << QPoint(2,1)/*Wavy up*/ << QPoint(3,1)/*Wavy down*/ << QPoint(4,100)/*irregular (ascending or descending)*/ << QPoint(5,0)/*constant*/
        /*Energy range (2nd last pair) ... This is the multiplier of the previous energy*/
        << QPoint(1,3)/*Energy range*/
        /*Angle range (last pair) */
        << QPoint(30,50)/*Angle range ... try (10, 170)*/;
    //                /*Curviness pair (13th pair) */
    //                << QPoint(-100,100)/*Angle range ... try (10, 170)*/;
    jointsArrangementTrends_RS.insert(9, tempPolygon);

    // RIB = 10
    tempPolygon.resize(0);
    tempPolygon /*Energy trends (First 5 pairs) ... Each pair is the (trend specifier, likelihood) */
        << QPoint(0,0)/*Ascending*/ << QPoint(1,4)/*Descending*/ << QPoint(2,0)/*Wavy up*/ << QPoint(3,0)/*Wavy down*/ << QPoint(4,0)/*irregular (ascending or descending)*/ << QPoint(5,4)/*constant*/
        /*Angle trends (next 5 pairs) ... Each pair is the (trend specifier, likelihood) */
        << QPoint(0,0)/*Ascending*/ << QPoint(1,4)/*Descending*/ << QPoint(2,0)/*Wavy up*/ << QPoint(3,0)/*Wavy down*/ << QPoint(4,4)/*irregular (ascending or descending)*/ << QPoint(5,0)/*constant*/
        /*Energy range (2nd last pair) ... This is the multiplier of the previous energy*/
        << QPoint(1,3)/*Energy range*/
        /*Angle range (last pair) */
        << QPoint(30,70)/*Angle range ... try (10, 170)*/;
    //                /*Curviness pair (13th pair) */
    //                << QPoint(-100,100)/*Angle range ... try (10, 170)*/;
    jointsArrangementTrends_RS.insert(10, tempPolygon);

    // Head SubBP Maxilla = 11
    tempPolygon.resize(0);
    tempPolygon /*Energy trends (First 5 pairs) ... Each pair is the (trend specifier, likelihood) */
        << QPoint(0,0)/*Ascending*/ << QPoint(1,0)/*Descending*/ << QPoint(2,0)/*Wavy up*/ << QPoint(3,0)/*Wavy down*/ << QPoint(4,0)/*irregular (ascending or descending)*/ << QPoint(5,4)/*constant*/
        /*Angle trends (next 5 pairs) ... Each pair is the (trend specifier, likelihood) */
        << QPoint(0,4)/*Ascending*/ << QPoint(1,4)/*Descending*/ << QPoint(2,0)/*Wavy up*/ << QPoint(3,0)/*Wavy down*/ << QPoint(4,0)/*irregular (ascending or descending)*/ << QPoint(5,0)/*constant*/
        /*Energy range (2nd last pair) ... This is the multiplier of the previous energy*/
        << QPoint(1,3)/*Energy range*/
        /*Angle range (last pair) */
        << QPoint(5,15)/*Angle range ... try (10, 170)*/;
    //                /*Curviness pair (13th pair) */
    //                << QPoint(-100,100)/*Angle range*/;
    jointsArrangementTrends_RS.insert(11, tempPolygon);

    // Head SubBP Mandible = 12
    tempPolygon.resize(0);
    tempPolygon /*Energy trends (First 5 pairs) ... Each pair is the (trend specifier, likelihood) */
        << QPoint(0,0)/*Ascending*/ << QPoint(1,0)/*Descending*/ << QPoint(2,0)/*Wavy up*/ << QPoint(3,0)/*Wavy down*/ << QPoint(4,4)/*irregular (ascending or descending)*/ << QPoint(5,4)/*constant*/
        /*Angle trends (next 5 pairs) ... Each pair is the (trend specifier, likelihood) */
        << QPoint(0,0)/*Ascending*/ << QPoint(1,0)/*Descending*/ << QPoint(2,0)/*Wavy up*/ << QPoint(3,0)/*Wavy down*/ << QPoint(4,4)/*irregular (ascending or descending)*/ << QPoint(5,4)/*constant*/
        /*Energy range (2nd last pair) ... This is the multiplier of the previous energy*/
        << QPoint(1,3)/*Energy range*/
        /*Angle range (last pair) */
        << QPoint(30,70)/*Angle range ... try (10, 170)*/;
    //                /*Curviness pair (13th pair) */
    //                << QPoint(-100,100)/*Angle range*/;
    jointsArrangementTrends_RS.insert(12, tempPolygon);

    // Head SubBP Zygomatic = 13
    tempPolygon.resize(0);
    tempPolygon /*Energy trends (First 5 pairs) ... Each pair is the (trend specifier, likelihood) */
        << QPoint(0,0)/*Ascending*/ << QPoint(1,4)/*Descending*/ << QPoint(2,0)/*Wavy up*/ << QPoint(3,0)/*Wavy down*/ << QPoint(4,0)/*irregular (ascending or descending)*/ << QPoint(5,4)/*constant*/
        /*Angle trends (next 5 pairs) ... Each pair is the (trend specifier, likelihood) */
        << QPoint(0,0)/*Ascending*/ << QPoint(1,4)/*Descending*/ << QPoint(2,0)/*Wavy up*/ << QPoint(3,0)/*Wavy down*/ << QPoint(4,4)/*irregular (ascending or descending)*/ << QPoint(5,0)/*constant*/
        /*Energy range (2nd last pair) ... This is the multiplier of the previous energy*/
        << QPoint(1,3)/*Energy range*/
        /*Angle range (last pair) */
        << QPoint(10,40)/*Angle range*/;
    //                /*Curviness pair (13th pair) */
    //                << QPoint(-100,100)/*Angle range ... try (10, 170)*/;
    jointsArrangementTrends_RS.insert(13, tempPolygon);

    // Extremity Base Carpal = 17
    tempPolygon.resize(0);
    tempPolygon /*Energy trends (First 6 pairs) ... Each pair is the (trend specifier, likelihood) */
        << QPoint(0,4)/*Ascending*/ << QPoint(1,4)/*Descending*/ << QPoint(2,4)/*Wavy up*/ << QPoint(3,4)/*Wavy down*/ << QPoint(4,4)/*irregular (ascending or descending)*/ << QPoint(5,4)/*constant*/
        /*Angle trends (next 6 pairs) ... Each pair is the (trend specifier, likelihood) */
        << QPoint(0,4)/*Ascending*/ << QPoint(1,4)/*Descending*/ << QPoint(2,0)/*Wavy up*/ << QPoint(3,0)/*Wavy down*/ << QPoint(4,10)/*irregular (ascending or descending)*/ << QPoint(5,0)/*constant*/
        /*Energy range (13th pair) ... This is the multiplier of the previous energy*/
        << QPoint(1,2)/*Energy range*/
        /*Angle range (14th pair) */
        << QPoint(10,65)/*Angle range ... */;
    jointsArrangementTrends_RS.insert(17, tempPolygon);

    // Extremity Digit = 18
    tempPolygon.resize(0);
    tempPolygon /*Energy trends (First 6 pairs) ... Each pair is the (trend specifier, likelihood) */
        << QPoint(0,0)/*Ascending*/ << QPoint(1,4)/*Descending*/ << QPoint(2,0)/*Wavy up*/ << QPoint(3,0)/*Wavy down*/ << QPoint(4,0)/*irregular (ascending or descending)*/ << QPoint(5,4)/*constant*/
        /*Angle trends (next 6 pairs) ... Each pair is the (trend specifier, likelihood) */
        << QPoint(0,0)/*Ascending*/ << QPoint(1,0)/*Descending*/ << QPoint(2,0)/*Wavy up*/ << QPoint(3,0)/*Wavy down*/ << QPoint(4,4)/*irregular (ascending or descending)*/ << QPoint(5,0)/*constant*/
        /*Energy range (13th pair) ... This is the multiplier of the previous energy*/
        << QPoint(1,2)/*Energy range*/
        /*Angle range (14th pair) */
        << QPoint(10,65)/*Angle range ... */;
    // tempPolygon /*Energy trends (First 6 pairs) ... Each pair is the (trend specifier, likelihood) */
    //     << QPoint(0,4)/*Ascending*/ << QPoint(1,4)/*Descending*/ << QPoint(2,4)/*Wavy up*/ << QPoint(3,4)/*Wavy down*/ << QPoint(4,4)/*irregular (ascending or descending)*/ << QPoint(5,4)/*constant*/
    //     /*Angle trends (next 6 pairs) ... Each pair is the (trend specifier, likelihood) */
    //     << QPoint(0,4)/*Ascending*/ << QPoint(1,4)/*Descending*/ << QPoint(2,0)/*Wavy up*/ << QPoint(3,0)/*Wavy down*/ << QPoint(4,4)/*irregular (ascending or descending)*/ << QPoint(5,0)/*constant*/
    //     /*Energy range (13th pair) ... This is the multiplier of the previous energy*/
    //     << QPoint(1,2)/*Energy range*/
    //     /*Angle range (14th pair) */
    //     << QPoint(10,65)/*Angle range ... */;
    jointsArrangementTrends_RS.insert(18, tempPolygon);
}

void Skeleton_RuleSet::setJointsArrangementTrends_RS(QMap<int, QPolygon> jointsArrangementTrends_RS)
{
    jointsArrangementTrends_RS = QMap<int, QPolygon>(jointsArrangementTrends_RS);
}

void Skeleton_RuleSet::modifyJointsArrangementTrends_RS(int BP_Type, QPolygon valuePoly)
{
    // Set the energy trends with their likelihood
    if (valuePoly[0].x() != -1) // if energy trend is not random
    {
        for (int i=0; i<6; i++) // reset the likelihoods of energy trends to 0
        {
            jointsArrangementTrends_RS[BP_Type][i].setY(0);
        }
        jointsArrangementTrends_RS[BP_Type][valuePoly[0].x() + 0].setY(4); // now set this energy trend likelihood to 4
    }
    else
    {
        for (int i=0; i<6; i++) // if is random then all gets equal likelihood
        {
            jointsArrangementTrends_RS[BP_Type][i].setY(4);
        }
    }

    // Set the angle trends with their likelihood
    if (valuePoly[0].y() != -1) // if angle trend is not random
    {
        for (int i=6; i<12; i++)
        {
            jointsArrangementTrends_RS[BP_Type][i].setY(0);
        }
        jointsArrangementTrends_RS[BP_Type][valuePoly[0].y() + 6].setY(4);
    }
    else
    {
        for (int i=6; i<12; i++) // if it is random then all gets equal likelihood
        {
            jointsArrangementTrends_RS[BP_Type][i].setY(4);
        }
    }

    // set the energy range
    if (valuePoly[1].x() >= valuePoly[1].y())
    {
        jointsArrangementTrends_RS[BP_Type][12].setX(valuePoly[1].y()-1);
        jointsArrangementTrends_RS[BP_Type][12].setY(valuePoly[1].y());
    }
    else
    {
        jointsArrangementTrends_RS[BP_Type][12].setX(valuePoly[1].x());
        jointsArrangementTrends_RS[BP_Type][12].setY(valuePoly[1].y());
    }

    // set the angle range
    if (valuePoly[2].x() >= valuePoly[2].y())
    {
        jointsArrangementTrends_RS[BP_Type][13].setX(valuePoly[2].y()-1);
        jointsArrangementTrends_RS[BP_Type][13].setY(valuePoly[2].y());
    }
    else
    {
        jointsArrangementTrends_RS[BP_Type][13].setX(valuePoly[2].x());
        jointsArrangementTrends_RS[BP_Type][13].setY(valuePoly[2].y());
    }
}

QMap<int, QPolygon> Skeleton_RuleSet::getJointsArrangementTrends_RS()
{
    return jointsArrangementTrends_RS;
}

void Skeleton_RuleSet::initBP_AttachmentToBackBone_ZoneAndScattering_RS()
{
    BP_AttachmentToBackBone_ZoneAndScattering_RS.clear();
    // initializing the BP_AttachmentToBackBone_ZoneAndScattering_RS
    // ** LOWER LIMB = 2 (BP_TYPE_ID)
    QMap<int, QPolygonF> tempMap;
    QPolygonF tempPolygonF;
    tempMap.clear();
    tempPolygonF.resize(0);
    // BIPEDAL = 0
    tempPolygonF    /*First 5 points are for zones and likelihoods*/
                << QPointF(1,0)/*start zone*/ << QPointF(2,5)/*end zone*/ << QPointF(3,0)/*upper middle zone*/
                << QPointF(4,0)/*dead middle zone*/ << QPointF(5,2)/*lower middle zone*/
                    /*Last 5 points are for min and max scattering %ages (SP) for each zone*/
                << QPointF(0,1)/*start zone*/ << QPointF(5,20)/*end zone*/ << QPointF(0,1)/*upper middle zone*/
                << QPointF(0,1)/*dead middle zone*/ << QPointF(5,20)/*lower middle zone*/;
    tempMap.insert(0, tempPolygonF);
    tempPolygonF.resize(0);
    // QUADPEDAL = 1
    tempPolygonF    /*First 5 points are for zones and likelihoods*/
                << QPointF(1,0)/*start zone*/ << QPointF(2,5)/*end zone*/ << QPointF(3,0)/*upper middle zone*/
                << QPointF(4,1)/*dead middle zone*/ << QPointF(5,3)/*lower middle zone*/
                    /*Last 5 points are for min and max scattering %ages (SP) for each zone*/
                << QPointF(0,1)/*start zone*/ << QPointF(5,20)/*end zone*/ << QPointF(0,1)/*upper middle zone*/
                << QPointF(10,20)/*dead middle zone*/ << QPointF(10,20)/*lower middle zone*/;
    tempMap.insert(1, tempPolygonF);
    tempPolygonF.resize(0);
    // AQUATIC = 2
    tempPolygonF    /*First 5 points are for zones and likelihoods*/
                << QPointF(1,0)/*start zone*/ << QPointF(2,1)/*end zone*/ << QPointF(3,0)/*upper middle zone*/
                << QPointF(4,3)/*dead middle zone*/ << QPointF(5,4)/*lower middle zone*/
                    /*Last 5 points are for min and max scattering %ages (SP) for each zone*/
                << QPointF(0,1)/*start zone*/ << QPointF(5,40)/*end zone*/ << QPointF(0,1)/*upper middle zone*/
                << QPointF(10,30)/*dead middle zone*/ << QPointF(10,30)/*lower middle zone*/;
    tempMap.insert(2, tempPolygonF);
    tempPolygonF.resize(0);
    // AMPHIBIOUS = 3
    tempPolygonF    /*First 5 points are for zones and likelihoods*/
                << QPointF(1,0)/*start zone*/ << QPointF(2,5)/*end zone*/ << QPointF(3,0)/*upper middle zone*/
                << QPointF(4,1)/*dead middle zone*/ << QPointF(5,3)/*lower middle zone*/
                    /*Last 5 points are for min and max scattering %ages (SP) for each zone*/
                << QPointF(0,1)/*start zone*/ << QPointF(20,40)/*end zone*/ << QPointF(0,1)/*upper middle zone*/
                << QPointF(20,40)/*dead middle zone*/ << QPointF(20,40)/*lower middle zone*/;
    tempMap.insert(3, tempPolygonF);
    tempPolygonF.resize(0);
    // OCTAPEDAL = 4
    tempPolygonF    /*First 5 points are for zones and likelihoods*/
                << QPointF(1,0)/*start zone*/ << QPointF(2,5)/*end zone*/ << QPointF(3,0)/*upper middle zone*/
                << QPointF(4,1)/*dead middle zone*/ << QPointF(5,3)/*lower middle zone*/
                    /*Last 5 points are for min and max scattering %ages (SP) for each zone*/
                << QPointF(0,1)/*start zone*/ << QPointF(5,20)/*end zone*/ << QPointF(0,1)/*upper middle zone*/
                << QPointF(10,20)/*dead middle zone*/ << QPointF(10,20)/*lower middle zone*/;
    tempMap.insert(4, tempPolygonF);
    tempPolygonF.resize(0);
    // ANTHROPOD = 5
    tempPolygonF    /*First 5 points are for zones and likelihoods*/
                << QPointF(1,0)/*start zone*/ << QPointF(2,5)/*end zone*/ << QPointF(3,0)/*upper middle zone*/
                << QPointF(4,5)/*dead middle zone*/ << QPointF(5,3)/*lower middle zone*/
                    /*Last 5 points are for min and max scattering %ages (SP) for each zone*/
                << QPointF(0,1)/*start zone*/ << QPointF(65,95)/*end zone*/ << QPointF(0,1)/*upper middle zone*/
                << QPointF(65,95)/*dead middle zone*/ << QPointF(30,50)/*lower middle zone*/;
    tempMap.insert(5, tempPolygonF);
    BP_AttachmentToBackBone_ZoneAndScattering_RS.insert(2, tempMap);

    // ** UPPER_LIMB = 3 (BP_TYPE_ID)
    tempMap.clear();
    tempPolygonF.resize(0);
    // BIPEDAL = 0
    tempPolygonF    /*First 5 points are for zones and likelihoods*/
                << QPointF(1,2)/*start zone*/ << QPointF(2,0)/*end zone*/ << QPointF(3,5)/*upper middle zone*/
                << QPointF(4,1)/*dead middle zone*/ << QPointF(5,0)/*lower middle zone*/
                    /*Last 5 points are for min and max scattering %ages (SP) for each zone*/
                << QPointF(5,15)/*start zone*/ << QPointF(0,1)/*end zone*/ << QPointF(10,20)/*upper middle zone*/
                << QPointF(10,20)/*dead middle zone*/ << QPointF(0,1)/*lower middle zone*/;
    tempMap.insert(0, tempPolygonF);
    tempPolygonF.resize(0);
    // QUADPEDAL = 1
    tempPolygonF    /*First 5 points are for zones and likelihoods*/
                << QPointF(1,1)/*start zone*/ << QPointF(2,0)/*end zone*/ << QPointF(3,5)/*upper middle zone*/
                << QPointF(4,2)/*dead middle zone*/ << QPointF(5,1)/*lower middle zone*/
                    /*Last 5 points are for min and max scattering %ages (SP) for each zone*/
                << QPointF(5,15)/*start zone*/ << QPointF(0,1)/*end zone*/ << QPointF(10,20)/*upper middle zone*/
                << QPointF(10,20)/*dead middle zone*/ << QPointF(5,15)/*lower middle zone*/;
    tempMap.insert(1, tempPolygonF);
    tempPolygonF.resize(0);
    // AQUATIC = 2
    tempPolygonF    /*First 5 points are for zones and likelihoods*/
                << QPointF(1,4)/*start zone*/ << QPointF(2,0)/*end zone*/ << QPointF(3,5)/*upper middle zone*/
                << QPointF(4,4)/*dead middle zone*/ << QPointF(5,0)/*lower middle zone*/
                    /*Last 5 points are for min and max scattering %ages (SP) for each zone*/
                << QPointF(15,40)/*start zone*/ << QPointF(0,1)/*end zone*/ << QPointF(30,50)/*upper middle zone*/
                << QPointF(30,45)/*dead middle zone*/ << QPointF(0,1)/*lower middle zone*/;
    tempMap.insert(2, tempPolygonF);
    tempPolygonF.resize(0);
    // AMPHIBIOUS = 3
    tempPolygonF    /*First 5 points are for zones and likelihoods*/
                << QPointF(1,1)/*start zone*/ << QPointF(2,0)/*end zone*/ << QPointF(3,5)/*upper middle zone*/
                << QPointF(4,2)/*dead middle zone*/ << QPointF(5,1)/*lower middle zone*/
                    /*Last 5 points are for min and max scattering %ages (SP) for each zone*/
                << QPointF(5,15)/*start zone*/ << QPointF(0,1)/*end zone*/ << QPointF(10,20)/*upper middle zone*/
                << QPointF(10,20)/*dead middle zone*/ << QPointF(5,15)/*lower middle zone*/;
    tempMap.insert(3, tempPolygonF);
    tempPolygonF.resize(0);
    // OCTAPEDAL = 4
    tempPolygonF    /*First 5 points are for zones and likelihoods*/
                << QPointF(1,2)/*start zone*/ << QPointF(2,0)/*end zone*/ << QPointF(3,5)/*upper middle zone*/
                << QPointF(4,1)/*dead middle zone*/ << QPointF(5,0)/*lower middle zone*/
                    /*Last 5 points are for min and max scattering %ages (SP) for each zone*/
                << QPointF(5,15)/*start zone*/ << QPointF(0,1)/*end zone*/ << QPointF(10,20)/*upper middle zone*/
                << QPointF(10,20)/*dead middle zone*/ << QPointF(0,1)/*lower middle zone*/;
    tempMap.insert(4, tempPolygonF);
    tempPolygonF.resize(0);
    // ANTHROPOD = 5
    tempPolygonF    /*First 5 points are for zones and likelihoods*/
                << QPointF(1,1)/*start zone*/ << QPointF(2,1)/*end zone*/ << QPointF(3,3)/*upper middle zone*/
                << QPointF(4,5)/*dead middle zone*/ << QPointF(5,3)/*lower middle zone*/
                    /*Last 5 points are for min and max scattering %ages (SP) for each zone*/
                << QPointF(65,95)/*start zone*/ << QPointF(65,95)/*end zone*/ << QPointF(30,50)/*upper middle zone*/
                << QPointF(65,95)/*dead middle zone*/ << QPointF(30,50)/*lower middle zone*/;
    tempMap.insert(5, tempPolygonF);
    BP_AttachmentToBackBone_ZoneAndScattering_RS.insert(3, tempMap);

    // ** HEAD = 6 (BP_TYPE_ID)
    tempMap.clear();
    tempPolygonF.resize(0);
    // BIPEDAL = 0
    tempPolygonF    /*First 5 points are for zones and likelihoods*/
                << QPointF(1,5)/*start zone*/ << QPointF(2,0)/*end zone*/ << QPointF(3,0)/*upper middle zone*/
                << QPointF(4,0)/*dead middle zone*/ << QPointF(5,0)/*lower middle zone*/
                    /*Last 5 points are for min and max scattering %ages (SP) for each zone*/
                << QPointF(5,15)/*start zone*/ << QPointF(0,1)/*end zone*/ << QPointF(10,20)/*upper middle zone*/
                << QPointF(10,20)/*dead middle zone*/ << QPointF(0,1)/*lower middle zone*/;
    tempMap.insert(0, tempPolygonF);
    tempPolygonF.resize(0);
    // QUADPEDAL = 1
    tempPolygonF    /*First 5 points are for zones and likelihoods*/
                << QPointF(1,5)/*start zone*/ << QPointF(2,0)/*end zone*/ << QPointF(3,0)/*upper middle zone*/
                << QPointF(4,0)/*dead middle zone*/ << QPointF(5,0)/*lower middle zone*/
                    /*Last 5 points are for min and max scattering %ages (SP) for each zone*/
                << QPointF(5,15)/*start zone*/ << QPointF(0,1)/*end zone*/ << QPointF(10,20)/*upper middle zone*/
                << QPointF(10,20)/*dead middle zone*/ << QPointF(0,1)/*lower middle zone*/;
    tempMap.insert(1, tempPolygonF);
    tempPolygonF.resize(0);
    // AQUATIC = 2
    tempPolygonF    /*First 5 points are for zones and likelihoods*/
                << QPointF(1,5)/*start zone*/ << QPointF(2,0)/*end zone*/ << QPointF(3,0)/*upper middle zone*/
                << QPointF(4,0)/*dead middle zone*/ << QPointF(5,0)/*lower middle zone*/
                    /*Last 5 points are for min and max scattering %ages (SP) for each zone*/
                << QPointF(5,15)/*start zone*/ << QPointF(0,1)/*end zone*/ << QPointF(10,20)/*upper middle zone*/
                << QPointF(10,20)/*dead middle zone*/ << QPointF(0,1)/*lower middle zone*/;
    tempMap.insert(2, tempPolygonF);
    tempPolygonF.resize(0);
    // AMPHIBIOUS = 3
    tempPolygonF    /*First 5 points are for zones and likelihoods*/
                << QPointF(1,5)/*start zone*/ << QPointF(2,0)/*end zone*/ << QPointF(3,0)/*upper middle zone*/
                << QPointF(4,0)/*dead middle zone*/ << QPointF(5,0)/*lower middle zone*/
                    /*Last 5 points are for min and max scattering %ages (SP) for each zone*/
                << QPointF(5,15)/*start zone*/ << QPointF(0,1)/*end zone*/ << QPointF(10,20)/*upper middle zone*/
                << QPointF(10,20)/*dead middle zone*/ << QPointF(0,1)/*lower middle zone*/;
    tempMap.insert(3, tempPolygonF);
    tempPolygonF.resize(0);
    // OCTAPEDAL = 4
    tempPolygonF    /*First 5 points are for zones and likelihoods*/
                << QPointF(1,5)/*start zone*/ << QPointF(2,0)/*end zone*/ << QPointF(3,0)/*upper middle zone*/
                << QPointF(4,0)/*dead middle zone*/ << QPointF(5,0)/*lower middle zone*/
                    /*Last 5 points are for min and max scattering %ages (SP) for each zone*/
                << QPointF(5,15)/*start zone*/ << QPointF(0,1)/*end zone*/ << QPointF(10,20)/*upper middle zone*/
                << QPointF(10,20)/*dead middle zone*/ << QPointF(0,1)/*lower middle zone*/;
    tempMap.insert(4, tempPolygonF);
    tempPolygonF.resize(0);
    // ANTHROPOD = 5
    tempPolygonF    /*First 5 points are for zones and likelihoods*/
                << QPointF(1,5)/*start zone*/ << QPointF(2,0)/*end zone*/ << QPointF(3,0)/*upper middle zone*/
                << QPointF(4,0)/*dead middle zone*/ << QPointF(5,0)/*lower middle zone*/
                    /*Last 5 points are for min and max scattering %ages (SP) for each zone*/
                << QPointF(5,15)/*start zone*/ << QPointF(0,1)/*end zone*/ << QPointF(10,20)/*upper middle zone*/
                << QPointF(10,20)/*dead middle zone*/ << QPointF(0,1)/*lower middle zone*/;
    tempMap.insert(5, tempPolygonF);
    BP_AttachmentToBackBone_ZoneAndScattering_RS.insert(6, tempMap);

    // ** TAIL = 5 (BP_TYPE_ID)
    tempMap.clear();
    tempPolygonF.resize(0);
    // BIPEDAL = 0
    tempPolygonF    /*First 5 points are for zones and likelihoods*/
                << QPointF(1,0)/*start zone*/ << QPointF(2,5)/*end zone*/ << QPointF(3,0)/*upper middle zone*/
                << QPointF(4,0)/*dead middle zone*/ << QPointF(5,2)/*lower middle zone*/
                    /*Last 5 points are for min and max scattering %ages (SP) for each zone*/
                << QPointF(0,1)/*start zone*/ << QPointF(5,10)/*end zone*/ << QPointF(0,1)/*upper middle zone*/
                << QPointF(0,1)/*dead middle zone*/ << QPointF(5,20)/*lower middle zone*/;
    tempMap.insert(0, tempPolygonF);
    tempPolygonF.resize(0);
    // QUADPEDAL = 1
    tempPolygonF    /*First 5 points are for zones and likelihoods*/
                << QPointF(1,0)/*start zone*/ << QPointF(2,5)/*end zone*/ << QPointF(3,0)/*upper middle zone*/
                << QPointF(4,0)/*dead middle zone*/ << QPointF(5,2)/*lower middle zone*/
                    /*Last 5 points are for min and max scattering %ages (SP) for each zone*/
                << QPointF(0,1)/*start zone*/ << QPointF(5,10)/*end zone*/ << QPointF(0,1)/*upper middle zone*/
                << QPointF(0,1)/*dead middle zone*/ << QPointF(5,20)/*lower middle zone*/;
    tempMap.insert(1, tempPolygonF);
    tempPolygonF.resize(0);
    // AQUATIC = 2
    tempPolygonF    /*First 5 points are for zones and likelihoods*/
                << QPointF(1,0)/*start zone*/ << QPointF(2,5)/*end zone*/ << QPointF(3,0)/*upper middle zone*/
                << QPointF(4,2)/*dead middle zone*/ << QPointF(5,2)/*lower middle zone*/
                    /*Last 5 points are for min and max scattering %ages (SP) for each zone*/
                << QPointF(0,1)/*start zone*/ << QPointF(10,30)/*end zone*/ << QPointF(0,1)/*upper middle zone*/
                << QPointF(10,20)/*dead middle zone*/ << QPointF(10,30)/*lower middle zone*/;
    tempMap.insert(2, tempPolygonF);
    tempPolygonF.resize(0);
    // AMPHIBIOUS = 3
    tempPolygonF    /*First 5 points are for zones and likelihoods*/
                << QPointF(1,0)/*start zone*/ << QPointF(2,5)/*end zone*/ << QPointF(3,0)/*upper middle zone*/
                << QPointF(4,0)/*dead middle zone*/ << QPointF(5,2)/*lower middle zone*/
                    /*Last 5 points are for min and max scattering %ages (SP) for each zone*/
                << QPointF(0,1)/*start zone*/ << QPointF(5,10)/*end zone*/ << QPointF(0,1)/*upper middle zone*/
                << QPointF(0,1)/*dead middle zone*/ << QPointF(5,20)/*lower middle zone*/;
    tempMap.insert(3, tempPolygonF);
    tempPolygonF.resize(0);
    // OCTAPEDAL = 4
    tempPolygonF    /*First 5 points are for zones and likelihoods*/
            << QPointF(1,0)/*start zone*/ << QPointF(2,5)/*end zone*/ << QPointF(3,0)/*upper middle zone*/
            << QPointF(4,0)/*dead middle zone*/ << QPointF(5,2)/*lower middle zone*/
                /*Last 5 points are for min and max scattering %ages (SP) for each zone*/
            << QPointF(0,1)/*start zone*/ << QPointF(5,10)/*end zone*/ << QPointF(0,1)/*upper middle zone*/
            << QPointF(0,1)/*dead middle zone*/ << QPointF(5,20)/*lower middle zone*/;
    tempMap.insert(4, tempPolygonF);
    tempPolygonF.resize(0);
    // ANTHROPOD = 5
    tempPolygonF    /*First 5 points are for zones and likelihoods*/
            << QPointF(1,0)/*start zone*/ << QPointF(2,5)/*end zone*/ << QPointF(3,0)/*upper middle zone*/
            << QPointF(4,0)/*dead middle zone*/ << QPointF(5,2)/*lower middle zone*/
                /*Last 5 points are for min and max scattering %ages (SP) for each zone*/
            << QPointF(0,1)/*start zone*/ << QPointF(5,10)/*end zone*/ << QPointF(0,1)/*upper middle zone*/
            << QPointF(0,1)/*dead middle zone*/ << QPointF(5,20)/*lower middle zone*/;
    tempMap.insert(5, tempPolygonF);
    BP_AttachmentToBackBone_ZoneAndScattering_RS.insert(5, tempMap);

    // ** NECK = 9 (BP_TYPE_ID)
    tempMap.clear();
    tempPolygonF.resize(0);
    // BIPEDAL = 0
    tempPolygonF    /*First 5 points are for zones and likelihoods*/
                << QPointF(1,5)/*start zone*/ << QPointF(2,0)/*end zone*/ << QPointF(3,0)/*upper middle zone*/
                << QPointF(4,0)/*dead middle zone*/ << QPointF(5,0)/*lower middle zone*/
                    /*Last 5 points are for min and max scattering %ages (SP) for each zone*/
                << QPointF(5,20)/*start zone*/ << QPointF(0,1)/*end zone*/ << QPointF(0,1)/*upper middle zone*/
                << QPointF(0,1)/*dead middle zone*/ << QPointF(0,1)/*lower middle zone*/;
    tempMap.insert(0, tempPolygonF);
    tempPolygonF.resize(0);
    // QUADPEDAL = 1
    tempPolygonF    /*First 5 points are for zones and likelihoods*/
            << QPointF(1,5)/*start zone*/ << QPointF(2,0)/*end zone*/ << QPointF(3,0)/*upper middle zone*/
            << QPointF(4,0)/*dead middle zone*/ << QPointF(5,0)/*lower middle zone*/
                /*Last 5 points are for min and max scattering %ages (SP) for each zone*/
            << QPointF(5,20)/*start zone*/ << QPointF(0,1)/*end zone*/ << QPointF(0,1)/*upper middle zone*/
            << QPointF(0,1)/*dead middle zone*/ << QPointF(0,1)/*lower middle zone*/;
    tempMap.insert(1, tempPolygonF);
    tempPolygonF.resize(0);
    // AQUATIC = 2
    tempPolygonF    /*First 5 points are for zones and likelihoods*/
            << QPointF(1,5)/*start zone*/ << QPointF(2,0)/*end zone*/ << QPointF(3,0)/*upper middle zone*/
            << QPointF(4,0)/*dead middle zone*/ << QPointF(5,0)/*lower middle zone*/
                /*Last 5 points are for min and max scattering %ages (SP) for each zone*/
            << QPointF(5,20)/*start zone*/ << QPointF(0,1)/*end zone*/ << QPointF(0,1)/*upper middle zone*/
            << QPointF(0,1)/*dead middle zone*/ << QPointF(0,1)/*lower middle zone*/;
    tempMap.insert(2, tempPolygonF);
    tempPolygonF.resize(0);
    // AMPHIBIOUS = 3
    tempPolygonF    /*First 5 points are for zones and likelihoods*/
            << QPointF(1,5)/*start zone*/ << QPointF(2,0)/*end zone*/ << QPointF(3,0)/*upper middle zone*/
            << QPointF(4,0)/*dead middle zone*/ << QPointF(5,0)/*lower middle zone*/
                /*Last 5 points are for min and max scattering %ages (SP) for each zone*/
            << QPointF(5,20)/*start zone*/ << QPointF(0,1)/*end zone*/ << QPointF(0,1)/*upper middle zone*/
            << QPointF(0,1)/*dead middle zone*/ << QPointF(0,1)/*lower middle zone*/;
    tempMap.insert(3, tempPolygonF);
    tempPolygonF.resize(0);
    // OCTAPEDAL = 4
    tempPolygonF    /*First 5 points are for zones and likelihoods*/
            << QPointF(1,5)/*start zone*/ << QPointF(2,0)/*end zone*/ << QPointF(3,0)/*upper middle zone*/
            << QPointF(4,0)/*dead middle zone*/ << QPointF(5,0)/*lower middle zone*/
                /*Last 5 points are for min and max scattering %ages (SP) for each zone*/
            << QPointF(5,20)/*start zone*/ << QPointF(0,1)/*end zone*/ << QPointF(0,1)/*upper middle zone*/
            << QPointF(0,1)/*dead middle zone*/ << QPointF(0,1)/*lower middle zone*/;
    tempMap.insert(4, tempPolygonF);
    tempPolygonF.resize(0);
    // ANTHROPOD = 5
    tempPolygonF    /*First 5 points are for zones and likelihoods*/
            << QPointF(1,5)/*start zone*/ << QPointF(2,0)/*end zone*/ << QPointF(3,0)/*upper middle zone*/
            << QPointF(4,0)/*dead middle zone*/ << QPointF(5,0)/*lower middle zone*/
                /*Last 5 points are for min and max scattering %ages (SP) for each zone*/
            << QPointF(5,20)/*start zone*/ << QPointF(0,1)/*end zone*/ << QPointF(0,1)/*upper middle zone*/
            << QPointF(0,1)/*dead middle zone*/ << QPointF(0,1)/*lower middle zone*/;
    tempMap.insert(5, tempPolygonF);
    BP_AttachmentToBackBone_ZoneAndScattering_RS.insert(9, tempMap);
    tempMap.clear();
    tempPolygonF.resize(0);
}

void Skeleton_RuleSet::modifyBP_AttachmentToBackBone_ZoneAndScattering_RS(int BP_TypeID, int alienTypeID, QPolygonF inputAttmntRS_Poly)
{
    if (BP_AttachmentToBackBone_ZoneAndScattering_RS.contains(BP_TypeID))
    {
        if (BP_AttachmentToBackBone_ZoneAndScattering_RS.value(BP_TypeID).contains(alienTypeID))
        {
            if (inputAttmntRS_Poly.isEmpty()) // create new entries in this inputAttmntRS_Poly i.e. 10 QPoints
            {
                inputAttmntRS_Poly    /*First 5 points are for zones and likelihoods*/
                            << QPointF(1,GeometryAlgorithms::getRandomInteger(4,5))/*start zone*/
                            << QPointF(2,GeometryAlgorithms::getRandomInteger(4,5))/*end zone*/
                            << QPointF(3,GeometryAlgorithms::getRandomInteger(4,5))/*upper middle zone*/
                            << QPointF(4,GeometryAlgorithms::getRandomInteger(4,5))/*dead middle zone*/
                            << QPointF(5,GeometryAlgorithms::getRandomInteger(4,5))/*lower middle zone*/
                                /*Last 5 points are for min and max scattering %ages (SP) for each zone*/
                            << QPointF(5,100)/*start zone scattering*/
                            << QPointF(5,100)/*end zone*/
                            << QPointF(5,100)/*upper middle zone*/
                            << QPointF(5,100)/*dead middle zone*/
                            << QPointF(5,100)/*lower middle zone*/;
            }
            BP_AttachmentToBackBone_ZoneAndScattering_RS[BP_TypeID].remove(alienTypeID);
            BP_AttachmentToBackBone_ZoneAndScattering_RS[BP_TypeID].insert(alienTypeID, inputAttmntRS_Poly);
        }
    }
}

void Skeleton_RuleSet::initGroupBP_AttachmentToAnotherBP_ZoneScatteringTrend_RS()
{
    // initialize the GroupBP_AttachmentToAnotherBP_ZoneScatteringTrend_RS
    GroupBP_AttachmentToAnotherBP_ZoneScatteringTrend_RS    << QPoint(5,11) // (min num of Group BPs, max num of Group BPs)
                                                            << QPoint(2,3) // Bone Zone ID ==> 0=start zone, 1=end zone, 2=upper middle zone, 3=dead middle zone, 4=lower middle zone
                                                            << QPoint(30,100) // (min scattering %age, max scattering %age)
                                                            << QPoint(0,4) // (0,4). Grouping Trend ID: 0 = Solo Trend, 1 = Leader trend, 2 = Diagonal Trend, 3 = Circular trend
                                                            << QPoint(50,85); // min and max pitch and yaw angles for attachments
}

void Skeleton_RuleSet::modifySkeletal_BP_Bulkiness_RS(QPolygonF modPoly)
{
    int BP_Type = modPoly.first().x();
    if (Skeletal_BP_Bulkiness_RS.contains(BP_Type))
    {
        // Set the energy trends with their likelihood
        if (modPoly[0].y() != -1) // if energy trend is not random
        {
            for (int i=0; i<4; i++) // reset the likelihoods of energy trends to 0
            {
                Skeletal_BP_Bulkiness_RS[BP_Type][i].setY(0);
            }
            Skeletal_BP_Bulkiness_RS[BP_Type][modPoly[0].y()].setY(1); // now set this energy trend likelihood to 4
        }
        else // if it's random, then equal likelihoods for all the
        {
            for (int i=0; i<4; i++) // if is random then all trrends gets equal likelihood
            {
                Skeletal_BP_Bulkiness_RS[BP_Type][i].setY(1);
            }
        }

        // set the start scale range
        if (modPoly[1].x() >= modPoly[1].y())
        {
            Skeletal_BP_Bulkiness_RS[BP_Type][5].setX(modPoly[1].x());
            Skeletal_BP_Bulkiness_RS[BP_Type][5].setY(modPoly[1].x()+0.2);
        }
        else
        {
            Skeletal_BP_Bulkiness_RS[BP_Type][5].setX(modPoly[1].x());
            Skeletal_BP_Bulkiness_RS[BP_Type][5].setY(modPoly[1].y());
        }

        // set the scaling multiplier range
        if (modPoly[2].x() >= modPoly[2].y())
        {
            Skeletal_BP_Bulkiness_RS[BP_Type][6].setX(modPoly[2].x());
            Skeletal_BP_Bulkiness_RS[BP_Type][6].setY(modPoly[2].x()+0.1);
        }
        else
        {
            Skeletal_BP_Bulkiness_RS[BP_Type][6].setX(modPoly[2].x());
            Skeletal_BP_Bulkiness_RS[BP_Type][6].setY(modPoly[2].y());
        }

        // Set the axis/axes of scaling
        if (modPoly[3].x() == 1) // scaling on X+Y
        {
            Skeletal_BP_Bulkiness_RS[BP_Type][7].setX(31);
            Skeletal_BP_Bulkiness_RS[BP_Type][7].setY(100);
        }
        else if (modPoly[3].y() == 1) // X only
        {
            Skeletal_BP_Bulkiness_RS[BP_Type][7].setX(-80);
            Skeletal_BP_Bulkiness_RS[BP_Type][7].setY(30);
        }
    }
}

void Skeleton_RuleSet::initBulkinessTrendsMap()
{
    BulkinessTrendsMap.clear();

    BulkinessTrendsMap.insert("Random", -1);
    BulkinessTrendsMap.insert("Varying", 0);
    BulkinessTrendsMap.insert("Ascending", 1);
    BulkinessTrendsMap.insert("Descending", 2);
    BulkinessTrendsMap.insert("Constant", 3);
}

void Skeleton_RuleSet::initSkeletal_BP_SubDivCells_RS()
{
    Skeletal_BP_SubDivCells_RS.clear();
    // 11 points in total
    Skeletal_BP_SubDivCells_RS << QPointF(5,20) // P0
                               << QPointF(0,2)  // P1
                               << QPointF(20,50)  // P2
                               << QPointF(0,5)  // P3
                               << QPointF(1.8,18.5)  // P4
                               << QPointF(1.1,1.5)  // P5
                               << QPointF(20,80)    // P6
                               << QPointF(10,100)    // P7
                               << QPointF(2,75)    // P8
                               << QPointF(0,20)    // P9
                               << QPointF(10,100);    // P10
}

void Skeleton_RuleSet::initBP_Densification_RS()
{
    // initialize the BP_Densification_RS i.e. Extrusions
    BP_Densification_RS << QPointF(1,2) /*      P0 : (number of bones acting as pivot bones) */
                        << QPointF(40,100) /*   P1 : (percentage of bones selected on either sides) */
                        << QPointF(3,100) /*    P2 : (edge on the left face used for main vector for generating stacked densers, max %age of distance to travel on this vector to find the location for all its stacked densers) */
                        << QPointF(1.8,30) /*   P3 : (Protrusion level factor, max number of densers/extrusions stacked on top of this base denser) */
                        << QPointF(0.9,1.0) /*  P4 : (X scaling value for the pivot denser) */
                        << QPointF(0.9,1.0) /*  P5 : (Y scaling value for the pivot denser) */
                        << QPointF(0.8,1.0) /*  P6 : (Z scaling value for the pivot denser) */
                        << QPointF(10,2) /*     P7 : (roll angle for each denser, Should it mirror?) */
                        << QPointF(2,20) /*     P8 : (should roll angle increase after certain %age of densers?, roll angle increment amount - min:0 max:20) */
                        << QPointF(70,60) /*     P9 : (max start %age after which roll angle increases- min20 - max70, %age of the remaining densers until the roll angle increase - min:20 max:60) */
                        << QPointF(2,40) /*     P10 : (should pitch angle change? (Y/N), pitch angle - min:0 max:40) */
                        << QPointF(2,40) /*     P11 : (should yaw angle change after certain %age of densers? (Y/N), yaw angle - min:0 max:40) */
                        << QPointF(2,10) /*    P12 : (should densers swell after certain %age of densers (Y/N), maximum swelling amount i.e. max y-scale factor) */
                        << QPointF(70,60) /*    P13 : (max %age after which densers start swelling - min20 - max70, %age of the remaining densers until the swelling happens - min20 - max60) */
                        << QPointF(25,75); /*    P14 : (min and max %age for moving the mid point of face along the bottom edge vector) */
}

void Skeleton_RuleSet::initBP_Bulgification_RS()
{
    BP_Bulgification_RS.clear();

    BP_Bulgification_RS << QPoint(5,10)    // P0: Number of subdiv cells
                  << QPoint(0,2)    // P1: curving allowed on last bone?
                  << QPoint(0,4)    // P2: bulking trend...0=decreasing,1=increasing,2=swellingOnEnds,3=swelling in middle
                  << QPoint(0,2)    // P3: extend subdiv cells at the tip/end of last bone?
                  << QPoint(0,2)    // P4: cell smoothing allowed? // This means a cell will be made like a ball using the method smoothCell
                  << QPoint(0,2)    // P5: x/y/z-scaling-smoothing-factor - to be converted to float
                  << QPoint(0,20)   // P6: curving angle
                  << QPoint(0,2)    // P7: should-curving-angle-change?
                  << QPoint(0,2)    // P8: generate-mounds-on-subdivcells/bone-root-cell?
                  << QPoint(0,2);   // P9: generate-bridges-from-mounds/subcells/bone-root-cell?
}


void Skeleton_RuleSet::initHeadSubBP_Densification_RS()
{
    HeadSubBP_Densification_RS              << QPointF(1,2) /* (min and max number of bones acting as pivot bones) */
                                            << QPointF(99,100) /* (percentage of bones selected on either sides) */
                                            << QPointF(3,100) /* (edge on the left face used for main vector for generating stacked densers, %age of distance to travel on this vector to find the location for all its stacked densers) */
                                            << QPointF(1.0,15) /* (Protrusion level factor, number of densers/extrusions stacked on top of this base denser) */
                                            << QPointF(0.8,0.9) /* (min and max X scaling value for the pivot denser) */
                                            << QPointF(1.0,1.0) /* (min and max Y scaling value for the pivot denser) */
                                            << QPointF(1.0,1.0) /* (min and max Z scaling value for the pivot denser) */
                                            << QPointF(5,2) /* (min and max roll angle for each denser, Should it mirror?) */
                                            << QPointF(0,2) /* (should roll angle change after certain num of densers? (Y/N)) */
                                            << QPointF(30,100) /* (min and max %age of densers after which the roll angle will increase/change) */
                                            << QPointF(3,1) /* (min and max pitch angle for each denser - 0 to 20, min and max yaw angle for each denser - 0 to 20) */
                                            << QPointF(0,2) /* (should pitch angle change after certain num of densers? (Y/N)) */
                                            << QPointF(30,100) /* (min and max %age of densers after which the pitch angle will increase/change) */
                                            << QPointF(0,2) /* (should yaw angle change after certain num of densers? (Y/N)) */
                                            << QPointF(30,100) /* (min and max %age of densers after which the yaw angle will increase/change) */
                                               ;

}

void Skeleton_RuleSet::initHeadSubBP_AlignmentRS()
{
    HeadSubBP_AlignmentRS << QPoint(10,100)
                          << QPoint(10,100)
                          << QPoint(2,60)
                          << QPoint(2,60)
                          << QPoint(2,30)
                          << QPoint(5,50);
}

void Skeleton_RuleSet::initJATsTrendsMap()
{
    JATsTrendsMap.clear();

    JATsTrendsMap.insert("Ascending", 0);
    JATsTrendsMap.insert("Descending", 1);
    JATsTrendsMap.insert("Wavy Up", 2);
    JATsTrendsMap.insert("Wavy Down", 3);
    JATsTrendsMap.insert("Varying", 4);
    JATsTrendsMap.insert("Constant", 5);
}

void Skeleton_RuleSet::intHeadStretching_RS()
{
    headStretching_RS << QPoint(30,60) << QPoint(200,300) << QPoint(70,80);
}

void Skeleton_RuleSet::intExtremity_RS()
{
    extremity_RS << QPoint(3,8)     // min and max digits
                 << QPoint(3,4)     // min and max carpals
                 << QPoint(10,40)   // min and max angle between digits (b/w 10 and 40)
                 << QPoint(80,450)   // min and max distance between carpals ... these are in %ages
                 << QPoint(30,250)  // min and max size compared to parent BP in %age
                 << QPoint(0,1)     // size increase or decrease ... chose a random number b/w (0,2) ...
                 << QPoint(0,3);    // carpal alignment trend (0,3) .. 0=side-ways, 1=circular, 2=wavy/zigzag
}
