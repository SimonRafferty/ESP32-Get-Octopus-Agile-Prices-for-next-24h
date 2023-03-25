#include <WiFi.h>
#include <WiFiManager.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

WiFiManager wifiManager;

const char* host = "api.octopus.energy";
const int httpsPort = 443;
const char* url = "/v1/products/AGILE-18-02-21/electricity-tariffs/E-1R-AGILE-18-02-21-H/standard-unit-rates/";
const int NUM_TARIFFS = 24;
double tariffs[NUM_TARIFFS];

void setup() {
  Serial.begin(115200);

  wifiManager.autoConnect("AutoConnectAP");

  Serial.println("Connected to WiFi!");

  // create an HTTP client object
  HTTPClient http;

  // set the target URL
  http.begin("https://" + String(host) + String(url));

  // send a GET request to the Octopus API endpoint
  int httpResponseCode = http.GET();

  if (httpResponseCode == HTTP_CODE_OK) {
    // read the server's response
    String response = http.getString();

    // parse the JSON response
    const size_t bufferSize = 2*JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(3) + 27*JSON_OBJECT_SIZE(5) + 3*JSON_OBJECT_SIZE(7) + 730;
    DynamicJsonBuffer jsonBuffer(bufferSize);
    JsonObject& root = jsonBuffer.parseObject(response);

    if (!root.success()) {
      Serial.println("JSON parsing failed!");
      return;
    }

    // extract the tariff values from the JSON response and store them in the tariffs array
    const JsonArray& results = root["results"];
    for (int i = 0; i < NUM_TARIFFS; i++) {
      const JsonObject& result = results[i];
      const double valueIncVat = result["value_inc_vat"];
      tariffs[i] = valueIncVat;
    }

    // print the tariffs array to the serial monitor
    for (int i = 0; i < NUM_TARIFFS; i++) {
      Serial.println(tariffs[i]);
    }
  } else {
    Serial.println("HTTP GET request failed!");
  }

  // close the HTTP connection
  http.end();
}

void loop() {
  // do nothing
}
