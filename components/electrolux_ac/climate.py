import esphome.codegen as cg
from esphome.components import climate_ir

AUTO_LOAD = ["climate_ir"]
CODEOWNERS = ["@sys27"]

electrolux_ac = cg.esphome_ns.namespace("electrolux_ac")
ElectroluxClimate = electrolux_ac.class_("ElectroluxClimate", climate_ir.ClimateIR)

CONFIG_SCHEMA = climate_ir.climate_ir_with_receiver_schema(ElectroluxClimate)


async def to_code(config):
    await climate_ir.new_climate_ir(config)