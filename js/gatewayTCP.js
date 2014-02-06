#!/usr/bin/env node

var net = require('net');
var client = new net.Socket();
var fs = require('fs');
var http = require('http');
var querystring = require('querystring');
var devices = require("./dbDetails").collection('devices');
var devicesSkynet = require("./dbDetails").collection('devicesSkynet');
var radioIds = require("./dbDetails").collection('radioIds');
var user = require("./user");


var msgType = {PRESENTATION:0,
               SET_VARIABLE:1,
               REQ_VARIABLE:2,
               ACK_VARIABLE:3,
               INTERNAL:4};
var intType = {BATTERY_LEVEL:0,
               BATTERY_DATE:1,
               LAST_TRIP:2,
               TIME:3,
               VERSION:4,
               REQUEST_ID:5,
               INCLUSION_MODE:6,
               RELAY_NODE:7,
               LAST_UPDATE:8,
               PING:9,
               PING_ACK:10,
               LOG_MESSAGE:11,
               CHILDREN:12,
               UNIT:13};


function processIncoming(s)
{
    var fields=s.split(";");
    var radioId=fields[0];
    var childId=fields[1];
    var msgTypeId=parseInt(fields[2]);
    var sensorTypeId=fields[3];
    var sensorValue=fields[4];
    
    console.log("msgTypeId="+msgTypeId+" int="+msgType.INTERNAL);
    switch(msgTypeId)
    {
        case msgType.SET_VARIABLE:
             setVariable(radioId,childId,sensorTypeId,sensorValue);
             break;
        case msgType.INTERNAL:
             processInternalMessage(radioId,childId,sensorTypeId,sensorValue);
             break;
        default:
             console.log("Message type "+msgTypeId+" is not yet implemented!"); 
    }
}

function setVariable(radioId,childId,sensorTypeId,sensorValue)
{
    var timestamp = new Date().getTime();
    var id = user.id+"-"+radioId+"-"+childId+"-"+sensorTypeId;
    var dataJSON = {userId:user.id,radioId:radioId,childId:childId,sensorTypeId:sensorTypeId,sensorValue:sensorValue,timestamp:timestamp};

    //Lookup if uuid exists in local database
    var info = devicesSkynet.find({id:id},function (err, data)
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

function processInternalMessage(radioId,childId,index,data)
{
    switch(parseInt(index))
    {
        case intType.TIME :
             sendInternalCommand(radioId,childId,intType.TIME,new Date().getTime());
             break;
        case intType.UNIT :
             sendInternalCommand(radioId,childId,intType.UNIT,0);
        case intType.REQUEST_ID :
             sendNextAvailRadioId(radioId,childId);
             break;
        
    } 
}

function sendNextAvailRadioId(radioId,childId)
{
    //Lookup max+1 radioId
    
    var info = radioIds.find({},function (err, data)
    {
      if (err)
      {
          console.log("Error looking up radioIds");
      }
      else
      {
          if (data.length == 0)
          {
              sendNewRadioId(radioId,childId,1);
          }
          else
          {
              max=1;
              for (i=0; i<data.length; i++)
              {
                  if (data[i].radioId > max)
                  {
                      max=data[i].radioId;
                  }
              }
              sendNewRadioId(radioId,childId,parseInt(max+1));
          }
      }
    });
 
}

function sendNewRadioId(radioId,childId,newRadioId)
{
    insertRadioId(newRadioId);
    sendInternalCommand(radioId,childId,intType.REQUEST_ID,newRadioId);
}

function sendInternalCommand(radioId,childId,msgTypeId,data)
{
    sendCommandWithMessageType(radioId,childId,msgType.INTERNAL,msgTypeId,data);
}

function sendCommandWithMessageType(radioId,childId,msgTypeId1,msgTypeId2,data)
{
    dataToSend=radioId+";"+childId+";"+msgTypeId1+";"+msgTypeId2+";"+data;

    console.log("Sending data "+dataToSend);
    if (client !== undefined)
    {
        client.write(dataToSend+"\n");
    }
    else
    {
        console.log("Write socket not ready yet");
    }
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
                
                dataIdent={id:id,uuid:data.uuid,token:data.token};
                insertLocalSkynet(dataIdent);
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
    devices.insert(dataJSON,function(err,data){if (err ) console.log("Data not saved! "+err); else console.log("Inserted id="+dataJSON.id)});
}

function insertLocalSkynet(dataJSON)
{
    devicesSkynet.insert(dataJSON,function(err,data){if (err ) console.log("Skynet: Data not saved! "+err); else console.log("Skynet: Inserted id="+dataJSON.id+" uuid="+dataJSON.uuid)});
}

function insertRadioId(newRadioId)
{
    radioIds.insert({radioId:newRadioId},function(err,data){if (err ) console.log("radioId: Data not saved! "+err); else console.log("radioId: Inserted id="+newRadioId)});

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
console.log("Processing messages...");

client.on('data', function(c) {
    processIncoming(c.toString());
    //console.log("Skipping "+c.toString());
  });
client.on('close', function() {
    console.log("Got Close!");
  });
client.on('error', function() {
    console.log("Got Error!");
  });

client.connect(1315,'localhost', function() {
    console.log("Connected to server");
    client.write("0;0;4;4;Get Version");
    console.log("Sent Version");
});

