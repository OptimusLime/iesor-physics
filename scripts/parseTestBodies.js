var fs = require('fs');
var neatjs = require('neatjs');

var NeatGenome = neatjs.neatGenome;
var NeatNode = neatjs.neatNode;
var NeatConnection = neatjs.neatConnection;
var NodeType = neatjs.NodeType;



var converter = neatjs.genomeSharpToJS;
var testJSON = JSON.parse(fs.readFileSync("testBodiesAndGenomes.json"));
var genomes = testJSON.genomes;

var NeuronTypeToNodeType = function(type)
{
    switch(type)
    {
        case 1:
            return NodeType.bias;
        case 0:
            return NodeType.input;
        case 3:
            return NodeType.output;
        case 2:
            return NodeType.hidden;
        default:
            throw new Error("inpropper C# neuron type detected");
    }
};
var convertCSharpJSONToJS = function(jsonGenome)
{

    //we need to parse through a c# version of genome, and make a js genome from it

    var aNeurons = jsonGenome.NeuronGeneList;
    var aConnections = jsonGenome.ConnectionGeneList;
	
	console.log(aNeurons.length);
	console.log(aConnections.length);


    //we will use nodes and connections to make our genome
    var nodes = [], connections = [];
    var inCount = 0, outCount = 0;

    for(var i=0; i < aNeurons.length; i++)
    {
        var csNeuron = aNeurons[i];
		//console.log(csNeuron);
        var jsNode = new NeatNode(csNeuron.InnovationId, csNeuron.ActivationFunction.FunctionId, csNeuron.Layer, {type: NeuronTypeToNodeType(csNeuron.NeuronType)});
        nodes.push(jsNode);

        if(csNeuron.type == 0) inCount++;
        else if(csNeuron.type == 3) outCount++;
    }

    for(var i=0; i < aConnections.length; i++)
    {
        var csConnection = aConnections[i];
        var jsConnection = new NeatConnection(csConnection.InnovationId, csConnection.Weight, {sourceID:csConnection.SourceNeuronId, targetID: csConnection.TargetNeuronId});
        connections.push(jsConnection);
    }
	
	console.log(nodes.length);
	console.log(connections.length);

    var ng = new NeatGenome(jsonGenome['GenomeId'], nodes, connections, inCount, outCount);
    ng.adaptable = (jsonGenome['networkAdaptable'] == 'True');
    ng.modulated = (jsonGenome['networkModulatory'] == 'True');
    ng.fitness = jsonGenome['Fitness'];
    ng.realFitness = jsonGenome['RealFitness'];
    ng.age = jsonGenome['GenomeAge'];

    return ng;
};


function convertOldFormat(bodyInfo)
{
	var nFormat = {};
 
 
	nFormat["useLEO"] = bodyInfo.useLEO;
	nFormat["allBodyOutputs"] = bodyInfo.AllBodyOutputs;
	nFormat["allBodyInputs"] = bodyInfo.AllBodyInputs;
		
	var inLocations = bodyInfo.InputLocations;
	var hidLocations = bodyInfo.HiddenLocations;

	var nodes = [];

	if (bodyInfo.InputLocations != null)
		nodes = nodes.concat(bodyInfo.InputLocations);

	if (bodyInfo.HiddenLocations != null)
		nodes = nodes.concat(bodyInfo.HiddenLocations);

	var jNodes = [];
	for(var i=0; i < nodes.length; i++)
	{
		var n = nodes[i];
		var pt = {x: n.X, y: n.Y};
		jNodes.push(pt);
	}
	
	//save our nodely information
	nFormat["nodes"] = jNodes;


	var connections = bodyInfo.Connections;
	var jConnections = [];
	
	for(var i=0; i < connections.length; i++)
	{
		var c = connections[i];
		
		var cAdjust = {};
		cAdjust["sourceID"] = "" + c.SourceNeuronId;
		cAdjust["targetID"] = "" + c.TargetNeuronId;
		cAdjust["cppnOutputs"] = c.cppnOutputs;

		jConnections.push(cAdjust);
	}

	//save dem connections yo
	nFormat["connections"] = jConnections;
	
	return nFormat;
}
var gCount = genomes.length;
for(var i=0; i < gCount; i++)
{
	var gObject = genomes[i];
	
	var cSharpGenome = gObject.genome;
	//console.log(cSharpGenome);
	var body = gObject.body;
	
	//need to convert cSharpGenome to js genome
	var jsBody = convertOldFormat(body);
	var jsGenome = convertCSharpJSONToJS(cSharpGenome);

		//now write genome and body to files-- identify with gid
	var genomeSave = "./generated/jsGenome" + jsGenome.gid + ".json";
	fs.writeFileSync(genomeSave, JSON.stringify(jsGenome));
		
	var bodySave = "./generated/jsBody" + jsGenome.gid + ".json";		
	fs.writeFileSync(bodySave, JSON.stringify(jsBody));
}
