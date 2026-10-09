#include "apc_ups_switch.h"
#include "esphome/core/log.h"
#include "esphome/core/application.h"

// Fallback for ESPHome < 2026.10.0
#ifndef ESPHOME_LOG_TAG
#define ESPHOME_LOG_TAG(name, tag) static const char *const name = tag
#endif

namespace esphome::apc_ups {

ESPHOME_LOG_TAG(TAG, "apc_ups.switch");

void ApcUpsSwitch::dump_config() { LOG_SWITCH(TAG, "ApcUps Switch", this); }
void ApcUpsSwitch::write_state(bool state) {
  if (state) {
    if (!this->on_command_.empty()) {
      this->parent_->switch_command(this->on_command_);
    }
  } else {
    if (!this->off_command_.empty()) {
      this->parent_->switch_command(this->off_command_);
    }
  }
  this->publish_state(state);
}

}  // namespace esphome::apc_ups
