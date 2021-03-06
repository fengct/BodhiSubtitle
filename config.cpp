﻿#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <wchar.h>

#include "config.h"
#include "qfile.h"
#include "qdir.h"
#include "qfileinfo.h"
#include "qjsonobject.h"
#include "qjsonarray.h"
#include "qjsonvalue.h"
#include "qjsondocument.h"
#include "const.h"
#pragma execution_character_set("utf-8")

static QString s_timeFormat = QObject::tr("yyyy-M-d h:mm:ss");
static QString s_shortTimeFormat = QObject::tr("h:mm:ss");
static QString s_hisFileName = QObject::tr("history");

Work::Work()
    : type(WT_UNKOWN)
    , lastIndex(-1)
{
}

#define TAKE_STR(json, name, receiver) {\
    if (!json.contains(name)) break; \
    QJsonValue val = json.take(name); \
    if (!val.isString()) break; \
    qDebug() << "name: " << name << ", value: " << val.toString(); \
    receiver = val.toString(); }

#define TAKE_INT(json, name, receiver) {\
    if (!json.contains(name)) break; \
    QJsonValue val = json.take(name); \
    if (!val.isDouble()) break; \
    qDebug() << "name: " << name << ", value: " << val.toInt(); \
    receiver = val.toInt(); }

QJsonObject* Work2Json(const Work &w, QJsonObject *json)
{
    json->insert("SoundFile", w.soundFile);
    json->insert("SubtitleFile", w.subtitleFile);
    json->insert("Type", (int)w.type);
//    json->insert("FirstTime", w.firstTime.toString(s_timeFormat));
//    json->insert("LastTime", w.lastTime.toString(s_timeFormat));
//    json->insert("UsedTime", w.usedTime.toString(s_shortTimeFormat));
    json->insert("LastIndex", w.lastIndex);
    return json;
}

bool WorkFromJson(QJsonObject &json, Work *w)
{
    QJsonValue val;
    QString sval;
    int ival;
    do{
        TAKE_STR(json, "SoundFile", w->soundFile);
        TAKE_STR(json, "SubtitleFile", w->subtitleFile);
        TAKE_INT(json, "Type", ival);
        w->type = (WorkType)ival;
//        TAKE_STR(json, "FirstTime", sval);
//        w->firstTime = QDateTime::fromString(sval, s_timeFormat);
//        TAKE_STR(json, "LastTime", sval);
//        w->lastTime = QDateTime::fromString(sval, s_timeFormat);
//        TAKE_STR(json, "UsedTime", sval);
//        w->usedTime = QTime::fromString(sval, s_shortTimeFormat);
        TAKE_INT(json, "LastIndex", w->lastIndex)
        return true;
    }while(0);
    return false;
}

QString getEnvironVariant(const QString &name, const QString &defaultVal)
{
    QString val;
    wchar_t buf[MAX_PATH];
    DWORD rt = GetEnvironmentVariableW(name.toStdWString().data(), buf, sizeof(buf));
    if (rt == 0){
        qWarning() << "GetEnvironmentVariable of " << name << " failed. error=" << ::GetLastError();
        val = defaultVal;
    }else{
        val = QString::fromWCharArray(buf, rt);
    }
    return val;
}

QString Config::getCfgDir()
{
    QString path = getEnvironVariant("LOCALAPPDATA", QDir::currentPath());
    qDebug() << "Checking 'LOCALAPPDATA' ... got path=" << path;
    if (!path.endsWith("\\"))
        path += "\\";
    path += APP_NAME;
    //path = path.replace("\\", "-");
    qDebug() << "Config dir: " << path;
    return path;
}

bool Config::isFirstTimeRun()
{
    QDir dir(getCfgDir());
    return !dir.exists();
}

bool Config::saveHistory()
{
    QString path = getCfgDir();
    if (path.isEmpty()){
        //qWarning() << "Save history failed!";
        return false;
    }
    QFile doc(path + "\\" + s_hisFileName);
    if (!doc.open(QIODevice::WriteOnly | QIODevice::Text)){
        qWarning() << "Open file " << path << " failed!" << ::GetLastError();
        return false;
    }

    QJsonObject json;
    QJsonArray jsArray;
    json.insert("WorkDir", m_workDir);
    //json.insert("HisRecordsCount", m_history.size());
    WorkList::const_iterator it = m_history.begin();
    for(; it != m_history.end(); it++){
        const Work &w = *it;
        QJsonObject jsObj;
        Work2Json(w, &jsObj);
        jsArray.append(jsObj);
    }
    json.insert("History", jsArray);
    QJsonDocument jd(json);
    QByteArray ba = jd.toJson();
    qint64 sz = doc.write(ba);
    qDebug() << "ba length=" << ba.length() << ", sz=" << sz;
    return ba.length() == sz;
}

bool Config::readHistory()
{
    QString path = getCfgDir();
    if (path.isEmpty()){
        qWarning() << "Read history failed!";
        return false;
    }

    path += '\\';
    path += s_hisFileName;
    qInfo() << "Reading history file: " << path;
    QFile doc(path);
    if (!doc.open(QIODevice::ReadOnly | QIODevice::Text)){
        qWarning() << "Open file failed!";
        return false;
    }

    QByteArray ba = doc.readAll();
    QJsonParseError jerr;
    QJsonDocument jd = QJsonDocument::fromJson(ba, &jerr);
    if (jerr.error != QJsonParseError::NoError){
        qWarning() << "Parse JSON error: " << jerr.errorString();
        return false;
    }

    if (!jd.isObject()){
        qWarning() << "Unexpecte file format: JsonObject expectd!";
        return false;
    }

    QJsonObject json = jd.object();
    QString sval;
    bool error = true;
    do{
        TAKE_STR(json, "WorkDir", m_workDir);
        error = false;
    }while(0);
    if (error)
        return false;

    QJsonValue hisRecords = json.take("History");
    if (!hisRecords.isArray()){
        qWarning() << "No history found!";
        return true;
    }
    QJsonArray ja = hisRecords.toArray();
    QJsonArray::const_iterator it = ja.begin();
    Work w;
    for(; it != ja.end(); it++){
        QJsonValue &jval = *it;
        if (!jval.isObject()){
            qWarning() << "Unexpecte file format: JsonObject expectd!";
            break;
        }
        QJsonObject jobj = jval.toObject();
        if (!WorkFromJson(jobj, &w)){
            qWarning() << "WorkFromJson failed.";
            break;
        }
        m_history.push_back(w);
    }
    return true;
}

void Config::addHistory(Work &w)
{
    WorkList::iterator it = m_history.begin();
    for(; it!=m_history.end(); it++){
        Work &work = *it;
        if (work.subtitleFile == w.subtitleFile){
            m_history.erase(it);
            break;
        }
    }
    m_history.push_front(w);
}

QString getMatchedPath(const QString &path)
{
    QFileInfo fi(path);
    if (!fi.isFile())
        return "";

    QString p = path.mid(0, path.lastIndexOf('.'));
    QString ext = fi.suffix().toLower();
    if (ext == "srt"){
        p += ".mp3";
    } else {
        p += ".srt";
    }
    qDebug() << "matched path for [" << path << "] is [" << p << "]";
    return p;
}
