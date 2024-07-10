#include "jointsarrangementtrend.h"

JointsArrangementTrend::JointsArrangementTrend()
{

}

JointsArrangementTrend::~JointsArrangementTrend()
{
    qDebug() << "deleting JointsArrangementTrend";
    BP_JAT_rootJoint_DNA.clear();
    BP_to_JAT_rootJoint_Map_DNA.clear();
    VectorEnergyAngleTrends_TypeGuided.clear();
    randIntForBookKeeping_EnergyTrend.clear();
    randFloatForBookKeeping_FinalEnergy.clear();
    randIntForBookKeeping_AngleTrend.clear();
    randIntForBookKeeping_quaterEulerAngleSelectorForFirstChild.clear();
    randQVector3DForBookKeeping_quaterEulerAngleSelectorForNotFirstChild.clear();
    BP_JAT_needRecomputingIndicator.clear();
    isThisJntSeperateBranch_List.clear();
    VectorEnergyAngleTrends.clear();
    qDebug() << "JointsArrangementTrend deleted";
}

