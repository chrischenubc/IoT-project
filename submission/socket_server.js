//import libarais, incliding express and socket.io framework
var app = require('express')();
var http = require('http').Server(app);
var io = require('socket.io')(http);
var temp, humidity, light, distance;

//dealing with the http request and sent the index.html to clients
app.get('/',function(req,res){
    res.sendFile(__dirname+'/index.html');
})

//make the server listen to port 9000
http.listen(9000,function(){
    console.log('server listening on port 9000');
})

//once the web socket connection is on
io.on('connection',function(socket){
    //log in the console that there is a client connected
    console.log('one user connected '+socket.id);

    //log in the console that there is a client disconnectted
    socket.on('disconnect',function(){
        console.log('one user disconnected '+socket.id);
    })

    //deal with 'startTime' event
		socket.on('startTime', function(data) {
			setInterval(function(){
			io.sockets.emit("TimeAndroid", 'hi');
		}, 10000);
		});

    //deal with 'LCDtoServer' event
		socket.on('LCDtoServer', function(data) {
			io.sockets.emit('LCDAndroid', data);
		});

        //request from html, send info to android
        socket.on('led', function(data) {
                console.log("led on/off");
                io.sockets.emit('ledToAndroid', 'led on/off');
        });

        //request from html, send info to android
        socket.on('piezo', function(data) {
                console.log("piezo on/off");
                io.sockets.emit('piezoToAndroid', 'led on/off');
        });

        //request from html, send info to android
        socket.on('motor', function(data) {
                console.log("motor on/off");
                io.sockets.emit('motorToAndroid', 'motor on/off');
        });

        //request from html, send info to android
        socket.on('Automode', function(data) {
          console.log("piezo on/off");
          io.sockets.emit('AutoModeAndroid', 'automode on/off');
        });

        //request from html, send info to android
        socket.on('Reset', function(data) {
          console.log("piezo on/off");
          io.sockets.emit('ResetAndroid', 'automode on/off');
        });

        //request from html, send info to android
        socket.on('turnOff', function(data) {
          console.log("piezo on/off");
          io.sockets.emit('TurnOffAndroid', 'turn on/off');
        });

        //request from android, send info to html
    		socket.on('tempToServer', function(data) {
    			temp = data;
    			//console.log("Received temperature" + data);
    			io.sockets.emit('temp', data);
    		});

        //request from android, send info to html
    		socket.on('lightToServer', function(data) {
    			light = data;
    			//console.log("Received photosensor" + data);
    			io.sockets.emit('light', data);
    		});

        //request from android, send info to html
    		socket.on('distanceToServer', function(data) {
    			distance = data;
    			//console.log("Received temperature" + data);
    			io.sockets.emit('distance', data);
    		});

        //request from android, send info to html
    		socket.on('humidityToServer', function(data) {
    			humidity = data;
    			//console.log("Received temperature" + data);
    			io.sockets.emit('humidity', data);
    		});

    		//request from PI
    		socket.on('tempFromPi', function(data) {
    			console.log(data);
    			io.sockets.emit('tempFromServer', temp);	//send to android
    		});

    		socket.on('message', function(msg){
    			//io.emit('chat', msg);
    			socket.broadcast.emit('chat', msg);
    		});

        socket.on('message-arduino', function(msg){
    			//io.emit('chat', msg);
    			socket.broadcast.emit('chat-arduino-android', msg);
    		});
})
