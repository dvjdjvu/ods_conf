#ifndef ODS_CONF_H
#define ODS_CONF_H

#include <QObject>
#include <QMap>
#include <QStringList>
#include <QRegExp>

#include <OdsAppSrvInterface.h>
#include "serializer.h"
#include "parser.h"

#include <IObjectScheme>

using namespace ODS;

class OdsConf : public QObject
{
    Q_OBJECT

public:
    OdsConf();
    ~OdsConf();
    
    void init(QString db_ip, QString db_name, QString db_login, QString db_pass, QString scheme_name, QString type_name);
    
    bool addRecord(QString task, QString value, QString key);
    bool addSpecialRecord(QString task, QString value, QString key, ByteArray specialValue, QString specialType);
    bool delTask(QString task);
    bool delTaskKey(QString task, QString key);
    bool delAll();
    
    bool updateTaskKey(QString task, QString value, QString key);
    bool updateSpecialValue(QString task, QString value, QString key, ByteArray specialValue, QString specialType);
    
    QString getReplaceVal(ODS::OdsInterface* odsIface, QString scheme_name, QString type_name, QString val, QString field_repl);
    QStringList getAll();
    IObjectCursor getSpecialAll();
    QStringList getTask(QString task);
    QString getTaskKey(QString task, QString key);
    QStringList getTaskList();
    
    bool connect(ODS::OdsInterface* odsIface);
    bool isconnect();
    bool disconnect();

private:
    ODS::OdsInterface* odsIface;
    ODS::ConnectionManager odsIfaceMgr;
    ODS::IObjectManager ioMgr;
    
    QMap<QString, QString> conf;

    QJson::Parser parser;
    QJson::Serializer serializer;

    QVariantMap query;
    QVariantMap response;
    QVariantMap error;
    
    QString db_ip;
    QString db_name;
    QString db_login;
    QString db_pass;
    QString scheme_name;
    QString type_name;
};

#endif // ODS_CONF_H
