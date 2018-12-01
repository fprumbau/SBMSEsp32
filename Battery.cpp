#include "Battery.h"
#include "Vars.h"

Battery::Battery(Vars &vars): vars(vars) { }

bool Battery::isBatteryOn() {
    return digitalRead(vars.RELAY_PIN);
}
