#ifndef BONE_RULESET_H
#define BONE_RULESET_H

// TODO(ismail): REFACTOR: perhaps have a single base class for all the ruleset class
//                          that can include all popular headers such <QVector>.
//                          This will stop including these headers in each header file
#include <QVector>
#include <QMap>
#include <QPoint>

/*
 *  This class defines the parameters as well as the rulesets for modelling and sculpting bones.
 *  Perhaps embed the Bone Ruleset inside the Skeleton RS class, and get rid of this one
*/
class Bone_RuleSet
{
public:
    Bone_RuleSet();

    void setBoneRuleSet(QVector<QPoint> inBrackets);
//    void setBoneRidgesRuleset(QVector<QPoint> inBrackets);

    //+++++++ Bone Sculpting Rulesets/Parameters ++++++++

    //  ======== BONE GROWTH RULESETS ========
    //  0 - Number of random points where further branchings can originate. Branching Points: min Points=0, max Points=20
    //  1 - Has Mirrored Branches?: false=0, true=1
    //  2 - Curvature amount: min Angle=0 degrees, max Angle=90 degrees (Might not need it)
    //  3 - Level of Growth: min levels=0, max levels=10 (Might not need it)
    //  4 - Number of parallel bones: min parallel=0, max parallel bones=2 (Might need it very few times)
    //  5 - Squashiness/Elongation amount of Bone: min %age=10, max %age=600
    //  NEED MORE IDEAS such as:
    //          - dents in the middle of the bone (dents can be useful to act as origin sections for muscles)
    QVector<QPoint> boneSpecsBrackets;


    // ====== Bone Bulging Ruleset ======
    //  0 - Number of Bulgings (1-8)
    //  1 - Which area of bone for bulging  (0 = both ends of the bones, 1 = front end of the bone, 2 = rear end of the bone,
    //                                      3 = middle of the bone (SECONDARY))
    //  2 - Bone scaling factor along its z-axis (roll vector). This will make the bone go thinner or thicker. The probability of
    //      ... getting thinner is more than thicker. This rule is only applicable if bulging is ON
    //      ... Another Idea: Perhaps allow different scalings along the roll vector
    //  3 - are end bulgings bilobed towards the end?

    //  ======== BONE BRANCHINGS RULESETS ========
    //  0 - has branchings (boolean)
    //  1 - regular/irregular branchings (boolean). This means that at a particular face, the branches will follow a random
    //      ... pattern if irregular, while for the regular pattern, the branches will most likely follow a line
    //  2 - Has Mirrored Branches?: false=0, true=1
    //  3 - Number of branchings (2 - 15)
    //  4 - Branch growth/protrusion aesthetics rulesets (an additional ruleset specifying the aesthetics of the branch)

    // ====== Bone Surface Scalings (snake skin like scalings) Ruleset ======
    //  Notes: The scalings will also be created using the same technique of ridges/bulgings
    //  0 - Number of scalings per surface (1 to 5)
    //  1 - How many scalings per scaling line
    //  2 - Width of scaling
    //  ... perhaps some more parameters

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

    // For each bone: Min Bones, and Max Bones, that can be attached to a joint
    // key is the Bone ID, and the value is a two element int array for specifying min and max bones

    QMap<int, QPoint> minMaxBonesAllowedForAnyJoint;
};

#endif // BONE_RULESET_H
