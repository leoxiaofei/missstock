#include "StdAfx.h"
#include "MissTools.h"
#include <QCryptographicHash>

QString MissTools::MD5( const QString& strSrc )
{
    return QCryptographicHash::hash(strSrc.toAscii(), QCryptographicHash::Md5).toHex().data();
}
