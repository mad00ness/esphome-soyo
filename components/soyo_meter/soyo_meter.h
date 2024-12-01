#pragma once

#include "esphome/core/component.h"
#include "esphome/core/automation.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/uart/uart.h"

namespace esphome
{
    namespace soyo_meter
    {
        class SoyoMeterUart : public PollingComponent, public uart::UARTDevice
        {
        public:
            float get_setup_priority() const override;

            void setup() override;
            void update() override;
            void dump_config() override;

            void set_power_sensor(sensor::Sensor *power_sensor) { power_sensor_ = power_sensor; }

        protected:
            sensor::Sensor *power_sensor_{nullptr};
        };
    }  // namespace soyo_meter_serial
}  // namespace esphome
