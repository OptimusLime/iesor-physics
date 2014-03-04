var net = require('net');

var hCount = 0;
var connIDs = 0;
var cHeartBeats = {};

var server = net.createServer(function(c) { //'connection' listener
  console.log('server connected');

  c.id = connIDs++;
  cHeartBeats[c.id] = true;

  c.on('data', function(data)
  {
  	console.log('Receiving: ', data.toString());
  })
  c.on('end', function() {
   	cHeartBeats[c.id] = false;
    console.log('server disconnected');
  });
  
  setInterval(function()
  {
  	if(cHeartBeats[c.id])
  		c.write('honk check ' + (hCount++) + '\r\n');
  }, 2000)

  // c.pipe(c);
  
});

server.listen(8001, function() { //'listening' listener
  console.log('server bound');
});




 

