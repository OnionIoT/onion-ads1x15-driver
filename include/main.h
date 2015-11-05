#ifndef _ONION_ADS1X15_DRIVER_MAIN_H_
#define _ONION_ADS1X15_DRIVER_MAIN_H_

#include <stdlib.h>

#include <onion-ads1x15-driver.h>

#define ADS1X15_MAIN_VERBOSITY_QUIET			(-1)
#define ADS1X15_MAIN_VERBOSITY_NORMAL			(0)
#define ADS1X15_MAIN_VERBOSITY_VERBOSE			(1)
#define ADS1X15_MAIN_VERBOSITY_EXTRA_VERBOSE	(2)

#define ADS1X15_MAIN_DEFAULT_VERBOSITY 			(ADS1X15_MAIN_VERBOSITY_NORMAL)
#define ADS1X15_MAIN_DEFAULT_DEBUG	 			(0)


struct adsSetup {
	int 	channel;
	int 	gain;

	// general options
	int 	verbose;
	int 	debug;
};


#endif // _ONION_ADS1X15_DRIVER_MAIN_H_