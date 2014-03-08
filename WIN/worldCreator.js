// This loads our extension in the iesor variable. 
// It will only load a constructor function, notify.notification().
var iesor = require("../Simulation/node-iesor/build/Release/iesorworld"); // path to our extension

var Q = require('q');

var neatNetworkConverter = require("./neatNetworkConverter.js");

module.exports = worldCreator();

function worldCreator()
{
	var self = this;

	self.qCreateWorld = function(genomeJSON)
	{
		var defer = Q.defer();

		var reject = function() { defer.reject.apply(defer, arguments); }
		var success = function() { defer.resolve.apply(defer, arguments); }


		//todo catch errors in body gen
		process.nextTick(function()
		{
			try
			{
				var iWorld = new iesor.iesorWorld();
				//convert from genome json info, into an actual network to be sent to C++
				var networkForBody = neatNetworkConverter.NeatGenomeToByteCode(genomeJSON);

				//call native C++ with network, in the future this iwll take a callback for when it's done
				var s = iWorld.loadBodyFromNetwork(JSON.stringify(networkForBody));

				//pass back info about the body morphology
				success({world: iWorld, morphology: JSON.parse(s)});
			}
			catch(e)
			{
				console.log('Error in create: ', e);
				reject(e);
			}

		});

		return defer.promise;		
	}	

	//send back the creator!
	return self;

}


