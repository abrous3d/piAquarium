var intervalPoll = 1000;
var dataArray    = []; //[ [1,15,20],[2,25,30] ];
var graphOpts    = { 
	 labels: [ "Time", "pH", "Temp" ],  
	 valueRange: [3.0, 10],
	 ylabel: 'pH',
	 legend: 'always',
	 labelsDivStyles: { 'textAlign': 'right' },	
     animatedZooms: true,	  
     rollPeriod: 8,
     showRoller: true, 
	 title: 'Tank monitor',
					   Temp  : {
                       axis : { }
                     }
	 };

var dataArrayGr2    = []; //[ [10,15,20],[20,25,30] ];	 
var graphOptsGr2    = { 
	 labels: [ "Time", "Temp", "Hum" ],  	 
	 ylabel: 'Temperature',
	 legend: 'always',
	 labelsDivStyles: { 'textAlign': 'right' },	
     animatedZooms: true,	  
     rollPeriod: 8,
     showRoller: true, 
	 title: 'Environment monitor',
					   Temp  : {
                       axis : { }
                     }
	 };
	 
 function Lamp_CO2(state) {
    var image = document.getElementById('CO2_lamp');
    if (state == 0) {
        image.src = "green_light.png";
    } else {
        image.src = "red_light.png";
    }
  }
  
  function Lamp_Heat(state) {
    var image = document.getElementById('Heat_lamp');
    if (state == 0) {
        image.src = "green_light.png";
    } else {
        image.src = "red_light.png";
    }
  }	 
	
  function Lamp_Fan(state) {
    var image = document.getElementById('Fan_lamp');
    if (state == 0) {
        image.src = "green_light.png";
    } else {
        image.src = "red_light.png";
    }
  }	
	 
function TimedServices()
{
  console.log("Fired [TimedServices]");
  
  
	
  getReadings(
    //////////////// Function will be called on success of getting a reading from C service
    function(obj) {
	  if('pH' in obj) $('#pH').html(obj.pH); else $('#pH').html('??'); 
      if('temp' in obj) $('#temp').html(obj.temp); else $('#temp').html('??'); 
      if('envtmp1' in obj) $('#envtmp1').html(obj.envtmp1); else $('#envtmp1').html('??'); 
	  if('envhum1' in obj) $('#envhum1').html(obj.envhum1); else $('#envhum1').html('??'); 
	  dataArray.push( [ new Date(), parseFloat(obj.pH), parseFloat(obj.temp)] );
	  dataArrayGr2.push( [ new Date(), parseFloat(obj.envtmp1) , parseFloat(obj.envhum1) ] );
	  
	  
	  Lamp_Heat(obj.ther_on);
	  Lamp_CO2(obj.co2_on);
	  Lamp_Fan(obj.fan_on);
	  
	  if(dataArray.length > 4096) dataArray.splice(0, 1);
	  if(dataArrayGr2.length > 4096) dataArrayGr2.splice(0, 1);
	  //console.log(JSON.stringify(dataArray));
	  g1.updateOptions( {'file':dataArray} );
	  g2.updateOptions( {'file':dataArrayGr2} );
	  
	  document.getElementById('disp1').innerHTML =  parseFloat(obj.pH) + "pH";
      document.getElementById('disp2').innerHTML =  parseFloat(obj.temp)+ "C";
      document.getElementById('disp3').innerHTML =  parseFloat(obj.envhum1)+ "%RH";
	  document.getElementById('disp4').innerHTML =  parseFloat(obj.envtmp1)+ "C";
	  
	
		

	
      setTimeout(TimedServices, intervalPoll); // Restart timer (next request)
    },
    //////////////// Function will be called on FAILURE
    function() {
	  $('#pH').html('');
      $('#temp').html('');
      $('#envtmp1').html('');
	  $('#envhum1').html('');
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
  
  g2 = new Dygraph(
    document.getElementById("graphdiv2"),
    dataArrayGr2,
    graphOptsGr2
  );
  
  
  $('#btn1').click(function() {
	$.ajax({
      url: "signal.cgi?data=123", type: "GET",
      success: function(response, textStatus, jqXHR) {  },
      error: function(jqXHR, textStatus, errorThrown) {    }
    });
  });
  
})


