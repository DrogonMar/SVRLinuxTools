//
// Created by drogonmar on 11/19/19.
//

#ifndef SVRLINUXTOOLS_GPMDP_H
#define SVRLINUXTOOLS_GPMDP_H

#include <QWidget>
#include <QJsonArray>

struct Track{
public:
    QString Id;
    int Index;
    QString Title;
    QString Artist;
    QString Album;
    QString AlbumArt;
    QString Duration;
    int PlayCount;

    QString ToString(){
        return Artist + ": " + Title;
    }
};

namespace Ui{
    class GPMDP;
}

class GPMDP : public QWidget{
    Q_OBJECT

public:
    explicit GPMDP(QWidget *parent = nullptr);
    ~GPMDP() override;

    inline void Rewind(){
        SendRPC("playback", "rewind");
    }

    inline void PlayPause(){
        SendRPC("playback", "playPause");
    }

    inline void Forward(){
        SendRPC("playback", "forward");
    }

    inline void SetCurrentTime(int pos){
        QJsonArray args;
        args.append(pos);

        SendRPC("playback", "setCurrentTime", args);
    }

Q_SIGNALS:

private slots:
    void networkFinished(class QNetworkReply *reply);

    void closed();
    void onConnected();
    void onMessage(QString message);

    void Channel_connect(const QString &message);
    void Channel_track(const QString &message);
    void Channel_playState(const QString &message);
    void Channel_lyrics(const QString &message);
    void Channel_time(const QString &message);

private:

    void SendRPC(const QString &Namespace, const QString &Method, const QJsonArray &Arguments = *new QJsonArray);

    bool expectingCodeNext = false;
    bool fullyRegistered = false;

    Ui::GPMDP *ui;
    class QNetworkAccessManager *network;
    class QSlider *timeSlider;
    class QLabel *image;
    class QLabel *label;
    class QWebSocket *webSocket;


    //Player Info Vars

    // PlayState
    bool IsPlaying;
    Track *CurrentTrack;

    // Time
    int SongCurrentTime;
    int SongTotalTime;

    // Lyrics
    QString SongLyrics;
};

#endif //SVRLINUXTOOLS_GPMDP_H
