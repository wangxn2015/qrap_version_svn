
#ifndef QGSREADMAPVALUE_H
#define QGSREADMAPVALUE_H

#include "ui_qgsidentifyresultsbase.h"
#include <qgsmaptoolidentify.h>
#include <qgsexpressioncontext.h>


#include "qgsactionmanager.h"
#include "qgscontexthelp.h"
#include "qgsfeature.h"
#include "qgsfeaturestore.h"
#include "qgsfield.h"

#include "qgscoordinatereferencesystem.h"
#include "qgsmaplayeractionregistry.h"
#include "qgswebview.h"



#include <QWidget>
#include <QList>
//#include <QNetworkRequest>
//#include <QNetworkReply>
//#include <QUrl>

//class QgisApp;
class QCloseEvent;
class QTreeWidgetItem;
class QAction;
class QMenu;

class QgsVectorLayer;
class QgsRasterLayer;
//class QgsHighlight;
class QgsMapCanvas;
class QgsDockWidget;

//class QwtPlotCurve;

class QgsIdentifyResultsWebView : public QgsWebView
{
    Q_OBJECT
  public:
    QgsIdentifyResultsWebView( QWidget *parent = nullptr );
    QSize sizeHint() const override;
  public slots:
    void print();
    void downloadRequested( const QNetworkRequest &request );
    void unsupportedContent( QNetworkReply *reply );
  protected:
    void contextMenuEvent( QContextMenuEvent* ) override;
    QgsWebView *createWindow( QWebPage::WebWindowType type ) override;
  private:
    void handleDownload( QUrl url );
};


class QgsIdentifyResultsFeatureItem: public QTreeWidgetItem
{
  public:
    QgsIdentifyResultsFeatureItem( const QgsFields &fields, const QgsFeature &feature, const QgsCoordinateReferenceSystem &crs, const QStringList & strings = QStringList() );
    const QgsFields &fields() const { return mFields; }
    const QgsFeature &feature() const { return mFeature; }
    const QgsCoordinateReferenceSystem &crs() { return mCrs; }

  private:
    QgsFields mFields;
    QgsFeature mFeature;
    QgsCoordinateReferenceSystem mCrs;
};

class QgsIdentifyResultsWebViewItem: public QObject, public QTreeWidgetItem
{
    Q_OBJECT

  public:
    QgsIdentifyResultsWebViewItem( QTreeWidget *treeWidget = nullptr );
    QgsIdentifyResultsWebView *webView() { return mWebView; }
    void setHtml( const QString &html );
    /** @note added in 2.1 */
    void setContent( const QByteArray & data, const QString & mimeType = QString(), const QUrl & baseUrl = QUrl() );

  public slots:
    void loadFinished( bool ok );

  private:
    QgsIdentifyResultsWebView *mWebView;
};




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
    void highlightFeature( QTreeWidgetItem *item );
    void addFeature( const QgsMapToolIdentify::IdentifyResult& result );
    void setExpressionContextScope( const QgsExpressionContextScope &scope );
    /** Add add feature from vector layer */
    void addFeature( QgsVectorLayer * layer,
                     const QgsFeature &f,
                     const QMap< QString, QString > &derivedAttributes );

    /** Add add feature from other layer */
    void addFeature( QgsRasterLayer * layer,
                     const QString& label,
                     const QMap< QString, QString > &attributes,
                     const QMap< QString, QString > &derivedAttributes,
                     const QgsFields &fields = QgsFields(),
                     const QgsFeature &feature = QgsFeature(),
                     const QMap<QString, QVariant> &params = ( QMap<QString, QVariant>() ) );

    void setColumnText( int column, const QString & label );
    QTreeWidgetItem *featureItem( QTreeWidgetItem *item );
    QTreeWidgetItem *layerItem( QTreeWidgetItem *item );
    QTreeWidgetItem *layerItem( QObject *layer );
    QgsMapLayer *layer( QTreeWidgetItem *item );
    QgsVectorLayer *vectorLayer( QTreeWidgetItem *item );
    QgsRasterLayer *rasterLayer( QTreeWidgetItem *item );
    QString representValue( QgsVectorLayer* vlayer, const QString& fieldName, const QVariant& value );
    QMap< QString, QMap< QString, QVariant > > mWidgetCaches;
    void disconnectLayer( QObject *layer );

    enum ItemDataRole
    {
      GetFeatureInfoUrlRole = Qt::UserRole + 10
    };
//  signals:


  public slots:
    /** Remove results */
    void show();
    void updateViewModes();
    void clear();
    void layerDestroyed();

  private:
//    QMenu *mActionPopup;
    QgsMapCanvas *mCanvas;
    QgsDockWidget *mDock;
    QList<QgsFeature> mFeatures;

//    QgisApp *mQGisApp;
    QMainWindow *mQGisApp;
};



#endif
