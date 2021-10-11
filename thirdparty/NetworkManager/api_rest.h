#pragma once

#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QByteArray>
#include <QPoint>

////////////////////////////////////////////////////
//// API
namespace API
{
  ////////////////////////////////////////////////////
  //// REST API
  namespace REST
  {
    //// Handlers
    typedef std::function<void(QNetworkReply*, QJsonDocument)>               ReplyHandleFunc;
    typedef std::function<void(QNetworkReply*, QNetworkReply::NetworkError)> ReplyErrorFunc;

    ////JWT_demo
    void JWT_Login(QString login, QString pass, ReplyHandleFunc handle, ReplyErrorFunc error);
    void JWT_Protected(QMap<QString, QString> header, ReplyHandleFunc handle, ReplyErrorFunc error);
    void JWT_Public(QMap<QString, QString> headers, ReplyHandleFunc handle, ReplyErrorFunc error);
    void JWT_Refresh(QMap<QString, QString> headers, QString refreshToken, ReplyHandleFunc handle, ReplyErrorFunc error);
  }
}
