#!/usr/bin/env node

var net = require('net');
var fs = require('fs');
var http = require('http');
var querystring = require('querystring');
var msgType = require("./messageTypes");
var devices = require("./dbDetails").collection('devices');
var user = require("./user");

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
    var timestamp = new Date().getTime();
    var id = user.id+"-"+radioId+"-"+childId+"-"+sensorTypeId;
    var dataJSON = {userId:user.id,radioId:radioId,childId:childId,sensorTypeId:sensorTypeId,sensorValue:sensorValue,timestamp:timestamp};

    //Lookup if uuid exists in local database
    var info = devices.find({uid:id},function (err, data)
    {
      if (err)
      {
          console.log("Error finding key "+id);
      }
      else
      {
          if (data.length == 0)
          {
              createNewDevice(id,dataJSON);
          }
          else
          {
              console.log("data found="+data[0]);
              updateDevice(id,dataJSON,data[0].uuid,data[0].token);
          }
      }
    });
}

function skynetPOST(id,dataJSON)
{
    console.log("Before POST id="+id); 
    var p_params = querystring.stringify(dataJSON);
    var p_details = {host:'mysensors',port:'4711',path:'/devices',method:'POST',headers: {
         'Content-Type': 'application/x-www-form-urlencoded',
         'Content-Length': p_params.length}};
    var p_request = http.request(p_details, function (res) {
         res.setEncoding('utf8');
         res.on('data', function (chunk) {
             data = JSON.parse(chunk);
             console.log("uuid="+data.uuid+" data="+data);
             if (data.uuid) // Successful insert uuid+data
             { 
                
                dataIdent={uid:id,uuid:data.uuid,token:data.token};
                insertLocal(dataIdent);
                dataJSON.id=id;
                dataJSON.updated=1;
                insertLocal(dataJSON);
             }
             else
             { 
                dataJSON.id=id;
                dataJSON.updated=0;
                insertLocal(dataJSON);
             }
         });
    });

    p_request.write(p_params);
    p_request.end();
}

function skynetPUT(id,dataSkynetJSON,dataJSON,uuid,token)
{
    dataSkynetJSON.token=token;
    console.log("Before PUT id="+id+" uuid="+uuid);
    var p_params = querystring.stringify(dataSkynetJSON);
    var p_details = {host:'mysensors',port:'4711',path:'/devices/'+uuid,method:'PUT',headers: {
         'Content-Type': 'application/x-www-form-urlencoded',
         'Content-Length': p_params.length}};
    var p_request = http.request(p_details, function (res) {
         res.setEncoding('utf8');
         res.on('data', function (chunk) {
             data=JSON.parse(chunk);
             console.log("data: "+ data);
             if (data.uuid) 
             {
                 dataJSON.id=id;
                 dataJSON.updated=1;
                 insertLocal(dataJSON);
             }
             else
             {
                 dataJSON.id=id;
                 dataJSON.updated=0;
                 insertLocal(dataJSON);
             }
         });
    });

    p_request.write(p_params);
    p_request.end();
}


function insertLocal(dataJSON)
{
    devices.insert(dataJSON,function(err,data){if (err ) console.log("Data not saved! "+err); else console.log("Inserted id="+dataJSON.id+" uid="+dataJSON.uid+" uuid="+dataJSON.uuid)});

}
function cloneJSON(obj) {
    // basic type deep copy
    if (obj === null || obj === undefined || typeof obj !== 'object')  {
        return obj
    }
    // array deep copy
    if (obj instanceof Array) {
        var cloneA = [];
        for (var i = 0; i < obj.length; ++i) {
            cloneA[i] = cloneJSON(obj[i]);
        }              
        return cloneA;
    }                  
    // object deep copy
    var cloneO = {};   
    for (var i in obj) {
        cloneO[i] = cloneJSON(obj[i]);
    }                  
    return cloneO;
}

function createNewDevice(id,dataJSON)
{
    console.log("Creating device "+id);
    skynetPOST(id, dataJSON);
}

function updateDevice(id,dataJSON,uuid,token)
{
    console.log("Updating device "+id+" uuid "+uuid);
    skynetPUTJSON={sensorValue:dataJSON.sensorValue,timestamp:dataJSON.timestamp};
    skynetPUT(id, skynetPUTJSON, dataJSON, uuid, token);
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
