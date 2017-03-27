/**
 * Created by sreejeshpillai on 09/05/15.
 */
var app = require('express')();
var http = require('http').Server(app);
var io = require('socket.io')(http);
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
	
	//request from html, send request to Android
	socket.on('request', function(data) {
		//console.log(data);
		io.sockets.emit('getUpdateInfo', 'getUpdateInfo');	//send to android
	});
	
	//request from Android, send info to html
	socket.on('givenUpdateInfo', function(data) {
		/*console.log("received update info from Android");
		var splitString = data.split(/[ ,]+/);
		console.log(splitString);
		*/
		io.sockets.emit('update', data);
	});
	
	//request from html, send info to android
	socket.on('led', function(data) {
		console.log("led on/off");
		io.sockets.emit('ledToAndroid', 'led on/off');
	});
	
	//request from html, send info to android
	socket.on('piezo', function(data) {
		console.log("piezo on/off");
		io.sockets.emit('piezo', 'led on/off');
	});
	
	socket.on('tempToServer', function(data) {
		console.log("Received temperature" + data);
		io.sockets.emit('temp', data);
	});
})



http.listen(3000,function(){
    console.log('server listening on port 3000');
})
