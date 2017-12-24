#include <WiFi.h>
#include <ArduinoJson.h>
WiFiServer server(80); 

char   host[] = "api.thingspeak.com"; // ThingSpeak address
String APIkey1 = "320098";             // Thingspeak Read Key, works only if a PUBLIC viewable channel
String APIkey2 = "320099";             // Thingspeak Read Key, works only if a PUBLIC viewable channel

const int httpPort = 80;
 
const char *ssid     = "your_SSID"; 
const char *password = "your_PASSWORD"; 
WiFiClient client;
const unsigned long HTTP_TIMEOUT = 10000;  // max respone time from server

String CH1_field1reading, CH1_field2reading,
       CH2_field1reading, CH2_field2reading;

int    CH1_field1reading_int, CH1_field2reading_int,
       CH2_field1reading_int, CH2_field2reading_int;

String currentAPIKey;


void setup(){
  Serial.begin(115200);
  WiFi.begin(ssid,password);
}

void loop(){
  currentAPIKey = APIkey1;
  RetrieveTSChannelData(APIkey1);
  currentAPIKey = APIkey2;
  RetrieveTSChannelData(APIkey2);
  Serial.println("Channel-1 field1 value = "+CH1_field1reading);
  Serial.println("Channel-1 field2 value = "+CH1_field2reading);
  Serial.println("Channel-2 field1 value = "+CH2_field1reading);
  Serial.println("Channel-2 field2 value = "+CH2_field2reading);
  CH1_field1reading_int = CH1_field1reading.toInt();
  CH1_field2reading_int = CH1_field2reading.toInt();
  CH2_field1reading_int = CH2_field1reading.toInt();
  CH2_field2reading_int = CH2_field2reading.toInt();
  delay(60000); //Wait before we request again
}

void RetrieveTSChannelData(String APIkey) {  // Receive data from Thingspeak
  static char responseBuffer[3*1024]; // Buffer for received data
  client = server.available(); 
  if (!client.connect(host, httpPort)) { 
    Serial.println("connection failed"); 
    return; 
  } 
  String url = "/channels/" + APIkey; // Start building API request string
  url += "/fields/1.json?results=5";  // 5 is the results request number, so 5 are returned, 1 woudl return the last result received
  client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");
  while (!skipResponseHeaders());                      // Wait until there is some data and skip headers
  while (client.available()) {                         // Now receive the data
    String line = client.readStringUntil('\n');
    if ( line.indexOf('{',0) >= 0 ) {                  // Ignore data that is not likely to be JSON formatted, so must contain a '{'
      Serial.println(line);                            // Show the text received
      line.toCharArray(responseBuffer, line.length()); // Convert to char array for the JSON decoder
      decodeJSON(responseBuffer);                      // Decode the JSON text
    }
  }
  client.stop();
}

bool skipResponseHeaders() { 
  char endOfHeaders[] = "\r\n\r\n"; // HTTP headers end with an empty line 
  client.setTimeout(HTTP_TIMEOUT); 
  bool ok = client.find(endOfHeaders); 
  if (!ok) { Serial.println("No response or invalid response!"); } 
  return ok; 
} 

bool decodeJSON(char *json) {
  StaticJsonBuffer <3*1024> jsonBuffer;
  char *jsonstart = strchr(json, '{'); // Skip characters until first '{' found and ignore length, if present
  if (jsonstart == NULL) {
    Serial.println("JSON data missing");
    return false;
  }
  json = jsonstart;
  JsonObject& root = jsonBuffer.parseObject(json); // Parse JSON
  if (!root.success()) {
    Serial.println(F("jsonBuffer.parseObject() failed"));
    return false;
  }
  JsonObject& root_data = root["channel"]; // Begins and ends within first set of { }
  String id   = root_data["id"];
  String name = root_data["name"];
  String field1_name = root_data["field1"]; // Extracts field name in this example field1 is called 'Pressure'
  String field2_name = root_data["field2"]; // Extracts field name in this example field2 is called 'Temperature'
  String field3_name = root_data["field3"]; // Extracts field name in this example field3 is called 'Humidity'
  String datetime    = root_data["updated_at"];
  Serial.println("\n\n Channel id: "+id+" Name: "+ name);
  Serial.println(" Readings last updated at: "+datetime);
  
  for (int result = 0; result < 5; result++){
    JsonObject& channel = root["feeds"][result]; // Now we can read 'feeds' values and so-on
    String entry_id     = channel["entry_id"];
    String field1value  = channel["field1"];
    String field2value  = channel["field2"];
    String field3value  = channel["field3"];
    Serial.print(" Field1 entry number ["+entry_id+"] had a value of: ");Serial.println(field1value);
    Serial.print(" Field2 entry number ["+entry_id+"] had a value of: ");Serial.println(field2value);
    Serial.print(" Field3 entry number ["+entry_id+"] had a value of: ");Serial.println(field3value);
    if (currentAPIKey == APIkey1) CH1_field1reading = field1value;
    if (currentAPIKey == APIkey1) CH1_field2reading = field2value;
    if (currentAPIKey == APIkey2) CH2_field1reading = field1value;
    if (currentAPIKey == APIkey2) CH2_field2reading = field2value;
  }
}
  //Thing speak response to GET request(headers removed) and /result=1:
  /*
   * 
    {"channel":{"id":320098,"name":"Test Channel","latitude":"0.0","longitude":"0.0","field1":"Pressure","field2":"Temperature","field3":"Humidity",
     "created_at":"2017-08-21T13:22:12Z","updated_at":"2017-08-26T22:18:16Z","last_entry_id":85},"feeds":[{"created_at":"2017-08-26T22:18:16Z","entry_id":85,"field1":"40"}]}
   */




