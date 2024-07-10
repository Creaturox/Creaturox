#include "alien_dna.h"

Alien_DNA::Alien_DNA()
{
}

Alien_DNA::~Alien_DNA()
{
    qDebug() << "deleting Alien DNA";
    delete this->skeletonDNA;
    qDebug() << "Alien DNA deleted";
}
