import esphome.codegen as cg
from esphome.components import uart
import esphome.config_validation as cv
from esphome.const import CONF_ID

DEPENDENCIES = ["uart"]
CODEOWNERS = ["@syssi"]
AUTO_LOAD = ["binary_sensor", "text_sensor", "sensor", "switch"]
MULTI_CONF = True

CONF_APC_UPS_ID = "apc_ups_id"

apc_ups_ns = cg.esphome_ns.namespace("apc_ups")
ApcUpsComponent = apc_ups_ns.class_("ApcUps", cg.Component)

APC_UPS_COMPONENT_SCHEMA = cv.Schema(
    {
        cv.Required(CONF_APC_UPS_ID): cv.use_id(ApcUpsComponent),
    }
)

CONFIG_SCHEMA = cv.All(
    cv.Schema({cv.GenerateID(): cv.declare_id(ApcUpsComponent)})
    .extend(cv.polling_component_schema("1s"))
    .extend(uart.UART_DEVICE_SCHEMA)
)


def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    yield cg.register_component(var, config)
    yield uart.register_uart_device(var, config)
