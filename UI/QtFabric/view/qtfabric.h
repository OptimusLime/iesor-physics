#ifndef QTFABRIC_H
#define QTFABRIC_H

#include <QtGui/QMainWindow>
#include <QtWebKit\QWebView>
#include <stdio.h>

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

private:
	JSBridge* bridgeObject;
	QWebView* view;
	QString fabric;
	QVector<std::string> jsonQueue;
	bool loadedView;

	void processDrawQueue();
};

#endif 
// QTFABRIC_H
