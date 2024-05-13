#include "esphome/core/log.h"
#include "a7670_lte.h"
#include <Arduino.h>
// TinyGsm modem(Serial2);
TinyGsm        thisModem(SerialAT);
TinyGsmClient client(thisModem);
PubSubClient  mqtt(client);
namespace esphome {
namespace a7670_lte {
static const char *TAG = "a7670_lte.component";

String A7670LTEComponent::setAPN() {
  thisModem.sendAT("+CGDCONT=1,'IP','iot.truphone.com'");
  String res;
  thisModem.waitResponse(1000L,res);
  // Serial.println("res is ("+res+")");
  res.replace("\r\n", " ");
  return res;
}

String A7670LTEComponent::getNetwork() {
  thisModem.sendAT("+COPS?");
  String res;
  thisModem.waitResponse(1000L,res);
  res.replace("\r\n", " ");
  return res;
}
void A7670LTEComponent::powerOnModem() {
  if (modemPoweredOn == 0) {
    ESP_LOGD(TAG, "Modem is not powered on. Cannot send initial message");
    
  }
  if (millis() > 10000 && mqttSent == 0) {
    ESP_LOGD(TAG, "10 seconds have passed. Setting up MQTT... ");
    mqttSent = 1;
  }
  else {
    
  }
  // wasSetUp = 1;
}

void A7670LTEComponent::setup() {
  SerialAT.begin(115200, SERIAL_8N1, MODEM_RX_PIN, MODEM_TX_PIN);
  ESP_LOGD(TAG, "Powering on modem... ");
  #ifdef BOARD_POWERON_PIN
    pinMode(BOARD_POWERON_PIN, OUTPUT);
    digitalWrite(BOARD_POWERON_PIN, HIGH);
  #endif
  // Set modem reset pin ,reset modem
  pinMode(MODEM_RESET_PIN, OUTPUT);
  digitalWrite(MODEM_RESET_PIN, !MODEM_RESET_LEVEL); delay(100);
  digitalWrite(MODEM_RESET_PIN, MODEM_RESET_LEVEL); delay(2600);
  digitalWrite(MODEM_RESET_PIN, !MODEM_RESET_LEVEL);
  pinMode(BOARD_PWRKEY_PIN, OUTPUT);
  digitalWrite(BOARD_PWRKEY_PIN, LOW);
  delay(100);
  digitalWrite(BOARD_PWRKEY_PIN, HIGH);
  delay(100);
  digitalWrite(BOARD_PWRKEY_PIN, LOW);
  ESP_LOGD(TAG, "Done Powering on modem... ");
  modemPoweredOn = 1;
}

void A7670LTEComponent::initializeModem() {
  static const char* thisTag = "A7670 modem initialization";
  if (waitPeriodPassed == 0) {
    ESP_LOGI(thisTag, "Modem not ready to be initialized. 10 seconds following boot is needed.");
    return;
  }
  if (modemStarted == 0) {
      ESP_LOGD(thisTag, "Modem starting...");
      SerialAT.begin(115200, SERIAL_8N1, MODEM_RX_PIN, MODEM_TX_PIN);
      modemStarted = 1;
  }
  ESP_LOGD(thisTag, "Modem started.");
  if (modemInitialized == 0) {
    ESP_LOGD(thisTag, "Modem initializing...");
    ESP_LOGD(thisTag, "Modem setting APN...");
    String result = this->setAPN();
    ESP_LOGD(thisTag, "The result of setting the APN is (%s)",result.c_str()); 
    if (!thisModem.init()) {
            ESP_LOGD(thisTag,"Modem initialization failed.");
            return;
    }
    modemInitialized = 1;
  }
  ESP_LOGD(thisTag, "Modem initialized.");
  // String name = thisModem.getModemName();
  // ESP_LOGD(thisTag,"Modem Name:", name);
  // String modemInfo = thisModem.getModemInfo();
  // ESP_LOGD(thisTag, "Modem Info:", modemInfo);
  if (networkAvailable == 0) {
    ESP_LOGD(thisTag, "Waiting for network for 2 seconds.");
    if (!thisModem.waitForNetwork(2000)) {
       ESP_LOGD(thisTag, "Network not available yet.");
       return;
    }
    networkAvailable = 1;
  }
  ESP_LOGD(thisTag, "Network available.");
  if (networkConnected == 0) {
    ESP_LOGD(thisTag, "Network connection...");
    if (thisModem.isNetworkConnected()) {
      networkConnected = 1;
    }
    else {
      ESP_LOGD(thisTag, "Network is not yet connected");
      return;
    }
    modemReady = 1;
  }
  String network = this->getNetwork();
  int strength = thisModem.getSignalQuality();
  String strengthMessage = "Network connected to ("+network+") with quality ("+(String) strength+")";
  ESP_LOGD(thisTag, strengthMessage.c_str());
  if (apnConnected == 0) {
    ESP_LOGD(thisTag, "APN connecting...");
    thisModem.gprsConnect("iot.truphone.com");
    ESP_LOGD(thisTag, "APN call to gprsConnect() complete.");
    if (thisModem.isGprsConnected()) {
      apnConnected = 1;
    }
    else {
      ESP_LOGD(thisTag, "APN Not connected to GPRS");
      return;
    }
  }
  ESP_LOGD(thisTag, "APN GPRS connected");
}

void A7670LTEComponent::loop() {
  if (millis() > 10000) {
    waitPeriodPassed = 1;
  }
}
void A7670LTEComponent::dump_config(){
    ESP_LOGCONFIG(TAG, "A7670Component config goes here. Was I set up? (%d)",wasSetUp);
}

void A7670LTEComponent::sendCommand(std::string command) {
  ESP_LOGD(TAG, "Trying to execute AT cmmand (%s)",command.c_str());
  thisModem.sendAT(command.c_str());
  String res;
  thisModem.waitResponse(1000L,res);
  res.replace("\r\n", "");
  res.replace("\n", "");
  ESP_LOGD(TAG, "This was the result (%s)",res.c_str());
}
void A7670LTEComponent::publishMessage(std::string topic, std::string message) {
  if (waitPeriodPassed == 0) {
    ESP_LOGI(TAG, "Modem not ready");
    return;
  }
  if (apnConnected == 0) {
    ESP_LOGD(TAG, "APN not connected. Going through the initialization process...");
    return;
  }
  ESP_LOGI(TAG, "Publishing...");
  if (1==2) {
  this->sendCommand("AT+CMQTTSTOP");
  this->sendCommand("AT+CMQTTSTART");
  this->sendCommand("AT+CMQTTACCQ=0,'esphome-client',0");
  this->sendCommand("AT+CMQTTCONNECT=0,'192.241.215.75:1883',30,1,'chad','diesel'");
  this->sendCommand("AT+CMQTTTOPIC=0,5\ntopictopic");
  this->sendCommand("AT+CMQTTPAYLOAD=0,6\npayloadpayload");
  this->sendCommand("AT+CMQTTPUB=0,1,60\n");
  }
  else {
    if (!mqtt.connected()) {
      this->mqttConnect();
    }
    mqtt.publish(topic.c_str(), message.c_str());  
  }
}

boolean A7670LTEComponent::mqttConnect() {
    Serial.print("Connecting to dieselwurks");
    mqtt.setServer("dieselwurks.com", 1883);
    // Connect to MQTT Broker
    // bolean status = mqtt.connect("GsmClientTest");

    // Or, if you want to authenticate MQTT:
    boolean status = mqtt.connect("cell", "chad", "diesel");

    if (status == false) {
        Serial.println(" fail");
        return false;
    }
    Serial.println(" success");
    int strength = thisModem.getSignalQuality();
    Serial.println("Quality: " + (String)strength);
    String message = "Alive on 1Global. Strength is ("+(String) strength+")";
    Serial.println("Publishing this message: " + message);
    mqtt.publish("chadwashere", message.c_str());
    Serial.println("Done publishing.");
    return mqtt.connected();
}


}  // namespace empty_component
}  // namespace esphome
