#include <QLabel>
#include <QtCore/QDebug>
#include <QtWebSockets/QWebSocket>
#include <QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtWidgets/QInputDialog>
#include <QtCore/QJsonArray>
#include <QtCore/QFile>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QSlider>
#include <QtWidgets/QPushButton>
#include <QCloseEvent>

#include "src/SVRLT.h"
#include "ImageCache.h"
#include "GPMDP.h"
#include "ui_GPMDP.h"

#define Name DisplayName

GPMDP::GPMDP(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GPMDP)
{
    CurrentTrack = new Track;

    ui->setupUi(this);

    coverCache = new ImageCache("Covers");
    connect(coverCache, &ImageCache::ReceiveFinished, this, &GPMDP::CoverCacheFinished);

    //Temp UI
    QPushButton *rewind = new QPushButton;
    rewind->setText("Rewind");
    connect(rewind, &QPushButton::clicked, this, &GPMDP::Rewind);

    QPushButton *playpause = new QPushButton;
    playpause->setText("Play/Pause");
    connect(playpause, &QPushButton::clicked, this, &GPMDP::PlayPause);

    QPushButton *next = new QPushButton;
    next->setText("Next");
    connect(next, &QPushButton::clicked, this, &GPMDP::Forward);

    timeSlider = new QSlider(Qt::Orientation::Horizontal);
    //Should pause the music first then change the time but for now it works
    connect(timeSlider, &QSlider::sliderMoved, this, &GPMDP::SetCurrentTime);

    label = new QLabel(SongLyrics);
    image = new QLabel();
    ui->verticalLayout_2->addWidget(image);
    ui->verticalLayout_2->addWidget(label);
    ui->verticalLayout_2->addWidget(timeSlider);

    QHBoxLayout *hbox = new QHBoxLayout();
    hbox->addWidget(rewind);
    hbox->addWidget(playpause);
    hbox->addWidget(next);

    ui->verticalLayout_2->addLayout(hbox);


    webSocket = new QWebSocket("", QWebSocketProtocol::VersionLatest, nullptr);

    connect(webSocket, &QWebSocket::connected, this, &GPMDP::onConnected);
    connect(webSocket, &QWebSocket::disconnected, this, &GPMDP::closed);

    webSocket->open(QUrl("ws://127.0.0.1:5672"));
}

GPMDP::~GPMDP(){
    coverCache->SaveCache();
    delete coverCache;
    delete ui;
}

void GPMDP::closed() {
    qDebug() << "Closed";
}

void GPMDP::onConnected(){
    qDebug() << "Connected!";
    connect(webSocket, &QWebSocket::textMessageReceived, this, &GPMDP::onMessage);

    QJsonArray carray;
    carray.append(Name);

    QFile file("token.json");
    if(file.open(QIODevice::ReadOnly)){
        QTextStream stream(&file);

        QJsonDocument doc = QJsonDocument::fromJson(stream.readAll().toStdString().c_str());
        if(doc["token"].isString()){
            carray.append(doc["token"].toString());
        }
        file.close();
    }

    QJsonObject json;
    json["namespace"] = "connect";
    json["method"] = "connect";
    json["arguments"] = carray;

    webSocket->sendTextMessage((new QJsonDocument(json))->toJson());
}

void GPMDP::onMessage(QString message){
    QJsonDocument doc = QJsonDocument::fromJson(message.toStdString().c_str());
    if(doc["channel"].isNull()) return;

    QString chnl = doc["channel"].toString()
            .replace('-', "")
            .replace(':', '_');

    std::string buffer("Channel_");
    buffer.append(chnl.toStdString());

    const char* method = buffer.c_str();

    QMetaObject::invokeMethod(this, method, Qt::DirectConnection, Q_ARG(QString, message));

    //This is for temp, we should be updating these values in their respective channels.

    timeSlider->setMinimum(0);
    timeSlider->setMaximum(SongTotalTime);
    timeSlider->setValue(SongCurrentTime);

    label->setText(CurrentTrack->ToString());
}

void GPMDP::Channel_connect(const QString &message) {
    QJsonDocument doc = QJsonDocument::fromJson(message.toStdString().c_str());
    if(doc["payload"].isString()){
        QString payload = doc["payload"].toString();
        if(payload == "CODE_REQUIRED"){
            //We need to get the code from the user
            QString scode = QInputDialog::getText(this, "GPMDP", "Please give the code thats in GPMDP");

            QJsonArray app;
            app.append(Name);
            app.append(scode);

            QJsonObject json;
            json["namespace"] = "connect";
            json["method"] = "connect";
            json["arguments"] = app;

            expectingCodeNext = true;

            QJsonDocument doc(json);
            QString sj = doc.toJson(QJsonDocument::JsonFormat::Compact);
            qDebug().noquote() << sj;
            webSocket->sendTextMessage(sj);
        }else{
            if(expectingCodeNext){
                //its prob the code
                //the payload is our code
                qDebug() << payload;

                QFile file("token.json");
                if(file.open(QIODevice::ReadWrite)){
                    file.resize(0);
                    QTextStream stream(&file);


                    QJsonObject json;
                    json["token"] = payload;

                    QJsonDocument doc(json);
                    stream << doc.toJson();
                    file.close();
                }
                fullyRegistered = true;
                expectingCodeNext = false;
            }
        }
    }
}

void GPMDP::Channel_playState(const QString &message) {
    QJsonDocument doc = QJsonDocument::fromJson(message.toStdString().c_str());
    if(doc["payload"].isBool()){
        IsPlaying = doc["payload"].toBool(false);
    }
}

void GPMDP::Channel_track(const QString &message) {
    QJsonDocument doc = QJsonDocument::fromJson(message.toStdString().c_str());
    if(doc["payload"].isObject()){
        //Would be nice if I could do something like C# aka
        // payload.ToObject<Track>();
        //but for now it works :|
        QJsonObject pay = doc["payload"].toObject();

        CurrentTrack->Id = pay["id"].toString();
        CurrentTrack->Index = pay["index"].toInt();
        CurrentTrack->Title = pay["title"].toString();
        CurrentTrack->Artist = pay["artist"].toString();
        CurrentTrack->Album = pay["album"].toString();
        CurrentTrack->AlbumArt = pay["albumArt"].toString();
        CurrentTrack->Duration = pay["duration"].toInt();
        CurrentTrack->PlayCount = pay["playCount"].toInt();

        if(!CurrentTrack->AlbumArt.isEmpty()){
            qDebug() << "Sending request for image: " << CurrentTrack->AlbumArt;
            coverCache->getData(CurrentTrack->AlbumArt);
        }
    }
}

void GPMDP::Channel_lyrics(const QString &message) {
    QJsonDocument doc = QJsonDocument::fromJson(message.toStdString().c_str());
    if(!doc["payload"].isNull()){
        SongLyrics = doc["payload"].toString();
    }
}

void GPMDP::Channel_time(const QString &message) {
    QJsonDocument doc = QJsonDocument::fromJson(message.toStdString().c_str());
    if(!doc["payload"].isNull()){
        QJsonObject lol = doc["payload"].toObject();
        SongCurrentTime = lol["current"].toInt();
        SongTotalTime = lol["total"].toInt();
    }
}

void GPMDP::SendRPC(const QString &Namespace, const QString &Method, const QJsonArray &Arguments) {

    QJsonObject json;
    json["namespace"] = Namespace;
    json["method"] = Method;
    json["arguments"] = Arguments;

    webSocket->sendTextMessage((new QJsonDocument(json))->toJson());
}

void GPMDP::CoverCacheFinished(const QByteArray& data){
    QPixmap pixmap;
    pixmap.loadFromData(data);
    image->setPixmap(pixmap);
}

void GPMDP::closeEvent(QCloseEvent *event) {
    coverCache->SaveCache();
    event->accept();
}
