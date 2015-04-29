var http = require('http');
var url = require('url');
var request = require("request");
var manager = new (require('./asterisk'))(5038, '127.0.0.1');

manager.on('connect', function(err, val) {
	manager.authenticate('admin', 'amp111');
});

manager.on('close', function() {
console.log("[SE HA CERRADO LA CONEXION CON EL SERVIDOR]"); 
});

manager.on('error', function(err) {});

manager.on('response', function(res) {});

manager.connect();


var server = http.createServer( function(requestServer, response){
  var queryData = url.parse(requestServer.url, true).query;
  response.writeHead(200, { 'Content-Type': 'text/plain' });

  console.log('DEBUG:'+queryData.status);

  if (queryData.status) {
    
    if(queryData.status == 'alert')
    {
	console.log('Llamando a la extension 6401');
	manager.action({
	'Action':'originate',
	'Channel':'IAX2/PortablePBX/NUMBER_TO_CALL',
	'Context':'alert-context',
	'Exten':'s',
	'Priority':1,
	'Async':'yes'
	},function(err, res) {});
	console.log('Llamada realizada');
      response.write('Realizando llamada de alerta\n');
    }

  }
  else
  {
    console.log("No data received");
    response.write("No data received");
  }

  response.end();
});

server.listen(8080);

console.log('Server Started');
