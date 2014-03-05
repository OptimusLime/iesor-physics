// This loads our extension in the iesor variable. 
// It will only load a constructor function, notify.notification().
var iesor = require("../build/Release/iesorworld.node"); // path to our extension

var fs = require('fs');
var iWorld = new iesor.iesorWorld();

//here we load a json genome, and inject it into the world
//var sGenome = fs.readFileSync('./sampleGenome.json');

var s = iWorld.loadBody("echo here?");//sGenome.toString());

console.log("Ret val: ", s);

//set up a timer for simulation
//simulate, then ask it for a world description each time step. 


