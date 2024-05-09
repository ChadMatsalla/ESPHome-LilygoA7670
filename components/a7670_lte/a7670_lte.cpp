#include "esphome/core/log.h"
#include "a7670_lte.h"

namespace esphome {
namespace a7670_lte {

static const char *TAG = "a7670_lte.component";

void A7670LTEComponent::setup() {
  ESP_LOGD("Setting up modem...");
}

void A7670LTEComponent::loop() {

}

void A7670LTEComponent::dump_config(){
    ESP_LOGCONFIG(TAG, "Empty component");
}

void A7670LTEComponent::publishMessage(std::string topic, std::string message) {
  ESP_LOGI("Publishing...");
}


}  // namespace empty_component
}  // namespace esphome
