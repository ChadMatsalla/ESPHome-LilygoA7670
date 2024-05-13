#pragma once
#include "utilities.h"
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
