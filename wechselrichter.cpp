#include "global.h"

bool Inverter::isBatteryOn() {
    return digitalRead(RELAY_PIN);
}
