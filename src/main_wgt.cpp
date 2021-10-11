#include "main_wgt.h"
#include "login_dialog.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QMetaEnum>
#include <QLabel>
#include "thirdparty/NetworkManager/network_manager.h"


MainWgt::MainWgt(QString user, QString pass, QString IP, uint port, QWidget *parent)
  : QWidget(parent),
    User(user),
    Pass(pass)
{
  NetworkManager::Get()->SetHost(IP, port);
  SetupUI();
}

void MainWgt::SetupUI()
{
  auto l = new QVBoxLayout(this);
  auto buttonLayout = new QHBoxLayout();

  auto protectedBtn = new QPushButton(tr("Protected"));
  auto publicBtn = new QPushButton(tr("Public"));
  auto refreshBtn = new QPushButton(tr("Refresh"));
  auto reloginBtn = new QPushButton(tr("Relogin"));
  ConsoleText = new QPlainTextEdit();

  buttonLayout->addWidget(publicBtn);
  buttonLayout->addWidget(protectedBtn);
  buttonLayout->addWidget(refreshBtn);
  buttonLayout->addWidget(reloginBtn);
  buttonLayout->addSpacerItem(new QSpacerItem(1,1, QSizePolicy::Expanding));
  l->addLayout(buttonLayout);
  l->addWidget(ConsoleText);

  connect(protectedBtn, &QPushButton::clicked, this, &MainWgt::accessProtectedData);
  connect(publicBtn, &QPushButton::clicked, this, &MainWgt::accessPublicData);
  connect(refreshBtn, &QPushButton::clicked, this, &MainWgt::refresh);
  connect(reloginBtn, &QPushButton::clicked, this, &MainWgt::relogin);

  authorize();
}

void MainWgt::accessPublicData()
{
  API::REST::JWT_Public(Headers,
  [this](QNetworkReply* r, QJsonDocument data)
  {
    printReplyHandle(">>Public>>", r, data);
  },
  [this](QNetworkReply* r, QNetworkReply::NetworkError e)
  {
    printErrorHandle(">>Public>>", r, e);
  });
}

void MainWgt::accessProtectedData()
{
  API::REST::JWT_Protected(Headers,
  [this](QNetworkReply* r, QJsonDocument data)
  {
    printReplyHandle(">>Protected>>", r, data);
  },
  [this](QNetworkReply* r, QNetworkReply::NetworkError e)
  {
    printErrorHandle(">>Protected>>", r, e);
  });
}

void MainWgt::refresh()
{
  API::REST::JWT_Refresh(Headers, RefreshToken,
  [this](QNetworkReply* r, QJsonDocument data)
  {
    printReplyHandle(">>Refresh>>", r, data);

    if(r->hasRawHeader("Authorization"))
      Headers["Authorization"] = r->rawHeader("Authorization");
    if(r->hasRawHeader("Refresh"))
      RefreshToken = r->rawHeader("Refresh");
  },
  [this](QNetworkReply* r, QNetworkReply::NetworkError e)
  {
    printErrorHandle(">>Refresh>>", r, e);
  });
}

void MainWgt::authorize()
{
  API::REST::JWT_Login(User, Pass,
  [this](QNetworkReply* r, QJsonDocument data)
  {
    printReplyHandle(">>Authorize>>", r, data);

    if(r->hasRawHeader("Authorization"))
      Headers["Authorization"] = r->rawHeader("Authorization");
    if(r->hasRawHeader("Refresh"))
      RefreshToken = r->rawHeader("Refresh");
  },
  [this](QNetworkReply* r, QNetworkReply::NetworkError e)
  {
    printErrorHandle(">>Authorize>>", r, e);
  });
}

void MainWgt::relogin()
{
  LoginDialog d;
  if(d.exec())
  {
    User = d.login();
    Pass = d.password();
    authorize();
  }
}

void MainWgt::printStatus(const QNetworkReply &r)
{
  ConsoleText->appendPlainText(">>STATUS_CODE>>");
  ConsoleText->appendPlainText(r.attribute(QNetworkRequest::Attribute::HttpStatusCodeAttribute).toString());
}

void MainWgt::printMessage(const QJsonDocument &data)
{
  if(!data.isEmpty())
  {
    ConsoleText->appendPlainText(">>MESSAGE>>");
    ConsoleText->appendPlainText(data["msg"].toString());
  }
}

void MainWgt::printReplyHandle(QString header, QNetworkReply *r, QJsonDocument data)
{
  ConsoleText->appendPlainText(header);
  ConsoleText->appendPlainText(">>HEADERS>>");
  for(auto h : r->rawHeaderPairs())
  {
    ConsoleText->appendPlainText(h.first + " : " + h.second);
  }
  printStatus(*r);
  printMessage(data);
  ConsoleText->appendPlainText("\n");
}

void MainWgt::printErrorHandle(QString header, QNetworkReply *r, QNetworkReply::NetworkError e)
{
  ConsoleText->appendPlainText(header);
  QMetaEnum en = QMetaEnum::fromType<QNetworkReply::NetworkError>();
  ConsoleText->appendPlainText(">>ERROR>>");
  ConsoleText->appendPlainText(en.valueToKey(e));
  printStatus(*r);
  auto data = QJsonDocument::fromJson(r->readAll());
  printMessage(data);
  ConsoleText->appendPlainText("\n");
}
