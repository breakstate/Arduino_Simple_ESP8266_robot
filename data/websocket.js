
var connection = new WebSocket('ws://' + location.hostname + ':82/', ['arduino']);

connection.onopen = function () {
    connection.send('Connect ' + new Date());
  };
  connection.onerror = function (error) {
    console.log('WebSocket Error ', error);
  };
  connection.onmessage = function (e) {
    console.log('Server: ', e.data);
  };
  connection.onclose = function () {
    console.log('WebSocket connection closed');
  };

function setInstruction(id) {
    var instruction;
    switch(id){
        case "Forward" :
            instruction = 'F';
            break;
        case "Stop" :
            instruction = 'S';
            break;
        case "Backward" :
            instruction = 'B';
            break;
        case "Left" :
            instruction = 'L';
            break;
        case "Right" :
            instruction = 'R';
            break;
    }
    connection.send(instruction);
}

function changeCamSource(id) {
  var camSource;
  if (id == "btnWAN") {
    camSource = "http://102.65.87.193:8080/video"
  } else {
    camSource = "http://192.168.1.13:8080/video"
  }
  var video = document.getElementById("video");
  video.src = camSource;
}