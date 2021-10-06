#include "login_dialog.h"
#include "main_wgt.h"

#include <QApplication>

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);

  LoginDialog loginDial(true);
  if(loginDial.exec() == QDialog::Rejected)
    return 0;

  MainWgt mainW(loginDial.login(),
                loginDial.password(),
                loginDial.ipAdress(),
                loginDial.port());
  mainW.show();

  return a.exec();
}
