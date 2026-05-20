#include <gtest/gtest.h>
#include <sstream>
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "common.h"

namespace esphome::apc_ups::testing {

// Responses sourced from tests/esp8266-apc-su420inet-emulator.yaml

class ApcUpsDecodeTest : public ::testing::Test {
 protected:
  TestableApcUps ups_;
  sensor::Sensor battery_voltage_;
  sensor::Sensor grid_frequency_;
  sensor::Sensor grid_voltage_;
  sensor::Sensor max_grid_voltage_;
  sensor::Sensor min_grid_voltage_;
  sensor::Sensor ac_output_voltage_;
  sensor::Sensor ac_output_load_;
  sensor::Sensor status_bitmask_;
  sensor::Sensor state_of_charge_;
  sensor::Sensor nominal_battery_voltage_;
  sensor::Sensor estimated_runtime_;
  sensor::Sensor low_transfer_voltage_;
  sensor::Sensor nominal_output_voltage_;
  sensor::Sensor upper_transfer_voltage_;
  binary_sensor::BinarySensor smart_mode_;
  binary_sensor::BinarySensor runtime_calibration_;
  binary_sensor::BinarySensor smart_trim_;
  binary_sensor::BinarySensor smart_boost_;
  binary_sensor::BinarySensor on_line_;
  binary_sensor::BinarySensor on_battery_;
  binary_sensor::BinarySensor replace_battery_;
  text_sensor::TextSensor status_;
  text_sensor::TextSensor protocol_info_;
  text_sensor::TextSensor firmware_revision_;
  text_sensor::TextSensor local_identifier_;
  text_sensor::TextSensor manufacture_date_;

  void SetUp() override {
    ups_.set_battery_voltage(&battery_voltage_);
    ups_.set_grid_frequency(&grid_frequency_);
    ups_.set_grid_voltage(&grid_voltage_);
    ups_.set_max_grid_voltage(&max_grid_voltage_);
    ups_.set_min_grid_voltage(&min_grid_voltage_);
    ups_.set_ac_output_voltage(&ac_output_voltage_);
    ups_.set_ac_output_load(&ac_output_load_);
    ups_.set_status_bitmask(&status_bitmask_);
    ups_.set_state_of_charge(&state_of_charge_);
    ups_.set_nominal_battery_voltage(&nominal_battery_voltage_);
    ups_.set_estimated_runtime(&estimated_runtime_);
    ups_.set_low_transfer_voltage(&low_transfer_voltage_);
    ups_.set_nominal_output_voltage(&nominal_output_voltage_);
    ups_.set_upper_transfer_voltage(&upper_transfer_voltage_);
    ups_.set_smart_mode(&smart_mode_);
    ups_.set_runtime_calibration(&runtime_calibration_);
    ups_.set_smart_trim(&smart_trim_);
    ups_.set_smart_boost(&smart_boost_);
    ups_.set_on_line(&on_line_);
    ups_.set_on_battery(&on_battery_);
    ups_.set_replace_battery(&replace_battery_);
    ups_.set_status(&status_);
    ups_.set_protocol_info(&protocol_info_);
    ups_.set_firmware_revision(&firmware_revision_);
    ups_.set_local_identifier(&local_identifier_);
    ups_.set_manufacture_date(&manufacture_date_);
  }
};

TEST_F(ApcUpsDecodeTest, BatteryVoltage) {
  ups_.decode_and_publish(POLLING_B, "13.61\r");
  EXPECT_NEAR(battery_voltage_.state, 13.61f, 0.005f);
}

TEST_F(ApcUpsDecodeTest, GridFrequency) {
  ups_.decode_and_publish(POLLING_F, "50.00\r");
  EXPECT_NEAR(grid_frequency_.state, 50.0f, 0.005f);
}

TEST_F(ApcUpsDecodeTest, GridVoltage) {
  ups_.decode_and_publish(POLLING_L, "231.8\r");
  EXPECT_NEAR(grid_voltage_.state, 231.8f, 0.05f);
}

TEST_F(ApcUpsDecodeTest, MaxGridVoltage) {
  ups_.decode_and_publish(POLLING_M, "233.2\r");
  EXPECT_NEAR(max_grid_voltage_.state, 233.2f, 0.05f);
}

TEST_F(ApcUpsDecodeTest, MinGridVoltage) {
  ups_.decode_and_publish(POLLING_N, "231.8\r");
  EXPECT_NEAR(min_grid_voltage_.state, 231.8f, 0.05f);
}

TEST_F(ApcUpsDecodeTest, AcOutputVoltage) {
  ups_.decode_and_publish(POLLING_O, "231.8\r");
  EXPECT_NEAR(ac_output_voltage_.state, 231.8f, 0.05f);
}

TEST_F(ApcUpsDecodeTest, AcOutputLoad) {
  ups_.decode_and_publish(POLLING_P, "009.1\r");
  EXPECT_NEAR(ac_output_load_.state, 9.1f, 0.05f);
}

TEST_F(ApcUpsDecodeTest, StatusBitmaskHex) {
  ups_.decode_and_publish(POLLING_Q, "8F\r");
  EXPECT_FLOAT_EQ(status_bitmask_.state, 0x8F);
}

TEST_F(ApcUpsDecodeTest, StatusBitmaskOnLine) {
  ups_.decode_and_publish(POLLING_Q, "8F\r");
  EXPECT_TRUE(on_line_.state);
}

TEST_F(ApcUpsDecodeTest, StatusBitmaskOnBatteryFalse) {
  ups_.decode_and_publish(POLLING_Q, "8F\r");
  EXPECT_FALSE(on_battery_.state);
}

TEST_F(ApcUpsDecodeTest, StatusTextOnline) {
  ups_.decode_and_publish(POLLING_Q, "08\r");
  EXPECT_EQ(status_.state, "Online");
}

TEST_F(ApcUpsDecodeTest, StatusTextOnBattery) {
  ups_.decode_and_publish(POLLING_Q, "10\r");
  EXPECT_EQ(status_.state, "On Battery");
}

TEST_F(ApcUpsDecodeTest, StatusBitmaskReplaceBattery) {
  ups_.decode_and_publish(POLLING_Q, "8F\r");
  EXPECT_TRUE(replace_battery_.state);
}

TEST_F(ApcUpsDecodeTest, SmartModeTrue) {
  ups_.decode_and_publish(POLLING_Y, "SM\r");
  EXPECT_TRUE(smart_mode_.state);
}

TEST_F(ApcUpsDecodeTest, StateOfCharge) {
  ups_.decode_and_publish(POLLING_LOWER_F, "100.0\r");
  EXPECT_NEAR(state_of_charge_.state, 100.0f, 0.05f);
}

TEST_F(ApcUpsDecodeTest, NominalBatteryVoltage) {
  ups_.decode_and_publish(POLLING_LOWER_G, "012\r");
  EXPECT_NEAR(nominal_battery_voltage_.state, 12.0f, 0.05f);
}

TEST_F(ApcUpsDecodeTest, EstimatedRuntime) {
  ups_.decode_and_publish(POLLING_LOWER_J, "0042:\r");
  EXPECT_NEAR(estimated_runtime_.state, 42.0f, 0.05f);
}

TEST_F(ApcUpsDecodeTest, LowTransferVoltage) {
  ups_.decode_and_publish(POLLING_LOWER_L, "208\r");
  EXPECT_NEAR(low_transfer_voltage_.state, 208.0f, 0.5f);
}

TEST_F(ApcUpsDecodeTest, NominalOutputVoltage) {
  ups_.decode_and_publish(POLLING_LOWER_O, "230\r");
  EXPECT_NEAR(nominal_output_voltage_.state, 230.0f, 0.5f);
}

TEST_F(ApcUpsDecodeTest, UpperTransferVoltage) {
  ups_.decode_and_publish(POLLING_LOWER_U, "253\r");
  EXPECT_NEAR(upper_transfer_voltage_.state, 253.0f, 0.5f);
}

TEST_F(ApcUpsDecodeTest, FirmwareRevision) {
  ups_.decode_and_publish(POLLING_LOWER_B, "21.3.I\r");
  EXPECT_EQ(firmware_revision_.state.substr(0, 6), "21.3.I");
}

TEST_F(ApcUpsDecodeTest, LocalIdentifier) {
  ups_.decode_and_publish(POLLING_LOWER_C, "UPS_IDEN\r");
  EXPECT_EQ(local_identifier_.state.substr(0, 8), "UPS_IDEN");
}

TEST_F(ApcUpsDecodeTest, ManufactureDate) {
  ups_.decode_and_publish(POLLING_LOWER_M, "11/17/98\r");
  EXPECT_EQ(manufacture_date_.state.substr(0, 8), "11/17/98");
}

TEST_F(ApcUpsDecodeTest, StringSanitizesAllBytes) {
  for (int b = 0x00; b <= 0xFF; b++) {
    if (b == 0x0D)
      continue;  // \r is the response terminator, not part of the value
    uint8_t buf[2] = {(uint8_t) b, 0x0D};
    ups_.decode_and_publish_bytes(POLLING_LOWER_A, buf, sizeof(buf));
    std::ostringstream ctx;
    ctx << "byte 0x" << std::hex << b;
    if (b == 0x00) {
      EXPECT_EQ(protocol_info_.state, "") << ctx.str() << " (null) should yield empty string";
    } else if (b >= 0x20 && b <= 0x7E) {
      EXPECT_EQ(protocol_info_.state, std::string(1, (char) b)) << ctx.str() << " (printable) should pass unchanged";
    } else {
      EXPECT_EQ(protocol_info_.state, "?") << ctx.str() << " (non-printable) should become '?'";
    }
  }
}

}  // namespace esphome::apc_ups::testing
