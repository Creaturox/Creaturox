HEADERS       = glwidget.h \
    extremitybp.h \
    extremitybp_base.h \
    extremitybp_base_carpal.h \
    extremitybp_digit.h \
    mainwindow1.h \
    planexy.h \
    cuttingplane.h \
    boxyMuscle.h \
    tiny_obj_loader.h \
    genericalien.h \
    box.h \
    quadface.h \
    torsomodeller.h \
    alienmodeller.h \
    uppertorso.h \
    geometryalgorithms.h \
    level_2_feature.h \
    level2featurespawnproperties.h \
    cell.h \
    alien.h \
    bodypart.h \
    ruleset.h \
    level_1_2_featurePlaceHolder.h \
    level_1_2_featureplaceholderspecs.h \
    edge.h \
    skeleton_ruleset.h \
    joint_ruleset.h \
    bone_ruleset.h \
    bone.h \
    joint.h \
    skeleton.h \
    jointsarrangementtrend.h \
    ridge.h \
    ridge_ruleset.h \
    lowerlimb.h \
    upperlimb.h \
    backbone.h \
    tail.h \
    head.h \
    alien_dna.h \
    skeleton_dna.h \
    rib.h \
    headsubbp_lineage.h \
    headsubbp_lineagedna.h \
    head_ruleset.h \
    head_dna.h \
    headsubbp_mandible.h \
    headsubbp_maxilla.h \
    headsubbp_zygomatic.h

SOURCES       = glwidget.cpp \
    extremitybp.cpp \
    extremitybp_base.cpp \
    extremitybp_base_carpal.cpp \
    extremitybp_digit.cpp \
                main.cpp \
    mainwindow1.cpp \
    planexy.cpp \
    cuttingplane.cpp \
    boxyMuscle.cpp \
    genericalien.cpp \
    box.cpp \
    quadface.cpp \
    torsomodeller.cpp \
    alienmodeller.cpp \
    uppertorso.cpp \
    geometryalgorithms.cpp \
    level_2_feature.cpp \
    level2featurespawnproperties.cpp \
    cell.cpp \
    alien.cpp \
    bodypart.cpp \
    ruleset.cpp \
    level_1_2_featurePlaceHolder.cpp \
    level_1_2_featureplaceholderspecs.cpp \
    edge.cpp \
    skeleton_ruleset.cpp \
    joint_ruleset.cpp \
    bone_ruleset.cpp \
    bone.cpp \
    joint.cpp \
    skeleton.cpp \
    jointsarrangementtrend.cpp \
    ridge.cpp \
    ridge_ruleset.cpp \
    lowerlimb.cpp \
    upperlimb.cpp \
    backbone.cpp \
    tail.cpp \
    head.cpp \
    alien_dna.cpp \
    skeleton_dna.cpp \
    rib.cpp \
    headsubbp_lineage.cpp \
    headsubbp_lineagedna.cpp \
    head_ruleset.cpp \
    head_dna.cpp \
    headsubbp_mandible.cpp \
    headsubbp_maxilla.cpp \
    headsubbp_zygomatic.cpp

QT           += widgets
QT += openglwidgets

# install
target.path = $$[QT_INSTALL_EXAMPLES]/opengl/idea1_QtProject
INSTALLS += target

FORMS += \
    mainwindow1.ui

#INCLUDEPATH += "C:/glm"
INCLUDEPATH += "/Users/ismailkazmi/Desktop/glm"

DISTFILES += \
    cornell_box.obj \
    cornell_box.mtl \
    alien1_OBJexport_1.obj

# DEFINES += QT_NO_DEBUG_OUTPUT
