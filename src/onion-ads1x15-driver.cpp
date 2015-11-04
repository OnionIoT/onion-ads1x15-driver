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

	gainValue			= ADS1015_REG_CONFIG_PGA_2_048V;
}

void ads1X15::SetVerbosity (int input)
{
	verbosityLevel	= input;

	// also set the i2c lib verbosity
	i2c_setVerbosity(input);
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
	int 	configReg 	= 0;
	int 	result;

	if (channel >= ADS1X15_NUM_CHANNELS) {
		return EXIT_FAILURE;
	}

	// read the current reg value
	status = i2c_read	(	
							ADS1X15_I2C_DEVICE_NUM, 
							devAddr, 
							ADS1X15_REG_ADDR_CONFIG, 
							&configReg, 
							2
						);
	if (status != EXIT_SUCCESS) {
		// ERROR!
	}

	// set up the config register for single-ended read
	_SetBit(configReg, ADS1X15_REG_CONFIG_COMP_QUEUE_OFFSET, 	ADS1X15_COMP_QUEUE_DISABLE_COMP);	// disable comparator
	_SetBit(configReg, ADS1X15_REG_CONFIG_COMP_LATCH_OFFSET, 	ADS1X15_COMP_LATCH_NO_LATCH);		// non-latching
	_SetBit(configReg, ADS1X15_REG_CONFIG_COMP_POLARITY_OFFSET, ADS1X15_COMP_POLARITY_ACTIVE_LOW);	// ALRT pin is active-low
	_SetBit(configReg, ADS1X15_REG_CONFIG_COMP_MODE_OFFSET,		ADS1X15_COMP_MODE_TRADITIONAL);		// set comparator to traditional
	_SetBit(configReg, ADS1X15_REG_CONFIG_DATARATE_OFFSET,		ADS1X15_DATARATE_1600SPS);			// 1600 samples per second
	_SetBit(configReg, ADS1X15_REG_CONFIG_MODE_OFFSET,			ADS1X15_MODE_SINGLE_SHOT);

	// set PGA/voltage gain
	_SetBit(configReg, ADS1X15_REG_CONFIG_PGA_OFFSET,			gainValue);		

	// set the channel
	_SetBit(configReg, ADS1X15_REG_CONFIG_INPUT_MUX_OFFSET,		channel);		

	// set 'start single-conversion' bit
	_SetBit(configReg, ADS1X15_REG_CONFIG_OP_STATUS_START_OFFSET,	1);


	// write to the config register
	status = i2c_writeBytes	( 	
							ADS1X15_I2C_DEVICE_NUM,
							devAddr,
							ADS1X15_REG_ADDR_CONFIG,
							configReg,
							2
						);
	if (status != EXIT_SUCCESS) {
		// ERROR!
	}

	// wait for the conversion to complete
	usleep(conversionDelayUs);

	// read the conversion results
	status = i2c_read	(	
							ADS1X15_I2C_DEVICE_NUM, 
							devAddr, 
							ADS1X15_REG_ADDR_CONFIG, 
							&result, 
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




