var express = require('express');
var socket=require('socket.io');
var app = express();

app.get('/', function (req, res) {

   res.send('Hello World from');
})

console.log("server started")

var server=app.listen(8080, function() {
    console.log('Listening to port:  ' + 8080);
});

var io=socket(server);

io.on('connection',function (ws) {

	ws.on('message',function (message) {
		console.log("Received from the client: "+message);

		io.emit('chat message', message);


	})
	// body...
})
