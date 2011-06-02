#ifndef MESSAGE_H
#define MESSAGE_H

#include <QString>


class QSqlRecord;
class QSqlQuery;

class Message {

public:
    Message(quint16 clientId, QString host, quint16 port, QString body, int id = -1);
    Message(const Message& other);
    Message(QSqlRecord record);

    static Message* byId(int id);
    static QList<Message> oldest(int limit = 10);
    static QString lastError();
    static bool hasLastError();
    static int count();

    bool touch();
    bool save();
    bool destroy();

    int id;
    quint16 clientId;
    QString host;
    quint16 port;
    QString body;

private:
    static const QString TABLE_NAME;

    static void saveLastError(QSqlQuery query);

    static QString lastErrorText;

};

#endif // MESSAGE_H
