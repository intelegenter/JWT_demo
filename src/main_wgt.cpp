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
//  auto caption = new QLabel();
  auto fl = new QFormLayout();
  auto protectedBtn = new QPushButton(tr("Protected"));
  auto publicBtn = new QPushButton(tr("Public"));
  auto refreshBtn = new QPushButton(tr("Refresh"));
  auto reloginBtn = new QPushButton(tr("Relogin"));
  ProtectedLineEdit = new QLineEdit();
  PublicLineEdit = new QLineEdit();
  ConsoleText = new QPlainTextEdit();

//  l->addWidget(caption);
  fl->addRow(protectedBtn, ProtectedLineEdit);
  fl->addRow(publicBtn, PublicLineEdit);
  fl->addWidget(refreshBtn);
  fl->addWidget(reloginBtn);
  l->addLayout(fl);
  l->addWidget(ConsoleText);

  ProtectedLineEdit->setReadOnly(true);
  PublicLineEdit->setReadOnly(true);

  connect(protectedBtn, &QPushButton::clicked, this, &MainWgt::onProtectedBtnPressed);
  connect(publicBtn, &QPushButton::clicked, this, &MainWgt::onPublicBtnPressed);
  connect(refreshBtn, &QPushButton::clicked, this, &MainWgt::refresh);
  connect(reloginBtn, &QPushButton::clicked, this, &MainWgt::relogin);

  authorize();
}

void MainWgt::onPublicBtnPressed()
{
  API::REST::JWT_Public(Headers,
  [this](QNetworkReply* r, QJsonDocument data)
  {
    ConsoleText->appendPlainText(">>Protected>>");
    for(auto h : r->rawHeaderPairs())
    {
      ConsoleText->appendPlainText(h.first);
      ConsoleText->appendPlainText(h.second);
    }
    ConsoleText->appendPlainText(">>STATUS_CODE>>");
//    uint statusCode = r->attribute(QNetworkRequest::Attribute::HttpStatusCodeAttribute).toUInt();
    ConsoleText->appendPlainText(r->attribute(QNetworkRequest::Attribute::HttpStatusCodeAttribute).toString());

    if(!data.isEmpty())
    {
      ConsoleText->appendPlainText(">>MESSAGE>>");
      ConsoleText->appendPlainText(data["msg"].toString());
      PublicLineEdit->setText(data["msg"].toString());
    }

    ConsoleText->appendPlainText("\n");
  },
  [this](QNetworkReply* r, QNetworkReply::NetworkError e)
  {
    QMetaEnum en = QMetaEnum::fromType<QNetworkReply::NetworkError>();
    ConsoleText->appendPlainText(">>ERROR>>");
    ConsoleText->appendPlainText(en.valueToKey(e));
    ConsoleText->appendPlainText(">>STATUS_CODE>>");
    ConsoleText->appendPlainText(r->attribute(QNetworkRequest::Attribute::HttpStatusCodeAttribute).toString());
    ConsoleText->appendPlainText("\n");
  });
}

void MainWgt::onProtectedBtnPressed()
{
  API::REST::JWT_Protected(Headers,
  [this](QNetworkReply* r, QJsonDocument data)
  {
    ConsoleText->appendPlainText(">>Protected>>");
    for(auto h : r->rawHeaderPairs())
    {
      ConsoleText->appendPlainText(h.first);
      ConsoleText->appendPlainText(h.second);
    }
    ConsoleText->appendPlainText(">>STATUS_CODE>>");
    ConsoleText->appendPlainText(r->attribute(QNetworkRequest::Attribute::HttpStatusCodeAttribute).toString());

    if(!data.isEmpty())
    {
      ConsoleText->appendPlainText(">>MESSAGE>>");
      ConsoleText->appendPlainText(data["msg"].toString());
      ProtectedLineEdit->setText(data["msg"].toString());
    }

    ConsoleText->appendPlainText("\n");
  },
  [this](QNetworkReply* r, QNetworkReply::NetworkError e)
  {
    QMetaEnum en = QMetaEnum::fromType<QNetworkReply::NetworkError>();
    ConsoleText->appendPlainText(">>ERROR>>");
    ConsoleText->appendPlainText(en.valueToKey(e));
    ConsoleText->appendPlainText(">>STATUS_CODE>>");
    ConsoleText->appendPlainText(r->attribute(QNetworkRequest::Attribute::HttpStatusCodeAttribute).toString());
    ConsoleText->appendPlainText("\n");
  });
}

void MainWgt::refresh()
{
  API::REST::JWT_Refresh(Headers, RefreshToken,
  [this](QNetworkReply* r, QJsonDocument data)
  {
    ConsoleText->appendPlainText(">>Protected>>");
    for(auto h : r->rawHeaderPairs())
    {
      ConsoleText->appendPlainText(h.first);
      ConsoleText->appendPlainText(h.second);
    }
    ConsoleText->appendPlainText(">>STATUS_CODE>>");
    ConsoleText->appendPlainText(r->attribute(QNetworkRequest::Attribute::HttpStatusCodeAttribute).toString());

    if(!data.isEmpty())
    {
      ConsoleText->appendPlainText(">>MESSAGE>>");
      ConsoleText->appendPlainText(data["msg"].toString());
      ProtectedLineEdit->setText(data["msg"].toString());
    }

    ConsoleText->appendPlainText("\n");

    if(r->hasRawHeader("Authorization"))
    {
      Headers["Authorization"] = r->rawHeader("Authorization");
      ConsoleText->appendPlainText(Headers["Authorization"]);
    }
    if(r->hasRawHeader("Refresh"))
    {
      RefreshToken = r->rawHeader("Refresh");
      ConsoleText->appendPlainText(r->rawHeader("Refresh"));
    }
  },
  [this](QNetworkReply* r, QNetworkReply::NetworkError e)
  {
    QMetaEnum en = QMetaEnum::fromType<QNetworkReply::NetworkError>();
    ConsoleText->appendPlainText(">>ERROR>>");
    ConsoleText->appendPlainText(en.valueToKey(e));
    ConsoleText->appendPlainText(">>STATUS_CODE>>");
    ConsoleText->appendPlainText(r->attribute(QNetworkRequest::Attribute::HttpStatusCodeAttribute).toString());
    ConsoleText->appendPlainText("\n");
  });
}

void MainWgt::authorize()
{
  API::REST::JWT_Login(User, Pass,
  [this](QNetworkReply* r, QJsonDocument data)
  {
    ConsoleText->appendPlainText(">>Login>>");
    if(r->hasRawHeader("Authorization"))
    {
      Headers["Authorization"] = r->rawHeader("Authorization");
      ConsoleText->appendPlainText(Headers["Authorization"]);
    }
    if(r->hasRawHeader("Refresh"))
    {
      RefreshToken = r->rawHeader("Refresh");
      ConsoleText->appendPlainText(r->rawHeader("Refresh"));
    }

    if(!data.isEmpty())
    {
      ConsoleText->appendPlainText(">>MESSAGE>>");
      ConsoleText->appendPlainText(data["msg"].toString());
    }

    ConsoleText->appendPlainText("\n");
  },
  [this](QNetworkReply* r, QNetworkReply::NetworkError e)
  {
    QMetaEnum en = QMetaEnum::fromType<QNetworkReply::NetworkError>();
    ConsoleText->appendPlainText(">>ERROR>>");
    ConsoleText->appendPlainText(en.valueToKey(e));
    ConsoleText->appendPlainText(">>STATUS_CODE>>");
    ConsoleText->appendPlainText(r->attribute(QNetworkRequest::Attribute::HttpStatusCodeAttribute).toString());
    ConsoleText->appendPlainText("\n");
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
