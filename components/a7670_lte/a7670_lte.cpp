#include "esphome.h"
#include "esphome/core/log.h"
#include "a7670_lte.h"
#include <Arduino.h>
#include <PubSubClient.h>

// TinyGsm modem(Serial2);
TinyGsm        thisModem(SerialAT);
TinyGsmClient client(thisModem);
PubSubClient  pubsub(client);
namespace esphome {
namespace a7670_lte {
static const char *TAG = "a7670_lte.component";

void A7670LTEComponent::set_modem_warmup_delay(int delay) {
  this->modem_warmup_delay = delay;
}
bool A7670LTEComponent::isConnected() {
  ESP_LOGD("csm","Someone called isConnected()");
  return false;
}
bool A7670LTEComponent::is_connected() {
  ESP_LOGD("csm","Someone called is_connected()");
  return false;
}

bool A7670LTEComponent::is_disabled() {
  ESP_LOGD("csm","Someone called is_disabled()");
  return false;
}
bool A7670LTEComponent::can_proceed() {
  // ESP_LOGD("csm","Someone called can_proceed()");
  if (this->modem_ready == 1) {
    return true;
  }
  return false;
}


bool isDisabled() {
  ESP_LOGD("csm","Someone called isDisabled()");
  return false;
}

void A7670LTEComponent::set_apn(std::string thisapn) {
  this->apn = thisapn;
}

String A7670LTEComponent::get_network() {
  thisModem.sendAT("+COPS?");
  String res;
  thisModem.waitResponse(1000L,res);
  res.replace("\r\n", " ");
  return res;
}
void A7670LTEComponent::power_on_modem() {
  if (this->modem_powered_on == 0) {
    ESP_LOGD(TAG, "Modem is not powered on. Cannot send initial message");
    
  }
  if (millis() > 10000 && this->mqtt_sent == 0) {
    ESP_LOGD(TAG, "10 seconds have passed. Setting up MQTT... ");
    this->mqtt_sent = 1;
  }
  else {
    
  }
  // wasSetUp = 1;
}

void A7670LTEComponent::setup() {
  if (this->use_cellular == 1) {
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
    this->modem_powered_on = 1;
  }
  else {
    ESP_LOGD(TAG, "Not setting up the modem because enable_cellular is 0");
  }
}

bool A7670LTEComponent::modem_is_ready() {
  if (this->modem_ready == 1) { return true; }
  return false;
}
void A7670LTEComponent::initializeModem() {
  if (this->modem_ready == 1) { return; }
  static const char* thisTag = "A7670 modem initialization";
  // ESP_LOGD(TAG, "mqtt should connect to server  %s with username %s and password %s", this->mqtt_server.c_str(), this->mqtt_username.c_str(), this->mqtt_password.c_str());
  if (this->wait_period_passed == 0) {
    ESP_LOGI(thisTag, "Modem not ready to be initialized. 10 seconds following boot is needed.");
    return;
  }
  if (this->modem_started == 0) {
      ESP_LOGD(thisTag, "Modem starting...");
      SerialAT.begin(115200, SERIAL_8N1, MODEM_RX_PIN, MODEM_TX_PIN);
      this->modem_started = 1;
  }
  ESP_LOGD(thisTag, "Modem started.");
  if (this->apn_written == 0) {
    std::string command = "+CGDCONT=1,\"IP\",\""+this->apn+"\"";
    const char* thisCommand = command.c_str();
    ESP_LOGD(thisTag, "Writing APN (%s) to modem with command (%s)...",this->apn.c_str(),thisCommand);
    thisModem.sendAT(thisCommand);
    String res;
    thisModem.waitResponse(1000L,res);
    // Serial.println("res is ("+res+")");
    res.replace("\r\n", " ");
    ESP_LOGD(thisTag, "Modem replied (%s). Doublechecking...",res.c_str());
    String res2;
    thisModem.sendAT("+CGDCONT?");
    thisModem.waitResponse(1000L,res2);
    ESP_LOGD(thisTag, "APN is (%s)",res2.c_str());
    this->apn_written = 1;
  }
  if (this->modem_initialized == 0) {
    ESP_LOGD(thisTag, "Modem initializing...");
    // ESP_LOGD(thisTag, "Modem setting APN...");
    // String result = this->set_apn();
    // ESP_LOGD(thisTag, "The result of setting the APN is (%s)",result.c_str()); 
    if (!thisModem.init()) {
            ESP_LOGD(thisTag,"Modem initialization failed.");
            return;
    }
    this->modem_initialized = 1;
  }
  ESP_LOGD(thisTag, "Modem initialized.");
  // String name = thisModem.getModemName();
  // ESP_LOGD(thisTag,"Modem Name:", name);
  // String modemInfo = thisModem.getModemInfo();
  // ESP_LOGD(thisTag, "Modem Info:", modemInfo);
  // if (this->network_available == 0) {
  //  ESP_LOGD(thisTag, "Network not available yet.");
  //  return;
  //}
  // this->network_available = 1;
  // ESP_LOGD(thisTag, "Network available.");
  if (this->network_connected == 0) {
    ESP_LOGD(thisTag, "Modem connecting to network...");
    if (thisModem.isNetworkConnected()) {
      this->network_connected = 1;
    }
    else {
      ESP_LOGD(thisTag, "Network is not yet connected");
      return;
    }
  }
  String network = this->get_network();
  int strength = thisModem.getSignalQuality();
  String strengthMessage = "Network connected to ("+network+") with quality ("+(String) strength+")";
  ESP_LOGD(thisTag, strengthMessage.c_str());
  if (this->apn_connected == 0) {
    ESP_LOGD(thisTag, "APN connecting...");
    thisModem.gprsConnect(this->apn.c_str());
    ESP_LOGD(thisTag, "APN call to gprsConnect() complete.");
    if (thisModem.isGprsConnected()) {
     this->apn_connected = 1;
    }
    else {
      ESP_LOGD(thisTag, "APN Not connected to GPRS");
      return;
    }
  }
  ESP_LOGD(thisTag, "APN GPRS connected");
  this->modem_ready = 1;
}

void A7670LTEComponent::loop() {
  if (millis() > 10000) {
    this->wait_period_passed = 1;
  }
}
void A7670LTEComponent::dump_config(){
    ESP_LOGCONFIG(TAG, "A7670Component config:");
    ESP_LOGCONFIG(TAG, "apn: (%s)",this->apn.c_str());
    ESP_LOGCONFIG(TAG, "mqtt server: (%s)",this->mqtt_server.c_str());
    ESP_LOGCONFIG(TAG, "mqtt username: (%s)",this->mqtt_username.c_str());
    ESP_LOGCONFIG(TAG, "mqtt password: (%s)",this->mqtt_password.c_str());
}

void A7670LTEComponent::send_command(std::string command) {
  if (this->use_cellular == 0) {
    ESP_LOGD(TAG, "Cannot sendCommand because use_cellular == 0");
    return;
  }
  ESP_LOGD(TAG, "Trying to execute AT cmmand (%s)",command.c_str());
  thisModem.sendAT(command.c_str());
  String res;
  thisModem.waitResponse(1000L,res);
  res.replace("\r\n", "");
  res.replace("\n", "");
  ESP_LOGD(TAG, "This was the result (%s)",res.c_str());
}
void A7670LTEComponent::publish_message(std::string topic, std::string message) {
  if (this->use_cellular == 0) {
    ESP_LOGD(TAG, "Cannot publish because use_cellular == 0");
    return;
  }
  if (this->wait_period_passed == 0) {
    ESP_LOGI(TAG, "Cannot publish because modem not ready because wait period was not passed");
    return;
  }
  if (this->apn_connected == 0) {
    ESP_LOGD(TAG, "Cannot publish because APN not connected. Going through the initialization process...");
    this->initializeModem();
    return;
  }
  if (1==2) {
    ESP_LOGI(TAG, "Publishing using AT+ commands...");
    this->send_command("AT+CMQTTSTOP");
    this->send_command("AT+CMQTTSTART");
    this->send_command("AT+CMQTTACCQ=0,'esphome-client',0");
    this->send_command("AT+CMQTTCONNECT=0,'192.241.215.75:1883',30,1,'"+this->mqtt_username+"','"+this->mqtt_password+"'");
    this->send_command("AT+CMQTTTOPIC=0,5\ntopictopic");
    this->send_command("AT+CMQTTPAYLOAD=0,6\npayloadpayload");
    this->send_command("AT+CMQTTPUB=0,1,60\n");
  }
  else {
    ESP_LOGVV(TAG, "Publishing using PubSub commands...");
    if (!pubsub.connected()) {
      this->mqtt_connect();
    }
    pubsub.publish(topic.c_str(), message.c_str());  
  }
}

void A7670LTEComponent::deep_sleep() {
  // put the modem to sleep
  ESP_LOGD(TAG,"Going to sleep. First powering down the modem...");
  this->send_command("+CPOF"); 
  ESP_LOGD(TAG,"Done. Now going to sleep for 60 seconds.");
  int sleep_time = 10*1000*1000;
  esp_sleep_enable_timer_wakeup(sleep_time);
  esp_deep_sleep_start();
  // App.register_component("goo");
  // esphome::deep_sleep::DeepSleepComponent sleeper = App.get_sensor_by_key("sleeper");
}
#ifdef DALLAS
void A7670LTEComponent::set_external_temperature_sensor(esphome::dallas_temp::DallasTemperatureSensor*&  c) {
  this->sensor_external_temperature = c;
}
void A7670LTEComponent::set_internal_temperature_sensor(esphome::dallas_temp::DallasTemperatureSensor*&  c) {
  this->sensor_internal_temperature = c;
  float temp = this->sensor_internal_temperature->get_state();
  // float temp = this->sensor_internal_temperature->get_temp_c();
  ESP_LOGD(TAG,"This is the internal temperature (%f)",temp); 
}
#endif
void A7670LTEComponent::set_mqtt_server(std::string s) {
  this->mqtt_server = s;
}
void A7670LTEComponent::set_mqtt_password(std::string p) {
  this->mqtt_password = p;
}
void A7670LTEComponent::set_mqtt_username(std::string u) {
  this->mqtt_username = u;
}
void A7670LTEComponent::check_sensors() {
  ESP_LOGD(TAG,"Checking sensors");
#ifdef DALLAS
  this->sensor_internal_temperature->update();
  float temp_internal = this->sensor_internal_temperature->get_state();
  this->publish_message("csm/beebot/sensor/internal_temperature/state","23.4");
  float temp_external = this->sensor_external_temperature->get_state();
  this->publish_message("csm/beebot/sensor/external_temperature/state","23.4");
  float state = this->sensor_external_temperature->state;
  ESP_LOGD(TAG,"Internal(%f) external(%f) state(%f)",temp_internal,temp_external,state);
#endif
  this->publish_message("csm/beebot/heartbeat","heartbeat");
  this->sensors_checked = 1;
  // this->deep_sleep();
}
void A7670LTEComponent::set_use_cellular(int value) {
  this->use_cellular = value;
}
void A7670LTEComponent::set_global(std::string name,int value) {
  
}
// IPAddress A7670LTEComponent::get_dns_address(uint8_t value) {
//   ESP_LOGD("csm","Returning ip address");
//   IPAddress returner = new IPAddress(8,8,8,8);
//   return returner;
// }

void A7670LTEComponent::power_down() {
  ESP_LOGD("csm","Powering down modem");
}
boolean A7670LTEComponent::mqtt_connect() {
    if (this->use_cellular == 0) {
      ESP_LOGD(TAG, "You asked to connect to Mqtt but also asked not to use cellular. Sorry, no can do");
      return false;
    }
    ESP_LOGD(TAG, "Connecting to %s with username %s and password %s", this->mqtt_server.c_str(), this->mqtt_username.c_str(), this->mqtt_password.c_str());
    // pubsub.setServer(this->mqtt_server.c_str(), 1883);
    // Connect to MQTT Broker
    // bolean status = mqtt.connect("GsmClientTest");

    // Or, if you want to authenticate MQTT:
    boolean status = false;
    /*
    boolean status = pubsub.connect("cell",
      this->mqtt_username.c_str(),
      this->mqtt_password.c_str());
    */

    if (status == false) {
        ESP_LOGD(TAG,"Failed to connect to MQTT");
        return false;
    }
    ESP_LOGD(TAG,"Successfully connected to MQTT");
    /*
      int strength = thisModem.getSignalQuality();
      Serial.println("Quality: " + (String)strength);
      String message = "Alive on 1Global. Strength is ("+(String) strength+")";
      Serial.println("Publishing this message: " + message);
      mqtt.publish("beebot", message.c_str());
      Serial.println("Done publishing.");
    */
    /*
    return pubsub.connected();
    */
    return false;
}


}  // namespace empty_component
}  // namespace esphome
