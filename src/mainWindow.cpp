#include "mainWindow.hpp"

#include <QMouseEvent>
#include <QWheelEvent>
#include <qt6/QtCore/QtCore>
#include <qt6/QtGui/QPixmap>
#include <qt6/QtWidgets/QHBoxLayout>
#include <qt6/QtWidgets/QLabel>
#include <qt6/QtWidgets/QPushButton>
#include <qt6/QtWidgets/QVBoxLayout>
#include <qt6/QtGui/QIcon>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusInterface>
#include <QtDBus/QtDBus>
#include <QtDBus/QtDBus>
#include <qt6/QtNetwork/QNetworkAccessManager>
#include <qt6/QtNetwork/QNetworkReply>

MainWindow::MainWindow() {
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Window);
    this->setFixedSize(150, 200);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignCenter);

    songCover = new QLabel(this);
    songCover->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(songCover);

    songName = new QLabel("Nothing playing right now", this);
    songName->setAlignment(Qt::AlignCenter);
    songName->setFont(QFont("Fantasque Sans Mono", 11));
    songName->setStyleSheet("margin-top: 20px; color: white;");
    songName->setWordWrap(true);
    
    mainLayout->addWidget(songName);

    artist = new QLabel("...", this);
    artist->setAlignment(Qt::AlignCenter);
    artist->setWordWrap(true);
    artist->setStyleSheet("color: gray;");
    mainLayout->addWidget(artist);

    QLabel* coverLabel = new QLabel(this);

    setupDbus();
}

void MainWindow::setupDbus() {
    QDBusConnection::sessionBus().connect(
        "org.mpris.MediaPlayer2.spotify",
        "/org/mpris/MediaPlayer2",
        "org.freedesktop.DBus.Properties",
        "PropertiesChanged",
        this,
        SLOT(onSongChanged(QString, QVariantMap, QStringList))
    ); 

    QDBusConnection bus = QDBusConnection::sessionBus();

    QDBusMessage message = QDBusMessage::createMethodCall(
        "org.mpris.MediaPlayer2.spotify",
        "/org/mpris/MediaPlayer2",
        "org.freedesktop.DBus.Properties",
        "GetAll"
    );

    message << "org.mpris.MediaPlayer2.Player";
    QDBusMessage reply = bus.call(message);

    QVariantMap properties = qdbus_cast<QVariantMap>(reply.arguments().at(0));
    QVariantMap metadataMap;
    properties["Metadata"].value<QDBusArgument>() >> metadataMap;

    updateSong(metadataMap);
}

void MainWindow::onSongChanged(QString interface, QVariantMap changedProperties, QStringList invalidatedProperties) {
    if (changedProperties.contains("Metadata")) {
        QVariantMap metadataMap;
        changedProperties["Metadata"].value<QDBusArgument>() >> metadataMap;
        updateSong(metadataMap);
    }
}

void MainWindow::updateSong(QVariantMap metadataMap) {
    QString title = metadataMap["xesam:title"].toString();
    this->songName->setText(title);

    QNetworkAccessManager manager;
    QNetworkReply *reply = manager.get(QNetworkRequest(QUrl(metadataMap["mpris:artUrl"].toString())));
    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();
    if (reply->error() == QNetworkReply::NoError) {
        QPixmap cover;
        cover.loadFromData(reply->readAll());
        cover = cover.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        songCover->setPixmap(cover);
    }

    reply->deleteLater();

    artist->setText(metadataMap["xesam:artist"].toString());

}

void MainWindow::mousePressEvent(QMouseEvent *event) {
    QDBusInterface spotifyInterface(
        "org.mpris.MediaPlayer2.spotify", 
        "/org/mpris/MediaPlayer2", 
        "org.mpris.MediaPlayer2.Player", 
        QDBusConnection::sessionBus()
    );

    if (event->button() == Qt::LeftButton && spotifyInterface.isValid()) {
        spotifyInterface.call("PlayPause");
    } else if (event->button() == Qt::RightButton) {
        spotifyInterface.call("Next");
    }
}

void MainWindow::wheelEvent(QWheelEvent *event) {
    if (event->modifiers() & Qt::ControlModifier) {
        double steps = event->angleDelta().y() > 0 ? 1.0 : -1.0;
        adjustVolume(steps * 0.05);
    }
}

void MainWindow::adjustVolume(double volume) {
    QDBusInterface properties(
        "org.mpris.MediaPlayer2.spotify",
        "/org/mpris/MediaPlayer2",
        "org.freedesktop.DBus.Properties",
        QDBusConnection::sessionBus()
    );

    if (properties.isValid()) {
        QDBusReply<QDBusVariant> reply = properties.call("Get", "org.mpris.MediaPlayer2.Player", "Volume");

        if (reply.isValid()) {
            double currentVolume = reply.value().variant().toDouble();
            double newVol = currentVolume + volume;

            properties.call("Set", "org.mpris.MediaPlayer2.Player", "Volume", QVariant::fromValue(QDBusVariant(newVol)));
        }
    }
}
