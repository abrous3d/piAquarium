var intervalPoll = 1000;
var dataArray    = []; //[ [10,15,20],[20,25,30] ];
var graphOpts    = { 
	 labels: [ "Time", "pH", "Temp", "Env1" ],  
	 legend: 'always',
     animatedZooms: true,	  
     rollPeriod: 8,
     showRoller: true, 
	 };

function TimedServices()
{
  console.log("Fired [TimedServices]");
  getReadings(
    //////////////// Function will be called on success of getting a reading from C service
    function(obj) {
	  if('pH' in obj) $('#pH').html(obj.pH); else $('#pH').html('??'); 
      if('temp' in obj) $('#temp').html(obj.temp); else $('#temp').html('??'); 
      if('envtmp1' in obj) $('#envtmp1').html(obj.envtmp1); else $('#envtmp1').html('??'); 
	  dataArray.push( [ new Date(), parseFloat(obj.pH), parseFloat(obj.temp), parseFloat(obj.envtmp1) ] );
	  if(dataArray.length > 4096) dataArray.splice(0, 1);
	  //console.log(JSON.stringify(dataArray));
	  g1.updateOptions( {'file':dataArray} );
      setTimeout(TimedServices, intervalPoll); // Restart timer (next request)
    },
    //////////////// Function will be called on FAILURE
    function() {
	  $('#pH').html('');
      $('#temp').html('');
      $('#envtmp1').html('');
      setTimeout(TimedServices, intervalPoll); // Restart timer (next request)
    }
  );
}

function getReadings(okfn, failfn)
{
  $.ajax({
    url: "getreadings.cgi", type: "GET",
    success: function(response, textStatus, jqXHR) {
      var obj = JSON.parse(response);
      okfn(obj);
    },
    error: function(jqXHR, textStatus, errorThrown) {
      failfn();
    }
  });         
}


$(document).ready(function()
{
  console.log("Started");
  setTimeout(TimedServices, 4000); // Start first timer
  
   g1 = new Dygraph(
    document.getElementById("graphdiv1"),
    dataArray,
    graphOpts
  );
  
  
  
})


