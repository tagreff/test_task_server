#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QListWidget>
#include <QTextCodec>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    _server = new QTcpServer();
    _server->listen(QHostAddress::Any, 5000);
    connect(_server, &QTcpServer::newConnection, this, &MainWindow::onNewConnection);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onNewConnection()
{
   QTcpSocket *clientSocket = _server->nextPendingConnection();
   connect(clientSocket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
   connect(clientSocket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(onSocketStateChanged(QAbstractSocket::SocketState)));

    _sockets.push_back(clientSocket);
    for (QTcpSocket* socket : _sockets) {
        socket->write(QByteArray::fromStdString(clientSocket->peerAddress().toString().toStdString() + " connected to server !\n"));
    }
}

void MainWindow::onSocketStateChanged(QAbstractSocket::SocketState socketState)
{
    if (socketState == QAbstractSocket::UnconnectedState)
    {
        QTcpSocket* sender = static_cast<QTcpSocket*>(QObject::sender());
        _sockets.removeOne(sender);
    }
}

void MainWindow::onReadyRead()
{
    QTcpSocket* sender = static_cast<QTcpSocket*>(QObject::sender());
    QByteArray datas = sender->readAll();
    QString DataAsString = datas;
    QStringList longList = DataAsString.split("  ");

    for (int i = 0; i<longList.size(); i++)
    {
      listItem = new QListWidgetItem(longList.at(i), ui->itemsList);
    }


}

void MainWindow::on_acceptButton_clicked()
{
    QTcpSocket* sender = static_cast<QTcpSocket*>(QObject::sender());
    for (QTcpSocket* socket : _sockets) {
        if (socket != sender){
            socket->write(QByteArray("true"));
        }
    }
    ui->itemsList->clear();
}


void MainWindow::on_rejectButton_clicked()
{
    QTcpSocket* sender = static_cast<QTcpSocket*>(QObject::sender());
    for (QTcpSocket* socket : _sockets) {
        if (socket != sender){
            socket->write(QByteArray("false"));
        }
    }
     ui->itemsList->clear();
}
