#include "soyo_meter.h"
#include "esphome/core/log.h"

#include <cinttypes>

namespace esphome
{
    namespace soyo_meter
    {

        static const char *const TAG = "soyo_meter";
        static const uint8_t SM_RESPONSE_LENGTH = 8;

        uint8_t soyo_meter_checksum(const uint8_t *data)
        {
            uint8_t sum = 0;
            for (uint8_t i = 1; i < SM_RESPONSE_LENGTH; i++) sum += data[i];
            return 0xFF - sum;
        }

        bool soyo_meter_preamble_check(const uint8_t *data)
        {
            return (data[0] != 0x36 && data[1] != 0x86);
        }

        void SoyoMeterUart::setup()
        {

        }

        void SoyoMeterUart::update()
        {
            uint8_t response[SM_RESPONSE_LENGTH];

            if (this->read_array(response, SM_RESPONSE_LENGTH))
            {
                if (soyo_meter_preamble_check(response))
                {
                    ESP_LOGW(TAG, "Invalid preamble from SOYO Meter!");
                    this->status_set_warning();
                    // to do find first preamble for data normal read
                    return;
                }
				
				uint8_t& rchksum = response[7];
                uint8_t checksum = soyo_meter_checksum(response);
                if (rchksum != checksum)
                {
                    ESP_LOGW(TAG, "SOYO Meter Checksum doesn't match: 0x%02X!=0x%02X", rchksum, checksum);
                    this->status_set_warning();
                    return;
                }

                const uint16_t power = (uint16_t(response[4]) << 8) | response[5];

                ESP_LOGD(TAG, "SOYO Meter Power=%uWatt", power);
                if (this->power_sensor_ != nullptr)
                {
                    this->power_sensor_->publish_state(power);
                    this->status_clear_warning();
                }
            }
        }

        float SoyoMeterUart::get_setup_priority() const
        {
            return setup_priority::DATA;
        }

        void SoyoMeterUart::dump_config()
        {
            ESP_LOGCONFIG(TAG, "SOYO Meter:");
            LOG_SENSOR("  ", "Power", this->power_sensor_);
            this->check_uart_settings(4800);
        }

    }  // namespace soyo_meter_serial
}  // namespace esphome
