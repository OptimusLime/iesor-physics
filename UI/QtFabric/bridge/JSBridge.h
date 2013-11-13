#ifndef JSBRIDGE_H
#define JSBRIDGE_H
 
#include <QtCore/QObject>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QVariant>
 
class JSBridge : public QObject
{
    Q_OBJECT
public:
    explicit JSBridge(QObject *parent = 0);
 
signals:
    void signal(QMap<QString, QVariant> object);
public slots:
    QMap<QString, QVariant> slotThatReturns(const QMap<QString, 
                                            QVariant>& object);
    void slotThatEmitsSignal();
 
private:
    int m_signalEmited;
    QMap<QString, QVariant> m_returnObject;
    QMap<QString, QVariant> m_emitSignal;
};
 
#endif // JSBRIDGE_H