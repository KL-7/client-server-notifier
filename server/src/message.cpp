#include <QtSql>
#include <QVariant>
#include "message.h"


const QString Message::TABLE_NAME = "messages";
QString Message::lastErrorText;

Message::Message(int clientId, QString host, quint16 port, QString body, int id)
    : id(id), clientId(clientId), host(host), port(port), body(body) {
}

Message::Message(const Message &other) {
    clientId = other.clientId;
    host = other.host;
    port = other.port;
    body = other.body;
    id = other.id;
}

Message::Message(QSqlRecord record) {
    id = record.value("id").toInt();
    clientId = record.value("client_id").toInt();
    host = record.value("host").toString();
    port = record.value("port").toInt();
    body = record.value("body").toString();
}

void Message::saveLastError(QSqlQuery query) {
    if (!query.lastError().text().isEmpty()) {
        Message::lastErrorText = query.lastError().text();
    }
}

QString Message::lastError() {
    QString tmp = lastErrorText;
    lastErrorText = QString();
    return tmp;
}

int Message::count() {
    QSqlQuery query(QString("select count(*) from %1").arg(TABLE_NAME));
    if (query.next() && query.isValid()) {
        return query.value(0).toInt();
    }

    saveLastError(query);
    return -1;
}

Message* Message::byId(int id) {
    QSqlQuery query(QString("select * from %1 where id=%2").arg(TABLE_NAME).arg(id));
    if (query.next() && query.isValid()) {
        return new Message(query.record());
    }

    saveLastError(query);
    return 0;
}

QList<Message> Message::oldest(int limit) {
    QSqlQuery query(QString("select * from %1 order by updated_at limit %2").arg(TABLE_NAME).arg(limit));

    QList<Message> list;
    while (query.next()) {
        list.push_back(Message(query.record()));
    }

    saveLastError(query);
    return list;
}

bool Message::destroy() {
    QSqlQuery query;
    if (query.exec(QString("delete from %1 where id=%2").arg(TABLE_NAME).arg(id))) {
        id = -1;
        return true;
    } else {
        id = -1;
        return false;
    }
}

bool Message::touch() {
    QSqlQuery query(QString("select * from %1 where id=%2").arg(TABLE_NAME).arg(id));

    if (query.next() && query.isValid()) {
        query.exec(QString("update %1 set updated_at=datetime('now') where id=%2").arg(TABLE_NAME).arg(id));
        if (query.lastError().type() == QSqlError::NoError) {
            return true;
        }
    }

    saveLastError(query);
    return false;
}

bool Message::save() {
    QString values("(client_id, host, port, body, updated_at) values(%1, \"%2\", %3, \"%4\", datetime('now'))");
    values = values.arg(clientId).arg(host).arg(port).arg(body);

    QSqlQuery query;
    if (query.exec(QString("insert into %1 %2").arg(TABLE_NAME).arg(values))) {
        query.exec(QString("select id from %1 order by id desc limit 1").arg(TABLE_NAME));
        if (query.next() && query.isValid()) {
            id = query.value(0).toInt();
            return true;
        }
    }

    saveLastError(query);
    return false;
}
