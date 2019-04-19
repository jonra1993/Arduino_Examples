const http = require('http');
const express = require ('express');
const socketIo = require('socket.io');

const app = express();
const server = http.createServer(app);
const io = socketIo.listen(server);

app.use(express.static(__dirname + '/public'));
server.listen(3000, function(){
    console.log('server listening on port 3000')
})


io.on('connection', function (socket) {
    socket.on('retorno', function (data) {
        //console.log(data)
        escribir(data);
     });
});


//Serial Communication
const serialPort = require('serialport');
const readLine = serialPort.parsers.Readline;

const port = new serialPort('COM6',{
    baudRate:19200
});

const parser=port.pipe(new readLine({delimeter: '\r\n'}));

parser.on('open',function(){
    console.log('Conexión abierta');
});

parser.on('data',function(data){
    let temp = parseInt(data,10) + " °C";
    //console.log(temp);
    io.emit('temp', data.toString());
});

parser.on('error',function(err){
    console.log(err);
});

port.on('error', function(err){
    console.log(err);
});

function escribir(data){
    port.write(data+'\n', function(err) {
        if (err) {
          return console.log('Error on write: ', err.message)
        }
        //console.log('message written')
      });
}



  


