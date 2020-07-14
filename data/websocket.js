

document.addEventListener('keydown', function(event) 
{ 
const key = event.key; 
switch (key) { 
case "ArrowLeft": 
document.getElementById("keyOutput").innerHTML = 'Left'; 
console.log("left");
setInstruction("Left");
break; 
case "ArrowRight": 
document.getElementById("keyOutput").innerHTML =  'Right';
setInstruction("Right");
break; 
case "ArrowUp": 
document.getElementById("keyOutput").innerHTML =  'Up'; 
setInstruction("Forward");
break; 
case "ArrowDown": 
document.getElementById("keyOutput").innerHTML =  'Down'; 
setInstruction("Backward");
break; 
case " ": 
document.getElementById("keyOutput").innerHTML =  'Space'; 
setInstruction("Stop");
break; 
} 
}); 

document.addEventListener('keyup', function(event) 
{ 
const key = event.key; 
switch (key) { 
  case "ArrowLeft": 
  document.getElementById("keyOutput").innerHTML = 'Left up'; 
  console.log("left up");
  setInstruction("Stop");
  break; 
  case "ArrowRight": 
  document.getElementById("keyOutput").innerHTML =  'Right up';
  setInstruction("Stop");
  break; 
  case "ArrowUp": 
  document.getElementById("keyOutput").innerHTML =  'Up up'; 
  setInstruction("Stop");
  break; 
  case "ArrowDown": 
  document.getElementById("keyOutput").innerHTML =  'Down up'; 
  setInstruction("Stop");
  break; 
  case " ": 
  document.getElementById("keyOutput").innerHTML =  'Space'; 
  setInstruction("Stop");
  break; 
  }
  setInstruction("Stop");
  setInstruction("Stop");
  setInstruction("Stop");

}); 

function changeCamSource(id) {
  var camSource;
  if (id == "btnWAN") {
    camSource = "http://102.65.87.193:8080/video";
  } else {
    camSource = "http://192.168.1.13:8080/video";
  }
  var video = document.getElementById("video");
  video.src = camSource;
}

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

