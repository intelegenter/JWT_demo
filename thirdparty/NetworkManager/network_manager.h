#pragma once

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QByteArray>

#include "thirdparty/NetworkManager/api_rest.h"

////////////////////////////////////////////////////
//// Network Manager
class NetworkManager
{
protected:
  static NetworkManager* _Instance;
  NetworkManager();
  ~NetworkManager();

  void ConnectReply(QNetworkReply* reply
                    , API::REST::ReplyHandleFunc handle
                    , API::REST::ReplyErrorFunc error);

  void PrecessQueued();

public:
  //// Server address
  void SetHost(const QString& host, quint16 port);

  //// Authorize
  void Authorize(const QString& username, const QString& password
                    , API::REST::ReplyHandleFunc handle
                    , API::REST::ReplyErrorFunc error);

  //// Get instance
  static NetworkManager* Get();

  //// Get
  QNetworkReply* GetRequest(const QString& url
                    , const QUrlQuery& params
                    , API::REST::ReplyHandleFunc handle
                    , API::REST::ReplyErrorFunc error
                    , QVariant tag = QVariant()
                    , Qt::ConnectionType connectionType = Qt::AutoConnection);
  //// Get
  QNetworkReply* GetRequest(const QString& url
                            , QNetworkRequest r
                            , API::REST::ReplyHandleFunc handle
                            , API::REST::ReplyErrorFunc error
                            , QVariant tag = QVariant()
                                             , Qt::ConnectionType connectionType = Qt::AutoConnection);

  //// Post / url
  QNetworkReply* PostRequest(const QString& url
                    , const QUrlQuery& params
                    , API::REST::ReplyHandleFunc handle
                    , API::REST::ReplyErrorFunc error
                    , QVariant tag = QVariant()
                    , Qt::ConnectionType connectionType = Qt::AutoConnection);
  
  //// Post / data
  QNetworkReply* PostRequest(const QString& url
                    , const QJsonObject& data
                    , API::REST::ReplyHandleFunc handle
                    , API::REST::ReplyErrorFunc error
                    , QVariant tag = QVariant()
                    , Qt::ConnectionType connectionType = Qt::AutoConnection);

  //// Post / data & raw file data
  QNetworkReply* PostRequest(const QString& url
                    , const QJsonObject& data
                    , const QByteArray& file
                    , API::REST::ReplyHandleFunc handle
                    , API::REST::ReplyErrorFunc error
                    , QVariant tag = QVariant()
                    , Qt::ConnectionType connectionType = Qt::AutoConnection);  

  //// Put / url
  QNetworkReply* PutRequest(const QString& url
                    , const QUrlQuery& params
                    , API::REST::ReplyHandleFunc handle
                    , API::REST::ReplyErrorFunc error
                    , QVariant tag = QVariant()
                    , Qt::ConnectionType connectionType = Qt::AutoConnection);

  //// Put / data
  QNetworkReply* PutRequest(const QString& url
                    , const QJsonObject& data
                    , API::REST::ReplyHandleFunc handle
                    , API::REST::ReplyErrorFunc error
                    , QVariant tag = QVariant()
                    , Qt::ConnectionType connectionType = Qt::AutoConnection);

protected:
  //// Server address
  QString Host;
  QString Port;

  //// Qt connectivity
  QScopedPointer<QNetworkAccessManager> Manager;
  
  //// Default strategy
  Qt::ConnectionType ConnectionType;

  //// Queue
  enum RequestFields
  {
    RequestFieldType,
    RequestFieldUrl,
    RequestFieldQuery,
    RequestFieldData,
    RequestFieldBlob,
    RequestFieldHandle,
    RequestFieldError,
    RequestFieldReply,
    RequestFieldTag
  };

  //// Request type
  enum RequestType
  {
    RequestTypeGet,
    RequestTypePost,
    RequestTypePostData,
    RequestTypePostDataAndFile,
    RequestTypePut,
    RequestTypePutData,
  };

  //// Quued request 
  typedef std::tuple<RequestType, QString, QUrlQuery, QJsonObject, QByteArray, API::REST::ReplyHandleFunc, API::REST::ReplyErrorFunc, QNetworkReply*, QVariant> QueuedRequest;

  //// Queue of requests
  QList<QSharedPointer<QueuedRequest>> Queue;
};
