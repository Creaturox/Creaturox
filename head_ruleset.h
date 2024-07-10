#ifndef HEAD_RULESET_H
#define HEAD_RULESET_H

#include <QMap>
#include <QPair>
#include <QPolygon>



class Head_Ruleset
{
public:
    Head_Ruleset();

    void initHeadRuleset();

    // general Sub BP ruleset
    QPair<int,int> headNumOfEachSubBP_Ruleset;
    QMap<int, QPolygon> headSubBP_AlignRuleset;
    QPolygon headSubBP_AttachmentToBB_DNA;

    QPolygon headSubBP_LineageRuleset;
};

#endif // HEAD_RULESET_H
