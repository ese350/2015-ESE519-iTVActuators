var sys = require('sys')

var serialport = require("serialport"),             // include the serialport library
	SerialPort  = serialport.SerialPort,              // make a local instance of serial
	express = require('express'),					            // make an instance of express
	app = express(),                                  // start Express framework
  	server = require('http').createServer(app);		  // start an HTTP server
 
var portName = "/dev/ttyACM0";						          // serial port name on which the mbed is connected
console.log("opening serial port: " + portName);	  // print out the port you're listening on

server.listen(8080);								                // listen for incoming requests on the server

console.log("Listening for new clients on port 8080");

function setPixels(pixelArray) {
  var pixels = '[' + pixelArray.join(';') + ']';
  return pixels
}

function createPattern(type, red, green, blue, patternDirection, shape, timestamp, period, pixelArray){
  var returnObject = {};
  // returnObject["command"] = 'x' + type + red + green + blue + patternDirection + shape + timestamp + period;
  if (typeof pixelArray !== 'undefined' && pixelArray.length > 0) {
    var pixels = setPixels(pixelArray);
    returnObject["command"] = type + ',' + red + ',' + green + ',' + blue + ',' + patternDirection + ',' + shape + ',' + timestamp + ',' + period + ',' + pixels ;
  } else {
    returnObject["command"] = type + ',' + red + ',' + green + ',' + blue + ',' + patternDirection + ',' + shape + ',' + timestamp + ',' + period ;     
  }
  return returnObject;
}

// configure server to serve static files from /js and /css:
app.use('/js', express.static(__dirname + '/js'));
app.use('/css', express.static(__dirname + '/css'));

// respond to web GET requests with the index.html page:
app.get('/', function (request, response) {
  console.log("print '/'");
  response.sendFile(__dirname + '/index.html');
});


// respond to web GET requests with the index.html page:
app.get('/shape', function (request, response) {
  console.log("print '/shape'");
  response.sendFile(__dirname + '/shape.html');
});

// open the serial port. Changing the name our port's name
var serialPort = new SerialPort(portName);

app.get('/message/*', function (request, response) {
  // the route is the first parameter of the URL request:
  var command = request.params[0];
  // send it out the serial port 
  // after appending it with carriage return and new line characters
  // (they are needed by the mbed to read it as serial input):
  command = command + '\r\n';
  serialPort.write(command);
  // send an HTTP header to the client:
  response.writeHead(200, {'Content-Type': 'text/html'});
  // send the data and close the connection:
  console.log('command: ' + command);
  response.end(command);
});
function chooseShape(commandReceived) {
  var pixelArray=[];
  if (commandReceived === 'X') {
    console.log('pattern received: ' + commandReceived);
    // shape = 1;
    // pixelArray = [0,1,2,3,4,5,6,8,9,12];
    pixelArray = [];
  } else if(commandReceived === 'L') {
    console.log('pattern received: ' + commandReceived);
    // shape = 2;
    pixelArray = [5,25,45,46,48];
  } else if(commandReceived === 'A') {
    console.log('pattern received: ' + commandReceived);
    shape = 3;
  } else if(commandReceived === 'B') {
    console.log('pattern received: ' + commandReceived);
    shape = 4;
  }  else if(commandReceived === 'triangle') {
    console.log('pattern received: ' + commandReceived);
    // shape = 5;
    pixelArray = [0,1,2,3,4,5,6,8,9,12];
  } else if (commandReceived === '<') {
    console.log('pattern received: ' + commandReceived);
    shape = 6;
  } else if (commandReceived === '>') {
    console.log('pattern received: ' + commandReceived);
    shape = 7;
  }

  return pixelArray;
}
  
app.get('/shape/*', function (request, response) {
  // the route is the first parameter of the URL request:
  var commandReceived = request.params[0];
  var shape = 0, timestamp = 15, pixelArray = [];
  // console.log(commandReceived)
  // shape = chooseShape(commandReceived);
  pixelArray = chooseShape(commandReceived);
  if(pixelArray.length != 0) {
    var returnValue = createPattern('xflash', 150, 20, 180, 'up', 0, timestamp, 4000, pixelArray);
    // send it out the serial port 
    // after appending it with carriage return and new line characters
    // (they are needed by the mbed to read it as serial input):
    returnValue.command = returnValue.command + '\r\n';
    serialPort.write(returnValue.command);
    // send an HTTP header to the client:
    response.writeHead(200, {'Content-Type': 'text/html'});
    // send the data and close the connection:
    console.log('command sent: ' + returnValue.command);
    response.end(returnValue.command);
  }
});