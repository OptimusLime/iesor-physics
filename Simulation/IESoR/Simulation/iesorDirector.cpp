#include "iesorDirector.h"
#include <IESoR/iesor.h>
#include <json.h>

static Json::Reader reader;
static Json::FastWriter writer;

IESoRDirector::IESoRDirector()
{
	//create a generic world for us to manage
	world_ = new IESoRWorld();
}

std::string json2String(std::string s)
{
	//return a non styled json object
	return writer.write(s);
}

Json::Value string2JSON(std::string s)
{
	Json::Value root;
	//return a non styled json object
	reader.parse(s,root);
	return root;
}

std::map<std::string, double> IESoRDirector::insertBodyFromNetwork(std::string& network)
{
	Json::Value val = string2JSON(network);
	return insertBodyFromNetwork(val);
}

std::map<std::string, double> IESoRDirector::insertBodyFromNetwork(Json::Value& byteNetwork)
{

	//build class capable of generating a body from a network
	iesorBody* bodyCreator = new iesorBody(byteNetwork);

	//in the async version, this is the call we would be doing in a worker thread
	Json::Value fullBody = bodyCreator->buildBody();

	//   //This is a temporary piece -- in reality, we should return a local v8 object to JS
	// Json::StyledWriter writer;
	// //For now, we simply write the body to a json string
	// std::string outputConfig = writer.write(fullBody);

	// printf("%s", outputConfig.c_str());

	//now we specifically load the generated body into the world
	std::map<std::string, double> morphology = world_->loadBodyIntoWorld(fullBody);

	//if we aren't check the morph, we can skip the temp variable
	//either way, return the result
	return morphology;
	// for(std::map<std::string, double>::iterator iterator = morphology.begin(); iterator != morphology.end(); iterator++) {

	// 	printf("Morph Key: %s \n", iterator->first.c_str());
	// 	printf("Morph Value: %f \n", iterator->second);
	// }

}
