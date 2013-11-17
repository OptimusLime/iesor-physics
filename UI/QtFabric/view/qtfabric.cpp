#include <QtGui>
#include "qtfabric.h"
#include "QtFabric/bridge/JSBridge.h"
#include <QtWebKit/QWebView>
#include <QtWebKit/QWebFrame>


static QString fabric;
static QString neatjs;
static QString threejs;


QString readFile(QString filename) {
	QFile file(filename);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		return NULL;
	}

	QByteArray total;
	QByteArray line;
	while (!file.atEnd()) {
		line = file.read(1024);
		total.append(line);
	}

	return QString(total);
}


QString readFileToString(QString filePath)
{	
	QFile file;
	file.setFileName(filePath);
	file.open(QIODevice::ReadOnly);
	QString allRead = file.readAll();
	file.close();
	return allRead;
}

QtFabric::QtFabric()
{
	//ui.setupUi(this);
	loadedView = false;
	this->resize(1024, 768);

	//enable webview inspector
	QWebSettings::globalSettings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);

	//add in webgl support
	QWebSettings::globalSettings()->setAttribute(QWebSettings::WebGLEnabled, true);


	view = new QWebView(this);
	view->resize(this->size());
	view->settings()->setAttribute(QWebSettings::LocalContentCanAccessFileUrls,true);

	//view->setLayout(new QLayout());
	this->setSizePolicy(QSizePolicy::Expanding, view->sizePolicy().verticalPolicy());
	this->setSizePolicy(QSizePolicy::Expanding, view->sizePolicy().horizontalPolicy());


	//QUrl u = QUrl(":/QtFabric/html/basic.html");
	/*QUrl u = QUrl("http://google.com");
	view->load(u);*/
	connect(view, SIGNAL(loadFinished(bool)), SLOT(finishLoading(bool)));

	this->setCentralWidget(view);

	//pull in our js required libraries
	fabric = readFileToString(":/QtFabric/html/js/fabric.min.js");
	neatjs = readFileToString(":/QtFabric/neatjs");
	threejs = readFileToString(":/QtFabric/html/js/three.min.js");
	
	bridgeObject = new JSBridge(this);
	connect(view->page()->mainFrame(), 
		SIGNAL(javaScriptWindowObjectCleared()),
		this, 
		SLOT(addJavaScriptObject()));

	connect(bridgeObject, SIGNAL(triggerUpdate()), this, SLOT(physicsUpdate()));


	QString r =  readFile(":/QtFabric/html/basic.html");
	view->setHtml(r);
	qDebug() << "contents: " << r;


	world = new IESoRWorld();
	this->drawToFabric(world->worldDrawList());


	//view->show();
}

void QtFabric::physicsUpdate()
{
	//qDebug() << "Triggering physics update!";

	world->updateWorld(16.67);
	this->drawToFabric(world->worldDrawList());
}

void 
	QtFabric::addJavaScriptObject()
{
	/**
	* This code calls another interesting method 
	* QWebFrame::addJavaScriptObject
	* which can expose any QObject to the JavaScript context with the 
	* name given as the first parameter.
	*/

	QFile f;

	this->view->page()
		->mainFrame()
		->addToJavaScriptWindowObject("jsBridgeObject", 
		this->bridgeObject);
}



void addProperties(QMap<QString, QVariant>* map, QString prop, QVariant mapVal)
{
	(*map)[prop] = mapVal;
}

QVariant createDoubleMap(QString xName, QString xVal, QString yName, QString yVal)
{
	QMap<QString, QVariant> map;
	addProperties(&map, xName, xVal);
	addProperties(&map, yName, yVal);
	return map;
}


QVariant createPoint(QString x, QString y)
{
	return createDoubleMap("x", x, "y", y);
}
QVariant createWidthHeight(QString x, QString y)
{
	return createDoubleMap("width", x, "height", y);
}

QVariant createRectangle(QString left, QString top, QString width, QString height, QString color)
{
	QMap<QString, QVariant> jsonRect;	
	addProperties(&jsonRect, "type", "Rect");
	addProperties(&jsonRect, "topLeft", createPoint(left, top));
	addProperties(&jsonRect, "widthHeight", createWidthHeight(width, height));
	addProperties(&jsonRect, "color", color);
	return jsonRect;
}

QVariant createTriangle(QString left, QString top, QString width, QString height, QString color)
{
	QMap<QString, QVariant> jsonRect;	
	addProperties(&jsonRect, "type", "Triangle");
	addProperties(&jsonRect, "topLeft", createPoint(left, top));
	addProperties(&jsonRect, "widthHeight", createWidthHeight(width, height));
	addProperties(&jsonRect, "color", color);
	return jsonRect;
}

QVariant createCircle(QString centerX, QString centerY, QString radius, QString color)
{
	QMap<QString, QVariant> jsonRect;	
	addProperties(&jsonRect, "type", "Circle");
	addProperties(&jsonRect, "center", createPoint(centerX, centerY));
	addProperties(&jsonRect, "radius", radius);
	addProperties(&jsonRect, "color", color);
	return jsonRect;
}


void QtFabric::processDrawQueue()
{
	QVectorIterator<std::string> jIt(jsonQueue);

	while (jIt.hasNext()) {

		//create a qstring from our json string
		QString drawJSON(jIt.next().c_str());

		//add the items in (or update!)
		view->page()->mainFrame()->evaluateJavaScript("injectFabricObjects(" + drawJSON + ")");

	}

	//remove the objects from the list -- we drew everything already!
	jsonQueue.clear();


}
void QtFabric::drawToFabric(std::string json)
{
	jsonQueue.push_back(json);

	if(loadedView)
		processDrawQueue();	

}


void QtFabric::finishLoading(bool)
{
	//pull in neatjs code for now
	view->page()->mainFrame()->evaluateJavaScript(neatjs);

	//need code for doing GPU stuff in webkit for right now
	view->page()->mainFrame()->evaluateJavaScript(threejs);

	//we also want to use fabric code too
	view->page()->mainFrame()->evaluateJavaScript(fabric);

	loadedView = true;

	if(jsonQueue.size())
		processDrawQueue();
}

QtFabric::~QtFabric()
{

}
