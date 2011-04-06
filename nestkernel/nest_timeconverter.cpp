#include "nest.h"
#include "nest_time.h"
#include "nest_timeconverter.h"

namespace nest {

  TimeConverter::TimeConverter()
  {
    OLD_TICS_PER_STEP = Time::get_tics_per_step();
    OLD_TICS_PER_MS = Time::get_tics_per_ms();
  }

  Time TimeConverter::from_old_steps(long_t s_old) const
  {
    double ms = s_old * OLD_TICS_PER_STEP / OLD_TICS_PER_MS; 
    return Time::ms(ms);
  }

  Time TimeConverter::from_old_tics(tic_t t_old) const
  {
    double ms = t_old / OLD_TICS_PER_MS;
    return Time::ms(ms);
  }

}
