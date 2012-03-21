#ifndef MISSQTEXCEL_H
#define MISSQTEXCEL_H

#include <QObject>

class MissQtExcel : public QObject
{
    Q_OBJECT
    class MissQtExcelImpl;
public:
    MissQtExcel(QObject *parent);
    ~MissQtExcel();

public:
    bool Init();
    bool Open(const QString& strFileName);
    bool NewBook();
    bool Save(const QString& strFileName);
    void ShowExcel();
    void OpenSheet(short sNo);
    void OpenSheet(const QString& strSheetName);
    QString GetValue();
    QString GetText();

    int GetUsedMaxRowCount();
    int GetUsedMaxColumnCount();

    void SetValue(const QVariant& strValue);
    void SetValue(int nValue);

    void SetRange(const QString& strCell);
    void SetRange(const QString& strCellBegin, const QString& strCellEnd);

    void AutoFit(bool bRow = false);
    void Merge(bool bAcross = false);
    void SetHorizontalAlignment(int nType = -4108);
    void SetVerticalAlignment(int nType = -4108);

public:
    static QString GetCellName(int nRow, int nColumn);
    static QString GetColumnName(int nColumn);

private:
    std::tr1::shared_ptr<MissQtExcelImpl> m_pImpl;
};

#endif // MISSQTEXCEL_H
