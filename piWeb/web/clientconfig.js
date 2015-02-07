var intervalPoll = 1000;

$(document).ready(function()
{
  setTimeout(TimedServices, 1000); // Start first timer
   
  /* 
  $('#Auto1').click(function() {
    console.log("AUTO1");
    $('#pHSetP').val("1234");
  });
  $('#Auto2').click(function() {
	console.log("AUTO2");
    var v = $('#pHSetP').val("5678");
	console.log(v);
  });
  */
  ////////////////// Read config ini
window.inifile = "";
  $.ajax({
    url: "schedule.js", type: "GET",
    success: function(response, textStatus, jqXHR) {
	  window.config = JSON.parse(response);	  	  
	  
	  $('#pHSetP').val(config.ph.Set);
	  $('#pH_Hyst').val(config.ph.Hysteresis);
	  $('#pH_Alarm').val(config.ph.Alarm);
	  if (config.ph.On == 0) {
			document.getElementById("pHActFlag").checked = false;
	  } else {
			document.getElementById("pHActFlag").checked = true;
	  }
	  	  
	  $('#TempSetP').val(config.temperature.Set);
	  $('#Temp_Hyst').val(config.temperature.HeatHysteresis);
	  $('#Temp_Alarm').val(config.temperature.Alarm);
	  if (config.temperature.On == 0) {
			document.getElementById("TempActFlag").checked = false;
	  } else {
			document.getElementById("TempActFlag").checked = true;
	  }
	  
	  
	  $('#FanSetP').val(config.temperature.Fan);
	  $('#Fan_Hyst').val(config.temperature.FanHysteresis);
	  
	  $('#Scale').val(config.Lights.Scale);
	  $('#intChA').val(config.Lights.ManA);
	  $('#intChB').val(config.Lights.ManB);
	  $('#intChC').val(config.Lights.ManC);
	  
	  
	  
		if (config.system.ADConCE0 == 0) {
			document.getElementById("AD0_ActFlag").checked = false;
		} else {
			document.getElementById("AD0_ActFlag").checked = true;
		}
	
		if (config.system.ADConCE1 == 0) {
			document.getElementById("AD1_ActFlag").checked = false;
		} else {
			document.getElementById("AD1_ActFlag").checked = true;
		}
	
	//=====================================================
		if (config.Lights.PolA  == 0) {
			document.getElementById("PolarityCh1Flag").checked = false;
		} else {
			document.getElementById("PolarityCh1Flag").checked = true;
		}
	
		if (config.Lights.PolB  == 0) {
			document.getElementById("PolarityCh2Flag").checked = false;
		} else {
			document.getElementById("PolarityCh2Flag").checked = true;
		}
		
		if (config.Lights.PolC  == 0) {
			document.getElementById("PolarityCh3Flag").checked = false;
		} else {
			document.getElementById("PolarityCh3Flag").checked = true;
		}
	
	    
    },
    error: function(jqXHR, textStatus, errorThrown) {
      alert("Read config failed: "+errorThrown);
    }
  });         
  
})
function TimedServices()
{
 
  getReadings(
    //////////////// Function will be called on success of getting a reading from C service
    function(obj) {
	  if('row1' in obj) $('#row1').html(obj.row1); else $('#row1').html('??'); 
      if('row2' in obj) $('#row2').html(obj.row2); else $('#row2').html('??'); 
	   
	  
	  $('#AD0_ch0').val(obj.row1);
	  $('#AD0_ch1').val(obj.row2);
	  //document.getElementById('AD0_ch0').innerHTML = parseInt(obj.row1) + " ";
      //document.getElementById('AD0_ch1').innerHTML = parseInt(obj.row2) + " ";
	  //console.log(parseInt(obj.row1));		  
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

//=======================================================================
var qa = 0;

function UpdateReadings()
{
  setTimeout(UpdateReadings, intervalPoll); 
}

//========= Send the configuration object back to server to save ========
 function ConfigSave()
  {
	if(!ReadParseVal()) return;
	$.ajax({
		url: "SaveSchedule.js", type: "POST", 
		data: JSON.stringify(config),
		success: function(response, textStatus, jqXHR) {
			alert("Configuration saved");
		}
	});
  }
  
  //================== Read back configuration values =================
  function ReadParseVal()
  {

	config.ph.On = ($('#pHActFlag').is(":checked")?1:0);
	config.ph.Set = $('#pHSetP').val();
	config.ph.Hysteresis = $('#pH_Hyst').val();
	config.ph.Alarm = $('#pH_Alarm').val();
	

	config.temperature.On = ($('#TempActFlag').is(":checked")?1:0);
	config.temperature.Set = $('#TempSetP').val();
	config.temperature.HeatHysteresis = $('#Temp_Hyst').val();
	config.temperature.Alarm = $('#Temp_Alarm').val();
	
	config.temperature.Fan = $('#FanSetP').val();
	config.temperature.FanHysteresis = $('#Fan_Hyst').val();
	  
	config.Lights.Scale = $('#Scale').val();
	config.Lights.ManA = $('#intChA').val();
	config.Lights.ManB = $('#intChB').val();
	config.Lights.ManC = $('#intChC').val();
	
	config.system.ADConCE0 = ($('#AD0_ActFlag').is(":checked")?1:0);
	config.system.ADConCE1 = ($('#AD1_ActFlag').is(":checked")?1:0);
		
	config.Lights.PolA = ($('#PolarityCh1Flag').is(":checked")?1:0);
	config.Lights.PolB = ($('#PolarityCh2Flag').is(":checked")?1:0);
	config.Lights.PolC = ($('#PolarityCh3Flag').is(":checked")?1:0);
	
	
	return(true);
	
  }
  
function ButtonLightsManual()
{
	config.Lights.Overide = 1;
	ConfigSave();
}

function ButtonLightsAuto()
{
	config.Lights.Overide = 0;
	ConfigSave();
}


