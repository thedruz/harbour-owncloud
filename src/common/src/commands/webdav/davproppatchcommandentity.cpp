#include "davproppatchcommandentity.h"

DavPropPatchCommandEntity::DavPropPatchCommandEntity(QObject* parent,
                                                     QString remotePath,
                                                     QWebdav::PropValues properties,
                                                     QWebdav* client) :
              WebDavCommandEntity(parent, client)
{
    this->m_remotePath = remotePath;
    this->m_props = properties;

    QMap<QString, QVariant> info;
    info["type"] = QStringLiteral("davPropPatch");
    info["remotePath"] = remotePath;
    this->m_commandInfo = CommandEntityInfo(info);
}

bool DavPropPatchCommandEntity::startWork()
{
    if (!this->m_client) {
        qWarning() << "No valid client object available, aborting";
        abortWork();
        return false;
    }

    this->m_reply = this->m_client->proppatch(this->m_remotePath, m_props);

    QObject::connect(m_reply, &QNetworkReply::finished, this, [=]() {
        qDebug() << "PropPatching " << this->m_remotePath << "complete.";
    });

    const bool canStart = WebDavCommandEntity::startWork();
    if (!canStart)
        return false;

    setState(RUNNING);
    return true;
}
