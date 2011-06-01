#include "message.h"

Message::Message(int id, int clientId, QString host, quint16 port, QString body)
    : id(id), clientId(clientId), host(host), port(port), body(body) {
}

Message::Message(const Message &other) {
    id = other.id;
    clientId = other.clientId;
    port = other.port;
    host = other.host;
    body = other.body;
}
