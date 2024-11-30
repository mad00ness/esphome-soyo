import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart
from esphome.const import CONF_ID, CONF_BAUD_RATE

DEPENDENCIES = ['uart']

ns = cg.esphome_ns.namespace('soyo_meter')
SMUComponent = ns.class_('SMUComponent', cg.Component, uart.UARTDevice)

uart_ns = cg.esphome_ns.namespace("uart")

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(SMUComponent),
}).extend(cv.COMPONENT_SCHEMA).extend(uart.UART_DEVICE_SCHEMA)

def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    yield cg.register_component(var, config)

    yield uart.register_uart_device(var, config)