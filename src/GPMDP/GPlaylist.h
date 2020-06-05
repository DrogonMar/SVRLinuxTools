#ifndef GPLAYLIST_H
#define GPLAYLIST_H

#include "../SVRLT.h"
#include "GTrack.h"

#include <QJsonObject>
#include <QJsonArray>

namespace GStructs {

class GPlaylist : public QObject
{

    Q_OBJECT
    Q_PROPERTY(QString id READ getId NOTIFY IdChanged)
    Q_PROPERTY(QString name READ getName NOTIFY NameChanged)
    Q_PROPERTY(QVector<GTrack> tracks READ getTracks NOTIFY TracksChanged)

public:
    explicit GPlaylist(QObject *parent = nullptr) : QObject(parent) {}
    virtual ~GPlaylist() {}
    GPlaylist(const GPlaylist &val){
        this->m_Id = val.m_Id;
        this->m_Name = val.m_Name;
        this->m_Tracks = val.m_Tracks;
        emit IdChanged();
        emit NameChanged();
        emit TracksChanged();
    }
    bool operator=(const GPlaylist& vale){
        return (vale.m_Id == getId());
    }

    QPROPGETSET(QString, Id, m_Id)
    QPROPGETSET(QString, Name, m_Name)
    QPROPGETSET(QVector<GTrack>, Tracks, m_Tracks)

    inline void ClearTracks(){
        m_Tracks.clear();
        emit TracksChanged();
    }

    void FillFromObject(const QJsonObject& object){
        this->m_Id = object["id"].toString();
        this->m_Name = object["name"].toString();

        ClearTracks();
        QJsonArray array = object["tracks"].toArray();
        QVector<GTrack> newArr;
        for(auto iter : array){
            QJsonObject track = iter.toObject();
            GTrack tr;
            tr.FillFromObject(track);
            newArr.append(tr);
        }
        this->m_Tracks = newArr;

        emit IdChanged();
        emit NameChanged();
        emit TracksChanged();
    }

signals:
    void IdChanged();
    void NameChanged();
    void TracksChanged();

private:
    QString m_Id;
    QString m_Name;
    QVector<GTrack> m_Tracks;
};
}

Q_DECLARE_METATYPE(GStructs::GPlaylist);

#endif // GPLAYLIST_H
