#include "api_rest.h"
#include "network_manager.h"
#include <QUrlQuery>

////////////////////////////////////////////////////
//// API
namespace API
{
  ////////////////////////////////////////////////////
  //// REST API
  namespace REST
  {
    void JWT_Login(QString login, QString pass, ReplyHandleFunc handle, ReplyErrorFunc error)
    {
      QJsonObject obj;
      obj["username"] = login;
      obj["password"] = pass;
      NetworkManager::Get()->PostRequest("login"
                                         , obj
                                         , handle
                                         , error);
    }

    void JWT_Protected(QMap<QString, QString> headers, ReplyHandleFunc handle, ReplyErrorFunc error)
    {
      QNetworkRequest r;
      for(auto h : headers.keys())
      {
        r.setRawHeader(h.toUtf8(), headers[h].toUtf8());
      }
      NetworkManager::Get()->GetRequest("protected",
                                        r,
                                        handle,
                                        error);
    }

    void JWT_Public(QMap<QString, QString> headers, ReplyHandleFunc handle, ReplyErrorFunc error)
    {
      QNetworkRequest r;
      for(auto h : headers.keys())
      {
        r.setRawHeader(h.toUtf8(), headers[h].toUtf8());
      }
      NetworkManager::Get()->GetRequest("public",
                                        r,
                                        handle,
                                        error);
    }

    void JWT_Refresh(QMap<QString, QString> headers, QString refreshToken, ReplyHandleFunc handle, ReplyErrorFunc error)
    {
      QNetworkRequest r;
      for(auto h : headers.keys())
      {
        r.setRawHeader(h.toUtf8(), headers[h].toUtf8());
      }
      r.setRawHeader("Refresh", refreshToken.toUtf8());
      NetworkManager::Get()->GetRequest("refresh",
                                        r,
                                        handle,
                                        error);
    }

  }
}
