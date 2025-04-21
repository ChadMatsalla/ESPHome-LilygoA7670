#pragma once
// #include <utilities.h>
#define MODEM_BAUDRATE                      (115200)
#define MODEM_DTR_PIN                       (25)
#define MODEM_TX_PIN                        (26)
#define MODEM_RX_PIN                        (27)
// The modem boot pin needs to follow the startup sequence.
#define BOARD_PWRKEY_PIN                    (4)
#define BOARD_ADC_PIN                       (35)
// The modem power switch must be set to HIGH for the modem to supply power.
#define BOARD_POWERON_PIN                   (12)
#define MODEM_RING_PIN                      (33)
#define MODEM_RESET_PIN                     (5)
#define BOARD_MISO_PIN                      (2)
#define BOARD_MOSI_PIN                      (15)
#define BOARD_SCK_PIN                       (14)
#define BOARD_SD_CS_PIN                     (13)
#define BOARD_BAT_ADC_PIN                   (35)
#define MODEM_RESET_LEVEL                   HIGH
#define SerialAT                            Serial1

#define MODEM_GPS_ENABLE_GPIO               (-1)


// #include <utilities.h>
#include "esp_netif.h"
#include "TinyGsmClient.h"
// #include <PubSubClient.h>
#include "esphome/core/component.h"
#include "esphome/core/defines.h"
// #include "esphome/components/one_wire/one_wire.h"
// #include "esphome/components/dallas_temp/dallas_temp.h"
#include "esphome/components/network/ip_address.h"
// #include "esphome/components/deep_sleep/deep_sleep_component.h"
// #include "esphome/components/dallas/dallas_component.h"
// #include "dallas_component.h"


namespace esphome {
namespace a7670_lte {

class A7670LTEComponent : public Component {
 protected:
  int was_set_up = 0;
  int modem_powered_on = 0;
  int wait_period_passed = 0;
  int network_connected = 0;
  int network_available = 0;
  int modem_started = 0;
  int modem_initialized = 0;
  int apn_connected = 0;
  int apn_written = 0;
  int mqtt_sent = 0;
  int modem_ready = 0;
  int use_cellular = 0;
  int modem_warmup_delay = 0;
  int sensors_checked = 0;
  std::string apn = "";
  std::string mqtt_username = "";
  std::string mqtt_password = "";
  std::string mqtt_server = "";
  String sensor_temperature_internal = "";
  // esphome::dallas::DallasTemperatureSensor*& internal_temperature = new esphome::dallas::DallasTemperatureSensor();
  // esphome::one_wire::OneWireBus one_wire_bus = {};
  // esphome::dallas_temp::DallasTemperatureSensor* sensor_internal_temperature = {};
  // esphome::dallas_temp::DallasTemperatureSensor* sensor_external_temperature = {};
 public:
  void setup() override;
  void loop() override;
  void dump_config() override;
  void connect_to_server();
  void power_on_modem();
  void publish_message(std::string topic, std::string message);
  void initializeModem();
  void send_command(std::string command);
  void set_apn(std::string thisapn);
  String get_network();
  boolean mqtt_connect();
  void deep_sleep();
  // void set_internal_temperature_sensor(esphome::dallas_temp::DallasTemperatureSensor*& c);
  // void set_external_temperature_sensor(esphome::dallas_temp::DallasTemperatureSensor*& c);
  void set_global(std::string,int); 
  void set_use_cellular(int);
  void set_mqtt_server(std::string a);
  void set_mqtt_username(std::string u);
  void set_mqtt_password(std::string p);
  void check_sensors();
  bool modem_is_ready();
  bool isConnected();
  bool is_connected();
  bool is_disabled();
  bool can_proceed() override;
  void set_modem_warmup_delay(int delay);
  void on_shutdown() override { power_down(); }
  // IPAddress get_dns_address(uint8_t num);
 protected:
  void power_down(); 
  
};



}  // namespace empty_component
}  // namespace esphome
