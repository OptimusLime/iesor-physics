var neatjs = require('neatjs');
var fs = require('fs');
var xml2js = require('xml2js');

module.exports = xmlConverter;

var converter = neatjs.genomeSharpToJS;
var attrkey = '$';

function xmlConverter(filePath, callback)
{
	var parser = new xml2js.Parser({explicitArray : false, attrkey: attrkey, mergeAttrs: true});

	parser.addListener('end', function(result) {

		var genome = result['genome'];

		//now we should have a defined genome, let's pass it along
		var ng = converter.ConvertCSharpToJS(genome);
		
		
		callback(null, ng, filePath);	

	});
	
	fs.readFile(filePath, 'utf8', function (err,data) {
		if (err) {
			console.log('Err data:');
			console.log(err);
			callback(err);
			return;
		}
		
		//we need to parse the data, and create the genome!
		parser.parseString(data);

	});	
}



 

