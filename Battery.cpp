#include "global.h"

bool Battery::isBatteryOn() {
    return digitalRead(vars.RELAY_PIN);
}
