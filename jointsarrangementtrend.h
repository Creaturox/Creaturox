#ifndef JOINTSARRANGEMENTTREND_H
#define JOINTSARRANGEMENTTREND_H

//#include <QVector>
#include <QPolygonF>
#include <QMap>
#include <QVector3D>
#include <qdebug.h>

class JointsArrangementTrend
{
public:
    JointsArrangementTrend();
    ~JointsArrangementTrend();

    // This DS specifies the energy and angle trends for the root joint of each BP (other than Bone and Joint),
    // ... which serves as the starting information for calculating the JATs for the BP
    // key(int) is the BP_TYPE ID. QPolygon is 2*numOfBP pairs(QPoints) for each BP_Type
    // Each two pairs(QPointF) are...
    // 1st pair correspond to (energy trend for the BP, starting energy selected randomly)
    // 2nd pair correspond to (angle trend for the BP, starting angle selected randomly)
    QMap<int, QPolygonF> BP_JAT_rootJoint_DNA;

    // This DS is usually for bookkeeping used for cloning purposes
    // key(int) is the BP_ID (other than Joint and Bone)
    // QPolygonF is the set of two points, where
    // 1st pair correspond to (energy trend for the root Jnt of this BP, starting energy for the root of this BP)
    // 2nd pair correspond to (angle trend for the root Jnt of this BP, starting angle for the root of this BP)
    QMap<int, QPolygonF> BP_to_JAT_rootJoint_Map_DNA;

    // This DS stores the final energy and angles for EACH joint of the BP, to end up in its final location when its parent
    // ... BP was being modelled. The energy is multiplied with the scaling vector and the vector is rotated along
    // ... the angle (Pitch angle)
    // key (int) is the Joint_ID. Each QPolygon is 2 pairs(QPointF) for each Joint
    // Each two pairs(QPointF) are...
    // 1st pair correspond to (final energy trend, final energy selected randomly).
    // 2nd pair corresponds to (final angle trend, final angle selected randomly)
    QMap<int, QPolygonF> VectorEnergyAngleTrends_TypeGuided;

    // The following DS are used mostly for cloning purposes
    // This DS contains items that will be pre-assigned using randomization when a BP JATs are to be computed
    // For cloned aliens, this DS will be copied as is, and then simply used by the compute_JAT_AND_VecLocCplForEachJnt_ForBP()
    // ... of AlienModeller
    // Key(int) is the Jnt ID
    QMap<int, int> randIntForBookKeeping_EnergyTrend;
    QMap<int, float> randFloatForBookKeeping_FinalEnergy; // TODO(Ismail): check if we really need this or not,
                                                          // ... cuz we are also storing info in VectorEnergyAngleTrends_TypeGuided
    QMap<int, int> randIntForBookKeeping_AngleTrend;
    QMap<int, int> randIntForBookKeeping_quaterEulerAngleSelectorForFirstChild;
    QMap<int, QVector3D> randQVector3DForBookKeeping_quaterEulerAngleSelectorForNotFirstChild;


    // this DS tells the alienmodeller whether the BP_JAT_rootJoint_DNA for a BP has been changed
    // By default, the values are all FALSE. When the JATs are computed for each BP, the value is flipped to TRUE
    // Whenever the BP_JAT_rootJoint_DNA undergoes mutation/modification, the value of that BP is turned to FALSE again
    // This DS is used by the compute_JAT_AND_VecLocCplForEachJnt_ForBP() of AlienModeller
    // int is the BP_ID
    QMap<int, bool> BP_JAT_needRecomputingIndicator;


    // This attribute is not used yet
    QVector<int> isThisJntSeperateBranch_List;  //  stores the BP_ID for only that joint that was arranged
                                                //  at a random vector/direction from its parent joint.

    //  *** NOT NEEDED FOR TYPE_GUIDED ***
    //  This list contains the arrangement trend for each joint in the alien. key(int) is the Joint_ID
    //  It is an M x 4 array. M = number of joints in the alien.
    //  In the columns we have:
    //  [0] = newly calculated energy. [1] = ENERGY trend (0=ASCENDING, 1=DESCENDING, 2=WAVY UP, 3=WAVY DOWN, 4=STABLE)
    //  [2] = newly calculated angle. [3] = ANGLE trend (0=ASCENDING, 1=DESCENDING, 2=WAVY UP, 3=WAVY DOWN, 4=STABLE, 5=RANDOM)
    //  Here the 'Vector' in VectorEnergyAngleTrends refers the vector that is computed by the system whenever computing the final location of the joint
    //  ... from its parent
    QMap<int, QPolygonF> VectorEnergyAngleTrends;

};

#endif // JOINTSARRANGEMENTTREND_H
