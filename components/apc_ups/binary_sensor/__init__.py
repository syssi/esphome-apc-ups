import esphome.codegen as cg
from esphome.components import binary_sensor
import esphome.config_validation as cv

from .. import APC_UPS_COMPONENT_SCHEMA, CONF_APC_UPS_ID

DEPENDENCIES = ["uart"]

CONF_FAULTS_PRESENT = "faults_present"

CONF_SMART_MODE = "smart_mode"
CONF_RUNTIME_CALIBRATION = "runtime_calibration"
CONF_SMART_TRIM = "smart_trim"
CONF_SMART_BOOST = "smart_boost"
CONF_ON_LINE = "on_line"
CONF_ON_BATTERY = "on_battery"
CONF_OUTPUT_OVERLOADED = "output_overloaded"
CONF_BATTERY_LOW = "battery_low"
CONF_REPLACE_BATTERY = "replace_battery"

TYPES = [
    CONF_SMART_MODE,
    CONF_RUNTIME_CALIBRATION,
    CONF_SMART_TRIM,
    CONF_SMART_BOOST,
    CONF_ON_LINE,
    CONF_ON_BATTERY,
    CONF_OUTPUT_OVERLOADED,
    CONF_BATTERY_LOW,
    CONF_REPLACE_BATTERY,
]

CONFIG_SCHEMA = APC_UPS_COMPONENT_SCHEMA.extend(
    {cv.Optional(type): binary_sensor.binary_sensor_schema() for type in TYPES}
)


async def to_code(config):
    paren = await cg.get_variable(config[CONF_APC_UPS_ID])
    for type in TYPES:
        if type in config:
            conf = config[type]
            var = await binary_sensor.new_binary_sensor(conf)
            cg.add(getattr(paren, f"set_{type}")(var))
