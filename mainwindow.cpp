#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDomNodeList>
#include <QListWidget>
#include <QTextCodec>
#include <QtXml>
#include <QFile>
#include <QFileDialog>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    MainWindow::getConnectionAttributes();
    _server = new QTcpServer(); //создаем новый экземпляр сервера
    _server->listen(QHostAddress(address), port); //слушаем порт
    connect(_server, &QTcpServer::newConnection, this, &MainWindow::onNewConnection);
    ui->acceptButton->setEnabled(false);
    ui->rejectButton->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::getConnectionAttributes() // Получаем адрес и порт из xml файла
{
    QDomDocument attrsFile;
    QFile attrs(qApp->applicationDirPath() + "/attributes.xml"); //открываем файл с портом и адресом
    attrsFile.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"utf-8\""); // задаем инструкции для обработки
    attrs.open(QIODevice::ReadOnly);
    attrsFile.setContent(&attrs);
    QDomNodeList fileAttributes = attrsFile.elementsByTagName("attributes"); //получаем родительские элемент адреса и порта
    QDomNode attribute = fileAttributes.item(0); //задаем переменную QDomNode значением первого элемента
    address = attribute.firstChildElement("ip").text(); //получаем адрес
    port = attribute.firstChildElement("port").text().toUShort();//получаем порт
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
    QStringList longList = DataAsString.split(";");

    for (int i = 0; i<longList.size(); i++)
    {
      listItem = new QListWidgetItem(longList.at(i), ui->itemsList);
    }
    longList.clear();
    ui->acceptButton->setEnabled(true);
    ui->rejectButton->setEnabled(true);
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
    ui->acceptButton->setEnabled(false);
    ui->rejectButton->setEnabled(false);
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
     ui->acceptButton->setEnabled(false);
     ui->rejectButton->setEnabled(false);
}
