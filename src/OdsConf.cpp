#include "OdsConf.h"

#include <Kernel/OdsInterface.h>

#include <QThread>
#include <QString>
#include <QDebug>

#include <QDomDocument>
#include <QDomElement>
#include <qt4/QtCore/qglobal.h>

OdsConf::OdsConf()
{
    
}

OdsConf::OdsConf(QString db_ip, QString db_name, QString db_login, QString db_pass, QString scheme_name, QString type_name)
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
                Task << ("{\"task:\" \"" + obj.getStringAttr("Задача") + "\", \"val:\" \"" + obj.getStringAttr("Значение") + "\", \"key:\" \"" + obj.getStringAttr("Ключ") + "\"}"); 
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

QString 
OdsConf::getURIFunct(QString uri)
{
    QRegExp rx("\\/(\\w+)[\\?|\\z]");
    
    int pos = rx.indexIn(uri);
    if (pos > -1) {
        return rx.cap(1);
    }
    
    return "/";
}

QString
OdsConf::getURIParam(const HttpRequest& _req, QString par)
{
    QHash<QString, QString> hash = _req.getParameters("utf8");
    QHash<QString, QString>::const_iterator i = hash.constBegin();
    while (i != hash.constEnd()) {
        if (i.key() == par) {
            return QString(i.value().toLocal8Bit());
        }    
        ++i;
    }
    
    return "";
}

bool
OdsConf::connect()
{
    do {

        this->odsIface = new OdsInterface();
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

