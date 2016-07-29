#include <onion-ads1x15-driver.h>

ads1X15::ads1X15 (int addr, int type )
{
	Reset();

	// set up specifics
	devAddr 	= addr;

	if (type == ADS1X15_TYPE_ADS1015) {
		conversionDelay		= ADS1015_CONVERSION_DELAY;
		conversionBitShift	= ADS1015_CONVERSION_DATA_SHIFT;
	}
	else if (type == ADS1X15_TYPE_ADS1115) {
		conversionDelay		= ADS1115_CONVERSION_DELAY;
		conversionBitShift	= ADS1115_CONVERSION_DATA_SHIFT;
	}

	// change conversion delay to microseconds
	conversionDelayUs		= conversionDelay * 1000;
}

ads1X15::~ads1X15 (void)
{
	// nothing for now
}

void ads1X15::Reset (void)
{
	// set defaults
	devAddr				= ADS1X15_I2C_DEVICE_ADDR;

	conversionDelay 	= ADS1015_CONVERSION_DELAY;
	conversionBitShift	= ADS1015_CONVERSION_DATA_SHIFT;

	gainValue			= ADS1X15_REG_CONFIG_PGA_2_048V;
}

void ads1X15::SetVerbosity (int input)
{
	Module::SetVerbosity(input);

	// also set the debug lib verbosity
	onionSetVerbosity(input > 1 ? ONION_VERBOSITY_VERBOSE : ONION_VERBOSITY_NORMAL);
}


// private class function
int ads1X15::_ReadReg (int addr, uint8_t *buffer, int numBytes)
{
	int status;

	_Print(ADS1X15_SEVERITY_DEBUG, "%s Reading reg addr 0x%02x...", ADS1X15_PRINT_BANNER, addr);

	if (debugLevel == 0) {
		status = i2c_read	(	
							ADS1X15_I2C_DEVICE_NUM, 
							devAddr, 
							addr, 
							buffer, 
							numBytes
						);
	}
	else {
		status 		= EXIT_SUCCESS;;
		buffer[0] 	= 0xe1;
		buffer[1] 	= 0x90;
	}

	_Print(ADS1X15_SEVERITY_DEBUG, "\tRead value 0x%04x\n", (buffer[1] << 8 & buffer[0]) );

	return status;
}

int ads1X15::_WriteReg (int addr, int value, int numBytes)
{
	int status 		= EXIT_SUCCESS;

	_Print(ADS1X15_SEVERITY_DEBUG, "%s Writing Reg addr 0x%02x with 0x%04x\n", ADS1X15_PRINT_BANNER, addr, value);

	if (debugLevel == 0) {
		status = i2c_writeBytes	(	
							ADS1X15_I2C_DEVICE_NUM, 
							devAddr, 
							addr, 
							value, 
							numBytes
						);


	}
	else {
		status 	= EXIT_SUCCESS;
	}

	return status;
}

int ads1X15::_ChannelToInputMux (int channel, int &inputMuxSel)
{
	int status 	= EXIT_SUCCESS;

	switch(channel) {
		case ADS1X15_CHANNEL_A0:
			inputMuxSel 	= ADS1X15_INPUT_MUX_SINGLE_A0;
			break;

		case ADS1X15_CHANNEL_A1:
			inputMuxSel 	= ADS1X15_INPUT_MUX_SINGLE_A1;
			break;

		case ADS1X15_CHANNEL_A2:
			inputMuxSel 	= ADS1X15_INPUT_MUX_SINGLE_A2;
			break;

		case ADS1X15_CHANNEL_A3:
			inputMuxSel 	= ADS1X15_INPUT_MUX_SINGLE_A3;
			break;

		default:
			inputMuxSel 	= ADS1X15_INPUT_MUX_SINGLE_A0;
			status 			= EXIT_FAILURE;
			break;
	}

	return status;
}

// read the conversion register and return the value
int ads1X15::_ReadConverson (int &value)
{
	int 	status;
	int 	result;
	uint8_t	*buffer = new uint8_t[I2C_BUFFER_SIZE];

	// read the conversion results
	status = _ReadReg	(	ADS1X15_REG_ADDR_CONVERT, 
							buffer, 
							2
						);
	if (status != EXIT_SUCCESS) {
		_Print(ADS1X15_SEVERITY_FATAL, " ERROR: Reading Conversion Register failed!\n");
		return status;
	}

	// reconstruct the value from the buffer
	result 	= 0;
	result 	= ( ((buffer[0] & 0xff) << 8) | (buffer[1] & 0xff) );
	_Print(ADS1X15_SEVERITY_DEBUG_EXTRA, "%s result = 0x%04x\n", ADS1X15_PRINT_BANNER, result);

	// shift the flipped result
	value 	= result >> conversionBitShift;
	_Print(ADS1X15_SEVERITY_DEBUG_EXTRA, "%s shifted = 0x%04x, value = %d\n", ADS1X15_PRINT_BANNER, value, value);

	// keep sign bit intact if shifting
	// REQUIRES DBG OF SIGNED/UNSIGNED
	/*if (conversionBitShift > 0) {
		if (value > 0x07ff) {
			value |= 0xf000;
			_Print(ADS1X15_SEVERITY_DEBUG, "%s sign-extend = 0x%04x, value = %d\n", ADS1X15_PRINT_BANNER, value, (unsigned int)value);
		}
	}*/

	return status;
}


//// class functions
int ads1X15::SetGain (int gain)
{
	gainValue	= gain;

	return EXIT_SUCCESS;
}

// convert max input voltage into gain setting
int ads1X15::ReadMaxVoltage (float maxVoltage) 
{
	int 	index	= -1;

	// define the voltage levels
	float 	voltages[ADS1X15_NUM_GAIN]	= {
		(float)ADS1X15_MAX_VOLTAGE_6_144V / 1000.0f,
		(float)ADS1X15_MAX_VOLTAGE_4_096V / 1000.0f,
		(float)ADS1X15_MAX_VOLTAGE_2_048V / 1000.0f,
		(float)ADS1X15_MAX_VOLTAGE_1_024V / 1000.0f,
		(float)ADS1X15_MAX_VOLTAGE_0_512V / 1000.0f,
		(float)ADS1X15_MAX_VOLTAGE_0_256V / 1000.0f
	};

	// find the 
	while (maxVoltage < voltages[index+1]) {
		index++;
	}

	return index;
}

int ads1X15::ReadAdc (int channel, int &value)
{
	int 			status, channelVal;
	adsRegConfig_t 	configReg;
	uint8_t			*buffer = new uint8_t[I2C_BUFFER_SIZE];


	if (channel >= ADS1X15_NUM_CHANNELS) {
		return EXIT_FAILURE;
	}

	// read the current reg value
	status = _ReadReg	(	ADS1X15_REG_ADDR_CONFIG, 
							buffer, 
							2
						);
	if (status != EXIT_SUCCESS) {
		_Print(ADS1X15_SEVERITY_FATAL, " ERROR: Reading Configuration Register failed!\n");
		return status;
	}

	// reconstruct the register value
	configReg.val 					= ((buffer[1] & 0xff) << 8) & (buffer[0] & 0xff);

	// set up the config register for single-ended read
	configReg.f.comp_queue 			= ADS1X15_COMP_QUEUE_DISABLE_COMP;		// disable comparator
	configReg.f.comp_latch 			= ADS1X15_COMP_LATCH_NO_LATCH;			// non-latching
	configReg.f.comp_polarity 		= ADS1X15_COMP_POLARITY_ACTIVE_LOW;		// ALRT pin is active-low
	configReg.f.comp_mode 			= ADS1X15_COMP_MODE_TRADITIONAL;		// set comparator to traditional
	configReg.f.data_rate 			= ADS1X15_DATARATE_1600SPS;				// 1600 samples per second
	configReg.f.mode 	 			= ADS1X15_MODE_SINGLE_SHOT;				// single-shot mode

	// set PGA/voltage gain
	configReg.f.pga 	 			= gainValue;

	// set the channel
	status = _ChannelToInputMux(channel, channelVal);
	if (status != EXIT_SUCCESS) {
		_Print(ADS1X15_SEVERITY_FATAL, " INFO: invalid channel selected, defaulting to Channel0\n");
	}
	configReg.f.input_mux 			= channelVal;

	// set 'start single-conversion' bit
	configReg.f.status_start 		= 1;


	// write to the config register after required swap of byte order
        configReg.val = ((configReg.val & 0xff) << 8) | ((configReg.val & 0xff00) >> 8);
	status = _WriteReg	( 	ADS1X15_REG_ADDR_CONFIG,
							configReg.val,
							2
						);
	if (status != EXIT_SUCCESS) {
		_Print(ADS1X15_SEVERITY_FATAL, " ERROR: Writing to Configuration Register failed!\n");
		//return status;
	}

	// wait for the conversion to complete
	usleep(conversionDelayUs);


	// read the conversion results
	status = _ReadConverson(value);


	return status;
}

int ads1X15::ReadLastConversion (int &value)
{
	int status;

	// read the conversion results
	status = _ReadConverson(value);

	return status;
}


