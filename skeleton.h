#ifndef SKELETON_H
#define SKELETON_H

#include "bodypart.h"

#include <QMap>
//#include <QPointF>
#include <QPolygonF>

// TODO(ismail): REFACTOR:
//      A skeleton can also inherit from BodyPart to adopt all the important members of BodyPart
//      such as Pivot Point, transformation matrices, sub BPs, bounding box etc
class Skeleton
{
public:
    Skeleton();
    ~Skeleton();

    // void setRootBP(BodyPart* bpPtr);
    // BodyPart *getRootBP();

    // BodyPart *rootBP_Ptr;

    // Joint to Joint relations for all the skeletal BPs
    // In this QMap, the key(int) is the BP_ID (other than a JOINT or a BONE).
    // QPolygon is a list of QPoints where each pair is (parent joint, child joint)
    // TODO: Better NOT to use a pointer to QPolygon here. Just use a simple copy, to avoid any potential bugs in future
    QMap<int, QPolygon*> Jnt2JntRelations_TypeGuided;

    // This map stores (pivot vector, location) for each joint
    // Key(int) is the Joint ID. declare a map of pairs/couples, where the first element stores the vector for guiding the orientation
    // ... and the second element is the position/location of the joint
    // TODO(ismail): This needs to be updated whenever a joint/BP undergoes transformation such as rotation and translation
    // ... Also check whether this need to be updated when the JATs are modified or any other mutation happens to the BP
    // ... such as sqwashing, smoothing etc
    QMap<int, QPair<QVector3D, QVector3D>> vectorLocationCoupleForEachJoint_TypeGuided;

    // This list stores the mapping of the joint in the source BP to the destination/cloned
    // ... BP. This can be useful for some purposes, mainly for cloning BPs, but also for modifying DNAs i.e. mutating BPs
    // Key(int) is the joint ID of the source BP. Value(int) is the joint ID of the destination BP
    QMap<int, int> JntToJntMappings_ForBookKeeping;

    //  int: joint ID
    //  QPoint: x-coord = Bone ID, y-coord = number of bones with the Bone ID
//    QMap<int, QPoint> skeleton_specs; // The final specs used by the ALienModeller to generate the skeleton
    QVector<QPolygon> skeleton_specs; // Joint-Bone relationships (UNUSED - PERHAPS NOT NEED - GET RID OF IT)

    // Jnt2JntDS: A DS for joint-joint relationship computed from skeleton_specs is needed
//    QVector<QPoint> Jnt2JntRelations;
    QPolygon Jnt2JntRelations; // PERHAPS NOT NEEDED

    // collection of Extremities
};

#endif // SKELETON_H
