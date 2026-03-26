import esphome.codegen as cg
from esphome.components import switch
import esphome.config_validation as cv
from esphome.const import CONF_BEEPER, ENTITY_CATEGORY_CONFIG, ICON_POWER

from .. import APC_UPS_COMPONENT_SCHEMA, CONF_APC_UPS_ID, apc_ups_ns

DEPENDENCIES = ["uart"]

# CONF_BEEPER = "beeper"
CONF_QUICK_TEST = "quick_test"
CONF_DEEP_TEST = "deep_test"
CONF_TEN_MINUTES_TEST = "ten_minutes_test"

CONF_FRONT_PANEL_TEST = "front_panel_test"
CONF_SELF_TEST = "self_test"
CONF_START_RUNTIME_CALIBRATION = "start_runtime_calibration"
CONF_SIMULATE_POWER_FAILURE = "simulate_power_failure"

TYPES = {
    CONF_BEEPER: ("Q", "Q", ENTITY_CATEGORY_CONFIG),
    CONF_QUICK_TEST: ("T", "CT", None),
    CONF_DEEP_TEST: ("TL", "CT", None),
    CONF_TEN_MINUTES_TEST: ("T10", "CT", None),
    CONF_FRONT_PANEL_TEST: ("A", None, None),
    CONF_SELF_TEST: ("W", None, None),
    CONF_START_RUNTIME_CALIBRATION: ("D", "D", None),
    CONF_SIMULATE_POWER_FAILURE: ("U", None, None),
}

ApcUpsSwitch = apc_ups_ns.class_("ApcUpsSwitch", switch.Switch, cg.Component)

CONFIG_SCHEMA = APC_UPS_COMPONENT_SCHEMA.extend(
    {
        cv.Optional(type): switch.switch_schema(
            ApcUpsSwitch,
            icon=ICON_POWER,
            block_inverted=True,
            **({} if cat is None else {"entity_category": cat}),
        ).extend(cv.COMPONENT_SCHEMA)
        for type, (_, _, cat) in TYPES.items()
    }
)


async def to_code(config):
    paren = await cg.get_variable(config[CONF_APC_UPS_ID])

    for type, (on, off, _) in TYPES.items():
        if type in config:
            conf = config[type]
            var = await switch.new_switch(conf)
            await cg.register_component(var, conf)
            cg.add(getattr(paren, f"set_{type}")(var))
            cg.add(var.set_parent(paren))
            cg.add(var.set_on_command(on))
            if off is not None:
                cg.add(var.set_off_command(off))
