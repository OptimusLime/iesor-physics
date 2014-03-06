#ifndef IESORBODY_H
#define IESORBODY_H

#include <stdio.h>
#include <JSON/json.h>
#include "network.h"
class Point;

class iesorBody 
{
	public:
		iesorBody(Network* sNetwork);

		//build body using network
		//keep as separate call so it can be made on another thread
		Json::Value buildBody(Json::Value compareBody);

	private:

		Network* network;

		double* queryCPPNOutputs(double x1, double y1, double x2, double y2, double maxXDist, double minYDist);
		Point* gridQueryPoints(int resolution);

		~iesorBody();
};

#endif //IESORBODY_H