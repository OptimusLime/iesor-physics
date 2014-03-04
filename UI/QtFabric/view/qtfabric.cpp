#include <QtGui>
#include "qtfabric.h"
#include "QtFabric/bridge/JSBridge.h"
#include <QtWebKit/QWebView>
#include <QtWebKit/QWebFrame>
#include <QtCore/QMap>
#include <QtCore/QVector>
#include <string>
#include <sstream>

static QString fabric;
static QString neatjs;
static QString threejs;
static QString sampleGenome;
static bool drawToJS = true;

//enum ActivationInt
//{
//	BipolarSigmoid = 0,
//	Gaussian,
//	StepFunction,
//	Sine,
//	Linear
//};

//TODO: Make a helper singleton for these functions
Json::Value parseJSON(std::string inString)
{
		//pull in our JSON body plz
	Json::Value inBody;
	Json::Reader read;
	read.parse(inString, inBody, true);
	return inBody;
}

Q_DECLARE_METATYPE(QVector<int>)

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

std::string asString(double number)
{
    std::ostringstream convert;   // stream used for the conversion
    convert << number;      // insert the textual representation of 'number' in the characters in the stream
    return convert.str();
}

void testBodyConversion(IESoRWorld* world, QWebFrame* frame)
{
		//Pulling in some sample data for testing!
	std::string bodyJSON = world->loadDataFile("jsBody112.json");
	
	//now get our genome k thx-- make it a QString for testing here
	QString genomeJSON = world->loadDataFile("jsGenome112.json").c_str();

	//pull in our JSON body plz
	Json::Value inBody = parseJSON(bodyJSON);

	//do a quick genome test please
	QVariant genomeTest = frame->evaluateJavaScript("doGPU(" + genomeJSON + ")");

	//convert our returned string into a json object
	Json::Value genomeByteNetwork = parseJSON(genomeTest.toString().toStdString());
	

	//let's make a body-- give it a new network
	Network* net = new Network(genomeByteNetwork);

	//pass to our genome converter
	iesorBody* genomeConverter = new iesorBody(net);

	//build the body
	Json::Value convertedBody = genomeConverter->buildBody(inBody);

	//now we compare!
	//must check nodes and connections

	Json::Value truthNodes = inBody["nodes"];
	Json::Value testNodes = convertedBody["nodes"];

	
	qDebug() << "truth nodes: " << truthNodes.size() << " test nodes: " << testNodes.size() << "\n";
	
	/*if(truthNodes.size() == testNodes.size())
	{
*/
		for(int i=0; i < truthNodes.size(); i++)
		{
			Json::Value tNode = truthNodes[i];
			Json::Value testNode = testNodes[i];

			double tX = tNode["x"].asDouble();
			double tY = tNode["y"].asDouble();
			std::string truth = asString(tNode["x"].asDouble()) + "," + asString(tNode["y"].asDouble());
			std::string test = asString(testNode["x"].asDouble()) + "," + asString(testNode["y"].asDouble());

			qDebug() << "truth node: " << truth.c_str() << " test node: " << test.c_str() << "\n";
		}
	//}


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


	world = new IESoRWorld();	
	//sampleGenome = world->loadDataFile("jsGenome224532.json").c_str();
	sampleGenome = world->loadDataFile("jsGenome142856.json").c_str();







	QString r =  readFile(":/QtFabric/html/basic.html");
	view->setHtml(r);
	//qDebug() << "contents: " << r;


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
	if(drawToJS)
		jsonQueue.push_back(json);

	if(loadedView)
		processDrawQueue();	
}

// number to be converted to a string
std::string nToS(int number)
{
	std::ostringstream convert;   // stream used for the conversion
	convert << number;      // insert the textual representation of 'number' in the characters in the stream
	return convert.str();
}
//
//int activationToInteger(QString activationType)
//{
//	if(activationType == "BipolarSigmoid")
//		return ActivationInt::BipolarSigmoid;
//	else if(activationType == "Gaussian")
//		return ActivationInt::Gaussian;
//	else if(activationType == "StepFunction")
//		return ActivationInt::StepFunction;
//	else if(activationType == "Sine")
//		return ActivationInt::Sine;
//	else if(activationType == "input")
//		return ActivationInt::Linear;
//	else
//	{
//		printf("No known activation type!");
//		throw 1;
//	}
//}



void convertJSONGenome(QWebView* view, QString& jsonString)
{
	Json::Value json;
	Json::Reader readJSON;

	//parse the string into a json object -- wahoooooooo
	readJSON.parse(jsonString.toStdString(), json);
	
	//nodeOrder
	//nodeArrays
	//QVector<int>& nOrder = json["nodeOrder"].convert(QVariant::ar;

	//biasCount: cppn.biasNeuronCount,
	//inputCount: cppn.inputNeuronCount,
	//nodeCount: cppn.totalNeuronCount,
	//connectionCount: cppn.connections.length,
	

	int biasCount = json["biasCount"].asInt();//.toInt();
	int inputCount = json["inputCount"].asInt();
	int outputCount = json["outputCount"].asInt();

	int nodeCount = json["nodeCount"].asInt();
	int connectionCount = json["connectionCount"].asInt();

	////have all the info to create weights and registers
	//double* weights = new double[connectionCount]; 
	//double* registers = new double[nodeCount];

	//int** registerArrays = new int*[nodeCount];
	//int** weightArrays = new int*[nodeCount];

	//int* nodeIncoming = new int[nodeCount];	

	//int* activationTypes = new int[nodeCount];	

	//int* nodeOrder = new int[nodeCount];	

	//QList<QVariant> nodeOrderJSON = json["nodeOrder"].toList();
	//int nIx = 0;
	////we loop through all our ordered nodes to create a node execution list
	//for (QList<QVariant>::iterator it = nodeOrderJSON.begin() ; it != nodeOrderJSON.end(); ++it)
	//{
	//	printf("In order: %d \n",  it->toInt());
	//	nodeOrder[nIx++] = it->toInt();
	//}

	//QList<QVariant> weightsJSON = json["weights"].toList();
	//nIx = 0;
	////we loop through all our ordered nodes to create a node execution list
	//for (QList<QVariant>::iterator it = weightsJSON.begin() ; it != weightsJSON.end(); ++it)
	//{
	//	printf("Weight: %f \n",  it->toDouble());
	//	weights[nIx++] = it->toDouble();
	//}

	////QVector<int> vecOrder = json["nodeOrder"].value<QVector<int>>();

	//QMap<QString, QVariant> nodeArrays = json["nodeArrays"].toMap();

	//for(int i=biasCount + inputCount; i < nodeCount; i++)
	//{
	//	//need to look inside node arrays
	//	QMap<QString, QVariant> nodeInfo = nodeArrays[nToS(i).c_str()].toMap();

	//	int inCount = nodeInfo["inCount"].toInt();
	//	nodeIncoming[i] = inCount;
	//	QList<QVariant> registerList = nodeInfo["registerList"].toList();
	//	QList<QVariant> weightList = nodeInfo["weightList"].toList();

	//	//this integer array holds the order of registers to draw from
	//	registerArrays[i] = new int[inCount];
	//	weightArrays[i] = new int[inCount];

	//	nIx = 0;
	//	//we loop through all our registers to query
	//	for (QList<QVariant>::iterator it = registerList.begin() ; it != registerList.end(); ++it)
	//	{
	//		printf("Register sampling: %d \n",  it->toInt());
	//		registerArrays[i][nIx++] = it->toInt();
	//	}
	//	//do the same for the weight indices
	//	nIx = 0;
	//	for (QList<QVariant>::iterator it = weightList.begin() ; it != weightList.end(); ++it)
	//	{
	//		printf("Weight index: %d \n",  it->toInt());
	//		weightArrays[i][nIx++] = it->toInt();
	//	}

	//	//handle activation functions
	//	QString activation = nodeInfo["activation"].toString();
	//	//activationTypes[i] = activationToInteger(activation);

	//}


	//we have enough to do activation now, I believe


	//double* weights = new double[connectionCount]; 
	//double* registers = new double[nodeCount];
	//int** registerArrays = new int*[nodeCount];
	//int** weightArrays = new int*[nodeCount];
	//int* nodeIncoming = new int[nodeCount];		
	//int* activationTypes = new int[nodeCount];	
	//int* nodeOrder = new int[nodeCount];	

	int totalInputs = inputCount + biasCount;

	//set bias = 1 
	//for(int i=0; i < biasCount; i++)
		//registers[i] = 1.0;
	Network* network = new Network(json);
	QMap<QString, QVariant> inOut = view->page()->mainFrame()->evaluateJavaScript("randomEvaluation(" + sampleGenome + ")").toMap();

	QList<QVariant> inputs = inOut["inputs"].toList();
	
	QList<QVariant> neuronSignals = inOut["signals"].toList();
	QList<QVariant> startSignals = inOut["startSignals"].toList();
	QList<QVariant> activationFunctions = inOut["activations"].toList();

	//outputs pulled from the list
	QList<QVariant> outputs = inOut["outputs"].toList();	
	
	double* dInputs = new double[inputCount];
	for(int i=0; i < inputCount; i++)
	{
		dInputs[i] = inputs[i].toDouble();
	}
	double* results = network->activate(dInputs);
	double* registers = network->GetRegisters();

	for(int i=0; i < nodeCount; i++)
	{
		printf("%d: \n", i);
		printf("start signals: %f \n", startSignals[i].toDouble());
		printf("real signals: %f \n", neuronSignals[i].toDouble());
		printf("registers: %f \n", registers[i]);
		QString act = activationFunctions[i].toString();
		
		printf("actFunction %s \n", act.toStdString().c_str());


	}

	for(int i=0; i < outputCount; i++)
	{
		int ix = i+totalInputs;
		printf("real %d - outputs: %f \n", ix, outputs[i].toDouble());
		printf("tgtIx %d - registers: %f \n", ix, registers[ix]);
		//printf("Output real: %f, output calc: %f \n", outputs[i], registers[i+totalInputs]);
	}

	printf("comp over");


}

void QtFabric::finishLoading(bool)
{
	//pull in neatjs code for now
	view->page()->mainFrame()->evaluateJavaScript(neatjs);

	//need code for doing GPU stuff in webkit for right now
	view->page()->mainFrame()->evaluateJavaScript(threejs);

	//we also want to use fabric code too
	view->page()->mainFrame()->evaluateJavaScript(fabric);

	//do a quick genome test please
	QVariant genomeTest = view->page()->mainFrame()->evaluateJavaScript("doGPU(" + sampleGenome + ")");

	//testing conversion to genome
	//convertJSONGenome(view, genomeTest.toString());
	testBodyConversion(world, view->page()->mainFrame());

	loadedView = true;

	if(jsonQueue.size())
		processDrawQueue();
}

QtFabric::~QtFabric()
{

}
