#include "JSBridge.h"
 
#include <QtCore/QDebug>
 
JSBridge::JSBridge(QObject *parent) :
    QObject(parent)
{
    m_signalEmited = 0;
}
 
QMap<QString, QVariant>
JSBridge::slotThatReturns(const QMap<QString, QVariant>& object)
{
    qDebug() << "JSBridge::slotThatReturns";
    this->m_returnObject.clear();
    this->m_returnObject.unite(object);
    QString addedBonus = QString::number(object["intValue"].toInt(), 
                                         10).append(" added bonus.");
    this->m_returnObject["stringValue"] = QVariant(addedBonus);
    qDebug() << "JSBridge::slotThatReturns" << this->m_returnObject;
    return this->m_returnObject;
}
void JSBridge::jsCallPhysicsUpdate()
{
	emit triggerUpdate();
}

void 
JSBridge::slotThatEmitsSignal()
{
    qDebug() << "JSBridge::slotThatEmitsSignal";
    this->m_signalEmited++;
    this->m_emitSignal.clear();
    this->m_emitSignal["signalsEmited"] = QVariant(this->m_signalEmited);
    this->m_emitSignal["sender"] = QVariant("JSBridge::slotThatEmitsSignal");
    qDebug() << "JSBridge::slotThatEmitsSignal" << this->m_emitSignal;
    emit signal(this->m_emitSignal);
}