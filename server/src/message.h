#ifndef MESSAGE_H
#define MESSAGE_H

#include <QString>


class Message {

public:
    Message(int id, int clientId, QString host, quint16 port, QString body);
    Message(const Message& other);

    int id;
    int clientId;
    QString host;
    quint16 port;
    QString body;

};

#endif // MESSAGE_H
