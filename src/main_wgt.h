#ifndef MAINWGT_H
#define MAINWGT_H

#include <QWidget>
#include <QPointer>
#include <QMap>
#include <QNetworkReply>

class QLineEdit;
class QPlainTextEdit;
class MainWgt : public QWidget
{
  Q_OBJECT
public:
  explicit MainWgt(QString user, QString pass, QString IP, uint port, QWidget *parent = nullptr);

protected:
  void SetupUI();

  //REST API
  void accessPublicData();
  void accessProtectedData();
  void refresh();
  void authorize();
  void relogin();

  //console prints
  void printStatus(const QNetworkReply &r);
  void printMessage(const QJsonDocument &data);
  void printReplyHandle(QString header, QNetworkReply* r, QJsonDocument data);
  void printErrorHandle(QString header, QNetworkReply* r, QNetworkReply::NetworkError e);

  QString User, Pass, RefreshToken;
  QMap<QString, QString> Headers;
  QPointer<QPlainTextEdit> ConsoleText;

};

#endif // MAINWGT_H
