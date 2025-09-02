#include "electrolux_ac_component.h"
#include "esphome/core/log.h"

namespace esphome
{
    namespace electrolux_ac
    {
        static const char *const TAG = "electrolux.climate";

        const uint8_t PACKET_SIZE = 13;
        const uint16_t FREQUENCY = 38000;
        const uint16_t HEADER_MARK = 8950;
        const uint16_t HEADER_SPACE = 4530;
        const uint16_t BIT_MARK = 563;
        const uint16_t ONE_SPACE = 1690;
        const uint16_t ZERO_SPACE = 538;
        const uint16_t FOOTER_SPACE = 10000;

        void ElectroluxClimate::transmit_state()
        {
            auto transmit = this->transmitter_->transmit();
            auto *data = transmit.get_data();

            data->set_carrier_frequency(FREQUENCY);

            data->item(HEADER_MARK, HEADER_SPACE);

            uint8_t arr[PACKET_SIZE] = {0};
            setConstHeader(arr);
            setSwingMode(arr);
            setTemp(arr);
            setFanSpeed(arr);
            setMode(arr);
            setState(arr);
            setChecksum(arr);

            logPacket(arr);
            writePacketToData(data, arr);

            data->item(BIT_MARK, FOOTER_SPACE);

            transmit.perform();
        }

        void ElectroluxClimate::setConstHeader(uint8_t *arr) const
        {
            arr[0] = 0b11000011;
        }

        void ElectroluxClimate::setSwingMode(uint8_t *arr) const
        {
            if (this->swing_mode != climate::CLIMATE_SWING_VERTICAL)
                arr[1] = 0b11100000;
            else
                arr[1] = 0b00000000;
        }

        void ElectroluxClimate::setTemp(uint8_t *arr) const
        {
            auto temp = static_cast<uint8_t>(
                std::clamp(this->target_temperature, ELECTROLUX_TEMP_MIN, ELECTROLUX_TEMP_MAX) - 8);
            temp = reverse_bits(temp) >> 3;

            arr[1] |= temp;
        }

        void ElectroluxClimate::setFanSpeed(uint8_t *arr)
        {
            auto fanSpeed = this->fan_mode.value_or(climate::CLIMATE_FAN_LOW);

            // Auto fan is not supported in Dry and Fan modes, set to Low instead
            if (fanSpeed == climate::CLIMATE_FAN_AUTO &&
                (this->mode == climate::CLIMATE_MODE_DRY ||
                 this->mode == climate::CLIMATE_MODE_FAN_ONLY))
            {
                fanSpeed = climate::CLIMATE_FAN_LOW;
                this->set_fan_mode_(fanSpeed);
            }

            // In Dry mode, fan speed is always Low
            if (this->mode == climate::CLIMATE_MODE_DRY)
            {
                fanSpeed = climate::CLIMATE_FAN_LOW;
                this->set_fan_mode_(fanSpeed);
            }

            uint8_t fanByte = 0;
            switch (fanSpeed)
            {
            case climate::CLIMATE_FAN_AUTO:
                fanByte = 0b00000101;
                break;
            case climate::CLIMATE_FAN_LOW:
                fanByte = 0b00000110;
                break;
            case climate::CLIMATE_FAN_MEDIUM:
                fanByte = 0b00000010;
                break;
            case climate::CLIMATE_FAN_HIGH:
                fanByte = 0b00000100;
                break;
            }

            arr[4] = fanByte;
        }

        void ElectroluxClimate::setMode(uint8_t *arr) const
        {

            if (this->mode == climate::CLIMATE_MODE_OFF)
                return;

            uint8_t modeByte = 0;
            switch (this->mode)
            {
            case climate::CLIMATE_MODE_HEAT_COOL:
                modeByte = 0b00000000;
                break;
            case climate::CLIMATE_MODE_COOL:
                modeByte = 0b00000100;
                break;
            case climate::CLIMATE_MODE_HEAT:
                modeByte = 0b00000001;
                break;
            case climate::CLIMATE_MODE_FAN_ONLY:
                modeByte = 0b00000011;
                break;
            case climate::CLIMATE_MODE_DRY:
                modeByte = 0b00000010;
                break;
            }

            arr[6] = modeByte;
        }

        void ElectroluxClimate::setState(uint8_t *arr) const
        {
            if (this->mode != climate::CLIMATE_MODE_OFF)
                arr[9] = 0b00000100;
            else
                arr[9] = 0b00000000;
        }

        void ElectroluxClimate::setChecksum(uint8_t *arr) const
        {
            uint8_t checksum = 0;
            for (auto i = 0; i < PACKET_SIZE - 1; ++i)
                checksum += reverse_bits(arr[i]);

            arr[12] = reverse_bits(checksum);
        }

        void ElectroluxClimate::logPacket(uint8_t *arr) const
        {
            for (auto i = 0; i < PACKET_SIZE; i++)
                ESP_LOGD(TAG, "Sending: %02X.", arr[i]);
        }

        void ElectroluxClimate::writePacketToData(esphome::remote_base::RemoteTransmitData *data, uint8_t *arr) const
        {
            for (auto i = 0; i < PACKET_SIZE; i++)
            {
                auto byte = arr[i];

                for (auto i = 7; i >= 0; --i)
                {
                    data->mark(BIT_MARK);

                    if (byte & (1 << i))
                        data->space(ONE_SPACE);
                    else
                        data->space(ZERO_SPACE);
                }
            }
        }
    }
}