#include <qt6/QtGui/QPixmap>
#include <qt6/QtWidgets/QLabel>
#include <qt6/QtWidgets/QVBoxLayout>
#include <qt6/QtCore/QtCore>

#pragma once

class MainWindow : public QWidget {
    Q_OBJECT

public:
    QLabel *songCover;
    QLabel *songName;
    QLabel *artist;

    MainWindow();
    void setupDbus();
    void updateSong(QVariantMap metadataMap);
    void adjustVolume(double volume);

protected:
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void wheelEvent(QWheelEvent *event) override;
    
public slots:
  void onSongChanged(QString interface, QVariantMap changedProperties, QStringList invalidatedProperties);
};

