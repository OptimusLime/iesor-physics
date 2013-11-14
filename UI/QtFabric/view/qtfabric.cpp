#include <QtGui>
#include "qtfabric.h"
#include "QtFabric\bridge\JSBridge.h"
#include <QtWebKit\QWebView>
#include <QtWebKit\QWebFrame>

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

QtFabric::QtFabric()
{
	//ui.setupUi(this);
	loadedView = false;
	this->resize(1024, 768);

	//enable webview inspector
	QWebSettings::globalSettings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);

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

	QFile file;
	file.setFileName(":/QtFabric/html/js/fabric.min.js");
	file.open(QIODevice::ReadOnly);
	fabric = file.readAll();
	file.close();



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

	world->updateWorld(33.333);
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
	view->page()->mainFrame()->evaluateJavaScript(fabric);
	////view->page()->mainFrame()->evaluateJavaScript("genericFabricAdd()");

	loadedView = true;

	if(jsonQueue.size())
		processDrawQueue();



	//QVariant jsonRect = createRectangle("600", "200", "100", "50", "#836948");	
	//QVariant jsonCircle = createCircle("200", "400", "65", "#369");	
	//QVariant jsonTriangle = createTriangle("100", "25", "100", "50", "#953");	

	//addProperties(&jsonRect, "type", "Rect");
	//addProperties(&jsonRect, "topLeft", createPoint("600", "200"));
	//addProperties(&jsonRect, "widthHeight", createWidthHeight("100", "50"));
	//addProperties(&jsonRect, "color", "#369");

	//create rect with the above props and named rect1
	//QMap<QString, QVariant> jsonShapes;
	//addProperties(&jsonShapes, "rect1", jsonRect);
	//addProperties(&jsonShapes, "circle1", jsonCircle);
	//addProperties(&jsonShapes, "triangle1", jsonTriangle);
	//
	//SimpleJson* sj = new SimpleJson();
	//QString json = sj->encode(jsonShapes);

	//view->page()->mainFrame()->evaluateJavaScript("injectFabricObjects(" + json + ")");

	//QMap<QString, QVariant> jsonVariant;
	//jsonVariant["sup"] = QString("diddle");

	//SimpleJson* sj = new SimpleJson();
	//QString json = sj->encode(jsonVariant);

	////this will parse the string as JSON, and send in a json object to our evaluated callback
	//QVariant f1result = view->page()->mainFrame()->evaluateJavaScript("wakkawakka("+ json + ")");
	//   qDebug() << f1result.toString();


}

QtFabric::~QtFabric()
{

}
