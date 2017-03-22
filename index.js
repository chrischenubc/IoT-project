var express = require('express');
var socket=require('socket.io');
var app = express();

app.get('/', function (req, res) {
   res.send('Hello World from !!!!');
})

var server=app.listen(8080, '206.87.193.27', function() {
    console.log('Listening to port:  ' + 8080);
});

var io=socket(server);

io.sockets.on('connection',function (ws) {

	ws.on('message',function (message) {
		console.log("Received from the client: "+message);
		
		ws.send(message);
		
		
	})
	// body...
})
