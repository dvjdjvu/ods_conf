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
OdsConf::init(QString db_ip, QString db_name, QString db_login, QString db_pass, String scheme_name, String type_name)
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
OdsConf::setRules(IObject iobj)
{
    if (!iobj.isValid()) {
        return false;
    }
    
    Subject user = this->subMgr.getCurrentUser();
    Subject grp  = this->subMgr.getGroup("$$$Все пользователи");
    Subject grp1 = this->subMgr.getGroup("$$$Системные администраторы");

    if (!user.isValid()) {
        return false;
    }
    
    if (!grp.isValid()) {
        return false;
    }
    
    if (!grp1.isValid()) {
        return false;
    }
    
    if (!grp1.setRightsToIObject(iobj, ODS::Right::Read)) {
        return false;
    }
                
    if (!grp.setRightsToIObject(iobj, ODS::Right::Read)) {
        return false;
    }
    
    if (!user.setRightsToIObject(iobj, ODS::Right::NoRight)) {
        return false;
    }
    
    return true;
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
            if (obj.getStringAttr("Задача").toQString() == task && obj.getStringAttr("Ключ").toQString() == key) {            
                return false;
            }
            it++;
        }
        
        // Добавление записи.
        IObject obj = this->ioMgr.createIObject(this->scheme_name + "." + this->type_name);
        obj.setAttr("Задача", task);
        obj.setAttr("Значение", value);
        obj.setAttr("Ключ", key);
        
        if (!this->ioMgr.saveIObject(obj)) {
            return false;
        }
        
        return setRules(obj);
        
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
            if (obj.getStringAttr("Задача").toQString() == task && obj.getStringAttr("Ключ").toQString() == key) {            
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
        
        if(!this->ioMgr.saveIObject(obj)) {
            return false;
        }
        
        return setRules(obj);
        
    } while(0);
    
    return false;
}

bool 
OdsConf::delTask(QString task)
{
    //QString del = QString("\"Задача\" = '") + task + QString("'");
    //return this->ioMgr.deleteIObjectByCondition(this->scheme_name + String(".") + this->type_name, String(del));
    
    do {
        IObjectCursor cursor = this->ioMgr.getIObjects(this->scheme_name + "." + this->type_name);
        if (!cursor.isValid()) {
            break;
        }
        
        IObjectCursor::iterator it = cursor.begin();
        while (it != cursor.end()) {
            IObject obj = *it;
            if (obj.getStringAttr("Задача").toQString() == task) {            
                this->ioMgr.deleteIObject(obj);
            }
            it++;
        }
    } while(0);
    
    return true;
}

bool 
OdsConf::delTaskKey(QString task, QString key)
{
    //QString del = "\"Задача\" = '" + task + "' and \"Ключ\" = '" + key + "'";
    //return this->ioMgr.deleteIObjectByCondition(this->scheme_name + "." + this->type_name, String(del));
          
    do {
        IObjectCursor cursor = this->ioMgr.getIObjects(this->scheme_name + "." + this->type_name);
        if (!cursor.isValid()) {
            break;
        }
        
        IObjectCursor::iterator it = cursor.begin();
        while (it != cursor.end()) {
            IObject obj = *it;
            if (obj.getStringAttr("Задача").toQString() == task && obj.getStringAttr("Ключ").toQString() == key) {            
                return this->ioMgr.deleteIObject(obj);
            }
            it++;
        }
    } while(0);
    
    return false;
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
            if (obj.getStringAttr("Задача").toQString() == task && obj.getStringAttr("Ключ").toQString() == key) {            
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
OdsConf::updateTaskKey(QString task, QString value, QString key_old, QString key_new)
{    
    do {
        IObjectCursor cursor = this->ioMgr.getIObjects(this->scheme_name + "." + this->type_name);
        if (!cursor.isValid()) {
            break;
        }
        
        IObjectCursor::iterator it = cursor.begin();
        while (it != cursor.end()) {
            IObject obj = *it;
            if (obj.getStringAttr("Задача").toQString() == task && obj.getStringAttr("Ключ").toQString() == key_old) {            
                obj.setAttr("Задача", task);
                obj.setAttr("Значение", value);
                obj.setAttr("Ключ", key_new);
                return this->ioMgr.updateIObject(obj);
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
            if (obj.getStringAttr("Задача").toQString() == task && obj.getStringAttr("Ключ").toQString() == key) {            
                obj.setAttr("Задача", task);
                obj.setAttr("Значение", value);
                obj.setAttr("Ключ", key);
                obj.setAttr("Специальное значение", specialValue);
                obj.setAttr("Специальный тип", specialType);
                return this->ioMgr.updateIObject(obj);
            }
            it++;
        }
    } while(0);
    
    return false;
}

bool 
OdsConf::updateSpecialValue(QString task, QString value, QString key_old, QString key_new, ByteArray specialValue, QString specialType)
{    
    do {
        IObjectCursor cursor = this->ioMgr.getIObjects(this->scheme_name + "." + this->type_name);
        if (!cursor.isValid()) {
            break;
        }
        
        IObjectCursor::iterator it = cursor.begin();
        while (it != cursor.end()) {
            IObject obj = *it;
            if (obj.getStringAttr("Задача").toQString() == task && obj.getStringAttr("Ключ").toQString() == key_old) {            
                obj.setAttr("Задача", task);
                obj.setAttr("Значение", value);
                obj.setAttr("Ключ", key_new);
                obj.setAttr("Специальное значение", specialValue);
                obj.setAttr("Специальный тип", specialType);
                return this->ioMgr.updateIObject(obj);
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
            All << ("{\"task:\" \"" + obj.getStringAttr("Задача").toQString() + "\", \"val:\" \"" + obj.getStringAttr("Значение").toQString() + "\", \"key:\" \"" + obj.getStringAttr("Ключ").toQString() + "\"}"); 
            it++;
        }
    } while(0);
    
    return All;
}

IObjectCursor
OdsConf::getSpecialAll()
{
    /*
    IObjectCursor cursor = this->ioMgr.getIObjects(this->scheme_name + "." + this->type_name);
    if (!cursor.isValid()) {
        
    }
    */  
    return this->ioMgr.getIObjects(this->scheme_name + "." + this->type_name);;
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
                Task << ("{\"task\": \"" + obj.getStringAttr("Задача").toQString() + "\", \"val\": \"" + obj.getStringAttr("Значение").toQString() + "\", \"key\": \"" + obj.getStringAttr("Ключ").toQString() + "\"}"); 
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
            if (obj.getStringAttr("Задача").toQString() == task && obj.getStringAttr("Ключ").toQString() == key) {
                return obj.getStringAttr("Значение").toQString();
            }
            it++;
        }
    } while(0);
    
    return "";
}

ByteArray
OdsConf::getSpecialValue(QString task, QString key)
{
    ByteArray byte;
    
    do {
        IObjectCursor cursor = this->ioMgr.getIObjects(this->scheme_name + "." + this->type_name);
        if (!cursor.isValid()) {
            break;
        }
        
        IObjectCursor::iterator it = cursor.begin();
        while (it != cursor.end()) {
            IObject obj = *it;
            if (obj.getStringAttr("Задача").toQString() == task && obj.getStringAttr("Ключ").toQString() == key) {
                return obj.getByteArray("Специальное значение");
            }
            it++;
        }
    } while(0);
    
    return byte;
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
            
            //taskList << "\"" + obj.getStringAttr("Задача") + "\"";
            taskList << obj.getStringAttr("Задача").toQString();
            
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
        if (odsIface == NULL) {
            this->odsIface = new OdsInterface();
        } else {
            this->odsIface = odsIface;
        }
        
        this->odsIfaceMgr = this->odsIface->connectionManager();
        
        if (this->odsIfaceMgr.isConnected()) {
            break;
        }
        
        qsrand((uint) getpid());
        QString prog_id = "ods_conf" + QString::number(qrand());
        
        if (!this->odsIfaceMgr.connect(this->db_login, this->db_pass, prog_id, this->db_ip, this->db_name))
        {
            return false;
        }
        /*
        IObjectScheme scheme =  this->odsIface->structureManager().getScheme(this->scheme_name);//"$$$Системные ИО");//this->scheme_name);
        if (!scheme.isValid())
        {
            break;
        }

        IObjectType type = scheme.getType(this->type_name);//"Настройка");//this->type_name);
        if (!type.isValid())
        {
            break;
        }
        */

    } while (0);
    
    this->ioMgr = this->odsIface->iobjectManager();
    if (!this->ioMgr.isValid())
    {
        return false;
    }
        
    this->subMgr = this->odsIface->subjectManager();
    if (!this->subMgr.isValid())
    {
        return false;
    }
    
    return true;
}

