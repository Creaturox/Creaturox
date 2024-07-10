#ifndef CHARACTERBOXMODEL_H
#define CHARACTERBOXMODEL_H

#include "box3d.h"

#include <QVector>



class CharacterBoxModel
{
public:
    CharacterBoxModel();
    ~CharacterBoxModel();

private:
    QVector<Box3D> charBoxModel;
};

#endif // CHARACTERBOXMODEL_H
