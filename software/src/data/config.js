var baseHost = window.location.hostname;
//var baseHost = 'emonesp.local';
//var baseHost = '192.168.4.1';
//var baseHost = '172.16.0.52';
var baseEndpoint = 'http://' + baseHost;

var statusupdate = false;
var selected_network_ssid = "";
var lastmode = "";
var ipaddress = "";

function scaleString(string, scale, precision) {
  var tmpval = parseInt(string) / scale;
  return tmpval.toFixed(precision);
}

function BaseViewModel(defaults, remoteUrl, mappings) {
  if(mappings === undefined){
   mappings = {};
  }
  var self = this;
  self.remoteUrl = remoteUrl;
  // Observable properties
  ko.mapping.fromJS(defaults, mappings, self);
  self.fetching = ko.observable(false);
}

BaseViewModel.prototype.update = function (after) {
  if(after === undefined){
   after = function () { };
  }
  var self = this;
  self.fetching(true);
  $.get(self.remoteUrl, function (data) {
    ko.mapping.fromJS(data, self);
  }, 'json').always(function () {
    self.fetching(false);
    after();
  });
};


function StatusViewModel() {
  var self = this;
  BaseViewModel.call(self, {
    "chip_id":"",
    "CurrentTime":"",
    "TimeOperation":"",
	"timemqtt":"",
    "count_mqttdisconnect":"",
    "mode": "ERR",
    "networks": [],
    "rssi": [],
    "srssi": "",
    "ipaddress": "",
    "packets_sent": "",
    "packets_success": "",
    "emoncms_connected": "",
    "mqtt_connected": "",
    "free_heap": "",
    "CurrentTime":"",
    "sch_datestart":"",
    "sch_timestart":"",
    "sch_datestop":"",
    "sch_timestop":"",
    "sch_state":"",
    "count_mqttdisconnect":"",
    "timemqtt":"",
    "timeoperation":"",
    "state_load":"",
    "sdcard_state":"",
    "sdcard_init":""
  }, baseEndpoint + '/status');
  // Some devired values
  self.isWifiClient = ko.pureComputed(function () {
    return ("STA" == self.mode()) || ("STA+AP" == self.mode());
  });
  self.isWifiAccessPoint = ko.pureComputed(function () {
    return ("AP" == self.mode()) || ("STA+AP" == self.mode());
  });
  self.fullMode = ko.pureComputed(function () {
    switch (self.mode()) {
      case "AP":
        return "Access Point (AP)";
      case "STA":
        return "Client (STA)";
      case "STA+AP":
        return "Client + Access Point (STA+AP)";
    }
    return "Unknown (" + self.mode() + ")";
  });
}
StatusViewModel.prototype = Object.create(BaseViewModel.prototype);
StatusViewModel.prototype.constructor = StatusViewModel;

function ConfigViewModel() {
  BaseViewModel.call(this, {
    "ssid": "",
    "pass": "",
    "emoncms_server": "data.openevse.com/emoncms",
    "emoncms_apikey": "",
    "emoncms_node": "",
    "emoncms_fingerprint": "",
    "mqtt_server": "",
    "mqtt_topic": "",
    "mqtt_feed_prefix": "",
    "mqtt_user": "",
    "mqtt_pass": "",
    "schdatestart": "",
    "schtimestart": "",
    "schdatestop": "",
    "schtimestop": "",
    "www_username": "",
    "www_password": "",
    "espflash": "",
    "version": "0.0.0"
  }, baseEndpoint + '/config');
}
ConfigViewModel.prototype = Object.create(BaseViewModel.prototype);
ConfigViewModel.prototype.constructor = ConfigViewModel;

function LastValuesViewModel() {
  var self = this;
  self.remoteUrl = baseEndpoint + '/lastvalues';
  // Observable properties
  self.fetching = ko.observable(false);
  self.values = ko.mapping.fromJS([]);
  self.update = function (after) {
    if(after === undefined){
     after = function () { };
    }
    self.fetching(true);
    $.get(self.remoteUrl, function (data) {
      // Transform the data into somethinf a bit easier to handle as a binding
      var namevaluepairs = data.split(",");
      var vals = [];
      for (var z in namevaluepairs) {
        var namevalue = namevaluepairs[z].split(":");
        vals.push({key: namevalue[0], value: namevalue[1]});
      }
      ko.mapping.fromJS(vals, self.values);
    }, 'text').always(function () {
      self.fetching(false);
      after();
    });
  };
}

function EmonEspViewModel() {
  var self = this;
  self.config = new ConfigViewModel();
  self.status = new StatusViewModel();
  self.last = new LastValuesViewModel();
  self.initialised = ko.observable(false);
  self.updating = ko.observable(false);
  var updateTimer = null;
  var updateTime = 1 * 1000;
  // Upgrade URL
  self.upgradeUrl = ko.observable('about:blank');
  // -----------------------------------------------------------------------
  // Initialise the app
  // -----------------------------------------------------------------------
  self.start = function () {
    self.updating(true);
    self.config.update(function () {
      self.status.update(function () {
        self.last.update(function () {
          self.initialised(true);
          updateTimer = setTimeout(self.update, updateTime);
          self.upgradeUrl(baseEndpoint + '/update');
          self.updating(false);
        });
      });
    });
  };

  // -----------------------------------------------------------------------
  // Get the updated state from the ESP
  // -----------------------------------------------------------------------
  self.update = function () {
    if (self.updating()) {
      return;
    }
    self.updating(true);
    if (null !== updateTimer) {
      clearTimeout(updateTimer);
      updateTimer = null;
    }
    self.status.update(function () {
      self.last.update(function () {
        updateTimer = setTimeout(self.update, updateTime);
        self.updating(false);
      });
    });
  };

  self.wifiConnecting = ko.observable(false);
  self.status.mode.subscribe(function (newValue) {
    if(newValue === "STA+AP" || newValue === "STA") {
      self.wifiConnecting(false);
    }
  });

  // -----------------------------------------------------------------------
  // Event: WiFi Connect
  // -----------------------------------------------------------------------
  self.saveNetworkFetching = ko.observable(false);
  self.saveNetworkSuccess = ko.observable(false);
  self.saveNetwork = function () {
    if (self.config.ssid() == "") {
      alert("Please select network");
    } else {
      self.saveNetworkFetching(true);
      self.saveNetworkSuccess(false);
      $.post(baseEndpoint + "/savenetwork", { ssid: self.config.ssid(), pass: self.config.pass() }, function (data) {
          self.saveNetworkSuccess(true);
          self.wifiConnecting(true);
        }).fail(function () {
          alert("Failed to save WiFi config");
        }).always(function () {
          self.saveNetworkFetching(false);
        });
    }
  };

  self.saveAdminFetching = ko.observable(false);
  self.saveAdminSuccess = ko.observable(false);
  self.saveAdmin = function () {
    self.saveAdminFetching(true);
    self.saveAdminSuccess(false);
    $.post(baseEndpoint + "/saveadmin", { user: self.config.www_username(), pass: self.config.www_password() }, function (data) {
      self.saveAdminSuccess(true);
    }).fail(function () {
      alert("Failed to save Admin config");
    }).always(function () {
      self.saveAdminFetching(false);
    });
  };

  self.saveEmonCmsFetching = ko.observable(false);
  self.saveEmonCmsSuccess = ko.observable(false);
  self.saveEmonCms = function () {
    var emoncms = {
      server: self.config.emoncms_server(),
      apikey: self.config.emoncms_apikey(),
      node: self.config.emoncms_node(),
      fingerprint: self.config.emoncms_fingerprint()
    };

    if (emoncms.server === "" || emoncms.node === "") {
      alert("Please enter Emoncms server and node");
    } else if (emoncms.apikey.length != 32) {
      alert("Please enter valid Emoncms apikey");
    } else if (emoncms.fingerprint !== "" && emoncms.fingerprint.length != 59) {
      alert("Please enter valid SSL SHA-1 fingerprint");
    } else {
      self.saveEmonCmsFetching(true);
      self.saveEmonCmsSuccess(false);
      $.post(baseEndpoint + "/saveemoncms", emoncms, function (data) {
        self.saveEmonCmsSuccess(true);
      }).fail(function () {
        alert("Failed to save Admin config");
      }).always(function () {
        self.saveEmonCmsFetching(false);
      });
    }
  };

  self.saveMqttFetching = ko.observable(false);
  self.saveMqttSuccess = ko.observable(false);
  self.saveMqtt = function () {
    var mqtt = {
      server: self.config.mqtt_server(),
      topic: self.config.mqtt_topic(),
      prefix: self.config.mqtt_feed_prefix(),
      user: self.config.mqtt_user(),
      pass: self.config.mqtt_pass()
    };

    if (mqtt.server === "") {
      alert("Please enter MQTT server");
    } else {
      self.saveMqttFetching(true);
      self.saveMqttSuccess(false);
      $.post(baseEndpoint + "/savemqtt", mqtt, function (data) {
        self.saveMqttSuccess(true);
      }).fail(function () {
        alert("Failed to save MQTT config");
      }).always(function () {
        self.saveMqttFetching(false);
      });
    }
  };

  self.saveScheduleFetching = ko.observable(false);
  self.saveScheduleSuccess = ko.observable(false);
  self.saveSchedule = function () {
    var sch = {
      datestart: self.config.schdatestart(),
      timestart: self.config.schtimestart(),
      datestop: self.config.schdatestop(),
      timestop: self.config.schtimestop()
    };

    if (sch.datestart === "" || sch.timestart === "" || sch.timestop === "" || sch.timestop === "") {
      alert("Please enter all parameters");
    } else {
      self.saveScheduleFetching(true);
      self.saveScheduleSuccess(false);
      $.post(baseEndpoint + "/savescheduler", sch, function (data) {
        self.saveScheduleSuccess(true);
      }).fail(function () {
        alert("Failed to save Schedule config");
      }).always(function () {
        self.saveScheduleFetching(false);
      });
    }
  };
}

$(function () {
  // Activates knockout.js
  var emonesp = new EmonEspViewModel();
  ko.applyBindings(emonesp);
  emonesp.start();
});

document.getElementById("apoff").addEventListener("click", function (e) {
  var r = new XMLHttpRequest();
  r.open("POST", "apoff", true);
  r.onreadystatechange = function () {
    if (r.readyState != 4 || r.status != 200)
      return;
    var str = r.responseText;
    console.log(str);
    document.getElementById("apoff").style.display = 'none';
    if (ipaddress !== "")
      window.location = "http://" + ipaddress;
  };
  r.send();
});

document.getElementById("reset").addEventListener("click", function (e) {
  if (confirm("CAUTION: Do you really want to Factory Reset? All setting and config will be lost.")) {
	document.getElementById("reset").innerHTML = "Resetting...";
    var r = new XMLHttpRequest();
    r.open("POST", "reset", true);
    r.onreadystatechange = function () {
      if (r.readyState != 4 || r.status != 200) return;
      var str = r.responseText;
      if (str !=1) alert("Failed resetting device!");
      document.getElementById("reset").innerHTML = "Reset";
    };
    r.send();
  }
});

document.getElementById("restart").addEventListener("click", function (e) {
  if (confirm("Restart emonESP? Current config will be saved, takes approximately 10s.")) {
	document.getElementById("reset").innerHTML = "Restarting";
    var r = new XMLHttpRequest();
    r.open("POST", "restart", true);
    r.onreadystatechange = function () {
      if (r.readyState != 4 || r.status != 200) return;
      var str = r.responseText;
      if (str != 1) alert("Failed restarting device!");
      document.getElementById("reset").innerHTML = "Restart";
    };
    r.send();
  }
});

document.getElementById("ScanWifi").addEventListener("click", function(e) {
  document.getElementById("ScanWifi").innerHTML = "Scanning....";
  var r2 = new XMLHttpRequest();
  r2.open("GET","scan", true);
  r2.onreadystatechange = function () {
    if (r2.readyState != 4 || r2.status != 200) return;
    var str = r2.responseText; 
	if(str!=1) alert("Failed scanning WiFi!");
    document.getElementById("ScanWifi").innerHTML = "Scan";
  };
  r2.send();
});
// -----------------------------------------------------------------------
// Event:Upload Firmware
// -----------------------------------------------------------------------
//document.getElementById("upload").addEventListener("click", function(e) {
//  window.location.href='/upload'
//});
document.getElementById("LoadON").addEventListener("click", function(e) {
  document.getElementById("LoadON").innerHTML = "Turning ON..";
  var r2 = new XMLHttpRequest();
  r2.open("GET", "load_on", true);
  r2.onreadystatechange = function () {
    if (r2.readyState != 4 || r2.status != 200) return;
	var str = r2.responseText; 
	if(str!=1) alert("Failed turning on load!");
	document.getElementById("LoadON").innerHTML = "Turn ON";
  };
  r2.send();
});
document.getElementById("LoadOFF").addEventListener("click", function(e) {
  document.getElementById("LoadOFF").innerHTML = "Turning OFF..";
  var r2 = new XMLHttpRequest();
  r2.open("GET", "load_off", true);
  r2.onreadystatechange = function () {
    if (r2.readyState != 4 || r2.status != 200) return;
	var str = r2.responseText; 
	if(str!=1) alert("Failed turning off load!");
	document.getElementById("LoadOFF").innerHTML = "Turn OFF";
  };
  r2.send();
});
document.getElementById("sch_TurnON").addEventListener("click", function(e) {
  document.getElementById("sch_TurnON").innerHTML = "Activating..";
  var r2 = new XMLHttpRequest();
  r2.open("GET", "schTurnON", true);
  r2.onreadystatechange = function () {
    if (r2.readyState != 4 || r2.status != 200) return;
	var str = r2.responseText; 
	if(str!=1) alert("Failed activated scheduler!");
	document.getElementById("sch_TurnON").innerHTML = "Active";
  };
  r2.send();
});
document.getElementById("sch_TurnOFF").addEventListener("click", function(e) {
  document.getElementById("sch_TurnOFF").innerHTML = "De-activating..";
  var r2 = new XMLHttpRequest();
  r2.open("GET", "schTurnOFF", true);
  r2.onreadystatechange = function () {
    if (r2.readyState != 4 || r2.status != 200) return;
	var str = r2.responseText; 
	if(str!=1) alert("Failed de-activated scheduler!");
	document.getElementById("sch_TurnOFF").innerHTML = "De-active";
  };
  r2.send();
});
document.getElementById("SDcard-Active").addEventListener("click", function(e) {
  document.getElementById("SDcard-Active").innerHTML = "Activating..";
  var r2 = new XMLHttpRequest();
  r2.open("GET", "SDactive", true);
  r2.onreadystatechange = function () {
    if (r2.readyState != 4 || r2.status != 200) return;
	var str = r2.responseText; 
	if(str!=1) alert("Failed activated data logger!");
	document.getElementById("SDcard-Active").innerHTML = "Active";
  };
  r2.send();
});
document.getElementById("SDcard-Deactive").addEventListener("click", function(e) {
  document.getElementById("SDcard-Deactive").innerHTML = "De-activating..";
  var r2 = new XMLHttpRequest();
  r2.open("GET", "SDdeactive", true);
  r2.onreadystatechange = function () {
    if (r2.readyState != 4 || r2.status != 200) return;
	var str = r2.responseText; 
	if(str!=1) alert("Failed de-activated data logger!");
	document.getElementById("SDcard-Deactive").innerHTML = "De-active";
  };
  r2.send();
});
document.getElementById("SDcard-Reset").addEventListener("click", function(e) {
  if (confirm("CAUTION: Do you really want to Reset SD Card? All stored data will be lost.")) {
    document.getElementById("SDcard-Reset").innerHTML = "Resetting..";
    var r2 = new XMLHttpRequest();
    r2.open("GET", "SDreset", true);
    r2.onreadystatechange = function () {
      if (r2.readyState != 4 || r2.status != 200) return;
	  var str = r2.responseText; 
	  if(str!=1) alert("Failed reset data logger!");
	  document.getElementById("SDcard-Reset").innerHTML = "Reset";
    };
    r2.send();
  }
});
