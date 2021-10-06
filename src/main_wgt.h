#ifndef MAINWGT_H
#define MAINWGT_H

#include <QWidget>
#include <QPointer>
#include <QMap>

class QLineEdit;
class QPlainTextEdit;
class MainWgt : public QWidget
{
  Q_OBJECT
public:
  explicit MainWgt(QString user, QString pass, QString IP, uint port, QWidget *parent = nullptr);

protected:
  void SetupUI();
  void onPublicBtnPressed();
  void onProtectedBtnPressed();
  void refresh();
  void authorize();
  void relogin();

  QString User, Pass, RefreshToken;

  QMap<QString, QString> Headers;

  QPointer<QLineEdit> ProtectedLineEdit,
                      PublicLineEdit;
  QPointer<QPlainTextEdit> ConsoleText;

};

#endif // MAINWGT_H
