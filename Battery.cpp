#include "global.h"

bool Battery::isBatteryOn() {
    return digitalRead(RELAY_PIN);
}
