#pragma once

#include "esphome/core/component.h"

namespace esphome {
namespace a7670_lte {

class A7670LTEComponent : public Component {
 public:
  void setup() override;
  void loop() override;
  void dump_config() override;
  void connectToServer();
  void publishMessage(std::string topic, std::string message);
};


}  // namespace empty_component
}  // namespace esphome
