import esphome.codegen as cg
from esphome.components import sensor
import esphome.config_validation as cv
from esphome.const import (
    CONF_BATTERY_VOLTAGE,
    DEVICE_CLASS_EMPTY,
    DEVICE_CLASS_VOLTAGE,
    ICON_CURRENT_AC,
    ICON_TIMELAPSE,
    STATE_CLASS_MEASUREMENT,
    STATE_CLASS_TOTAL_INCREASING,
    UNIT_EMPTY,
    UNIT_HERTZ,
    UNIT_MINUTE,
    UNIT_PERCENT,
    UNIT_VOLT,
)

from .. import APC_UPS_COMPONENT_SCHEMA, CONF_APC_UPS_ID

DEPENDENCIES = ["uart"]

# CONF_BATTERY_VOLTAGE = "battery_voltage"
CONF_GRID_FREQUENCY = "grid_frequency"
CONF_GRID_VOLTAGE = "grid_voltage"
CONF_AC_OUTPUT_VOLTAGE = "ac_output_voltage"
CONF_AC_OUTPUT_LOAD = "ac_output_load"
CONF_STATUS_BITMASK = "status_bitmask"
CONF_STATE_OF_CHARGE = "state_of_charge"
CONF_ESTIMATED_RUNTIME = "estimated_runtime"

ICON_STATE_OF_CHARGE = "mdi:battery-50"
ICON_OPERATION_STATUS_BITMASK = "mdi:heart-pulse"

TYPES = {
    CONF_BATTERY_VOLTAGE: sensor.sensor_schema(
        unit_of_measurement=UNIT_VOLT,
        accuracy_decimals=2,
        device_class=DEVICE_CLASS_VOLTAGE,
    ),
    CONF_GRID_FREQUENCY: sensor.sensor_schema(
        unit_of_measurement=UNIT_HERTZ,
        icon=ICON_CURRENT_AC,
        accuracy_decimals=2,
    ),
    CONF_GRID_VOLTAGE: sensor.sensor_schema(
        unit_of_measurement=UNIT_VOLT,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_VOLTAGE,
    ),
    CONF_AC_OUTPUT_VOLTAGE: sensor.sensor_schema(
        unit_of_measurement=UNIT_VOLT,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_VOLTAGE,
    ),
    CONF_AC_OUTPUT_LOAD: sensor.sensor_schema(
        unit_of_measurement=UNIT_PERCENT,
        accuracy_decimals=1,
    ),
    cv.Optional(CONF_STATUS_BITMASK): sensor.sensor_schema(
        unit_of_measurement=UNIT_EMPTY,
        icon=ICON_OPERATION_STATUS_BITMASK,
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_EMPTY,
        state_class=STATE_CLASS_MEASUREMENT,
    ),
    cv.Optional(CONF_STATE_OF_CHARGE): sensor.sensor_schema(
        unit_of_measurement=UNIT_PERCENT,
        icon=ICON_STATE_OF_CHARGE,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_EMPTY,
        state_class=STATE_CLASS_MEASUREMENT,
    ),
    cv.Optional(CONF_ESTIMATED_RUNTIME): sensor.sensor_schema(
        unit_of_measurement=UNIT_MINUTE,
        icon=ICON_TIMELAPSE,
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_EMPTY,
        state_class=STATE_CLASS_TOTAL_INCREASING,
    ),
    # CONF_TEMPERATURE: sensor.sensor_schema(
    #     unit_of_measurement=UNIT_CELSIUS,
    #     accuracy_decimals=1,
    #     device_class=DEVICE_CLASS_TEMPERATURE,
    # ),
    # CONF_AC_OUTPUT_RATING_VOLTAGE: sensor.sensor_schema(
    #     unit_of_measurement=UNIT_VOLT,
    #     accuracy_decimals=1,
    #     device_class=DEVICE_CLASS_VOLTAGE,
    # ),
    # CONF_BATTERY_RATING_VOLTAGE: sensor.sensor_schema(
    #     unit_of_measurement=UNIT_VOLT,
    #     accuracy_decimals=1,
    #     device_class=DEVICE_CLASS_VOLTAGE,
    # ),
    # CONF_AC_OUTPUT_RATING_FREQUENCY: sensor.sensor_schema(
    #     unit_of_measurement=UNIT_HERTZ,
    #     icon=ICON_CURRENT_AC,
    #     accuracy_decimals=1,
    # ),
}

CONFIG_SCHEMA = APC_UPS_COMPONENT_SCHEMA.extend(
    {cv.Optional(type): schema for type, schema in TYPES.items()}
)


async def to_code(config):
    paren = await cg.get_variable(config[CONF_APC_UPS_ID])

    for type, _ in TYPES.items():
        if type in config:
            conf = config[type]
            sens = await sensor.new_sensor(conf)
            cg.add(getattr(paren, f"set_{type}")(sens))
