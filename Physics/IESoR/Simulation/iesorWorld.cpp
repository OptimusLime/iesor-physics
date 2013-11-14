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

PhysicsID* IESoRWorld::createShapeID()
{
	//We just keep a static int of all the shapes for all identification
	int bcnt = staticBodyCount++;

	//Track the shape string
	PhysicsID* shapeID = new PhysicsID(std::to_string((long double)bcnt));

	//Save our shape to the list of shapes
	this->shapeList.push_back(shapeID);

	return shapeID;
}

void setShapeID(b2Fixture* fix, PhysicsID* shapeID)
{
	fix->SetUserData(shapeID);
}

b2Fixture* IESoRWorld::addShapeToBody(b2Body*body, b2Shape* shape, float density)
{
	//create physics id based on global shape count
	PhysicsID* pid = createShapeID();

	//add the fixture to our body, 
	b2Fixture* fix = body->CreateFixture(shape, density);

	//Attach this info to the shape for identification
	setShapeID(fix, pid);

	//send back created fixture
	return fix;
}

b2Fixture* IESoRWorld::addShapeToBody(b2Body* body, b2FixtureDef* fixDef)
{
	//create physics id based on global shape count
	PhysicsID* pid = createShapeID();

	//add the fixture to our body using the definition
	b2Fixture* fix = body->CreateFixture(fixDef);

	//Attach this info to the shape for identification
	setShapeID(fix, pid);

	//send back created fixture
	return fix;
}


IESoRWorld::IESoRWorld()
{
	this->interpolation =0;
	accumulator = 0;
	desiredFPS = 30;
	simulationRate = 1.0 / desiredFPS;
	radians = 0;
	
	//this->bodyList = new vector<PhysicsID*>();
	//this->shapeList = new vector<PhysicsID*>();
	//this->bodyList = new Json::Value(Json::arrayValue);
	//this->shapeList = new Json::Value(Json::arrayValue);

	// Define the gravity vector.
	b2Vec2 gravity(0.0f, -10.0f);

	// Construct a world object, which will hold and simulate the rigid bodies.
	this->world = new b2World(gravity);
	this->world->SetAutoClearForces(false);

	// Define the ground body.
	b2BodyDef groundBodyDef;
	groundBodyDef.type = b2_staticBody;
	groundBodyDef.position.Set(0.0f, -18.0f);

	// Call the body factory which allocates memory for the ground body
	// from a pool and creates the ground box shape (also from a pool).
	// The body is also added to the world.
	b2Body* groundBody = this->addBodyToWorld("ground", &groundBodyDef);//this->world->CreateBody(&groundBodyDef);

	//b2Body* groundBody = this->world->CreateBody(&groundBodyDef);


	// Define the ground box shape.
	b2PolygonShape groundBox;

	// The extents are the half-widths of the box.
	groundBox.SetAsBox(50.0f, 10.0f);

	// Add the ground fixture to the ground body.
	this->addShapeToBody(groundBody, &groundBox, 0.0f);
	//groundBody->CreateFixture(&groundBox, 0.0f);
	
	// Define the dynamic body. We set its position and call the body factory.
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set(0.0f, 24.0f);

	//add body to world using definition
	b2Body* body = this->addBodyToWorld("rect1", &bodyDef);

	// Define another box shape for our dynamic body.
	b2PolygonShape dynamicBox;
	dynamicBox.SetAsBox(5.0f, 5.0f);

	// Define the dynamic body fixture.
	b2FixtureDef fixtureDef;
	fixtureDef.shape = &dynamicBox;
	fixtureDef.restitution = .5;

	// Set the box density to be non-zero, so it will be dynamic.
	fixtureDef.density = 1.0f;

	// Override the default friction.
	fixtureDef.friction = 0.3f;

	// Add the shape to the body.
	this->addShapeToBody(body, &fixtureDef);

	//Add some forces!
	//body->ApplyAngularImpulse(.4, true);
	
	body->ApplyTorque(150, true);
	b2Vec2 pulse(70, 0);
	b2Vec2 o(0,3);
	body->ApplyLinearImpulse(pulse, o, true);

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
			//fetch our body identifier
			PhysicsID* pid = static_cast<PhysicsID*>(B->GetUserData());//*((string*)B->GetUserData());
			std::string bodyID = pid->ID();
			
			//we must get all our shapes
			b2Fixture* F = B->GetFixtureList();

			//cycle through the shapes
			while(F != NULL)
			{
				//Hold our shape drawing information
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
						singleShape["bodyOffset"] = positionToJSONValue(B->GetPosition());
						singleShape["rotation"] = B->GetAngle();
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
						singleShape["bodyOffset"] = positionToJSONValue(B->GetPosition());
						singleShape["rotation"] = B->GetAngle();
						singleShape["color"] = "#38F";

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

	return writer->write(root);
}


int IESoRWorld::updateWorld(double msUpdate)
{
    int stepCount = 0;

    //# of seconds since last time
    double frameTime = msUpdate/1000.0f;

    //maximum frame time, to prevent what is called the spiral of death
    if (frameTime > .35)
        frameTime = .35;


    //we don't need last time anymore, set it to the current time
    //this.lastTime = currentTime;

    //we accumulate all the time we haven't rendered things in
    this->accumulator += frameTime;

    //        console.log('Frame time: ' + frameTime + ' accumulator: ' + accumulator);

    //        console.log('Pre acc');
    while (accumulator >= simulationRate)
    {
        stepCount++;
        //push the muscles outward a bit
        float speedup = 3;

        /*for (int i = 0; i < this->muscles.size(); i++)
        {
            Muscle* muscle = this.muscles[i];
            int oLength = muscle.Joint.GetLength();
            int lengthCalc = (1 + muscle.GetAmplitude() * (float)cos(radians + muscle.GetPhase() * 2 * Math.PI)) * muscle.GetOriginalLength();
            muscle.Joint.SetLength(lengthCalc);
        }*/

        //step the physics world
        this->world->Step(
            this->simulationRate   //frame-rate
            , 10       //velocity iterations
            , 10       //position iterations
        );


		this->world->ClearForces();

        //increment the radians for the muscles
        radians += speedup * this->simulationRate;

        //decrement the accumulator - we ran a chunk just now!
        accumulator -= this->simulationRate;

    }
            
    this->interpolation = accumulator / this->simulationRate;

    return stepCount;//{stepCount: stepCount, deltaChange: (Date.now() - currentTime), log:logEvents}; 
}

IESoRWorld::~IESoRWorld()
{

}