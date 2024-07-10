#include "joint_ruleset.h"

Joint_RuleSet::Joint_RuleSet()
{

}

void Joint_RuleSet::setJointRuleSet(QVector<QPoint> inBrackets)
{
    for (int i=0; i<inBrackets.length(); i++)
    {
        jointSpecsBrackets.append(QPoint(inBrackets[i].x(),inBrackets[i].y()));
    }
}
