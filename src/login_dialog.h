#ifndef LOGIN_DIALOG_H
#define LOGIN_DIALOG_H

#include <QDialog>
#include <QPointer>

class QLineEdit;
class QSpinBox;
class LoginDialog : public QDialog
{
  Q_OBJECT

public:
  LoginDialog(bool networkSetup = false, QWidget *parent = nullptr);
  ~LoginDialog();
  QString login();
  QString password();
  QString ipAdress();
  uint port();

  bool IsNetworkSetup() const;

private:
  void SetupUI();

  QPointer<QLineEdit> LoginEdit;
  QPointer<QLineEdit> PasswordEdit;
  QPointer<QLineEdit> IPEdit;
  QPointer<QSpinBox> PortEdit;

  bool isNetworkSetup;
};
#endif // LOGIN_DIALOG_H
