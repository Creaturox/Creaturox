#ifndef HEAD_H
#define HEAD_H

#include "bodypart.h"

// TODO: Perhaps think of a new Head class that does not derive from BP.
// Within that head, we can have several Head Sub Lineages, which can be derived from a simlpe BP
// Perhaps make a class called HeadSubLineage, or rename the below class.
// Head can be an enclosing class that is composed of several HeadSubLineages
class Head : public BodyPart
{
public:
    Head();
    ~Head();

    bool getJnt2JntLinBonesInOrder(Alien *alienPtr, int linID);

    BodyPart *duplicateBP(Alien *parentAlienPtr, QMap<int, bool> &tempClonedBP_Checklist) override;

    // MEMBER VARIABLES

    // These lineages are the cell lists that are generated from the main head lineage. These will form bridges
    // with the main Head lineage as well as other cell lists (housed inside allCellListLineages DS)
    QVector<Cell*> allLoneCells; // lonely cells that generated on any of the faces of a cell
    QVector<QVector<Cell*>> allCellListLineages;
    QVector<Cell*> allBridgesCells; // This DS is only used in modelNewHead_Algo2()
    QVector<QVector<Cell*>> allBridges; // This DS is a list of all the bridges on the head
                                        // A bridge's sole purpose is to link two points on the head in a bridge/curve like fashion

    // subHeadJnt2JntLineages are lineages that that similar to any BP's jnt2jnt lineages.
    // ... Their purpose is to enhance the shape of the Head/Skull and provide additional creative cues
    // ... to the artist.
    // key(int) is the ID of the lineage
    // QPolygon is composed of (parentJnt,ChildJnt) IDs list
    QMap<int, QPolygon*> subHeadJnt2JntLineages;    // PERHAPS NOT NEEDED

};

#endif // HEAD_H
