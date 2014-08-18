Pebble.addEventListener("ready",
    function(e) {
       	navigator.geolocation.getCurrentPosition(geoSuccess);
    }
);

function geoSuccess(loc) {
	getStations(loc.coords.latitude, loc.coords.longitude);
}

function getStations(lat, long) {
	var req = new XMLHttpRequest();
	req.open('GET', 'http://transport.opendata.ch/v1/locations?y='+long+'&x='+lat+'&type=station', true);
	req.onload = function(e) {
		if (req.readyState == 4 && req.status == 200) {
			if(req.status == 200) {
				var response = JSON.parse(req.responseText);
				
				var count = Math.min(response.stations.length, 8);
				var hashtable = {};
				hashtable[0] = count;
				for (i = 0; i < count; i++) {
					var stationName = response.stations[i].name;	
					hashtable[i+1] = stationName;
				}
				
				var transactionId = Pebble.sendAppMessage(hashtable,
					function(e) {
						console.log("Successfully delivered message with transactionId=" + e.data.transactionId);
					},
				  	function(e) {
				    	console.log("Unable to deliver message with transactionId=" + e.data.transactionId + " Error is: " + e.error.message);
				  	}
				);
				
			} else { 
				console.log("Error"); 
			}
		}
	}
	req.send(null);	
}