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
#include "TinyGsmClient.h"
#include <PubSubClient.h>
#include "esphome/core/component.h"

namespace esphome {
namespace a7670_lte {

class A7670LTEComponent : public Component {
 protected:
  int wasSetUp = 0;
  int modemPoweredOn = 0;
  int waitPeriodPassed = 0;
  int networkConnected = 0;
  int networkAvailable = 0;
  int modemStarted = 0;
  int modemInitialized = 0;
  int apnConnected = 0;
  int mqttSent = 0;
  int modemReady = 0;
 public:
  void setup() override;
  void loop() override;
  void dump_config() override;
  void connectToServer();
  void powerOnModem();
  void publishMessage(std::string topic, std::string message);
  void initializeModem();
  void sendCommand(std::string command);
  String setAPN();
  String getNetwork();
  boolean mqttConnect();
};


}  // namespace empty_component
}  // namespace esphome
