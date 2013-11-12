#include "iesorWorld.h"

#include <iostream>
#include <fstream>
#include <string>

//including our json info for easy access
using namespace Json;
using namespace std;

IESoRWorld::IESoRWorld()
{

}
// Stores the trimmed input string into the given output buffer, which must be
// large enough to store the result.  If it is too small, the output is
// truncated.
size_t trimwhitespace(char *out, size_t len, const char *str)
{
	if(len == 0)
		return 0;

	const char *end;
	size_t out_size;

	// Trim leading space
	while(isspace(*str)) str++;

	if(*str == 0)  // All spaces?
	{
		*out = 0;
		return 1;
	}

	// Trim trailing space
	end = str + strlen(str) - 1;
	while(end > str && isspace(*end)) end--;
	end++;

	// Set output size to minimum of trimmed string length and buffer size minus 1
	out_size = (end - str) < len-1 ? (end - str) : len-1;

	// Copy trimmed string and add null terminator
	memcpy(out, str, out_size);
	out[out_size] = 0;

	return out_size;
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



void IESoRWorld::hamWash()
{
	
	//do nuffin!
	string fullJSON = get_file_contents("../../../Data/basic.json");
	printf("JSON: %s\n", fullJSON.c_str());
	processJSONStream(fullJSON);

	//ifstream myfile("basic.json");
	//ifstream myfile(); //("example.json");
	//if (myfile.is_open())
	//{


	//}

	//else cout << "Unable to open file";  


}
IESoRWorld::~IESoRWorld()
{

}