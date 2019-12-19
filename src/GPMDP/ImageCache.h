//
// Created by drogonmar on 12/19/19.
//

#ifndef SVRLINUXTOOLS_IMAGECACHE_H
#define SVRLINUXTOOLS_IMAGECACHE_H

#include <QObject>
#include <QHash>

/*
 * Takes a url and will give you the cached version.
 */
class ImageCache : public QObject {
    Q_OBJECT
public:
    ImageCache(const QString& CacheDir);
    ~ImageCache();

    void RefreshCache();
    void SaveCache();
    void LoadCache();

public slots:
    void getData(const QString& url);

signals:
    void ReceiveFinished(const QByteArray &Data);

private slots:
    void networkFinished(class QNetworkReply *reply);

private:
    void CacheUrl(const QString& Url, const QByteArray &Data);

    class QNetworkAccessManager *network;

    QString CacheDir = "defaultcache";
    //1: URL
    //2: FileName aka Hash
    QHash<QString, QString> Files;
};


#endif //SVRLINUXTOOLS_IMAGECACHE_H
