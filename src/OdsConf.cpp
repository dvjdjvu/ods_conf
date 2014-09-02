#include "OdsConf.h"

#include <Kernel/OdsInterface.h>

#include <QThread>
#include <QString>
#include <QDebug>

#include <QDomDocument>
#include <QDomElement>
#include <qt4/QtCore/qglobal.h>
#include <DictManager>
#include <Dictionary>
#include <Entry>
#include <Errors>
#include <DictManager.h>
#include <Entry.h>

OdsConf::OdsConf()
{
    
}

void
OdsConf::init(QString db_ip, QString db_name, QString db_login, QString db_pass, QString scheme_name, QString type_name)
{
    this->db_ip = db_ip;
    this->db_name = db_name;
    this->db_login = db_login;
    this->db_pass = db_pass;
    this->scheme_name = scheme_name;
    this->type_name = type_name;   
}

OdsConf::~OdsConf()
{

}

bool 
OdsConf::addRecord(QString task, QString value, QString key)
{   
    do {
        IObjectCursor cursor = this->ioMgr.getIObjects(this->scheme_name + "." + this->type_name);
        if (!cursor.isValid()) {
            break;
        }
        
        IObjectCursor::iterator it = cursor.begin();
        while (it != cursor.end()) {
            IObject obj = *it;
            if (obj.getStringAttr("Задача") == task && obj.getStringAttr("Ключ") == key) {            
                return false;
            }
            it++;
        }
        
        // Добавление записи.
        IObject obj = this->ioMgr.createIObject(this->scheme_name + "." + this->type_name);
        obj.setAttr("Задача", task);
        obj.setAttr("Значение", value);
        obj.setAttr("Ключ", key);
        return this->ioMgr.saveIObject(obj);
        
    } while(0);
    
    return false;
}

bool 
OdsConf::addSpecialRecord(QString task, QString value, QString key, ByteArray specialValue, QString specialType)
{   
    do {
        IObjectCursor cursor = this->ioMgr.getIObjects(this->scheme_name + "." + this->type_name);
        if (!cursor.isValid()) {
            break;
        }
        
        IObjectCursor::iterator it = cursor.begin();
        while (it != cursor.end()) {
            IObject obj = *it;
            if (obj.getStringAttr("Задача") == task && obj.getStringAttr("Ключ") == key) {            
                return false;
            }
            it++;
        }
        
        // Добавление записи.
        IObject obj = this->ioMgr.createIObject(this->scheme_name + "." + this->type_name);
        obj.setAttr("Задача", task);
        obj.setAttr("Значение", value);
        obj.setAttr("Ключ", key);
        obj.setAttr("Специальное значение", specialValue);
        obj.setAttr("Специальный тип", specialType);
        return this->ioMgr.saveIObject(obj);
        
    } while(0);
    
    return false;
}

bool 
OdsConf::delTask(QString task)
{
    QString del = "\"Задача\" = '" + task + "'";
    return this->ioMgr.deleteIObjectByCondition(this->scheme_name + "." + this->type_name, del);
}

bool 
OdsConf::delTaskKey(QString task, QString key)
{
    QString del = "\"Задача\" = '" + task + "' and \"Ключ\" = '" + key + "'";
    return this->ioMgr.deleteIObjectByCondition(this->scheme_name + "." + this->type_name, del);
}

bool 
OdsConf::delAll()
{
    
    IObjectCursor cursor = this->ioMgr.getIObjects(this->scheme_name + "." + this->type_name);
    if (!cursor.isValid()) {
        return false;
    }
        
    IObjectCursor::iterator it = cursor.begin();
    while (it != cursor.end()) {
        IObject obj = *it;
        this->ioMgr.deleteIObject(obj, IObject::Default);
        it++;
    }
    
    return true;
}

bool 
OdsConf::updateTaskKey(QString task, QString value, QString key)
{    
    do {
        IObjectCursor cursor = this->ioMgr.getIObjects(this->scheme_name + "." + this->type_name);
        if (!cursor.isValid()) {
            break;
        }
        
        IObjectCursor::iterator it = cursor.begin();
        while (it != cursor.end()) {
            IObject obj = *it;
            if (obj.getStringAttr("Задача") == task && obj.getStringAttr("Ключ") == key) {            
                obj.setAttr("Задача", task);
                obj.setAttr("Значение", value);
                obj.setAttr("Ключ", key);
                this->ioMgr.updateIObject(obj);
                return true;
            }
            it++;
        }
    } while(0);
    
    return false;
}

bool 
OdsConf::updateSpecialValue(QString task, QString value, QString key, ByteArray specialValue, QString specialType)
{    
    do {
        IObjectCursor cursor = this->ioMgr.getIObjects(this->scheme_name + "." + this->type_name);
        if (!cursor.isValid()) {
            break;
        }
        
        IObjectCursor::iterator it = cursor.begin();
        while (it != cursor.end()) {
            IObject obj = *it;
            if (obj.getStringAttr("Задача") == task && obj.getStringAttr("Ключ") == key) {            
                obj.setAttr("Задача", task);
                obj.setAttr("Значение", value);
                obj.setAttr("Ключ", key);
                obj.setAttr("Специальное значение", specialValue);
                obj.setAttr("Специальный тип", specialType);
                this->ioMgr.updateIObject(obj);
                return true;
            }
            it++;
        }
    } while(0);
    
    return false;
}

QString
OdsConf::getReplaceVal(ODS::OdsInterface* odsIface, QString scheme_name, QString type_name, QString val, QString field_repl)
{
    QString replace = "";
    Dict::DictManager dm(odsIface);    
    QList<Dict::Entry> entries = dm.getEntries(scheme_name, type_name, QString("\"Краткое наименование\" = '%1' OR \"Полное наименование\" = '%1'").arg(val));
    
    foreach(const Dict::Entry &entry, entries) {
        replace += entry.attrValue(field_repl);
        break;
    }
    
    return replace;
    
    /* ******************************************************************* 
    QString replace = "";
    
    do {
        
        ODS::IObjectManager ioMgr = odsIface->iobjectManager();
        if (!ioMgr.isValid())
        {
            break;
        }
        
        IObjectCursor cursor = ioMgr.getIObjects(scheme_name + "." + type_name);
        if (!cursor.isValid()) {
            break;
        }
        
        IObjectCursor::iterator it = cursor.begin();
        while (it != cursor.end()) {
            IObject obj = *it;
            if (obj.getStringAttr("Полное наименование") == val || obj.getStringAttr("Краткое наименование") == val) {
                replace = obj.getStringAttr(field_repl);
                break;
            }
            it++;
        }
    } while(0);
    return replace;
    */
}

QStringList 
OdsConf::getAll()
{
    QStringList All;
    
    do {
        IObjectCursor cursor = this->ioMgr.getIObjects(this->scheme_name + "." + this->type_name);
        if (!cursor.isValid()) {
            break;
        }
        
        QString::number(cursor.size());
        
        IObjectCursor::iterator it = cursor.begin();
        while (it != cursor.end()) {
            IObject obj = *it;
            All << ("{\"task:\" \"" + obj.getStringAttr("Задача") + "\", \"val:\" \"" + obj.getStringAttr("Значение") + "\", \"key:\" \"" + obj.getStringAttr("Ключ") + "\"}"); 
            it++;
        }
    } while(0);
    
    return All;
}
    
QStringList 
OdsConf::getTask(QString task)
{
    QStringList Task;
    
    do {
        IObjectCursor cursor = this->ioMgr.getIObjects(this->scheme_name + "." + this->type_name);
        if (!cursor.isValid()) {
            break;
        }
        
        QString::number(cursor.size());
        
        IObjectCursor::iterator it = cursor.begin();
        while (it != cursor.end()) {
            IObject obj = *it;
            if (obj.getStringAttr("Задача") == task) {
                Task << ("{\"task\": \"" + obj.getStringAttr("Задача") + "\", \"val\": \"" + obj.getStringAttr("Значение") + "\", \"key\": \"" + obj.getStringAttr("Ключ") + "\"}"); 
            }
            it++;
        }
    } while(0);
    
    return Task;
}

QString 
OdsConf::getTaskKey(QString task, QString key)
{
    do {
        IObjectCursor cursor = this->ioMgr.getIObjects(this->scheme_name + "." + this->type_name);
        if (!cursor.isValid()) {
            break;
        }
        
        QString::number(cursor.size());
        
        IObjectCursor::iterator it = cursor.begin();
        while (it != cursor.end()) {
            IObject obj = *it;
            if (obj.getStringAttr("Задача") == task && obj.getStringAttr("Ключ") == key) {
                return obj.getStringAttr("Значение");
            }
            it++;
        }
    } while(0);
    
    return "";
}

QStringList 
OdsConf::getTaskList()
{
    QStringList taskList;
    do {
        IObjectCursor cursor = this->ioMgr.getIObjects(this->scheme_name + "." + this->type_name);
        if (!cursor.isValid()) {
            break;
        }
        
        QString::number(cursor.size());
        
        IObjectCursor::iterator it = cursor.begin();
        while (it != cursor.end()) {
            IObject obj = *it;
            
            taskList << "\"" + obj.getStringAttr("Задача") + "\"";
            
            it++;
        }
    } while(0);
    
    taskList.sort();
    taskList.removeDuplicates();
    
    return taskList;
}

bool
OdsConf::isconnect()
{
    return this->odsIfaceMgr.isConnected();
}

bool
OdsConf::disconnect()
{
    return this->odsIfaceMgr.disconnect();
}

bool
OdsConf::connect(ODS::OdsInterface* odsIface)
{
    do {

        this->odsIface = odsIface;
        this->odsIfaceMgr = this->odsIface->connectionManager();
        
        qsrand((uint) getpid());
        QString prog_id = "ods_conf" + QString::number(qrand());
        
        if (!this->odsIfaceMgr.connect(this->db_login, this->db_pass, prog_id, this->db_ip, this->db_name))
        {
            break;
        }
        
        IObjectScheme scheme =  this->odsIface->structureManager().getScheme(this->scheme_name);
        if (!scheme.isValid())
        {
            break;
        }
        
        IObjectType type = scheme.getType(this->type_name);
        if (!type.isValid())
        {
            break;
        }
        
        this->ioMgr = this->odsIface->iobjectManager();
        if (!this->ioMgr.isValid())
        {
            break;
        }
        
        return true;
    } while (0);
    
    return false;
}

