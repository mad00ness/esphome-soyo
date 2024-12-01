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
            ESP_LOGW(TAG, "SOYO Meter response:0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X",
			data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]);
			
			uint8_t sum = 0;
            for (uint8_t i = 1; i < SM_RESPONSE_LENGTH; i++)
			{
				sum += data[i];
			}
			
			ESP_LOGW(TAG, "SOYO Meter Checksum: 0x%02X", sum);
            return (0xFF - sum);
        }

        bool soyo_meter_preamble_check(const uint8_t *data)
        {
            return (data[0] != 0x24 && data[1] != 0x56);
        }
		
		void SoyoMeterUart::init_uart()
		{
			init_state = false;
			
			if (this->available() < 2 * SM_RESPONSE_LENGTH + 1) return;
			
			uint8_t init[2];
			this->read_array(init, 2);
			
			while (!init_state)
			{
				if (!soyo_meter_preamble_check(init))
				{
					init[0] = init[1];
					this->read_byte(&init[1]);
				}
				else
				{
					uint8_t part[5];
					this->read_array(part, 5);
					init_state = true;
				}
			}
		}

        void SoyoMeterUart::setup()
        {

        }

        void SoyoMeterUart::update()
        {
            if (!init_state)
			{
				this->status_set_warning();
				init_uart();
				return;
			}
			
			uint8_t response[SM_RESPONSE_LENGTH];

            while (this->read_array(response, SM_RESPONSE_LENGTH))
            {
                if (soyo_meter_preamble_check(response))
                {
                    ESP_LOGW(TAG, "Invalid preamble from SOYO Meter!");
					ESP_LOGW(TAG, "SOYO Meter response:0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X",
					response[0], response[1], response[2], response[3], response[4], response[5], response[6], response[7]);
                    this->status_set_warning();
                    init_uart();
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
