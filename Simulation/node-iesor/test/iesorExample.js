// This loads our extension in the iesor variable. 
// It will only load a constructor function, notify.notification().
var iesor = require("../build/Release/iesorworld.node"); // path to our extension

var testGenome = require("./gpuTest.js");

var tgString = JSON.stringify(testGenome);

var fs = require('fs');
var iWorld = new iesor.iesorWorld();

//here we load a json genome, and inject it into the world
//var sGenome = fs.readFileSync('./sampleGenome.json');

var s = iWorld.loadBodyFromNetwork(tgString);//sGenome.toString());

var body = JSON.parse(s);
console.log("Generated body: ", body);


//simulate for 1/2 second
var simTimeMS = 500;

//tell the wrold to simulate
var simInfo = iWorld.simulateWorldMS(simTimeMS);


console.log('Sim info: ', JSON.parse(simInfo));

var drawInfo = iWorld.getWorldDrawList();

console.log("Draw it: ", JSON.parse(drawInfo));




// console.log("Ret val: ", s);

//set up a timer for simulation
//simulate, then ask it for a world description each time step. 


