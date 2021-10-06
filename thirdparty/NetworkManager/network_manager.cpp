#include "network_manager.h"

#include <QJsonObject>
#include <QJsonDocument>
#include <QUrlQuery>
#include <QHttpMultiPart>

NetworkManager* NetworkManager::_Instance = nullptr;

NetworkManager::NetworkManager()
  : Manager(new QNetworkAccessManager())
  , ConnectionType(Qt::DirectConnection)
{
}

NetworkManager::~NetworkManager()
{
}

void NetworkManager::ConnectReply(QNetworkReply* reply
                                  , API::REST::ReplyHandleFunc handle
                                  , API::REST::ReplyErrorFunc error)
{
  auto onFinished = [this, reply, handle, error]()
  {
    if (reply->error() == QNetworkReply::NoError)
    {
      if (handle)
      {
        auto data = reply->readAll();
        handle(reply, QJsonDocument::fromJson(data));
      }
    }
    else
    {
      qDebug() << "Network error:" << reply->errorString();
      if (error)
        error(reply, reply->error());
    }
    reply->deleteLater();

    //// Remove from queue
    QList<QSharedPointer<QueuedRequest>>::iterator recieved = std::find_if(
      Queue.begin(),
      Queue.end(),
      [reply](QSharedPointer<QueuedRequest> request) -> bool { return std::get<RequestFieldReply>(*request) == reply; });
    
    //// Was queued?
    if (recieved != Queue.end())
    {
      //// Done
      Queue.removeOne(*recieved);

      //// Proceed
      PrecessQueued();
    }
  };
  QObject::connect(reply, &QNetworkReply::finished, onFinished);
}

void NetworkManager::PrecessQueued()
{
  if (!Queue.empty())
  {
    QList<QSharedPointer<QueuedRequest>>::iterator sent = std::find_if(
      Queue.begin(),
      Queue.end(), 
      [](QSharedPointer<QueuedRequest> request) -> bool { return std::get<RequestFieldReply>(*request); });
    
    QList<QSharedPointer<QueuedRequest>>::iterator unsent = std::find_if(
      Queue.begin(),
      Queue.end(),
      [](QSharedPointer<QueuedRequest> request) -> bool { return !std::get<RequestFieldReply>(*request); });

    if (sent == Queue.end()
      && unsent != Queue.end())
    {
      //// Send
      QNetworkReply* reply = nullptr;
      switch (std::get<RequestFieldType>(**unsent))
      {
      case RequestTypeGet:
        reply = GetRequest(
          std::get<RequestFieldUrl>(**unsent),
          std::get<RequestFieldQuery>(**unsent),
          std::get<RequestFieldHandle>(**unsent),
          std::get<RequestFieldError>(**unsent),
          std::get<RequestFieldTag>(**unsent),
          Qt::DirectConnection);
        break;
      case RequestTypePost:
        reply = PostRequest(
          std::get<RequestFieldUrl>(**unsent),
          std::get<RequestFieldQuery>(**unsent),
          std::get<RequestFieldHandle>(**unsent),
          std::get<RequestFieldError>(**unsent),
          std::get<RequestFieldTag>(**unsent),
          Qt::DirectConnection);
        break;
      case RequestTypePostData:
        reply = PostRequest(
          std::get<RequestFieldUrl>(**unsent),
          std::get<RequestFieldData>(**unsent),
          std::get<RequestFieldHandle>(**unsent),
          std::get<RequestFieldError>(**unsent),
          std::get<RequestFieldTag>(**unsent),
          Qt::DirectConnection);
        break;
      case RequestTypePostDataAndFile:
        reply = PostRequest(
          std::get<RequestFieldUrl>(**unsent),
          std::get<RequestFieldData>(**unsent),
          std::get<RequestFieldBlob>(**unsent),
          std::get<RequestFieldHandle>(**unsent),
          std::get<RequestFieldError>(**unsent),
          std::get<RequestFieldTag>(**unsent),
          Qt::DirectConnection);
        break;
      case RequestTypePut:
        reply = PutRequest(
          std::get<RequestFieldUrl>(**unsent),
          std::get<RequestFieldQuery>(**unsent),
          std::get<RequestFieldHandle>(**unsent),
          std::get<RequestFieldError>(**unsent),
          std::get<RequestFieldTag>(**unsent),
          Qt::DirectConnection);
        break;
      case RequestTypePutData:
        reply = PutRequest(
          std::get<RequestFieldUrl>(**unsent),
          std::get<RequestFieldData>(**unsent),
          std::get<RequestFieldHandle>(**unsent),
          std::get<RequestFieldError>(**unsent),
          std::get<RequestFieldTag>(**unsent),
          Qt::DirectConnection);
        break;
      default:
        break;
      }

      //// Mark
      std::get<RequestFieldReply>(**unsent) = reply;
    }
  }
}

void NetworkManager::SetHost(const QString &host, quint16 port)
{
  Host = host;
  Port = QString::number(port);
}

void NetworkManager::Authorize(const QString& username, const QString& password
                                 , API::REST::ReplyHandleFunc handle
                                 , API::REST::ReplyErrorFunc error)
{
  Manager->connectToHostEncrypted(Host, Port.toUInt());

  QString concatenated = username + ":" + password;
  QByteArray data = concatenated.toLocal8Bit().toBase64();
  QString headerData = "Basic " + data;
  QNetworkRequest request = QNetworkRequest(QUrl(QString("http://%1:%2").arg(Host, Port)));
  request.setRawHeader("Authorization", headerData.toLocal8Bit());

  ConnectReply(Manager->get(request), handle, error);
}

NetworkManager *NetworkManager::Get()
{
  if (!_Instance)
    _Instance = new NetworkManager;
  return _Instance;
}

//// Get
QNetworkReply* NetworkManager::GetRequest(const QString& url
                                , const QUrlQuery& params
                                , API::REST::ReplyHandleFunc handle
                                , API::REST::ReplyErrorFunc error
                                , QVariant tag
                                , Qt::ConnectionType connectionType)
{
  //// Resolve auto
  if (connectionType == Qt::AutoConnection)
  {
    connectionType = ConnectionType;
  }

  //// Immediate
  if (connectionType == Qt::DirectConnection)
  {
    QUrl u(QString("http://%1:%2/%3").arg(Host, Port, url));
    u.setQuery(params);
    QNetworkRequest request = QNetworkRequest(u);
    qDebug() << "GET" << u.toDisplayString(QUrl().FullyEncoded);
    
    QNetworkReply* reply = Manager->get(request);    
    ConnectReply(reply, handle, error);
    reply->setProperty("tag", tag);
    return reply;
  }

  //// Queued
  if (connectionType == Qt::QueuedConnection)
  {
    QSharedPointer<QueuedRequest> request(new QueuedRequest);
    std::get<RequestFieldType>(*request)    = RequestTypeGet;
    std::get<RequestFieldUrl>(*request)     = url;
    std::get<RequestFieldQuery>(*request)   = params;
    std::get<RequestFieldHandle>(*request)  = handle;
    std::get<RequestFieldError>(*request)   = error;
    std::get<RequestFieldReply>(*request)   = nullptr;
    std::get<RequestFieldTag>(*request)     = tag;

    Queue.push_back(request);
    PrecessQueued();
  }
  return nullptr;
}

QNetworkReply *NetworkManager::GetRequest(const QString &url, QNetworkRequest r, API::REST::ReplyHandleFunc handle, API::REST::ReplyErrorFunc error, QVariant tag, Qt::ConnectionType connectionType)
{
  //// Resolve auto
  if (connectionType == Qt::AutoConnection)
  {
    connectionType = ConnectionType;
  }

  //// Immediate
  if (connectionType == Qt::DirectConnection)
  {
    auto& request = r;
    request.setUrl(QString("http://%1:%2/%3").arg(Host, Port, url));

    QNetworkReply* reply = Manager->get(request);
    qDebug() << "GET_HEADER_AUTH:" << request.rawHeader("Authorization");
    ConnectReply(reply, handle, error);
    reply->setProperty("tag", tag);
    return reply;
  }

  //// Queued
  if (connectionType == Qt::QueuedConnection)
  {
    QSharedPointer<QueuedRequest> request(new QueuedRequest);
    std::get<RequestFieldType>(*request)    = RequestTypeGet;
    std::get<RequestFieldUrl>(*request)     = url;
    std::get<RequestFieldHandle>(*request)  = handle;
    std::get<RequestFieldError>(*request)   = error;
    std::get<RequestFieldReply>(*request)   = nullptr;
    std::get<RequestFieldTag>(*request)     = tag;

    Queue.push_back(request);
    PrecessQueued();
  }
  return nullptr;
}

//// Post / url
QNetworkReply* NetworkManager::PostRequest(const QString& url
                                  , const QUrlQuery& params
                                  , API::REST::ReplyHandleFunc handle
                                  , API::REST::ReplyErrorFunc error
                                  , QVariant tag
                                  , Qt::ConnectionType connectionType)
{
  //// Resolve auto
  if (connectionType == Qt::AutoConnection)
  {
    connectionType = ConnectionType;
  }

  //// Immediate
  if (connectionType == Qt::DirectConnection)
  {
    QNetworkRequest request = QNetworkRequest(QString("http://%1:%2/%3").arg(Host, Port, url));
    qDebug() << "POST" << request.url().toDisplayString(QUrl().FullyEncoded);
    
    QNetworkReply* reply = Manager->post(request, params.toString(QUrl().FullyEncoded).toUtf8());
    ConnectReply(reply, handle, error);
    reply->setProperty("tag", tag);
    return reply;
  }

  //// Queued
  if (connectionType == Qt::QueuedConnection)
  {
    QSharedPointer<QueuedRequest> request(new QueuedRequest);
    std::get<RequestFieldType>(*request)    = RequestTypePost;
    std::get<RequestFieldUrl>(*request)     = url;
    std::get<RequestFieldQuery>(*request)   = params;
    std::get<RequestFieldHandle>(*request)  = handle;
    std::get<RequestFieldError>(*request)   = error;
    std::get<RequestFieldReply>(*request)   = nullptr;
    std::get<RequestFieldTag>(*request)     = tag;

    Queue.push_back(request);
    PrecessQueued();
  }
  return nullptr;
}

//// Post / data
QNetworkReply* NetworkManager::PostRequest(const QString& url
                                 , const QJsonObject& data
                                 , API::REST::ReplyHandleFunc handle
                                 , API::REST::ReplyErrorFunc error
                                 , QVariant tag
                                 , Qt::ConnectionType connectionType)
{
  //// Resolve auto
  if (connectionType == Qt::AutoConnection)
  {
    connectionType = ConnectionType;
  }

  //// Immediate
  if (connectionType == Qt::DirectConnection)
  {
    QNetworkRequest request = QNetworkRequest(QString("http://%1:%2/%3").arg(Host, Port, url));
    request.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/json"));
    qDebug() << "POST" << request.url().toDisplayString(QUrl().FullyEncoded);
    
    QNetworkReply* reply = Manager->post(request, QJsonDocument(data).toJson(QJsonDocument::Compact));
    ConnectReply(reply, handle, error);
    reply->setProperty("tag", tag);
    return reply;
  }

  //// Queued
  if (connectionType == Qt::QueuedConnection)
  {
    QSharedPointer<QueuedRequest> request(new QueuedRequest);
    std::get<RequestFieldType>(*request)    = RequestTypePostData;
    std::get<RequestFieldUrl>(*request)     = url;
    std::get<RequestFieldData>(*request)    = data;
    std::get<RequestFieldHandle>(*request)  = handle;
    std::get<RequestFieldError>(*request)   = error;
    std::get<RequestFieldReply>(*request)   = nullptr;
    std::get<RequestFieldTag>(*request)     = tag;

    Queue.push_back(request);
    PrecessQueued();
  }
  return nullptr;
}

//// Post / data & raw file data
QNetworkReply* NetworkManager::PostRequest(const QString& url
                          , const QJsonObject& data
                          , const QByteArray& file
                          , API::REST::ReplyHandleFunc handle
                          , API::REST::ReplyErrorFunc error
                          , QVariant tag
                          , Qt::ConnectionType connectionType)
{
  //// Resolve auto
  if (connectionType == Qt::AutoConnection)
  {
    connectionType = ConnectionType;
  }

  //// Immediate
  if (connectionType == Qt::DirectConnection)
  {
    //// Request
    QNetworkRequest request = QNetworkRequest(QString("http://%1:%2/%3").arg(Host, Port, url));

    //// As multipart
    QHttpMultiPart* multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    //// Json parts
    {
      for (auto j = data.begin(); j != data.end(); j++)
      {
        QHttpPart jsonPart;
        jsonPart.setHeader(QNetworkRequest::ContentDispositionHeader,
          QVariant(QString("form-data; name=\"%1\"").arg(j.key())));
        jsonPart.setHeader(QNetworkRequest::ContentTypeHeader,
          QVariant("text/plain"));

        jsonPart.setBody(QByteArray::fromStdString(j.value().toString().toStdString()));
        multiPart->append(jsonPart);
      }
    }

    //// File part
    QHttpPart filePart;
    filePart.setHeader(QNetworkRequest::ContentDispositionHeader, 
      QVariant(QString("form-data; name=\"cdf\"; filename=\"") + data["file_name"].toString() + "\""));
    filePart.setHeader(QNetworkRequest::ContentTypeHeader, 
      QVariant("application/octet-stream"));
    filePart.setBody(file);
    multiPart->append(filePart);

    //// Send
    qDebug() << "POST" << request.url().toDisplayString(QUrl().FullyEncoded);
    
    QNetworkReply* reply = Manager->post(request, multiPart);
    multiPart->setParent(reply);
    ConnectReply(reply, handle, error);
    reply->setProperty("tag", tag);
    return reply;
  }

  //// Queued
  if (connectionType == Qt::QueuedConnection)
  {
    QSharedPointer<QueuedRequest> request(new QueuedRequest);
    std::get<RequestFieldType>(*request)    = RequestTypePostDataAndFile;
    std::get<RequestFieldUrl>(*request)     = url;
    std::get<RequestFieldData>(*request)    = data;
    std::get<RequestFieldBlob>(*request)    = file;
    std::get<RequestFieldHandle>(*request)  = handle;
    std::get<RequestFieldError>(*request)   = error;
    std::get<RequestFieldReply>(*request)   = nullptr;
    std::get<RequestFieldTag>(*request)     = tag;

    Queue.push_back(request);
    PrecessQueued();
  }
  return nullptr;
}

//// Put / url
QNetworkReply* NetworkManager::PutRequest(const QString& url
                                , const QUrlQuery& params
                                , API::REST::ReplyHandleFunc handle
                                , API::REST::ReplyErrorFunc error
                                , QVariant tag
                                , Qt::ConnectionType connectionType)
{
  //// Resolve auto
  if (connectionType == Qt::AutoConnection)
  {
    connectionType = ConnectionType;
  }

  //// Immediate
  if (connectionType == Qt::DirectConnection)
  {
    QNetworkRequest request = QNetworkRequest(QString("http://%1:%2/%3").arg(Host, Port, url));
    qDebug() << "PUT" << request.url().toDisplayString(QUrl().FullyEncoded);
    
    QNetworkReply* reply = Manager->put(request, params.toString(QUrl().FullyEncoded).toUtf8());
    ConnectReply(reply, handle, error);
    reply->setProperty("tag", tag);
    return reply;
  }

  //// Queued
  if (connectionType == Qt::QueuedConnection)
  {
    QSharedPointer<QueuedRequest> request(new QueuedRequest);
    std::get<RequestFieldType>(*request)    = RequestTypePut;
    std::get<RequestFieldUrl>(*request)     = url;
    std::get<RequestFieldQuery>(*request)   = params;
    std::get<RequestFieldHandle>(*request)  = handle;
    std::get<RequestFieldError>(*request)   = error;
    std::get<RequestFieldReply>(*request)   = nullptr;
    std::get<RequestFieldTag>(*request)     = tag;

    Queue.push_back(request);
    PrecessQueued();
  }
  return nullptr;
}

//// Put / data
QNetworkReply* NetworkManager::PutRequest(const QString& url
                                , const QJsonObject& data
                                , API::REST::ReplyHandleFunc handle
                                , API::REST::ReplyErrorFunc error
                                , QVariant tag
                                , Qt::ConnectionType connectionType)
{
  //// Resolve auto
  if (connectionType == Qt::AutoConnection)
  {
    connectionType = ConnectionType;
  }

  //// Immediate
  if (connectionType == Qt::DirectConnection)
  {
    QNetworkRequest request = QNetworkRequest(QString("http://%1:%2/%3").arg(Host, Port, url));
    request.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/json"));
    qDebug() << "PUT" << request.url().toDisplayString(QUrl().FullyEncoded);
    
    QNetworkReply* reply = Manager->put(request, QJsonDocument(data).toJson(QJsonDocument::Compact));
    ConnectReply(reply, handle, error);
    reply->setProperty("tag", tag);
    return reply;
  }

  //// Queued
  if (connectionType == Qt::QueuedConnection)
  {
    QSharedPointer<QueuedRequest> request(new QueuedRequest);
    std::get<RequestFieldType>(*request)    = RequestTypePut;
    std::get<RequestFieldUrl>(*request)     = url;
    std::get<RequestFieldData>(*request)    = data;
    std::get<RequestFieldHandle>(*request)  = handle;
    std::get<RequestFieldError>(*request)   = error;
    std::get<RequestFieldReply>(*request)   = nullptr;
    std::get<RequestFieldTag>(*request)     = tag;

    Queue.push_back(request);
    PrecessQueued();
  }
  return nullptr;
}
