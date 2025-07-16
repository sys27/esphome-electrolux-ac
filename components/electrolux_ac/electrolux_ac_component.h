#pragma once

#include "esphome/components/climate_ir/climate_ir.h"
#include "esphome/components/remote_base/remote_base.h"

namespace esphome
{
    namespace electrolux_ac
    {
        const float ELECTROLUX_TEMP_MAX = 32.0;
        const float ELECTROLUX_TEMP_MIN = 16.0;
        const float ELECTROLUX_TEMP_STEP = 1.0;
        const bool ELECTROLUX_SUPPORTS_DRY = true;
        const bool ELECTROLUX_SUPPORTS_FAN_ONLY = true;

        class ElectroluxClimate : public climate_ir::ClimateIR
        {
        public:
            ElectroluxClimate()
                : climate_ir::ClimateIR(
                      ELECTROLUX_TEMP_MIN,
                      ELECTROLUX_TEMP_MAX,
                      ELECTROLUX_TEMP_STEP,
                      ELECTROLUX_SUPPORTS_DRY,
                      ELECTROLUX_SUPPORTS_FAN_ONLY,
                      {climate::CLIMATE_FAN_AUTO, climate::CLIMATE_FAN_LOW, climate::CLIMATE_FAN_MEDIUM, climate::CLIMATE_FAN_HIGH},
                      {climate::CLIMATE_SWING_OFF, climate::CLIMATE_SWING_VERTICAL}) {}

        protected:
            void transmit_state() override;

        private:
            void setConstHeader(uint8_t *arr) const;
            void setSwingMode(uint8_t *arr) const;
            void setTemp(uint8_t *arr) const;
            void setFanSpeed(uint8_t *arr);
            void setMode(uint8_t *arr) const;
            void setState(uint8_t *arr) const;
            void setChecksum(uint8_t *arr) const;
            void logPacket(uint8_t *arr) const;
            void writePacketToData(esphome::remote_base::RemoteTransmitData *data, uint8_t *arr) const;
        };
    }
}