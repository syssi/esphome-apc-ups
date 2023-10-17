import esphome.codegen as cg
from esphome.components import text_sensor
import esphome.config_validation as cv

from .. import APC_UPS_COMPONENT_SCHEMA, CONF_APC_UPS_ID

DEPENDENCIES = ["uart"]

CONF_CAUSE_OF_LAST_TRANSFER = "cause_of_last_transfer"
CONF_PROTOCOL_INFO = "protocol_info"
CONF_FIRMWARE_REVISION = "firmware_revision"
CONF_MANUFACTURE_DATE = "manufacture_date"
CONF_LAST_BATTERY_CHANGE_DATE = "last_battery_change_date"

TYPES = [
    CONF_CAUSE_OF_LAST_TRANSFER,
    CONF_PROTOCOL_INFO,
    CONF_FIRMWARE_REVISION,
    CONF_MANUFACTURE_DATE,
    CONF_LAST_BATTERY_CHANGE_DATE,
]

CONFIG_SCHEMA = APC_UPS_COMPONENT_SCHEMA.extend(
    {cv.Optional(type): text_sensor.text_sensor_schema() for type in TYPES}
)


async def to_code(config):
    paren = await cg.get_variable(config[CONF_APC_UPS_ID])

    for type in TYPES:
        if type in config:
            conf = config[type]
            var = await text_sensor.new_text_sensor(conf)
            cg.add(getattr(paren, f"set_{type}")(var))
