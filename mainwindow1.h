#ifndef MAINWINDOW1_H
#define MAINWINDOW1_H

#include <QMainWindow>
#include <QSlider>
#include <glwidget.h>
#include <QPushButton>
#include <QVBoxLayout>
#include <QKeyEvent>
// #include <QDesktopWidget>
#include <QScreen>
#include <QLabel>
#include <alienmodeller.h>
//#include <inferenceengine.h>
#include <stdlib.h>     /* srand, rand */
#include <time.h> /* for taking the system time for random number generation */


namespace Ui {
class MainWindow1;
}

class MainWindow1 : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow1(QWidget *parent = 0);
    ~MainWindow1();

    // This method creates a new tab on the main window, then adds 5 frames to it stacked on top of each other, then adds glwidgets to it layed out horizontally on each frame
    // ... and then renders the incoming BP in the glWidgets
    void createNewTabAndPopulateWithBodyParts(Alien *alienPtr, QVector<BodyPart*> &BP_List);
    // TODO: Make it more flexible by inputting a list of BPs from the list of list of aliens to be displayed
    // ... in the GLW
    QWidget* createNewTabAndPopulateWithAliens(QVector<Alien*> &alienList, QTabWidget *tabWidgetPtr, const QString tabTitle, int numOfFrames, int numOfGLWgtsPerFrame, bool isThisAlienOfInterest=false);
    // This will create multiple frames add multiple GLWidgets to each of these frame that is created,
    // ... by calling createMultipleGLWidgetsInFrame()
    void createMultipleFramesInTab(QWidget *tabPtr, int numOfFrames, int numOfGLWgtsPerFrame);
    void createMultipleGLWidgetsInFrame(QFrame *framePtr, int numOfGLWidgets, QMap<int, QPolygon> &tempMap);
    void updateAll_GLWidgets();
    QWidget *createNewTabInTabWidget(QTabWidget *tabWidgetPtr, const QString tabLabel);
    QFrame *addNewFrameToTab(QWidget* tabPtr, const QString &frameName);
    GLWidget *addNewGLWidgetToFrame(QFrame *framePtr);
    void assignAlienModellerToGLWidget(GLWidget *glWgt);
    void assignAlienToGLWidget(GLWidget *glWgt, Alien* alienPtr); // Assign an alien to be rendered in this GLWidget
    void assignBodyPartToGLWidget(GLWidget *glWgt, BodyPart *bpPtr); // Assign a BP to be rendered in this GLWidget
    QTabWidget* createNewTabWidgetInsideTab(QWidget* tabPtr);
    void displayAlienOfInterestInNewTab(QVector<Alien*> alienList, QTabWidget *tabWidgetPtr, QString tabTitle);
    int getTabContainingGLWidget(int GLW_ID); // returns the ID of the tab housing this GLWidget
    // This method is used to find all those GLWs inside a given TAB that are occupied and unoccupied by Aliens, Aliens of Interest, or BPs
    void getFullAndEmptyGLWsInTab(QWidget* tabPtr, QVector<int> &full, QVector<int> &empty);
    int getTotalGLWsInTab(QWidget* tabPtr);
    int getAlienID_InGLW(int glwID);

    void setJAT_RS_ValuesFromGUI();

protected:
    void keyPressEvent(QKeyEvent *e) Q_DECL_OVERRIDE;
    void keyReleaseEvent(QKeyEvent *e) Q_DECL_OVERRIDE;

private slots:
    void dockUndock(); // dont need docking/undocking
    void handlePushButton_MutateBP_BulkinessDNA();
    void handlePushButton_27_UpdateBulkinessRS_WithGUI_SaveValues();
    void handlePushButton_17__ReverseBP_BulkinessTrend();
    void handlePushButton_22__mutateNumOfJointsPerBP_DNA();
    void handlePushButton_2__cloneAlien();
    void handlePushButton_25__ReplaceAliens();
    void handlePushButton_52__ReplaceSelectedAliens();
    void handlePushButton_70__RemoveSelectedAliens();
    void handlePushButton_29__modelExtremityBPs();
    void handlePushButton_33__attachSeveralExtremityBPsOnRandomBPs();
    void handlePushButton_47__transferExtBP_FromSourceAlienToDestAlienWithCloning();
    void handlePushButton_51__RemoveAndDeleteExtremityBP_FromAlien();
    void handlePushButton_8__DensifyBodyPart();
    void handlePushButton_12__ApplySepBP_ChangesToOrigBP_OnAlien();
    void handlePushButton_3__CloneBodyPartAndDisplayInNewTabWithAlienOfInterest();  // Clone the BP and display it in the same tab as the cloned Alien of Interest.
                                // If that tab does not exist, create one with the cloned aline in the first GLW pane.
                                // The cloned BPs will be displayed on the other GLW panes
    void handlePushButton_14__BevelBP();
    void handlePushButton_30__IncreaseNumOfJoints();
    void handlePushButton_31__DecreaseNumOfJoints();
    void handlePushButton_6__SmoothBP();
    void handlePushButton_34__IncreaseDensifierSpikiness();
    void handlePushButton_35__DecreaseDensifierSpikiness();
    void handlePushButton_38__IncreaseDensifierCurvature();
    void handleHorizontalSlider_16__IncreaseDensifierCurvature();
    void handleHorizontalSlider_20__incDecDenserSpikeMovingUpDownPerc();
    void handleHorizontalSlider_23__moveDensersAcrossBP_Bones();
    void handlePushButton_39__DecreaseDensifierCurvature();
    void handlePushButton_40__IncreaseDensifierExtrusions(); // increase stacked densers
    void handlePushButton_41__DecreaseDensifierExtrusions(); // decrease stacked densers
    void handlePushButton_43__MutateExtremBPsCollectively();
    void handlePushButton_48__GenerateAliens();
    void handlePushButton_4__SaveAlienToFile();
    void handlePushButton_9__LoadAlienFromFile();
    void handlePushButton_73__AddNewBPsOfSelectedType();
    void handlePushButton_74__RemoveSelectedBP();
    void handlePushButton_54__IncreaseBP_Size();
    void handlePushButton_55__DecreaseBP_Size();
    void handlePushButton_20__mutateBP_JAT_Randomly();
    void handlePushButton_21__mutateBP_JAT_FromOptions();
    void handlePushButton_18__generateGroupBPs();
    void handlePushButton_15__squashBP();
    void handlePushButton_42__stretchBP();
    void handlePushButton_26__computeAngleBetweenVectors();
    void handlePushButton_56__showAngleTrendsForJntsOfBP();
    void handlePushButton_69__genClonedBPsAndAttachNearby();
    void handlePushButton_84__PruneBP();
    void handlePushButton_16__modelHeads();
    void handlePushButton_46__genAndAttachNewHeadOnAlien();
    void handlePushButton_13__reattachSelectedBPsToDiffLocations();
    void handlePushButton_81__BulgifyBP();
    void handlePushButton_89__SubdiveBP_BonesIntoSubdivCells();
    // void handlePushButton_92__BulkUpSubdivCellsForGivenBP_Bones();

    void handlePushButton_JAT_RS_SaveValues();
    void handlePushButton_JAT_RS_Default_Values();
    void updateSpinBoxesMinAndMax_AlienTab();
    void setActivationOfExtremGroupMutationControls();

    void selectAllBPsOfSelectedType_FromComboBox18();

    void mutateSelectedBP_JAT_EnergyOrAngleUsingHorSlider();

    void increaseDecreaseBulkinessUsingHorizontalSlider_15();

    void mutateStretchSquashExtremBPsCollectively();
    void mutateNumOfJointsExtremBPsCollectively();

private:
    Ui::MainWindow1 *ui;

    QPushButton *dockBtn;

    int iInd;
    int jInd;
    int kInd;
    int new_iInd;
    int new_jInd;
    int new_kInd;

    AlienModeller *alModlerPtr;

    QMap<int, GLWidget*> glWidgetID_Map; // associates a unique key (int) to each glWidget. This is because there are 1 or many glWidgets in a frame
    QMap<int, QFrame*> frameID_Map; // associates a unique key (int) to each frame. This is because there are several frames in a tab
    QMap<int, QWidget*> tabID_Map; // associates a unique key (int) to each tab.
    QMap<int, QTabWidget*> tabWidgetID_Map; // associates a unique key (int) to each TabWidget.
    QMap<int, int> tabToBPType_Map; // used to keep track of which tab is displaying which BP. Key is tab_ID, value is the BP_TYPE_ID
    // QMap< tab_ID, QMap< frame_ID, A list of glwidgets_ID >>
    QMap<int, QMap<int, QPolygon>> tabToFrameToGLWidget_Map;
    int GLWidgetID_Counter;

    // Handy Maps
    QMap<int, int> glWgtToAlien_Map;    // This map allows us to identify which alien is being rendered in which GLwidget
                                        // ... Key(int) is the widget ID and value is the ALien ID
                                        // TODO: Later on there might be a need to have multiple aliens displayed in a GLW
                                        // ... Perhaps make it a multimap
    QMap<int, int> tabToAlienOfInterest_Map;    // This map allows us to identify which alien of interet is being allocated to which tab
                                        // ... Key(int) is the tab ID and value is the ALien ID
    QMap<int, int> glWgtToAlienOfInterest_Map;  // This map allows us to identify the first GLW that houses the Alien of Interest
                                                // ... Key(int) is the widget ID and value is the ALien ID

    QMap<int,int> tabToTabWgtMap; // This map stores the index of each tab and allocates it the pointer
                                        // ... to the tabwidget that it houses.
                                        // Each nested tabwidget houses a clone of the alien that was selected by the user
    QMap<int,int> tabWgtToTabMap;
    int tabCounter;
    QVector3D globalPos;
    QVector3D globalVec;

    int squashStretchSliderVal;
    int numOfJntsSliderVal;

    int prevHorizSlider15Value_IncDecBulkiness;
    int prevHorizSlider16Value_IncDecDensCurvature;
    int prevHorizSlider20Value_incDecDenserSpikeMovingUpDownPerc;
    int prevHorizSlider23Value_MoveDensersAcrossBP_Bones;

    // int prevMutateJAT_EnergyValue_Slider13;
    // int prevMutateJAT_AngleValue_Slider14;
};

#endif // MAINWINDOW1_H



/*
 *
#ifndef UI_MAINWINDOW1_H
#define UI_MAINWINDOW1_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow1
{
public:
    QWidget *centralwidget;
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout_2;
    QFrame *frame_3;
    QTabWidget *tabWidget_2;
    QWidget *tab_4;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout;
    QFrame *frame_6;
    QFrame *frame_5;
    QFrame *frame_4;
    QFrame *frame;
    QFrame *frame_2;
    QWidget *tab_5;
    QTabWidget *tabWidget;
    QWidget *tab;
    QWidget *tab_2;
    QWidget *tab_3;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow1)
    {
        if (MainWindow1->objectName().isEmpty())
            MainWindow1->setObjectName(QStringLiteral("MainWindow1"));
        MainWindow1->resize(1273, 784);
        centralwidget = new QWidget(MainWindow1);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        horizontalLayout = new QHBoxLayout(centralwidget);
        horizontalLayout->setSpacing(2);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(3, 3, 3, 3);
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setSpacing(1);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout_2->setSizeConstraint(QLayout::SetMinimumSize);
        verticalLayout_2->setContentsMargins(1, 1, 1, 1);
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(3);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        frame_3 = new QFrame(centralwidget);
        frame_3->setObjectName(QStringLiteral("frame_3"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(frame_3->sizePolicy().hasHeightForWidth());
        frame_3->setSizePolicy(sizePolicy);
        frame_3->setMinimumSize(QSize(500, 0));
        frame_3->setLayoutDirection(Qt::LeftToRight);
        frame_3->setFrameShape(QFrame::StyledPanel);
        frame_3->setFrameShadow(QFrame::Raised);

        horizontalLayout_2->addWidget(frame_3);

        tabWidget_2 = new QTabWidget(centralwidget);
        tabWidget_2->setObjectName(QStringLiteral("tabWidget_2"));
        tab_4 = new QWidget();
        tab_4->setObjectName(QStringLiteral("tab_4"));
        verticalLayoutWidget = new QWidget(tab_4);
        verticalLayoutWidget->setObjectName(QStringLiteral("verticalLayoutWidget"));
        verticalLayoutWidget->setGeometry(QRect(0, 0, 1011, 701));
        verticalLayout = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout->setSpacing(1);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        frame_6 = new QFrame(verticalLayoutWidget);
        frame_6->setObjectName(QStringLiteral("frame_6"));
        frame_6->setFrameShape(QFrame::StyledPanel);
        frame_6->setFrameShadow(QFrame::Raised);

        verticalLayout->addWidget(frame_6);

        frame_5 = new QFrame(verticalLayoutWidget);
        frame_5->setObjectName(QStringLiteral("frame_5"));
        frame_5->setFrameShape(QFrame::StyledPanel);
        frame_5->setFrameShadow(QFrame::Raised);

        verticalLayout->addWidget(frame_5);

        frame_4 = new QFrame(verticalLayoutWidget);
        frame_4->setObjectName(QStringLiteral("frame_4"));
        frame_4->setFrameShape(QFrame::StyledPanel);
        frame_4->setFrameShadow(QFrame::Raised);

        verticalLayout->addWidget(frame_4);

        frame = new QFrame(verticalLayoutWidget);
        frame->setObjectName(QStringLiteral("frame"));
        sizePolicy.setHeightForWidth(frame->sizePolicy().hasHeightForWidth());
        frame->setSizePolicy(sizePolicy);
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        frame->setLineWidth(0);

        verticalLayout->addWidget(frame);

        frame_2 = new QFrame(verticalLayoutWidget);
        frame_2->setObjectName(QStringLiteral("frame_2"));
        frame_2->setFrameShape(QFrame::StyledPanel);
        frame_2->setFrameShadow(QFrame::Raised);

        verticalLayout->addWidget(frame_2);

        tabWidget_2->addTab(tab_4, QString());
        tab_5 = new QWidget();
        tab_5->setObjectName(QStringLiteral("tab_5"));
        tabWidget_2->addTab(tab_5, QString());

        horizontalLayout_2->addWidget(tabWidget_2);


        verticalLayout_2->addLayout(horizontalLayout_2);


        horizontalLayout->addLayout(verticalLayout_2);

        tabWidget = new QTabWidget(centralwidget);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(tabWidget->sizePolicy().hasHeightForWidth());
        tabWidget->setSizePolicy(sizePolicy1);
        tabWidget->setMinimumSize(QSize(250, 0));
        tabWidget->setMaximumSize(QSize(16777215, 16777215));
        tabWidget->setSizeIncrement(QSize(0, 0));
        tabWidget->setBaseSize(QSize(0, 0));
        tabWidget->setTabPosition(QTabWidget::North);
        tabWidget->setTabShape(QTabWidget::Rounded);
        tabWidget->setElideMode(Qt::ElideNone);
        tabWidget->setUsesScrollButtons(true);
        tab = new QWidget();
        tab->setObjectName(QStringLiteral("tab"));
        tabWidget->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QStringLiteral("tab_2"));
        tabWidget->addTab(tab_2, QString());
        tab_3 = new QWidget();
        tab_3->setObjectName(QStringLiteral("tab_3"));
        tabWidget->addTab(tab_3, QString());

        horizontalLayout->addWidget(tabWidget);

        MainWindow1->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow1);
        menubar->setObjectName(QStringLiteral("menubar"));
        menubar->setGeometry(QRect(0, 0, 1273, 21));
        MainWindow1->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow1);
        statusbar->setObjectName(QStringLiteral("statusbar"));
        MainWindow1->setStatusBar(statusbar);

        retranslateUi(MainWindow1);

        tabWidget_2->setCurrentIndex(0);
        tabWidget->setCurrentIndex(2);


        QMetaObject::connectSlotsByName(MainWindow1);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow1)
    {
        MainWindow1->setWindowTitle(QApplication::translate("MainWindow1", "MainWindow", Q_NULLPTR));
        tabWidget_2->setTabText(tabWidget_2->indexOf(tab_4), QApplication::translate("MainWindow1", "Tab 1", Q_NULLPTR));
        tabWidget_2->setTabText(tabWidget_2->indexOf(tab_5), QApplication::translate("MainWindow1", "Tab 2", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("MainWindow1", "Tab 1", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("MainWindow1", "Tab 2", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(tab_3), QApplication::translate("MainWindow1", "Page", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MainWindow1: public Ui_MainWindow1 {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW1_H

*/
