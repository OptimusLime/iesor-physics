#include "iesorWorld.h"

#include <string>
#include <iostream>
#include <fstream>
#include <string>

#include <JSON\src\writer.h>


//including our json info for easy access
using namespace Json;
using namespace std;



struct PhysicsID
{
	public:
		PhysicsID(string pid)
		{
			this->id = pid;
		}
		string ID(){return id;}
	private: 
		string id;
		~PhysicsID()
		{
			this->id = "";
		}
};


static int staticBodyCount = 0;

std::string get_file_contents(const char *filename)
{
	std::ifstream in(filename, std::ios::in | std::ios::binary);
	if (in)
	{
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return(contents);
	}
	throw(errno);
}
void stuff()
{
	// Define the gravity vector.
	b2Vec2 gravity(0.0f, -10.0f);

	// Construct a world object, which will hold and simulate the rigid bodies.
	b2World world(gravity);

	// Define the ground body.
	b2BodyDef groundBodyDef;
	groundBodyDef.position.Set(0.0f, -10.0f);

	// Call the body factory which allocates memory for the ground body
	// from a pool and creates the ground box shape (also from a pool).
	// The body is also added to the world.
	b2Body* groundBody = world.CreateBody(&groundBodyDef);

	// Define the ground box shape.
	b2PolygonShape groundBox;

	// The extents are the half-widths of the box.
	groundBox.SetAsBox(50.0f, 10.0f);

	// Add the ground fixture to the ground body.
	groundBody->CreateFixture(&groundBox, 0.0f);

	// Define the dynamic body. We set its position and call the body factory.
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set(0.0f, 4.0f);
	b2Body* body = world.CreateBody(&bodyDef);

	// Define another box shape for our dynamic body.
	b2PolygonShape dynamicBox;
	dynamicBox.SetAsBox(1.0f, 1.0f);

	// Define the dynamic body fixture.
	b2FixtureDef fixtureDef;
	fixtureDef.shape = &dynamicBox;

	// Set the box density to be non-zero, so it will be dynamic.
	fixtureDef.density = 1.0f;

	// Override the default friction.
	fixtureDef.friction = 0.3f;

	// Add the shape to the body.
	body->CreateFixture(&fixtureDef);

	// Prepare for simulation. Typically we use a time step of 1/60 of a
	// second (60Hz) and 10 iterations. This provides a high quality simulation
	// in most game scenarios.
	float32 timeStep = 1.0f / 60.0f;
	int32 velocityIterations = 6;
	int32 positionIterations = 2;

	// This is our little game loop.
	for (int32 i = 0; i < 60; ++i)
	{
		// Instruct the world to perform a single step of simulation.
		// It is generally best to keep the time step and iterations fixed.
		world.Step(timeStep, velocityIterations, positionIterations);

		// Now print the position and angle of the body.
		b2Vec2 position = body->GetPosition();
		float32 angle = body->GetAngle();

		printf("%4.2f %4.2f %4.2f\n", position.x, position.y, angle);
	}
}

void processJSONStream(string jsonString)
{
	Json::Value root;   // will contains the root value after parsing.
	Json::Reader reader;
	bool parsingSuccessful = reader.parse(jsonString, root );
	if ( !parsingSuccessful )
	{
		// report to the user the failure and their locations in the document.
		std::cout  << "Failed to parse configuration\n"
			<< reader.getFormatedErrorMessages();
		return;
	}

	// Get the value of the member of root named 'encoding', return 'UTF-8' if there is no
	// such member.
	std::string encoding = root.get("encoding", "UTF-8" ).asString();
	// Get the value of the member of root named 'encoding', return a 'null' value if
	//// there is no such member.
	//const Json::Value plugins = root["plug-ins"];
	//for ( int index = 0; index < plugins.size(); ++index )  // Iterates over the sequence elements.
	//	loadPlugIn( plugins[index].asString() );

	//setIndentLength( root["indent"].get("length", 3).asInt() );
	//setIndentUseSpace( root["indent"].get("use_space", true).asBool() );

	// ...
	// At application shutdown to make the new configuration document:
	// Since Json::Value has implicit constructor for all value types, it is not
	//// necessary to explicitly construct the Json::Value object:
	//root["encoding"] = getCurrentEncoding();
	//root["indent"]["length"] = getCurrentIndentLength();
	//root["indent"]["use_space"] = getCurrentIndentUseSpace();

	Json::StyledWriter writer;
	// Make a new JSON document for the configuration. Preserve original comments.
	std::string outputConfig = writer.write( root );

	// You can also use streams.  This will put the contents of any JSON
	// stream at a particular sub-value, if you'd like.
	std::cin >> root["subtree"];

	// And you can write to a stream, using the StyledWriter automatically.
	std::cout << root;

}

b2Body* IESoRWorld::addBodyToWorld(string bodyID, b2BodyDef* bodyDef)
{
	//add body to world
	b2Body* body = this->world->CreateBody(bodyDef);
	
	//assign the bodyID to a new struct object
	PhysicsID* pid = new PhysicsID(bodyID);
	
	//track all the struct objects
	this->bodyList.push_back(pid);
	
	//identify body with user data
	body->SetUserData(pid);

	return body;
}
		

b2Fixture* IESoRWorld::addShapeToBody(b2Body* body, b2FixtureDef* fixDef)
{
	//We just keep a static int of all the shapes for all identification
	int bcnt = staticBodyCount++;

	//Track the shape string
	PhysicsID* shapeID = new PhysicsID(std::to_string((long double)bcnt));

	//Save our shape to the list of shapes
	this->shapeList.push_back(shapeID);
	
	//Attach this info to the shape for identification
	fixDef->userData = shapeID;

	//add the fixture to our body, 
	return body->CreateFixture(fixDef);
	
}


IESoRWorld::IESoRWorld()
{
	
	//this->bodyList = new vector<PhysicsID*>();
	//this->shapeList = new vector<PhysicsID*>();
	//this->bodyList = new Json::Value(Json::arrayValue);
	//this->shapeList = new Json::Value(Json::arrayValue);

	// Define the gravity vector.
	b2Vec2 gravity(0.0f, -10.0f);

	// Construct a world object, which will hold and simulate the rigid bodies.
	this->world = new b2World(gravity);

	// Define the ground body.
	b2BodyDef groundBodyDef;
	groundBodyDef.position.Set(0.0f, -10.0f);

	// Call the body factory which allocates memory for the ground body
	// from a pool and creates the ground box shape (also from a pool).
	// The body is also added to the world.
	b2Body* groundBody = this->world->CreateBody(&groundBodyDef);

	// Define the ground box shape.
	b2PolygonShape groundBox;

	// The extents are the half-widths of the box.
	groundBox.SetAsBox(50.0f, 10.0f);

	// Add the ground fixture to the ground body.
	groundBody->CreateFixture(&groundBox, 0.0f);

	// Define the dynamic body. We set its position and call the body factory.
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set(0.0f, 4.0f);

	std::string bodyID = "rect1";

	b2Body* body = this->addBodyToWorld("rect1", &bodyDef);

	//int blSize = this->bodyList.size();
	//Json::Value va = *this->bodyList;
	//vector<PhysicsID*> va = *this->bodyList;

	//va.push_back(bodyID);

	//va[blSize] = bodyID;

	//this->bodyMap[std::to_string((long double)world->GetBodyCount())] = bodyID;
	//body->SetUserData(&va[blSize]);//bodyID);//va[blSize]);
	// Define another box shape for our dynamic body.
	b2PolygonShape dynamicBox;
	dynamicBox.SetAsBox(1.0f, 1.0f);

	// Define the dynamic body fixture.
	b2FixtureDef fixtureDef;
	fixtureDef.shape = &dynamicBox;

	//listCount = std::to_string((long double)this->shapeList->size());
	//va = *this->shapeList;
	//va.push_back(std::to_string((long double)bcnt));

	//blSize = this->shapeList->size();
	//va[blSize] = std::to_string((long double)bcnt);

	// Set the box density to be non-zero, so it will be dynamic.
	fixtureDef.density = 1.0f;

	// Override the default friction.
	fixtureDef.friction = 0.3f;

	this->addShapeToBody(body, &fixtureDef);

	// Add the shape to the body.
	//body->CreateFixture(&fixtureDef);
}


Json::Value positionToJSONValue(b2Vec2 vec)
{
	Json::Value pos;
	pos["x"] = vec.x;
	pos["y"] = vec.y;
	return pos;
}

Json::Value listOfPoints(b2Vec2* points, int length)
{
	Json::Value pArray(Json::arrayValue);

	for(int i=0;i<length;i++)
	{
		pArray[i] = positionToJSONValue(points[i]);
	}

	return pArray;
}

string IESoRWorld::worldDrawList()
{
	//This will be written to json
	//fastwriter is not human readable --> compact
	//exactly what we want to sent to our html files
	Json::FastWriter* writer = new Json::FastWriter();
	//root is what we'll be sending back with all the shapes (stored in shapes!)
	Json::Value root;
	Json::Value shapes;


	b2Body * B = this->world->GetBodyList();
	while(B != NULL)
	{
		if(B->GetUserData())
		{
			PhysicsID* pid = static_cast<PhysicsID*>(B->GetUserData());//*((string*)B->GetUserData());
			//std::string bodyID = *static_cast<std::string*>(B->GetUserData());//*((string*)B->GetUserData());
			std::string bodyID = pid->ID();
			//int blSize = *static_cast<int*>(B->GetUserData());
			//IESoRWorld* theWorld = static_cast<IESoRWorld*>(B->GetUserData());
			//Json::Value bodyJSON = *static_cast<Json::Value*>(B->GetUserData());
			//std::string bodyID = *static_cast<std::string*>(B->GetUserData());//*((string*)B->GetUserData());
			//string bodyID = "";//writer->write(bodyJSON);
			//get all our shapes
			b2Fixture* F = B->GetFixtureList();

			//cycle through the shapes
			while(F != NULL)
			{
				Json::Value singleShape;

				switch (F->GetType())
				{
				case b2Shape::e_circle:
					{
						b2CircleShape* circle = (b2CircleShape*) F->GetShape();                     
						/* Do stuff with a circle shape */

						Json::Value center = positionToJSONValue(circle->m_p);
						Json::Value radius = circle->m_radius;
						singleShape["type"] = "Circle";
						singleShape["center"] = center;
						singleShape["radius"] = circle->m_radius;
						singleShape["color"] = "#369";
					}
					break;
				case b2Shape::e_polygon:
					{
						b2PolygonShape* poly = (b2PolygonShape*) F->GetShape();
						/* Do stuff with a polygon shape */

						Json::Value points = listOfPoints(poly->m_vertices, poly->m_count);
						singleShape["type"] = "Polygon";
						singleShape["points"] = points;
						singleShape["color"] = "#963";

					}
					break;
				}

				//Each shape is the unique combination of 
				pid = static_cast<PhysicsID*>(F->GetUserData());//*((string*)B->GetUserData());
				
				string shapeID = pid->ID();// *((string*)F->GetUserData());

				string fullID = bodyID + "_" + shapeID;
				shapes[fullID] = singleShape;

				F = F->GetNext();
			}
		}

		B = B->GetNext();
	}    


	root["shapes"] = shapes;

	//string rString = writer->write(root);

	return writer->write(root);
	/*
	string fullJSON = get_file_contents("../../../Physics/Data/basic.json");
	return fullJSON;*/
}

IESoRWorld::~IESoRWorld()
{

}