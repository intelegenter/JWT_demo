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

    //// Get chromotogram data
    void GetProjectFileCromotogramData(int id, ReplyHandleFunc handle, ReplyErrorFunc error);
    void GetProjectFileCromotogramPeakIntegration(int id, ReplyHandleFunc handle, ReplyErrorFunc error);
    void GetProjectFileCromotogramTableData(QJsonDocument markupJson, ReplyHandleFunc handle, ReplyErrorFunc error);
    void GetProjectFileCromotogramAnalysisPCA(QJsonObject tableDataJson, ReplyHandleFunc handle, ReplyErrorFunc error);
    void GetProjectFileCromotogramAnalysisMCR(QJsonObject tableDataJson, ReplyHandleFunc handle, ReplyErrorFunc error);

    void GetIndexingVersions(int indexingId, ReplyHandleFunc handle, ReplyErrorFunc error);
    void GetIndexingVersionLast(int indexingId, ReplyHandleFunc handle, ReplyErrorFunc error);
    void AddIndexing(int projectId, const QString& title, int user_id, ReplyHandleFunc handle, ReplyErrorFunc error);
    void GetIndexingVersionMarkup(int projectId, int indexingId, int versionId, ReplyHandleFunc handle, ReplyErrorFunc error);
    void SetIndexingVersionMarkup(QJsonObject data, ReplyHandleFunc handle, ReplyErrorFunc error);

    ////JWT_demo
    void JWT_Login(QString login, QString pass, ReplyHandleFunc handle, ReplyErrorFunc error);
    void JWT_Protected(QMap<QString, QString> header, ReplyHandleFunc handle, ReplyErrorFunc error);
    void JWT_Public(QMap<QString, QString> headers, ReplyHandleFunc handle, ReplyErrorFunc error);
    void JWT_Refresh(QMap<QString, QString> headers, QString refreshToken, ReplyHandleFunc handle, ReplyErrorFunc error);

    //// Upload file
    void UploadFile(const QString& fileName, int userId, int projectId, int typeId, const QByteArray& data, ReplyHandleFunc handle, ReplyErrorFunc error);
  }
}
