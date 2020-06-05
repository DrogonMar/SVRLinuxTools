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
#include <QtConcurrent/QtConcurrent>
#include <QtWidgets/QMessageBox>

#include "src/SVRLT.h"
#include "src/Utils.h"
#include "ImageCache.h"
#include "GPMDP.h"
#include "ui_GPMDP.h"

//Make it easy to rename it.
#define ClientName DisplayName
#define TokenJson  Utils::GetSettingsDir() + "token.json"
#define CheckPayload if(doc["payload"].isNull()) return;

GPMDP::GPMDP(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::GPMDP)
{
    m_CurrentTrack = *new GStructs::GTrack;

    //region Temp UI
    ui->setupUi(this);

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

    label = new QLabel();
    label->setAlignment(Qt::AlignHCenter);

    image = new QLabel();
    ui->verticalLayout_2->addWidget(image);
    ui->verticalLayout_2->addWidget(label);
    ui->verticalLayout_2->addWidget(timeSlider);

    QHBoxLayout *hbox = new QHBoxLayout();
    hbox->addWidget(rewind);
    hbox->addWidget(playpause);
    hbox->addWidget(next);

    ui->verticalLayout_2->addLayout(hbox);

    //endregion

    coverCache = new ImageCache(Utils::GetCacheDir() + "Covers");
    connect(coverCache, &ImageCache::ReceiveFinished, this, &GPMDP::CoverCacheFinished);

    webSocket = new QWebSocket("", QWebSocketProtocol::VersionLatest, nullptr);

    connect(webSocket, &QWebSocket::connected, this, &GPMDP::onConnected);
    connect(webSocket, &QWebSocket::disconnected, this, &GPMDP::closed);

    webSocket->open(QUrl("ws://127.0.0.1:5672"));
}

GPMDP::~GPMDP(){
    coverCache->SaveCache();
    delete coverCache;
}

void GPMDP::closed() {
    qDebug() << "Closed";
}

void GPMDP::onConnected(){
    qDebug() << "Connected!";
    connect(webSocket, &QWebSocket::textMessageReceived, this, &GPMDP::onMessage);

    QJsonArray carray;
    carray.append(ClientName);

    QFile file(TokenJson);
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
    if(doc["channel"].isUndefined()){
        //Okay not a channel, lets see if its a RPC response
        //Just check if the request id exists because even if it is a RPC response we cant do shit without it
        if(!doc["requestID"].isUndefined()){
            //Ok cool!
            int id = doc["requestID"].toInt();
            qDebug() << "Received RPC, ID: " << id;
            QtConcurrent::run(callbacks[id], doc);
            callbacks.remove(id);
        }
    }

    QString chnl = doc["channel"].toString()
            .replace('-', "")
            .replace(':', '_');

    std::string buffer("Channel_");
    buffer.append(chnl.toStdString());

    const char* method = buffer.c_str();

    if(!QMetaObject::invokeMethod(this, method, Qt::DirectConnection, Q_ARG(const QJsonDocument&, doc))){
        auto lol = doc.toJson().toStdString();
        qDebug() << "No method for channel " << doc["channel"].toString() << "\nPlease make a slot called " << method;
        assert(true);
    }

    //This is for temp, we should be updating these values in their respective channels.

    timeSlider->setMinimum(0);
    timeSlider->setMaximum(m_SongTotalTime);
    timeSlider->setValue(m_SongCurrentTime);

    label->setText(m_CurrentTrack.getArtist() + " - " + m_CurrentTrack.getTitle());
}

void GPMDP::SendRPC(const QString &Namespace, const QString &Method, const QJsonArray &Arguments, const std::function<void(QJsonDocument)>& callback) {

    QJsonObject json;
    json["namespace"] = Namespace;
    json["method"] = Method;
    json["arguments"] = Arguments;
    if(callback != nullptr) {
        int Id = callbacks.count() + 1;
        json["requestID"] = Id;
        callbacks.insert(Id, callback);
    }

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

//region Channels

void GPMDP::Channel_API_VERSION(const QJsonDocument& doc) {
    CheckPayload
    this->m_APIVersion = doc["payload"].toString();
    emit apiVersionChanged();
}

void GPMDP::Channel_connect(const QJsonDocument& doc) {
    if(doc["payload"].isString()){
        QString payload = doc["payload"].toString();
        if(payload == "CODE_REQUIRED"){
            //We need to get the code from the user
            QString scode = QInputDialog::getText(nullptr, "GPMDP", "Please give the code thats in GPMDP");

            QJsonArray app;
            app.append(ClientName);
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

                QFile file(TokenJson);
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

void GPMDP::Channel_playState(const QJsonDocument& doc) {
    if(doc["payload"].isBool()){
        m_IsPlaying = doc["payload"].toBool(false);
    }
}

void GPMDP::Channel_track(const QJsonDocument& doc) {
    CheckPayload
    //Would be nice if I could do something like C# aka
    // payload.ToObject<Track>();
    //but for now it works :|
    QJsonObject pay = doc["payload"].toObject();
    m_CurrentTrack.FillFromObject(pay);
    emit CurrentTrackChanged();

    if(!m_CurrentTrack.getAlbumArt().isEmpty()){
        //qDebug() << "Sending request for image: " << CurrentTrack->AlbumArt;
        coverCache->getData(m_CurrentTrack.getAlbumArt());
    }

    qDebug() << "New Track info: " << getCurrentTrack().getTitle();
}

void GPMDP::Channel_lyrics(const QJsonDocument& doc) {
    CheckPayload

    m_SongLyrics = doc["payload"].toString();
}

void GPMDP::Channel_time(const QJsonDocument& doc) {
    CheckPayload

    QJsonObject lol = doc["payload"].toObject();
    m_SongCurrentTime = lol["current"].toInt();
    m_SongTotalTime = lol["total"].toInt();
}

void GPMDP::Channel_shuffle(const QJsonDocument &doc) {
    CheckPayload
    if(doc["payload"].toString() == "ALL_SHUFFEL"){
        m_Shuffle = ALL_SHUFFLE;
    }
    else{
        m_Shuffle = NO_SHUFFLE;
    }
}

void GPMDP::Channel_repeat(const QJsonDocument &doc){
    CheckPayload
    QString pay = doc["payload"].toString();
    //Cant use a switch and cant figure out qmetaenums
    if(pay == "NO_REPEAT"){
        m_Repeat = NO_REPEAT;
    }else if(pay == "SINGLE_REPEAT"){
        m_Repeat = SINGLE_REPEAT;
    }else if(pay == "LIST_REPEAT"){
        m_Repeat = LIST_REPEAT;
    }
}

void GPMDP::Channel_queue(const QJsonDocument &doc){
    CheckPayload
    QJsonArray array = doc["payload"].toArray();
    if(!array.isEmpty()){
        m_Queue.clear();

        for(auto iter : array){
            QJsonObject obj = iter.toObject();
            GStructs::GTrack track;
            track.FillFromObject(obj);
            m_Queue.append(track);
        }
    }
}

void GPMDP::Channel_rating(const QJsonDocument &doc) {
    CheckPayload
    QJsonObject obj = doc["payload"].toObject();
    m_IsLiked = obj["liked"].toBool(false);
    m_IsDisliked = obj["disliked"].toBool(false);
}

void GPMDP::Channel_volume(const QJsonDocument &doc) {
    CheckPayload
    m_Volume = doc["payload"].toInt(0);
}

void GPMDP::Channel_playlists(const QJsonDocument &doc) {
    CheckPayload
    m_Playlists.clear();

    QJsonArray array = doc["payload"].toArray();

    for(auto iter : array){
        QJsonObject playlist = iter.toObject();

        GStructs::GPlaylist pl;
        pl.FillFromObject(playlist);

        m_Playlists.append(pl);
    }
}

void GPMDP::Channel_searchresults(const QJsonDocument &doc) {
    CheckPayload

}

//endregion
