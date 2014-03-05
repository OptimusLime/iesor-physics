var fs = require('fs');
var ngConvert = require('./ngConvert.js');


var jsonIDs = JSON.parse(fs.readFileSync("genomeIDs.json"));
var jsonBodies = JSON.parse(fs.readFileSync("data.json"));

var desiredIDs = ["142856"];

function convertOldFormat(bodyInfo)
{
	var nFormat = {};
 
 
	nFormat["useLEO"] = bodyInfo.useLEO;
 
		
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



for(var i=0; i < desiredIDs.length; i++)
{
	var dID = desiredIDs[i];
	var genomeFilePath = jsonIDs[dID];
	console.log('Converting: ' + genomeFilePath);
	
	ngConvert(genomeFilePath, function(err, ng)
	{
		var gSave = "jsGenome" + ng.gid + ".json";
		fs.writeFileSync(gSave, JSON.stringify(ng));
		
		var bSave = "jsBody" + ng.gid + ".json";
		
		var nBody = convertOldFormat(jsonBodies[ng.gid]);
		
		fs.writeFileSync(bSave, JSON.stringify(nBody));
		
		
		console.log('Saved: ' + gSave + ' and ' + bSave);
		
	});	
}