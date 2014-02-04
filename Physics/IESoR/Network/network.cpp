#include "network.h"
#include <sstream>

enum ActivationInt
{
	BipolarSigmoid = 0,
	Gaussian,
	StepFunction,
	Sine,
	Sine2,
	Linear
};


static double bipolarSigmoid(double val)
{
	 return (2.0 / (1.0 + exp(-4.9 * val))) - 1.0;
}
static double sine(double val)
{
	return sin(val);
}

static double sine2(double val)
{
	return sin(2*val);
}

static double stepFunction(double val)
{
	if(val<=0.0)
		return 0.0;
	else
		return 1.0;
}

static double linear(double val)
{
	return val;
}

static double gaussian(double val)
{
	 return 2 * exp(-pow(val * 2.5, 2)) - 1;
}


// number to be converted to a string
std::string numAsString(int number)
{
	std::ostringstream convert;   // stream used for the conversion
	convert << number;      // insert the textual representation of 'number' in the characters in the stream
	return convert.str();
}


int Network::activationToInteger(std::string activationType)
{
	if(activationType == "BipolarSigmoid")
		return ActivationInt::BipolarSigmoid;
	else if(activationType == "Gaussian")
		return ActivationInt::Gaussian;
	else if(activationType == "StepFunction")
		return ActivationInt::StepFunction;
	else if(activationType == "Sine")
		return ActivationInt::Sine;
	else if(activationType == "input")
		return ActivationInt::Linear;
	else
	{
		printf("No known activation type!");
		throw 1;
	}
}


Network::Network(Json::Value jsonNetwork)
{
	biasCount = jsonNetwork["biasCount"].asInt();
	inputCount = jsonNetwork["inputCount"].asInt();
	outputCount = jsonNetwork["outputCount"].asInt();

	nodeCount = jsonNetwork["nodeCount"].asInt();
	connectionCount = jsonNetwork["connectionCount"].asInt();

	//have all the info to create weights and registers
	weights = new double[connectionCount]; 
	registers = new double[nodeCount];

	registerArrays = new int*[nodeCount];
	weightArrays = new int*[nodeCount];

	nodeIncoming = new int[nodeCount];	

	activationTypes = new int[nodeCount];	

	nodeOrder = new int[nodeCount];	

	Json::Value nodeOrderJSON = jsonNetwork["nodeOrder"];
	//we loop through all our ordered nodes to create a node execution list
	for (int nIx =0; nIx < nodeOrderJSON.size(); nIx++)
	{
		printf("In order: %d \n",  nodeOrderJSON[nIx].asInt());
		nodeOrder[nIx] = nodeOrderJSON[nIx].asInt();
	}

	Json::Value weightsJSON = jsonNetwork["weights"];

	//we loop through all our ordered nodes to create a node execution list
	for (int nIx =0; nIx < weightsJSON.size(); nIx++)
	{
		printf("Weight: %f \n", weightsJSON[nIx].asDouble());
		weights[nIx] = weightsJSON[nIx].asDouble();
	}

	//QVector<int> vecOrder = json["nodeOrder"].value<QVector<int>>();

	Json::Value nodeArrays = jsonNetwork["nodeArrays"];

	for(int i=biasCount + inputCount; i < nodeCount; i++)
	{
		//need to look inside node arrays
		Json::Value nodeInfo = nodeArrays[numAsString(i).c_str()];

		int inCount = nodeInfo["inCount"].asInt();
		nodeIncoming[i] = inCount;
		Json::Value registerList = nodeInfo["registerList"];
		Json::Value weightList = nodeInfo["weightList"];

		//this integer array holds the order of registers to draw from
		registerArrays[i] = new int[inCount];
		weightArrays[i] = new int[inCount];

		//we loop through all our registers to query
		for (int nIx =0; nIx < registerList.size(); nIx++)
		{
			printf("Register sampling: %d \n",  registerList[nIx].asInt());
			registerArrays[i][nIx] = registerList[nIx].asInt();
		}

		//do the same for the weight indices
		for (int nIx =0; nIx < weightList.size(); nIx++)
		{
			printf("Weight index: %d \n",  weightList[nIx].asInt());
			weightArrays[i][nIx++] = weightList[nIx].asInt();
		}

		//handle activation functions
		Json::Value activation = nodeInfo["activation"];
		activationTypes[i] = activationToInteger(activation.asString());
	}

}

double* Network::activate(double* inputs)
{
	
	int totalInputs = inputCount + biasCount;
	
		//set bias
	for(int i=0; i < biasCount; i++)
		registers[i] = 1.0;

	//set inputs
	for(int i=0; i < inputCount; i++)
		registers[i+biasCount] = inputs[i];

	for(int i=0; i < nodeCount; i++)
	{
		//activate in order
		int tgtNeuronIx = nodeOrder[i];

		//skip inputs and bias
		if(tgtNeuronIx < totalInputs)
			continue;


		//Hello. Are you there?


		//Ix

		int* regIxArray = registerArrays[tgtNeuronIx];
		int* weightIxArray = weightArrays[tgtNeuronIx];

		int nCount = nodeIncoming[tgtNeuronIx];
		int aType = activationTypes[tgtNeuronIx];

		double nodeSum = 0;

		for(int r=0; r < nCount; r++)
		{
			nodeSum += registers[regIxArray[r]]*weights[weightIxArray[r]];
		}

		switch(aType)
		{
			case ActivationInt::BipolarSigmoid:
				registers[tgtNeuronIx] = bipolarSigmoid(nodeSum);
				break;

			case ActivationInt::Gaussian:
				registers[tgtNeuronIx] = gaussian(nodeSum);
				break;
			case ActivationInt::Linear:
				registers[tgtNeuronIx] = linear(nodeSum);
				break;
			case ActivationInt::Sine:
				registers[tgtNeuronIx] = sine(nodeSum);
				break;
			case ActivationInt::StepFunction:
				registers[tgtNeuronIx] = stepFunction(nodeSum);
				break;
		}

		printf("tgtIx %d - calc: %f \n", tgtNeuronIx, registers[tgtNeuronIx]);

		//register done, move on!
	}
	
	//activate 
	double* fullOutputs = new double[outputCount];

	//copy to double array
	memcpy (fullOutputs,  &registers[totalInputs], outputCount);

	//send back registers starting at outputs!
	return fullOutputs;
}

Network::~Network()
{
	
}