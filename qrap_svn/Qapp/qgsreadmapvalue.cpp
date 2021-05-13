
#include "qgsreadmapvalue.h"
//#include "qgsfeatureaction.h"

//#include "qgsapplication.h"
//#include "qgsactionmanager.h"
//#include "qgsattributedialog.h"
#include <qgsdockwidget.h>
#include "qgseditorwidgetregistry.h"

//#include "qgsgeometry.h"
//#include "qgshighlight.h"
//#include "qgsidentifyresultsdialog.h"
//#include "qgslogger.h"
//#include "qgsmapcanvas.h"
#include "qgsmaplayeractionregistry.h"
#include "qgsmaplayer.h"
//#include "qgsnetworkaccessmanager.h"
//#include "qgsproject.h"
#include "qgsrasterlayer.h"
#include "qgsvectorlayer.h"
#include "qgsvectordataprovider.h"
//#include "qgswebview.h"
//#include "qgswebframe.h"
#include "qgsstringutils.h"
//#include "qgsfiledownloader.h"


#include <QCloseEvent>
#include <QLabel>
#include <QAction>
#include <QTreeWidgetItem>  //
#include <QPixmap>
#include <QSettings>
#include <QMenu>
#include <QClipboard>
#include <QMenuBar>
#include <QPushButton>
#include <QPrinter>
#include <QPrintDialog>
#include <QDesktopServices>
#include <QMessageBox>
#include <QComboBox>
#include <QTextDocument>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFileDialog>
#include <QFileInfo>
#include <QRegExp>
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
  mHelpToolButton->hide();
  cbxAutoFeatureForm->hide();
  lblIdentifyMode->hide();
  lblIdentifyMode->hide();
  cmbIdentifyMode->hide();
  cmbViewMode->hide();

  QSettings mySettings;
  mDock = new QgsDockWidget( tr( "Wireless Map Reader" ), mQGisApp );

  mDock->setObjectName( "Wireless Map Reader" );
  mDock->setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
  mDock->setWidget( this );
//  if ( !mQGisApp->restoreDockWidget( mDock ) )
    mQGisApp->addDockWidget( Qt::RightDockWidgetArea, mDock );

    lstResults->setColumnCount( 2 );
    lstResults->sortByColumn( -1 );
    setColumnText( 0, tr( "Layer" ) );
    setColumnText( 1, tr( "Value" ) );

    int width = mySettings.value( "/Windows/Identify/columnWidth", "0" ).toInt();
    if ( width > 0 )
    {
      lstResults->setColumnWidth( 0, width );
    }
    width = mySettings.value( "/Windows/Identify/columnWidthTable", "0" ).toInt();
    if ( width > 0 )
    {
      tblResults->setColumnWidth( 0, width );
    }




}


void QgsReadMapValue::setColumnText( int column, const QString & label )
{
  QTreeWidgetItem* header = lstResults->headerItem();
  header->setText( column, label );
}




QgsReadMapValue::~QgsReadMapValue()
{



}




void QgsReadMapValue::deactivate()
{

}



//!    /** Map tool was activated */
void QgsReadMapValue::activate()
{
    std::cout<<"qgsreadmapvalue activate called"<<std::endl;
    if ( lstResults->topLevelItemCount() > 0 )
    {
      raise();  //! raise this widget to the top of the parent widget's stack
    }

}

//! problems here. please find what called this func?
void QgsReadMapValue::disconnectLayer( QObject *layer )
{
  if ( !layer )
    return;

  QgsVectorLayer *vlayer = qobject_cast<QgsVectorLayer *>( layer );
  if ( vlayer )
  {
    disconnect( vlayer, SIGNAL( layerDeleted() ), this, SLOT( layerDestroyed() ) );
    disconnect( vlayer, SIGNAL( featureDeleted( QgsFeatureId ) ), this, SLOT( featureDeleted( QgsFeatureId ) ) );
    disconnect( vlayer, SIGNAL( attributeValueChanged( QgsFeatureId, int, const QVariant & ) ),
                this, SLOT( attributeValueChanged( QgsFeatureId, int, const QVariant & ) ) );
    disconnect( vlayer, SIGNAL( editingStarted() ), this, SLOT( editingToggled() ) );
    disconnect( vlayer, SIGNAL( editingStopped() ), this, SLOT( editingToggled() ) );
  }
  else
  {
    disconnect( layer, SIGNAL( destroyed() ), this, SLOT( layerDestroyed() ) );
  }
}


//!------------------------------------------
void QgsReadMapValue::clear()
{
    for ( int i = 0; i < lstResults->topLevelItemCount(); i++ )
    {
      disconnectLayer( lstResults->topLevelItem( i )->data( 0, Qt::UserRole ).value<QObject *>() );
    }

    lstResults->clear();
    lstResults->sortByColumn( -1 );
//    clearHighlights();

    tblResults->clearContents();
    tblResults->setRowCount( 0 );

    mPlot->setVisible( false );
//    Q_FOREACH ( QgsIdentifyPlotCurve *curve, mPlotCurves )
//      delete curve;
//    mPlotCurves.clear();

    // keep it visible but disabled, it can switch from disabled/enabled
    // after raster format change
    mActionPrint->setDisabled( true );

}





void QgsReadMapValue::show()
{
    bool showFeatureForm = false;

    if ( lstResults->topLevelItemCount() > 0 )
    {
      QTreeWidgetItem *layItem = lstResults->topLevelItem( 0 );
      QTreeWidgetItem *featItem = layItem->child( 0 );

      if ( lstResults->topLevelItemCount() == 1 && layItem->childCount() == 1 )
      {
        lstResults->setCurrentItem( featItem );

        if ( QSettings().value( "/Map/identifyAutoFeatureForm", false ).toBool() )
        {
          QgsVectorLayer *layer = qobject_cast<QgsVectorLayer *>( layItem->data( 0, Qt::UserRole ).value<QObject *>() );
          if ( layer )
          {
            // if this is the only feature and it's on a vector layer
            // show the form dialog instead of the results window
            showFeatureForm = true;
          }
        }
      }

      // expand first layer and feature
      featItem->setExpanded( true );
      layItem->setExpanded( true );
    }

    // expand all if enabled
    if ( mExpandNewAction->isChecked() )
    {
      lstResults->expandAll();
    }

    QDialog::show();

    // when the feature form is opened don't show and raise the identify result.
    // If it's not docked, the results would open after or possibly on top of the
    // feature form and stay open (on top the canvas) after the feature form is
    // closed.
    if ( showFeatureForm )
    {
      //featureForm();
    }
    else
    {
      mDock->show();
      mDock->raise();
    }
}



//!-------------------------------------------------------------------------?????
void QgsReadMapValue::updateViewModes()
{
    // get # of identified vector and raster layers - there must be a better way involving caching
    int vectorCount = 0, rasterCount = 0;
    for ( int i = 0; i < lstResults->topLevelItemCount(); i++ )
    {
      QTreeWidgetItem *item = lstResults->topLevelItem( i );
      if ( vectorLayer( item ) ) vectorCount++;
      else if ( rasterLayer( item ) ) rasterCount++;
    }

    lblViewMode->setEnabled( rasterCount > 0 );
    cmbViewMode->setEnabled( rasterCount > 0 );
    if ( rasterCount == 0 )
      cmbViewMode->setCurrentIndex( 0 );



}



//!------------根据feature是vectorlayer or raster layer 分别处理----------------------------
void QgsReadMapValue::addFeature( const QgsMapToolIdentify::IdentifyResult& result )
{
    if ( result.mLayer->type() == QgsMapLayer::VectorLayer )
    {
      addFeature( qobject_cast<QgsVectorLayer *>( result.mLayer ), result.mFeature, result.mDerivedAttributes );
    }
    else if ( result.mLayer->type() == QgsMapLayer::RasterLayer )
    {
      addFeature( qobject_cast<QgsRasterLayer *>( result.mLayer ), result.mLabel, result.mAttributes, result.mDerivedAttributes, result.mFields, result.mFeature, result.mParams );
    }
}



//!----------------------------------------
void QgsReadMapValue::addFeature( QgsVectorLayer *vlayer, const QgsFeature &f, const QMap<QString, QString> &derivedAttributes )
{
  QTreeWidgetItem *layItem = layerItem( vlayer );

  if ( !layItem )
  {
    layItem = new QTreeWidgetItem( QStringList() << vlayer->name() );
    layItem->setData( 0, Qt::UserRole, QVariant::fromValue( qobject_cast<QObject *>( vlayer ) ) );
    lstResults->addTopLevelItem( layItem );

    connect( vlayer, SIGNAL( layerDeleted() ), this, SLOT( layerDestroyed() ) );
    connect( vlayer, SIGNAL( layerCrsChanged() ), this, SLOT( layerDestroyed() ) );
    connect( vlayer, SIGNAL( featureDeleted( QgsFeatureId ) ), this, SLOT( featureDeleted( QgsFeatureId ) ) );
    connect( vlayer, SIGNAL( attributeValueChanged( QgsFeatureId, int, const QVariant & ) ),
             this, SLOT( attributeValueChanged( QgsFeatureId, int, const QVariant & ) ) );
    connect( vlayer, SIGNAL( editingStarted() ), this, SLOT( editingToggled() ) );
    connect( vlayer, SIGNAL( editingStopped() ), this, SLOT( editingToggled() ) );
  }

  QgsIdentifyResultsFeatureItem *featItem = new QgsIdentifyResultsFeatureItem( vlayer->fields(), f, vlayer->crs() );
  featItem->setData( 0, Qt::UserRole, FID_TO_STRING( f.id() ) );
  featItem->setData( 0, Qt::UserRole + 1, mFeatures.size() );
  mFeatures << f;
  layItem->addChild( featItem );

  if ( derivedAttributes.size() >= 0 )
  {
    QTreeWidgetItem *derivedItem = new QTreeWidgetItem( QStringList() << tr( "(Derived)" ) );
    derivedItem->setData( 0, Qt::UserRole, "derived" );
    featItem->addChild( derivedItem );

    for ( QMap< QString, QString>::const_iterator it = derivedAttributes.begin(); it != derivedAttributes.end(); ++it )
    {
      derivedItem->addChild( new QTreeWidgetItem( QStringList() << it.key() << it.value() ) );
    }
  }

  //get valid QgsMapLayerActions for this layer
  QList< QgsMapLayerAction* > registeredActions = QgsMapLayerActionRegistry::instance()->mapLayerActions( vlayer );

  if ( !vlayer->fields().isEmpty() || vlayer->actions()->size() || !registeredActions.isEmpty() )
  {
    QTreeWidgetItem *actionItem = new QTreeWidgetItem( QStringList() << tr( "(Actions)" ) );
    actionItem->setData( 0, Qt::UserRole, "actions" );
    featItem->addChild( actionItem );

    if ( vlayer->fields().size() > 0 )
    {
      QTreeWidgetItem *editItem = new QTreeWidgetItem( QStringList() << "" << ( vlayer->isEditable() ? tr( "Edit feature form" ) : tr( "View feature form" ) ) );
      editItem->setIcon( 0, QgsApplication::getThemeIcon( "/mActionFormView.svg" ) );
      editItem->setData( 0, Qt::UserRole, "edit" );
      actionItem->addChild( editItem );
    }

    for ( int i = 0; i < vlayer->actions()->size(); i++ )
    {
      const QgsAction &action = vlayer->actions()->at( i );

      if ( !action.runable() )
        continue;

      QTreeWidgetItem *twi = new QTreeWidgetItem( QStringList() << "" << action.name() );
      twi->setIcon( 0, QgsApplication::getThemeIcon( "/mAction.svg" ) );
      twi->setData( 0, Qt::UserRole, "action" );
      twi->setData( 0, Qt::UserRole + 1, QVariant::fromValue( i ) );
      actionItem->addChild( twi );
    }

    //add actions from QgsMapLayerActionRegistry
    for ( int i = 0; i < registeredActions.size(); i++ )
    {
      QgsMapLayerAction* action = registeredActions.at( i );
      QTreeWidgetItem *twi = new QTreeWidgetItem( QStringList() << "" << action->text() );
      twi->setIcon( 0, QgsApplication::getThemeIcon( "/mAction.svg" ) );
      twi->setData( 0, Qt::UserRole, "map_layer_action" );
      twi->setData( 0, Qt::UserRole + 1, qVariantFromValue( qobject_cast<QObject *>( action ) ) );
      actionItem->addChild( twi );

      connect( action, SIGNAL( destroyed() ), this, SLOT( mapLayerActionDestroyed() ) );
    }
  }

  const QgsFields &fields = vlayer->fields();
  QgsAttributes attrs = f.attributes();
  bool featureLabeled = false;
  for ( int i = 0; i < attrs.count(); ++i )
  {
    if ( i >= fields.count() )
      continue;

    if ( vlayer->editFormConfig()->widgetType( i ) == "Hidden" )
    {
      continue;
    }

    // _TAG
    QString defVal;
    if ( fields.fieldOrigin( i ) == QgsFields::OriginProvider && vlayer->dataProvider() )
      defVal = vlayer->dataProvider()->defaultValue( fields.fieldOriginIndex( i ), true ).toString();

    QString value = defVal == attrs.at( i ) ? defVal : fields.at( i ).displayString( attrs.at( i ) );
    QTreeWidgetItem *attrItem = new QTreeWidgetItem( QStringList() << QString::number( i ) << value );
    featItem->addChild( attrItem );

    attrItem->setData( 0, Qt::DisplayRole, vlayer->attributeDisplayName( i ) );
    attrItem->setData( 0, Qt::UserRole, fields[i].name() );
    attrItem->setData( 0, Qt::UserRole + 1, i );

    attrItem->setData( 1, Qt::UserRole, value );

    value = representValue( vlayer, fields.at( i ).name(), attrs.at( i ) );
    bool foundLinks = false;
    QString links = QgsStringUtils::insertLinks( value, &foundLinks );
    if ( foundLinks )
    {
      QLabel* valueLabel = new QLabel( links );
      valueLabel->setOpenExternalLinks( true );
      attrItem->treeWidget()->setItemWidget( attrItem, 1, valueLabel );
      attrItem->setData( 1, Qt::DisplayRole, QString() );
    }
    else
    {
      attrItem->setData( 1, Qt::DisplayRole, value );
      attrItem->treeWidget()->setItemWidget( attrItem, 1, nullptr );
    }

    if ( fields[i].name() == vlayer->displayField() )
    {
      featItem->setText( 0, attrItem->text( 0 ) );
      featItem->setText( 1, attrItem->text( 1 ) );
      featureLabeled = true;
    }
  }

  if ( !featureLabeled )
  {
    featItem->setText( 0, tr( "feature id" ) );
    featItem->setText( 1, QString::number( f.id() ) );
  }

  // table
  int j = tblResults->rowCount();
  for ( int i = 0; i < attrs.count(); ++i )
  {
    if ( i >= fields.count() )
      continue;

    QString value = fields.at( i ).displayString( attrs.at( i ) );
    QString value2 = representValue( vlayer, fields.at( i ).name(), attrs.at( i ) );

    tblResults->setRowCount( j + 1 );

    QgsDebugMsg( QString( "adding item #%1 / %2 / %3 / %4" ).arg( j ).arg( vlayer->name(), vlayer->attributeDisplayName( i ), value2 ) );

    QTableWidgetItem *item = new QTableWidgetItem( vlayer->name() );
    item->setData( Qt::UserRole, QVariant::fromValue( qobject_cast<QObject *>( vlayer ) ) );
    item->setData( Qt::UserRole + 1, vlayer->id() );
    tblResults->setItem( j, 0, item );

    item = new QTableWidgetItem( FID_TO_STRING( f.id() ) );
    item->setData( Qt::UserRole, FID_TO_STRING( f.id() ) );
    item->setData( Qt::UserRole + 1, mFeatures.size() );
    tblResults->setItem( j, 1, item );

    item = new QTableWidgetItem( QString::number( i ) );
    if ( fields[i].name() == vlayer->displayField() )
      item->setData( Qt::DisplayRole, vlayer->attributeDisplayName( i ) + " *" );
    else
      item->setData( Qt::DisplayRole, vlayer->attributeDisplayName( i ) );
    item->setData( Qt::UserRole, fields[i].name() );
    item->setData( Qt::UserRole + 1, i );
    tblResults->setItem( j, 2, item );

    item = new QTableWidgetItem( value );
    item->setData( Qt::UserRole, value );
    item->setData( Qt::DisplayRole, value2 );
    tblResults->setItem( j, 3, item );

    // highlight first item
    // if ( i==0 )
    // {
    //   QBrush b = tblResults->palette().brush( QPalette::AlternateBase );
    //   for ( int k = 0; k <= 3; k++)
    //  tblResults->item( j, k )->setBackground( b );
    // }

    tblResults->resizeRowToContents( j );
    j++;
  }
  //tblResults->resizeColumnToContents( 1 );

  highlightFeature( featItem );
}



void QgsReadMapValue::highlightFeature( QTreeWidgetItem *item )
{
//  QgsMapLayer *layer;
//  QgsVectorLayer *vlayer = vectorLayer( item );
//  QgsRasterLayer *rlayer = rasterLayer( item );

//  layer = vlayer ? static_cast<QgsMapLayer *>( vlayer ) : static_cast<QgsMapLayer *>( rlayer );

//  if ( !layer ) return;

//  QgsIdentifyResultsFeatureItem *featItem = dynamic_cast<QgsIdentifyResultsFeatureItem *>( featureItem( item ) );
//  if ( !featItem )
//    return;

//  if ( mHighlights.contains( featItem ) )
//    return;

//  if ( !featItem->feature().constGeometry() || featItem->feature().constGeometry()->wkbType() == QGis::WKBUnknown )
//    return;

//  QgsHighlight *highlight = nullptr;
//  if ( vlayer )
//  {
//    highlight = new QgsHighlight( mCanvas, featItem->feature(), vlayer );
//  }
//  else
//  {
//    highlight = new QgsHighlight( mCanvas, featItem->feature().constGeometry(), layer );
//    highlight->setWidth( 2 );
//  }

//  QSettings settings;
//  QColor color = QColor( settings.value( "/Map/highlight/color", QGis::DEFAULT_HIGHLIGHT_COLOR.name() ).toString() );
//  int alpha = settings.value( "/Map/highlight/colorAlpha", QGis::DEFAULT_HIGHLIGHT_COLOR.alpha() ).toInt();
//  double buffer = settings.value( "/Map/highlight/buffer", QGis::DEFAULT_HIGHLIGHT_BUFFER_MM ).toDouble();
//  double minWidth = settings.value( "/Map/highlight/minWidth", QGis::DEFAULT_HIGHLIGHT_MIN_WIDTH_MM ).toDouble();

//  highlight->setColor( color ); // sets also fill with default alpha
//  color.setAlpha( alpha );
//  highlight->setFillColor( color ); // sets fill with alpha
//  highlight->setBuffer( buffer );
//  highlight->setMinWidth( minWidth );
//  highlight->show();
//  mHighlights.insert( featItem, highlight );
}



void QgsReadMapValue::setExpressionContextScope( const QgsExpressionContextScope &scope )
{

}


void QgsReadMapValue::addFeature( QgsRasterLayer *layer,
    const QString& label,
    const QMap<QString, QString> &attributes,
    const QMap<QString, QString> &derivedAttributes,
    const QgsFields &fields,
    const QgsFeature &feature,
    const QMap<QString, QVariant> &params )
{
  QgsDebugMsg( QString( "feature.isValid() = %1" ).arg( feature.isValid() ) );
  QTreeWidgetItem *layItem = layerItem( layer );

  QgsRaster::IdentifyFormat currentFormat = QgsRasterDataProvider::identifyFormatFromName( layer->customProperty( "identify/format" ).toString() );

  if ( !layItem )
  {
    layItem = new QTreeWidgetItem( QStringList() << QString::number( lstResults->topLevelItemCount() ) << layer->name() );
    layItem->setData( 0, Qt::UserRole, QVariant::fromValue( qobject_cast<QObject *>( layer ) ) );

    lstResults->addTopLevelItem( layItem );

    QComboBox *formatCombo = new QComboBox();

    // Add all supported formats, best first. HTML is considered the best because
    // it usually holds most information.
    int capabilities = layer->dataProvider()->capabilities();
    QList<QgsRaster::IdentifyFormat> formats;
    formats << QgsRaster::IdentifyFormatHtml
    << QgsRaster::IdentifyFormatFeature
    << QgsRaster::IdentifyFormatText
    << QgsRaster::IdentifyFormatValue;
    Q_FOREACH ( QgsRaster::IdentifyFormat f, formats )
    {
      if ( !( QgsRasterDataProvider::identifyFormatToCapability( f ) & capabilities ) )
        continue;
      formatCombo->addItem( QgsRasterDataProvider::identifyFormatLabel( f ), f );
      formatCombo->setItemData( formatCombo->count() - 1, qVariantFromValue( qobject_cast<QObject *>( layer ) ), Qt::UserRole + 1 );
      if ( currentFormat == f )
        formatCombo->setCurrentIndex( formatCombo->count() - 1 );
    }

    if ( formatCombo->count() > 1 )
    {
      // Add format combo box item
      // Space added before format to keep it first in ordered list: TODO better (user data)
      QTreeWidgetItem *formatItem = new QTreeWidgetItem( QStringList() << ' ' + tr( "Format" ) );
      layItem->addChild( formatItem );
      lstResults->setItemWidget( formatItem, 1, formatCombo );
      connect( formatCombo, SIGNAL( currentIndexChanged( int ) ), this, SLOT( formatChanged( int ) ) );
    }
    else
    {
      delete formatCombo;
    }
    //! No such slot QgsReadMapValue::layerDestroyed()
    connect( layer, SIGNAL( destroyed() ), this, SLOT( layerDestroyed() ) );
    connect( layer, SIGNAL( layerCrsChanged() ), this, SLOT( layerDestroyed() ) );
  }
  // Set/reset getFeatureInfoUrl (currently only available for Feature, so it may change if format changes)
  layItem->setData( 0, GetFeatureInfoUrlRole, params.value( "getFeatureInfoUrl" ) );

  QgsIdentifyResultsFeatureItem *featItem = new QgsIdentifyResultsFeatureItem( fields, feature, layer->crs(), QStringList() << label << "" );
  layItem->addChild( featItem );

  // add feature attributes
  if ( feature.isValid() )
  {
    QgsDebugMsg( QString( "fields size = %1 attributes size = %2" ).arg( fields.size() ).arg( feature.attributes().size() ) );
    QgsAttributes attrs = feature.attributes();
    for ( int i = 0; i < attrs.count(); ++i )
    {
      if ( i >= fields.count() )
        continue;

      QTreeWidgetItem *attrItem = new QTreeWidgetItem( QStringList() << QString::number( i ) << attrs.at( i ).toString() );

      attrItem->setData( 0, Qt::DisplayRole, fields[i].name() );

      QVariant value = attrs.at( i );
      attrItem->setData( 1, Qt::DisplayRole, value );
      featItem->addChild( attrItem );
    }
  }

  if ( currentFormat == QgsRaster::IdentifyFormatHtml || currentFormat == QgsRaster::IdentifyFormatText )
  {
    QgsIdentifyResultsWebViewItem *attrItem = new QgsIdentifyResultsWebViewItem( lstResults );
    featItem->addChild( attrItem ); // before setHtml()!
    if ( !attributes.isEmpty() )
    {
      attrItem->setContent( attributes.begin().value().toUtf8(), currentFormat == QgsRaster::IdentifyFormatHtml ? "text/html" : "text/plain; charset=utf-8" );
    }
    else
    {
      attrItem->setContent( tr( "No attributes." ).toUtf8(), "text/plain; charset=utf-8" );
    }
  }
  else
  {
    for ( QMap<QString, QString>::const_iterator it = attributes.begin(); it != attributes.end(); ++it )
    {
      featItem->addChild( new QTreeWidgetItem( QStringList() << it.key() << it.value() ) );
    }
  }

  if ( derivedAttributes.size() >= 0 )
  {
    QTreeWidgetItem *derivedItem = new QTreeWidgetItem( QStringList() << tr( "(Derived)" ) );
    derivedItem->setData( 0, Qt::UserRole, "derived" );
    featItem->addChild( derivedItem );

    for ( QMap< QString, QString>::const_iterator it = derivedAttributes.begin(); it != derivedAttributes.end(); ++it )
    {
      derivedItem->addChild( new QTreeWidgetItem( QStringList() << it.key() << it.value() ) );
    }
  }

  // table
  int i = 0;
  int j = tblResults->rowCount();
  tblResults->setRowCount( j + attributes.count() );

  for ( QMap<QString, QString>::const_iterator it = attributes.begin(); it != attributes.end(); ++it )
  {
    QgsDebugMsg( QString( "adding item #%1 / %2 / %3 / %4" ).arg( j ).arg( layer->name(), it.key(), it.value() ) );
    QTableWidgetItem *item = new QTableWidgetItem( layer->name() );
    item->setData( Qt::UserRole, QVariant::fromValue( qobject_cast<QObject *>( layer ) ) );
    item->setData( Qt::UserRole + 1, layer->id() );
    tblResults->setItem( j, 0, item );
    tblResults->setItem( j, 1, new QTableWidgetItem( QString::number( i + 1 ) ) );
    tblResults->setItem( j, 2, new QTableWidgetItem( it.key() ) );
    tblResults->setItem( j, 3, new QTableWidgetItem( it.value() ) );

    tblResults->resizeRowToContents( j );

    j++;
    i++;
  }
  //tblResults->resizeColumnToContents( 1 );

//  // graph
//  if ( !attributes.isEmpty() )
//  {
//    mPlotCurves.append( new QgsIdentifyPlotCurve( attributes, mPlot, layer->name() ) );
//  }
}



QTreeWidgetItem *QgsReadMapValue::featureItem( QTreeWidgetItem *item )
{
  if ( !item )
    return nullptr;

  QTreeWidgetItem *featItem = nullptr;
  if ( item->parent() )
  {
    if ( item->parent()->parent() )
    {
      if ( item->parent()->parent()->parent() )
      {
        // derived or action attribute item
        featItem = item->parent()->parent();
      }
      else
      {
        // attribute item
        featItem = item->parent();
      }
    }
    else
    {
      // feature item
      featItem = item;
    }
  }
  else
  {
    // top level layer item, return feature item if only one
#if 0
    if ( item->childCount() > 1 )
      return 0;
    featItem = item->child( 0 );
#endif

    int count = 0;

    for ( int i = 0; i < item->childCount(); i++ )
    {
      QgsIdentifyResultsFeatureItem *fi = dynamic_cast<QgsIdentifyResultsFeatureItem *>( item->child( i ) );
      if ( fi )
      {
        count++;
        if ( !featItem )
          featItem = fi;
      }
    }

    if ( count != 1 )
      return nullptr;
  }

  return featItem;
}



QTreeWidgetItem *QgsReadMapValue::layerItem( QTreeWidgetItem *item )
{
  if ( item && item->parent() )
  {
    item = featureItem( item )->parent();
  }

  return item;
}

QTreeWidgetItem *QgsReadMapValue::layerItem( QObject *object )
{
  for ( int i = 0; i < lstResults->topLevelItemCount(); i++ )
  {
    QTreeWidgetItem *item = lstResults->topLevelItem( i );

    if ( item->data( 0, Qt::UserRole ).value<QObject *>() == object )
      return item;
  }

  return nullptr;
}


QgsMapLayer *QgsReadMapValue::layer( QTreeWidgetItem *item )
{
  item = layerItem( item );
  if ( !item )
    return nullptr;
  return qobject_cast<QgsMapLayer *>( item->data( 0, Qt::UserRole ).value<QObject *>() );
}

QgsVectorLayer *QgsReadMapValue::vectorLayer( QTreeWidgetItem *item )
{
  item = layerItem( item );
  if ( !item )
    return nullptr;
  return qobject_cast<QgsVectorLayer *>( item->data( 0, Qt::UserRole ).value<QObject *>() );
}

QgsRasterLayer *QgsReadMapValue::rasterLayer( QTreeWidgetItem *item )
{
  item = layerItem( item );
  if ( !item )
    return nullptr;
  return qobject_cast<QgsRasterLayer *>( item->data( 0, Qt::UserRole ).value<QObject *>() );
}


QString QgsReadMapValue::representValue( QgsVectorLayer* vlayer, const QString& fieldName, const QVariant& value )
{
  QVariant cache;
  QMap<QString, QVariant>& layerCaches = mWidgetCaches[vlayer->id()];

  QString widgetType = vlayer->editFormConfig()->widgetType( fieldName );
  QgsEditorWidgetFactory* factory = QgsEditorWidgetRegistry::instance()->factory( widgetType );

  int idx = vlayer->fieldNameIndex( fieldName );

  if ( !factory )
    return value.toString();

  if ( layerCaches.contains( fieldName ) )
  {
    cache = layerCaches[ fieldName ];
  }
  else
  {
    cache = factory->createCache( vlayer, idx, vlayer->editFormConfig()->widgetConfig( fieldName ) );
    layerCaches.insert( fieldName, cache );
  }

  return factory->representValue( vlayer, idx, vlayer->editFormConfig()->widgetConfig( fieldName ), cache, value );
}



QgsIdentifyResultsFeatureItem::QgsIdentifyResultsFeatureItem( const QgsFields &fields, const QgsFeature &feature, const QgsCoordinateReferenceSystem &crs, const QStringList & strings )
    : QTreeWidgetItem( strings )
    , mFields( fields )
    , mFeature( feature )
    , mCrs( crs )
{
}


void QgsReadMapValue::layerDestroyed()
{
  QObject *theSender = sender();

  for ( int i = 0; i < lstResults->topLevelItemCount(); i++ )
  {
    QTreeWidgetItem *layItem = lstResults->topLevelItem( i );

    if ( layItem->data( 0, Qt::UserRole ).value<QObject *>() == sender() )
    {
      for ( int j = 0; j < layItem->childCount(); j++ )
      {
//        delete mHighlights.take( layItem->child( j ) );
      }
    }
  }

  disconnectLayer( theSender );
  delete layerItem( theSender );

  // remove items, starting from last
  for ( int i = tblResults->rowCount() - 1; i >= 0; i-- )
  {
    QgsDebugMsg( QString( "item %1 / %2" ).arg( i ).arg( tblResults->rowCount() ) );
    QTableWidgetItem *layItem = tblResults->item( i, 0 );
    if ( layItem && layItem->data( Qt::UserRole ).value<QObject *>() == sender() )
    {
      QgsDebugMsg( QString( "removing row %1" ).arg( i ) );
      tblResults->removeRow( i );
    }
  }
}
