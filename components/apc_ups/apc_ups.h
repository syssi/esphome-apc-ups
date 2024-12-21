#pragma once

#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/switch/switch.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "esphome/components/uart/uart.h"
#include "esphome/core/automation.h"
#include "esphome/core/component.h"

namespace esphome {
namespace apc_ups {

enum ENUMPollingCommand {
  POLLING_Y = 0,         // Enable smart mode
  POLLING_A = 1,         // Front panel test
  POLLING_B = 2,         // Battery voltage
  POLLING_C = 3,         // Internal temperature
  POLLING_D = 4,         // Runtime calibration
  POLLING_E = 5,         // Automatic self test interval @TODO
  POLLING_F = 6,         // Grid frequency
  POLLING_G = 7,         // Cause of last transfer to battery
  POLLING_I = 8,         // Measure-UPS alarm enable @TODO
  POLLING_J = 9,         // Measure-UPS alarm status @TODO
  POLLING_K = 10,        // Shutdown with grace period (no return) ---------------------------- @NEW
  POLLING_L = 11,        // Input line voltage
  POLLING_M = 12,        // Maximum line voltage
  POLLING_N = 13,        // Minimum line voltage
  POLLING_O = 14,        // Output voltage
  POLLING_P = 15,        // Power load in %
  POLLING_Q = 16,        // Status flags
  POLLING_R = 17,        // Turn dumb @TODO
  POLLING_S = 18,        // Soft shutdown ----------------------------------------------------- @NEW
  POLLING_U = 19,        // Simulate power failure
  POLLING_V = 20,        // Old firmware version
  POLLING_W = 21,        // Self test
  POLLING_X = 22,        // Self test results
  POLLING_Z = 23,        // Shutdown immediately ---------------------------------------------- @NEW
  POLLING_LOWER_A = 24,  // Protocol info  @TODO
  POLLING_LOWER_B = 25,  // Firmware revision
  POLLING_LOWER_C = 26,  // UPS local identifier
  POLLING_LOWER_E = 27,  // Return threshold
  POLLING_LOWER_F = 28,  // Battery level
  POLLING_LOWER_G = 29,  // Nominal battery voltage
  POLLING_LOWER_H = 30,  // Measure-UPS ambient humidity
  POLLING_LOWER_I = 31,  // Measure-UPS dry contacts @TODO
  POLLING_LOWER_J = 32,  // Estimated runtime
  POLLING_LOWER_K = 33,  // Alarm delay
  POLLING_LOWER_L = 34,  // Low transfer voltage
  POLLING_LOWER_M = 35,  // Manufacture date
  POLLING_LOWER_N = 36,  // Serial number
  POLLING_LOWER_O = 37,  // Nominal Output Voltage
  POLLING_LOWER_P = 38,  // Shutdown grace delay @TODO
  POLLING_LOWER_Q = 39,  // Low battery warning @TODO
  POLLING_LOWER_R = 40,  // Wakeup delay @TODO
  POLLING_LOWER_S = 41,  // Sensitivity @TODO
  POLLING_LOWER_T = 42,  // Measure-UPS ambient temperature
  POLLING_LOWER_U = 43,  // Upper transfer voltage
  POLLING_LOWER_V = 44,  // Measure-UPS firmware
  POLLING_LOWER_X = 45,  // Last battery change date ------------------------------------------ @NEW 
  POLLING_LOWER_Y = 46,  // Copyright notice
  POLLING_LOWER_Z = 47,  // Reset to factory settings @TODO
  POLLING_9 = 48,        // Line quality
  POLLING_CTRL_A = 49,   // Model name
  POLLING_DATE = 50,     // New battery change date before saving ----------------------------- @NEW
  POLLING_SDATE = 51,    // Save last battery change date ------------------------------------- @NEW
  POLLING_CTRL_N = 52,   // Turn ON UPS ------------------------------------------------------- @NEW
};
struct PollingCommand {
  uint8_t *command;
  uint8_t length = 0;
  uint8_t errors;
  ENUMPollingCommand identifier;
};

#define APC_UPS_VALUED_ENTITY_(type, name, ident, polling_command, value_type) \
 protected: \
  value_type value_##name##_; \
  APC_UPS_ENTITY_(type, name, ident, polling_command)

#define APC_UPS_ENTITY_(type, name, ident, polling_command) \
 protected: \
  type *name##_{}; /* NOLINT */ \
\
 public: \
  void set_##name(type *name) { /* NOLINT */ \
    this->name##_ = name; \
    this->add_polling_command_(#polling_command, POLLING_##ident); \
  } \


#define APC_UPS_SENSOR(name, ident, polling_command, value_type) \
  APC_UPS_VALUED_ENTITY_(sensor::Sensor, name, ident, polling_command, value_type)
#define APC_UPS_SWITCH(name, ident, polling_command) \
  APC_UPS_ENTITY_(switch_::Switch, name, ident, polling_command)
#define APC_UPS_VALUED_SWITCH(name, ident, polling_command, value_type) \
  APC_UPS_VALUED_ENTITY_(switch_::Switch, name, ident, polling_command, value_type)
#define APC_UPS_VALUED_BINARY_SENSOR(name, ident, polling_command, value_type) \
  APC_UPS_VALUED_ENTITY_(binary_sensor::BinarySensor, name, ident, polling_command, value_type)
#define APC_UPS_BINARY_SENSOR(name, ident, polling_command) \
  APC_UPS_ENTITY_(binary_sensor::BinarySensor, name, ident, polling_command)
#define APC_UPS_VALUED_TEXT_SENSOR(name, ident, polling_command, value_type) \
  APC_UPS_VALUED_ENTITY_(text_sensor::TextSensor, name, ident, polling_command, value_type)
#define APC_UPS_TEXT_SENSOR(name, ident, polling_command) \
  APC_UPS_ENTITY_(text_sensor::TextSensor, name, ident, polling_command)

class ApcUps : public uart::UARTDevice, public PollingComponent {
  APC_UPS_VALUED_BINARY_SENSOR(smart_mode, Y, Y, bool)
  APC_UPS_SENSOR(battery_voltage, B, B, float)
  APC_UPS_SENSOR(internal_temperature, C, C, float)
  APC_UPS_SENSOR(grid_frequency, F, F, float)
  APC_UPS_VALUED_TEXT_SENSOR(cause_of_last_transfer, G, G, std::string)
  APC_UPS_SENSOR(grid_voltage, L, L, float)
  APC_UPS_SENSOR(max_grid_voltage, M, M, float)
  APC_UPS_SENSOR(min_grid_voltage, N, N, float)
  APC_UPS_SENSOR(ac_output_voltage, O, O, float)
  APC_UPS_SENSOR(ac_output_load, P, P, float)
  APC_UPS_SENSOR(status_bitmask, Q, Q, int)
  APC_UPS_BINARY_SENSOR(runtime_calibration, Q, Q)
  APC_UPS_BINARY_SENSOR(smart_trim, Q, Q)
  APC_UPS_BINARY_SENSOR(smart_boost, Q, Q)
  APC_UPS_BINARY_SENSOR(on_line, Q, Q)
  APC_UPS_BINARY_SENSOR(on_battery, Q, Q)
  APC_UPS_BINARY_SENSOR(output_overloaded, Q, Q)
  APC_UPS_BINARY_SENSOR(battery_low, Q, Q)
  APC_UPS_BINARY_SENSOR(replace_battery, Q, Q)
  APC_UPS_SENSOR(ambient_humidity, LOWER_H, h, float)
  APC_UPS_SENSOR(return_threshold, LOWER_E, e, float)
  APC_UPS_SENSOR(state_of_charge, LOWER_F, f, float)
  APC_UPS_SENSOR(nominal_battery_voltage, LOWER_G, g, float)
  APC_UPS_SENSOR(estimated_runtime, LOWER_J, j, float)
  APC_UPS_SENSOR(low_transfer_voltage, LOWER_L, l, float)
  APC_UPS_SENSOR(nominal_output_voltage, LOWER_O, o, float)
  APC_UPS_SENSOR(upper_transfer_voltage, LOWER_U, u, float)

  APC_UPS_VALUED_TEXT_SENSOR(old_firmware_version, V, V, std::string)
  APC_UPS_VALUED_TEXT_SENSOR(self_test_results, X, X, std::string)
  APC_UPS_VALUED_TEXT_SENSOR(protocol_info, LOWER_A, a, std::string)
  APC_UPS_VALUED_TEXT_SENSOR(firmware_revision, LOWER_B, b, std::string)
  APC_UPS_VALUED_TEXT_SENSOR(local_identifier, LOWER_C, c, std::string)
  APC_UPS_VALUED_TEXT_SENSOR(alarm_delay, LOWER_K, k , std::string)
  APC_UPS_VALUED_TEXT_SENSOR(manufacture_date, LOWER_M, m, std::string)
  APC_UPS_VALUED_TEXT_SENSOR(serial_number, LOWER_N, n, std::string)
  APC_UPS_SENSOR(ambient_temperature, LOWER_T, t, float)
  APC_UPS_VALUED_TEXT_SENSOR(measure_upc_firmware, LOWER_V, v , std::string)
  APC_UPS_VALUED_TEXT_SENSOR(last_battery_change_date, LOWER_X, x, std::string)
  APC_UPS_VALUED_TEXT_SENSOR(copyright_notice, LOWER_Y, Y, std::string)
  APC_UPS_VALUED_TEXT_SENSOR(line_quality, 9 , 9, std::string)
  APC_UPS_VALUED_TEXT_SENSOR(model_name, CTRL_A, CTRL_A, std::string)
  APC_UPS_TEXT_SENSOR(last_battery_change_new_date, DATE, DATE) //                              New battery change date before saving
  
  APC_UPS_VALUED_SWITCH(front_panel_test, A, A, bool)
  APC_UPS_VALUED_SWITCH(self_test, W, W, bool)
  APC_UPS_VALUED_SWITCH(start_runtime_calibration, D, D, bool)
  APC_UPS_VALUED_SWITCH(simulate_power_failure, U, U, bool)
  APC_UPS_VALUED_SWITCH(save_last_battery_change_date, SDATE, SDATE, bool) //                   Save last battery change date
  APC_UPS_VALUED_SWITCH(shutdown_with_grace_period, K, K, bool) //                              Shutdown with grace period
  APC_UPS_VALUED_SWITCH(soft_shutdown, S, S, bool) //                                           Soft shutdown
  APC_UPS_SWITCH(shutdown_immediately, Z, Z) //                                                 Shutdown immediately
  APC_UPS_VALUED_SWITCH(turn_on, CTRL_N, CTRL_N, bool) //                                       Turn ON

  void switch_command(const std::string &command);
  void setup() override;
  void loop() override;
  void dump_config() override;
  void update() override;
  
  void set_last_battery_change_new_date(std::string value); //                                  Set New battery change date before saving

 protected:
  static const size_t APC_UPS_READ_BUFFER_LENGTH = 110;  // maximum supported answer length
  static const size_t COMMAND_QUEUE_LENGTH = 10;
  static const size_t COMMAND_TIMEOUT = 1600; //                                                For send two bytes command (pause between commands for more than 1.5 seconds)
  uint32_t last_poll_ = 0;
  void publish_state_(binary_sensor::BinarySensor *binary_sensor, const bool &state);
  void publish_state_(sensor::Sensor *sensor, float value);
  void publish_state_(switch_::Switch *obj, const bool &state);
  void publish_state_(text_sensor::TextSensor *text_sensor, const std::string &state);
  bool check_bit_(uint8_t mask, uint8_t flag) { return (mask & flag) == flag; }
  void add_polling_command_(const char *command, ENUMPollingCommand polling_command);
  void empty_uart_buffer_();
  uint8_t check_incoming_crc_();
  uint8_t check_incoming_length_(uint8_t length);
  uint8_t send_next_command_();
  void send_next_poll_();
  void queue_command_(const char *command, uint8_t length);
  std::string command_queue_[COMMAND_QUEUE_LENGTH];
  uint8_t command_queue_position_ = 0;
  uint8_t read_buffer_[APC_UPS_READ_BUFFER_LENGTH];
  size_t read_pos_{0};

  uint8_t multibyte_command_ = 0; //                                                            State - Sending a multibyte command
  const char *command_ctrl_a = "\x01";
  const char *command_ctrl_n = "\x0E";

  uint32_t command_start_millis_ = 0;
  uint8_t state_;
  enum State {
    STATE_IDLE = 0,
    STATE_POLL = 1,
    STATE_COMMAND = 2,
    STATE_POLL_COMPLETE = 3,
    STATE_COMMAND_COMPLETE = 4,
    STATE_POLL_CHECKED = 5,
    STATE_POLL_DECODED = 6,
  };

  uint8_t last_polling_command_ = 0;
  PollingCommand used_polling_commands_[42];
};

}  // namespace apc_ups
}  // namespace esphome
