//
// Created by drogonmar on 12/19/19.
//

#include "ImageCache.h"
#include <QFile>
#include <QtCore/QDebug>
#include <QSaveFile>
#include <QDir>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QCryptographicHash>
#include <QtCore/QDataStream>

#define cachedbFileLoc QString(CacheDir + '/' + "cachedb")

ImageCache::ImageCache(const QString &CacheDir) {
    this->CacheDir = CacheDir;
    QDir dir(CacheDir);
    if(!dir.exists()){
        QDir().mkdir(CacheDir);
    }else{
        LoadCache();
    }

    network = new QNetworkAccessManager(this);
    connect(network, &QNetworkAccessManager::finished, this, &ImageCache::networkFinished);
}

ImageCache::~ImageCache() {

}

void ImageCache::getData(const QString &url) {
    if(Files.contains(url)){
        QFile file = QFile(CacheDir  + '/' + Files[url]);
        if(file.exists()){
            qDebug() << "Url in db, giving the cached version.";
            file.open(QFile::ReadOnly);
            QByteArray data = file.readAll();
            emit ReceiveFinished(data);
        }else{
            qDebug() << "Url in db but file no longer exists, getting via network.";
            network->get(*new QNetworkRequest(url));
        }
    }else{
        qDebug() << "Url not in db, getting via network.";
        network->get(*new QNetworkRequest(url));
    }
}

void ImageCache::RefreshCache() {

}

void ImageCache::networkFinished(QNetworkReply *reply) {
    //If we get here its prob because we dont have it cached so lets do that.
    QByteArray Data = reply->readAll();
    CacheUrl(reply->url().toString(), Data);

    //We are done caching so lets give them their data.
    emit ReceiveFinished(Data);
}

void ImageCache::CacheUrl(const QString& Url, const QByteArray &Data) {
    QString hash = QString(QCryptographicHash::hash(Data, QCryptographicHash::Algorithm::Md5).toHex());
    QSaveFile file = QSaveFile(CacheDir + '/' + hash);
    file.open(QIODevice::WriteOnly);

    file.write(Data);
    if(file.commit()){
        Files.insert(Url, hash);
        SaveCache();
    }
}

void ImageCache::SaveCache() {
    qDebug() << "Saving....";
    QSaveFile file(cachedbFileLoc);
    if(!file.open(QIODevice::WriteOnly)){
        qDebug() << "Failed to open file";
        return;
    }

    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_5_0);
    out << Files;

    file.commit();
}

void ImageCache::LoadCache() {
    QFile file(cachedbFileLoc);

    if(!file.open(QIODevice::ReadOnly)){
        qDebug() << "Failed to load cache database.";
        qDebug() << file.errorString();
    }else{
        QDataStream in(&file);
        in.setVersion(QDataStream::Qt_5_0);
        Files.empty();
        in >> Files;
    }
}

