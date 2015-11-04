#include <onion-i2c.h>
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

	// also set the i2c lib verbosity
	i2c_setVerbosity(input);
}


// private class function
int ads1X15::_ReadReg (int addr, int &value, int numBytes)
{
	int status;

	_Print("%s Reading reg addr 0x%02x...", ADS1X15_PRINT_BANNER, addr);

	if (debugLevel == 0) {
		status = i2c_read	(	
							ADS1X15_I2C_DEVICE_NUM, 
							devAddr, 
							addr, 
							&value, 
							numBytes
						);
	}
	else {
		status 	= EXIT_SUCCESS;
		value 	= 0x00;
	}

	_Print("\tRead value 0x%04x\n", value);

	return status;
}

int ads1X15::_WriteReg (int addr, int value, int numBytes)
{
	int status;

	_Print("%s Writing Reg addr 0x%02x with 0x%04x\n", ADS1X15_PRINT_BANNER, addr, value);

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


// class functions
int ads1X15::SetGain (int gain)
{
	gainValue	= gain;

	return EXIT_SUCCESS;
}

int ads1X15::ReadAdc (int channel, int &value)
{
	int 	status;
	adsRegConfig_t 	configReg;
	int 	result, channelVal;
	int 	configRegTmp;	//dbg

	if (channel >= ADS1X15_NUM_CHANNELS) {
		return EXIT_FAILURE;
	}

	// read the current reg value
	status = _ReadReg	(	ADS1X15_REG_ADDR_CONFIG, 
							configReg.val, 
							2
						);
	if (status != EXIT_SUCCESS) {
		// ERROR!
	}

	// set up the config register for single-ended read
	/*_SetBit(configRegTmp, ADS1X15_REG_CONFIG_COMP_QUEUE_OFFSET, 	ADS1X15_COMP_QUEUE_DISABLE_COMP);	// disable comparator
	_SetBit(configRegTmp, ADS1X15_REG_CONFIG_COMP_LATCH_OFFSET, 	ADS1X15_COMP_LATCH_NO_LATCH);		// non-latching
	_SetBit(configRegTmp, ADS1X15_REG_CONFIG_COMP_POLARITY_OFFSET, 	ADS1X15_COMP_POLARITY_ACTIVE_LOW);	// ALRT pin is active-low
	_SetBit(configRegTmp, ADS1X15_REG_CONFIG_COMP_MODE_OFFSET,		ADS1X15_COMP_MODE_TRADITIONAL);		// set comparator to traditional
	_SetBit(configRegTmp, ADS1X15_REG_CONFIG_DATARATE_OFFSET,		ADS1X15_DATARATE_1600SPS);			// 1600 samples per second
	_SetBit(configRegTmp, ADS1X15_REG_CONFIG_MODE_OFFSET,			ADS1X15_MODE_SINGLE_SHOT);			// single-shot mode

	// set PGA/voltage gain
	_SetBit(configRegTmp, ADS1X15_REG_CONFIG_PGA_OFFSET,			gainValue);		*/

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
	switch(channel) {
		case ADS1X15_CHANNEL_A0:
			channelVal 	= ADS1X15_INPUT_MUX_SINGLE_A0;
			break;

		case ADS1X15_CHANNEL_A1:
			channelVal 	= ADS1X15_INPUT_MUX_SINGLE_A1;
			break;

		case ADS1X15_CHANNEL_A2:
			channelVal 	= ADS1X15_INPUT_MUX_SINGLE_A2;
			break;

		case ADS1X15_CHANNEL_A3:
			channelVal 	= ADS1X15_INPUT_MUX_SINGLE_A3;
			break;

		default:
			channelVal 	= ADS1X15_INPUT_MUX_SINGLE_A0;
			break;
	}

	configReg.f.input_mux 			= channelVal;
	_SetBit(configRegTmp, ADS1X15_REG_CONFIG_INPUT_MUX_OFFSET,		channelVal);		

	// set 'start single-conversion' bit
	configReg.f.status_start 		= 1;
	_SetBit(configRegTmp, ADS1X15_REG_CONFIG_OP_STATUS_START_OFFSET,	1);


	// write to the config register
	status = _WriteReg	( 	ADS1X15_REG_ADDR_CONFIG,
							configReg.val,
							2
						);
	if (status != EXIT_SUCCESS) {
		// ERROR!
	}

	// wait for the conversion to complete
	usleep(conversionDelayUs);

	// read the conversion results
	status = _ReadReg	(	ADS1X15_REG_ADDR_CONFIG, 
							result, 
							2
						);
	if (status != EXIT_SUCCESS) {
		// ERROR!
	}

	// shift the result
	result 	= result >> conversionBitShift;

	// return the result
	value 	= result;


	return status;
}




