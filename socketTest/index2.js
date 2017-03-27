/**
 * Created by sreejeshpillai on 09/05/15.
 */
var express = require('express');
var app = express();
var server = app.listen(3000);

app.use(express.static('public'));

console.log("Server is running");

var socket = require('socket.io');

var io = socket(server);

var id = [];
var pw = [];

io.sockets.on('connection', newConnection);

function newConnection(socket) {
	console.log('new connection: ' + socket.id )
	
	socket.on('disconnect', cutConnection);
	
	function cutConnection() {
		console.log('disconnection at: ' + socket.id);
	}
	
	socket.on('register', registerUser);
	
	function registerUser (data) {
		console.log('message at: ' + socket.id);
		var splitString = data.split(/[ ,]+/);
		id.push(splitString[0]);
		console.log('id added to database');
		pw.push(splitString[1]);
		console.log('password added to database');
	}
	
	socket.on('login', logInVerify);
	
	function logInVerify(data) {
		var splitString = data.split(/[ ,]+/);
		if( id.includes(splitString[0]) && pw.includes(splitString[1]) ) {
				socket.broadcast.emit('login', "success");
		} else {
			socket.broadcast.emit('login', "fail");
		}	
	}
	
	socket.on('getTemp', updateTemp);
	
	function updateTemp(data) {
		console.log('getting temperature');
	}
}