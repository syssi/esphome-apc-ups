#pragma once

#include "../apc_ups.h"
#include "esphome/components/switch/switch.h"
#include "esphome/core/component.h"

namespace esphome {
namespace apc_ups {
class ApcUps;
class ApcUpsSwitch : public switch_::Switch, public Component {
 public:
  void set_parent(ApcUps *parent) { this->parent_ = parent; };
  void set_on_command(const std::string &command) { this->on_command_ = command; };
  void set_off_command(const std::string &command) { this->off_command_ = command; };
  void dump_config() override;

 protected:
  void write_state(bool state) override;
  std::string on_command_;
  std::string off_command_;
  ApcUps *parent_;
};

}  // namespace apc_ups
}  // namespace esphome
