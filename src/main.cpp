#include <main.h>

void usage(const char* progName) {
	printf("Usage:\n");
	printf("\t%s [-qvd] <channel> [max voltage]\n", progName);
	printf("\n");
	printf("<channel>\tAnalog input channel, can be 0 to 3\n");
	printf("[max voltage]\tOptional: Set maximum expected input voltage\n");
	printf("\tmax:	 6.144 V\n");
	printf("\tmin: 	 0.256 V\n");
	printf("\tdefault: 6.144 V\n");
	printf("\n");
	printf("Options\n");
	printf("\t-q	Quiet: no output\n");
	printf("\t-v	Verbose: debug output\n");
	printf("\t-x	Extra Verbose: all debug output\n");
	printf("\t-d	Debug: do not carry out any I2C transactions\n");
	printf("\n");
}

/*
int parseArguments(const char* progName, int argc, char* argv[], gpioSetup *setup)
{
	// check for the correct number of arguments
	if ( argc < 2 ) 
	{
		usage(progName);
		return EXIT_FAILURE;
	}

	// get the channel number


	return EXIT_SUCCESS;
}*/

int main(int argc, char* argv[])
{
	int 		status;
	int 		value;

	const char 	*progname;
	int 		verbose, debug;
	int 		ch;

	int 		channel	= -1;
	int 		gain 	= -1;
	ads1X15*	adsObj	= new ads1X15;

	// save the program name
	progname 	= argv[0];	

	// set the defaults
	verbose 	= ADS1X15_MAIN_DEFAULT_VERBOSITY;
	debug 		= ADS1X15_MAIN_DEFAULT_DEBUG;

	//// parse the option arguments
	while ((ch = getopt(argc, argv, "xvqdh")) != -1) {
		switch (ch) {
		case 'x':
			// verbose output, including i2c lib
			verbose = ADS1X15_MAIN_VERBOSITY_EXTRA_VERBOSE;
			break;
		case 'v':
			// verbose output
			verbose = ADS1X15_MAIN_VERBOSITY_VERBOSE;
			break;
		case 'q':
			// quiet output
			verbose = ADS1X15_MAIN_VERBOSITY_QUIET;
			break;
		case 'd':
			// debug mode
			debug 	= 1;
			break;
		default:
			usage(progname);
			return 0;
		}
	}

	// advance past the option arguments
	argc 	-= optind;
	argv	+= optind;

	
	//// parse the arguments
	// channel argument
	if (argc > 0) {
		channel 	= atoi(argv[0]);

		if (channel < 0 || channel > ADS1X15_NUM_CHANNELS) {
			usage(progname);
			printf ("ERROR: invalid number of channels!\n");
			return EXIT_FAILURE;
		}
	}
	else {
		usage(progname);
		return EXIT_FAILURE;
	}

	// gain argument
	if (argc > 1) {
		gain 		= adsObj->ReadMaxVoltage( atof(argv[1]) );
		printf ("> Max input voltage %0.3f V corresponds to gain setting of %d\n", atof(argv[1]), gain);

		if (gain < 0 || gain > ADS1X15_NUM_GAIN) {
			usage(progname);
			printf ("ERROR: invalid max input voltage!\n");
			return EXIT_FAILURE;
		}
	} else {
            gain = 0;
        }


	//// actual program
	// setup the verbosity
	adsObj->SetVerbosity(verbose);
	adsObj->SetDebugMode(debug);

	// set the gain if specified
	//	otherwise, default will be used
	if (gain != -1) {
		status 	= adsObj->SetGain(gain);
		if (status != EXIT_SUCCESS) {
			printf("ERROR setting gain!\n");
			return EXIT_FAILURE;
		}
	}

	// run the ADC conversion
	status 	= adsObj->ReadAdc(channel, value);
	if (status == EXIT_SUCCESS) {
		printf("> ADS channel%d, value is %d\n", channel, value);
	}


	return 0;
}