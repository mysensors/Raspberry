#!/usr/bin/env node

var net = require('net');
var fs = require('fs');
var msgType = require("./messageTypes");

var server = net.createServer(function(stream) {
  stream.on('data', function(c) {
    processIncoming(c.toString());
  });
  stream.on('end', function() {
    server.close();
  });
});

function processIncoming(s)
{
    var fields=s.split(";");
    var radioId=fields[0];
    var childId=fields[1];
    var msgTypeId=fields[2];
    var sensorTypeId=fields[3];
    var sensorValue=fields[4];
    
    if (msgTypeId == msgType.SET_VARIABLE)
    {
        setVariable(radioId,childId,sensorTypeId,sensorValue);
    }
    else
    {
        console.log("Message type "+msgTypeId+" is not yet implemented!");
    }
}

function setVariable(radioId,childId,sensorTypeId,sensorValue)
{
    console.log("Setting variable: radioId="+radioId+" childId="+childId+" sensorTypeId="+sensorTypeId+" sensorValue="+sensorValue);
}

function main() {
    var stream2 = net.connect('/tmp/testc.sock');
    stream2.write('0;0;4;4;Get Version');
    console.log("Sent Version");
}

if (fs.existsSync('/tmp/testjs.sock'))
{
    fs.unlinkSync('/tmp/testjs.sock');
}
server.listen('/tmp/testjs.sock');

var stream = net.connect('/tmp/testjs.sock');
setTimeout(main,5000);
