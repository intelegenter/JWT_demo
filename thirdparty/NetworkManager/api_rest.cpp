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
    //// Get chromotogram data
    void GetProjectFileCromotogramData(int id, ReplyHandleFunc handle, ReplyErrorFunc error)
    {
      NetworkManager::Get()->GetRequest(QString("imports/%1/chromatogram").arg(id)
                                        , QUrlQuery()
                                        , handle
                                        , error);
    }

    void GetProjectFileCromotogramPeakIntegration(int id, ReplyHandleFunc handle, ReplyErrorFunc error)
    {
      NetworkManager::Get()->GetRequest(QString("imports/%1/chromatogram/peak_integration").arg(id)
                                        , QUrlQuery()
                                        , handle
                                        , error);
    }

    void GetProjectFileCromotogramTableData(QJsonDocument markupJson, ReplyHandleFunc handle, ReplyErrorFunc error)
    {
      NetworkManager::Get()->PostRequest("indexing/table"
                                         , markupJson.object()
                                         , handle
                                         , error);
    }

    void GetIndexingVersions(int indexingId,
                             ReplyHandleFunc handle, ReplyErrorFunc error)
    {
      NetworkManager::Get()->GetRequest(QString("indexing/versions/%1").arg(indexingId)
                                        , QUrlQuery()
                                        , handle
                                        , error);
    }

    void GetIndexingVersionLast(int indexingId, ReplyHandleFunc handle, ReplyErrorFunc error)
    {
      NetworkManager::Get()->GetRequest(QString("indexing/versions/%1/last").arg(indexingId)
                                        , QUrlQuery()
                                        , handle
                                        , error);
    }

    void AddIndexing(int projectId, const QString& title, int userId,
                                ReplyHandleFunc handle, ReplyErrorFunc error)
    {
      QJsonObject obj;
      obj["project_id"] = projectId;
      obj["title"] = title;
      obj["user_id"] = userId;
      NetworkManager::Get()->PostRequest("indexing"
                                         , obj
                                         , handle
                                         , error);
    }

    void GetIndexingVersionMarkup(int projectId, int indexingId, int versionId,
                                       ReplyHandleFunc handle, ReplyErrorFunc error)
    {
      NetworkManager::Get()->GetRequest(QString("indexing/markup/%1/%2/%3").arg(projectId).arg(indexingId).arg(versionId)
                                        , QUrlQuery()
                                        , handle
                                        , error);
    }

    void SetIndexingVersionMarkup(QJsonObject data, ReplyHandleFunc handle, ReplyErrorFunc error)
    {
      NetworkManager::Get()->PostRequest(QString("indexing")
                                         , data
                                         , handle
                                         , error);
    }

    //// Upload file
    void UploadFile(const QString& fileName, int userId, int projectId, int typeId, const QByteArray& data, ReplyHandleFunc handle, ReplyErrorFunc error)
    {
      QJsonObject obj;
      obj["user_id"]    = QString::number(userId);
      obj["project_id"] = QString::number(projectId);
      obj["type_id"]    = QString::number(typeId);
      obj["file_name"]  = fileName;

      QString tag = fileName.split("/").last();
      NetworkManager::Get()->PostRequest("imports", obj, data, handle, error, tag, Qt::QueuedConnection);
    }

    void GetProjectFileCromotogramAnalysisPCA(QJsonObject tableDataJson, ReplyHandleFunc handle, ReplyErrorFunc error)
    {
      NetworkManager::Get()->PostRequest("indexing/pca"
                                         , tableDataJson
                                         , handle
                                         , error);
    }

    void GetProjectFileCromotogramAnalysisMCR(QJsonObject tableDataJson, ReplyHandleFunc handle, ReplyErrorFunc error)
    {
      NetworkManager::Get()->PostRequest("indexing/mcr"
                                         , tableDataJson
                                         , handle
                                         , error);
    }

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
