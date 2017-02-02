// Group 2 : Embedded Systems of Computer Science, UDE, Germany
// AMAN BATRA , 3022386
// Alvaro Teruel Cañones ,  6053549
// Montse Rodríguez Zamorano

// Requires the following additional packages. From the directory containing tcpserver.js, run:
// npm install particle-api-js yargs node-persist
// 

// Run:node tcpserver.js --login <user> <password> 
// Atlease Once to generate the access token used to access the Particle cloud.

var path = require('path');


var setup = require('./library/setup.js'); 
setup.settingsDir = path.join(__dirname, 'settings');

 
var cloud = require('./library/cloud.js');
setup.addModule(cloud);


var devices = require('./library/devices.js');
setup.addModule(devices);
cloud.addEventHandler('devices', devices);


var server = require('./library/server.js');
server.serverPort = 8070;
setup.addModule(server);
devices.server = server;


setup.init();





