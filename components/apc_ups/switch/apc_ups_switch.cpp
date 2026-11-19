#include "apc_ups_switch.h"

namespace esphome::apc_ups {

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
