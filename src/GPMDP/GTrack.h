#ifndef GTRACK_H
#define GTRACK_H

#include "../SVRLT.h"

#include <QJsonObject>

namespace GStructs {
    class GTrack : public QObject
    {
    Q_OBJECT
        Q_PROPERTY(QString id MEMBER m_Id NOTIFY IdChanged)
        Q_PROPERTY(int index MEMBER m_Index NOTIFY IndexChanged)
        Q_PROPERTY(QString title MEMBER m_Title NOTIFY TitleChanged)
        Q_PROPERTY(QString artist MEMBER m_Artist NOTIFY ArtistChanged)
        Q_PROPERTY(QString album MEMBER m_Album NOTIFY AlbumChanged)
        Q_PROPERTY(QString albumArt MEMBER m_AlbumArt NOTIFY AlbumArtChanged)
        Q_PROPERTY(int duration MEMBER m_Duration NOTIFY DurationChanged)
        Q_PROPERTY(int playCount MEMBER m_PlayCount NOTIFY PlayCountChanged)

    public:
        explicit GTrack(QObject *parent = nullptr) : QObject(parent) {}
        virtual ~GTrack() {}
        GTrack(const GTrack &val){
            this->m_Id = val.m_Id;
            this->m_Index = val.m_Index;
            this->m_Title = val.m_Title;
            this->m_Artist = val.m_Artist;
            this->m_Album = val.m_Album;
            this->m_AlbumArt = val.m_AlbumArt;
            this->m_Duration = val.m_Duration;
            this->m_PlayCount = val.m_PlayCount;

            emit IdChanged();
            emit IndexChanged();
            emit TitleChanged();
            emit ArtistChanged();
            emit AlbumChanged();
            emit AlbumArtChanged();
            emit DurationChanged();
            emit PlayCountChanged();
        }
        bool operator=(const GTrack& vale){
            return (vale.m_Id ==  m_Id);
        }

        QPROPGETSET(QString, Id, m_Id)
        QPROPGETSET(int, Index, m_Index)
        QPROPGETSET(QString, Title, m_Title)
        QPROPGETSET(QString, Artist, m_Artist)
        QPROPGETSET(QString, Album, m_Album)
        QPROPGETSET(QString, AlbumArt, m_AlbumArt)
        QPROPGETSET(int, Duration, m_Duration)
        QPROPGETSET(int, PlayCount, m_PlayCount)

        void FillFromObject(const QJsonObject& object){
            this->m_Id = object["id"].toString();
            this->m_Index = object["index"].toInt();
            this->m_Title = object["title"].toString();
            this->m_Artist = object["artist"].toString();
            this->m_Album = object["album"].toString();
            this->m_AlbumArt = object["albumArt"].toString();
            this->m_Duration = object["duration"].toInt();
            this->m_PlayCount = object["playCount"].toInt();

            emit IdChanged();
            emit IndexChanged();
            emit TitleChanged();
            emit ArtistChanged();
            emit AlbumChanged();
            emit AlbumArtChanged();
            emit DurationChanged();
            emit PlayCountChanged();
        }

    signals:
        void IdChanged();
        void IndexChanged();
        void TitleChanged();
        void ArtistChanged();
        void AlbumChanged();
        void AlbumArtChanged();
        void DurationChanged();
        void PlayCountChanged();

    private:
        int m_Index;
        int m_PlayCount;
        QString m_Id;
        QString m_Title;
        QString m_Artist;
        QString m_Album;
        QString m_AlbumArt;
        int m_Duration;
    };
}


Q_DECLARE_METATYPE(GStructs::GTrack);

#endif // GTRACK_H
