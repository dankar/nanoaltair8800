const express = require('express');
const app = express();
const http = require('http');
const server = http.createServer(app);
const { Server } = require("socket.io");
const io = new Server(server);
const zmq = require("zeromq"),
 
sub_sock = zmq.socket("sub");
sub_sock.connect("ipc://@/nanoaltair8800.termout");
sub_sock.subscribe("DATA");

pub_sock = zmq.socket("pub");
pub_sock.bindSync("ipc://@/nanoaltair8800.termin");

app.get('/', (req, res) => {
  res.sendFile(__dirname + '/html/index.html');
});

app.get('/control/reset', (req, res) => {
  console.log("Emulator was reset");
  pub_sock.send(['CONTROL', 'RESET']);
  res.send('OK');
});

sub_sock.on("message", function(topic, message) {
	io.emit('TERM', message.toString());
});

io.on('connection', (socket) => {
  console.log('a user connected');
  socket.on('disconnect', () => {
    console.log('user disconnected');
  });
});

io.on('connection', (socket) => {
  socket.on('TERM', (msg) => {
	  pub_sock.send(["DATA", msg]);
  });
});

server.listen(3465, () => {
  console.log('listening on *:3465');
});
