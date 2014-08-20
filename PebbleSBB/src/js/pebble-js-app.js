Pebble.addEventListener("ready",
    function(e) {
       	navigator.geolocation.getCurrentPosition(geoSuccess);
       	
       	Pebble.addEventListener("appmessage",
			function(e) {
				var msg = e.payload['dummy'];
				var parts = msg.split(';');
				if (parts[0] == "gc") {
					getConnections(parts[1]);	
				} else if (parts[0] == "gd") {
					getConnectionDetail(parts[1]);	
				}
			}
		);
    }
);

var _stations = [];
var _connections = [];

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

function getConnections(index) {
	var req = new XMLHttpRequest();
	req.open('GET', 'http://transport.opendata.ch/v1/stationboard?id='+_stations[index]+'&limit=8', true);
	req.onload = function(e) {
		if (req.readyState == 4 && req.status == 200) {
			if(req.status == 200) {
				var response = JSON.parse(req.responseText);
				
				_connections = response;
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

function getConnectionDetail(index) {
	var connection = _connections.stationboard[index];
	var hashtable = {};
	var icon = 0;
	switch (connection.categoryCode) {
		case 1:
			icon = 3;
			break;
		case 2:
			icon = 3;
			break;
		case 3:
			icon = 3;
			break;
		case 4:
			icon = 2;
			break;
		case 5:
			icon = 3;
			break;
		case 6:
			icon = 0;
			break;
		case 7:
			icon = 1;
			break;
		case 8:
			icon = 3;
			break;
		case 9:
			icon = 4;
			break;
	}
	hashtable[0] = icon;
	
	var p = connection.stop.platform;
	if (p == "") {
		p = "?"
	}
	hashtable[1] = "Platform: " + p;
	
	var d = connection.stop.delay;
	if (d == null) {
		d = "0";	
	}
	hashtable[2] = "Delay: " + d;
	Pebble.sendAppMessage(hashtable);
}
