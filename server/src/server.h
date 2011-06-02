#ifndef SERVER_H
#define SERVER_H

#include "ui_server.h"
#include "message.h"


class QSqlTableModel;
class AddMessageDialog;

class Server : public QDialog, private Ui::Server {
    Q_OBJECT

public:
    explicit Server(QWidget *parent = 0);

private slots:
    void initUi();
    void initDB();
    void updateUi();
    void showAddMessageDialog();
    void addMessage(Message message);
    void onError(QString error);
    void onMessageDelivered(int messageId);
    void deliverMessagesBunch();
    void toggleDelivery();
    void toggleAdminConnection();

private:
    enum LogMessageType { SYS, MSG, ERR };

    static const int CLIENT_WAIT_TIMEOUT;
    static const int MESSAGES_DELIVERY_BUNCH_SIZE;
    static const int MESSAGES_DELIVERY_INTERVAL;
    static const QString DATABASE_NAME;

    QTimer* deliveryTimer;
    QSqlTableModel* model;
    AddMessageDialog* addMessageDialog;

    bool isAdminConnectionRunning();
    void sendMessage(Message message);
    QString logMessageTypeToQString(LogMessageType logMessageType);
    void log(QString message, LogMessageType type = SYS);

};

#endif // SERVER_H
