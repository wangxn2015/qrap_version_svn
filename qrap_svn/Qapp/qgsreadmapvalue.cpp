
#include "qgsreadmapvalue.h"
//#include "qgsfeatureaction.h"

//#include "qgsapplication.h"
//#include "qgsactionmanager.h"
//#include "qgsattributedialog.h"
#include <qgsdockwidget.h>
//#include "qgseditorwidgetregistry.h"

//#include "qgsgeometry.h"
//#include "qgshighlight.h"
//#include "qgsidentifyresultsdialog.h"
//#include "qgslogger.h"
//#include "qgsmapcanvas.h"
//#include "qgsmaplayeractionregistry.h"
//#include "qgsmaplayer.h"
//#include "qgsnetworkaccessmanager.h"
//#include "qgsproject.h"
//#include "qgsrasterlayer.h"
//#include "qgsvectorlayer.h"
//#include "qgsvectordataprovider.h"
//#include "qgswebview.h"
//#include "qgswebframe.h"
//#include "qgsstringutils.h"
//#include "qgsfiledownloader.h"

//#include <QCloseEvent>
//#include <QLabel>
//#include <QAction>
//#include <QTreeWidgetItem>
//#include <QPixmap>
//#include <QSettings>
//#include <QMenu>
//#include <QClipboard>
//#include <QMenuBar>
//#include <QPushButton>
//#include <QPrinter>
//#include <QPrintDialog>
//#include <QDesktopServices>
//#include <QMessageBox>
//#include <QComboBox>
//#include <QTextDocument>
//#include <QNetworkRequest>
//#include <QNetworkReply>
//#include <QFileDialog>
//#include <QFileInfo>
//#include <QRegExp>
#include <QMainWindow>


////graph
//#include <qwt_plot.h>
//#include <qwt_plot_curve.h>
//#include <qwt_symbol.h>
//#include <qwt_legend.h>
//#include "qgsvectorcolorrampv2.h" // for random colors




QgsReadMapValue::QgsReadMapValue( QWidget *QGisApp, QgsMapCanvas *canvas, QWidget *parent, Qt::WindowFlags f )
    : QDialog( parent, f )
//    , mActionPopup( nullptr )
    , mCanvas( canvas )
    , mDock( nullptr )

{
  setupUi( this );

  //!---------------------------
  mQGisApp = static_cast<QMainWindow*>(QGisApp);

  mOpenFormAction->setDisabled( true );

//  QSettings mySettings;
//  mDock = new QgsDockWidget( tr( "Identify Results" ), QgisApp::instance() );
    mDock = new QgsDockWidget( tr( "Identify Results" ), mQGisApp );

  mDock->setObjectName( "IdentifyResultsDock" );
  mDock->setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
  mDock->setWidget( this );
//  if ( !mQGisApp->restoreDockWidget( mDock ) )
    mQGisApp->addDockWidget( Qt::RightDockWidgetArea, mDock );

}


QgsReadMapValue::~QgsReadMapValue()
{



}




void QgsReadMapValue::deactivate()
{

}

//    /** Map tool was activated */
void QgsReadMapValue::activate()
{

}
void QgsReadMapValue::clear()
{

}
void QgsReadMapValue::show()
{

}
void QgsReadMapValue::updateViewModes()
{

}
void QgsReadMapValue::addFeature( const QgsMapToolIdentify::IdentifyResult& result )
{

}
void QgsReadMapValue::setExpressionContextScope( const QgsExpressionContextScope &scope )
{

}

