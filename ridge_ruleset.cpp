#include "ridge_ruleset.h"

Ridge_Ruleset::Ridge_Ruleset()
{

}

void Ridge_Ruleset::setRidgeRulesetBrackets_global(QVector<QPoint> inBrackets)
{
    for (int i=0; i<inBrackets.length(); i++)
    {
        this->ridgeRulesetBrackets_global.append(QPoint(inBrackets[i].x(),inBrackets[i].y()));
    }
}

void Ridge_Ruleset::setRidgeRulesetBrackets_local(QVector<QPoint> inBrackets)
{
    for (int i=0; i<inBrackets.length(); i++)
    {
        this->ridgeRulesetBrackets_local.append(QPoint(inBrackets[i].x(),inBrackets[i].y()));
    }
}
