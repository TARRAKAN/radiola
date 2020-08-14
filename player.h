#ifndef PLAYER_H
#define PLAYER_H

#include <QWidget>
#include <QStandardItemModel>
#include <QMediaPlayer>
#include <QMediaPlaylist>

namespace Ui{
class Player;
}

class Player : public QWidget{
    Q_OBJECT

public:
    explicit Player(QWidget *parent = nullptr);

private slots:
    void sAddMusic();
    void sTrackPosition(qint64);
    void sSliderPosition (int);
    void sTime(qint64);
    void sPlay();
    void sStatusChanged(QMediaPlayer::State);
    void sMute();
    void sVolume(int vol);
    void switchTrack(const QModelIndex &);
    void deleteTrack(const QModelIndex &);
    void showTrackName(const QModelIndex &);
    void showHidePlayList();


private:
    Ui::Player *ui;
    QMediaPlayer  *musicPlayer;
    QStandardItemModel *playListItemModel;
    QMediaPlaylist *pL;
    QString msToStr(qint64);
};

#endif
