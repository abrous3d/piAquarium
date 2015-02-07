// To send request to C service
var http    = require("http"),
    url     = require("url"),
    path    = require("path"),
    fs      = require("fs"),
	ini     = require("./ini");

var http_server_port = 5553;  // 
var reading = { "result":"no reading yet" };

/////////////////////////////////////////////////////////////////////
// Create/Start server
/////////////////////////////////////////////////////////////////////
http.createServer(function(request, response) {

  var uri       = url.parse(request.url).pathname;
  var filename  = path.join(process.cwd(), 'web/'+uri);

  
  request.on('data', function(chunk) {
	if(filename.indexOf("SaveSchedule.js") != -1) {
		var cfgobj = chunk.toString();
		console.log(cfgobj);
		cfgobj = JSON.parse(cfgobj);
		fs.writeFileSync('./web/scheduler/schedule.ini', ini.stringify(cfgobj));					
	}
  });
  

  if(filename.indexOf("signal.cgi") != -1) {
	//Process whatever u want to do
	console.log("SIGNALED!!");
	
    response.writeHead(200, {"Content-Type": "text/JSON"});
    response.write("{result:true}");
    response.end();
    return;
  }
  ///////// REQUEST
  if(filename.indexOf("getreadings.cgi") != -1) {
    response.writeHead(200, {"Content-Type": "text/JSON"});
    response.write(JSON.stringify(reading));
    response.end();
    return;
  }

  if(filename.indexOf("gettables.cgi") != -1) {
    response.writeHead(200, {"Content-Type": "text/JSON"});
    response.write(JSON.stringify(timetables));
    response.end();
    return;
  }
  
  
  if(filename.indexOf("schedule.js") != -1) {
  
	var config = ini.parse(fs.readFileSync('./web/scheduler/schedule.ini', 'utf-8'))	
    response.writeHead(200, {"Content-Type": "text/JSON"});
    response.write(JSON.stringify(config));
    response.end();
    return;
  }
  
  if(filename.indexOf("SaveSchedule.js") != -1) {
	response.writeHead(200, {"Content-Type": "text/JSON"});    
    response.end();
    return;
  }
  
  ///////// Else try to get/send static file
  path.exists(filename, function(exists) {
    if(!exists) {
      response.writeHead(404, {"Content-Type": "text/plain"});
      response.write("404 Not Found\n");
      response.end();
      return;
    }
    if(fs.statSync(filename).isDirectory()) filename += '/index.html';
    fs.readFile(filename, "binary", function(err, file) {
      if(err) {
        response.writeHead(500, {"Content-Type": "text/plain"});
        response.write(err + "\n");
        response.end();
        return;
      }
      //== Todo: proper type?
      response.writeHead(200);
      response.write(file, "binary");
      response.end();
    });
  });

}).listen(parseInt(http_server_port, 10));

function getReadings(onResult, onFailure)
{
  var options = { host: '127.0.0.1', port:4440, path:'/getread.req', method: 'GET', headers: { 'Content-Type': 'application/json' }};
  
  //console.log("Requesting a reading from service..");
  var req = http.request(options, function(res)
  {
    var output = '';      
    res.on('data', function (chunk) { output += chunk; });
    res.on('end', function() {
	  //console.log("request result:" + output);
      reading = JSON.parse(output);	  
      onResult(res.statusCode, reading);
    });
  });

  req.on('error', function(err) { console.log("ERROR: "+err.message); console.log("err="+JSON.stringify(err)); onFailure(err.message); });
  req.end();
};


function getTables(onResult, onFailure)
{
  var options = { host: '127.0.0.1', port:4440, path:'/gettables.req', method: 'GET', headers: { 'Content-Type': 'application/json' }};
  
  //console.log("Requesting scheduler tables from service..");
  var req = http.request(options, function(res)
  {
    var output = '';      
    res.on('data', function (chunk) { output += chunk; });
    res.on('end', function() {	  
      timetables = JSON.parse(output);	  
      onResult(res.statusCode, timetables);
    });
  });

  req.on('error', function(err) { console.log("ERROR: "+err.message); console.log("err="+JSON.stringify(err)); onFailure(err.message); });
  req.end();
};



setTimeout(TimedServices, 200); // Start first timer

function TimedServices()
{
  //console.log("Fired [TimedServices]");
  getReadings(
    //////////////// Function will be called on success of getting a reading from C service
    function() {
      setTimeout(TimedServices, 2000); // Restart timer (next request)
    },
    //////////////// Function will be called on FAILURE
    function() {
      setTimeout(TimedServices, 2000); // Restart timer (next request)
    }
  );
}



