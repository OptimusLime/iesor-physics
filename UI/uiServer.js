//This file is responsible for making a websocket connection to the UI, then passing information between WIN control and the UI object

var express = require('express');
var socketio = require('socket.io');

module.exports = uiSocketServer;

function uiSocketServer()
{
	var self = this;

	var app = express()
	  , server = require('http').createServer(app)
	  , io = require('socket.io').listen(server);


	  //disable verbose logging for our heavy traffic webscoket
	  io.set('log level', 1);
	  //now we have an express app with socket.io listening to our http server

	  //we'll set a port to listen to later
	  //lets attach our UI behavior

	app.get('/', function (req, res) {
		res.sendfile(__dirname + '/iesorUI.html');
	});

	app.use("/js", express.static(__dirname + '/js'));

	
	var socketID = 0;
	var socketCollection = {};
	var socketCallbacks = {};
	var eventList = {};

	var wrapSocketCallback = function(socketID, callback)
	{
		return function()
		{
			//we insert our arguments at the beginning of the old arguments
			[].splice.call(arguments, 0,0, socketID);

			//pass the arguments along with the socket info
			callback.apply(this, arguments);
		}
	}

	var applyEventsToSingleSocket = function(socketID)
	{
		//grab our socket
		var socket = socketCollection[socketID];
		//grab all registered events for that socket
		var sCallback = socketCallbacks[socketID];

		//loop through all our events
		for(var eventName in eventList)
		{
			//if we haven't registered this callback yet
			//then add it to our list for the socket!
			if(!sCallback[eventName]){
				//wrap the call with the socketID and pass it the callback inside eventlist
				var wrappedCall = wrapSocketCallback(socketID, eventList[eventName]);
				socket.on(eventName, wrappedCall);
				sCallback[eventName] = wrappedCall;
			}
		}
	}

	var removeEventsFromSingleSocket = function(socketID)
	{
		//grab our socket
		var socket = socketCollection[socketID];
		var sCallbacks = socketCallbacks[socketID];

		//loop through all our events
		for(var eventName in sCallbacks)
		{
			//remove the event and it's callback function
			socket.removeListener(eventName, sCallbacks[eventName]);
		}
	}

	var applyEventsToSockets = function()
	{
		for(var socketID in socketCollection)
			applyEventsToSingleSocket(socketID);			
	}

	self.addSocket = function(s)
	{
		s.sID = socketID++;
		socketCollection[s.sID] = s;
		socketCallbacks[s.sID] = {};

		//apply callbacks to new socket
		applyEventsToSingleSocket(s.sID);
	}

	self.removeSocket = function(s)
	{
		//pull off all the callbacks from the sockets (don't want zombie emit calls)
		removeEventsFromSingleSocket(s.sID);

		//then remove it permanently
		delete socketCollection[s.sID];
		delete socketCallbacks[s.sID];
	}

	self.registerEvent = function(eventName, callback)
	{
		if(registerEvent[eventName])
			throw new Error("Cannot register multiple callbacks for the same event name. ");

		eventList[eventName] = callback;

		//anytime you register a new event, we apply our callbacks to the sockets
		//don't worry, no dupe callbacks by design
		applyEventsToSockets();
	}

	io.sockets.on('connection', function (socket) {
	  
	  self.addSocket(socket);

	  socket.on('disconnect', function()
	  {
	  	console.log('Disconnected: ', socket.sID);
	  	removeSocket(socket);
	  })

	});

	return {expressApp: app, httpServer: server, uiControl: self};	
}


