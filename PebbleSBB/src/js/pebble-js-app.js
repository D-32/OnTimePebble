Pebble.addEventListener("ready",
    function(e) {
       	navigator.geolocation.getCurrentPosition(geoSuccess);
       	
       	Pebble.addEventListener("appmessage",
			function(e) {
				var msg = e.payload['dummy'];
				var parts = msg.split(';');
				if (parts[0] == "gc") {
					getConnections(parts[1]);	
				}
			}
		);
    }
);

var _stations = [];

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
					_stations[i] = response.stations[i].id;
					hashtable[i+1] = stationName;
				}
				Pebble.sendAppMessage(hashtable);	
			}
		}
	}
	req.send(null);	
}

function getConnections(station) {
	var req = new XMLHttpRequest();
	req.open('GET', 'http://transport.opendata.ch/v1/stationboard?id='+_stations[station]+'&limit=8', true);
	req.onload = function(e) {
		if (req.readyState == 4 && req.status == 200) {
			if(req.status == 200) {
				var response = JSON.parse(req.responseText);
				
				var count = Math.min(response.stationboard.length, 8);
				var hashtable = {};
				hashtable[0] = count;
				for (i = 0; i < count; i++) {
					var name = response.stationboard[i].name;
					var time = response.stationboard[i].stop.departure.substring(11, 16);
					var destination = response.stationboard[i].to;
					var title = time + " (" + name + ")\n" + destination;
					hashtable[i+1] = title;
				}
				Pebble.sendAppMessage(hashtable);	
			}
		}
	}
	req.send(null);	
}