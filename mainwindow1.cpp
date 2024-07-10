#include "mainwindow1.h"
#include "ui_mainwindow1.h"
//#include "genericalien.h"

#include <QMessageBox>

MainWindow1::MainWindow1(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow1)
{
    // We can instantiate the AlienModeller object here and assign it to each GLWidget object that we create.
    // The AlienModeller can maintain a QMap where each key is the GLWidget ID, and the value is the BodyPart to be displayed in that GLWidget
    // Assign a list of bodyparts to render to each GLWidget object so that the AlienModeller object is not created everytime GLWidget is created

    ui->setupUi(this);
    GLWidgetID_Counter = -1;
    tabCounter = -1;

    alModlerPtr = new AlienModeller();

    this->tabWidgetID_Map.insert(++tabCounter, this->ui->tabWidget_2);

//    // Now create N aliens to be displayed in the first tab
//    for (int i=0; i<12; i++)
//    {
//        alienModellerObject->modelNewAlien_TypeGuided();
//        qDebug() << "Number of Aliens created: " << alienModellerObject->alienList.length();
//    }

////    ui->tab_4->layout()->addWidget(glWidget10);

//    // Insert as many tabs to the tabWidget_2 as there are types of BPs in the alien.
//    // Each tab should display all the alien BPs

//    // Seed Alien - Create several widgets in the tab and display all the seed aliens in these
//    createNewTabAndPopulateWithAliens(alienModellerObject->alienList, "Aliens");
////    createNewTabAndPopulateWithBodyParts(AlienModeller::BodyPartsEnum::UPPER_LIMB);

//    // Create Alien clones
//    QVector <Alien*> tempClonedAliensList; tempClonedAliensList.resize(0);
//    for (int i=0; i<2/*clones*/; i++)
//    {
//       tempClonedAliensList.append(alienModellerObject->cloneAlien_Algo2(alienModellerObject->alienList[0]));
//    }

//    createNewTabAndPopulateWithAliens(tempClonedAliensList, "Cloned Aliens");

//    // Create clones of LowerLimb
//    alienModellerObject->cloneSkeletalBP_viaDNA(tempClonedAliensList[0],tempClonedAliensList[0]->alienLowerLimbs[0],3);
//    createNewTabAndPopulateWithBodyParts(tempClonedAliensList[0], tempClonedAliensList[0]->cloneBPs, AlienModeller::BodyPartsEnum::LOWER_LIMB);

    setWindowTitle(tr("Procedural Alien Modeller"));

    iInd=0;
    jInd=0;
    kInd=0;

    new_iInd = 0;
    new_jInd = 0;
    new_kInd = 0;

    // Connect button signal to appropriate slot
    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow1::handlePushButton_MutateBP_BulkinessDNA);
    connect(ui->pushButton_27, &QPushButton::clicked, this, &MainWindow1::handlePushButton_27_UpdateBulkinessRS_WithGUI_SaveValues);
    connect(ui->pushButton_22, &QPushButton::clicked, this, &MainWindow1::handlePushButton_22__mutateNumOfJointsPerBP_DNA);
    connect(ui->pushButton_2, &QPushButton::clicked, this, &MainWindow1::handlePushButton_2__cloneAlien);
    connect(ui->pushButton_25, &QPushButton::clicked, this, &MainWindow1::handlePushButton_25__ReplaceAliens);
    connect(ui->pushButton_52, &QPushButton::clicked, this, &MainWindow1::handlePushButton_52__ReplaceSelectedAliens);
    connect(ui->pushButton_70, &QPushButton::clicked, this, &MainWindow1::handlePushButton_70__RemoveSelectedAliens);
    connect(ui->pushButton_29, &QPushButton::clicked, this, &MainWindow1::handlePushButton_29__modelExtremityBPs);
    connect(ui->pushButton_33, &QPushButton::clicked, this, &MainWindow1::handlePushButton_33__attachSeveralExtremityBPsOnRandomBPs);
    connect(ui->pushButton_47, &QPushButton::clicked, this, &MainWindow1::handlePushButton_47__transferExtBP_FromSourceAlienToDestAlienWithCloning);
    connect(ui->pushButton_51, &QPushButton::clicked, this, &MainWindow1::handlePushButton_51__RemoveAndDeleteExtremityBP_FromAlien);
    connect(ui->pushButton_8, &QPushButton::clicked, this, &MainWindow1::handlePushButton_8__DensifyBodyPart);
    connect(ui->pushButton_12, &QPushButton::clicked, this, &MainWindow1::handlePushButton_12__ApplySepBP_ChangesToOrigBP_OnAlien);
    connect(ui->pushButton_3, &QPushButton::clicked, this, &MainWindow1::handlePushButton_3__CloneBodyPartAndDisplayInNewTabWithAlienOfInterest);
    connect(ui->pushButton_30, &QPushButton::clicked, this, &MainWindow1::handlePushButton_30__IncreaseNumOfJoints);
    connect(ui->pushButton_31, &QPushButton::clicked, this, &MainWindow1::handlePushButton_31__DecreaseNumOfJoints);
    connect(ui->pushButton_6, &QPushButton::clicked, this, &MainWindow1::handlePushButton_6__SmoothBP);
    connect(ui->pushButton_34, &QPushButton::clicked, this, &MainWindow1::handlePushButton_34__IncreaseDensifierSpikiness);
    connect(ui->pushButton_35, &QPushButton::clicked, this, &MainWindow1::handlePushButton_35__DecreaseDensifierSpikiness);
    connect(ui->pushButton_40, &QPushButton::clicked, this, &MainWindow1::handlePushButton_40__IncreaseDensifierExtrusions);
    connect(ui->pushButton_41, &QPushButton::clicked, this, &MainWindow1::handlePushButton_41__DecreaseDensifierExtrusions);
    connect(ui->pushButton_42, &QPushButton::clicked, this, &MainWindow1::handlePushButton_42__stretchBP);
    connect(ui->pushButton_43, &QPushButton::clicked, this, &MainWindow1::handlePushButton_43__MutateExtremBPsCollectively);
    connect(ui->pushButton_48, &QPushButton::clicked, this, &MainWindow1::handlePushButton_48__GenerateAliens);
    connect(ui->pushButton_4, &QPushButton::clicked, this, &MainWindow1::handlePushButton_4__SaveAlienToFile);
    connect(ui->pushButton_9, &QPushButton::clicked, this, &MainWindow1::handlePushButton_9__LoadAlienFromFile);
    connect(ui->pushButton_54, &QPushButton::clicked, this, &MainWindow1::handlePushButton_54__IncreaseBP_Size);
    connect(ui->pushButton_55, &QPushButton::clicked, this, &MainWindow1::handlePushButton_55__DecreaseBP_Size);
    connect(ui->pushButton_20, &QPushButton::clicked, this, &MainWindow1::handlePushButton_20__mutateBP_JAT_Randomly);
    connect(ui->pushButton_21, &QPushButton::clicked, this, &MainWindow1::handlePushButton_21__mutateBP_JAT_FromOptions);
    connect(ui->pushButton_18, &QPushButton::clicked, this, &MainWindow1::handlePushButton_18__generateGroupBPs);
    connect(ui->pushButton_15, &QPushButton::clicked, this, &MainWindow1::handlePushButton_15__squashBP);
    connect(ui->pushButton_26, &QPushButton::clicked, this, &MainWindow1::handlePushButton_26__computeAngleBetweenVectors);
    connect(ui->pushButton_56, &QPushButton::clicked, this, &MainWindow1::handlePushButton_56__showAngleTrendsForJntsOfBP);
    connect(ui->pushButton_69, &QPushButton::clicked, this, &MainWindow1::handlePushButton_69__genClonedBPsAndAttachNearby);
    connect(ui->pushButton_84, &QPushButton::clicked, this, &MainWindow1::handlePushButton_84__PruneBP);
    connect(ui->pushButton_38, &QPushButton::clicked, this, &MainWindow1::handlePushButton_38__IncreaseDensifierCurvature);
    connect(ui->pushButton_39, &QPushButton::clicked, this, &MainWindow1::handlePushButton_39__DecreaseDensifierCurvature);
    connect(ui->pushButton_13, &QPushButton::clicked, this, &MainWindow1::handlePushButton_13__reattachSelectedBPsToDiffLocations);
    connect(ui->pushButton_14, &QPushButton::clicked, this, &MainWindow1::handlePushButton_14__BevelBP);
    connect(ui->pushButton_16, &QPushButton::clicked, this, &MainWindow1::handlePushButton_16__modelHeads);
    connect(ui->pushButton_46, &QPushButton::clicked, this, &MainWindow1::handlePushButton_46__genAndAttachNewHeadOnAlien);
    connect(ui->pushButton_17, &QPushButton::clicked, this, &MainWindow1::handlePushButton_17__ReverseBP_BulkinessTrend);
    connect(ui->pushButton_81, &QPushButton::clicked, this, &MainWindow1::handlePushButton_81__BulgifyBP);
    connect(ui->pushButton_89, &QPushButton::clicked, this, &MainWindow1::handlePushButton_89__SubdiveBP_BonesIntoSubdivCells);
    connect(ui->pushButton_JAT_RS_SaveValues, &QPushButton::clicked, this, &MainWindow1::handlePushButton_JAT_RS_SaveValues);
    connect(ui->pushButton_JAT_RS_Default_Values, &QPushButton::clicked, this, &MainWindow1::handlePushButton_JAT_RS_Default_Values);

    connect(ui->pushButton_73, &QPushButton::clicked, this, &MainWindow1::handlePushButton_73__AddNewBPsOfSelectedType);
    connect(ui->pushButton_74, &QPushButton::clicked, this, &MainWindow1::handlePushButton_74__RemoveSelectedBP);

    connect(ui->comboBox_5, &QComboBox::currentTextChanged, this, &MainWindow1::updateSpinBoxesMinAndMax_AlienTab);
    connect(ui->comboBox_10, &QComboBox::currentTextChanged, this, &MainWindow1::setActivationOfExtremGroupMutationControls);
    connect(ui->comboBox_18, &QComboBox::currentTextChanged, this, &MainWindow1::selectAllBPsOfSelectedType_FromComboBox18);

    connect(ui->horizontalSlider_11, &QAbstractSlider::sliderMoved, this, &MainWindow1::mutateStretchSquashExtremBPsCollectively);
    connect(ui->horizontalSlider_12, &QAbstractSlider::sliderMoved, this, &MainWindow1::mutateNumOfJointsExtremBPsCollectively);
    connect(ui->horizontalSlider_13, &QAbstractSlider::sliderMoved, this, &MainWindow1::mutateSelectedBP_JAT_EnergyOrAngleUsingHorSlider);
    connect(ui->horizontalSlider_14, &QAbstractSlider::sliderMoved, this, &MainWindow1::mutateSelectedBP_JAT_EnergyOrAngleUsingHorSlider);
    connect(ui->horizontalSlider_15, &QAbstractSlider::sliderMoved, this, &MainWindow1::increaseDecreaseBulkinessUsingHorizontalSlider_15);
    connect(ui->horizontalSlider_16, &QAbstractSlider::sliderMoved, this, &MainWindow1::handleHorizontalSlider_16__IncreaseDensifierCurvature);
    connect(ui->horizontalSlider_20, &QAbstractSlider::sliderMoved, this, &MainWindow1::handleHorizontalSlider_20__incDecDenserSpikeMovingUpDownPerc);
    connect(ui->horizontalSlider_23, &QAbstractSlider::sliderMoved, this, &MainWindow1::handleHorizontalSlider_23__moveDensersAcrossBP_Bones);

    squashStretchSliderVal = ui->horizontalSlider_11->value();
    numOfJntsSliderVal = ui->horizontalSlider_12->value();

    prevHorizSlider15Value_IncDecBulkiness = ui->horizontalSlider_15->value();
    prevHorizSlider16Value_IncDecDensCurvature = ui->horizontalSlider_16->value();
    prevHorizSlider20Value_incDecDenserSpikeMovingUpDownPerc = ui->horizontalSlider_20->value();
    prevHorizSlider23Value_MoveDensersAcrossBP_Bones = ui->horizontalSlider_23->value();

    // prevMutateJAT_EnergyValue_Slider13 = ui->horizontalSlider_13->value();
    // prevMutateJAT_AngleValue_Slider14 = ui->horizontalSlider_14->value();
    //    srand (time(NULL));
}

void MainWindow1::createNewTabAndPopulateWithBodyParts(Alien *alienPtr, QVector<BodyPart *> &BP_List)
{
    QWidget *newTab = this->createNewTabInTabWidget(ui->tabWidget_2, "New BP");

    // Add N frames to this tab along with GLWidgets
    createMultipleFramesInTab(newTab, 3, 4);

    // Now populate this newly created tab with the incoming BodyParts
//    tabToBPType_Map.insert(tabID_Map.key(newTab), BP_Type);

    // first store all these body parts in a temp list
    QVector<int> tempBPs;
    for (int i=0; i<BP_List.length(); i++)
    {
//        if (BP_List[i]->BP_Type == BP_Type)
            tempBPs.append(BP_List[i]->BP_ID);
    }

    qDebug() << "reached";

    // Now assign each of these tempBPs to the GLWidgets
    QMap<int,QPolygon> tempMap = tabToFrameToGLWidget_Map.value(tabID_Map.key(newTab));
    QMapIterator<int,QPolygon> j(tempMap);
    while (j.hasNext())
    {
        QPolygon tempPoly = j.next().value();
        QVector<int> tempBPList;
        for (int g=0; g<tempPoly.length(); g++)
        {
            // assign BPs
            if (!tempBPs.isEmpty())
            {
                tempBPList << tempBPs.first();
                glWidgetID_Map.value(tempPoly.at(g).x())->assignBodyPartsToRender(alienPtr, tempBPList, false);
                tempBPs.removeFirst();
                tempBPList.resize(0);
            }
            if (!tempBPs.isEmpty())
            {
                tempBPList << tempBPs.first();
                glWidgetID_Map.value(tempPoly.at(g).y())->assignBodyPartsToRender(alienPtr, tempBPList, false);
                tempBPs.removeFirst();
                tempBPList.resize(0);
            }
            else
                break;
        }
    }

    qDebug() << "reached";
}

QWidget *MainWindow1::createNewTabAndPopulateWithAliens(QVector<Alien *> &alienList, QTabWidget *tabWidgetPtr, const QString tabTitle,
                                                        int numOfFrames, int numOfGLWgtsPerFrame, bool isThisAlienOfInterest)
{
    QWidget *newTab = this->createNewTabInTabWidget(tabWidgetPtr, tabTitle);

    // Add N frames to this tab along with GLWidgets
    createMultipleFramesInTab(newTab, numOfFrames, numOfGLWgtsPerFrame);

    // first store all these aliens in a temp list
    QVector<Alien *> tempAlienList = QVector<Alien *>(alienList);

    qDebug() << "reached";

    // Now assign each of these aliens to the GLWidgets
    QMap<int,QPolygon> tempMap = tabToFrameToGLWidget_Map.value(tabID_Map.key(newTab));
    // TODO(ismail): resolve the bug. it is only having one entry in the map
    qDebug() << "number of Frames in this tab : " << tempMap.size();
    QMapIterator<int,QPolygon> frameIter(tempMap);
    while (frameIter.hasNext() && !tempAlienList.isEmpty())
    {
        frameIter.next();
        QPolygon tempPoly = frameIter.value();
        for (int g=0; g<tempPoly.length(); g++)
        {
            // assign Alien
            if (!tempAlienList.isEmpty())
            {
                GLWidget *tempGlW_Ptr = this->glWidgetID_Map.value(tempPoly.at(g).x());
                if (tempGlW_Ptr)
                {
                    assignAlienToGLWidget(tempGlW_Ptr, tempAlienList.first());
                    this->glWgtToAlien_Map.insert(tempPoly.at(g).x(), tempAlienList.first()->alienID);
                    if (isThisAlienOfInterest) // by default its always false
                    {
                        // Pair this Alien of Interest with this GLW
                        glWgtToAlienOfInterest_Map.insert(tempPoly.at(g).x(), tempAlienList.first()->alienID);
                        // Also Pair this Alien of Interest with the tab that was created above
                        tabToAlienOfInterest_Map.insert(tabID_Map.key(newTab), tempAlienList.first()->alienID);
                    }
                    QVector<int> BP_ListToDisplay;  BP_ListToDisplay.clear();
                    QVector<int> sepClnBP_ListToDisplay;  sepClnBP_ListToDisplay.clear();
                    QVector<int> BP_ListToDisplayInDiffColor; BP_ListToDisplayInDiffColor.clear();
                    this->alModlerPtr->getAllBP_ID_List(tempAlienList.first(), BP_ListToDisplay, sepClnBP_ListToDisplay);
                    // update the GLW_to_Alien_to_BP_to_ColorDisplay_Map
                    this->alModlerPtr->updateGLW_to_Alien_to_BP_to_ColorDisplay_Map(tempAlienList.first(),
                                                                                    tempPoly.at(g).x(), // get the GLW_ID
                                                                                    BP_ListToDisplay, BP_ListToDisplayInDiffColor, -1);

                    tempAlienList.removeFirst();
                }
            }
            if (!tempAlienList.isEmpty())
            {
                GLWidget *tempGlW_Ptr = this->glWidgetID_Map.value(tempPoly.at(g).y());
                if (tempGlW_Ptr)
                {
                    assignAlienToGLWidget(tempGlW_Ptr, tempAlienList.first());
                    this->glWgtToAlien_Map.insert(tempPoly.at(g).y(), tempAlienList.first()->alienID);
                    QVector<int> BP_ListToDisplay;  BP_ListToDisplay.clear();
                    QVector<int> sepClnBP_ListToDisplay;  sepClnBP_ListToDisplay.clear();
                    QVector<int> BP_ListToDisplayInDiffColor; BP_ListToDisplayInDiffColor.clear();
                    this->alModlerPtr->getAllBP_ID_List(tempAlienList.first(), BP_ListToDisplay, sepClnBP_ListToDisplay);
                    // update the GLW_to_Alien_to_BP_to_ColorDisplay_Map
                    this->alModlerPtr->updateGLW_to_Alien_to_BP_to_ColorDisplay_Map(tempAlienList.first(),
                                                                                    tempPoly.at(g).y(), // get the GLW_ID
                                                                                    BP_ListToDisplay, BP_ListToDisplayInDiffColor, -1);

                    tempAlienList.removeFirst();
                }
            }
            else
                break;
        }
    }
    qDebug() << "reached";
    return newTab;
}

void MainWindow1::createMultipleFramesInTab(QWidget *tabPtr, int numOfFrames, int numOfGLWgtsPerFrame)
{
    // Add N frames to this tab
    QMap<int, QPolygon> tempMap;
    for (int i=0; i<numOfFrames; i++)
    {
        QFrame *frame = this->addNewFrameToTab(tabPtr, "frame");

        // add 'numOfFrames' glwidgets to this frame. We will be adding pairs of glwidgets at a time
        createMultipleGLWidgetsInFrame(frame, numOfGLWgtsPerFrame, tempMap);
    }

    tabToFrameToGLWidget_Map.insert(tabID_Map.key(tabPtr), tempMap);
}

void MainWindow1::createMultipleGLWidgetsInFrame(QFrame *framePtr, int numOfGLWidgets, QMap<int, QPolygon> &tempMap)
{
    framePtr->setLayout(new QHBoxLayout);
    QPolygon tempPoly;
    tempPoly.resize(0);
    QPoint tempPt = QPoint(-1,-1);
    if (numOfGLWidgets == 1)
    {
        tempPt.setX(glWidgetID_Map.key(addNewGLWidgetToFrame(framePtr)));
        tempPoly << tempPt;
    }
    else
    {
        for (int j=0; j<static_cast<int>(ceilf(numOfGLWidgets/2.f)); j++)
        {
            tempPt.setX(glWidgetID_Map.key(addNewGLWidgetToFrame(framePtr)));
            tempPt.setY(glWidgetID_Map.key(addNewGLWidgetToFrame(framePtr)));
            tempPoly << tempPt;
        }
    }
    tempMap.insert(frameID_Map.key(framePtr), tempPoly);
}

GLWidget *MainWindow1::addNewGLWidgetToFrame(QFrame *framePtr)
{
    GLWidget* newGLWidget = new GLWidget();
    framePtr->layout()->addWidget(newGLWidget);

    newGLWidget->glWidgetID = ++GLWidgetID_Counter;
    glWidgetID_Map.insert(newGLWidget->glWidgetID, newGLWidget);

    newGLWidget->setAlienModeller(alModlerPtr);

    return newGLWidget;
}

void MainWindow1::assignAlienModellerToGLWidget(GLWidget *glWgt)
{
    glWgt->setAlienModeller(alModlerPtr);
}

void MainWindow1::assignAlienToGLWidget(GLWidget *glWgt, Alien *alienPtr)
{
    glWgt->assignAlienForRendering(alienPtr);
}

QTabWidget *MainWindow1::createNewTabWidgetInsideTab(QWidget *tabPtr)
{
    // First create a frame inside the input tab
    // A Frame is very important otherwise nothing is displayed.
    // TODO(ismail): It needs proper formatting as at the moment, some frames inside
    QFrame *frame = this->addNewFrameToTab(tabPtr, "frame");

    QTabWidget *newTabWgtPtr = new QTabWidget(frame);
//    newTabWgtPtr->setLayout(new QHBoxLayout);
//    QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Expanding);
//    sizePolicy2.setHorizontalStretch(0);
//    sizePolicy2.setVerticalStretch(0);
//    sizePolicy2.setHeightForWidth(newTabWgtPtr->sizePolicy().hasHeightForWidth());
//    newTabWgtPtr->setSizePolicy(sizePolicy2);
//    newTabWgtPtr->setMinimumSize(QSize(250, 0));
//    newTabWgtPtr->setMaximumSize(QSize(16777215, 16777215));
//    newTabWgtPtr->setSizeIncrement(QSize(0, 0));
//    newTabWgtPtr->setBaseSize(QSize(0, 0));

    this->tabWidgetID_Map.insert(++tabCounter, newTabWgtPtr);
    tabToTabWgtMap.insert(tabID_Map.key(tabPtr), tabWidgetID_Map.key(newTabWgtPtr));

    return newTabWgtPtr;
}

void MainWindow1::displayAlienOfInterestInNewTab(QVector<Alien*> alienList, QTabWidget *tabWidgetPtr, QString tabTitle)
{
//    // First Create a new tab at the outside tabwidget
//    QWidget *newTab = this->createNewTabInTabWidget(tabWidgetPtr, tabTitle);

//    // Inside this tab, Create a new TabWidget
//    QTabWidget *tempNewTabWgtPtr = createNewTabWidgetInsideTab(newTab);

//    QVector<Alien*> tempAlienList; tempAlienList.clear();
//    tempAlienList.append(alienPtr);

//    // Now create a new Tab in the inner TabWidget and display the cloned alien in it
//    createNewTabAndPopulateWithAliens(tempAlienList, tempNewTabWgtPtr, tabTitle, 1, 1);

//    // pair up this alien with this tabwidget so we can keep a track for future purposes


//    QVector<Alien*> tempAlienList; tempAlienList.clear();
//    tempAlienList.append(alienPtr);

    // Now create a new Tab in the inner TabWidget and display the cloned alien in it
    QWidget *tempTabPtr = createNewTabAndPopulateWithAliens(alienList, tabWidgetPtr, tabTitle, 1, 1, true);
}

int MainWindow1::getTabContainingGLWidget(int GLW_ID)
{
    // get the tab which houses this GLW, using tabToFrameToGLWidget_Map DS
    QMapIterator<int, QMap<int, QPolygon>> tabToFrameToGLWgtIter(tabToFrameToGLWidget_Map);
    while (tabToFrameToGLWgtIter.hasNext())
    {
        int tempTabID = tabToFrameToGLWgtIter.next().key();
        QMapIterator<int, QPolygon> frameToGLWgtIter(tabToFrameToGLWgtIter.value());
        while (frameToGLWgtIter.hasNext())
        {
            QPolygon tempPoly = frameToGLWgtIter.next().value();
            for (QPoint p : tempPoly)
            {
                if (p.x() == GLW_ID  ||  p.y() == GLW_ID)
                    return tempTabID;
            }
        }
    }

    qDebug() << "tab from glwidget not found";
    return -1;
}

void MainWindow1::getFullAndEmptyGLWsInTab(QWidget *tabPtr, QVector<int> &full, QVector<int> &empty)
{
    QMap<int, QPolygon> tempMap = tabToFrameToGLWidget_Map.value(tabID_Map.key(tabPtr));
    QMapIterator<int, QPolygon> tempMapIter(tempMap); // iterate over the frames of this tab

    while (tempMapIter.hasNext())
    {
        tempMapIter.next();
        QPolygon tempGLW_Poly = tempMapIter.value();
        // Now check whether this GLW is occupied or not
        for (QPoint p : tempGLW_Poly)
        {
            int alienID = getAlienID_InGLW(p.x());
            if (alienID != -1)
                full.append(p.x());
            else
                empty.append(p.x());

            alienID = getAlienID_InGLW(p.y());
            if (alienID != -1)
                full.append(p.y());
            else
                empty.append(p.y());
        }
    }
}

int MainWindow1::getTotalGLWsInTab(QWidget *tabPtr)
{
    QMap<int, QPolygon> tempMap = tabToFrameToGLWidget_Map.value(tabID_Map.key(tabPtr));
    QMapIterator<int, QPolygon> tempMapIter(tempMap); // iterate over frames

    int total = 0;
    while (tempMapIter.hasNext())
    {
        tempMapIter.next();
        QPolygon tempGLW_Poly = tempMapIter.value();
        // Now check whether this GLW is occupied or not
        for (QPoint p : tempGLW_Poly)
        {
            if (p.x() != -1)
            {
                if (p.y() != -1)
                    total+=2;
                else
                    total++;
            }
        }
    }

    return total;
}

int MainWindow1::getAlienID_InGLW(int glwID)
{
    int alienID = -1;

    if (glWgtToAlien_Map.contains(glwID))
    {
        alienID = glWgtToAlien_Map.value(glwID);
        // additional check that whether this alien is really housed in the GLW or not
        if (!glWidgetID_Map.value(glwID)->allAliensToRender.contains(this->alModlerPtr->getAlienFromID(alienID)))
        {
            // This is not part of the GLW so remove it from glWgtToAlien_Map
            glWgtToAlien_Map.remove(alienID);
            return -1;
        }

    }
    return alienID;
}

void MainWindow1::setJAT_RS_ValuesFromGUI()
{
//    QMap<int, QPolygon> JAT_RS_ValuesFromGUI;
//    QPolygon tempPolygon; tempPolygon.clear();
//    QVector<int> tempVecEnergy(6,0);
//    QVector<int> tempVecAngle(6,0);

//    // TODO(ismail): Make a seperate method for the below code,
//    // ... taking as input the BP_TYPE_ID
//    if (ui->comboBox_JAT_RS_SelectBP->currentText() == "Lower Limb")
//    {
//        // for energy
//        if (ui->comboBox_JAT_RS_SelectEneryTrend->currentText() == "Any")
//        {
//            tempVecEnergy.fill(4);
//        }
//        else if (ui->comboBox_JAT_RS_SelectEneryTrend->currentText() == "Ascending")
//        {
//            tempVecEnergy.fill(0);
//            tempVecEnergy[0] = 4;
//        }
//        else if (ui->comboBox_JAT_RS_SelectEneryTrend->currentText() == "Descending")
//        {
//            tempVecEnergy.fill(0);
//            tempVecEnergy[1] = 4;
//        }
//        else if (ui->comboBox_JAT_RS_SelectEneryTrend->currentText() == "Wavy")
//        {
//            tempVecEnergy.fill(0);
//            tempVecEnergy[2] = 4; tempVecEnergy[3] = 4;
//        }
//        else if (ui->comboBox_JAT_RS_SelectEneryTrend->currentText() == "Varying")
//        {
//            tempVecEnergy.fill(0);
//            tempVecEnergy[4] = 4;
//        }
//        else if (ui->comboBox_JAT_RS_SelectEneryTrend->currentText() == "Constant")
//        {
//            tempVecEnergy.fill(0);
//            tempVecEnergy[5] = 4;
//        }

//        // for angle
//        if (ui->comboBox_JAT_RS_SelectAngleTrend->currentText() == "Any")
//        {
//            tempVecAngle.fill(4);
//            tempVecAngle[5] = 0;
//        }
//        else if (ui->comboBox_JAT_RS_SelectAngleTrend->currentText() == "Ascending")
//        {
//            tempVecAngle.fill(0);
//            tempVecAngle[0] = 4;
//        }
//        else if (ui->comboBox_JAT_RS_SelectAngleTrend->currentText() == "Descending")
//        {
//            tempVecAngle.fill(0);
//            tempVecAngle[1] = 4;
//        }
//        else if (ui->comboBox_JAT_RS_SelectAngleTrend->currentText() == "Wavy")
//        {
//            tempVecAngle.fill(0);
//            tempVecAngle[2] = 4; tempVecAngle[3] = 4;
//        }
//        else if (ui->comboBox_JAT_RS_SelectAngleTrend->currentText() == "Varying")
//        {
//            tempVecAngle.fill(0);
//            tempVecAngle[4] = 4;
//        }

//        tempPolygon /*Energy trends (First 6 pairs) ... Each pair is the (trend specifier, likelihood) */
//                    << QPoint(0,4)/*Ascending*/ << QPoint(1,4)/*Descending*/ << QPoint(2,4)/*Wavy up*/ << QPoint(3,4)/*Wavy down*/ << QPoint(4,4)/*irregular (ascending or descending)*/ << QPoint(5,2)/*constant*/
//                    /*Angle trends (next 6 pairs) ... Each pair is the (trend specifier, likelihood) */
//                    << QPoint(0,4)/*Ascending*/ << QPoint(1,4)/*Descending*/ << QPoint(2,1)/*Wavy up*/ << QPoint(3,1)/*Wavy down*/ << QPoint(4,4)/*irregular (ascending or descending)*/ << QPoint(5,2)/*constant*/
//                    /*Energy range (13th pair) ... This is the multiplier of the previous energy*/
//                    << QPoint(ui->spinBox_JAT_RS_MinEngyRange->value(),ui->spinBox_JAT_RS_MaxEngyRange->value())/*Energy range*/
//                    /*Angle range (14th pair) */
//                    << QPoint(ui->spinBox_JAT_RS_MinAngleRange->value(),ui->spinBox_JAT_RS_MaxAngleRange->value())/*Angle range ... try (10, 170)*/;
//        skeletonRuleSet->jointsArrangementTrends_RS.insert(2, tempPolygon);

//    }
}

void MainWindow1::updateAll_GLWidgets()
{
    QMapIterator<int, GLWidget*> i(glWidgetID_Map);
    while (i.hasNext())
    {
        i.next().value()->update();
    }
}

QWidget *MainWindow1::createNewTabInTabWidget(QTabWidget *tabWidgetPtr, const QString tabLabel)
{
    QWidget *newTab = new QWidget();
//    newTab->setObjectName(QStringLiteral("newBP"));
    tabWidgetPtr->addTab(newTab, tabLabel);
//    QWidget *verticalLayoutWidget = new QWidget(newTab);
//    verticalLayoutWidget->setObjectName(QStringLiteral("verticalLayoutWidget"));
//    verticalLayoutWidget->setGeometry(QRect(0, 0, 1011, 701));
    QVBoxLayout *verticalLayout = new QVBoxLayout(newTab);
    verticalLayout->setSpacing(1);
    verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
    verticalLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
    verticalLayout->setContentsMargins(0, 0, 0, 0);

//    tabID_Map.insert(tabWidgetPtr->indexOf(newTab), newTab);
    tabID_Map.insert(++this->tabCounter, newTab);
    this->tabWgtToTabMap.insert(this->tabWidgetID_Map.key(tabWidgetPtr), tabID_Map.key(newTab));
    return newTab;
}

QFrame *MainWindow1::addNewFrameToTab(QWidget *tabPtr, const QString &frameName)
{
    QFrame *frame = new QFrame();
    frame->setObjectName(frameName);
//    QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
//    sizePolicy.setHeightForWidth(frame->sizePolicy().hasHeightForWidth());
//    frame->setSizePolicy(sizePolicy);
    frame->setFrameShape(QFrame::StyledPanel);
    frame->setFrameShadow(QFrame::Raised);
    frame->setLineWidth(0);
    tabPtr->layout()->addWidget(frame);

    frameID_Map.insert(frameID_Map.size(), frame);
    return frame;
}

void MainWindow1::keyPressEvent(QKeyEvent *e)
{
//    int xDis=0, yDis=0, zDis=0;

    if (e->key() == Qt::Key_Escape)
    {
        QMapIterator<int, GLWidget*> glWgtIter(glWidgetID_Map);
        while (glWgtIter.hasNext())
        {
            glWgtIter.next();
            delete glWgtIter.value();
        }

        close();
    }
//    else if (e->key() == Qt::Key_T)
//    {
//        xDis=(rand()%1000)-500; yDis=(rand()%1000)-500; zDis=0;
//        // translate the model to a random location
//        GeometryAlgorithms::translateModel(glWidget->allAliensToRender[0]->rootCellPtr,
//                                        xDis, yDis, zDis); // random numbers between -500 and +500

//        glWidget->update();
//    }

//    else if (e->key() == Qt::Key_Y)
//    {
//        // rotate the model along yaw (up) axis
//        GeometryAlgorithms::rotateCell(glWidget->allAliensToRender[0]->rootCellPtr, 0.0, 5.0, 0.0);
//        glWidget->update();

//    }

//    else if (e->key() == Qt::Key_P)
//    {
//        // rotate the model along pitch (x) axis
//        GeometryAlgorithms::rotateCell(glWidget->allAliensToRender[0]->rootCellPtr, 5.0, 0.0, 0.0);
//        glWidget->update();

//    }

    else if (e->key() == Qt::Key_C)
    {
        QMapIterator<int, GLWidget*> glWgtIter(glWidgetID_Map);
        while (glWgtIter.hasNext())
        {
            glWgtIter.next();
            glWgtIter.value()->R_keyPressed = false;
            glWgtIter.value()->update();
        }
    }


    else if (e->key() == Qt::Key_R)
    {
        QMapIterator<int, GLWidget*> glWgtIter(glWidgetID_Map);
        while (glWgtIter.hasNext())
        {
            glWgtIter.next();
            // Render the rotation gizmo inside the GLW


            glWgtIter.value()->R_keyPressed = true;
            glWgtIter.value()->update();
        }
    }

//    else if (e->key() == Qt::Key_1)
//    {
//        // scale the model along roll (forward) axis
//        // rotate the model along roll (forward) axis
//        Cell *tempCell = glWidget->allAliensToRender[0]->rootCellPtr;
//        GeometryAlgorithms::scaleCell(tempCell, 1.05, 1.0, 1.05);
//        glWidget->update();
//    }

    else if (e->key() == Qt::Key_W)
    {
        // move the camera towards its roll/forward vector OR move the world towards the camera
//        glWidget->m_ZTrans +=0.5;
//        glWidget->update();
    }

    else if (e->key() == Qt::Key_2)
    {
        // scale the model along yaw (up) axis
    }

    else if (e->key() == Qt::Key_3)
    {
        // scale the model along pitch (x) axis
    }

    else
        QWidget::keyPressEvent(e);
}

void MainWindow1::keyReleaseEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_T)
    {
//        glWidget->T_keyPressed=false;
    }

    else if (e->key() == Qt::Key_W)
    {
        // move the camera towards its roll/forward vector OR move the world towards the camera
//        glWidget->m_ZTrans =0.0;
//        glWidget->update();
    }

    else if (e->key() == Qt::Key_C)
    {
        QMapIterator<int, GLWidget*> glWgtIter(glWidgetID_Map);
        while (glWgtIter.hasNext())
        {
            glWgtIter.next();
            glWgtIter.value()->R_keyPressed = false;
            glWgtIter.value()->update();
        }
    }

    else if (e->key() == Qt::Key_R)
    {
        QMapIterator<int, GLWidget*> glWgtIter(glWidgetID_Map);
        while (glWgtIter.hasNext())
        {
            glWgtIter.next();
            glWgtIter.value()->R_keyPressed = true;
            glWgtIter.value()->update();
        }
    }
}


void MainWindow1::dockUndock()
{
    //if (parent()) {
    //    setParent(0);
    //    setAttribute(Qt::WA_DeleteOnClose);
    //    move(QApplication::desktop()->width() / 2 - width() / 2,
    //         QApplication::desktop()->height() / 2 - height() / 2);
    //    dockBtn->setText(tr("Dock"));
    //    show();
    //} else {
    //    if (!mainWindow->centralWidget()) {
    //        if (mainWindow->isVisible()) {
    //            setAttribute(Qt::WA_DeleteOnClose, false);
    //            dockBtn->setText(tr("Undock"));
    //            mainWindow->setCentralWidget(this);
    //        } else {
    //            QMessageBox::information(0, tr("Cannot dock"), tr("Main window already closed"));
    //        }
    //    } else {
    //        QMessageBox::information(0, tr("Cannot dock"), tr("Main window already occupied"));
    //    }
    //}
}

void MainWindow1::handlePushButton_MutateBP_BulkinessDNA()
{
    qDebug() << "mutate bulkify BP button clicked.";

    qDebug() << alModlerPtr->selectedAlienAndBP_ID;
    if (!alModlerPtr->selectedAlienAndBP_ID.isEmpty())
    {
        if (alModlerPtr->selectedAlienAndBP_ID.begin().value().first != -1
            &&
            alModlerPtr->selectedAlienAndBP_ID.begin().value().second != -1)
        {
            // Get the Alien and BP that was selected by the user. It is always updated in the AlienModeller
            int key = alModlerPtr->selectedAlienAndBP_ID.begin().key();
            GLWidget *wgt = glWidgetID_Map.value(key);
            Alien *alienPtr = alModlerPtr->getAlienFromID(alModlerPtr->selectedAlienAndBP_ID.value(key).first);
            int BP_ID = alModlerPtr->selectedAlienAndBP_ID.value(key).second;
            BodyPart *bpPtr = alModlerPtr->getBodyPartFromID(alienPtr, BP_ID);

            if (bpPtr)
            {
                // get the values from combobox, combobox_2, radiobutton_8, radiobutton_9
                QString trendStr = ui->comboBox->currentText();
                QString XY_ScalingAxisStr = ui->comboBox_2->currentText();

                float bulkTrendID = -1;
                QPointF XY_ScalingAxisFlags = QPointF(0,0);
                if (trendStr == "Ascending")
                    bulkTrendID = 1;
                else if (trendStr == "Descending")
                    bulkTrendID = 2;
                else if (trendStr == "Irregular")
                    bulkTrendID = 4;

                if (XY_ScalingAxisStr == "Both X and Y")
                {
                    XY_ScalingAxisFlags.setX(1);
                    XY_ScalingAxisFlags.setY(1);
                }
                else if (XY_ScalingAxisStr == "X only (Tapered Style)")
                    XY_ScalingAxisFlags.setX(1);
                else if (XY_ScalingAxisStr == "Y only")
                    XY_ScalingAxisFlags.setY(1);

                float minStartScaleVal = ui->doubleSpinBox_10->value();
                float maxStartScaleVal = ui->doubleSpinBox_11->value();
                float minScaleMultiplier = ui->doubleSpinBox_13->value();
                float maxScaleMultiplier = ui->doubleSpinBox_12->value();
                // Also have controls for X and Y scale values

                if (minStartScaleVal >= maxStartScaleVal)
                    maxStartScaleVal = minStartScaleVal+0.1;
                if (minScaleMultiplier >= maxScaleMultiplier)
                    maxScaleMultiplier = minScaleMultiplier+0.1;

                float newScaleVal = GeometryAlgorithms::getRandomFloat(minStartScaleVal, maxStartScaleVal);
                float newScaleMult = GeometryAlgorithms::getRandomFloat(minScaleMultiplier, maxScaleMultiplier);

                QVector<int> BP_IDsToBulkifyList; BP_IDsToBulkifyList.clear();
                BP_IDsToBulkifyList << BP_ID;
                alModlerPtr->modifySkeletalBP_BulkinessDNA(alienPtr, BP_IDsToBulkifyList, -1, -1, bulkTrendID, XY_ScalingAxisFlags, QPointF(newScaleVal,newScaleMult));
                BodyPart *mirrPtr0 = alModlerPtr->getMirrorOrOriginalBP(alienPtr, bpPtr);
                if (mirrPtr0)
                {
                    BP_IDsToBulkifyList.clear();
                    BP_IDsToBulkifyList << mirrPtr0->BP_ID;
                    alModlerPtr->modifySkeletalBP_BulkinessDNA(alienPtr, BP_IDsToBulkifyList, -1, -1, bulkTrendID, XY_ScalingAxisFlags, QPointF(newScaleVal,newScaleMult));
                }

                wgt->update();
            }


            // if (bpPtr)
            // {
            //     QVector<BodyPart *> allRenderableBPsList; allRenderableBPsList.clear();
            //     QVector<int> allRenderableBPsIDsList; allRenderableBPsIDsList.clear();
            //     alModlerPtr->getAllRenderableBPs(alienPtr, allRenderableBPsList, key);
            //     QVector<int> whoseMirrorsToRender; whoseMirrorsToRender.clear();
            //     // We need to extract the IDs of those BPs that are mirrors of the BPs in this list
            //     for (BodyPart *bp : allRenderableBPsList)
            //     {
            //         allRenderableBPsIDsList << bp->BP_ID;
            //         // If this BP has a mirror, note it down in the whoseMirrorsToRender so we can render its mirror after
            //         if (!bp->isSymmetric)
            //         {
            //             BodyPart *mirrPtr0 = alModlerPtr->getMirrorOrOriginalBP(alienPtr, bp);
            //             if (mirrPtr0)
            //             {
            //                 if (allRenderableBPsList.contains(mirrPtr0))
            //                     whoseMirrorsToRender << bp->BP_ID;
            //                 if (bp != bpPtr)
            //                 {
            //                     if (!allRenderableBPsIDsList.contains(mirrPtr0->BP_ID))
            //                         allRenderableBPsIDsList << mirrPtr0->BP_ID;
            //                 }
            //             }
            //         }
            //     }


            //     // get the values from combobox, combobox_2, radiobutton_8, radiobutton_9
            //     QString trendStr = ui->comboBox->currentText();
            //     QString XY_ScalingAxisStr = ui->comboBox_2->currentText();

            //     float bulkTrendID = -1;
            //     QPointF XY_ScalingAxisFlags = QPointF(0,0);
            //     if (trendStr == "Ascending")
            //         bulkTrendID = 1;
            //     else if (trendStr == "Descending")
            //         bulkTrendID = 2;
            //     else if (trendStr == "Irregular")
            //         bulkTrendID = 4;

            //     if (XY_ScalingAxisStr == "Both X and Y")
            //     {
            //         XY_ScalingAxisFlags.setX(1);
            //         XY_ScalingAxisFlags.setY(1);
            //     }
            //     else if (XY_ScalingAxisStr == "X only (Tapered Style)")
            //         XY_ScalingAxisFlags.setX(1);
            //     else if (XY_ScalingAxisStr == "Y only")
            //         XY_ScalingAxisFlags.setY(1);

            //     float minStartScaleVal = ui->doubleSpinBox_10->value();
            //     float maxStartScaleVal = ui->doubleSpinBox_11->value();
            //     float minScaleMultiplier = ui->doubleSpinBox_13->value();
            //     float maxScaleMultiplier = ui->doubleSpinBox_12->value();
            //     // Also have controls for X and Y scale values

            //     if (minStartScaleVal >= maxStartScaleVal)
            //         maxStartScaleVal = minStartScaleVal+0.1;
            //     if (minScaleMultiplier >= maxScaleMultiplier)
            //         maxScaleMultiplier = minScaleMultiplier+0.1;

            //     float newScaleVal = GeometryAlgorithms::getRandomFloat(minStartScaleVal, maxStartScaleVal);
            //     float newScaleMult = GeometryAlgorithms::getRandomFloat(minScaleMultiplier, maxScaleMultiplier);

            //     QVector<int> BP_ID_List; BP_ID_List.clear();
            //     BP_ID_List << BP_ID;
            //     alModlerPtr->modifySkeletalBP_BulkinessDNA(alienPtr, BP_ID_List, -1, -1, bulkTrendID, XY_ScalingAxisFlags, QPointF(newScaleVal,newScaleMult));


            //     for (int i=0; i<allRenderableBPsIDsList.length(); i++)
            //         if (!alModlerPtr->getBodyPartFromID(alienPtr, allRenderableBPsIDsList[i]))
            //             allRenderableBPsIDsList.remove(i--);

            //     for (int bpID : whoseMirrorsToRender)
            //     {
            //         BodyPart *currBP_Ptr = alModlerPtr->getBodyPartFromID(alienPtr, bpID);
            //         if (currBP_Ptr)
            //         {
            //             if (!currBP_Ptr->isSymmetric)
            //             {
            //                 BodyPart *currBP_MirrPtr = alModlerPtr->getMirrorOrOriginalBP(alienPtr, currBP_Ptr);
            //                 if (currBP_MirrPtr)
            //                 {
            //                     if (!allRenderableBPsIDsList.contains(currBP_MirrPtr->BP_ID))
            //                         allRenderableBPsIDsList << currBP_MirrPtr->BP_ID;
            //                 }
            //             }
            //         }
            //     }

                // QVector<int> BP_ListToDisplayInDiffColor; BP_ListToDisplayInDiffColor.clear();
                // BP_ListToDisplayInDiffColor << bpPtr->BP_ID;
                // // update the GLW_to_Alien_to_BP_to_ColorDisplay_Map
                // this->alModlerPtr->updateGLW_to_Alien_to_BP_to_ColorDisplay_Map(alienPtr,
                //                                                                 wgt->glWidgetID, // get the GLW_ID
                //                                                                 allRenderableBPsIDsList, BP_ListToDisplayInDiffColor,
                //                                                                 AlienModeller::ColorEnum::RED);

            //     wgt->update();
            // }
        }
    }
    else
        qDebug() << "BP not selected";
    qDebug() << "reached end";
}

void MainWindow1::handlePushButton_27_UpdateBulkinessRS_WithGUI_SaveValues()
{
    // Get the values from the comboboxes
    QString tempBP_TypeStr = ui->comboBox_12->currentText();
    QString tempBulkTrndStr = ui->comboBox_11->currentText();

    int minStartScale = ui->doubleSpinBox->value();
    int maxStartScale = ui->doubleSpinBox_2->value();
    int minScaleMult = ui->doubleSpinBox_3->value();
    int maxScaleMult = ui->doubleSpinBox_4->value();

    QPolygonF tempValuePoly; tempValuePoly.clear();
    tempValuePoly << QPointF(alModlerPtr->BP_TypeMap.value(tempBP_TypeStr,-1), alModlerPtr->skeletonRuleSet->BulkinessTrendsMap.value(tempBulkTrndStr,-1))
                  << QPointF(minStartScale, maxStartScale)
                  << QPointF(minScaleMult, maxScaleMult)
                  << QPointF(ui->radioButton_5->isChecked(), ui->radioButton_6->isChecked());

    // modify the default values of the ruleset with the user specified values
    alModlerPtr->skeletonRuleSet->modifySkeletal_BP_Bulkiness_RS(tempValuePoly);
}

void MainWindow1::handlePushButton_17__ReverseBP_BulkinessTrend()
{
    /*
     * ALGO
     *
     * If the trend is Ascending, change it to Descending
     * and wiseversa
     *
     * Call the bulkify method again on the selected BP
     *
    */

    qDebug() << "Reverse BP Bulkiness BP button clicked.";

    if (!alModlerPtr->selectedAlienAndBP_ID.isEmpty())
    {
        qDebug() << alModlerPtr->selectedAlienAndBP_ID;
        if (alModlerPtr->selectedAlienAndBP_ID.begin().value().first != -1
            &&
            alModlerPtr->selectedAlienAndBP_ID.begin().value().second != -1)
        {
            // Get the Alien and BP that was selected by the user. It is always updated in the AlienModeller
            int key = alModlerPtr->selectedAlienAndBP_ID.begin().key();
            GLWidget *wgt = glWidgetID_Map.value(key);
            Alien *alienPtr = alModlerPtr->getAlienFromID(alModlerPtr->selectedAlienAndBP_ID.value(key).first);
            int BP_ID = alModlerPtr->selectedAlienAndBP_ID.value(key).second;
            QVector<int> BP_ID_List; BP_ID_List.clear();
            BP_ID_List << BP_ID;

            // get the current bulkiness trend for this BP from the DNA
            float bulkTrnd = 1;
            if (alienPtr->alienDNA->skeletonDNA->Skeletal_BP_Bulkiness_DNA_ForEachBP.contains(BP_ID))
                bulkTrnd = alienPtr->alienDNA->skeletonDNA->Skeletal_BP_Bulkiness_DNA_ForEachBP.value(BP_ID).at(0).x();
            if (bulkTrnd == 1)
                bulkTrnd = 2;
            else if (bulkTrnd == 2)
                bulkTrnd = 1;

            alModlerPtr->modifySkeletalBP_BulkinessDNA(alienPtr, BP_ID_List, -1, -1, bulkTrnd, QPointF(-1,-1));

            wgt->update();
        }
        else
            qDebug() << "BP not selected";
        qDebug() << "reached end";
    }
}

void MainWindow1::handlePushButton_22__mutateNumOfJointsPerBP_DNA()
{
    qDebug() << "mutate Num of Joints per BPs DNA button clicked.";

    Alien *alienPtr=nullptr;

    for (int i=0; i<alModlerPtr->alienList.length(); i++)
    {
        if (alModlerPtr->alienList[i]->clonedFrom != -1)
        {
            alienPtr = alModlerPtr->alienList[i];
            QVector<int> tempVec; tempVec.clear();
            tempVec << alienPtr->alienBackBones[0]->BP_ID;
            alModlerPtr->modifyNumOfJointsPerBP_DNA(alienPtr, tempVec);
            break;
        }
    }

    QMapIterator<int, GLWidget*> glWgtIter(glWidgetID_Map);
    while (glWgtIter.hasNext())
    {
        glWgtIter.next();
//        qDebug() << "reached 1";
        if (glWgtIter.value()->allAliensToRender.at(0)->alienID == alienPtr->alienID)
        {
//            qDebug() << "reached 2";
            glWgtIter.value()->update();
            break;
        }
    }

    qDebug() << "reached end";
}

void MainWindow1::handlePushButton_2__cloneAlien()
{
    qDebug() << "cloneAlien button clicked";

    // Get the aliens that were double-clicked, clone them and display these in their seperate work areas
    // TODO(ismail): Only the most recently added alien should be cloned, and not all
    // ... Revert the other alien's colors back to their original colors.

    if (!alModlerPtr->selectedAliensInGLWidgets.isEmpty())
    {
        QPair<int,int> p = this->alModlerPtr->selectedAliensInGLWidgets.first();
        Alien *aPtr = this->alModlerPtr->getAlienFromID(p.second);
        // Need to create a clone of this alien
        QVector<Alien*> clonedAlienList; clonedAlienList.clear();
        clonedAlienList.append(this->alModlerPtr->cloneAlien_Algo2(aPtr));
        //        alienModellerObject->symmetrifyAlienBodyParts(clAlienPtr);
        displayAlienOfInterestInNewTab(clonedAlienList, ui->tabWidget_2, "Cloned Alien");
    }
    qDebug() << "reached end";
}

void MainWindow1::handlePushButton_25__ReplaceAliens() // Replace All Aliens in current tab
{
//    QMapIterator<int, QMap<int, QPolygon>> iter(tabToFrameToGLWidget_Map);
    QMap<int,QPair<int,int>> bpAmntsMap; bpAmntsMap.empty();
    QVector<int> tempAliensToDeleteList; tempAliensToDeleteList.clear();

    int tempTabID = tabID_Map.key(ui->tabWidget_2->currentWidget());
    // Go through all the tabs and find the one with 'Aliens' as its title.
//    while (iter.hasNext())
//    {
//        iter.next();
//        if (ui->tabWidget_2->tabText(iter.key()) == "Aliens")
//        {
            // go through all the glwidgets in this tab and erase them and assign new aliens to them
            QMap<int, QPolygon> tempFrameToGLW_Map = tabToFrameToGLWidget_Map.value(tempTabID);
//            QMap<int, QPolygon> tempMap = iter.value();
            QMapIterator<int, QPolygon> tempFrameToGLW_Map_iter(tempFrameToGLW_Map);
            while (tempFrameToGLW_Map_iter.hasNext())
            {
                tempFrameToGLW_Map_iter.next();
                QPolygon allGLWsPairs = tempFrameToGLW_Map_iter.value();
                for (int i=0; i<allGLWsPairs.length(); i++)
                {
                    GLWidget *tempGlW_Ptr = this->glWidgetID_Map.value(allGLWsPairs[i].x());
                    if (tempGlW_Ptr)
                    {
                        if (!tempGlW_Ptr->allAliensToRender.empty())
                        {
                            if (tempGlW_Ptr->allAliensToRender[0])
                                tempAliensToDeleteList.append(tempGlW_Ptr->allAliensToRender[0]->alienID);
                            tempGlW_Ptr->allAliensToRender[0] = nullptr;
                            tempGlW_Ptr->allAliensToRender.clear();
                            // At the moment I'm defaulting it to Bipedal and empty bpAmntsMap
                            // This bpAmntsMap may contain the min and max number of BPs for a particular BP Type (key)
                            alModlerPtr->modelNewAlien_TypeGuided("Bipedal", bpAmntsMap);
                            alModlerPtr->symmetrifyAlienBodyParts(alModlerPtr->alienList.last());

                            QVector<int> BP_ListToDisplay;  BP_ListToDisplay.clear();
                            QVector<int> sepClnBP_ListToDisplay;  sepClnBP_ListToDisplay.clear();
                            QVector<int> BP_ListToDisplayInDiffColor; BP_ListToDisplayInDiffColor.clear();
                            this->alModlerPtr->getAllBP_ID_List(alModlerPtr->alienList.last(), BP_ListToDisplay, sepClnBP_ListToDisplay);
                            // update the GLW_to_Alien_to_BP_to_ColorDisplay_Map
                            this->alModlerPtr->updateGLW_to_Alien_to_BP_to_ColorDisplay_Map(alModlerPtr->alienList.last(),
                                                                                                    tempGlW_Ptr->glWidgetID, // get the GLW_ID
                                                                                                    BP_ListToDisplay, BP_ListToDisplayInDiffColor, -1);

                            assignAlienToGLWidget(tempGlW_Ptr, alModlerPtr->alienList.last());

                            // update the glWgtToAlien_Map
                            this->glWgtToAlien_Map.remove(allGLWsPairs[i].x());
                            this->glWgtToAlien_Map.insert(allGLWsPairs[i].x(), alModlerPtr->alienList.last()->alienID);

                            tempGlW_Ptr->update();
                        }
                    }

                    tempGlW_Ptr = this->glWidgetID_Map.value(allGLWsPairs[i].y());
                    if (tempGlW_Ptr)
                    {
                        if (!tempGlW_Ptr->allAliensToRender.empty())
                        {
                            if (tempGlW_Ptr->allAliensToRender[0])
                                tempAliensToDeleteList.append(tempGlW_Ptr->allAliensToRender[0]->alienID);
                            tempGlW_Ptr->allAliensToRender[0] = nullptr;
                            tempGlW_Ptr->allAliensToRender.clear();
                            alModlerPtr->modelNewAlien_TypeGuided("Bipedal", bpAmntsMap);
                            alModlerPtr->symmetrifyAlienBodyParts(alModlerPtr->alienList.last());

                            QVector<int> BP_ListToDisplay;  BP_ListToDisplay.clear();
                            QVector<int> sepClnBP_ListToDisplay;  sepClnBP_ListToDisplay.clear();
                            QVector<int> BP_ListToDisplayInDiffColor; BP_ListToDisplayInDiffColor.clear();
                            this->alModlerPtr->getAllBP_ID_List(alModlerPtr->alienList.last(), BP_ListToDisplay, sepClnBP_ListToDisplay);
                            // update the GLW_to_Alien_to_BP_to_ColorDisplay_Map
                            this->alModlerPtr->updateGLW_to_Alien_to_BP_to_ColorDisplay_Map(alModlerPtr->alienList.last(),
                                                                                                    tempGlW_Ptr->glWidgetID, // get the GLW_ID
                                                                                                    BP_ListToDisplay, BP_ListToDisplayInDiffColor, -1);

                            assignAlienToGLWidget(tempGlW_Ptr, alModlerPtr->alienList.last());

                            // update the glWgtToAlien_Map
                            this->glWgtToAlien_Map.remove(allGLWsPairs[i].x());
                            this->glWgtToAlien_Map.insert(allGLWsPairs[i].x(), alModlerPtr->alienList.last()->alienID);

                            tempGlW_Ptr->update();
                        }
                    }
                }
            }
//        }
//    }

            alModlerPtr->deleteAliens(tempAliensToDeleteList);
            tempAliensToDeleteList.clear();
}

void MainWindow1::handlePushButton_52__ReplaceSelectedAliens()
{
    qDebug() << "Replace selected Alien button clicked";

    QMap<int,QPair<int,int>> bpAmntsMap; bpAmntsMap.empty();
    QVector <Alien*> tempAliensList; tempAliensList.empty();
    // Get the aliens that were double-clicked, clone them and display these in their seperate work areas
    // TODO(ismail): Only the most recently added alien should be cloned, and not all
    // ... Revert the other alien's colors back to their original colors.
    QVector<int> tempAliensToDeleteList; tempAliensToDeleteList.clear();
    for (QPair<int,int> p : this->alModlerPtr->selectedAliensInGLWidgets)
    {
        Alien *aPtr = this->alModlerPtr->getAlienFromID(p.second);
        GLWidget *tempGlW_Ptr = this->glWidgetID_Map.value(p.first);

        if (tempGlW_Ptr)
        {
            if (!tempGlW_Ptr->allAliensToRender.empty())
            {
                if (aPtr)
                {
                    tempAliensToDeleteList.append(aPtr->alienID);
                    // tempGlW_Ptr->allAliensToRender[0] = nullptr;
                    // tempGlW_Ptr->allAliensToRender.clear();
                    tempGlW_Ptr->removeAlienForRendering(aPtr);
                }
                // At the moment I'm defaulting it to Bipedal and empty bpAmntsMap
                alModlerPtr->modelNewAlien_TypeGuided("Bipedal", bpAmntsMap);
                alModlerPtr->symmetrifyAlienBodyParts(alModlerPtr->alienList.last());

                QVector<int> BP_ListToDisplay;  BP_ListToDisplay.clear();
                QVector<int> sepClnBP_ListToDisplay;  sepClnBP_ListToDisplay.clear();
                QVector<int> BP_ListToDisplayInDiffColor; BP_ListToDisplayInDiffColor.clear();
                this->alModlerPtr->getAllBP_ID_List(alModlerPtr->alienList.last(), BP_ListToDisplay, sepClnBP_ListToDisplay);
                // update the GLW_to_Alien_to_BP_to_ColorDisplay_Map
                this->alModlerPtr->updateGLW_to_Alien_to_BP_to_ColorDisplay_Map(alModlerPtr->alienList.last(),
                                                                                tempGlW_Ptr->glWidgetID, // get the GLW_ID
                                                                                BP_ListToDisplay, BP_ListToDisplayInDiffColor, -1);

                assignAlienToGLWidget(tempGlW_Ptr, alModlerPtr->alienList.last());
                qDebug() << "reached 1";

                // update the glWgtToAlien_Map
                this->glWgtToAlien_Map.remove(p.first);
                this->glWgtToAlien_Map.insert(p.first, alModlerPtr->alienList.last()->alienID);
                qDebug() << "reached 2";

                if (!this->alModlerPtr->selectedAliensInGLWidgets.isEmpty())
                    this->alModlerPtr->selectedAliensInGLWidgets.removeFirst();
                qDebug() << "reached 3";
                tempGlW_Ptr->update();
                qDebug() << "reached 4";
            }
        }
    }
    if (!tempAliensToDeleteList.isEmpty())
        alModlerPtr->deleteAliens(tempAliensToDeleteList);

    tempAliensToDeleteList.clear();

    qDebug() << "Selected Alien/s successfully replaced";
}

void MainWindow1::handlePushButton_70__RemoveSelectedAliens()
{
    qDebug() << "Remove selected Alien button clicked";

    QMap<int,QPair<int,int>> bpAmntsMap; bpAmntsMap.empty();
    QVector <Alien*> tempAliensList; tempAliensList.empty();
    // Get the aliens that were double-clicked, clone them and display these in their seperate work areas
    // TODO(ismail): Only the most recently added alien should be cloned, and not all
    // ... Revert the other alien's colors back to their original colors.
    QVector<int> tempAliensToDeleteList; tempAliensToDeleteList.clear();
    for (QPair<int,int> p : this->alModlerPtr->selectedAliensInGLWidgets)
    {
        Alien *aPtr = this->alModlerPtr->getAlienFromID(p.second);
        GLWidget *tempGlW_Ptr = this->glWidgetID_Map.value(p.first);

        if (tempGlW_Ptr)
        {
            if (!tempGlW_Ptr->allAliensToRender.empty())
            {
                tempAliensToDeleteList.append(aPtr->alienID);
                // tempGlW_Ptr->allAliensToRender[0] = nullptr;
                // tempGlW_Ptr->allAliensToRender.clear();
                tempGlW_Ptr->removeAlienForRendering(aPtr);

                // update the glWgtToAlien_Map
                this->glWgtToAlien_Map.remove(p.first);

                if (!this->alModlerPtr->selectedAliensInGLWidgets.isEmpty())
                    this->alModlerPtr->selectedAliensInGLWidgets.removeFirst();
                tempGlW_Ptr->update();
            }
        }
    }
    if (!tempAliensToDeleteList.isEmpty())
        alModlerPtr->deleteAliens(tempAliensToDeleteList);

    tempAliensToDeleteList.clear();

    qDebug() << "Selected Alien/s successfully removed";

}

void MainWindow1::handlePushButton_29__modelExtremityBPs()
{
    qDebug() << "Model Extremity BP button pressed";

    // This map will hold the min and max values of each BP type that was selected by the user
    // key(int) is the BP_Type_ID. QPair is (min num of BPs, max num of BPs)
    QMap<int, QPair<int,int>> tempBP_AmntMap; tempBP_AmntMap.clear();
    QMap<int, QPoint> tempJnts_AmntMap; tempJnts_AmntMap.clear();

    int alienType = AlienModeller::ALienTypesEnum::BIPEDAL;

    tempBP_AmntMap.insert(AlienModeller::BodyPartsEnum::BACK_BONE, QPair<int,int>(1,2));
    tempBP_AmntMap.insert(AlienModeller::BodyPartsEnum::LOWER_LIMB, QPair<int,int>(1,2));
    tempBP_AmntMap.insert(AlienModeller::BodyPartsEnum::UPPER_LIMB, QPair<int,int>(1,2));

    // modify the default values of the ruleset with the user specified values
    alModlerPtr->skeletonRuleSet->modifyNumOfSkeletalBodyParts_RS(alienType, tempBP_AmntMap);
    tempBP_AmntMap.clear();

    // Get the types of BPs and their respective min and max amounts, from the spin controls
    tempJnts_AmntMap.insert(AlienModeller::BodyPartsEnum::BACK_BONE, QPoint(3,4));
    tempJnts_AmntMap.insert(AlienModeller::BodyPartsEnum::LOWER_LIMB, QPoint(3,4));
    tempJnts_AmntMap.insert(AlienModeller::BodyPartsEnum::UPPER_LIMB, QPoint(3,4));
    alModlerPtr->skeletonRuleSet->modifyMinMaxJntsForBodyParts_RS(alienType, tempJnts_AmntMap);
    tempJnts_AmntMap.clear();
    // Now send this ruleset to the ModelAlien_TypeGuided() method with the values sent as parameters.
    // ... Inside this method, the AlienModeller will temporarily update the ruleset brackets (min and max) values
    // Now create N aliens to be displayed in the first tab
    alModlerPtr->modelNewAlien_TypeGuided("Bipedal", tempBP_AmntMap);
    Alien *alienPtr = alModlerPtr->alienList.last();

    // Now create a new tab with grid style GLWs
    int numOfFrames=3, numOfGLWs=4;
    qDebug() << "reached 1";
    QVector<Alien*> tempAliensList; tempAliensList.clear();
    tempAliensList.append(alienPtr);
    QWidget* tabPtr = createNewTabAndPopulateWithAliens(tempAliensList, ui->tabWidget_2, "Extremity", numOfFrames, numOfGLWs);

    BodyPart *parentBP_Ptr = nullptr;
    if (!alienPtr->alienLowerLimbs.isEmpty())
        parentBP_Ptr = alienPtr->alienLowerLimbs.last();
    else
    {
        qDebug() << "No lower limbs in the alien";
        exit(0);
    }

    // Create several extremity BPs
    QVector<ExtremityBP*> newExtBPsList; newExtBPsList.clear();
    alModlerPtr->modelManyExtremityBPs(alienPtr, numOfFrames*numOfGLWs, newExtBPsList, parentBP_Ptr->BP_ID);

    // Now display these Extremities in the GLWs grid
    QVector<int> fullGLW, emptyGLW;     fullGLW.clear(); emptyGLW.clear();
    getFullAndEmptyGLWsInTab(tabPtr, fullGLW, emptyGLW);
    emptyGLW.append(fullGLW);
    for (int i=0; i<emptyGLW.length(); i++)
    {
        if (glWidgetID_Map.contains(emptyGLW[i]))
        {
            // Now we need to add that Extremity to the GLW for rendering. We need to ensure the Ext BP
            // ... gets displayed in the center only and not attached to any BP (such as Backbone)
            // ... i.e. its a stand alone BP (free BP)
            // Now extract all the carpals and digits (from newExtBPsList) that can actually be rendered in the GLW
            if (!newExtBPsList.isEmpty())
            {
                ExtremityBP *extBP = newExtBPsList.first();
                QVector<BodyPart*> digits, carpals; digits.clear(); carpals.clear();
                extBP->getAllCarpalsAndDigits(digits, carpals);
                QVector<int> BPsListToDisplay, BPsToDisplayInDiffClr;
                BPsListToDisplay.clear(); BPsToDisplayInDiffClr.clear();
                qDebug() << "reached 2";
                for (int i=0; i<carpals.length(); i++)
                    BPsListToDisplay << carpals[i]->BP_ID;
                for (int i=0; i<digits.length(); i++)
                    BPsListToDisplay << digits[i]->BP_ID;

                alModlerPtr->updateGLW_to_Alien_to_BP_to_ColorDisplay_Map(alienPtr, emptyGLW[i], BPsListToDisplay, BPsToDisplayInDiffClr, -1);
                assignAlienToGLWidget(glWidgetID_Map.value(emptyGLW[i]), alienPtr);
                glWidgetID_Map.value(emptyGLW[i])->update();

                newExtBPsList.removeFirst();
            }
        }
    }


    // qDebug() << alModlerPtr->selectedAlienAndBP_ID;
    // if (alModlerPtr->selectedAlienAndBP_ID.begin().value().first != -1
    //     &&
    //     alModlerPtr->selectedAlienAndBP_ID.begin().value().second != -1)
    // {
    //     // Get the Alien and BP that was selected by the user. It is always updated in the AlienModeller
    //     int key = alModlerPtr->selectedAlienAndBP_ID.begin().key();
    //     // GLWidget *wgt = glWidgetID_Map.value(key);
    //     Alien *alienPtr = alModlerPtr->getAlienFromID(alModlerPtr->selectedAlienAndBP_ID.value(key).first);
    //     BodyPart *bpPtr = alModlerPtr->getBodyPartFromID(alienPtr, alModlerPtr->selectedAlienAndBP_ID.value(key).second);

    //     QVector<ExtremityBP*> newExtBPsList; newExtBPsList.clear();
    //     alModlerPtr->modelManyExtremityBPs(alienPtr, 1, newExtBPsList, bpPtr->BP_ID);

    //     // Now extract all the carpals and digits (from newExtBPsList) that can actually be rendered in the GLW
    //     QVector<BodyPart*> digits, carpals; digits.clear(); carpals.clear();
    //     QVector<int> BPsListToDisplay; BPsListToDisplay.clear();
    //     for (ExtremityBP* extPtr : newExtBPsList)
    //     {
    //         extPtr->getAllCarpalsAndDigits(digits, carpals);
    //         qDebug() << "reached 0";
    //         for (int i=0; i<carpals.length(); i++)
    //             BPsListToDisplay << carpals[i]->BP_ID;
    //         for (int i=0; i<digits.length(); i++)
    //             BPsListToDisplay << digits[i]->BP_ID;
    //     }
    //     qDebug() << "reached 1";
    //     QVector<Alien*> tempAliensList; tempAliensList.clear();
    //     tempAliensList.append(alienPtr);
    //     QWidget* tabPtr = createNewTabAndPopulateWithAliens(tempAliensList, ui->tabWidget_2, "Extremity", 1, 2);

    //     QVector<int> fullGLW, emptyGLW;     fullGLW.clear(); emptyGLW.clear();
    //     getFullAndEmptyGLWsInTab(tabPtr, fullGLW, emptyGLW);

    //     // The reason we chose fullGLW is because the fullGLW will have an alien living in it.
    //     for (int i=0; i<fullGLW.length(); i++)
    //     {
    //         // Only display the Carpals and Digits i.e. the hand, foot, paws, claws
    //         QVector<int> BPsToDisplayInDiffClr; BPsToDisplayInDiffClr.clear();
    //         alModlerPtr->updateGLW_to_Alien_to_BP_to_ColorDisplay_Map(alienPtr, fullGLW[i], BPsListToDisplay, BPsToDisplayInDiffClr, -1);

    //         glWidgetID_Map.value(fullGLW[i])->update();
    //     }
    // }
    // else
    //     qDebug() << "BP not selected";

    qDebug() << "reached end";
}

void MainWindow1::handlePushButton_33__attachSeveralExtremityBPsOnRandomBPs()
{
    qDebug() << "Attach several Extremity BPs on Random BPs button pressed";

    qDebug() << alModlerPtr->selectedAlienAndBP_ID;
    if (alModlerPtr->selectedAlienAndBP_ID.begin().value().first != -1
        &&
        alModlerPtr->selectedAlienAndBP_ID.begin().value().second != -1)
    {
        // Get the Alien and BP that was selected by the user. It is always updated in the AlienModeller
        int key = alModlerPtr->selectedAlienAndBP_ID.begin().key();
        GLWidget *wgt = glWidgetID_Map.value(key);
        Alien *alienPtr = alModlerPtr->getAlienFromID(alModlerPtr->selectedAlienAndBP_ID.value(key).first);
        // BodyPart *bpPtr = alModlerPtr->getBodyPartFromID(alienPtr, alModlerPtr->selectedAlienAndBP_ID.value(key).second);

        QVector<BodyPart *> allRenderableBPsList; allRenderableBPsList.clear();
        QVector<int> allRenderableBPsIDsList; allRenderableBPsIDsList.clear();
        alModlerPtr->getAllRenderableBPs(alienPtr, allRenderableBPsList, key);
        for (BodyPart *bp : allRenderableBPsList)
        {
            if (!allRenderableBPsIDsList.contains(bp->BP_ID))
                allRenderableBPsIDsList << bp->BP_ID;
        }

        QVector<ExtremityBP*> newExtBPsList; newExtBPsList.clear();
        // we would like to attach the newly generated extremities to the BP present in allRenderableBPsList
        alModlerPtr->modelAndAttachSeveralExtremBPsOnSeveralRandomParentBPs(alienPtr, 1, newExtBPsList,
                                                                            allRenderableBPsList);

        // Now extract all the carpals and digits (from newExtBPsList) that can actually be rendered in the GLW
        QVector<BodyPart*> digits, carpals; digits.clear(); carpals.clear();
        for (ExtremityBP* extPtr : newExtBPsList)
        {
            extPtr->getAllCarpalsAndDigits(digits, carpals);
            qDebug() << "reached 0";
            for (int i=0; i<carpals.length(); i++)
                allRenderableBPsIDsList << carpals[i]->BP_ID;
            for (int i=0; i<digits.length(); i++)
                allRenderableBPsIDsList << digits[i]->BP_ID;
        }
        qDebug() << "reached 1";

        QVector<int> BP_ListToDisplayInDiffColor; BP_ListToDisplayInDiffColor.clear();
        // update the GLW_to_Alien_to_BP_to_ColorDisplay_Map
        this->alModlerPtr->updateGLW_to_Alien_to_BP_to_ColorDisplay_Map(alienPtr,
                                                                        wgt->glWidgetID, // get the GLW_ID
                                                                        allRenderableBPsIDsList, BP_ListToDisplayInDiffColor,
                                                                        AlienModeller::ColorEnum::RED);

        wgt->update();
    }
    else
        qDebug() << "BP not selected";

    qDebug() << "reached end";

}

void MainWindow1::handlePushButton_47__transferExtBP_FromSourceAlienToDestAlienWithCloning()
{
    qDebug() << "Attach several Extremity BPs on Random BPs button pressed";

    qDebug() << alModlerPtr->selectedAlienAndBP_ID;
    if (!alModlerPtr->selectedAlienAndBP_ID.isEmpty()  &&  alModlerPtr->selectedExtremBP_InGLWidgets!=QVector3D(-1,-1,-1))
    {
        if (alModlerPtr->selectedAlienAndBP_ID.begin().value() != QPair<int,int>(-1,-1))
        {
            // Get the Alien and BP that was selected by the user. It is always updated in the AlienModeller
            int key = alModlerPtr->selectedAlienAndBP_ID.begin().key();
            GLWidget *glw_1 = glWidgetID_Map.value(key);
            Alien *alienPtr_1 = alModlerPtr->getAlienFromID(alModlerPtr->selectedAlienAndBP_ID.value(key).first);
            BodyPart *bpPtr = alModlerPtr->getBodyPartFromID(alienPtr_1, alModlerPtr->selectedAlienAndBP_ID.value(key).second);

            if (bpPtr)
            {
                if (bpPtr->isSymmetric) // get its original BP
                    bpPtr = alModlerPtr->getMirrorOrOriginalBP(alienPtr_1, bpPtr);

                QVector<BodyPart *> allRenderableBPsList; allRenderableBPsList.clear();
                QVector<int> allRenderableBPsIDsList; allRenderableBPsIDsList.clear();
                alModlerPtr->getAllRenderableBPs(alienPtr_1, allRenderableBPsList, key); // key is GLW ID
                for (BodyPart *bp : allRenderableBPsList)
                {
                    if (!allRenderableBPsIDsList.contains(bp->BP_ID))
                        allRenderableBPsIDsList << bp->BP_ID;
                }

                QVector3D triple = this->alModlerPtr->selectedExtremBP_InGLWidgets;
                GLWidget *glw_2 = glWidgetID_Map.value(triple.x());
                Alien *alienPtr_2 = this->alModlerPtr->getAlienFromID(triple.y());
                ExtremityBP *extrBP = alModlerPtr->getExtremityBP_FromID(alienPtr_2, triple.z());

                if (extrBP)
                {

                    extrBP = alModlerPtr->transferExtBP_FromSourceAlienToDestAlienWithCloning(alienPtr_2, alienPtr_1, extrBP, bpPtr->BP_ID);
                    if (extrBP)
                    {
                        // Now extract all the carpals and digits (from newExtBPsList) that can actually be rendered in the GLW
                        // allRenderableBPsIDsList.clear();
                        for (BodyPart *carpal : extrBP->extremBP_basePtr->extremBP_BaseCarpals)
                            allRenderableBPsIDsList << carpal->BP_ID;
                        for (BodyPart *digit : extrBP->extremBP_Digits)
                            allRenderableBPsIDsList << digit->BP_ID;
                        qDebug() << "reached 1";

                        QVector<int> BP_ListToDisplayInDiffColor; BP_ListToDisplayInDiffColor.clear();
                        BP_ListToDisplayInDiffColor << bpPtr->BP_ID;
                        // update the GLW_to_Alien_to_BP_to_ColorDisplay_Map
                        this->alModlerPtr->updateGLW_to_Alien_to_BP_to_ColorDisplay_Map(alienPtr_1,
                                                                                        glw_1->glWidgetID, // get the GLW_ID
                                                                                        allRenderableBPsIDsList, BP_ListToDisplayInDiffColor,
                                                                                        AlienModeller::ColorEnum::RED);

                        glw_1->update();
                        glw_2->update();
                    }
                }
            }
        }
    }
    else
        qDebug() << "BP not selected";

    qDebug() << "reached end";

}

void MainWindow1::handlePushButton_51__RemoveAndDeleteExtremityBP_FromAlien()
{
    qDebug() << "Remove and delete Ext BP from Alien button pressed";

    if (alModlerPtr->selectedExtremBP_InGLWidgets!=QVector3D(-1,-1,-1))
    {
            QVector3D triple = this->alModlerPtr->selectedExtremBP_InGLWidgets;
            GLWidget *glw_2 = glWidgetID_Map.value(triple.x());
            Alien *alienPtr_2 = this->alModlerPtr->getAlienFromID(triple.y());
            ExtremityBP *extrBP = alModlerPtr->getExtremityBP_FromID(alienPtr_2, triple.z());
            if (extrBP)
            {
                BodyPart *parentBP = alModlerPtr->getBodyPartFromID(alienPtr_2, alienPtr_2->extremToParentBP_Map.value(extrBP->BP_ID,-1));
                alModlerPtr->removeAndDeleteExtremityBP_FromAlien(alienPtr_2, extrBP, parentBP);

                glw_2->update();
            }
    }
    else
        qDebug() << "BP not selected";

    qDebug() << "reached end";
}

void MainWindow1::handlePushButton_8__DensifyBodyPart()
{
    qDebug() << "Densify BP button clicked";

    qDebug() << alModlerPtr->selectedAlienAndBP_ID;
    if (!alModlerPtr->selectedAlienAndBP_ID.isEmpty())
    {
        if (alModlerPtr->selectedAlienAndBP_ID.begin().value() != QPair<int,int>(-1,-1))
        {
            // Get the Alien and BP that was selected by the user. It is always updated in the AlienModeller
            int key = alModlerPtr->selectedAlienAndBP_ID.begin().key();
            GLWidget *wgt = glWidgetID_Map.value(key);
            Alien *alienPtr = alModlerPtr->getAlienFromID(alModlerPtr->selectedAlienAndBP_ID.value(key).first);
            BodyPart *bpPtr = alModlerPtr->getBodyPartFromID(alienPtr, alModlerPtr->selectedAlienAndBP_ID.value(key).second);

            if (bpPtr->isSymmetric)
                bpPtr = alModlerPtr->getMirrorOrOriginalBP(alienPtr, bpPtr);

            QVector<int> BP_ID_List; BP_ID_List.clear();
            if (bpPtr->BP_Type != AlienModeller::BodyPartsEnum::BACK_BONE)
            {

                BP_ID_List << bpPtr->BP_ID;
                QPair<int,int> BP_MirrPair = QPair<int,int>(-1,-1);
                alModlerPtr->modifyBP_JAT_rootJoint_DNA(alienPtr, BP_ID_List, BP_MirrPair,
                                                        QPair<bool,bool>(false,false),
                                                        QPair<bool,bool>(false,false),
                                                        QPointF(-1,-1), QPointF(-1,-1));

                bpPtr = alModlerPtr->getBodyPartFromID(alienPtr, BP_MirrPair.first);
            }

            // bring the BP back to origin
            GeometryAlgorithms::translateBP_AccordingToInputTranslationMat(bpPtr, bpPtr->BP_TranslateMatrix.inverted());
            // Unrotate the BP (using inverse quaternion)
            GeometryAlgorithms::rotateBP_AccordingToInputRotationMat(bpPtr, bpPtr->BP_OrientationQuat.inverted());
            // update the cells box geometry
            GeometryAlgorithms::updateBP_CellPrimitiveGeometry(bpPtr);

            // call mutate densification on the BP. Build up densification DNA for this BP
            BP_ID_List.clear();
            BP_ID_List.append(bpPtr->BP_ID);
            alModlerPtr->build_BP_DensificationDNA(alienPtr, BP_ID_List);

            // densify the BP
            alModlerPtr->densifyAllBodyParts(alienPtr, BP_ID_List);

            // rotate it back to its original orientation
            GeometryAlgorithms::rotateBP_AccordingToInputRotationMat(bpPtr, bpPtr->BP_OrientationQuat);
            // take it back to its actual location
            GeometryAlgorithms::translateBP_AccordingToInputTranslationMat(bpPtr, bpPtr->BP_TranslateMatrix);
            // update the cells box geometry
            GeometryAlgorithms::updateBP_CellPrimitiveGeometry(bpPtr);

            BP_ID_List.clear();
            if (bpPtr->BP_Type != AlienModeller::BodyPartsEnum::BACK_BONE)
            {

                BP_ID_List << bpPtr->BP_ID;
                QPair<int,int> BP_MirrPair = QPair<int,int>(-1,-1);
                alModlerPtr->modifyBP_JAT_rootJoint_DNA(alienPtr, BP_ID_List, BP_MirrPair,
                                                        QPair<bool,bool>(false,false),
                                                        QPair<bool,bool>(false,false),
                                                        QPointF(-1,-1), QPointF(-1,-1));

                bpPtr = alModlerPtr->getBodyPartFromID(alienPtr, BP_MirrPair.first);
            }

            QVector<int> BP_ListToDisplayInDiffColor; BP_ListToDisplayInDiffColor.clear();
            QVector<int> BP_ListToDisplay;  BP_ListToDisplay.clear();
            QVector<int> sepClnBP_ListToDisplay;  sepClnBP_ListToDisplay.clear();
            this->alModlerPtr->getAllBP_ID_List(alienPtr, BP_ListToDisplay, sepClnBP_ListToDisplay);
            BP_ListToDisplayInDiffColor << bpPtr->BP_ID;
            // update the GLW_to_Alien_to_BP_to_ColorDisplay_Map
            this->alModlerPtr->updateGLW_to_Alien_to_BP_to_ColorDisplay_Map(alienPtr,
                                                                            wgt->glWidgetID, // get the GLW_ID
                                                                            BP_ListToDisplay, BP_ListToDisplayInDiffColor, AlienModeller::ColorEnum::RED);

            wgt->update();
            alModlerPtr->selectedAlienAndBP_ID[key].second = bpPtr->BP_ID;
        }
    }
    else
        qDebug() << "BP not selected";

    qDebug() << "reached end";
}

void MainWindow1::handlePushButton_12__ApplySepBP_ChangesToOrigBP_OnAlien()
{
    qDebug() << "Clone BP and display in new tab button clicked";

    qDebug() << alModlerPtr->selectedAlienAndBP_ID;
    if (!alModlerPtr->selectedAlienAndBP_ID.isEmpty())
    {
        if (alModlerPtr->selectedAlienAndBP_ID.begin().value().first != -1
            &&
            alModlerPtr->selectedAlienAndBP_ID.begin().value().second != -1)
        {
            // Get the Alien and BP that was selected by the user. It is always updated in the AlienModeller
            // selectedAlienAndBP_ID will always have maximum one item
            int key = alModlerPtr->selectedAlienAndBP_ID.begin().key();
            GLWidget *wgt = glWidgetID_Map.value(key);
            Alien *alienPtr = alModlerPtr->getAlienFromID(alModlerPtr->selectedAlienAndBP_ID.value(key).first);
            BodyPart *bpPtr = alModlerPtr->getBodyPartFromID(alienPtr, alModlerPtr->selectedAlienAndBP_ID.value(key).second);

            if (wgt && alienPtr && bpPtr)
            {
                // Get the original BP from which is was cloned from
                BodyPart *origBP_Ptr = alModlerPtr->getOriginalBP_FromClonedBP(bpPtr);
                if (origBP_Ptr)
                {
                    // we also need GLW where the actual alien is being rendered
                    // We need to find that GLW with the same Alien as this BP's alien and it does not render this BP
                    QVector<int> candidateGLWsToSearchForBP = glWgtToAlienOfInterest_Map.keys(alienPtr->alienID);
                    qDebug() << candidateGLWsToSearchForBP;
                    int glw2 = alModlerPtr->getGLW_ID_ThatDisplaysThisAlienAndBP(alienPtr->alienID, origBP_Ptr->BP_ID, candidateGLWsToSearchForBP);
                    if (glw2 == -1)
                        exit(0);

                    QVector<BodyPart *> allRenderableBPsList; allRenderableBPsList.clear();
                    QVector<int> allRenderableBPsIDsList; allRenderableBPsIDsList.clear();
                    alModlerPtr->getAllRenderableBPs(alienPtr, allRenderableBPsList, glw2);
                    for (BodyPart *bp : allRenderableBPsList)
                    {
                        if (!allRenderableBPsIDsList.contains(bp->BP_ID))
                            allRenderableBPsIDsList << bp->BP_ID;
                    }

                    // create a clone of the bpPtr in seperateClones list
                    // reason for cloning is to be able to keep modifying the seperate BP in its GLW
                    bpPtr = alModlerPtr->createSeperateCloneBP(alienPtr, bpPtr);
                    // Now replace the orig BP with this new seperate clone
                    QPair<int,int> bpAndMirrPair = alModlerPtr->applySeperateBPChangesToOrigBPInAlien(alienPtr, bpPtr, origBP_Ptr);

                    // Remove IDs of those BPs that are no more in the Alien after the modification of JAT
                    for (int i=0; i<allRenderableBPsIDsList.length(); i++)
                        if (!alModlerPtr->getBodyPartFromID(alienPtr, allRenderableBPsIDsList[i]))
                            allRenderableBPsIDsList.remove(i--);


                    if (bpAndMirrPair.first != -1)
                    {
                        if (!allRenderableBPsIDsList.contains(bpAndMirrPair.first))
                            allRenderableBPsIDsList.append(bpAndMirrPair.first);
                        QVector<int> BP_ListToDisplayInDiffColor; BP_ListToDisplayInDiffColor.clear();
                        BP_ListToDisplayInDiffColor << bpAndMirrPair.first;

                        if (bpAndMirrPair.second != -1)
                        {
                            if (!allRenderableBPsIDsList.contains(bpAndMirrPair.second))
                                allRenderableBPsIDsList.append(bpAndMirrPair.second);
                        }

                        // update the GLW_to_Alien_to_BP_to_ColorDisplay_Map
                        this->alModlerPtr->updateGLW_to_Alien_to_BP_to_ColorDisplay_Map(alienPtr,
                                                                                        glw2, // get the GLW_ID
                                                                                        allRenderableBPsIDsList, BP_ListToDisplayInDiffColor,
                                                                                        AlienModeller::ColorEnum::RED);
                        glWidgetID_Map.value(glw2)->update();
                        wgt->update();

                        alModlerPtr->selectedAlienAndBP_ID[glw2].second = bpAndMirrPair.first;
                    }

                }
            }
        }
    }
    else
        qDebug() << "BP not selected";

    qDebug() << "reached end";
}

void MainWindow1::handlePushButton_3__CloneBodyPartAndDisplayInNewTabWithAlienOfInterest()
{
    qDebug() << "Clone BP and display in new tab button clicked";

    if (!alModlerPtr->selectedAlienAndBP_ID.isEmpty())
    {
        if (alModlerPtr->selectedAlienAndBP_ID.begin().value().first != -1
            &&
            alModlerPtr->selectedAlienAndBP_ID.begin().value().second != -1)
        {
            // Get the Alien and BP that was selected by the user. It is always updated in the AlienModeller
            // selectedAlienAndBP_ID will always have maximum one item
            int glwID = alModlerPtr->selectedAlienAndBP_ID.begin().key();
            GLWidget *glwPtr = glWidgetID_Map.value(glwID);
            Alien *alienPtr = alModlerPtr->getAlienFromID(alModlerPtr->selectedAlienAndBP_ID.value(glwID).first);
            BodyPart *bpPtr = alModlerPtr->getBodyPartFromID(alienPtr, alModlerPtr->selectedAlienAndBP_ID.value(glwID).second);
            QTabWidget *tempTabWgtPtr = tabWidgetID_Map.value(tabWgtToTabMap.key(getTabContainingGLWidget(glwID)));
            QWidget *tempTabPtr = tabID_Map.value(getTabContainingGLWidget(glwID));

            if (bpPtr->isSymmetric)
            {
                // select the BP that this BP was mirrored from
                bpPtr = alModlerPtr->getBodyPartFromID(alienPtr, bpPtr->mirrorOf);
                // print a message to "please select the original BP as mirror cannot be cloned"
            }

            Alien *alienOfInterest=nullptr;
            // Now first clone the alien that will be displayed in the new tab as AOI
            if (glWgtToAlienOfInterest_Map.contains(glwID)  &&  glWgtToAlienOfInterest_Map.value(glwID) == alienPtr->alienID)
            // i.e. already a tab has been created where this alien is being displayed so no need to create a new one
            {
                alienOfInterest = alienPtr;
            }
            else
            {
                alienOfInterest = alModlerPtr->cloneAlien_Algo2(alienPtr);
                QVector<Alien*> tempAlienList; tempAlienList.clear();
                tempAlienList.append(alienOfInterest);
                tempTabPtr = createNewTabAndPopulateWithAliens(tempAlienList, tempTabWgtPtr, "Alien of Interest", 3, 4, true);

                // get the BP in this AOI that was cloned from the selected BP in the original alien
                // alienOfInterest is the cloned Alien
                bpPtr = alModlerPtr->getBP_ThatWasClonedFromGivenBP(alienOfInterest, bpPtr);
                //            if (bpPtr != nullptr)
                //                qDebug() << "BP cloned from: " << bpPtr->cloneFromAlienAndBP_Pair;
            }

            // Now create clones of this BP. We need to create as many clones as there are empty panels in the GLW
            QVector<int> fullGLW; QVector<int> emptyGLW;
            fullGLW.clear(); emptyGLW.clear();
            getFullAndEmptyGLWsInTab(tempTabPtr, fullGLW, emptyGLW);
            //        qDebug() << "full GLWs in this pane: " << fullGLW;
            //        qDebug() << "empty GLWs in this pane: " << emptyGLW;
            //        exit(0);

            QVector<BodyPart*> clonedBPsForGLW_Panes; clonedBPsForGLW_Panes.clear();
            for (int i=0; i<emptyGLW.length(); i++)
            {
                // We always clone the BP from the AOI
                clonedBPsForGLW_Panes.append(alModlerPtr->createSeperateCloneBP(alienOfInterest, bpPtr));

                // Now we need to add that BP to the GLW for rendering. We need to ensure the BP
                // ... gets displayed in the center only and not attached to any BP (such as Backbone)
                // ... i.e. its a stand alone BP (free BP)
                // Get a list of all Alien's BPs
                QVector<int> BPsListToDisplay, BPsToDisplayInDiffClr;
                BPsListToDisplay.clear(); BPsToDisplayInDiffClr.clear();
                BPsListToDisplay << clonedBPsForGLW_Panes.last()->BP_ID;
                //            BPsToDisplayInDiffClr << clonedBPsForGLW_Panes.last()->BP_ID;
                //            exit(0);
                //            BPsToDisplayInDiffClr << clonedBPsForGLW_Panes.last()->BP_ID;
                alModlerPtr->updateGLW_to_Alien_to_BP_to_ColorDisplay_Map(alienOfInterest, emptyGLW[i], BPsListToDisplay, BPsToDisplayInDiffClr, AlienModeller::ColorEnum::RED);

                assignAlienToGLWidget(glWidgetID_Map.value(emptyGLW[i]), alienOfInterest);

                glWidgetID_Map.value(emptyGLW[i])->update();
            }

            for (int i=0; i<fullGLW.length(); i++)
            {
                // ensure the cloned BPs are not displayed in this AOI GLW pane
                QVector<int> BPsListToDisplay, BPsToDisplayInDiffClr;
                BPsListToDisplay.clear(); BPsToDisplayInDiffClr.clear();
                QVector<int> sepClnBP_ListToDisplay;  sepClnBP_ListToDisplay.clear();
                alModlerPtr->getAllBP_ID_List(alienOfInterest, BPsListToDisplay, sepClnBP_ListToDisplay);
                alModlerPtr->updateGLW_to_Alien_to_BP_to_ColorDisplay_Map(alienOfInterest, fullGLW[i], BPsListToDisplay, BPsToDisplayInDiffClr, -1);

                glWidgetID_Map.value(fullGLW[i])->update();
            }
        }
        else
            qDebug() << "BP not selected";

        qDebug() << "reached end";
    }
}

void MainWindow1::handlePushButton_14__BevelBP()
{
    qDebug() << "Bevel BP button clicked.";

    qDebug() << alModlerPtr->selectedAlienAndBP_ID;
    if (alModlerPtr->selectedAlienAndBP_ID.begin().value().first != -1
        &&
        alModlerPtr->selectedAlienAndBP_ID.begin().value().second != -1)
    {
        // Get the Alien and BP that was selected by the user. It is always updated in the AlienModeller
        int key = alModlerPtr->selectedAlienAndBP_ID.begin().key();
        GLWidget *wgt = glWidgetID_Map.value(key);
        Alien *alienPtr = alModlerPtr->getAlienFromID(alModlerPtr->selectedAlienAndBP_ID.value(key).first);
        BodyPart *bpPtr = alModlerPtr->getBodyPartFromID(alienPtr, alModlerPtr->selectedAlienAndBP_ID.value(key).second);

        alModlerPtr->bevelBP(alienPtr, bpPtr);

        wgt->update();
    }
    else
        qDebug() << "BP not selected";

    qDebug() << "reached end";
}

void MainWindow1::handlePushButton_30__IncreaseNumOfJoints()
{
    qDebug() << "Increase Num of Joints button clicked.";

    qDebug() << alModlerPtr->selectedAlienAndBP_ID;
    if (alModlerPtr->selectedAlienAndBP_ID.begin().value().first != -1
            &&
        alModlerPtr->selectedAlienAndBP_ID.begin().value().second != -1)
    {
        // Get the Alien and BP that was selected by the user. It is always updated in the AlienModeller
        int key = alModlerPtr->selectedAlienAndBP_ID.begin().key();
        GLWidget *wgt = glWidgetID_Map.value(key);
        Alien *alienPtr = alModlerPtr->getAlienFromID(alModlerPtr->selectedAlienAndBP_ID.value(key).first);
        BodyPart *bpPtr = alModlerPtr->getBodyPartFromID(alienPtr, alModlerPtr->selectedAlienAndBP_ID.value(key).second);

        if (bpPtr->isSymmetric) // get its original BP
            bpPtr = alModlerPtr->getMirrorOrOriginalBP(alienPtr, bpPtr);

        if (bpPtr)
        {
            QVector<BodyPart *> allRenderableBPsList; allRenderableBPsList.clear();
            QVector<int> allRenderableBPsIDsList; allRenderableBPsIDsList.clear();
            alModlerPtr->getAllRenderableBPs(alienPtr, allRenderableBPsList, key);
            QVector<int> whoseMirrorsToRender; whoseMirrorsToRender.clear();
            // We need to extract the IDs of those BPs that are mirrors of the BPs in this list
            for (BodyPart *bp : allRenderableBPsList)
            {
                if (!allRenderableBPsIDsList.contains(bp->BP_ID))
                    allRenderableBPsIDsList << bp->BP_ID;
                // If this BP has a mirror, note it down in the whoseMirrorsToRender so we can render its mirror after
                if (!bp->isSymmetric)
                {
                    BodyPart *mirrPtr0 = alModlerPtr->getMirrorOrOriginalBP(alienPtr, bp);
                    if (mirrPtr0)
                    {
                        if (allRenderableBPsList.contains(mirrPtr0))
                        {
                            if (!whoseMirrorsToRender.contains(bp->BP_ID))
                                whoseMirrorsToRender << bp->BP_ID;
                        }
                        if (bp != bpPtr)
                        {
                            if (!allRenderableBPsIDsList.contains(mirrPtr0->BP_ID))
                                allRenderableBPsIDsList << mirrPtr0->BP_ID;
                        }
                    }
                }
            }
            // alModlerPtr->incDecButtonPressed=true; // JUGAAR!!
            bpPtr = alModlerPtr->incDecNumOfJointsForBP(alienPtr, bpPtr,1,false);
            // alModlerPtr->incDecButtonPressed=false;
            // if (alienPtr->alienDNA->skeletonDNA->BP_BulgificationDNA.contains(bpPtr->BP_ID))
            //     alModlerPtr->bulgifyBodyPart(alienPtr, bpPtr);

            if (bpPtr)
            {
                // Remove IDs of those BPs that are no more in the Alien after the modification of JAT
                for (int i=0; i<allRenderableBPsIDsList.length(); i++)
                    if (!alModlerPtr->getBodyPartFromID(alienPtr, allRenderableBPsIDsList[i]))
                        allRenderableBPsIDsList.remove(i--);

                for (int bpID : whoseMirrorsToRender)
                {
                    BodyPart *currBP_Ptr = alModlerPtr->getBodyPartFromID(alienPtr, bpID);
                    if (currBP_Ptr)
                    {
                        if (!currBP_Ptr->isSymmetric)
                        {
                            alModlerPtr->getAllCarpalsAndDigitsIDsOfBP(alienPtr, bpID, allRenderableBPsIDsList);
                            BodyPart *currBP_MirrPtr = alModlerPtr->getMirrorOrOriginalBP(alienPtr, currBP_Ptr);
                            if (currBP_MirrPtr)
                            {
                                if (!allRenderableBPsIDsList.contains(currBP_MirrPtr->BP_ID))
                                {
                                    allRenderableBPsIDsList << currBP_MirrPtr->BP_ID;
                                    alModlerPtr->getAllCarpalsAndDigitsIDsOfBP(alienPtr, currBP_MirrPtr->BP_ID, allRenderableBPsIDsList);
                                }
                            }
                        }
                    }
                }

                QVector<int> BP_ListToDisplayInDiffColor; BP_ListToDisplayInDiffColor.clear();
                BP_ListToDisplayInDiffColor << bpPtr->BP_ID;
                // update the GLW_to_Alien_to_BP_to_ColorDisplay_Map
                this->alModlerPtr->updateGLW_to_Alien_to_BP_to_ColorDisplay_Map(alienPtr,
                                                                                wgt->glWidgetID, // get the GLW_ID
                                                                                allRenderableBPsIDsList, BP_ListToDisplayInDiffColor,
                                                                                AlienModeller::ColorEnum::RED);
                wgt->update();
                alModlerPtr->selectedAlienAndBP_ID[key].second = bpPtr->BP_ID;
            }
        }
    }
    else
        qDebug() << "BP not selected";

    qDebug() << "reached end";
}

void MainWindow1::handlePushButton_31__DecreaseNumOfJoints()
{
    qDebug() << "Increase Num of Joints button clicked.";

    qDebug() << alModlerPtr->selectedAlienAndBP_ID;
    if (alModlerPtr->selectedAlienAndBP_ID.begin().value().first != -1
        &&
        alModlerPtr->selectedAlienAndBP_ID.begin().value().second != -1)
    {
        // Get the Alien and BP that was selected by the user. It is always updated in the AlienModeller
        int key = alModlerPtr->selectedAlienAndBP_ID.begin().key();
        GLWidget *wgt = glWidgetID_Map.value(key);
        Alien *alienPtr = alModlerPtr->getAlienFromID(alModlerPtr->selectedAlienAndBP_ID.value(key).first);
        BodyPart *bpPtr = alModlerPtr->getBodyPartFromID(alienPtr, alModlerPtr->selectedAlienAndBP_ID.value(key).second);

        if (bpPtr->isSymmetric) // get its original BP
            bpPtr = alModlerPtr->getMirrorOrOriginalBP(alienPtr, bpPtr);

        if (bpPtr)
        {
            QVector<BodyPart *> allRenderableBPsList; allRenderableBPsList.clear();
            QVector<int> allRenderableBPsIDsList; allRenderableBPsIDsList.clear();
            alModlerPtr->getAllRenderableBPs(alienPtr, allRenderableBPsList, key);
            QVector<int> whoseMirrorsToRender; whoseMirrorsToRender.clear();
            // We need to extract the IDs of those BPs that are mirrors of the BPs in this list
            for (BodyPart *bp : allRenderableBPsList)
            {
                if (!allRenderableBPsIDsList.contains(bp->BP_ID))
                    allRenderableBPsIDsList << bp->BP_ID;
                // If this BP has a mirror, note it down in the whoseMirrorsToRender so we can render its mirror after
                if (!bp->isSymmetric)
                {
                    BodyPart *mirrPtr0 = alModlerPtr->getMirrorOrOriginalBP(alienPtr, bp);
                    if (mirrPtr0)
                    {
                        if (allRenderableBPsList.contains(mirrPtr0))
                        {
                            if (!whoseMirrorsToRender.contains(bp->BP_ID))
                                whoseMirrorsToRender << bp->BP_ID;
                        }
                        if (bp != bpPtr)
                        {
                            if (!allRenderableBPsIDsList.contains(mirrPtr0->BP_ID))
                                allRenderableBPsIDsList << mirrPtr0->BP_ID;
                        }
                    }
                }
            }

            bpPtr = alModlerPtr->incDecNumOfJointsForBP(alienPtr, bpPtr,-1,false);
            if (bpPtr)
            {
                // Remove IDs of those BPs that are no more in the Alien after the modification of JAT
                for (int i=0; i<allRenderableBPsIDsList.length(); i++)
                    if (!alModlerPtr->getBodyPartFromID(alienPtr, allRenderableBPsIDsList[i]))
                        allRenderableBPsIDsList.remove(i--);

                for (int bpID : whoseMirrorsToRender)
                {
                    BodyPart *currBP_Ptr = alModlerPtr->getBodyPartFromID(alienPtr, bpID);
                    if (currBP_Ptr)
                    {
                        if (!currBP_Ptr->isSymmetric)
                        {
                            alModlerPtr->getAllCarpalsAndDigitsIDsOfBP(alienPtr, bpID, allRenderableBPsIDsList);
                            BodyPart *currBP_MirrPtr = alModlerPtr->getMirrorOrOriginalBP(alienPtr, currBP_Ptr);
                            if (currBP_MirrPtr)
                            {
                                if (!allRenderableBPsIDsList.contains(currBP_MirrPtr->BP_ID))
                                {
                                    allRenderableBPsIDsList << currBP_MirrPtr->BP_ID;
                                    alModlerPtr->getAllCarpalsAndDigitsIDsOfBP(alienPtr, currBP_MirrPtr->BP_ID, allRenderableBPsIDsList);
                                }
                            }
                        }
                    }
                }

                QVector<int> BP_ListToDisplayInDiffColor; BP_ListToDisplayInDiffColor.clear();
                BP_ListToDisplayInDiffColor << bpPtr->BP_ID;
                // update the GLW_to_Alien_to_BP_to_ColorDisplay_Map
                this->alModlerPtr->updateGLW_to_Alien_to_BP_to_ColorDisplay_Map(alienPtr,
                                                                                wgt->glWidgetID, // get the GLW_ID
                                                                                allRenderableBPsIDsList, BP_ListToDisplayInDiffColor,
                                                                                AlienModeller::ColorEnum::RED);
                wgt->update();
                alModlerPtr->selectedAlienAndBP_ID[key].second = bpPtr->BP_ID;
            }
        }
    }
    else
        qDebug() << "BP not selected";

    qDebug() << "reached end";
}

void MainWindow1::handlePushButton_6__SmoothBP()
{
    qDebug() << "Smooth BP button clicked";

    qDebug() << alModlerPtr->selectedAlienAndBP_ID;
    if (alModlerPtr->selectedAlienAndBP_ID.begin().value().first != -1
            &&
        alModlerPtr->selectedAlienAndBP_ID.begin().value().second != -1)
    {
        // Get the Alien and BP that was selected by the user. It is always updated in the AlienModeller
        int key = alModlerPtr->selectedAlienAndBP_ID.begin().key();
        GLWidget *wgt = glWidgetID_Map.value(key);
        Alien *alienPtr = alModlerPtr->getAlienFromID(alModlerPtr->selectedAlienAndBP_ID.value(key).first);
        BodyPart *bpPtr = alModlerPtr->getBodyPartFromID(alienPtr, alModlerPtr->selectedAlienAndBP_ID.value(key).second);

        if (bpPtr->isSymmetric) // If this is a mirror, get its original BP
            bpPtr = alModlerPtr->getMirrorOrOriginalBP(alienPtr, bpPtr);

        bpPtr = alModlerPtr->smoothenSkeletalBP(alienPtr, bpPtr, 2);

        alModlerPtr->selectedAlienAndBP_ID[key].second = bpPtr->BP_ID;

        wgt->update();
    }
    else
        qDebug() << "BP not selected";
    qDebug() << "reached end";
}

void MainWindow1::handlePushButton_34__IncreaseDensifierSpikiness()
{
    qDebug() << "Increase Spikiness of Densifiers";

    qDebug() << alModlerPtr->selectedAlienAndBP_ID;
    if (alModlerPtr->selectedAlienAndBP_ID.begin().value().first != -1
            &&
        alModlerPtr->selectedAlienAndBP_ID.begin().value().second != -1)
    {
        // Get the Alien and BP that was selected by the user. It is always updated in the AlienModeller
        int key = alModlerPtr->selectedAlienAndBP_ID.begin().key();
        GLWidget *wgt = glWidgetID_Map.value(key);
        Alien *alienPtr = alModlerPtr->getAlienFromID(alModlerPtr->selectedAlienAndBP_ID.value(key).first);
        BodyPart *bpPtr = alModlerPtr->getBodyPartFromID(alienPtr, alModlerPtr->selectedAlienAndBP_ID.value(key).second);

        // bring the BP back to origin
        GeometryAlgorithms::translateBP_AccordingToInputTranslationMat(bpPtr, bpPtr->BP_TranslateMatrix.inverted());
        // Unrotate the BP (using inverse quaternion)
        GeometryAlgorithms::rotateBP_AccordingToInputRotationMat(bpPtr, bpPtr->BP_OrientationQuat.inverted());
        // update the cells box geometry
        GeometryAlgorithms::updateBP_CellPrimitiveGeometry(bpPtr);

        // call mutate densification on the BP
        QVector<int> BP_ID_List; BP_ID_List.clear();
        BP_ID_List.append(bpPtr->BP_ID);
        alModlerPtr->modify_BP_DensificationDNA(alienPtr, BP_ID_List, 0);

        // densify the BP
        alModlerPtr->densifyAllBodyParts(alienPtr, BP_ID_List);

        // take it back to its actual location
        GeometryAlgorithms::rotateBP_AccordingToInputRotationMat(bpPtr, bpPtr->BP_OrientationQuat);
        // take it back to its actual location
        GeometryAlgorithms::translateBP_AccordingToInputTranslationMat(bpPtr, bpPtr->BP_TranslateMatrix);
        // update the cells box geometry
        GeometryAlgorithms::updateBP_CellPrimitiveGeometry(bpPtr);

        wgt->update();

    }
}

void MainWindow1::handlePushButton_35__DecreaseDensifierSpikiness()
{
    qDebug() << "Increase Spikiness of Densifiers";

    qDebug() << alModlerPtr->selectedAlienAndBP_ID;
    if (alModlerPtr->selectedAlienAndBP_ID.begin().value().first != -1
            &&
        alModlerPtr->selectedAlienAndBP_ID.begin().value().second != -1)
    {
        // Get the Alien and BP that was selected by the user. It is always updated in the AlienModeller
        int key = alModlerPtr->selectedAlienAndBP_ID.begin().key();
        GLWidget *wgt = glWidgetID_Map.value(key);
        Alien *alienPtr = alModlerPtr->getAlienFromID(alModlerPtr->selectedAlienAndBP_ID.value(key).first);
        BodyPart *bpPtr = alModlerPtr->getBodyPartFromID(alienPtr, alModlerPtr->selectedAlienAndBP_ID.value(key).second);

        // bring the BP back to origin
        GeometryAlgorithms::translateBP_AccordingToInputTranslationMat(bpPtr, bpPtr->BP_TranslateMatrix.inverted());
        // Unrotate the BP (using inverse quaternion)
        GeometryAlgorithms::rotateBP_AccordingToInputRotationMat(bpPtr, bpPtr->BP_OrientationQuat.inverted());
        // update the cells box geometry
        GeometryAlgorithms::updateBP_CellPrimitiveGeometry(bpPtr);

        // call mutate densification on the BP
        QVector<int> BP_ID_List; BP_ID_List.clear();
        BP_ID_List.append(bpPtr->BP_ID);
        alModlerPtr->modify_BP_DensificationDNA(alienPtr, BP_ID_List, 1);

        // densify the BP
        alModlerPtr->densifyAllBodyParts(alienPtr, BP_ID_List);

        // take it back to its actual location
        GeometryAlgorithms::rotateBP_AccordingToInputRotationMat(bpPtr, bpPtr->BP_OrientationQuat);
        // take it back to its actual location
        GeometryAlgorithms::translateBP_AccordingToInputTranslationMat(bpPtr, bpPtr->BP_TranslateMatrix);
        // update the cells box geometry
        GeometryAlgorithms::updateBP_CellPrimitiveGeometry(bpPtr);

        wgt->update();
    }
}

void MainWindow1::handlePushButton_38__IncreaseDensifierCurvature()
{
    qDebug() << "Increase Curvature of Densifiers";

    qDebug() << alModlerPtr->selectedAlienAndBP_ID;
    if (alModlerPtr->selectedAlienAndBP_ID.begin().value().first != -1
            &&
        alModlerPtr->selectedAlienAndBP_ID.begin().value().second != -1)
    {
        // Get the Alien and BP that was selected by the user. It is always updated in the AlienModeller
        int key = alModlerPtr->selectedAlienAndBP_ID.begin().key();
        GLWidget *wgt = glWidgetID_Map.value(key);
        Alien *alienPtr = alModlerPtr->getAlienFromID(alModlerPtr->selectedAlienAndBP_ID.value(key).first);
        BodyPart *bpPtr = alModlerPtr->getBodyPartFromID(alienPtr, alModlerPtr->selectedAlienAndBP_ID.value(key).second);

        // bring the BP back to origin
        GeometryAlgorithms::translateBP_AccordingToInputTranslationMat(bpPtr, bpPtr->BP_TranslateMatrix.inverted());
        // Unrotate the BP (using inverse quaternion)
        GeometryAlgorithms::rotateBP_AccordingToInputRotationMat(bpPtr, bpPtr->BP_OrientationQuat.inverted());
        // update the cells box geometry
        GeometryAlgorithms::updateBP_CellPrimitiveGeometry(bpPtr);

        // call mutate densification on the BP
        QVector<int> BP_ID_List; BP_ID_List.clear();
        BP_ID_List.append(bpPtr->BP_ID);
        alModlerPtr->modify_BP_DensificationDNA(alienPtr, BP_ID_List, 4);

        // densify the BP
        alModlerPtr->densifyAllBodyParts(alienPtr, BP_ID_List);

        // take it back to its actual location
        GeometryAlgorithms::rotateBP_AccordingToInputRotationMat(bpPtr, bpPtr->BP_OrientationQuat);
        // take it back to its actual location
        GeometryAlgorithms::translateBP_AccordingToInputTranslationMat(bpPtr, bpPtr->BP_TranslateMatrix);
        // update the cells box geometry
        GeometryAlgorithms::updateBP_CellPrimitiveGeometry(bpPtr);

        wgt->update();
    }
}

void MainWindow1::handleHorizontalSlider_16__IncreaseDensifierCurvature()
{
    qDebug() << "Increase Dec Curvature of Densifiers using hor slider";

    qDebug() << alModlerPtr->selectedAlienAndBP_ID;
    if (!alModlerPtr->selectedAlienAndBP_ID.isEmpty())
    {
        if (alModlerPtr->selectedAlienAndBP_ID.begin().value() != QPair<int,int>(-1,-1))
        {
            // Get the Alien and BP that was selected by the user. It is always updated in the AlienModeller
            int key = alModlerPtr->selectedAlienAndBP_ID.begin().key();
            GLWidget *wgt = glWidgetID_Map.value(key);
            Alien *alienPtr = alModlerPtr->getAlienFromID(alModlerPtr->selectedAlienAndBP_ID.value(key).first);
            BodyPart *bpPtr = alModlerPtr->getBodyPartFromID(alienPtr, alModlerPtr->selectedAlienAndBP_ID.value(key).second);

            // bring the BP back to origin
            GeometryAlgorithms::translateBP_AccordingToInputTranslationMat(bpPtr, bpPtr->BP_TranslateMatrix.inverted());
            // Unrotate the BP (using inverse quaternion)
            GeometryAlgorithms::rotateBP_AccordingToInputRotationMat(bpPtr, bpPtr->BP_OrientationQuat.inverted());
            // update the cells box geometry
            GeometryAlgorithms::updateBP_CellPrimitiveGeometry(bpPtr);

            // call mutate densification on the BP
            QVector<int> BP_ID_List; BP_ID_List.clear();
            BP_ID_List.append(bpPtr->BP_ID);

            int res = ui->horizontalSlider_16->value() - prevHorizSlider16Value_IncDecDensCurvature;
            // res = res > 0 ? 1 : 2;
            if (res > 0)
            {
                alModlerPtr->modify_BP_DensificationDNA(alienPtr, BP_ID_List, 4);
            }
            else if (res < 0)
            {
                alModlerPtr->modify_BP_DensificationDNA(alienPtr, BP_ID_List, 5);
            }

            // densify the BP
            alModlerPtr->densifyAllBodyParts(alienPtr, BP_ID_List);

            prevHorizSlider16Value_IncDecDensCurvature = ui->horizontalSlider_16->value();


            // take it back to its actual location
            GeometryAlgorithms::rotateBP_AccordingToInputRotationMat(bpPtr, bpPtr->BP_OrientationQuat);
            // take it back to its actual location
            GeometryAlgorithms::translateBP_AccordingToInputTranslationMat(bpPtr, bpPtr->BP_TranslateMatrix);
            // update the cells box geometry
            GeometryAlgorithms::updateBP_CellPrimitiveGeometry(bpPtr);

            wgt->update();
        }
    }



}

void MainWindow1::handleHorizontalSlider_20__incDecDenserSpikeMovingUpDownPerc()
{
    qDebug() << "incDecDenserSpikeMovingUpDownPerc using hor slider";

    qDebug() << alModlerPtr->selectedAlienAndBP_ID;
    if (!alModlerPtr->selectedAlienAndBP_ID.isEmpty())
    {
        if (alModlerPtr->selectedAlienAndBP_ID.begin().value() != QPair<int,int>(-1,-1))
        {
            // Get the Alien and BP that was selected by the user. It is always updated in the AlienModeller
            int key = alModlerPtr->selectedAlienAndBP_ID.begin().key();
            GLWidget *wgt = glWidgetID_Map.value(key);
            Alien *alienPtr = alModlerPtr->getAlienFromID(alModlerPtr->selectedAlienAndBP_ID.value(key).first);
            BodyPart *bpPtr = alModlerPtr->getBodyPartFromID(alienPtr, alModlerPtr->selectedAlienAndBP_ID.value(key).second);

            // bring the BP back to origin
            GeometryAlgorithms::translateBP_AccordingToInputTranslationMat(bpPtr, bpPtr->BP_TranslateMatrix.inverted());
            // Unrotate the BP (using inverse quaternion)
            GeometryAlgorithms::rotateBP_AccordingToInputRotationMat(bpPtr, bpPtr->BP_OrientationQuat.inverted());
            // update the cells box geometry
            GeometryAlgorithms::updateBP_CellPrimitiveGeometry(bpPtr);

            // call mutate densification on the BP
            QVector<int> BP_ID_List; BP_ID_List.clear();
            BP_ID_List.append(bpPtr->BP_ID);

            int res = ui->horizontalSlider_20->value() - prevHorizSlider20Value_incDecDenserSpikeMovingUpDownPerc;
            if (res > 0)
            {
                alModlerPtr->modify_BP_DensificationDNA(alienPtr, BP_ID_List, 6);
            }
            else if (res < 0)
            {
                alModlerPtr->modify_BP_DensificationDNA(alienPtr, BP_ID_List, 7);
            }

            // densify the BP
            alModlerPtr->densifyAllBodyParts(alienPtr, BP_ID_List);

            prevHorizSlider20Value_incDecDenserSpikeMovingUpDownPerc = ui->horizontalSlider_20->value();


            // take it back to its actual location
            GeometryAlgorithms::rotateBP_AccordingToInputRotationMat(bpPtr, bpPtr->BP_OrientationQuat);
            // take it back to its actual location
            GeometryAlgorithms::translateBP_AccordingToInputTranslationMat(bpPtr, bpPtr->BP_TranslateMatrix);
            // update the cells box geometry
            GeometryAlgorithms::updateBP_CellPrimitiveGeometry(bpPtr);

            wgt->update();
        }
    }
}

void MainWindow1::handleHorizontalSlider_23__moveDensersAcrossBP_Bones()
{
    qDebug() << "moveDensersAcrossBP_Bones using hor slider";

    qDebug() << alModlerPtr->selectedAlienAndBP_ID;
    if (!alModlerPtr->selectedAlienAndBP_ID.isEmpty())
    {
        if (alModlerPtr->selectedAlienAndBP_ID.begin().value() != QPair<int,int>(-1,-1))
        {
            // Get the Alien and BP that was selected by the user. It is always updated in the AlienModeller
            int key = alModlerPtr->selectedAlienAndBP_ID.begin().key();
            GLWidget *wgt = glWidgetID_Map.value(key);
            Alien *alienPtr = alModlerPtr->getAlienFromID(alModlerPtr->selectedAlienAndBP_ID.value(key).first);
            BodyPart *bpPtr = alModlerPtr->getBodyPartFromID(alienPtr, alModlerPtr->selectedAlienAndBP_ID.value(key).second);

            // bring the BP back to origin
            GeometryAlgorithms::translateBP_AccordingToInputTranslationMat(bpPtr, bpPtr->BP_TranslateMatrix.inverted());
            // Unrotate the BP (using inverse quaternion)
            GeometryAlgorithms::rotateBP_AccordingToInputRotationMat(bpPtr, bpPtr->BP_OrientationQuat.inverted());
            // update the cells box geometry
            GeometryAlgorithms::updateBP_CellPrimitiveGeometry(bpPtr);

            // call mutate densification on the BP
            QVector<int> BP_ID_List; BP_ID_List.clear();
            BP_ID_List.append(bpPtr->BP_ID);

            int res = ui->horizontalSlider_23->value() - prevHorizSlider23Value_MoveDensersAcrossBP_Bones;
            if (res > 0)
            {
                alModlerPtr->modify_BP_DensificationDNA(alienPtr, BP_ID_List, 8);
            }
            else if (res < 0)
            {
                alModlerPtr->modify_BP_DensificationDNA(alienPtr, BP_ID_List, 9);
            }

            // densify the BP
            alModlerPtr->densifyAllBodyParts(alienPtr, BP_ID_List);

            prevHorizSlider23Value_MoveDensersAcrossBP_Bones = ui->horizontalSlider_23->value();


            // take it back to its actual location
            GeometryAlgorithms::rotateBP_AccordingToInputRotationMat(bpPtr, bpPtr->BP_OrientationQuat);
            // take it back to its actual location
            GeometryAlgorithms::translateBP_AccordingToInputTranslationMat(bpPtr, bpPtr->BP_TranslateMatrix);
            // update the cells box geometry
            GeometryAlgorithms::updateBP_CellPrimitiveGeometry(bpPtr);

            wgt->update();
        }
    }
}

void MainWindow1::handlePushButton_39__DecreaseDensifierCurvature()
{
    qDebug() << "Descrease Curvature of Densifiers";

    qDebug() << alModlerPtr->selectedAlienAndBP_ID;
    if (alModlerPtr->selectedAlienAndBP_ID.begin().value().first != -1
            &&
        alModlerPtr->selectedAlienAndBP_ID.begin().value().second != -1)
    {
        // Get the Alien and BP that was selected by the user. It is always updated in the AlienModeller
        int key = alModlerPtr->selectedAlienAndBP_ID.begin().key();
        GLWidget *wgt = glWidgetID_Map.value(key);
        Alien *alienPtr = alModlerPtr->getAlienFromID(alModlerPtr->selectedAlienAndBP_ID.value(key).first);
        BodyPart *bpPtr = alModlerPtr->getBodyPartFromID(alienPtr, alModlerPtr->selectedAlienAndBP_ID.value(key).second);

        // bring the BP back to origin
        GeometryAlgorithms::translateBP_AccordingToInputTranslationMat(bpPtr, bpPtr->BP_TranslateMatrix.inverted());
        // Unrotate the BP (using inverse quaternion)
        GeometryAlgorithms::rotateBP_AccordingToInputRotationMat(bpPtr, bpPtr->BP_OrientationQuat.inverted());
        // update the cells box geometry
        GeometryAlgorithms::updateBP_CellPrimitiveGeometry(bpPtr);

        // call mutate densification on the BP
        QVector<int> BP_ID_List; BP_ID_List.clear();
        BP_ID_List.append(bpPtr->BP_ID);
        alModlerPtr->modify_BP_DensificationDNA(alienPtr, BP_ID_List, 5);

        // densify the BP
        alModlerPtr->densifyAllBodyParts(alienPtr, BP_ID_List);

        // take it back to its actual location
        GeometryAlgorithms::rotateBP_AccordingToInputRotationMat(bpPtr, bpPtr->BP_OrientationQuat);
        // take it back to its actual location
        GeometryAlgorithms::translateBP_AccordingToInputTranslationMat(bpPtr, bpPtr->BP_TranslateMatrix);
        // update the cells box geometry
        GeometryAlgorithms::updateBP_CellPrimitiveGeometry(bpPtr);

        wgt->update();
    }
}

void MainWindow1::handlePushButton_40__IncreaseDensifierExtrusions()
{
    qDebug() << "Increase Extrusions button clicked";

    qDebug() << alModlerPtr->selectedAlienAndBP_ID;
    if (alModlerPtr->selectedAlienAndBP_ID.begin().value().first != -1
            &&
        alModlerPtr->selectedAlienAndBP_ID.begin().value().second != -1)
    {
        // Get the Alien and BP that was selected by the user. It is always updated in the AlienModeller
        int key = alModlerPtr->selectedAlienAndBP_ID.begin().key();
        GLWidget *wgt = glWidgetID_Map.value(key);
        Alien *alienPtr = alModlerPtr->getAlienFromID(alModlerPtr->selectedAlienAndBP_ID.value(key).first);
        BodyPart *bpPtr = alModlerPtr->getBodyPartFromID(alienPtr, alModlerPtr->selectedAlienAndBP_ID.value(key).second);

        // bring the BP back to origin
        GeometryAlgorithms::translateBP_AccordingToInputTranslationMat(bpPtr, bpPtr->BP_TranslateMatrix.inverted());
        // Unrotate the BP (using inverse quaternion)
        GeometryAlgorithms::rotateBP_AccordingToInputRotationMat(bpPtr, bpPtr->BP_OrientationQuat.inverted());
        // update the cells box geometry
        GeometryAlgorithms::updateBP_CellPrimitiveGeometry(bpPtr);

        // call mutate densification on the BP
        QVector<int> BP_ID_List; BP_ID_List.clear();
        BP_ID_List.append(bpPtr->BP_ID);
        alModlerPtr->modify_BP_DensificationDNA(alienPtr, BP_ID_List, 2);

        // densify the BP
        alModlerPtr->densifyAllBodyParts(alienPtr, BP_ID_List);

        // take it back to its actual location
        GeometryAlgorithms::rotateBP_AccordingToInputRotationMat(bpPtr, bpPtr->BP_OrientationQuat);
        // take it back to its actual location
        GeometryAlgorithms::translateBP_AccordingToInputTranslationMat(bpPtr, bpPtr->BP_TranslateMatrix);
        // update the cells box geometry
        GeometryAlgorithms::updateBP_CellPrimitiveGeometry(bpPtr);

        wgt->update();
    }
}

void MainWindow1::handlePushButton_41__DecreaseDensifierExtrusions()
{
    qDebug() << "Increase Extrusions button clicked";

    qDebug() << alModlerPtr->selectedAlienAndBP_ID;
    if (alModlerPtr->selectedAlienAndBP_ID.begin().value().first != -1
            &&
        alModlerPtr->selectedAlienAndBP_ID.begin().value().second != -1)
    {
        // Get the Alien and BP that was selected by the user. It is always updated in the AlienModeller
        // selectedAlienAndBP_ID will always have maximum one item
        int key = alModlerPtr->selectedAlienAndBP_ID.begin().key();
        GLWidget *wgt = glWidgetID_Map.value(key);
        Alien *alienPtr = alModlerPtr->getAlienFromID(alModlerPtr->selectedAlienAndBP_ID.value(key).first);
        BodyPart *bpPtr = alModlerPtr->getBodyPartFromID(alienPtr, alModlerPtr->selectedAlienAndBP_ID.value(key).second);

        // bring the BP back to origin
        GeometryAlgorithms::translateBP_AccordingToInputTranslationMat(bpPtr, bpPtr->BP_TranslateMatrix.inverted());
        // Unrotate the BP (using inverse quaternion)
        GeometryAlgorithms::rotateBP_AccordingToInputRotationMat(bpPtr, bpPtr->BP_OrientationQuat.inverted());
        // update the cells box geometry
        GeometryAlgorithms::updateBP_CellPrimitiveGeometry(bpPtr);

        // call mutate densification on the BP
        QVector<int> BP_ID_List; BP_ID_List.clear();
        BP_ID_List.append(bpPtr->BP_ID);
        alModlerPtr->modify_BP_DensificationDNA(alienPtr, BP_ID_List, 3);

        // densify the BP
        alModlerPtr->densifyAllBodyParts(alienPtr, BP_ID_List);

        // take it back to its actual location
        GeometryAlgorithms::rotateBP_AccordingToInputRotationMat(bpPtr, bpPtr->BP_OrientationQuat);
        // take it back to its actual location
        GeometryAlgorithms::translateBP_AccordingToInputTranslationMat(bpPtr, bpPtr->BP_TranslateMatrix);
        // update the cells box geometry
        GeometryAlgorithms::updateBP_CellPrimitiveGeometry(bpPtr);

        wgt->update();
    }
}

void MainWindow1::handlePushButton_43__MutateExtremBPsCollectively()
{
    qDebug() << "Mutate Extremity BPs Collectively button clicked";

    qDebug() << alModlerPtr->selectedAlienAndBP_ID;
    if (alModlerPtr->selectedAlienAndBP_ID.begin().value().first != -1
        &&
        alModlerPtr->selectedAlienAndBP_ID.begin().value().second != -1)
    {
        // Get the Alien and BP that was selected by the user. It is always updated in the AlienModeller
        int key = alModlerPtr->selectedAlienAndBP_ID.begin().key();
        GLWidget *wgt = glWidgetID_Map.value(key);
        Alien *alienPtr = alModlerPtr->getAlienFromID(alModlerPtr->selectedAlienAndBP_ID.value(key).first);
        BodyPart *bpPtr = alModlerPtr->getBodyPartFromID(alienPtr, alModlerPtr->selectedAlienAndBP_ID.value(key).second);
        int selectedBP_ID = bpPtr->BP_ID; // for backup

        QVector<BodyPart *> allRenderableBPsList; allRenderableBPsList.clear();
        QVector<int> allRenderableBPsIDsList; allRenderableBPsIDsList.clear();
        alModlerPtr->getAllRenderableBPs(alienPtr, allRenderableBPsList, key);
        QVector<int> sameTypeBPsIDsToBeMutated; sameTypeBPsIDsToBeMutated.clear();
        // Get a list of all the renderable BPs in this GLW
        // ... and extract all those BPs that are extremities of the selected BP type
        for (BodyPart *bp : allRenderableBPsList)
        {
            if (!allRenderableBPsIDsList.contains(bp->BP_ID))
                allRenderableBPsIDsList << bp->BP_ID;

            // extract this BP if this is an extremity of the selected BP type
            if (bp->BP_Type == AlienModeller::BodyPartsEnum::ExtremBP_Digit
                ||
                bp->BP_Type == AlienModeller::BodyPartsEnum::ExtremBP_Carpal)
            {
                if (bpPtr->BP_Type == bp->BP_Type)
                    sameTypeBPsIDsToBeMutated << bp->BP_ID;
            }
        }

        if (!sameTypeBPsIDsToBeMutated.isEmpty())
        {
            // apply JAT mutations on all these same type BPs
            for (int smBP : sameTypeBPsIDsToBeMutated)
            {
                QVector<int> BP_ID_List; BP_ID_List.clear();
                BP_ID_List << smBP;
                QPair<int,int> ID_MirrPair = QPair<int,int>(-1,-1);
                if (ui->comboBox_10->currentText() == "JAT")
                {
                    alModlerPtr->modifyBP_JAT_rootJoint_DNA(alienPtr, BP_ID_List, ID_MirrPair,
                                                            QPair<bool,bool>(true,true), QPair<bool,bool>(true,true),
                                                            QPointF(-1,-1), QPointF(-1,-1));
                }
                else if (ui->comboBox_10->currentText() == "Bulkiness")
                {
                    alModlerPtr->modifySkeletalBP_BulkinessDNA(alienPtr, BP_ID_List);
                }
            }

            // Remove IDs of those BPs that are no more in the Alien after the modification of JAT (if any)
            for (int i=0; i<allRenderableBPsIDsList.length(); i++)
                if (!alModlerPtr->getBodyPartFromID(alienPtr, allRenderableBPsIDsList[i]))
                    allRenderableBPsIDsList.remove(i--);

            QVector<int> BP_ListToDisplayInDiffColor; BP_ListToDisplayInDiffColor.clear();
            BP_ListToDisplayInDiffColor << selectedBP_ID;
            // update the GLW_to_Alien_to_BP_to_ColorDisplay_Map
            this->alModlerPtr->updateGLW_to_Alien_to_BP_to_ColorDisplay_Map(alienPtr,
                                                                            wgt->glWidgetID, // get the GLW_ID
                                                                            allRenderableBPsIDsList, BP_ListToDisplayInDiffColor,
                                                                            AlienModeller::ColorEnum::RED);

            wgt->update();
        }
    }
    else
        qDebug() << "BP not selected";
    qDebug() << "reached end";
}

void MainWindow1::handlePushButton_73__AddNewBPsOfSelectedType()
{
    qDebug() << "Add new BPs of Selected Type button clicked";

    // Get the aliens that were double-clicked, clone them and display these in their seperate work areas
    // TODO(ismail): Only the most recently selected alien should be cloned, and not all
    // ... Revert the other alien's colors back to their original colors.
    if (!alModlerPtr->selectedAliensInGLWidgets.isEmpty())
    {
        QPair<int,int> p = this->alModlerPtr->selectedAliensInGLWidgets.first();
        Alien *aPtr = this->alModlerPtr->getAlienFromID(p.second);

        QString bpTypeName = ui->comboBox_4->currentText();
        int bpTypeID = -1;

        if (bpTypeName == "Upper Limb")
            bpTypeID = AlienModeller::BodyPartsEnum::UPPER_LIMB;
        else if (bpTypeName == "Lower Limb")
            bpTypeID = AlienModeller::BodyPartsEnum::LOWER_LIMB;
        else if (bpTypeName == "Head")
            bpTypeID = AlienModeller::BodyPartsEnum::HEAD;

        int numOfNewBPs = ui->spinBox_31->value();

        if (bpTypeID != -1  &&  numOfNewBPs > 0)
        {
            QVector<BodyPart *> allRenderableBPsList; allRenderableBPsList.clear();
            QVector<int> allRenderableBPsIDsList; allRenderableBPsIDsList.clear();
            alModlerPtr->getAllRenderableBPs(aPtr, allRenderableBPsList, p.first);
            for (BodyPart *bp : allRenderableBPsList)
                allRenderableBPsIDsList << bp->BP_ID;

            QVector<int> newBPsIDs; newBPsIDs.clear();
            alModlerPtr->incDecNumOfBP(aPtr, bpTypeID, true, numOfNewBPs, newBPsIDs);

            QVector<int> BP_ListToDisplayInDiffColor; BP_ListToDisplayInDiffColor.clear();
            if (!newBPsIDs.isEmpty())
            {
                // Now render these new BPs in addition to the ones already displayed in this GLW
                allRenderableBPsIDsList.append(newBPsIDs);
                BP_ListToDisplayInDiffColor.append(newBPsIDs);
            }

            // update the GLW_to_Alien_to_BP_to_ColorDisplay_Map
            this->alModlerPtr->updateGLW_to_Alien_to_BP_to_ColorDisplay_Map(aPtr,
                                                                            p.first, // get the GLW_ID
                                                                            allRenderableBPsIDsList, BP_ListToDisplayInDiffColor,
                                                                            AlienModeller::ColorEnum::RED);

            glWidgetID_Map.value(p.first)->update();

            qDebug() << "reached end";
        }
        else
            qDebug() << "0 new BPs specified";
    }
    else
        qDebug() << "Alien not selected - please first double click an alien";
}

void MainWindow1::handlePushButton_74__RemoveSelectedBP()
{
    qDebug() << "Remove selected BP button pressed";
    if (!alModlerPtr->selectedAlienAndBP_ID.isEmpty())
    {
        if (alModlerPtr->selectedAlienAndBP_ID.begin().value().first != -1
            &&
            alModlerPtr->selectedAlienAndBP_ID.begin().value().second != -1)
        {
            // Get the Alien and BP that was selected by the user. It is always updated in the AlienModeller
            int key = alModlerPtr->selectedAlienAndBP_ID.begin().key();
            GLWidget *wgt = glWidgetID_Map.value(key);
            Alien *alienPtr = alModlerPtr->getAlienFromID(alModlerPtr->selectedAlienAndBP_ID.value(key).first);
            int BP_ID = alModlerPtr->selectedAlienAndBP_ID.value(key).second;

            BodyPart *bpPtr = alModlerPtr->getBodyPartFromID(alienPtr, BP_ID);
            if (bpPtr->BP_Type != AlienModeller::BodyPartsEnum::BACK_BONE)
            {
                BodyPart *origBP_Ptr = nullptr;
                BodyPart *mirrBP_Ptr = nullptr;

                origBP_Ptr = bpPtr;
                mirrBP_Ptr = alModlerPtr->getMirrorOrOriginalBP(alienPtr, bpPtr);
                if (origBP_Ptr)
                {
                    QVector<int> newBP_IDs; newBP_IDs.clear();
                    alModlerPtr->incDecNumOfBP(alienPtr, -1, false, 1, newBP_IDs, origBP_Ptr);
                }
                if (mirrBP_Ptr)
                {
                    // Also delete its mirror (if it exists)
                    QVector<int> newBP_IDs; newBP_IDs.clear();
                    alModlerPtr->incDecNumOfBP(alienPtr, -1, false, 1, newBP_IDs, mirrBP_Ptr);
                }

                wgt->update();
            }
            else
                qDebug() << "Back bone cannot be removed - please modify the backbone using JAT mutation controls";
        }
    }
    else
        qDebug() << "BP not selected";
    qDebug() << "reached end";
}

void MainWindow1::handlePushButton_54__IncreaseBP_Size()
{
    qDebug() << "Mutate JAT of BP button clicked";

    qDebug() << alModlerPtr->selectedAlienAndBP_ID;
    if (alModlerPtr->selectedAlienAndBP_ID.begin().value().first != -1
            &&
        alModlerPtr->selectedAlienAndBP_ID.begin().value().second != -1)
    {
        // Get the Alien and BP that was selected by the user. It is always updated in the AlienModeller
        int key = alModlerPtr->selectedAlienAndBP_ID.begin().key();
        GLWidget *wgt = glWidgetID_Map.value(key);
        Alien *alienPtr = alModlerPtr->getAlienFromID(alModlerPtr->selectedAlienAndBP_ID.value(key).first);
        int BP_ID = alModlerPtr->selectedAlienAndBP_ID.value(key).second;

        QVector3D scaleVec(1.1,1.1,1.1); // scaling Vector
        alModlerPtr->incDecBP_Size(alienPtr, BP_ID, scaleVec);

        wgt->update();
    }
    else
        qDebug() << "BP not selected";
    qDebug() << "reached end";
}

void MainWindow1::handlePushButton_55__DecreaseBP_Size()
{
    qDebug() << "Mutate JAT of BP button clicked";

    qDebug() << alModlerPtr->selectedAlienAndBP_ID;
    if (alModlerPtr->selectedAlienAndBP_ID.begin().value().first != -1
            &&
        alModlerPtr->selectedAlienAndBP_ID.begin().value().second != -1)
    {
        // Get the Alien and BP that was selected by the user. It is always updated in the AlienModeller
        int key = alModlerPtr->selectedAlienAndBP_ID.begin().key();
        GLWidget *wgt = glWidgetID_Map.value(key);
        Alien *alienPtr = alModlerPtr->getAlienFromID(alModlerPtr->selectedAlienAndBP_ID.value(key).first);
        int BP_ID = alModlerPtr->selectedAlienAndBP_ID.value(key).second;

        QVector3D scaleVec(0.9,0.9,0.9); // scaling Vector
        alModlerPtr->incDecBP_Size(alienPtr, BP_ID, scaleVec);

        wgt->update();
    }
    else
        qDebug() << "BP not selected";
    qDebug() << "reached end";
}

void MainWindow1::handlePushButton_20__mutateBP_JAT_Randomly()
{
    qDebug() << "Mutate JAT of BP button clicked";

    qDebug() << alModlerPtr->selectedAlienAndBP_ID;
    if (!alModlerPtr->selectedAlienAndBP_ID.isEmpty())
    {
        if (alModlerPtr->selectedAlienAndBP_ID.begin().value().first != -1
            &&
            alModlerPtr->selectedAlienAndBP_ID.begin().value().second != -1)
        {
            // Get the Alien and BP that was selected by the user. It is always updated in the AlienModeller
            int key = alModlerPtr->selectedAlienAndBP_ID.begin().key();
            GLWidget *wgt = glWidgetID_Map.value(key);
            Alien *alienPtr = alModlerPtr->getAlienFromID(alModlerPtr->selectedAlienAndBP_ID.value(key).first);
            BodyPart *bpPtr1 = alModlerPtr->getBodyPartFromID(alienPtr, alModlerPtr->selectedAlienAndBP_ID.value(key).second);

            if (bpPtr1->isSymmetric) // get its original BP
                bpPtr1 = alModlerPtr->getMirrorOrOriginalBP(alienPtr, bpPtr1);

            QVector<BodyPart *> allRenderableBPsList; allRenderableBPsList.clear();
            QVector<int> allRenderableBPsIDsList; allRenderableBPsIDsList.clear();
            alModlerPtr->getAllRenderableBPs(alienPtr, allRenderableBPsList, key);
            QVector<int> whoseMirrorsToRender; whoseMirrorsToRender.clear();
            // We need to extract the IDs of those BPs that are mirrors of the BPs in this list
            for (BodyPart *bp : allRenderableBPsList)
            {
                if (!allRenderableBPsIDsList.contains(bp->BP_ID))
                    allRenderableBPsIDsList << bp->BP_ID;
                // If this BP has a mirror, note it down in the whoseMirrorsToRender so we can render its mirror after
                if (!bp->isSymmetric)
                {
                    BodyPart *mirrPtr0 = alModlerPtr->getMirrorOrOriginalBP(alienPtr, bp);
                    if (mirrPtr0)
                    {
                        if (allRenderableBPsList.contains(mirrPtr0))
                        {
                            if (!whoseMirrorsToRender.contains(bp->BP_ID))
                                whoseMirrorsToRender << bp->BP_ID;
                        }
                        if (bp != bpPtr1)
                        {
                            if (!allRenderableBPsIDsList.contains(mirrPtr0->BP_ID))
                                allRenderableBPsIDsList << mirrPtr0->BP_ID;
                        }
                    }
                }
            }

            QVector<int> BP_ID_List; BP_ID_List.clear();
            BP_ID_List << bpPtr1->BP_ID;
            QPair<int,int> BP_MirrPair = QPair<int,int>(-1,-1);
            alModlerPtr->modifyBP_JAT_rootJoint_DNA(alienPtr, BP_ID_List, BP_MirrPair,
                                                    QPair<bool,bool>(true,true),
                                                    QPair<bool,bool>(true,true),
                                                    QPointF(-1,-1), QPointF(-1,-1));

            // Remove IDs of those BPs that are no more in the Alien after the modification of JAT
            for (int i=0; i<allRenderableBPsIDsList.length(); i++)
                if (!alModlerPtr->getBodyPartFromID(alienPtr, allRenderableBPsIDsList[i]))
                    allRenderableBPsIDsList.remove(i--);

            for (int bpID : whoseMirrorsToRender)
            {
                BodyPart *currBP_Ptr = alModlerPtr->getBodyPartFromID(alienPtr, bpID);
                if (currBP_Ptr)
                {
                    if (!currBP_Ptr->isSymmetric)
                    {
                        alModlerPtr->getAllCarpalsAndDigitsIDsOfBP(alienPtr, bpID, allRenderableBPsIDsList);
                        BodyPart *currBP_MirrPtr = alModlerPtr->getMirrorOrOriginalBP(alienPtr, currBP_Ptr);
                        if (currBP_MirrPtr)
                        {
                            if (!allRenderableBPsIDsList.contains(currBP_MirrPtr->BP_ID))
                            {
                                allRenderableBPsIDsList << currBP_MirrPtr->BP_ID;
                                alModlerPtr->getAllCarpalsAndDigitsIDsOfBP(alienPtr, currBP_MirrPtr->BP_ID, allRenderableBPsIDsList);
                            }
                        }
                    }
                }
            }

            qDebug() << BP_MirrPair;
            BodyPart *mainBP = alModlerPtr->getBodyPartFromID(alienPtr, BP_MirrPair.first);
            QVector<int> BP_ListToDisplayInDiffColor; BP_ListToDisplayInDiffColor.clear();
            if (mainBP)
            {
                qDebug() << "main BP found";
                BP_ListToDisplayInDiffColor << mainBP->BP_ID;
                alModlerPtr->selectedAlienAndBP_ID[key].second = mainBP->BP_ID;
            }

            // update the GLW_to_Alien_to_BP_to_ColorDisplay_Map
            this->alModlerPtr->updateGLW_to_Alien_to_BP_to_ColorDisplay_Map(alienPtr,
                                                                            wgt->glWidgetID, // get the GLW_ID
                                                                            allRenderableBPsIDsList, BP_ListToDisplayInDiffColor,
                                                                            AlienModeller::ColorEnum::RED);

            // QVector<int> BP_ListToDisplay;  BP_ListToDisplay.clear();
            // QVector<int> sepClnBP_ListToDisplay;  sepClnBP_ListToDisplay.clear();
            // this->alModlerPtr->getAllBP_ID_List(alienPtr, BP_ListToDisplay, sepClnBP_ListToDisplay);
            // // update the GLW_to_Alien_to_BP_to_ColorDisplay_Map
            // this->alModlerPtr->updateGLW_to_Alien_to_BP_to_ColorDisplay_Map(alienPtr,
            //                                                                 wgt->glWidgetID, // get the GLW_ID
            //                                                                 BP_ListToDisplay, BP_ListToDisplayInDiffColor, AlienModeller::ColorEnum::RED);

            wgt->update();
        }
        else
            qDebug() << "BP not selected";
        qDebug() << "reached end";
    }
}

void MainWindow1::handlePushButton_21__mutateBP_JAT_FromOptions()
{
    qDebug() << "Mutate JAT of BP button clicked";

    qDebug() << alModlerPtr->selectedAlienAndBP_ID;
    if (!alModlerPtr->selectedAlienAndBP_ID.isEmpty())
    {
        if (alModlerPtr->selectedAlienAndBP_ID.begin().value().first != -1
            &&
            alModlerPtr->selectedAlienAndBP_ID.begin().value().second != -1)
        {
            // Get the Alien and BP that was selected by the user. It is always updated in the AlienModeller
            int key = alModlerPtr->selectedAlienAndBP_ID.begin().key();
            GLWidget *wgt = glWidgetID_Map.value(key);
            Alien *alienPtr = alModlerPtr->getAlienFromID(alModlerPtr->selectedAlienAndBP_ID.value(key).first);
            BodyPart *bpPtr = alModlerPtr->getBodyPartFromID(alienPtr, alModlerPtr->selectedAlienAndBP_ID.value(key).second);

            if (bpPtr->isSymmetric) // get its original BP
                bpPtr = alModlerPtr->getMirrorOrOriginalBP(alienPtr, bpPtr);

            // Get the type of BP, Energy and Angle Trends from the comboboxes
            QPointF energyVals, angleVals;
            QString tempEngyTrndStr = ui->comboBox_JAT_Mut_SelectEneryTrend->currentText();
            QString tempAngleTrndStr = ui->comboBox_JAT_Mut_SelectAngleTrend->currentText();

            QPolygon tempValuePoly; tempValuePoly.clear();
            energyVals = QPointF(alModlerPtr->skeletonRuleSet->JATsTrendsMap.value(tempEngyTrndStr,-1),
                                 ui->spinBox_JAT_Mut_StartEnergy->value());
            angleVals = QPoint(alModlerPtr->skeletonRuleSet->JATsTrendsMap.value(tempAngleTrndStr,-1),
                               ui->spinBox_JAT_Mut_StartAngle->value());

            ui->horizontalSlider_13->setValue(ui->spinBox_JAT_Mut_StartEnergy->value());
            ui->horizontalSlider_14->setValue(ui->spinBox_JAT_Mut_StartAngle->value());

            QVector<BodyPart *> allRenderableBPsList; allRenderableBPsList.clear();
            QVector<int> allRenderableBPsIDsList; allRenderableBPsIDsList.clear();
            alModlerPtr->getAllRenderableBPs(alienPtr, allRenderableBPsList, key);
            QVector<int> whoseMirrorsToRender; whoseMirrorsToRender.clear();
            // We need to extract the IDs of those BPs that are mirrors of the BPs in this list
            for (BodyPart *bp : allRenderableBPsList)
            {
                allRenderableBPsIDsList << bp->BP_ID;
                // If this BP has a mirror, note it down in the whoseMirrorsToRender so we can render its mirror after
                if (!bp->isSymmetric)
                {
                    BodyPart *mirrPtr0 = alModlerPtr->getMirrorOrOriginalBP(alienPtr, bp);
                    if (mirrPtr0)
                    {
                        if (allRenderableBPsList.contains(mirrPtr0))
                            whoseMirrorsToRender << bp->BP_ID;
                        if (bp != bpPtr)
                            allRenderableBPsIDsList << mirrPtr0->BP_ID;
                    }
                }
            }

            QVector<int> BP_ID_List; BP_ID_List.clear();
            BP_ID_List << bpPtr->BP_ID;
            QPair<int,int> ID_MirrPair = QPair<int,int>(-1,-1);
            energyVals.setY(energyVals.y()*10);
            alModlerPtr->modifyBP_JAT_rootJoint_DNA(alienPtr, BP_ID_List, ID_MirrPair,
                                                    QPair<bool,bool>(true,true), QPair<bool,bool>(true,true),
                                                    energyVals, angleVals);

            // Make sure that BP Ptr is valid and exists in the Alien after the BP was re-JATed
            // Cuz there might be a situation that the pointer goes NULL
            for (int i=0; i<allRenderableBPsIDsList.length(); i++)
                if (!alModlerPtr->getBodyPartFromID(alienPtr, allRenderableBPsIDsList[i]))
                    allRenderableBPsIDsList.remove(i--);

            for (int bpID : whoseMirrorsToRender)
            {
                BodyPart *currBP_Ptr = alModlerPtr->getBodyPartFromID(alienPtr, bpID);
                if (currBP_Ptr)
                {
                    if (!currBP_Ptr->isSymmetric)
                    {
                        alModlerPtr->getAllCarpalsAndDigitsIDsOfBP(alienPtr, bpID, allRenderableBPsIDsList);
                        BodyPart *currBP_MirrPtr = alModlerPtr->getMirrorOrOriginalBP(alienPtr, currBP_Ptr);
                        if (currBP_MirrPtr)
                        {
                            if (!allRenderableBPsIDsList.contains(currBP_MirrPtr->BP_ID))
                            {
                                allRenderableBPsIDsList << currBP_MirrPtr->BP_ID;
                                alModlerPtr->getAllCarpalsAndDigitsIDsOfBP(alienPtr, currBP_MirrPtr->BP_ID, allRenderableBPsIDsList);
                            }
                        }
                    }
                }
            }

            qDebug() << ID_MirrPair;
            BodyPart *mainBP = alModlerPtr->getBodyPartFromID(alienPtr, ID_MirrPair.first);
            QVector<int> BP_ListToDisplayInDiffColor; BP_ListToDisplayInDiffColor.clear();
            if (mainBP)
            {
                qDebug() << "main BP found";
                BP_ListToDisplayInDiffColor << mainBP->BP_ID;
                alModlerPtr->selectedAlienAndBP_ID[key].second = mainBP->BP_ID;
            }
            // update the GLW_to_Alien_to_BP_to_ColorDisplay_Map
            this->alModlerPtr->updateGLW_to_Alien_to_BP_to_ColorDisplay_Map(alienPtr,
                                                                            wgt->glWidgetID, // get the GLW_ID
                                                                            allRenderableBPsIDsList, BP_ListToDisplayInDiffColor,
                                                                            AlienModeller::ColorEnum::RED);

            wgt->update();

        }
        else
            qDebug() << "BP not selected";
        qDebug() << "reached end";
    }
}

void MainWindow1::handlePushButton_18__generateGroupBPs()
{
    qDebug() << "Group BP generation button clicked";

    QVector <Alien*> tempAliensList; tempAliensList.resize(0);
    // Get the aliens that were double-clicked, clone them and display these in their seperate work areas
    for (QPair<int,int> p : this->alModlerPtr->selectedAliensInGLWidgets)
    {
        Alien *alienPtr = this->alModlerPtr->getAlienFromID(p.second);
        GLWidget *glwPtr = glWidgetID_Map.value(p.second);
        QPolygon tempPoly; tempPoly.clear();
        alModlerPtr->updateGroupBP_AttachmentToAnotherBP_ZoneScatteringTrend_DNA(
                    alienPtr, AlienModeller::BodyPartsEnum::UPPER_LIMB, alienPtr->alienBackBones[0]->BP_ID, true, tempPoly);

        QVector<int> groupBPsIDs = alModlerPtr->generateAndAttachGroupBPsToAnotherBP(alienPtr, alienPtr->alienDNA->skeletonDNA->GroupBP_AttachmentToAnotherBP_ZoneScatteringTrend_DNA);

        QVector<int> BP_ListToDisplay;  BP_ListToDisplay.clear();
        QVector<int> sepClnBP_ListToDisplay;  sepClnBP_ListToDisplay.clear();
        QVector<int> BP_ListToDisplayInDiffColor; BP_ListToDisplayInDiffColor.clear();
        this->alModlerPtr->getAllBP_ID_List(alienPtr, BP_ListToDisplay, sepClnBP_ListToDisplay);
//        BP_ListToDisplayInDiffColor << groupBPsIDs;
        // update the GLW_to_Alien_to_BP_to_ColorDisplay_Map
        this->alModlerPtr->updateGLW_to_Alien_to_BP_to_ColorDisplay_Map(alienPtr,
                                                                                glwPtr->glWidgetID, // get the GLW_ID
                                                                                BP_ListToDisplay, BP_ListToDisplayInDiffColor,
                                                                                -1);
    }
    qDebug() << "reached end";
}

void MainWindow1::handlePushButton_15__squashBP()
{
    qDebug() << "Squash BP button clicked";

    qDebug() << alModlerPtr->selectedAlienAndBP_ID;
    if (!alModlerPtr->selectedAlienAndBP_ID.isEmpty())
    {
        if (alModlerPtr->selectedAlienAndBP_ID.begin().value().first != -1
            &&
            alModlerPtr->selectedAlienAndBP_ID.begin().value().second != -1)
        {
            // Get the Alien and BP that was selected by the user. It is always updated in the AlienModeller
            int key = alModlerPtr->selectedAlienAndBP_ID.begin().key();
            GLWidget *wgt = glWidgetID_Map.value(key);
            Alien *alienPtr = alModlerPtr->getAlienFromID(alModlerPtr->selectedAlienAndBP_ID.value(key).first);
            BodyPart *bpPtr = alModlerPtr->getBodyPartFromID(alienPtr, alModlerPtr->selectedAlienAndBP_ID.value(key).second);

            if (bpPtr->isSymmetric) // get its original BP
                bpPtr = alModlerPtr->getMirrorOrOriginalBP(alienPtr, bpPtr);

            QVector<BodyPart *> allRenderableBPsList; allRenderableBPsList.clear();
            QVector<int> allRenderableBPsIDsList; allRenderableBPsIDsList.clear();
            alModlerPtr->getAllRenderableBPs(alienPtr, allRenderableBPsList, key);
            QVector<int> whoseMirrorsToRender; whoseMirrorsToRender.clear();
            // We need to extract the IDs of those BPs that are mirrors of the BPs in this list
            for (BodyPart *bp : allRenderableBPsList)
            {
                if (!allRenderableBPsIDsList.contains(bp->BP_ID))
                    allRenderableBPsIDsList << bp->BP_ID;
                // If this BP has a mirror, note it down in the whoseMirrorsToRender so we can render its mirror after
                if (!bp->isSymmetric)
                {
                    BodyPart *mirrPtr0 = alModlerPtr->getMirrorOrOriginalBP(alienPtr, bp);
                    if (mirrPtr0)
                    {
                        if (allRenderableBPsList.contains(mirrPtr0))
                        {
                            if (!whoseMirrorsToRender.contains(bp->BP_ID))
                                whoseMirrorsToRender << bp->BP_ID;
                        }
                        if (bp != bpPtr)
                        {
                            if (!allRenderableBPsIDsList.contains(mirrPtr0->BP_ID))
                                allRenderableBPsIDsList << mirrPtr0->BP_ID;
                        }
                    }
                }
            }

            bpPtr = alModlerPtr->squashOrStrechBP(alienPtr, bpPtr->BP_ID, 0.0, 0.95);

            // Remove IDs of those BPs that are no more in the Alien after the modification of JAT
            for (int i=0; i<allRenderableBPsIDsList.length(); i++)
                if (!alModlerPtr->getBodyPartFromID(alienPtr, allRenderableBPsIDsList[i]))
                    allRenderableBPsIDsList.remove(i--);

            for (int bpID : whoseMirrorsToRender)
            {
                BodyPart *currBP_Ptr = alModlerPtr->getBodyPartFromID(alienPtr, bpID);
                if (currBP_Ptr)
                {
                    if (!currBP_Ptr->isSymmetric)
                    {
                        alModlerPtr->getAllCarpalsAndDigitsIDsOfBP(alienPtr, bpID, allRenderableBPsIDsList);
                        BodyPart *currBP_MirrPtr = alModlerPtr->getMirrorOrOriginalBP(alienPtr, currBP_Ptr);
                        if (currBP_MirrPtr)
                        {
                            if (!allRenderableBPsIDsList.contains(currBP_MirrPtr->BP_ID))
                            {
                                allRenderableBPsIDsList << currBP_MirrPtr->BP_ID;
                                alModlerPtr->getAllCarpalsAndDigitsIDsOfBP(alienPtr, currBP_MirrPtr->BP_ID, allRenderableBPsIDsList);
                            }
                        }
                    }
                }
            }

            QVector<int> BP_ListToDisplayInDiffColor; BP_ListToDisplayInDiffColor.clear();
            BP_ListToDisplayInDiffColor << bpPtr->BP_ID;
            // update the GLW_to_Alien_to_BP_to_ColorDisplay_Map
            this->alModlerPtr->updateGLW_to_Alien_to_BP_to_ColorDisplay_Map(alienPtr,
                                                                            wgt->glWidgetID, // get the GLW_ID
                                                                            allRenderableBPsIDsList, BP_ListToDisplayInDiffColor,
                                                                            AlienModeller::ColorEnum::RED);
            wgt->update();
            alModlerPtr->selectedAlienAndBP_ID[key].second = bpPtr->BP_ID;
        }
    }
    else
        qDebug() << "BP not selected";
    qDebug() << "reached end";
}

void MainWindow1::handlePushButton_42__stretchBP()
{
    qDebug() << "Squash BP button clicked";

    qDebug() << alModlerPtr->selectedAlienAndBP_ID;
    if (!alModlerPtr->selectedAlienAndBP_ID.isEmpty())
    {
        if (alModlerPtr->selectedAlienAndBP_ID.begin().value().first != -1
            &&
            alModlerPtr->selectedAlienAndBP_ID.begin().value().second != -1)
        {
            // Get the Alien and BP that was selected by the user. It is always updated in the AlienModeller
            int key = alModlerPtr->selectedAlienAndBP_ID.begin().key();
            GLWidget *wgt = glWidgetID_Map.value(key);
            Alien *alienPtr = alModlerPtr->getAlienFromID(alModlerPtr->selectedAlienAndBP_ID.value(key).first);
            BodyPart *bpPtr = alModlerPtr->getBodyPartFromID(alienPtr, alModlerPtr->selectedAlienAndBP_ID.value(key).second);

            if (bpPtr->isSymmetric) // get its original BP
                bpPtr = alModlerPtr->getMirrorOrOriginalBP(alienPtr, bpPtr);

            QVector<BodyPart *> allRenderableBPsList; allRenderableBPsList.clear();
            QVector<int> allRenderableBPsIDsList; allRenderableBPsIDsList.clear();
            alModlerPtr->getAllRenderableBPs(alienPtr, allRenderableBPsList, key);
            QVector<int> whoseMirrorsToRender; whoseMirrorsToRender.clear();
            // We need to extract the IDs of those BPs that are mirrors of the BPs in this list
            for (BodyPart *bp : allRenderableBPsList)
            {
                if (!allRenderableBPsIDsList.contains(bp->BP_ID))
                    allRenderableBPsIDsList << bp->BP_ID;
                // If this BP has a mirror, note it down in the whoseMirrorsToRender so we can render its mirror after
                if (!bp->isSymmetric)
                {
                    BodyPart *mirrPtr0 = alModlerPtr->getMirrorOrOriginalBP(alienPtr, bp);
                    if (mirrPtr0)
                    {
                        if (allRenderableBPsList.contains(mirrPtr0))
                        {
                            if (!whoseMirrorsToRender.contains(bp->BP_ID))
                                whoseMirrorsToRender << bp->BP_ID;
                        }
                        if (bp != bpPtr)
                        {
                            if (!allRenderableBPsIDsList.contains(mirrPtr0->BP_ID))
                                allRenderableBPsIDsList << mirrPtr0->BP_ID;
                        }
                    }
                }
            }

            bpPtr = alModlerPtr->squashOrStrechBP(alienPtr, bpPtr->BP_ID, 1.0, 1.05);

            // Remove IDs of those BPs that are no more in the Alien after the modification of JAT
            for (int i=0; i<allRenderableBPsIDsList.length(); i++)
                if (!alModlerPtr->getBodyPartFromID(alienPtr, allRenderableBPsIDsList[i]))
                    allRenderableBPsIDsList.remove(i--);

            for (int bpID : whoseMirrorsToRender)
            {
                BodyPart *currBP_Ptr = alModlerPtr->getBodyPartFromID(alienPtr, bpID);
                if (currBP_Ptr)
                {
                    if (!currBP_Ptr->isSymmetric)
                    {
                        alModlerPtr->getAllCarpalsAndDigitsIDsOfBP(alienPtr, bpID, allRenderableBPsIDsList);
                        BodyPart *currBP_MirrPtr = alModlerPtr->getMirrorOrOriginalBP(alienPtr, currBP_Ptr);
                        if (currBP_MirrPtr)
                        {
                            if (!allRenderableBPsIDsList.contains(currBP_MirrPtr->BP_ID))
                            {
                                allRenderableBPsIDsList << currBP_MirrPtr->BP_ID;
                                alModlerPtr->getAllCarpalsAndDigitsIDsOfBP(alienPtr, currBP_MirrPtr->BP_ID, allRenderableBPsIDsList);
                            }
                        }
                    }
                }
            }

            QVector<int> BP_ListToDisplayInDiffColor; BP_ListToDisplayInDiffColor.clear();
            BP_ListToDisplayInDiffColor << bpPtr->BP_ID;
            // update the GLW_to_Alien_to_BP_to_ColorDisplay_Map
            this->alModlerPtr->updateGLW_to_Alien_to_BP_to_ColorDisplay_Map(alienPtr,
                                                                            wgt->glWidgetID, // get the GLW_ID
                                                                            allRenderableBPsIDsList, BP_ListToDisplayInDiffColor,
                                                                            AlienModeller::ColorEnum::RED);
            wgt->update();
            alModlerPtr->selectedAlienAndBP_ID[key].second = bpPtr->BP_ID;
        }
    }
    else
        qDebug() << "BP not selected";
    qDebug() << "reached end";

}

void MainWindow1::handlePushButton_26__computeAngleBetweenVectors()
{
    qDebug() << "compute Angle Between Vectors button clicked";

    qDebug() << alModlerPtr->selectedAlienAndBP_ID;
    if (alModlerPtr->selectedAlienAndBP_ID.begin().value().first != -1
            &&
        alModlerPtr->selectedAlienAndBP_ID.begin().value().second != -1)
    {
        // Get the Alien and BP that was selected by the user. It is always updated in the AlienModeller
        int key = alModlerPtr->selectedAlienAndBP_ID.begin().key();
        GLWidget *wgt = glWidgetID_Map.value(key);
        Alien *alienPtr = alModlerPtr->getAlienFromID(alModlerPtr->selectedAlienAndBP_ID.value(key).first);
        BodyPart *bpPtr = alModlerPtr->getBodyPartFromID(alienPtr, alModlerPtr->selectedAlienAndBP_ID.value(key).second);

        QVector<Bone *> tempBoneList; tempBoneList.clear();
        QVector<int> tempBoneIDsList; tempBoneIDsList.clear();
        if (alModlerPtr->getAllBP_BonesInOrder(alienPtr, bpPtr, tempBoneList))
        {
            for (Bone *bPtr : tempBoneList)
                tempBoneIDsList << bPtr->BP_ID;

            qDebug() << alModlerPtr->getAnglesBetweenAdjacentVectors(alModlerPtr->getNormalVectorsOfAdjacentBonesForGivenFace(
                                                                                 alienPtr, tempBoneIDsList, Box::BoxFace::BOTTOM));
        }
        else
            qDebug() << "something's wrong";

        wgt->update();
    }
    else
        qDebug() << "BP not selected";
    qDebug() << "reached end";
}

void MainWindow1::handlePushButton_56__showAngleTrendsForJntsOfBP()
{
    qDebug() << "showAngleTrendsForJntsOfBP button clicked";

    qDebug() << alModlerPtr->selectedAlienAndBP_ID;
    if (alModlerPtr->selectedAlienAndBP_ID.begin().value().first != -1
            &&
        alModlerPtr->selectedAlienAndBP_ID.begin().value().second != -1)
    {
        // Get the Alien and BP that was selected by the user. It is always updated in the AlienModeller
        int key = alModlerPtr->selectedAlienAndBP_ID.begin().key();
        GLWidget *wgt = glWidgetID_Map.value(key);
        Alien *alienPtr = alModlerPtr->getAlienFromID(alModlerPtr->selectedAlienAndBP_ID.value(key).first);
        BodyPart *bpPtr = alModlerPtr->getBodyPartFromID(alienPtr, alModlerPtr->selectedAlienAndBP_ID.value(key).second);

        qDebug() << alModlerPtr->getAngleTrendsOfJntsOfBP(alienPtr, bpPtr);

        wgt->update();
    }
    else
        qDebug() << "BP not selected";
    qDebug() << "reached end";
}

void MainWindow1::handlePushButton_69__genClonedBPsAndAttachNearby()
{
    qDebug() << "genClonedBPsAndAttachNearby button clicked";

    if (!alModlerPtr->selectedAlienAndBP_ID.isEmpty())
    {
        if (alModlerPtr->selectedAlienAndBP_ID.begin().value().first != -1
            &&
            alModlerPtr->selectedAlienAndBP_ID.begin().value().second != -1)
        {
            // Get the Alien and BP that was selected by the user. It is always updated in the AlienModeller
            int key = alModlerPtr->selectedAlienAndBP_ID.begin().key();
            GLWidget *wgt = glWidgetID_Map.value(key);
            Alien *alienPtr = alModlerPtr->getAlienFromID(alModlerPtr->selectedAlienAndBP_ID.value(key).first);
            BodyPart *bpPtr = alModlerPtr->getBodyPartFromID(alienPtr, alModlerPtr->selectedAlienAndBP_ID.value(key).second);

            if (wgt && alienPtr && bpPtr)
            {
                QVector<BodyPart *> allRenderableBPsList; allRenderableBPsList.clear();
                QVector<int> allRenderableBPsIDsList; allRenderableBPsIDsList.clear();
                alModlerPtr->getAllRenderableBPs(alienPtr, allRenderableBPsList, key);
                for (BodyPart *bp : allRenderableBPsList)
                {
                    if (!allRenderableBPsIDsList.contains(bp->BP_ID))
                        allRenderableBPsIDsList << bp->BP_ID;
                }

                int numOfClones = ui->spinBox_19->value();

                for (int i=0; i<numOfClones; i++)
                {
                    QPair<int,int> bpAndMirrIDs = alModlerPtr->generateAndAttachClonedBPNearby(alienPtr, bpPtr->BP_ID, numOfClones);

                    // Remove IDs of those BPs that are no more in the Alien after the modification of JAT. This is just for sanity check
                    for (int i=0; i<allRenderableBPsIDsList.length(); i++)
                        if (!alModlerPtr->getBodyPartFromID(alienPtr, allRenderableBPsIDsList[i]))
                            allRenderableBPsIDsList.remove(i--);

                    if (bpAndMirrIDs == QPair<int,int>(-1,-1))
                        break;

                    if (bpAndMirrIDs.first != -1) // the BP's ID
                    {
                        if (!allRenderableBPsIDsList.contains(bpAndMirrIDs.first))
                            allRenderableBPsIDsList.append(bpAndMirrIDs.first);
                    }
                    if (bpAndMirrIDs.second != -1) // The BP's mirror ID
                    {
                        if (!allRenderableBPsIDsList.contains(bpAndMirrIDs.second))
                            allRenderableBPsIDsList.append(bpAndMirrIDs.second);
                    }
                    // Also include any sub BPs that were attached to the BP after cloning, such as Extremity BP
                    QVector<int> bpAndMirrID_List; bpAndMirrID_List.clear();
                    bpAndMirrID_List << bpAndMirrIDs.first << bpAndMirrIDs.second;
                    for (int mutatedBP_ID : bpAndMirrID_List)
                    {
                        bpPtr = alModlerPtr->getBodyPartFromID(alienPtr, mutatedBP_ID);
                        if (bpPtr)
                        {
                            for (int subExtID : bpPtr->subExtremityBPsIDs)
                            {
                                ExtremityBP *subExtPtr = alModlerPtr->getExtremityBP_FromID(alienPtr, subExtID);
                                if (subExtPtr)
                                {
                                    for (BodyPart *carpal : subExtPtr->extremBP_basePtr->extremBP_BaseCarpals)
                                        allRenderableBPsIDsList << carpal->BP_ID;
                                    for (BodyPart *digit : subExtPtr->extremBP_Digits)
                                        allRenderableBPsIDsList << digit->BP_ID;
                                }
                            }
                        }
                    }
                }

                QVector<int> BP_ListToDisplayInDiffColor; BP_ListToDisplayInDiffColor.clear();
                BP_ListToDisplayInDiffColor << bpPtr->BP_ID;
                // update the GLW_to_Alien_to_BP_to_ColorDisplay_Map
                this->alModlerPtr->updateGLW_to_Alien_to_BP_to_ColorDisplay_Map(alienPtr,
                                                                                wgt->glWidgetID, // get the GLW_ID
                                                                                allRenderableBPsIDsList, BP_ListToDisplayInDiffColor,
                                                                                AlienModeller::ColorEnum::RED);

                wgt->update();
            }
            alModlerPtr->selectedAlienAndBP_ID[key].second = bpPtr->BP_ID;
        }
    }
    else
        qDebug() << "BP not selected";
    qDebug() << "reached end";
}

void MainWindow1::handlePushButton_84__PruneBP()
{
    qDebug() << "PruneBP button clicked";

    qDebug() << alModlerPtr->selectedAlienAndBP_ID;
    if (alModlerPtr->selectedAlienAndBP_ID.begin().value().first != -1
            &&
        alModlerPtr->selectedAlienAndBP_ID.begin().value().second != -1)
    {
        // Get the Alien and BP that was selected by the user. It is always updated in the AlienModeller
        int key = alModlerPtr->selectedAlienAndBP_ID.begin().key();
        GLWidget *wgt = glWidgetID_Map.value(key);
        Alien *alienPtr = alModlerPtr->getAlienFromID(alModlerPtr->selectedAlienAndBP_ID.value(key).first);
        BodyPart *bpPtr = alModlerPtr->getBodyPartFromID(alienPtr, alModlerPtr->selectedAlienAndBP_ID.value(key).second);
        QTabWidget *tempTabWgtPtr = tabWidgetID_Map.value(tabWgtToTabMap.key(getTabContainingGLWidget(key)));
        QWidget *tempTabPtr = tabID_Map.value(getTabContainingGLWidget(key));

        // get a list of bones selected by the user
        BodyPart *prunedBP = nullptr;
        QVector<Bone*> bonesPtrList; bonesPtrList.clear();
        if (alModlerPtr->getAllBP_BonesInOrder(alienPtr, bpPtr, bonesPtrList))
        {
            int trimNum = static_cast<int>((0.2f * bonesPtrList.length()));
            while (trimNum > 0)
            {
                bonesPtrList.removeFirst();
                trimNum--;
            }
            QVector<int> bonesIDsList; bonesIDsList.clear();

            for (Bone *b : bonesPtrList)
                bonesIDsList << b->BP_ID;

            prunedBP = alModlerPtr->pruneBP(alienPtr, bpPtr, bonesIDsList);
        }

//        // Now display it in new tab with only one GLW
//        QVector<Alien*> tempAlienList; tempAlienList.clear();
//        tempAlienList.append(alienPtr);
//        tempTabPtr = createNewTabAndPopulateWithAliens(tempAlienList, tempTabWgtPtr, "Pruned BP", 1, 1, false);

//        QVector<int> fullGLW; QVector<int> emptyGLW;
//        fullGLW.clear(); emptyGLW.clear();
//        getFullAndEmptyGLWsInTab(tempTabPtr, fullGLW, emptyGLW);

//        for (int i=0; i<fullGLW.length(); i++)
//        {
//            QVector<int> BPsListToDisplay, BPsToDisplayInDiffClr;
//            BPsListToDisplay.clear(); BPsToDisplayInDiffClr.clear();
//            BPsListToDisplay << prunedBP->BP_ID;
//            alienModellerObject->updateGLW_to_Alien_to_BP_to_ColorDisplay_Map(alienPtr, fullGLW[i], BPsListToDisplay, BPsToDisplayInDiffClr, -1);

//            glWidgetID_Map.value(fullGLW[i])->update();
//        }
//        for (int i=0; i<emptyGLW.length(); i++)
//        {
//            QVector<int> BPsListToDisplay, BPsToDisplayInDiffClr;
//            BPsListToDisplay.clear(); BPsToDisplayInDiffClr.clear();
//            BPsListToDisplay << prunedBP->BP_ID;
//            alienModellerObject->updateGLW_to_Alien_to_BP_to_ColorDisplay_Map(alienPtr, emptyGLW[i], BPsListToDisplay, BPsToDisplayInDiffClr, -1);

//            assignAlienToGLWidget(glWidgetID_Map.value(emptyGLW[i]), alienPtr);

//            glWidgetID_Map.value(emptyGLW[i])->update();
//        }


        GeometryAlgorithms::translateBP(prunedBP, 300, 0, 0);

        // this code deals with which BPs to display and in what color
        QVector<int> BP_ListToDisplay;  BP_ListToDisplay.clear();
        QVector<int> sepClnBP_ListToDisplay;  sepClnBP_ListToDisplay.clear();
        QVector<int> BP_ListToDisplayInDiffColor; BP_ListToDisplayInDiffColor.clear();
        this->alModlerPtr->getAllBP_ID_List(alienPtr, BP_ListToDisplay, sepClnBP_ListToDisplay);
        // update the GLW_to_Alien_to_BP_to_ColorDisplay_Map
        this->alModlerPtr->updateGLW_to_Alien_to_BP_to_ColorDisplay_Map(alienPtr,
                                                                                wgt->glWidgetID, // get the GLW_ID
                                                                                BP_ListToDisplay, BP_ListToDisplayInDiffColor, -1);

        wgt->update();
    }
    else
        qDebug() << "BP not selected";
    qDebug() << "reached end";
}

void MainWindow1::handlePushButton_16__modelHeads()
{
    // ** ALGO **
    //  First create a new Alien
    //  Create a new Head of this Alien
    //  Display it in a new tab that has 1 frame and 2 GLW
    //  Only make the Head visible in the GLWs

    qDebug() << "model Heads button clicked";

    QMap<int, QPair<int,int>> tempBP_AmntMap;
    QVector<Alien*> tempAliensList; tempAliensList.clear();
//    QVector<int> tempAliensListToDel; tempAliensListToDel.clear();

    // Get the type of Alien from the combobox
    QString tempAlienTypeStr = ui->comboBox_5->currentText();

    // Get the types of BPs and their respective min and max amounts, from the spin controls
    tempBP_AmntMap.insert(AlienModeller::BodyPartsEnum::BACK_BONE,
                          QPair<int,int>(ui->spinBox->value(), ui->spinBox_6->value()));
    tempBP_AmntMap.insert(AlienModeller::BodyPartsEnum::LOWER_LIMB,
                          QPair<int,int>(ui->spinBox_2->value(), ui->spinBox_7->value()));
    tempBP_AmntMap.insert(AlienModeller::BodyPartsEnum::UPPER_LIMB,
                          QPair<int,int>(ui->spinBox_3->value(), ui->spinBox_8->value()));
//    tempBP_AmntMap.insert(AlienModeller::BodyPartsEnum::HEAD,
//                          QPair<int,int>(ui->spinBox_4->value(), ui->spinBox_9->value()));

    // modify the default values of the ruleset with the user specified values
    alModlerPtr->skeletonRuleSet->modifyNumOfSkeletalBodyParts_RS(alModlerPtr->alienTypeMap.value(tempAlienTypeStr),
                                                                          tempBP_AmntMap);

    tempBP_AmntMap.clear();

    for (int i=0; i<8; i++)
    {
        alModlerPtr->modelNewAlien_TypeGuided("Bipedal", tempBP_AmntMap);
        // create a new Head
        BodyPart *headPtr = alModlerPtr->modelNewHead_UsingStateMachine(alModlerPtr->alienList.last());
        // BodyPart *headPtr = alModlerPtr->modelNewHead(tempAliensList.last());
        tempAliensList.append(alModlerPtr->alienList.last());
    }

    QWidget* tabPtr = createNewTabAndPopulateWithAliens(tempAliensList, ui->tabWidget_2, "Aliens", 2, 4);

    QVector<int> fullGLW; QVector<int> emptyGLW;
    fullGLW.clear(); emptyGLW.clear();
    getFullAndEmptyGLWsInTab(tabPtr, fullGLW, emptyGLW);

    qDebug() << "reached 1";

    // The reason we choose fullGLW is because the the fullGLW will
    // ... have an alien living in it.
    for (int i=0; i<fullGLW.length() && i<tempAliensList.length(); i++)
    {
        // Only display the Head and its Sub BPs in the GLW
        QVector<int> BPsListToDisplay, BPsToDisplayInDiffClr;
        BPsListToDisplay.clear(); BPsToDisplayInDiffClr.clear();
        BPsListToDisplay << tempAliensList.at(i)->alienHeads.first()->BP_ID;
        alModlerPtr->updateGLW_to_Alien_to_BP_to_ColorDisplay_Map(tempAliensList.at(i), fullGLW[i], BPsListToDisplay, BPsToDisplayInDiffClr, -1);
        glWidgetID_Map.value(fullGLW[i])->update();
    }

    qDebug() << "reached 2";

    qDebug() << "reached end";
}

void MainWindow1::handlePushButton_46__genAndAttachNewHeadOnAlien()
{
    qDebug() << "genAndAttachNewHeadOnAlien button clicked";

    // Get the aliens that were double-clicked, clone them and display these in their seperate work areas
    // TODO(ismail): Only the most recently selected alien should be cloned, and not all
    // ... Revert the other alien's colors back to their original colors.
    if (!alModlerPtr->selectedAliensInGLWidgets.isEmpty())
    {
        QPair<int,int> p = this->alModlerPtr->selectedAliensInGLWidgets.first();
        Alien *aPtr = this->alModlerPtr->getAlienFromID(p.second);

        QVector<BodyPart *> allRenderableBPsList; allRenderableBPsList.clear();
        QVector<int> allRenderableBPsIDsList; allRenderableBPsIDsList.clear();
        alModlerPtr->getAllRenderableBPs(aPtr, allRenderableBPsList, p.first);
        for (BodyPart *bp : allRenderableBPsList)
            allRenderableBPsIDsList << bp->BP_ID;

        // First we need to delete any heads that were attached previously
        if (!aPtr->alienHeads.isEmpty())
        {
            QVector<int> headsToRemove; headsToRemove.clear();
            headsToRemove << aPtr->alienHeads.first()->BP_ID;
            alModlerPtr->removeAlienBodyParts(aPtr, headsToRemove);
        }

        // create a new Head
        BodyPart *headPtr = alModlerPtr->modelNewHead_UsingStateMachine(aPtr);
        if (headPtr)
        {
            // Attach the head to the Alien's BB
            alModlerPtr->attachHeadToParentBP(aPtr, headPtr, aPtr->alienBackBones.first());

            // Now render these new BPs in addition to the ones already displayed in this GLW
            QVector<int> BP_ListToDisplayInDiffColor; BP_ListToDisplayInDiffColor.clear();
            allRenderableBPsIDsList.append(headPtr->BP_ID);
            BP_ListToDisplayInDiffColor.append(headPtr->BP_ID);

            // update the GLW_to_Alien_to_BP_to_ColorDisplay_Map
            this->alModlerPtr->updateGLW_to_Alien_to_BP_to_ColorDisplay_Map(aPtr,
                                                                            p.first, // get the GLW_ID
                                                                            allRenderableBPsIDsList, BP_ListToDisplayInDiffColor,
                                                                            AlienModeller::ColorEnum::OLIVE);

            glWidgetID_Map.value(p.first)->update();
        }

    }
    else
        qDebug() << "Alien not selected - please first double click an alien";


}

void MainWindow1::handlePushButton_13__reattachSelectedBPsToDiffLocations()
{
    qDebug() << "Rearrange BPs Randomly button clicked";

    if (!ui->comboBox_18->currentText().isEmpty())
    {
        int BP_Type = alModlerPtr->BP_TypeMap.value(ui->comboBox_18->currentText(),-1);
        QVector<int> bpIDsToSelect; bpIDsToSelect.clear();
        if (BP_Type != -1)
        {
            if (!alModlerPtr->selectedAliensInGLWidgets.isEmpty())
            {
                QPair<int,int> p = this->alModlerPtr->selectedAliensInGLWidgets.first();
                Alien *aPtr = this->alModlerPtr->getAlienFromID(p.second);

                QVector<BodyPart *> allRenderableBPsList; allRenderableBPsList.clear();
                alModlerPtr->getAllRenderableBPs(aPtr, allRenderableBPsList, p.first);

                for (BodyPart *bp : allRenderableBPsList)
                    if (bp->BP_Type == BP_Type)
                    {
                        alModlerPtr->selectedAlienAndBP_ID.insert(p.first, QPair<int,int>(aPtr->alienID, bp->BP_ID));
                        break;
                    }

                alModlerPtr->selectedAliensInGLWidgets.clear();
            }
        }

    }

    if (!alModlerPtr->selectedAlienAndBP_ID.isEmpty())
    {
        if (alModlerPtr->selectedAlienAndBP_ID.begin().value().first != -1
            &&
            alModlerPtr->selectedAlienAndBP_ID.begin().value().second != -1)
        {
            // Get the Alien and BP that was selected by the user. It is always updated in the AlienModeller
            int key = alModlerPtr->selectedAlienAndBP_ID.begin().key();
            GLWidget *wgt = glWidgetID_Map.value(key);
            Alien *alienPtr = alModlerPtr->getAlienFromID(alModlerPtr->selectedAlienAndBP_ID.value(key).first);
            BodyPart *bpPtr = alModlerPtr->getBodyPartFromID(alienPtr, alModlerPtr->selectedAlienAndBP_ID.value(key).second);

            // Get its parent's ID
            int parentID = -1;
            if (alModlerPtr->getParentOfThisBP(alienPtr, bpPtr))
                parentID = alModlerPtr->getParentOfThisBP(alienPtr, bpPtr)->BP_ID;

            // If the selected BP doensn't have a parent, then we don't proceed
            if (parentID != -1)
            {
                // Find all the BPs that are of the same type as bpPtr
                QVector<int> BPsToReattach; BPsToReattach.clear();
                for (BodyPart *bp : alienPtr->bodyParts)
                    if (bp->BP_Type == bpPtr->BP_Type  &&  !bp->isSymmetric)
                        BPsToReattach.append(bp->BP_ID);

                // Now rearrange/reattach these
                alModlerPtr->reattachChildBPsToParentBP_DiffLocations(alienPtr, BPsToReattach, parentID);

                // this code deals with which BPs to display and in what color
                QVector<int> BP_ListToDisplay;  BP_ListToDisplay.clear();
                QVector<int> sepClnBP_ListToDisplay;  sepClnBP_ListToDisplay.clear();
                QVector<int> BP_ListToDisplayInDiffColor; BP_ListToDisplayInDiffColor.clear();
                this->alModlerPtr->getAllBP_ID_List(alienPtr, BP_ListToDisplay, sepClnBP_ListToDisplay);
                // update the GLW_to_Alien_to_BP_to_ColorDisplay_Map
                this->alModlerPtr->updateGLW_to_Alien_to_BP_to_ColorDisplay_Map(alienPtr,
                                                                                wgt->glWidgetID, // get the GLW_ID
                                                                                BP_ListToDisplay, BP_ListToDisplayInDiffColor, -1);

                wgt->update();
            }
            else
            {
                qDebug() << "This BP doesn't have a parent BP so exiting the program";
                // exit(0);
            }
        }
    }
    else
        qDebug() << "BP not selected";
    qDebug() << "reached end";
}

void MainWindow1::handlePushButton_81__BulgifyBP()
{
    qDebug() << "Bulgify BP button clicked";

    if (!alModlerPtr->selectedAlienAndBP_ID.isEmpty())
    {
        if (alModlerPtr->selectedAlienAndBP_ID.begin().value() != QPair<int,int>(-1,-1))
        {
            // Get the Alien and BP that was selected by the user. It is always updated in the AlienModeller
            int key = alModlerPtr->selectedAlienAndBP_ID.begin().key();
            GLWidget *wgt = glWidgetID_Map.value(key);
            Alien *alienPtr = alModlerPtr->getAlienFromID(alModlerPtr->selectedAlienAndBP_ID.value(key).first);
            BodyPart *bpPtr = alModlerPtr->getBodyPartFromID(alienPtr, alModlerPtr->selectedAlienAndBP_ID.value(key).second);

            if (bpPtr)
            {
                if (bpPtr->isSymmetric)
                    bpPtr = alModlerPtr->getMirrorOrOriginalBP(alienPtr, bpPtr);

                // Get parameters for subdivision from the UI controls
                // get the value for 'whether to mutate bulginess' from checkbox
                bool changeBulginess = ui->checkBox_6->isChecked();
                // First build the bulginess DNA for this BP
                alModlerPtr->buildRebuildUpdate_BP_BulgificationDNA(alienPtr, bpPtr->BP_ID, changeBulginess);
                alModlerPtr->bulgifyBodyPart(alienPtr, bpPtr);

                wgt->update();
            }
        }
        else
            qDebug() << "BP not selected";
        qDebug() << "reached end";
    }
}

void MainWindow1::handlePushButton_89__SubdiveBP_BonesIntoSubdivCells()
{
    qDebug() << "BPs Bones Subdivision button clicked";

    qDebug() << alModlerPtr->selectedAlienAndBP_ID;
    if (alModlerPtr->selectedAlienAndBP_ID.begin().value().first != -1
            &&
        alModlerPtr->selectedAlienAndBP_ID.begin().value().second != -1)
    {
        // Get the Alien and BP that was selected by the user. It is always updated in the AlienModeller
        int key = alModlerPtr->selectedAlienAndBP_ID.begin().key();
        GLWidget *wgt = glWidgetID_Map.value(key);
        Alien *alienPtr = alModlerPtr->getAlienFromID(alModlerPtr->selectedAlienAndBP_ID.value(key).first);
        BodyPart *bpPtr = alModlerPtr->getBodyPartFromID(alienPtr, alModlerPtr->selectedAlienAndBP_ID.value(key).second);

        // Get parameters for subdivision
        QVector<Bone*> bonesInOrder; bonesInOrder.clear();
        alModlerPtr->getAllBP_BonesInOrder(alienPtr, bpPtr, bonesInOrder);
        // First build up its subdivision DNA
        alModlerPtr->buildBoneSubDivCellsDNA_forGivenBP(alienPtr, bpPtr, bonesInOrder);
        // Do the subdivisions
        alModlerPtr->subDivideBP_BonesIntoSubdivCells(alienPtr, bpPtr);

        wgt->update();
    }
    else
        qDebug() << "BP not selected";
    qDebug() << "reached end";
}

void MainWindow1::handlePushButton_JAT_RS_SaveValues()
{
    // Get the type of BP, Energy and Angle Trends from the comboboxes
    QString tempBP_TypeStr = ui->comboBox_JAT_RS_SelectBP->currentText();
    QString tempEngyTrndStr = ui->comboBox_JAT_RS_SelectEneryTrend->currentText();
    QString tempAngleTrndStr = ui->comboBox_JAT_RS_SelectAngleTrend->currentText();

    int minEnergy = ui->spinBox_JAT_RS_MinEngyRange->value();
    int maxEnergy = ui->spinBox_JAT_RS_MaxEngyRange->value();
    int minAngle = ui->spinBox_JAT_RS_MinAngleRange->value();
    int maxAngle = ui->spinBox_JAT_RS_MaxAngleRange->value();

    QPolygon tempValuePoly; tempValuePoly.clear();
    tempValuePoly << QPoint(alModlerPtr->skeletonRuleSet->JATsTrendsMap.value(tempEngyTrndStr,-1),
                            alModlerPtr->skeletonRuleSet->JATsTrendsMap.value(tempAngleTrndStr,-1))
                  << QPoint(minEnergy, maxEnergy)
                  << QPoint(minAngle, maxAngle);

    // modify the default values of the ruleset with the user specified values
    alModlerPtr->skeletonRuleSet->modifyJointsArrangementTrends_RS(alModlerPtr->BP_TypeMap.value(tempBP_TypeStr),
                                                                          tempValuePoly);
}

void MainWindow1::handlePushButton_JAT_RS_Default_Values()
{
    alModlerPtr->skeletonRuleSet->initJointsArrangementTrends_RS();
}

void MainWindow1::updateSpinBoxesMinAndMax_AlienTab()
{
    qDebug() << "Current text changed on comboBox_5";
    // update the min and max values in the spin boxes
    if (ui->comboBox_5->currentText() == "Bipedal")
    {
        // Number of BPs
        ui->spinBox->setRange(1,1); ui->spinBox_6->setRange(2,2); // Backbone
        ui->spinBox_2->setRange(2,2); ui->spinBox_7->setRange(3,5); // LowerLimb
        ui->spinBox_3->setRange(2,2); ui->spinBox_8->setRange(3,6); // UpperLimb

        // Number of Joints
        ui->spinBox_14->setRange(5,6); ui->spinBox_17->setRange(10,20); // Backbone
        ui->spinBox_11->setRange(3,5); ui->spinBox_16->setRange(5,10); // LowerLimb
        ui->spinBox_15->setRange(3,5); ui->spinBox_12->setRange(5,10); // UpperLimb

    }
    else if (ui->comboBox_5->currentText() == "Aquatic")
    {
        // Number of BPs
        ui->spinBox->setRange(1,1); ui->spinBox_6->setRange(2,2); // Backbone
        ui->spinBox_2->setRange(2,3); ui->spinBox_7->setRange(4,8); // LowerLimb
        ui->spinBox_3->setRange(0,0); ui->spinBox_8->setRange(1,3); // UpperLimb

        // Number of Joints
        ui->spinBox_14->setRange(7,8); ui->spinBox_17->setRange(10,20); // Backbone
        ui->spinBox_11->setRange(3,5); ui->spinBox_16->setRange(6,10); // LowerLimb
        ui->spinBox_15->setRange(3,5); ui->spinBox_12->setRange(6,10); // UpperLimb
    }
    else if (ui->comboBox_5->currentText() == "Anthropodal")
    {
        // Number of BPs
        ui->spinBox->setRange(1,1); ui->spinBox_6->setRange(2,2); // Backbone
        ui->spinBox_2->setRange(2,6); ui->spinBox_7->setRange(7,10); // LowerLimb
        ui->spinBox_3->setRange(8,10); ui->spinBox_8->setRange(11,14); // UpperLimb

        // Number of Joints
        ui->spinBox_14->setRange(10,15); ui->spinBox_17->setRange(16,21); // Backbone
        ui->spinBox_11->setRange(3,5); ui->spinBox_16->setRange(6,7); // LowerLimb
        ui->spinBox_15->setRange(3,5); ui->spinBox_12->setRange(6,7); // UpperLimb
    }

    // Choosing these values will have the effect of changing the ruleset
    // Whenever the user pushes the Generate Aliens button, the ruleset will be updated with these values will
}

void MainWindow1::setActivationOfExtremGroupMutationControls()
{
    qDebug() << "Current text changed on comboBox_10";

    if (ui->comboBox_10->currentText() == "JAT"  ||  ui->comboBox_10->currentText() == "Bulkiness")
    {
        // Number of BPs
        ui->horizontalSlider_11->setEnabled(false); ui->horizontalSlider_12->setEnabled(false);
        ui->checkBox_4->setEnabled(false);
        ui->pushButton_43->setEnabled(true);
    }
    else if (ui->comboBox_10->currentText() == "Squash/Stretch")
    {
        ui->horizontalSlider_11->setEnabled(true); ui->horizontalSlider_12->setEnabled(false);
        ui->checkBox_4->setEnabled(true);
        ui->pushButton_43->setEnabled(false);
    }
    else if (ui->comboBox_10->currentText() == "No. of Joints")
    {
        ui->horizontalSlider_11->setEnabled(false); ui->horizontalSlider_12->setEnabled(true);
        ui->checkBox_4->setEnabled(false);
        ui->pushButton_43->setEnabled(false);
    }
}

void MainWindow1::selectAllBPsOfSelectedType_FromComboBox18()
{
    qDebug() << "Current text changed on comboBox_18";

    // first make sure the user has double clicked the alien
    if (!alModlerPtr->selectedAliensInGLWidgets.isEmpty())
    {
        QPair<int,int> p = this->alModlerPtr->selectedAliensInGLWidgets.first();
        Alien *aPtr = this->alModlerPtr->getAlienFromID(p.second);

        int BP_Type = alModlerPtr->BP_TypeMap.value(ui->comboBox_18->currentText(),-1);
        QVector<int> bpIDsToSelect; bpIDsToSelect.clear();
        if (BP_Type != -1)
        {
            QVector<BodyPart *> allRenderableBPsList; allRenderableBPsList.clear();
            QVector<int> allRenderableBPsIDsList; allRenderableBPsIDsList.clear();
            alModlerPtr->getAllRenderableBPs(aPtr, allRenderableBPsList, p.first);
            for (BodyPart *bp : allRenderableBPsList)
            {
                if (!allRenderableBPsIDsList.contains(bp->BP_ID))
                    allRenderableBPsIDsList << bp->BP_ID;
            }

            QVector<BodyPart*> sameTypeBPs; sameTypeBPs.clear();
            // make all these same type BPs 'red'
            alModlerPtr->getAllBPsOfGivenType(aPtr, BP_Type, sameTypeBPs, true);
            QVector<int> allBPsIDsToColor; allBPsIDsToColor.clear();
            for (BodyPart *bp : sameTypeBPs)
            {
                if (allRenderableBPsList.contains(bp))
                    if (!allBPsIDsToColor.contains(bp->BP_ID))
                        allBPsIDsToColor << bp->BP_ID;
            }
            // update the GLW_to_Alien_to_BP_to_ColorDisplay_Map
            this->alModlerPtr->updateGLW_to_Alien_to_BP_to_ColorDisplay_Map(aPtr,
                                                                            p.first, // get the GLW_ID
                                                                            allRenderableBPsIDsList, allBPsIDsToColor,
                                                                            AlienModeller::ColorEnum::RED);

            this->glWidgetID_Map.value(p.first)->update();

            // alModlerPtr->selectedAlienAndBP_ID.insert(p.first, QPair<int,int>(aPtr->alienID, allBPsIDsToColor.first()));
        }

    }
}

void MainWindow1::mutateSelectedBP_JAT_EnergyOrAngleUsingHorSlider()
{
    qDebug() << "Mutate JAT Energy / Angle slider moved";

    if (!alModlerPtr->selectedAlienAndBP_ID.isEmpty())
    {
        if (alModlerPtr->selectedAlienAndBP_ID.begin().value().first != -1
            &&
            alModlerPtr->selectedAlienAndBP_ID.begin().value().second != -1)
        {
            // Get the Alien and BP that was selected by the user. It is always updated in the AlienModeller
            int key = alModlerPtr->selectedAlienAndBP_ID.begin().key();
            GLWidget *wgt = glWidgetID_Map.value(key);
            Alien *alienPtr = alModlerPtr->getAlienFromID(alModlerPtr->selectedAlienAndBP_ID.value(key).first);
            BodyPart *bpPtr = alModlerPtr->getBodyPartFromID(alienPtr, alModlerPtr->selectedAlienAndBP_ID.value(key).second);

            if (bpPtr->isSymmetric) // get its original BP
                bpPtr = alModlerPtr->getMirrorOrOriginalBP(alienPtr, bpPtr);

            ui->spinBox_JAT_Mut_StartEnergy->setValue(ui->horizontalSlider_13->value());
            ui->spinBox_JAT_Mut_StartAngle->setValue( ui->horizontalSlider_14->value());

            // Get the type of BP, Energy and Angle Trends from the comboboxes
            QPointF energyVals, angleVals;
            QString tempEngyTrndStr = ui->comboBox_JAT_Mut_SelectEneryTrend->currentText();
            QString tempAngleTrndStr = ui->comboBox_JAT_Mut_SelectAngleTrend->currentText();

            QPolygon tempValuePoly; tempValuePoly.clear();
            energyVals = QPointF(alModlerPtr->skeletonRuleSet->JATsTrendsMap.value(tempEngyTrndStr,-1),
                                 ui->spinBox_JAT_Mut_StartEnergy->value());
            angleVals = QPoint(alModlerPtr->skeletonRuleSet->JATsTrendsMap.value(tempAngleTrndStr,-1),
                               ui->spinBox_JAT_Mut_StartAngle->value());


            QVector<BodyPart *> allRenderableBPsList; allRenderableBPsList.clear();
            QVector<int> allRenderableBPsIDsList; allRenderableBPsIDsList.clear();
            alModlerPtr->getAllRenderableBPs(alienPtr, allRenderableBPsList, key);
            QVector<int> whoseMirrorsToRender; whoseMirrorsToRender.clear();
            // We need to extract the IDs of those BPs that are mirrors of the BPs in this list
            for (BodyPart *bp : allRenderableBPsList)
            {
                allRenderableBPsIDsList << bp->BP_ID;
                // If this BP has a mirror, note it down in the whoseMirrorsToRender so we can render its mirror after
                if (!bp->isSymmetric)
                {
                    BodyPart *mirrPtr0 = alModlerPtr->getMirrorOrOriginalBP(alienPtr, bp);
                    if (mirrPtr0)
                    {
                        if (allRenderableBPsList.contains(mirrPtr0))
                            whoseMirrorsToRender << bp->BP_ID;
                        if (bp != bpPtr)
                        {
                            if (!allRenderableBPsIDsList.contains(mirrPtr0->BP_ID))
                                allRenderableBPsIDsList << mirrPtr0->BP_ID;
                        }
                    }
                }
            }

            QVector<int> BP_ID_List; BP_ID_List.clear();
            BP_ID_List << bpPtr->BP_ID;
            QPair<int,int> ID_MirrPair = QPair<int,int>(-1,-1);
            energyVals.setY(energyVals.y()*10);
            alModlerPtr->modifyBP_JAT_rootJoint_DNA(alienPtr, BP_ID_List, ID_MirrPair,
                                                    QPair<bool,bool>(true,true), QPair<bool,bool>(true,true),
                                                    energyVals, angleVals);

            // Make sure that BP Ptr is valid and exists in the Alien after the BP was re-JATed
            // Cuz there might be a situation that the pointer goes NULL
            for (int i=0; i<allRenderableBPsIDsList.length(); i++)
                if (!alModlerPtr->getBodyPartFromID(alienPtr, allRenderableBPsIDsList[i]))
                    allRenderableBPsIDsList.remove(i--);

            for (int bpID : whoseMirrorsToRender)
            {
                BodyPart *currBP_Ptr = alModlerPtr->getBodyPartFromID(alienPtr, bpID);
                if (currBP_Ptr)
                {
                    if (!currBP_Ptr->isSymmetric)
                    {
                        alModlerPtr->getAllCarpalsAndDigitsIDsOfBP(alienPtr, bpID, allRenderableBPsIDsList);
                        BodyPart *currBP_MirrPtr = alModlerPtr->getMirrorOrOriginalBP(alienPtr, currBP_Ptr);
                        if (currBP_MirrPtr)
                        {
                            if (!allRenderableBPsIDsList.contains(currBP_MirrPtr->BP_ID))
                            {
                                allRenderableBPsIDsList << currBP_MirrPtr->BP_ID;
                                alModlerPtr->getAllCarpalsAndDigitsIDsOfBP(alienPtr, currBP_MirrPtr->BP_ID, allRenderableBPsIDsList);
                            }
                        }
                    }
                }
            }

            qDebug() << ID_MirrPair;
            QVector<int> BP_ListToDisplayInDiffColor; BP_ListToDisplayInDiffColor.clear();
            BP_ListToDisplayInDiffColor << ID_MirrPair.first;
            alModlerPtr->selectedAlienAndBP_ID[key].second = ID_MirrPair.first;

            // update the GLW_to_Alien_to_BP_to_ColorDisplay_Map
            this->alModlerPtr->updateGLW_to_Alien_to_BP_to_ColorDisplay_Map(alienPtr,
                                                                            wgt->glWidgetID, // get the GLW_ID
                                                                            allRenderableBPsIDsList, BP_ListToDisplayInDiffColor,
                                                                            AlienModeller::ColorEnum::RED);

            wgt->update();
        }
        else
            qDebug() << "BP not selected";
        qDebug() << "reached end for Mutate JAT Energy / Angle slider moved";
    }
}

void MainWindow1::increaseDecreaseBulkinessUsingHorizontalSlider_15()
{
    qDebug() << "increase /Decrease BP bullkiness slider moved";

    if (!alModlerPtr->selectedAlienAndBP_ID.isEmpty())
    {
        // Get the Alien and BP that was selected by the user. It is always updated in the AlienModeller
        int key = alModlerPtr->selectedAlienAndBP_ID.begin().key();
        GLWidget *wgt = glWidgetID_Map.value(key);
        Alien *alienPtr = alModlerPtr->getAlienFromID(alModlerPtr->selectedAlienAndBP_ID.value(key).first);
        BodyPart *bpPtr = alModlerPtr->getBodyPartFromID(alienPtr, alModlerPtr->selectedAlienAndBP_ID.value(key).second);

        if (bpPtr)
        {
            QVector<BodyPart *> allRenderableBPsList; allRenderableBPsList.clear();
            QVector<int> allRenderableBPsIDsList; allRenderableBPsIDsList.clear();
            alModlerPtr->getAllRenderableBPs(alienPtr, allRenderableBPsList, key);
            QVector<int> whoseMirrorsToRender; whoseMirrorsToRender.clear();
            // We need to extract the IDs of those BPs that are mirrors of the BPs in this list
            for (BodyPart *bp : allRenderableBPsList)
            {
                allRenderableBPsIDsList << bp->BP_ID;
                // If this BP has a mirror, note it down in the whoseMirrorsToRender so we can render its mirror after
                if (!bp->isSymmetric)
                {
                    BodyPart *mirrPtr0 = alModlerPtr->getMirrorOrOriginalBP(alienPtr, bp);
                    if (mirrPtr0)
                    {
                        if (allRenderableBPsList.contains(mirrPtr0))
                            whoseMirrorsToRender << bp->BP_ID;
                        if (bp != bpPtr)
                        {
                            if (!allRenderableBPsIDsList.contains(mirrPtr0->BP_ID))
                                allRenderableBPsIDsList << mirrPtr0->BP_ID;
                        }
                    }
                }
            }


            QVector<int> BP_ID_List_2; BP_ID_List_2.clear();
            BP_ID_List_2 << bpPtr->BP_ID;
            BodyPart *mirrPtr0 = alModlerPtr->getMirrorOrOriginalBP(alienPtr, bpPtr);
            int res = ui->horizontalSlider_15->value() - prevHorizSlider15Value_IncDecBulkiness;
            // res = res > 0 ? 1 : 2;
            if (res > 0)
            {
                alModlerPtr->modifySkeletalBP_BulkinessDNA(alienPtr, BP_ID_List_2, 1);
                if (mirrPtr0)
                {
                    BP_ID_List_2.clear();
                    BP_ID_List_2 << mirrPtr0->BP_ID;
                    alModlerPtr->modifySkeletalBP_BulkinessDNA(alienPtr, BP_ID_List_2, 1);
                }
            }
            else if (res < 0)
            {
                alModlerPtr->modifySkeletalBP_BulkinessDNA(alienPtr, BP_ID_List_2, 2);
                if (mirrPtr0)
                {
                    BP_ID_List_2.clear();
                    BP_ID_List_2 << mirrPtr0->BP_ID;
                    alModlerPtr->modifySkeletalBP_BulkinessDNA(alienPtr, BP_ID_List_2, 2);
                }
            }

            prevHorizSlider15Value_IncDecBulkiness = ui->horizontalSlider_15->value();


            for (int i=0; i<allRenderableBPsIDsList.length(); i++)
                if (!alModlerPtr->getBodyPartFromID(alienPtr, allRenderableBPsIDsList[i]))
                    allRenderableBPsIDsList.remove(i--);

            for (int bpID : whoseMirrorsToRender)
            {
                BodyPart *currBP_Ptr = alModlerPtr->getBodyPartFromID(alienPtr, bpID);
                if (currBP_Ptr)
                {
                    if (!currBP_Ptr->isSymmetric)
                    {
                        BodyPart *currBP_MirrPtr = alModlerPtr->getMirrorOrOriginalBP(alienPtr, currBP_Ptr);
                        if (currBP_MirrPtr)
                        {
                            if (!allRenderableBPsIDsList.contains(currBP_MirrPtr->BP_ID))
                                allRenderableBPsIDsList << currBP_MirrPtr->BP_ID;
                        }
                    }
                }
            }

            QVector<int> BP_ListToDisplayInDiffColor; BP_ListToDisplayInDiffColor.clear();
            BP_ListToDisplayInDiffColor << bpPtr->BP_ID;
            // update the GLW_to_Alien_to_BP_to_ColorDisplay_Map
            this->alModlerPtr->updateGLW_to_Alien_to_BP_to_ColorDisplay_Map(alienPtr,
                                                                            wgt->glWidgetID, // get the GLW_ID
                                                                            allRenderableBPsIDsList, BP_ListToDisplayInDiffColor,
                                                                            AlienModeller::ColorEnum::RED);

            wgt->update();

            qDebug() << "reached end";
        }
    }
}

void MainWindow1::mutateStretchSquashExtremBPsCollectively()
{
    qDebug() << "Strech/Squash Extremity BPs Collectively slider moved";

    qDebug() << alModlerPtr->selectedAlienAndBP_ID;

    if (!alModlerPtr->selectedAlienAndBP_ID.isEmpty())
    {
        if (alModlerPtr->selectedAlienAndBP_ID.begin().value().first != -1
            &&
            alModlerPtr->selectedAlienAndBP_ID.begin().value().second != -1)
        {
            // Get the Alien and BP that was selected by the user. It is always updated in the AlienModeller
            int key = alModlerPtr->selectedAlienAndBP_ID.begin().key();
            GLWidget *wgt = glWidgetID_Map.value(key);
            Alien *alienPtr = alModlerPtr->getAlienFromID(alModlerPtr->selectedAlienAndBP_ID.value(key).first);
            BodyPart *bpPtr = alModlerPtr->getBodyPartFromID(alienPtr, alModlerPtr->selectedAlienAndBP_ID.value(key).second);

            // Find the extremity this BP belongs to
            if (bpPtr->isSymmetric)
                bpPtr = alModlerPtr->getMirrorOrOriginalBP(alienPtr, bpPtr);

            if (bpPtr)
            {
                int backupID = bpPtr->BP_ID;

                QVector<BodyPart *> allRenderableBPsList; allRenderableBPsList.clear();
                QVector<int> allRenderableBPsIDsList; allRenderableBPsIDsList.clear();
                alModlerPtr->getAllRenderableBPs(alienPtr, allRenderableBPsList, key);
                for (BodyPart *bp : allRenderableBPsList) // store the IDs of allRenderableBPsList
                {
                    if (!allRenderableBPsIDsList.contains(bp->BP_ID))
                        allRenderableBPsIDsList << bp->BP_ID;
                }
                if (bpPtr->BP_Type == AlienModeller::BodyPartsEnum::ExtremBP_Digit
                    ||
                    bpPtr->BP_Type == AlienModeller::BodyPartsEnum::ExtremBP_Carpal)
                {
                    QVector<int> bpIDsToMutate; bpIDsToMutate.clear();
                    for (ExtremityBP *extBP : alienPtr->alienExtremities)
                    {
                        if (extBP->extremBP_basePtr->extremBP_BaseCarpals.contains(bpPtr)
                            &&
                            allRenderableBPsList.contains(bpPtr)) // we have found the Ext BP whose carpal or digit has been selected by mouse
                        {
                            // add all the carpals of this extremity in bpIDsToMutate
                            for (BodyPart *carpal : extBP->extremBP_basePtr->extremBP_BaseCarpals)
                            {
                                bpIDsToMutate << carpal->BP_ID;
                            }
                            break;
                        }
                        else if (extBP->extremBP_Digits.contains(bpPtr)
                            &&
                            allRenderableBPsList.contains(bpPtr)) // we have found the Ext BP whose carpal or digit has been selected by mouse
                        {
                            for (BodyPart *digit : extBP->extremBP_Digits)
                            {
                                bpIDsToMutate << digit->BP_ID;
                            }
                            break;
                        }
                    }
                    for (int bpToMutate : bpIDsToMutate)
                    {
                        // BodyPart *bpPtr = alModlerPtr->getBodyPartFromID(alienPtr, bpToMutate);
                        int res = ui->horizontalSlider_11->value() - squashStretchSliderVal;
                        // at the moment we are using 0.0 for both squashing and stretching. As long as its not -1, it will perform squashing/stretching.
                        if (res < 0)
                        {
                            alModlerPtr->squashOrStrechBP(alienPtr, bpToMutate, 0.0, 0.95);
                        }
                        else if (res > 0)
                        {
                            alModlerPtr->squashOrStrechBP(alienPtr, bpToMutate, 0.0, 1.05);
                        }
                    }

                    QVector<int> BP_ListToDisplayInDiffColor; BP_ListToDisplayInDiffColor.clear();
                    BP_ListToDisplayInDiffColor << backupID;
                    this->alModlerPtr->updateGLW_to_Alien_to_BP_to_ColorDisplay_Map(alienPtr,
                                                                                    wgt->glWidgetID, // get the GLW_ID
                                                                                    allRenderableBPsIDsList, BP_ListToDisplayInDiffColor,
                                                                                    AlienModeller::ColorEnum::RED);

                    wgt->update();
                    squashStretchSliderVal = ui->horizontalSlider_11->value();
                    alModlerPtr->selectedAlienAndBP_ID[key].second = backupID;
                }
            }
        }
        else
            qDebug() << "BP not selected";
        qDebug() << "reached end";
    }
}

void MainWindow1::mutateNumOfJointsExtremBPsCollectively()
{
    qDebug() << "Mutate Num of Joints BPs Collectively slider moved";

    qDebug() << alModlerPtr->selectedAlienAndBP_ID;
    if (!alModlerPtr->selectedAlienAndBP_ID.isEmpty())
    {
        if (alModlerPtr->selectedAlienAndBP_ID.begin().value() != QPair<int,int>(-1,-1))
        {
            // Get the Alien and BP that was selected by the user. It is always updated in the AlienModeller
            int key = alModlerPtr->selectedAlienAndBP_ID.begin().key();
            GLWidget *wgt = glWidgetID_Map.value(key);
            Alien *alienPtr = alModlerPtr->getAlienFromID(alModlerPtr->selectedAlienAndBP_ID.value(key).first);
            BodyPart *bpPtr = alModlerPtr->getBodyPartFromID(alienPtr, alModlerPtr->selectedAlienAndBP_ID.value(key).second);
            int selectedBP_ID = bpPtr->BP_ID; // for backup

            QVector<BodyPart *> allRenderableBPsList; allRenderableBPsList.clear();
            QVector<int> allRenderableBPsIDsList; allRenderableBPsIDsList.clear();
            alModlerPtr->getAllRenderableBPs(alienPtr, allRenderableBPsList, key);
            QVector<int> sameTypeBPsIDsToBeMutated; sameTypeBPsIDsToBeMutated.clear();
            // Get a list of all the renderable BPs in this GLW
            // ... and extract all those BPs that are extremities of the selected BP type
            for (BodyPart *bp : allRenderableBPsList)
            {
                if (!allRenderableBPsIDsList.contains(bp->BP_ID))
                    allRenderableBPsIDsList << bp->BP_ID;

                // extract this BP if this is an extremity of the selected BP type
                if (bp->BP_Type == AlienModeller::BodyPartsEnum::ExtremBP_Digit
                    ||
                    bp->BP_Type == AlienModeller::BodyPartsEnum::ExtremBP_Carpal)
                {
                    if (bpPtr->BP_Type == bp->BP_Type)
                        sameTypeBPsIDsToBeMutated << bp->BP_ID;
                    // We also need to check that the BPs in sameTypeBPsIDsToBeMutated are parts of the
                    // ... same ExtremityBP as that of bpPtr
                }
            }

            if (!sameTypeBPsIDsToBeMutated.isEmpty())
            {
                // change Num of Joints for all these same type BPs
                int res = ui->horizontalSlider_12->value() - numOfJntsSliderVal;
                if (abs(res) % 3 == 0)
                {
                    for (int smBP_ID : sameTypeBPsIDsToBeMutated)
                    {
                        BodyPart *tempBP_Ptr = alModlerPtr->getBodyPartFromID(alienPtr, smBP_ID);
                        if (res<0)
                            tempBP_Ptr = alModlerPtr->incDecNumOfJointsForBP(alienPtr, tempBP_Ptr, -1, false);
                        else if (res>0)
                            tempBP_Ptr = alModlerPtr->incDecNumOfJointsForBP(alienPtr, tempBP_Ptr, 1, false);

                        // if (!allRenderableBPsIDsList.contains(tempBP_Ptr->BP_ID))
                        //     allRenderableBPsIDsList << tempBP_Ptr->BP_ID;
                    }
                    numOfJntsSliderVal = ui->horizontalSlider_12->value();
                }

                // Remove IDs of those BPs that are no more in the Alien after the modification of JAT (if any)
                for (int i=0; i<allRenderableBPsIDsList.length(); i++)
                    if (!alModlerPtr->getBodyPartFromID(alienPtr, allRenderableBPsIDsList[i]))
                        allRenderableBPsIDsList.remove(i--);

                QVector<int> BP_ListToDisplayInDiffColor; BP_ListToDisplayInDiffColor.clear();
                BP_ListToDisplayInDiffColor << selectedBP_ID;
                // update the GLW_to_Alien_to_BP_to_ColorDisplay_Map
                this->alModlerPtr->updateGLW_to_Alien_to_BP_to_ColorDisplay_Map(alienPtr,
                                                                                wgt->glWidgetID, // get the GLW_ID
                                                                                allRenderableBPsIDsList, BP_ListToDisplayInDiffColor,
                                                                                AlienModeller::ColorEnum::RED);

                wgt->update();
                alModlerPtr->selectedAlienAndBP_ID[key].second = selectedBP_ID;
            }
        }
    }
    else
        qDebug() << "BP not selected";
    qDebug() << "reached end";
}

void MainWindow1::handlePushButton_48__GenerateAliens()
{
    qDebug() << "Generate Aliens Button clicked";

    // This map will hold the min and max values of each BP type that was selected by the user
    // key(int) is the BP_Type_ID. QPair is (min num of BPs, max num of BPs)
    QMap<int, QPair<int,int>> tempBP_AmntMap; tempBP_AmntMap.clear();
    QMap<int, QPoint> tempJnts_AmntMap; tempJnts_AmntMap.clear();
    QVector<Alien*> tempAliensList; tempAliensList.clear();
//    QVector<int> tempAliensListToDel; tempAliensListToDel.clear();

    // Get the type of Alien from the combobox
    QString tempAlienTypeStr = ui->comboBox_5->currentText();

    // Get the number of aliens to make from the spin control
    int numOfAliens = ui->spinBox_5->value();

    // Get the types of BPs and their respective min and max amounts, from the spin controls
    tempBP_AmntMap.insert(AlienModeller::BodyPartsEnum::BACK_BONE,
                          QPair<int,int>(ui->spinBox->value(), ui->spinBox_6->value()));
    tempBP_AmntMap.insert(AlienModeller::BodyPartsEnum::LOWER_LIMB,
                          QPair<int,int>(ui->spinBox_2->value(), ui->spinBox_7->value()));
    tempBP_AmntMap.insert(AlienModeller::BodyPartsEnum::UPPER_LIMB,
                          QPair<int,int>(ui->spinBox_3->value(), ui->spinBox_8->value()));
    // We are not generating Head at this time
//    tempBP_AmntMap.insert(AlienModeller::BodyPartsEnum::HEAD,
//                          QPair<int,int>(ui->spinBox_4->value(), ui->spinBox_9->value()));

    // modify the default values of the ruleset with the user specified values
    alModlerPtr->skeletonRuleSet->modifyNumOfSkeletalBodyParts_RS(alModlerPtr->alienTypeMap.value(tempAlienTypeStr),
                                                                          tempBP_AmntMap);
    tempBP_AmntMap.clear();

    // Get the types of BPs and their respective min and max amounts, from the spin controls
    tempJnts_AmntMap.insert(AlienModeller::BodyPartsEnum::BACK_BONE,
                          QPoint(ui->spinBox_14->value(), ui->spinBox_17->value()));
    tempJnts_AmntMap.insert(AlienModeller::BodyPartsEnum::LOWER_LIMB,
                          QPoint(ui->spinBox_11->value(), ui->spinBox_16->value()));
    tempJnts_AmntMap.insert(AlienModeller::BodyPartsEnum::UPPER_LIMB,
                          QPoint(ui->spinBox_15->value(), ui->spinBox_12->value()));
    alModlerPtr->skeletonRuleSet->modifyMinMaxJntsForBodyParts_RS(alModlerPtr->alienTypeMap.value(tempAlienTypeStr),
                                                                          tempJnts_AmntMap);
    tempJnts_AmntMap.clear();
    // Now send this ruleset to the ModelAlien_TypeGuided() method with the values sent as parameters.
    // ... Inside this method, the AlienModeller will temporarily update the ruleset brackets (min and max) values
    // Now create N aliens to be displayed in the first tab
    for (int i=0; i<numOfAliens; i++)
    {
        alModlerPtr->modelNewAlien_TypeGuided(tempAlienTypeStr, tempBP_AmntMap);
        tempAliensList.append(alModlerPtr->alienList.last());
//        BodyPart *headPtr = alienModellerObject->modelNewHead(tempAliensList.last());
//        tempAliensListToDel.append(alienModellerObject->alienList.last()->alienID);
//        tempAliensList.append(alienModellerObject->cloneAlien_Algo2(alienModellerObject->alienList.last()));
//        qDebug() << "Number of Aliens created: " << alienModellerObject->alienList.length();
    }

    // Now symmetrify these aliens
    for (Alien *alPtr : tempAliensList)
    {
        alModlerPtr->symmetrifyAlienBodyParts(alPtr);
    }

    // Seed Aliens Tab - Create several widgets in the tab and display all the seed aliens in these
    int numOfFrames = ceil(numOfAliens/4.f);
    int numOfGLWs = ceil((numOfAliens*1.f)/numOfFrames);

    QWidget* tGLW_Ptr = createNewTabAndPopulateWithAliens(tempAliensList, ui->tabWidget_2, "Aliens", numOfFrames, numOfGLWs);

}

void MainWindow1::handlePushButton_4__SaveAlienToFile()
{
    qDebug() << "Save Alien button clicked";

    // Get the aliens that were double-clicked, clone them and display these in their seperate work areas
    // TODO(ismail): Only the most recently added alien should be cloned, and not all
    // ... Revert the other alien's colors back to their original colors.

    if (!alModlerPtr->selectedAliensInGLWidgets.isEmpty())
    {
        QPair<int,int> p = this->alModlerPtr->selectedAliensInGLWidgets.first();
        Alien *aPtr = this->alModlerPtr->getAlienFromID(p.second);
        if (aPtr)
        {
            if (alModlerPtr->saveAlienToFile(aPtr))
                qDebug() << "Alien Saved to file successfully";
            else
                exit(0);
        }
    }
    qDebug() << "reached end";
}

void MainWindow1::handlePushButton_9__LoadAlienFromFile()
{
    qDebug() << "Load Alien button clicked";

    Alien *alienPtr = alModlerPtr->loadAlienFromFile("/Users/ismailkazmi/Qt/Examples/Qt-6.6.1/opengl/idea1_QtProject/test_file.dat");
    if (alienPtr)
    {
        alienPtr->alienJustLoadedFlag=true;
        QVector<Alien*> tempAliensList; tempAliensList.clear();
        tempAliensList.append(alienPtr);

        // Now display this alien in a new GLW and new Tab
        // Seed Aliens Tab - Create several widgets in the tab and display all the seed aliens in these
        int numOfFrames = ceil(1/4.f);
        int numOfGLWs = ceil((1*1.f)/numOfFrames);

        QWidget* tGLW_Ptr = createNewTabAndPopulateWithAliens(tempAliensList, ui->tabWidget_2, "Loaded Aliens", numOfFrames, numOfGLWs);
    }
    else
    {
        qDebug() << "Alien could not be loaded - handlePushButton_9__LoadAlienFromFile method";
        exit(0);
    }

    qDebug() << "reached end";

}

MainWindow1::~MainWindow1()
{
    delete ui;
}

// void MainWindow1::handlePushButton_92__BulkUpSubdivCellsForGivenBP_Bones()
// {
//     qDebug() << "BPs Bones BulkUpSubdivCellsForGivenBP_Bones button clicked";

//     qDebug() << alModlerPtr->selectedAlienAndBP_ID;
//     if (alModlerPtr->selectedAlienAndBP_ID.begin().value().first != -1
//             &&
//         alModlerPtr->selectedAlienAndBP_ID.begin().value().second != -1)
//     {
//         // Get the Alien and BP that was selected by the user. It is always updated in the AlienModeller
//         int key = alModlerPtr->selectedAlienAndBP_ID.begin().key();
//         GLWidget *wgt = glWidgetID_Map.value(key);
//         Alien *alienPtr = alModlerPtr->getAlienFromID(alModlerPtr->selectedAlienAndBP_ID.value(key).first);
//         BodyPart *bpPtr = alModlerPtr->getBodyPartFromID(alienPtr, alModlerPtr->selectedAlienAndBP_ID.value(key).second);

//         // Get parameters for subdivision cells
//         QVector<Bone*> bonesInOrder; bonesInOrder.clear();
//         alModlerPtr->getAllBP_BonesInOrder(alienPtr, bpPtr, bonesInOrder);
//         alModlerPtr->bulkUpBP_BoneSubdivCells(alienPtr, bpPtr, bonesInOrder);

//         wgt->update();
//     }
//     else
//         qDebug() << "BP not selected";
//     qDebug() << "reached end";
// }
