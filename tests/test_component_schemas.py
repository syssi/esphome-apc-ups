"""Schema structure tests for apc_ups ESPHome component modules."""

import os
import sys

sys.path.insert(0, os.path.join(os.path.dirname(__file__), ".."))

import components.apc_ups as hub  # noqa: E402
from components.apc_ups import binary_sensor, sensor, switch, text_sensor  # noqa: E402


class TestHubConstants:
    def test_conf_id_defined(self):
        assert hub.CONF_APC_UPS_ID == "apc_ups_id"


class TestBinarySensorTypes:
    def test_types_list_completeness(self):
        assert binary_sensor.CONF_ON_LINE in binary_sensor.TYPES
        assert binary_sensor.CONF_ON_BATTERY in binary_sensor.TYPES
        assert binary_sensor.CONF_BATTERY_LOW in binary_sensor.TYPES
        assert binary_sensor.CONF_REPLACE_BATTERY in binary_sensor.TYPES
        assert len(binary_sensor.TYPES) == 9

    def test_type_values_are_strings(self):
        for t in binary_sensor.TYPES:
            assert isinstance(t, str)


class TestSensorTypes:
    def test_types_dict_completeness(self):
        assert sensor.CONF_GRID_VOLTAGE in sensor.TYPES
        assert sensor.CONF_AC_OUTPUT_VOLTAGE in sensor.TYPES
        assert sensor.CONF_STATE_OF_CHARGE in sensor.TYPES
        assert len(sensor.TYPES) == 18


class TestTextSensorTypes:
    def test_types_list_completeness(self):
        assert text_sensor.CONF_STATUS in text_sensor.TYPES
        assert text_sensor.CONF_MODEL_NAME in text_sensor.TYPES
        assert text_sensor.CONF_SERIAL_NUMBER in text_sensor.TYPES
        assert len(text_sensor.TYPES) == 15


class TestSwitchTypes:
    def test_types_dict_completeness(self):
        assert switch.CONF_QUICK_TEST in switch.TYPES
        assert switch.CONF_DEEP_TEST in switch.TYPES
        assert switch.CONF_SELF_TEST in switch.TYPES
        assert len(switch.TYPES) == 8
