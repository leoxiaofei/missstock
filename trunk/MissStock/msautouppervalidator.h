#ifndef MSAUTOUPPERVALIDATOR_H
#define MSAUTOUPPERVALIDATOR_H

#include <QValidator>

class MsAutoUpperValidator : public QValidator
{
public:
    MsAutoUpperValidator(QObject *parent);
    ~MsAutoUpperValidator();
    virtual QValidator::State validate ( QString & input, int & pos ) const;
private:
    
};

#endif // MSAUTOUPPERVALIDATOR_H
