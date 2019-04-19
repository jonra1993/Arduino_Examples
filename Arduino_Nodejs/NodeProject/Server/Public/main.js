const socket = io();
//var socket = io('http://localhost/');
const temperatureDisplay = document.getElementById('temperature');




socket.on('connect', function () {
  socket.on('temp', function (data) {
    console.log(data);
    temperature.innerHTML = `${data}°C`;
    socket.emit('retorno', "Hello");
  });
});
