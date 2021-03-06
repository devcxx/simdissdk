/* -*- mode: c++ -*- */
/****************************************************************************
 *****                                                                  *****
 *****                   Classification: UNCLASSIFIED                   *****
 *****                    Classified By:                                *****
 *****                    Declassify On:                                *****
 *****                                                                  *****
 ****************************************************************************
 *
 *
 * Developed by: Naval Research Laboratory, Tactical Electronic Warfare Div.
 *               EW Modeling & Simulation, Code 5773
 *               4555 Overlook Ave.
 *               Washington, D.C. 20375-5339
 *
 * License for source code at https://simdis.nrl.navy.mil/License.aspx
 *
 * The U.S. Government retains all rights to use, duplicate, distribute,
 * disclose, or release this software.
 *
 */
/* -*-c++-*- */
/**
 * Qt Dockable Views
 * Demonstrates using simVis::View objects in QDockWidgets with a QMainWindow
 */

#include "simNotify/Notify.h"
#include "simCore/Common/Version.h"
#include "simCore/Common/HighPerformanceGraphics.h"
#include "simUtil/ExampleResources.h"

#include "simVis/ViewManager.h"
#include "simVis/View.h"
#include "simVis/SceneManager.h"

#include "osgEarthQt/ViewWidget"
#include "osgEarthUtil/Controls"

#include <QAction>
#include <QApplication>
#include <QDialog>
#include <QDockWidget>
#include <QLayout>
#include <QMainWindow>
#include <QResizeEvent>
#include <QToolBar>

#include "MyMainWindow.h"

namespace ui = osgEarth::Util::Controls;

int usage(char** argv)
{
  SIM_NOTICE << argv[0] << "\n"
    << "    --framerate [n]     : set the framerate"
    << std::endl;

  return 0;
}

////////////////////////////////////////////////////////////////////
MyMainWindow::MyMainWindow(int framerate)
  :viewCounter_(1)
{
  // create toolbar
  QToolBar* toolbar = new QToolBar(this);
  QAction* dialogAction = new QAction("New Dialog", this);
  QAction* dockableAction = new QAction("New Dockable", this);
  QAction* mainViewAction = new QAction("New Main View Pane", this);
  toolbar->addAction(dialogAction);
  toolbar->addAction(dockableAction);
  toolbar->addAction(mainViewAction);
  addToolBar(Qt::TopToolBarArea, toolbar);

  // set a blank central widget
  QWidget* center = new QWidget(this);
  center->setLayout(new QHBoxLayout());
  setCentralWidget(center);

  // create a viewer manager. The "args" are optional.
  viewMan_ = new simVis::ViewManager();
  // Note that the logarithmic depth buffer is not installed

  // disable the default ESC-to-quit event:
  viewMan_->getViewer()->setKeyEventSetsDone(0);
  viewMan_->getViewer()->setQuitEventSetsDone(false);

  // we need a map.
  osg::ref_ptr<osgEarth::Map> map = simExamples::createDefaultExampleMap();

  // A scene manager that all our views will share.
  sceneMan_ = new simVis::SceneManager();
  sceneMan_->setMap(map);

  // add sky node
  simExamples::addDefaultSkyNode(sceneMan_);

  // create our first widget, seems to be required on startup
  createViewDockable_();

  // timer fires a paint event.
  timer_ = new QTimer();
  // timer single shot to avoid infinite loop problems in Qt on MSVC11
  timer_->setSingleShot(true);
  connect(timer_, SIGNAL(timeout()), this, SLOT(update()));
  timer_->start(1000/framerate);

  // connect actions to our slots
  connect(dialogAction, SIGNAL(triggered()), this, SLOT(createViewDialog_()));
  connect(dockableAction, SIGNAL(triggered()), this, SLOT(createViewDockable_()));
  connect(mainViewAction, SIGNAL(triggered()), this, SLOT(createMainView_()));

}

MyMainWindow::~MyMainWindow()
{
  delete timer_;
}

void MyMainWindow::paintEvent(QPaintEvent* e)
{
  // refresh all the views.
  viewMan_->frame();
  timer_->start();
}

simVis::ViewManager* MyMainWindow::getViewManager()
{
  return viewMan_.get();
}

void MyMainWindow::createViewDialog_()
{
  QString viewName = QString("Dialog View %1").arg(viewCounter_++);
  osg::ref_ptr<simVis::View> view = createView_(viewName);

  // now create a dock widget for each inset
  QDialog* dialog = new QDialog(this);
  QWidget* viewWidget = new osgEarth::QtGui::ViewWidget(view);
  dialog->setWindowTitle(viewName);
  dialog->setLayout(new QHBoxLayout());
  dialog->layout()->addWidget(viewWidget);
  dialog->resize(100, 100);
  dialog->show();
}

void MyMainWindow::createViewDockable_()
{
  QString viewName = QString("Dockable View %1").arg(viewCounter_++);
  osg::ref_ptr<simVis::View> view = createView_(viewName);

  // now create a dock widget for each inset
  QDockWidget* dockable = new QDockWidget(this);
  QWidget* viewWidget = new osgEarth::QtGui::ViewWidget(view);
  dockable->setWidget(viewWidget);
  dockable->setWindowTitle(viewName);
  dockable->resize(100, 100);
  addDockWidget(Qt::RightDockWidgetArea, dockable);
}

void MyMainWindow::createMainView_()
{
  // Make a main view, hook it up, and add it to the view manager.
  QString viewName = QString("Main View %1").arg(viewCounter_++);
  osg::ref_ptr<simVis::View> mainview = createView_(viewName);

  // Make a Qt Widget to hold our view, and add that widget to the
  // main window.
  QWidget* viewWidget = new osgEarth::QtGui::ViewWidget(mainview);
  viewWidget->resize(100, 100);
  centralWidget()->layout()->addWidget(viewWidget);
}

simVis::View* MyMainWindow::createView_(const QString& name) const
{
  simVis::View* view = new simVis::View();
  view->setNavigationMode(simVis::NAVMODE_ROTATEPAN);
  view->setName(name.toStdString());

  // attach the scene manager and add it to the view manager.
  view->setSceneManager(sceneMan_.get());
  viewMan_->addView(view);
  view->installDebugHandlers();

  return view;
}
////////////////////////////////////////////////////////////////////

int main(int argc, char** argv)
{
  simCore::checkVersionThrow();
  osg::ArgumentParser arguments(&argc, argv);
  simExamples::configureSearchPaths();

  if (arguments.read("--help"))
    return usage(argv);

  // read the framerate
  int framerate = 20;
  arguments.read("--framerate", framerate);
  if (framerate <= 0)
    framerate = 20;

  // OK, time to set up the Qt Application and windows.
  QApplication qapp(argc, argv);

  // Our custom main window contains a ViewManager.
  MyMainWindow win(framerate);
  win.setGeometry(200, 400, 400, 400);

  // fire up the GUI.
  win.show();
  qapp.exec();
  return 0;
}
