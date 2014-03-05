var path = require('path');
var fs = require('fs');
var ngConverter = require('./ngConvert.js');

var dataJSON = JSON.parse(fs.readFileSync("data.json"));

var allFiles = fs.readdirSync('../');

var saveJSON = {};

//var fLength = allFiles.length;

function scanFiles(fStart, fEnd, callback)
{
	var openCallbacks = fEnd-fStart;

	for(var i=fStart; i < fEnd; i++)
	{
		var filePath = allFiles[i];
		
		console.log(filePath);
		
		if(filePath.indexOf('.xml') !== -1 && filePath.indexOf('evoInformation') == -1 && filePath.indexOf('neatParameters') == -1 && filePath.indexOf('NeatParameters') == -1)
		{
			//we have an xml file!
			ngConverter(path.resolve(__dirname, ("../" + filePath)), function(err, ng, fp)
			{
				
				console.log(ng.gid);
				if(!err)
				{
				
					var cppn = ng.networkDecode();
		
					try{
					   var shader = cppn.fullShaderFromCPPN(function(){ return [];});
					   
						//successful non-recurrent shaders go on to make their mark!
					   if(dataJSON[ng.gid] != undefined)
						{
							saveJSON[ng.gid] = fp;
						}
					   
					 }
					catch(e)
					{
						
					}
		
				
				
					
				}			
				openCallbacks--;
				
				if(openCallbacks == 0)
				{			
					callback();
				}
			});		
		}
		else
			openCallbacks--;
	}
}

var s = 0; 
var chunk = 500;//1000;
var max = allFiles.length;

var inProgress = false;
var completed = 0;


//setTimeout(

var loopTillComplete = function()
{	
	if(!inProgress){
		var end = Math.min(s + chunk, max);
		inProgress = true;
		scanFiles(s, end, function()
		{
			var done = (end-s);
			completed += done;
			s += done;

			console.log('Completed chunk: ' + done + ' Total: ' + completed + ' New Start: ' + s);
			inProgress = false;			
		});
	}
	
	if(completed < max)
		setTimeout(loopTillComplete, 5);
	else
	{
		fs.writeFileSync("genomeIDs.json", JSON.stringify(saveJSON));
		console.log('Finished scanning!');
	}
}

setTimeout(loopTillComplete, 0);





