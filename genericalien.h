#ifndef GENERICALIEN_H
#define GENERICALIEN_H

#include <QVector>
#include <qopengl.h>
#include <QDebug>
#include <uppertorso.h>

/*
 * Although this class is not needed but it contains some useful code for importing a 3D model using TinyObjectLoader (tiny_obj_loader.h)
 * This class is composed of all the body parts that make up an alien creature.
 * Body Parts include: Upper and Lower Torsos, Head, Legs, Arms, Abnormal features
*/

class GenericAlien
{
public:
    GenericAlien();
    void loadAlien(); // load a mesh from an OBJ file

private:
    QVector<GLfloat> m_data; // this is our complete data list for generic alien geometry including vertices and normals, packed together, i.e. [ v1, v2, v3, n1, n2, n3, ... ]

};

#endif // GENERICALIEN_H
