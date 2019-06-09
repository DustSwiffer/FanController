#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <IRutils.h>

#define WLAN_SSID       "[SSID of your network]"
#define WLAN_PASS       "[Password of your network]"


#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME  "[username of Adafruit]"
#define AIO_KEY       "[Secretkey of Adafruit]"

//raw ir data needed to send to fan (can be different than yours)
uint16_t rawData0[] = {686, 558,  680, 572,  662, 1482,  684, 1494,  674, 1502,  666, 1492,  680, 1486,  688, 552,  690, 1484,  686, 1480,  690, 562,  678, 572,  668, 4574,  686, 572,  666, 570,  666, 1500,  666, 1486,  684, 1486,  684, 1504,  668, 1492,  678, 574,  666, 1502,  668, 1506,  664, 576,  668, 572,  664, 4596,  670, 560,  674, 572,  666, 1480,  684, 1504,  666, 1506,  666, 1504,  666, 1504,  666, 580,  662, 1504,  666, 1504,  666, 576,  666, 572,  666};
uint16_t rawData1[] = {704, 550,  690, 570,  664, 1500,  668, 1482,  686, 1466,  708, 1480,  692, 1480,  692, 572,  668, 1482,  690, 574,  666, 1486,  686, 572,  666, 4574,  690, 560,  676, 570,  666, 1500,  666, 1504,  668, 1482,  688, 1480,  688, 1486,  688, 574,  666, 1504,  670, 574,  666, 1504,  666, 560,  680, 4594,  670, 550,  686, 570,  666, 1480,  688, 1502,  666, 1504,  668, 1504,  666, 1504,  668, 574,  668, 1486,  688, 574,  664, 1486,  686, 574,  668};
uint16_t rawData2[] = {694, 548,  686, 550,  688, 1476,  692, 1460,  708, 1488,  682, 1482,  690, 1486,  686, 558,  686, 558,  682, 1490,  686, 562,  678, 558,  682, 4596,  668, 570,  666, 572,  666, 1490,  676, 1490,  680, 1506,  668, 1504,  666, 1504,  670, 550,  694, 574,  666, 1492,  682, 560,  682, 572,  666, 4578,  690, 570,  666, 568,  668, 1476,  692, 1488,  680, 1496,  678, 1504,  666, 1488,  686, 574,  668, 574,  666, 1490,  682, 578,  666, 570,  668, 4738,  668, 572,  666, 570,  664, 1504,  668, 1488,  682, 1504,  668, 1482,  688, 1506,  668, 576,  664, 578,  668, 1506,  666, 576,  666, 574,  666, 4598,  668, 568,  666, 574,  666, 1502,  666, 1504,  668, 1504,  666, 1506,  664, 1506,  668, 574,  666, 576,  666, 1490,  684, 574,  666, 572,  668, 4596,  668, 570,  666, 568,  666, 1502,  666, 1502,  668, 1506,  666, 1504,  670, 1502,  666, 576,  668, 574,  668, 1506,  666, 578,  666, 572,  666};
uint16_t rawData3[] = {724, 570,  668, 568,  668, 1478,  688, 1460,  710, 1484,  690, 1486,  684, 1492,  680, 562,  678, 1484,  686, 576,  668, 574,  666, 576,  664, 4596,  666, 560,  680, 568,  664, 1502,  666, 1460,  708, 1486,  686, 1502,  668, 1484,  690, 564,  676, 1490,  684, 564,  676, 576,  666, 574,  666, 4580,  686, 568,  668, 568,  668, 1498,  670, 1502,  666, 1490,  682, 1484,  686, 1492,  680, 574,  668, 1484,  688, 574,  668, 576,  662, 574,  666, 4730,  678, 568,  668, 570,  664, 1500,  668, 1492,  678, 1504,  668, 1486,  686, 1504,  668, 574,  666, 1490,  682, 574,  668, 576,  666, 572,  666, 4598,  666, 570,  670, 568,  666, 1500,  668, 1502,  666, 1506,  666, 1506,  664, 1508,  666, 578,  666, 1504,  668, 574,  666, 578,  664, 572,  668, 4596,  668, 570,  668, 568,  668, 1498,  670, 1502,  670, 1502,  668, 1502,  668, 1504,  668, 574,  670, 1502,  668, 574,  668, 574,  668, 572,  670};

const uint16_t kIrLed = 14;

IRsend irsend(kIrLed); 

WiFiClient client;

Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

Adafruit_MQTT_Subscribe slider = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/fan-speed-1");

void MQTT_connect();

void setup() {
  irsend.begin();
  Serial.begin(115200);
  delay(10);

  Serial.println(F("Adafruit MQTT demo"));

  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());

  mqtt.subscribe(&slider);
}

uint32_t x=0;

void loop() {

  MQTT_connect();

  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000))) {
    if (subscription == &slider){
      Serial.print(F("Slider: "));
      Serial.println((char *)slider.lastread);
        uint16_t sliderval = atoi((char *)slider.lastread);
      switch(sliderval){
        case 0:
            irsend.sendRaw(rawData0, sizeof(rawData0) / sizeof(rawData0[0]), 38);
          break;
        case 1:
            irsend.sendRaw(rawData1, sizeof(rawData1) / sizeof(rawData1[0]), 38);
          break;
        case 2:
            irsend.sendRaw(rawData2, sizeof(rawData2) / sizeof(rawData2[0]), 38);
          break;
        case 3:
            irsend.sendRaw(rawData3, sizeof(rawData3) / sizeof(rawData3[0]), 38);
          break;
        }


      
    }
  }
}

void MQTT_connect() {
  int8_t ret;

  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) {
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}
