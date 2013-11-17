#ifndef QTFABRIC_H
#define QTFABRIC_H

#include <QtGui/QMainWindow>
#include <QtWebKit/QWebView>
#include <stdio.h>
#include <IESoR/iesor.h>

class JSBridge;

class QtFabric : public QMainWindow
{
	Q_OBJECT

public:
	QtFabric();
	~QtFabric();

	void drawToFabric(std::string json);

protected slots:
	void finishLoading(bool);
	void addJavaScriptObject();
	void physicsUpdate();

private:
	JSBridge* bridgeObject;
	QWebView* view;
	QString fabric;
	QString neatjs;
	QVector<std::string> jsonQueue;
	bool loadedView;
	IESoRWorld* world;

	void processDrawQueue();
};

#endif 
// QTFABRIC_H
