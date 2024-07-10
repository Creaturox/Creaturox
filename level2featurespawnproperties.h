#ifndef LEVEL2FEATURESPAWNPROPERTIES_H
#define LEVEL2FEATURESPAWNPROPERTIES_H


/*
 *  This class contains properties for each feature that is spawned inside a level 1 box
*/

class Level2FeatureSpawnProperties
{
public:
    Level2FeatureSpawnProperties();

    // All these properties need to have default values so those level 1 boxes with no level 2 features should be defaulted out
    // Possible solution: We can use a NULL pointer as well

    bool canSpawn; // whether this feature should be spawned or not. default: False
    int numOfFeatures; // number of features that need to be spawned inside the containing box. Default: 0
    // spawnPattern -- perhaps need to define this in another class. default: no pattern
    bool isMutable; // default: false
    // spawnPoints. default: no points
    // sizeOfFeature. default: 0 units.

};

#endif // LEVEL2FEATURESPAWNPROPERTIES_H
