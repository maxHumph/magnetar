#include <core/asserts.h>
#include <core/log.h>
#include <platform/platform.h>

int main(void) {
  MFATAL("Message: %i", 42);
  MERROR("Message: %i", 42);
  MWARN("Message: %i", 42);
  MINFO("Message: %i", 42);
  MDEBUG("Message: %i", 42);
  MTRACE("Message: %i", 42);

  /* MASSERT_MSG(4 + 4 == 9, "4 + 4 does not equal 9"); */

  PlatformState state;

  if (platform_startup(&state, "Magnetar Test", 100, 100, 800, 800)) {
    while (TRUE) {
      platform_pump_messages(&state);
    }
  }

  return 0;
}
