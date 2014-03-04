#ifndef SIMPLEJSON_H
#define SIMPLEJSON_H
 
#include <QtCore/QObject>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QVariant>
#include <QtScript/QScriptValue>
#include <QtScript/QScriptEngine>
 

class SimpleJson : public QObject
{
    Q_OBJECT

	public:
		QString encode(const QMap<QString,QVariant> &map);
		QMap<QString, QVariant> decode(const QString &jsonStr);
		SimpleJson();

	private:
		QScriptValue encodeInner(const QMap<QString,QVariant> &map, QScriptEngine* engine);
		QMap<QString, QVariant> decodeInner(QScriptValue object);
		QList<QVariant> decodeInnerToList(QScriptValue arrayValue);
		~SimpleJson();
};

#endif //SIMPLEJSON_H