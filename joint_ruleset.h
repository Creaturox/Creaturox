#ifndef JOINT_RULESET_H
#define JOINT_RULESET_H

#include <QVector>
#include <QMap>
#include <QPoint>

class Joint_RuleSet
{
public:
    Joint_RuleSet();

    // this method is called by the client code (AlienModeller)
    void setJointRuleSet(QVector<QPoint> inBrackets);


    //  0 - Number of Sub Cells: min Cells=0, max Cells=5
    //  at the moment there is only one entry/spec. perhaps other specs for the joint will make it more organic??
    QVector<QPoint> jointSpecsBrackets;

    // minMaxBonesPerJoint: How many bones each joint will have
    // each row has two entries. Min Bones, and Max Bones.
    // key is the Joint ID, and the value is a two element int array for specifying min and max bones that joint can attach to
    QMap<int, QPoint> minMaxBonesPerJoint; // max = cannot be more than 10% of the max bones set inside Skeleton_Ruleset. min = 1 OR min bones set inside Skeleton_Ruleset
};

#endif // JOINT_RULESET_H
