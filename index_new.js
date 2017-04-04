var app = require('express')();
var http = require('http').Server(app);
var io = require('socket.io')(http);
var temp, humidity, light, distance;

app.get('/',function(req,res){
    res.sendFile(__dirname+'/index.html');
})
io.on('connection',function(socket){
    console.log('one user connected '+socket.id);
    /*socket.on('message',function(data){
                console.log(data);
        var sockets = io.sockets.sockets;
        sockets.forEach(function(sock){
            if(sock.id != socket.id)
            {
                sock.emit('message',data);
            }
        })
        //socket.broadcast.emit('message', {message:data});
                sockets.emit('update', data);
    })
        */

    socket.on('disconnect',function(){
        console.log('one user disconnected '+socket.id);
    })
	
		socket.on('startTime', function(data) {
			setInterval(function(){
			io.sockets.emit("TimeAndroid", 'hi');
		}, 10000);
		});
		
		
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

        socket.on('Automode', function(data) {
          console.log("piezo on/off");
          io.sockets.emit('AutoModeAndroid', 'automode on/off');
        });

        socket.on('Reset', function(data) {
          console.log("piezo on/off");
          io.sockets.emit('ResetAndroid', 'automode on/off');
        });

        socket.on('turnOff', function(data) {
          console.log("piezo on/off");
          io.sockets.emit('TurnOffAndroid', 'turn on/off');
        });


		socket.on('tempToServer', function(data) {
			temp = data;
			//console.log("Received temperature" + data);
			io.sockets.emit('temp', data);
		});

		socket.on('lightToServer', function(data) {
			light = data;
			//console.log("Received photosensor" + data);
			io.sockets.emit('light', data);
		});

		socket.on('distanceToServer', function(data) {
			distance = data;
			//console.log("Received temperature" + data);
			io.sockets.emit('distance', data);
		});

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

http.listen(9000,function(){
    console.log('server listening on port 3000');
})
