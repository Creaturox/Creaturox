#include "bone_ruleset.h"

Bone_RuleSet::Bone_RuleSet()
{

}

void Bone_RuleSet::setBoneRuleSet(QVector<QPoint> inBrackets)
{
    for (int i=0; i<inBrackets.length(); i++)
    {
        boneSpecsBrackets.append(QPoint(inBrackets[i].x(),inBrackets[i].y()));
    }
}

