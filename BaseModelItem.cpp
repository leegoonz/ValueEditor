#include "stdafx.h"
#include "BaseModelItem.h"

BaseModelItem::BaseModelItem()
{
}


BaseModelItem::~BaseModelItem()
{
}


BaseModelItem* BaseModelItem::GetChild( QString childName )
{
  int res = ChildIndex( childName );
  if (res >= 0)
    return GetChild( res );
  return NULL;
}

QString BaseModelItem::ChildName( int i )
{
  BaseModelItem* pChild = GetChild( i );
  if (pChild != NULL)
    return pChild->GetName();
  return QString();
}

int BaseModelItem::ChildIndex( QString childName )
{
  for (int i = 0; i < NumChildren(); i++)
  {
    if (ChildName( i ) == childName)
      return i;
  }
  return -1;
}

ItemMetadata* BaseModelItem::GetMetadata()
{
  return NULL;
}

int BaseModelItem::GetInOut()
{
  return FabricCore::DFGPortType_In;
}

// Include MOC'ed file here, in order
// to support PCH on windows.
#include "moc_BaseModelItem.cpp"