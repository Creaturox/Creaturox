#ifndef RIDGE_RULESET_H
#define RIDGE_RULESET_H

#include <QVector>
#include <QMap>
#include <QPoint>
#include <QPolygonF>


// This ruleset is used by the alien modeller to specify the ridge specs for each BP (such as bone, muscle etc)
class Ridge_Ruleset
{
public:
    Ridge_Ruleset();

    // This method initializes all the ridge rulset brackets
    void setRidgeRulesetBrackets_global(QVector<QPoint> inBrackets);
    void setRidgeRulesetBrackets_local(QVector<QPoint> inBrackets);

    // ====== Ridges Ruleset (GLOBAL) ======
    //  Notes: Can we have both bulgings and ridges in a bone? Yes, first the bulgings will be generated and then the ridges
    //  0 - Number of ridges (0 to 20) (GLOBAL SPEC ... For this spec, when setting it for individual ridges,
    //      ... there should be a -1 for the individual ridge in order to tell the system that this is a global spec
    //      ... that is specified for the BP on which the ridge appears
    //  1 - regular/irregular alignment b/w ridges. Regular alignment means all the ridges are stacked together
    //      ... in an organized row (GLOBAL SPEC) (SECONDARY)

    QVector<QPoint> ridgeRulesetBrackets_global;

    // ====== Ridges Ruleset (LOCAL) ======
    //  Notes: Can we have both bulgings and ridges in a bone? Yes, first the bulgings will be generated and then the ridges
    //  0 - how many faces to be involved to create the ridges (1 to 4 faces). (this is an interesting rule/parameter)
    //      ... The maximum number of faces could also be the maximum number of faces of the cell (for example if the cell is
    //      ... represented by a polyhedron other than a box/cube)
    //  1 - thickness of ridge (5% to 20% of the thickness of the face that this ridge appears on)
    //  2 - distance b/w each ridge (10% to 50% of the average thickness of the previous ridge)
    //  3 - waviness/disturbance of the ridge (b/w 0% and 50% of the thickness of the ridge.)
    //  4 - nature of protrusion of the ridges i.e. how much narrow/wide at the middle (this will be a seperate ruleset
    //      ... stored for each ridge)
    //  More ideas:
    //              - Number of spikes in a ridge. i.e. the number of protrusions on a ridge line (SECONDARY)

    QVector<QPoint> ridgeRulesetBrackets_local;

};

#endif // RIDGE_RULESET_H
