
#ifndef QGSREADMAPVALUE_H
#define QGSREADMAPVALUE_H

#include "ui_qgsidentifyresultsbase.h"
#include <qgsmaptoolidentify.h>
#include <qgsexpressioncontext.h>


//#include "qgsactionmanager.h"
//#include "qgscontexthelp.h"
//#include "qgsfeature.h"
//#include "qgsfeaturestore.h"
//#include "qgsfield.h"

//#include "qgscoordinatereferencesystem.h"
//#include "qgsmaplayeractionregistry.h"
//#include "qgswebview.h"



#include <QWidget>
//#include <QList>
//#include <QNetworkRequest>
//#include <QNetworkReply>
//#include <QUrl>

//class QgisApp;
//class QCloseEvent;
//class QTreeWidgetItem;
//class QAction;
//class QMenu;

//class QgsVectorLayer;
//class QgsRasterLayer;
//class QgsHighlight;
//class QgsMapCanvas;
class QgsDockWidget;

//class QwtPlotCurve;



class QgsReadMapValue: public QDialog, private Ui::QgsIdentifyResultsBase
{
    Q_OBJECT

  public:

    //! Constructor - takes it own copy of the QgsAttributeAction so

    QgsReadMapValue( QWidget *QGisApp, QgsMapCanvas *canvas, QWidget *parent = nullptr, Qt::WindowFlags f = nullptr );

    ~QgsReadMapValue();


//    /** Map tool was deactivated */
    void deactivate();

//    /** Map tool was activated */
    void activate();

    void addFeature( const QgsMapToolIdentify::IdentifyResult& result );
    void setExpressionContextScope( const QgsExpressionContextScope &scope );




//  signals:


  public slots:
    /** Remove results */
    void show();
    void updateViewModes();
    void clear();

  private:
//    QMenu *mActionPopup;
    QgsMapCanvas *mCanvas;
    QgsDockWidget *mDock;


//    QgisApp *mQGisApp;
    QMainWindow *mQGisApp;
};



#endif
