#include "ColorViewItem.h"
#include "BaseViewItemCreator.h"
#include <QtGui\QColorDialog>
#include <assert.h>

ColorViewItem::ColorViewItem(
  const QVariant& value,
  const QString& name
  )
  : BaseViewItem( name )
  , m_button( NULL )
{
  m_button = new QPushButton();
  m_button->setObjectName( "btnColorPicker" );
  m_button->setAutoFillBackground( true );

  // Connect button signal to appropriate slot
  connect( m_button, SIGNAL( clicked() ), this, SLOT( pickColor() ) );

  onModelValueChanged( value );
}

ColorViewItem::~ColorViewItem()
{
}

QWidget *ColorViewItem::getWidget()
{
  return m_button;
}

void ColorViewItem::onModelValueChanged( QVariant const &value )
{
  QColor currColor = value.value<QColor>();
  setButtonColor( currColor );
}

void ColorViewItem::setButtonColor( const QColor& color ) 
{
  if (color.isValid())
  {
    // Set value via CSS to keep in sync with other styles
    QString qss = QString( "background-color: %1" ).arg( color.name() );
    m_button->setStyleSheet( qss );
  }
}

void ColorViewItem::pickColor()
{
  QColor currColor = m_button->palette().color( QPalette::Button );
  QColorDialog qcd( currColor, NULL );
  connect( &qcd, SIGNAL( colorSelected( QColor ) ), 
           this, SLOT( onColorSelected( QColor ) ) );
  connect( &qcd, SIGNAL( currentColorChanged( QColor ) ), 
           this, SLOT( onColorChanged( QColor ) ) );
  qcd.exec();

  // If the user hits cancel, we wish to restore
  // the current state to the previous value
  QColor outColor = qcd.selectedColor();
  if (!outColor.isValid())
  {
    onColorChanged( currColor );
  }
}

void ColorViewItem::onColorChanged( QColor color )
{
  emit viewValueChanged(
    QVariant::fromValue( color ),
    0
    );
}

void ColorViewItem::onColorSelected( QColor color )
{
  emit viewValueChanged(
    QVariant::fromValue( color ),
    1
    );
}
//////////////////////////////////////////////////////////////////////////
//
static ColorViewItem* CreateItem(
  QString const &name,
  QVariant const &value,
  char const *tag
  )
{
  if (value.type() == QVariant( QColor() ).type())
  {
    return new ColorViewItem( value, name );
  }
  return nullptr;
}
EXPOSE_VIEW_ITEM( ColorViewItem, CreateItem, 3 );
