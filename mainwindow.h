#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QListWidget>
#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void onNewConnection();
    void onSocketStateChanged(QAbstractSocket::SocketState socketState);
    void onReadyRead();
private slots:
    void on_acceptButton_clicked();

    void on_rejectButton_clicked();

private:
    Ui::MainWindow *ui;
    QTcpServer  *_server = nullptr;
    QList<QTcpSocket*>  _sockets;
    QListWidgetItem* listItem;
};

#endif // MAINWINDOW_H
