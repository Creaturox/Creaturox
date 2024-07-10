#include "head_ruleset.h"

Head_Ruleset::Head_Ruleset()
{

}

void Head_Ruleset::initHeadRuleset()
{
    // headNumOfEachSubBP_Ruleset - Set the ruleset for min and max sub BP for the Head
    // ... for any sub BP. At the moment I'm fixing it to just one for each sub BP type
    headNumOfEachSubBP_Ruleset = QPair<int,int>(1, 2);

    // headSubBP_LineageRuleset
    // point 1: (min lineages, max lineages)
    // point 2: min and max %age lineages with varying sizes
    // point 3: min and max %age of distance from center point to the left or right face to place the next lineage
    // point 4: Lineages Thining Trend.i.e. which of the parallel lineages will be slimmer
    // ... 0=alternative lineages, 1=inner lineages, 2=outer lineages, and 3=no lineages
    // point 5: Lineage Closeness Trend. 0=close towards front, 1=close towards end
    // point 6: min and max angle of closeness of lineages
    headSubBP_LineageRuleset << QPoint(1,4) << QPoint(10,100) << QPoint(10, 100)
                             << QPoint(0,4) << QPoint(0,2) << QPoint(0, 5);


    // headSubBP_AlignRuleset -
    // Point 1: Angle of Alignment (min and max)
    // Point 2: Dist of Alignment (min and max)
    // ... It has only two points. The DNA will select a random value from the min and max
    QPolygon tempPoly; tempPoly.clear();
    // Alignment 1 - Front Mandible with Lower Maxilla
    tempPoly << QPoint(20, 160) << QPoint(5,15);
    headSubBP_AlignRuleset.insert(0, tempPoly);
    // Alignment 2 - Front Mandible with Rear Mandible
    tempPoly.clear();
    tempPoly << QPoint(90, 160) << QPoint(5,15);
    headSubBP_AlignRuleset.insert(0, tempPoly);
    // Alignment 3 - Lower Maxilla with Upper Maxilla
    tempPoly.clear();
    tempPoly << QPoint(10, 30) << QPoint(5,15);
    headSubBP_AlignRuleset.insert(0, tempPoly);
    // Alignment 4 - Lower Maxilla with Side Maxilla
    tempPoly.clear();
    tempPoly << QPoint(5, 20) << QPoint(5,15);
    headSubBP_AlignRuleset.insert(0, tempPoly);
    // Alignment 5 - Side Maxilla with Zygomatic Bone
    tempPoly.clear();
    tempPoly << QPoint(300, 350) << QPoint(5,15);
    headSubBP_AlignRuleset.insert(0, tempPoly);
    // Alignment 6 - Upper Maxilla with Frontal Bone (FBn)
    tempPoly.clear();
    tempPoly << QPoint(5, 20) << QPoint(5,15);
    headSubBP_AlignRuleset.insert(0, tempPoly);
    // Alignment 7 - Frontal Bone (FBn) with Parietal Bone (PBn)
    tempPoly.clear();
    tempPoly << QPoint(5, 20) << QPoint(5,15);
    headSubBP_AlignRuleset.insert(0, tempPoly);
    // Alignment 8 - Upper Maxilla with Nasal Bone
    // Nasal Bone aligns with right side of the Upper Maxilla and Lower Maxilla
}
