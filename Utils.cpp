#include "global.h"

long Utils::secondsSince(long milliSeconds) {
  return (millis() - milliSeconds) / 1000;
}
