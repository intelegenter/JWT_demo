#include "login_dialog.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QSpinBox>

LoginDialog::LoginDialog(bool networkSetup, QWidget *parent)
  : QDialog(parent),
    isNetworkSetup(networkSetup)
{
  SetupUI();
}

LoginDialog::~LoginDialog()
{
}

void LoginDialog::SetupUI()
{
  auto l = new QVBoxLayout(this);
  auto fl = new QFormLayout();
  using BTN = QDialogButtonBox::StandardButton;
  auto buttons = new QDialogButtonBox(BTN::Ok | BTN::Cancel);
  LoginEdit = new QLineEdit("");
  PasswordEdit = new QLineEdit("");
  IPEdit = new QLineEdit("");
  PortEdit = new QSpinBox();
  PortEdit->setMaximum(65535);
  PortEdit->setMinimum(1);

  fl->addRow(tr("login:"), LoginEdit);
  fl->addRow(tr("password:"), PasswordEdit);
  if(isNetworkSetup)
  {
    fl->addRow(tr("ip:"), IPEdit);
    fl->addRow(tr("port:"), PortEdit);
  }
  l->addLayout(fl);
  l->addWidget(buttons);

  connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
  connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
}

bool LoginDialog::IsNetworkSetup() const
{
  return isNetworkSetup;
}

QString LoginDialog::login()
{
  return LoginEdit->text();
}

QString LoginDialog::password()
{
  return PasswordEdit->text();
}

QString LoginDialog::ipAdress()
{
  return IPEdit->text();
}

uint LoginDialog::port()
{
  return PortEdit->value();
}

