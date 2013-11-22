//this will partially test our genomeToBody construction logic (build appropriate GPU shaders for the job)
var gpu = require('./gpu.js');
var fs = require('fs');
var ng = JSON.parse(fs.readFileSync('./jsGenome142856.json'));

var shadersSoFar = gpu.NeatGenomeToByteCode(ng);

console.log(shadersSoFar.nodeArrays);
//console.log('Frag shader: \n\n\n');
//console.log(shadersSoFar.fragment);
