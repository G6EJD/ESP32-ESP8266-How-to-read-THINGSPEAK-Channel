# ESP32-ESP8266-How-to-read-THINGSPEAK-Channel
Reading THINGSPEAK JSON formatted responses to GET request

When you issue a GET request to THINGSPEAK>COM for channel data like this:

GET https://api.thingspeak.com/channels/320098/fields/1.json?results=5

The Thingspeak server will respond with JSON formatted results like this:

{"channel":{"id":320098,"name":"Test
Channel","latitude":"0.0","longitude":"0.0",
"field1":"Pressure","field2":"Temperature","field3":"Humidity","created_at":"2017-08-21T13:22:12Z",
"updated_at":"2017-08-
8T09:53:29Z","last_entry_id":85},
"feeds":[
{"created_at":"2017-08-26T22:15:06Z","entry_id":81,"field1":"40"},
{"created_at":"2017-08-26T22:15:29Z","entry_id":82,"field1":"41"},
{"created_at":"2017-08-26T22:15:52Z","entry_id":83,"field1":"42"},
{"created_at":"2017-08-26T22:17:53Z","entry_id":84,"field1":"43"},
{"created_at":"2017-08-26T22:18:16Z","entry_id":85,"field1":"44"}
]}

This programme uses the Arduino JSON decoder to retreive the results.

