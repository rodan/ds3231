#ifndef __config_h_
  #define __config_h_

  // comment this out if you need unixtime support
  // this will add about 590 bytes to your firmware
  //#define CONFIG_UNIXTIME

  #ifdef CONFIG_UNIXTIME
    // comment this out if you need support time on unixtime < year 2000
    // you need to activate unixtime
    // this will make your firmware 166 bytes bigger
    #define CONFIG_UNIXTIME_FULL
  #endif

#endif
