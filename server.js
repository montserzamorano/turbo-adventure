var http = require('http');
var url = require('url');
var fs = require('fs');
var path = require('path');
var net = require('net');
var os = require('os'); // used to find local IP address


var urlHandlers = [];

(function(server)
{
	/**
	 * staticFileServer defaults to true, allows files to be served out of the directory specified
	 * by server.publicDir. If publicDir does not exist, the file server is disabled at startup as well.
	 */
	server.staticFileServer = true;

	/**
	 * publicDir is the directory containing your static files, if staticFileServer == true.
	 */
	server.publicDir = path.join(__dirname, 'public');

	/**
	 * serverPort is the port the web browser is listening on. You might open in your browser:
	 * http://localhost:8070/
	 * This is only the default, it can be overridden in the command line or settings
	 */
	server.serverPort = 8070;

	/**
	 * Standard MIME content-types mapped from file extensions to use in the static file server.
	 */
	server.contentTypeByExtension =
	{
			'.css':  'text/css',
			'.gif':  'image/gif',
			'.html': 'text/html',
			'.jpg':  'image/jpeg',
			'.js':   'text/javascript',
			'.json': 'application/json',
			'.png':  'image/png',
	};

	server.init = function()
	{
		fs.access(server.publicDir, fs.F_OK, function(err)
		{
			if (err) {
				server.staticFileServer = false;
			}
		});
	};

	server.yargs = function(yargs)
	{
		yargs.describe('serverAddr', 'Server address to used. Normally only needed if you have multiple interface and you want to select a specific one.')
			.nargs('serverAddr', 1)
			.describe('serverPort', 'TCP port to listen on. Saved in settings. Default=' + server.serverPort)
			.nargs('serverPort', 1);
	};

	server.loadSettings = function(setup, storage, argv)
	{
		// Handle the port setting:
		// 1. If set on the command line, use and set in settings.
		// 2. Otherwise, if set in the settings, use that.
		// 3. Otherwise, use the default in server.serverPort, set in the global variables above.
		if (argv.serverPort != undefined)
		{
			server.serverPort = argv.port;
			storage.setItemSync('serverPort', server.serverPort);
		}
		else
		{
			var temp = storage.getItemSync('serverPort');
			if (temp != undefined)
			{
				server.serverPort = temp;
			}
		}

		if (argv.serverAddr != undefined)
		{
			server.serverAddr = argv.serverAddr;
			storage.setItemSync('serverAddr', server.serverAddr);
		}
		else
		{
			var temp = storage.getItemSync('serverAddr');
			if (temp != undefined)
			{
				server.serverAddr = temp;
			}
			else {
				var addresses = server.getAddresses();
				if (addresses.length > 1)
				{
					console.log("multiple server addresses detected, using " + addresses[0]);
					server.serverAddr = addresses[0];
				}
				else
				if (addresses.length == 1)
				{
					console.log("Server Address: " + addresses[0]);
					server.serverAddr = addresses[0];
				}
				else
				{
					console.log("unable to determine server address");
				}
			}
		}
		setup.callNextLoadSettings();
	};

	server.run = function()
	{

		//console.log("Received data on server is " + data);
		server.createServer();
	};

	/************************************/
	server.sendtoclient = function (data)
	{
		//console.log("Received data on server is " + data);

	}
	/*********************************/
	server.addUrlHandler = function(prefix, module)
	{
		urlHandlers.push({prefix:prefix,module:module});
	};


	server.callUrlHandlers = function(methodName, param1, param2, param3, param4)
	{
		for(var ii = 0; ii < urlHandlers.length; ii++)
		{
			server.callUrlHandler(urlHandlers[ii], methodName, param1, param2, param3, param4)
		}
	};

	server.findUrlHandler = function(url)
	{
		for(var ii = 0; ii < urlHandlers.length; ii++)
		{
			if (url.indexOf(urlHandlers[ii].prefix) == 0)
			{
				return urlHandlers[ii];
			}
		}
		return undefined;
	};

	server.callUrlHandler = function(handler, methodName, param1, param2, param3, param4)
	{
		if (handler.module[methodName] != undefined)
		{
			handler.module[methodName](handler, param1, param2, param3, param4);
		}
	};

	// Called to start listening for TCP connections
	server.createServer = function ()
	{



		// Create an TCP server

//var HOST = '172.20.10.2'; //'XXX.XXX.XXX.XXX'; //your computer's ip address
var HOST = server.serverAddr
var PORT = server.serverPort;

var replymsg_1 = 'COMPLETED';
var replymsg_2 = 'COMPLETED PI';

var clientmsg_1 = 'FIBONACCI';
var clientmsg_2 = 'PI';
//var clientmsg = 100;

// Create a server instance, and chain the listen function to it
// The function passed to net.createServer() becomes the event handler for the 'connection' event
// The sock object the callback function receives UNIQUE for each connection
net.createServer(function(sock) {

    // We have a connection - a socket object is assigned to the connection automatically
    console.log('CONNECTED: ' + sock.remoteAddress +':'+ sock.remotePort);

    // Add a 'data' event handler to this instance of socket
    sock.on('data', function(data)
	{

        //console.log('RECEIVED DATA, Have to match Condition ' + sock.remoteAddress +':'+ sock.remotePort + ' - ' + data);
	//if (firstApp)
	//
		//console.log('DATA '+ ' - ' + data);
		//console.log('Matched the Fibonacci Sequence');
		//sock.on('data', function(data)
		//{
		//console.log('Entered to calculate the value of Fibonacci Sequence');
		console.log('Now I Am telling to the client that my service is Fibonacci right now');
		sock.write(clientmsg_1);
		console.log('Waiting for his values to continue the task...');
	    	sock.read(data);
		console.log('The Received value for FIB Sequence: '+ data);
		var b=a=0;
		var  a = data;
		if(data==5)
		var b = 3;
		else if(data==8)
		var b = 5;
		var c, i ;
		for (i=0;i<data;i++)
		{
			c = a + b;
			//console.log("The value of c is " + c);
			a = b;
			b = c;
		}
		console.log("The Last Computed value of FIB Sequence: " + c);
		sock.write(replymsg_1);
		//});
	//}

	console.log('Now our services have changed to Pi');
	sock.write(clientmsg_2);
	console.log('Waiting for his values to continue the task...');
	sock.read(data);
	console.log('The Received value for Pi Sequence: '+ data);
	{
		//console.log('DATA '+ ' - ' + data);
		console.log('Matched to calculate the value of pi');
		//sock.on('data', function(data_3)
		//{
		console.log('Entered to calculate the value of Fibonacci Sequence');
		//console.log('data');
		var width;
		var sum;
		var intervals;
		var  i;
		intervals = 25;
		width = 1.0 / intervals;

		sum = 0;
		for (i=0; i<intervals; i++)
		{
         x = (i + 0.5) * width;
         sum += 4.0 / (1.0 + x * x);
		}
		sum *= width;
		console.log("The Computed value of sum is " + sum);
		//sock.write(replymsg);
		sock.write(replymsg_2);
		//});
	}



		//sock.write(replymsg);
	/*if (data == clientmsg)
	{		//Doing Some Computations
		var x = 0;;
		for (var k=data;k<1000;k++)
		{
			x = k+1;
		}
		console.log("the value for x is" + x );
        // Write the data back to the socket, the client will receive it as data from the server
		console.log("Sending the Reply Message");
		//var rep = 'k';
		//console.log("The reply message is" + k);
		//sock.write(k);
		//var value = 50;
		//server.printf("%d\n", clientmsg);
        //sock.write(replymsg);
		sock.write(data);
		sock.write('You said "' + data + '"');
	}*/
		//console.log("Not Sending the Reply Message");
    });




    sock.on('error', function (error) {
        console.log('******* ERROR ' + error + ' *******');

        // close connection
 //       sock.end();
    });


    // Add a 'close' event handler to this instance of socket
    sock.on('close', function(data) {
        console.log('CLOSED: ************************');// + sock.remoteAddress +' '+ sock.remotePort);
    });



}).listen(PORT, HOST);

		console.log('Server listening on ' + HOST +':'+ PORT);
		//console.log('server running at http://' + server.serverAddr + ':' + server.serverPort + '/');
	};

	server.getAddresses = function() {
		var ifaces = os.networkInterfaces();

		var result = [];

		// http://stackoverflow.com/questions/3653065/get-local-ip-address-in-node-js
		console.log("Getting Local IP address of a device on which this server is running");
		Object.keys(ifaces).forEach(function (ifname) {
			ifaces[ifname].forEach(function (iface) {
				if ('IPv4' !== iface.family || iface.internal !== false) {
					// skip over internal (i.e. 127.0.0.1) and non-ipv4 addresses
					return;
				}
				//console.log("found address " + ifname + ": " + iface.address);
				console.log("Found the service used by the device for connectivity " + "Name: " + ifname + " and IP Address: " + iface.address);
				result.push(iface.address);
			});
		});

		return result;
	};

}(module.exports));



/*function handleApi(request, response, partialUrl)
{
	var headers = {
			'Content-Type': 'text/json',
			'Cache-Control': 'no-cache'
	};
	response.write('AMAN');
	response.writeHead(200, headers);

	console.log("handle api ", partialUrl);

	response.write('Hello');
}*/
