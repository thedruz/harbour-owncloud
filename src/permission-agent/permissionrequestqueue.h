#ifndef PERMISSIONREQUESTQUEUE_H
#define PERMISSIONREQUESTQUEUE_H

#include <QObject>
#include <QQueue>
#include <QDBusMessage>

class PermissionRequestQueue : public QObject
{
    Q_OBJECT

public:
    explicit PermissionRequestQueue(QObject *parent = 0);
    void enqueueRequest(QDBusMessage requestMessage);

public slots:
    void permitFirstInQueue();
    void discardFirstInQueue();

private:
    QQueue<QDBusMessage> m_requestQueue;

signals:
    void permissionRequested();
};

#endif // PERMISSIONREQUESTQUEUE_H
