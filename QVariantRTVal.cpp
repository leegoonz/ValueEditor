#include "StdAfx.h"
#include "QVariantRTVal.h"


const QVariant::Handler* RTVariant::origh = NULL;

void RTVariant::injectRTHandler()
{
  origh = handler;
  Handler* h = new Handler;
  *h = *origh;
  h->isNull = rtIsNull;
  h->canConvert = rtCanConvert;
  h->convert = rtConvert;
  h->debugStream = rtStreamDebug;
  handler = h;
}

bool isRTVal( const QVariant::Private *d )
{
  static const int rtType = qMetaTypeId<FabricCore::RTVal>();
  return d->type >= QVariant::UserType;
}

bool RTVariant::rtIsNull( const QVariant::Private *d )
{
  if (isRTVal(d))
  {
    const FabricCore::RTVal& val = value( d );
    if (!val.isValid() || (val.isObject() && val.isNullObject()))
      return true;
    return false;
  }
  return origh->isNull( d );
}

bool RTVariant::rtCanConvert( const QVariant::Private *d, Type t )
{
  if (isRTVal( d ))
  {
    const FabricCore::RTVal& val = value( d );
    const char* rtype = val.getTypeNameCStr();

    switch (t)
    {
      case QVariant::Bool:
        return strcmp( rtype, "Boolean" ) == 0;
      case QVariant::Int:
        return strcmp( rtype, "SInt32" ) == 0 ||
          strcmp( rtype, "SInt16" ) == 0 ||
          strcmp( rtype, "SInt8" ) == 0;
      case QVariant::UInt:
        return strcmp( rtype, "UInt32" ) == 0 ||
          strcmp( rtype, "UInt16" ) == 0 ||
          strcmp( rtype, "byte" ) == 0 ||
          strcmp( rtype, "Size" ) == 0 ||
          strcmp( rtype, "Count" ) == 0 ||
          strcmp( rtype, "Index" ) == 0 ||
          strcmp( rtype, "UInt8" ) == 0;
      case QVariant::LongLong:
        return strcmp( rtype, "SInt64" ) == 0;
      case QVariant::ULongLong:
        return strcmp( rtype, "UInt64" ) == 0;
      case QVariant::Double:
        strcmp( rtype, "Float32" ) == 0 ||
          strcmp( rtype, "Float64" ) == 0 ||
          strcmp( rtype, "Scalar" ) == 0;
      case QVariant::String:
        return true; // Anything can be converted to a string
      case QVariant::Color:
        return strcmp( rtype, "RGB" ) == 0 ||
          strcmp( rtype, "RGBA" ) == 0 ||
          strcmp( rtype, "Color" ) == 0;
      case QVariant::Matrix:
        return strcmp( rtype, "Mat22" ) == 0;
      case QVariant::Transform:
        return strcmp( rtype, "Mat33" ) == 0;
      case QVariant::Matrix4x4:
        return strcmp( rtype, "Mat44" ) == 0;
      case QVariant::Vector2D:
        return strcmp( rtype, "Vec2" ) == 0;
      case QVariant::Vector3D:
        return strcmp( rtype, "Vec3" ) == 0;
      case QVariant::Vector4D:
        return strcmp( rtype, "Vec4" ) == 0;
      case QVariant::Quaternion:
        return strcmp( rtype, "Quat" ) == 0;

        // For these more complex types, 
        // a user should directly work with
        // the RTVal rather than incur the cost
        // of copying to QType

      case QVariant::SizeF:
      case QVariant::Char:
      case QVariant::Map:
      case QVariant::List:
      case QVariant::StringList:
      case QVariant::ByteArray:
      case QVariant::BitArray:
      case QVariant::Date:
      case QVariant::Time:
      case QVariant::DateTime:
      case QVariant::Url:
      case QVariant::Locale:
      case QVariant::Rect:
      case QVariant::RectF:
      case QVariant::Size:
      case QVariant::Line:
      case QVariant::LineF:
      case QVariant::Point:
      case QVariant::PointF:
      case QVariant::RegExp:
      case QVariant::Hash:
      case QVariant::EasingCurve:
      case QVariant::Font:
      case QVariant::Pixmap:
      case QVariant::Brush:
      case QVariant::Palette:
      case QVariant::Icon:
      case QVariant::Image:
      case QVariant::Polygon:
      case QVariant::Region:
      case QVariant::Bitmap:
      case QVariant::Cursor:
      case QVariant::SizePolicy:
      case QVariant::KeySequence:
      case QVariant::Pen:
      case QVariant::TextLength:
      case QVariant::TextFormat:
        return false;
      default:
        assert( !"Unknown type" );
    }
  }
  return origh->canConvert( d, t );
}

bool RTVariant::rtConvert( const QVariant::Private *d, QVariant::Type t, void *result, bool *ok )
{
  //qDebug() << Q_FUNC_INFO << "type:" << d->type;
  if (isRTVal( d ))
  {
    const FabricCore::RTVal& val = value( d );
    const char* rtype = val.getTypeNameCStr();
    switch (t)
    {
      case QVariant::Bool:
        if (strcmp( rtype, "Boolean" ))
        {
          bool& b = *((bool*)result);
          b = val.getBoolean();
        }
        break;
      case QVariant::Int:
      {
        int& v = *((int*)result);
        if (strcmp(rtype, "SInt32") == 0)
        {
          v = val.getSInt32();
        }
        else if (strcmp(rtype, "SInt16") == 0)
        {
          v = val.getSInt16();
        }
        else if (strcmp(rtype, "SInt8") == 0)
        {
          v = val.getSInt8();
        }
      }
      break;
      case QVariant::UInt:
      {
        unsigned int& v = *((unsigned int*)result);
        if (strcmp(rtype, "UInt32") == 0)
        {
          v = val.getUInt32();
        }
        else if (strcmp(rtype, "UInt16") == 0)
        {
          v = val.getUInt16();
        }
        else if (strcmp(rtype, "UInt8") == 0)
        {
          v = val.getUInt8();
        }
        else if (strcmp( rtype, "byte" ) == 0)
        {
          v = val.getUInt8();
        }
        else if (strcmp( rtype, "Size" ) == 0)
        {
          v = val.getUInt32();
        }
        else if (strcmp( rtype, "Count" ) == 0)
        {
          v = val.getUInt32();
        }
        else if (strcmp( rtype, "Index" ) == 0)
        {
          v = val.getUInt32();
        }
      }
      break;
      case QVariant::LongLong:
        if ( rtype, "SInt64" )
        {
          long long int& v = *((long long int*)result);
          v = val.getSInt64();
        }
        break;
      case QVariant::ULongLong:
      {
        unsigned long long int& v = *((unsigned long long int*)result);
        if (strcmp(rtype, "UInt64") == 0)
        {
          v = val.getUInt64();
        }
      }
      break;
      case QVariant::Double:
      {
        double& v = *((double*)result);
        if (strcmp( rtype, "Float32" ) == 0)
        {
          v = val.getFloat32();
        }
        else if (strcmp( rtype, "Float64" ) == 0)
        {
          v = val.getFloat64();
        }
        else if (strcmp( rtype, "Scalar" ) == 0)
        {
          v = val.getFloat32();
        }
      }
      break;
      case QVariant::String:
      {
        QString& str = *((QString*)result);
        if (strcmp( rtype, "String" ) == 0)
          str = val.getStringCString();
        else
        {
          FabricCore::RTVal desc = val.getDesc();
          str = desc.getStringCString();
        }
        break;
      }
      case QVariant::Color:
      {
        QColor& v = *((QColor*)result);
        if (strcmp( rtype, "RGB" ) == 0)
        {
          v.setRed( val.maybeGetMember( "r" ).getUInt8() );
          v.setGreen( val.maybeGetMember( "g" ).getUInt8() );
          v.setBlue( val.maybeGetMember( "b" ).getUInt8());
        }
        else if (strcmp( rtype, "RGBA" ) == 0)
        {
          v.setRed( val.maybeGetMember( "r" ).getUInt8() );
          v.setGreen( val.maybeGetMember( "g" ).getUInt8() );
          v.setBlue( val.maybeGetMember( "b" ).getUInt8() );
          v.setAlpha( val.maybeGetMember( "a" ).getUInt8() );
        }
        else if (strcmp( rtype, "Color" ) == 0)
        {
          v.setRedF( val.maybeGetMember( "r" ).getFloat32() );
          v.setGreenF( val.maybeGetMember( "g" ).getFloat32() );
          v.setBlueF( val.maybeGetMember( "b" ).getFloat32() );
          v.setAlphaF( val.maybeGetMember( "a" ).getFloat32() );
        }
      }
      break;
      //case QVariant::Matrix:
      //  if ( strcmp( rtype, "Mat22" ) == 0 )
      //  {
      //    QMatrix& v = *((QMatrix*)result);
      //    FabricCore::RTVal row0 = val.maybeGetMemberRef( "row0" );
      //    v.m11 = row0.maybeGetMember( "x" ).getFloat32();
      //    v.m12 = row0.maybeGetMember( "y" ).getFloat32();

      //    FabricCore::RTVal row1 = val.maybeGetMemberRef( "row1" );
      //    v.m21 = row1.maybeGetMember( "x" ).getFloat32();
      //    v.m22 = row1.maybeGetMember( "y" ).getFloat32();
      //  }
      //  break;
      case QVariant::Transform:
        if ( strcmp( rtype, "Mat33" ) == 0 )
        {
          QTransform& v = *((QTransform*)result);
          FabricCore::RTVal row0 = val.maybeGetMemberRef( "row0" );
          FabricCore::RTVal row1 = val.maybeGetMemberRef( "row1" );
          FabricCore::RTVal row2 = val.maybeGetMemberRef( "row2" );
          v.setMatrix(
            row0.maybeGetMember( "x" ).getFloat32(),
            row0.maybeGetMember( "y" ).getFloat32(),
            row0.maybeGetMember( "z" ).getFloat32(),

            row1.maybeGetMember( "x" ).getFloat32(),
            row1.maybeGetMember( "y" ).getFloat32(),
            row1.maybeGetMember( "z" ).getFloat32(),

            row2.maybeGetMember( "x" ).getFloat32(),
            row2.maybeGetMember( "y" ).getFloat32(),
            row2.maybeGetMember( "z" ).getFloat32()
            );
        }
        break;
      case QVariant::Matrix4x4:
        if ( strcmp( rtype, "Mat44" ) == 0 )
        {
          QMatrix4x4& v = *((QMatrix4x4*)result);
          FabricCore::RTVal row0 = val.maybeGetMemberRef( "row0" );
          FabricCore::RTVal row1 = val.maybeGetMemberRef( "row1" );
          FabricCore::RTVal row2 = val.maybeGetMemberRef( "row2" );
          FabricCore::RTVal row3 = val.maybeGetMemberRef( "row3" );

          v(0, 0) = row0.maybeGetMember( "x" ).getFloat32();
          v(0, 1) = row0.maybeGetMember( "y" ).getFloat32();
          v(0, 2) = row0.maybeGetMember( "z" ).getFloat32();
          v(0, 3) = row0.maybeGetMember( "w" ).getFloat32();

          v(1, 0) = row1.maybeGetMember( "x" ).getFloat32();
          v(1, 1) = row1.maybeGetMember( "y" ).getFloat32();
          v(1, 2) = row1.maybeGetMember( "z" ).getFloat32();
          v(1, 3) = row1.maybeGetMember( "w" ).getFloat32();

          v( 2, 0 ) = row2.maybeGetMember( "x" ).getFloat32();
          v( 2, 1 ) = row2.maybeGetMember( "y" ).getFloat32();
          v( 2, 2 ) = row2.maybeGetMember( "z" ).getFloat32();
          v( 2, 3 ) = row2.maybeGetMember( "w" ).getFloat32();

          v( 3, 0 ) = row3.maybeGetMember( "x" ).getFloat32();
          v( 3, 1 ) = row3.maybeGetMember( "y" ).getFloat32();
          v( 3, 2 ) = row3.maybeGetMember( "z" ).getFloat32();
          v( 3, 3 ) = row3.maybeGetMember( "w" ).getFloat32();
        }
        break;
      case QVariant::Vector2D:
        if ( strcmp( rtype, "Vec2" ) == 0 )
        {
          QVector2D& v = *((QVector2D*)result);
          v.setX(val.maybeGetMember( "x" ).getFloat32());
          v.setY(val.maybeGetMember( "y" ).getFloat32());
        }
        break;
      case QVariant::Vector3D:
        if ( strcmp( rtype, "Vec3" ) == 0 )
        {
          QVector3D& v = *((QVector3D*)result);
          v.setX(val.maybeGetMember( "x" ).getFloat32());
          v.setY(val.maybeGetMember( "y" ).getFloat32());
          v.setZ(val.maybeGetMember( "z" ).getFloat32());
        }
        break;
      case QVariant::Vector4D:
        if ( strcmp( rtype, "Vec4" ) == 0 )
        {
          QVector4D& v = *((QVector4D*)result);
          v.setX( val.maybeGetMember( "x" ).getFloat32() );
          v.setY( val.maybeGetMember( "y" ).getFloat32() );
          v.setZ( val.maybeGetMember( "z" ).getFloat32());
          v.setW( val.maybeGetMember( "w" ).getFloat32());
        }
        break;
      case QVariant::Quaternion:
        if ( strcmp( rtype, "Quat" ) == 0 )
        {
          QQuaternion& v = *((QQuaternion*)result);
          FabricCore::RTVal row = val.maybeGetMemberRef( "row0" );
          v.setX(row.maybeGetMember( "x" ).getFloat32());
          v.setY(row.maybeGetMember( "y" ).getFloat32());
          v.setZ(row.maybeGetMember( "z" ).getFloat32());
          v.setScalar(val.maybeGetMember( "w" ).getFloat32());
        }
        break;

  //  case QVariant::String:
  //  {
  //    QString& str = *((QString*)result);
  //    if (strcmp( rtype, "String" ) == 0)
  //      str = val.getStringCString();
  //    else
  //    {
  //      FabricCore::RTVal desc = val.getDesc();
  //      str = desc.getStringCString();
  //    }
  //    break;
  //  }
  //  case QVariant::Double:
  //  {
  //    double& dbl = *((double*)result);
  //    if (strcmp( rtype, "Float64" ) == 0)
  //      dbl = val.getFloat64();
  //    else
  //      dbl = val.getFloat32();
  //    break;
  //  }
  //  case QVariant::Vector3D:
  //  {
  //    QVector3D& vec = *((QVector3D*)result);
  //    if (strcmp( rtype, "Vec3" ) == 0)
  //    {
  //      vec.setX( val.maybeGetMember( "x" ).getFloat64() );
  //      vec.setY( val.maybeGetMember( "y" ).getFloat64() );
  //      vec.setZ( val.maybeGetMember( "z" ).getFloat64() );
  //    }
  //    break;
    }
  }
  // Apply default conversion
  else
    return origh->convert( d, t, result, ok );
  return true;
}

void RTVariant::rtStreamDebug( QDebug dbg, const QVariant &v )
{
  if (isRTVal( v ))
    dbg << v.value<QString>();
  else
    origh->debugStream( dbg, v );
}
