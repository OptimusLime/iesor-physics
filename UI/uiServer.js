//This file is responsible for making a websocket connection to the UI, then passing information between WIN control and the UI object

var socketio = require('socket.io');

module.exports = uiSocketServer;

function uiSocketServer()
{
	var app = require('express')()
	  , server = require('http').createServer(app)
	  , io = require('socket.io').listen(server);

	  //now we have an express app with socket.io listening to our http server

	  //we'll set a port to listen to later
	  //lets attach our UI behavior

	app.get('/', function (req, res) {
		res.sendfile(__dirname + '/iesorUI.html');
	});

	io.sockets.on('connection', function (socket) {
	  socket.emit('news', { hello: 'world' });
	  socket.on('my other event', function (data) {
	    console.log(data);
	  });
	});

	return {expressApp: app, httpServer: server};	
}


