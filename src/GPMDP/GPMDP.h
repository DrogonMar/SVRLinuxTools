//
// Created by drogonmar on 11/19/19.
//

#ifndef SVRLINUXTOOLS_GPMDP_H
#define SVRLINUXTOOLS_GPMDP_H

#include <functional>
#include <QWidget>
#include <QMap>
#include <QJsonArray>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QtCore/QQueue>
#include "GTrack.h"
#include "GPlaylist.h"

#define ChannelSlot(name) void Channel_##name(const QJsonDocument& doc);

namespace Ui{
    class GPMDP;
}

using namespace GStructs;
class GPMDP : public QWidget{
Q_OBJECT
    Q_PROPERTY(QString apiVersion READ apiVersion NOTIFY apiVersionChanged)
    Q_PROPERTY(GStructs::GTrack currentTrack READ getCurrentTrack NOTIFY CurrentTrackChanged)

signals:
    void apiVersionChanged();
    void CurrentTrackChanged();

public:
    explicit GPMDP(QWidget *parent = nullptr);
    ~GPMDP() override;

    GPMDP(const GPMDP& val){

    }

    GStructs::GTrack getCurrentTrack() const{
        return m_CurrentTrack;
    }

    //region Structs & Enums

    struct SAlbum{
    public:
        QString Id;
        QString Name;
        QString Artist;
        QString AlbumArt;

        QVariantMap ToMap(){
            QVariantMap map;
            map["id"] = Id;
            map["name"] = Name;
            map["artist"] = Artist;
            map["albumArt"] = AlbumArt;
            return map;
        }
    };

    struct SSearchResult{
    public:
        QString SearchText;
        QVariantMap ToMap(){
            QVariantMap map;
            map["searchText"] = SearchText;
            return map;
        }
    };

    enum EShuffle{
        ALL_SHUFFLE,
        NO_SHUFFLE
    };


    enum ERepeat{
        LIST_REPEAT,
        SINGLE_REPEAT,
        NO_REPEAT
    };

    //endregion

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

    inline void SetVolume(int vol){
        QJsonArray args;
        args.append(vol);

        SendRPC("volume", "setVolume", args);
    }

    inline QString apiVersion() {
        return m_APIVersion;
    }

private slots:
    void CoverCacheFinished(const QByteArray &Data);

    void closed();
    void onConnected();
    void onMessage(QString message);

    ChannelSlot(API_VERSION)

    ChannelSlot(connect)
    ChannelSlot(track)
    ChannelSlot(playState)
    ChannelSlot(lyrics)
    ChannelSlot(time)
    ChannelSlot(shuffle)
    ChannelSlot(repeat)
    ChannelSlot(queue)
    ChannelSlot(rating)
    ChannelSlot(volume)
    ChannelSlot(playlists)
    ChannelSlot(searchresults)

private:
    Ui::GPMDP *ui;

    void closeEvent(QCloseEvent *event) override;
    void SendRPC(const QString &Namespace, const QString &Method, const QJsonArray &Arguments = *new QJsonArray, const std::function<void(QJsonDocument)>& callback = nullptr);

    bool expectingCodeNext = false;
    bool fullyRegistered = false;

    class ImageCache *coverCache;
    class QWebSocket *webSocket;

    QMap<int, std::function<void(QJsonDocument)>> callbacks;

    //Temp UI
    class QSlider *timeSlider;
    class QLabel *image;
    class QLabel *label;

    //Player Info Vars
    QString m_APIVersion;
    QVector<GPlaylist> m_Playlists;

    // PlayState
    GStructs::GTrack m_CurrentTrack;
    bool m_IsPlaying;
    bool m_IsLiked;
    bool m_IsDisliked;
    EShuffle m_Shuffle;
    ERepeat m_Repeat;
    int m_Volume;
    QVector<GTrack> m_Queue;
    int m_SongCurrentTime;
    int m_SongTotalTime;

    // Lyrics
    QString m_SongLyrics;
};

Q_DECLARE_METATYPE(GPMDP);

#endif //SVRLINUXTOOLS_GPMDP_H
