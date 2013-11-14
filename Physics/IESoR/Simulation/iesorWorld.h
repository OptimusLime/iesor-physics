#ifndef IESORWORLD_H
#define IESORWORLD_H

#include <JSON\json.h>
#include <stdio.h>
#include <Box2D\Box2D.h>

struct PhysicsID;

class IESoRWorld 
{
	public:
		IESoRWorld();
		std::string worldDrawList();
		b2World* world;
		int updateWorld(double msUpdate);

		b2Body* addBodyToWorld(std::string bodyID, b2BodyDef* bodyDef);
		b2Fixture* addShapeToBody(b2Body* body, b2FixtureDef* fixDef);
		b2Fixture* addShapeToBody(b2Body*body, b2Shape* shape, float density);

	private:
		//Json::Value* bodyList;
		//Json::Value* shapeList;
		double interpolation;
		double radians;
		double accumulator;
		double desiredFPS;
		double simulationRate;

		std::vector<PhysicsID*> bodyList;
		std::vector<PhysicsID*> shapeList;

		PhysicsID* createShapeID();
		

		~IESoRWorld();
		
};

#endif 
//IESORWORLD_H