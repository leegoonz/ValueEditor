#include "RTValViewItem.h"
#include "BaseViewItemCreator.h"
#include "ViewItemFactory.h"
#include <QtGui/QWidget.h>
#include "QVariantRTVal.h"
#pragma warning(push, 0)
#include <FTL/JSONValue.h>
#pragma warning(pop)

RTValViewItem::RTValViewItem( QString name, const FabricCore::RTVal& value )
  : BaseComplexViewItem( name )
  , m_val(value)
{
  UpdateWidget();
}

RTValViewItem::~RTValViewItem()
{
}

QWidget *RTValViewItem::getWidget()
{
  return m_widget;
}

void RTValViewItem::onModelValueChanged( QVariant const &value )
{
  m_val = toRTVal( value );
  for (int i = 0; i < m_childNames.size(); i++)
  {
    const char* childName = m_childNames[i].data();
    FabricCore::RTVal childVal = m_val.maybeGetMemberRef( childName );
    assert( childVal.isValid() );

    routeModelValueChanged( i, toVariant( childVal ) );
  }

  UpdateWidget();
}

void RTValViewItem::onChildViewValueChanged(
  int index,
  QVariant const &value,
  bool commit
  )
{

  assert( index < m_childNames.size() );
  const char* childName = m_childNames[index].data();

  // We cannot simply create a new RTVal based on the QVariant type, as 
  // we have to set the type exactly the same as the original.  Get the
  // original child value to ensure the new value matches the internal type
  FabricCore::RTVal oldChildVal = m_val.maybeGetMemberRef( childName );
  VariantToRTVal( value, oldChildVal );
  m_val.setMember( childName, oldChildVal );

  emit viewValueChanged( toVariant( m_val ), commit );
}

void RTValViewItem::doAppendChildViewItems( QList<BaseViewItem*>& items )
{
  if (!m_val.isValid())
    return;

  try {

    FabricCore::RTVal desc = m_val.getJSON();
    if (!desc.isValid())
      return;

    const char* cdesc = desc.getStringCString();

    // parse cdesc, Build children from desc.
    FTL::JSONValue* value = FTL::JSONValue::Decode( cdesc );
    FTL::JSONObject* obj = value->cast<FTL::JSONObject>();

    // Construct a child for each instance
    ViewItemFactory* factory = ViewItemFactory::GetInstance();
    for (FTL::JSONObject::const_iterator itr = obj->begin(); itr != obj->end(); itr++)
    {
      const char* childName = itr->first.c_str();
      FabricCore::RTVal childVal = m_val.maybeGetMemberRef( childName );
      if (childVal.isValid())
      {
        BaseViewItem* childItem = factory->CreateViewItem( childName, toVariant( childVal ), NULL );
        if (childItem != NULL)
        {
          int index = m_childNames.size();
          m_childNames.push_back( childName );
          connectChild( index, childItem );

          items.push_back( childItem );
        }
      }
    }
  }
  catch (FabricCore::Exception e)
  {
    const char* error = e.getDesc_cstr();
    printf( error );
  }
}

void RTValViewItem::UpdateWidget()
{
  FabricCore::RTVal desc = m_val.getDesc();
  QString str = desc.getStringCString();

  // We chew up tonnes of perf if we don't limit the length
  const int maxLen = 50;
  if (str.length() > maxLen)
    str.resize( maxLen );
  m_widget = new QLabel( str );
}

//////////////////////////////////////////////////////////
//
static BaseViewItem* CreateItem(
  QString const &name,
  QVariant const &value,
  FTL::JSONObject* /*metaData*/
  )
{
  if (value.type() != QVariant::UserType)
    return NULL;
  if (value.userType() != qMetaTypeId<FabricCore::RTVal>())
    return NULL;

  FabricCore::RTVal rtVal = value.value<FabricCore::RTVal>();
  if (rtVal.isObject() || rtVal.isStruct())
  {
    RTValViewItem* pViewItem = new RTValViewItem( QString( name ), rtVal );
    return pViewItem;
  }
  return NULL;
}

EXPOSE_VIEW_ITEM( RTValViewItem, CreateItem, 1 );
