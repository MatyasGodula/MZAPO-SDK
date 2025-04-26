#include "AudioDriver.hpp"

#include "mzapo_phys.h"
#include "mzapo_regs.h"

#include <cstdint>
#include <atomic>
#include <mutex>
#include <thread>
#include <expected>

AudioDriver::AudioDriver() {
    pwm_reg = map_phys_address(AUDIOPWM_REG_BASE_PHYS, AUDIOPWM_REG_SIZE, 0);
    
}