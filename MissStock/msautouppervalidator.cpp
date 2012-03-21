#include "StdAfx.h"
#include "msautouppervalidator.h"

MsAutoUpperValidator::MsAutoUpperValidator(QObject *parent)
    : QValidator(parent)
{

}

MsAutoUpperValidator::~MsAutoUpperValidator()
{

}

QValidator::State MsAutoUpperValidator::validate( QString & input, int & pos ) const
{
    input = input.toUpper();
    return QValidator::Acceptable;
}
