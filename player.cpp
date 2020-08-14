#include "player.h"
#include "ui_player.h"
#include <QFileDialog>
#include <QDir>
#include <QtWidgets>
#include <QApplication>

Player::Player(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Player)
{
    ui->setupUi(this);
    playListItemModel = new QStandardItemModel(this);
    ui->tPlayList->setModel(playListItemModel);
    playListItemModel->setHorizontalHeaderLabels(QStringList()  << tr("ТРЕКИ") << tr("File Path"));
    ui->tPlayList->hideColumn(1);
    ui->tPlayList->verticalHeader()->setVisible(false);
    ui->tPlayList->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tPlayList->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tPlayList->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tPlayList->horizontalHeader()->setStretchLastSection(true);

    musicPlayer = new QMediaPlayer(this);
    pL = new QMediaPlaylist(musicPlayer);
    musicPlayer->setPlaylist(pL);
    musicPlayer->setVolume(50);
    pL->setPlaybackMode(QMediaPlaylist::Loop);//#Похимичить с рандомом

    ui->bPlay->setEnabled(false);
    ui->bPlay->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    ui->bNext->setEnabled(false);
    ui->bNext->setIcon(style()->standardIcon(QStyle::SP_MediaSkipForward));
    ui->bPrevious->setEnabled(false);
    ui->bPrevious->setIcon(style()->standardIcon(QStyle::SP_MediaSkipBackward));
    ui->sTrack->setEnabled(false);
    ui->sVol->setEnabled(false);
    ui->bMute->setIcon(style()->standardIcon(QStyle::SP_MediaVolumeMuted));
    ui->bMute->setEnabled(false);

    //Style
    QPalette dark;
    dark.setColor(QPalette::Window, QColor(53, 53, 53));
    dark.setColor(QPalette::WindowText, Qt::white);
    dark.setColor(QPalette::Base, QColor(25,25,25));
    dark.setColor(QPalette::AlternateBase, QColor(53,53,53));
    dark.setColor(QPalette::ToolTipBase, Qt::white);
    dark.setColor(QPalette::ToolTipText, Qt::white);
    dark.setColor(QPalette::Text, Qt::darkCyan);
    dark.setColor(QPalette::Button, QColor(53,53,53));
    dark.setColor(QPalette::ButtonText, Qt::white);
    qApp->setPalette(dark);

    connect(ui->tPlayList, SIGNAL(clicked(const QModelIndex &)), SLOT(switchTrack(const QModelIndex &)));

    connect(ui->tPlayList, SIGNAL(doubleClicked(const QModelIndex &)), SLOT(deleteTrack(const QModelIndex &)));

    connect(pL, &QMediaPlaylist::currentIndexChanged, [this](int index){
        ui->currentTrack->setText(playListItemModel->data(playListItemModel->index(index, 0)).toString());});

    connect(ui->sVol,  SIGNAL(valueChanged(int)), SLOT(sVolume(int)));
    connect(ui->sTrack, SIGNAL(sliderMoved(int)),SLOT(sSliderPosition(int)));
    connect(musicPlayer, SIGNAL(positionChanged(qint64)), SLOT(sTrackPosition(qint64)));
    connect(musicPlayer, SIGNAL(durationChanged(qint64)), SLOT(sTime(qint64)));
    connect(ui->bAdd, SIGNAL(clicked()), SLOT(sAddMusic()));
    connect(ui->bPlay, SIGNAL(clicked()), SLOT(sPlay()));
    connect(ui->bPrevious, SIGNAL(clicked()), pL, SLOT(previous()));
    connect(ui->bNext, SIGNAL(clicked()), pL, SLOT(next()));
    connect(musicPlayer, SIGNAL(stateChanged(QMediaPlayer::State)), SLOT(sStatusChanged(QMediaPlayer::State)));
    connect(ui->bMute, SIGNAL(clicked()), SLOT(sMute()));
    connect(ui->bHide, SIGNAL(clicked()), SLOT(showHidePlayList()));


}

void Player::switchTrack(const QModelIndex &index){
     pL->setCurrentIndex(index.row());
}

void Player::deleteTrack(const QModelIndex &index){
    pL->removeMedia(index.row());
    playListItemModel->removeRows(index.row(),1);
    ui->currentTrack->setText(playListItemModel->data(playListItemModel->index(index.row()+1, 0)).toString());
    if(pL->isEmpty()){
        ui->lRem->setText("00:00:00");
        ui->bPlay->setEnabled(false);
        ui->bNext->setEnabled(false);
        ui->bPrevious->setEnabled(false);
        ui->sTrack->setEnabled(false);
        ui->sVol->setEnabled(false);
        ui->bMute->setEnabled(false);
    }
}

void Player::showTrackName(const QModelIndex &index){
    ui->currentTrack->setText(playListItemModel->data(playListItemModel->index(index.row(), 0)).toString());
}



void Player::sAddMusic(){
    QStringList files = QFileDialog::getOpenFileNames(this, tr("Выбор файлов"), QString(), tr("Аудио (*.mp3)"));
    foreach (QString filePath, files) {
        QList<QStandardItem *> items;
        items.append(new QStandardItem(QDir(filePath).dirName()));
        items.append(new QStandardItem(filePath));
        playListItemModel->appendRow(items);
        pL->addMedia(QUrl("file://" + filePath));
    }
    ui->bPlay->setEnabled(true);
    ui->bNext->setEnabled(true);
    ui->bPrevious->setEnabled(true);
    ui->sTrack->setEnabled(true);
    ui->sVol->setEnabled(true);
    ui->bMute->setEnabled(true);
    ui->bMute->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
}

void Player::showHidePlayList(){
    if(ui->tPlayList->isHidden()){
        ui->tPlayList->show();
        ui->bHide->setText("_");

    }else{
        ui->tPlayList->hide();
        ui->bHide->setText("V");
        this->setFixedWidth(this->width());
        this->adjustSize();
        this->setFixedWidth(16777215);
    }
}

void Player::sTrackPosition(qint64 pos)
{
    ui->sTrack->setValue(pos);
    int nDuration = ui->sTrack->maximum();
    ui->lCur->setText(msToStr(pos));
    ui->lRem->setText(msToStr(nDuration - pos));
}

void Player::sSliderPosition(int pos)
{
    musicPlayer->setPosition(pos);
}

QString Player::msToStr(qint64 pos){
    int hur   = (pos / (60 * 60 * 1000));
    int min = ((pos  % (60 * 60 * 1000)) / (60 * 1000));
    int sec = ((pos % (60 * 1000)) / 1000);
    return QTime(hur, min, sec).toString("hh:mm:ss");
}

void Player::sTime(qint64 pos){
    ui->sTrack->setRange(0, pos);
    ui->lCur->setText(msToStr(0));
    ui->lRem->setText(msToStr(pos));
}

void Player::sPlay(){
    if(musicPlayer->state() ==QMediaPlayer::PlayingState)
       musicPlayer->pause();
    else
       musicPlayer->play();
}

void Player::sStatusChanged(QMediaPlayer::State state){
    if(state == QMediaPlayer::PlayingState)
        ui->bPlay->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
    else
        ui->bPlay->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
}

void Player::sMute(){
    if(musicPlayer->isMuted()){
        musicPlayer->setMuted(false);
        ui->bMute->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));

    }else{
        musicPlayer->setMuted(true);
        ui->bMute->setIcon(style()->standardIcon(QStyle::SP_MediaVolumeMuted));
    }
}

void Player::sVolume(int vol){
    musicPlayer->setVolume(vol);
    if(vol == 0){
        ui->bMute->setIcon(style()->standardIcon(QStyle::SP_MediaVolumeMuted));
        musicPlayer->pause();
        ui->bPlay->setEnabled(false);
    }else {
        musicPlayer->play();
        ui->bMute->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
        ui->bPlay->setEnabled(true);
    }
}

