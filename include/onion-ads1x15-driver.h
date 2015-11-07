#ifndef _ONION_ADS1X15_DRIVER_H_
#define _ONION_ADS1X15_DRIVER_H_

#include <module.h>
#include <unistd.h>
		

#define ADS1X15_PRINT_BANNER			"ADS1X15::"

#define ADS1X15_I2C_DEVICE_NUM			(0)
#define ADS1X15_I2C_DEVICE_ADDR			(0x48)


#define ADS1X15_SEVERITY_FATAL			(-1)
#define ADS1X15_SEVERITY_INFO			(0)
#define ADS1X15_SEVERITY_DEBUG			(1)

#define ADS1015_CONVERSION_DELAY		(1)
#define ADS1115_CONVERSION_DELAY		(8)

#define ADS1015_CONVERSION_DATA_SHIFT	(4)		// CONVERSION[15:4] = DATA[11:0]
#define ADS1115_CONVERSION_DATA_SHIFT	(0)		// CONVERSION[15:0] = DATA[15:0]

#define ADS1X15_REG_ADDR_CONVERT		(0x00)
#define ADS1X15_REG_ADDR_CONFIG			(0x01)
#define ADS1X15_REG_ADDR_LOWTHRESH		(0x02)
#define ADS1X15_REG_ADDR_HITHRESH		(0x03)

#define ADS1X15_REG_CONFIG_PGA_6_144V				(0)
#define ADS1X15_REG_CONFIG_PGA_4_096V				(1)
#define ADS1X15_REG_CONFIG_PGA_2_048V				(2)
#define ADS1X15_REG_CONFIG_PGA_1_024V				(3)
#define ADS1X15_REG_CONFIG_PGA_0_512V				(4)
#define ADS1X15_REG_CONFIG_PGA_0_256V				(5)


// union to represent the CONFIG register
typedef union adsRegConfig_u {
	int val;
	struct {
#if BYTE_ORDER == BIG_ENDIAN
		// Big Endian - MSB in smallest addr
		int 			padding 			: 16;
		unsigned char 	status_start		: 1;	// config[15]
		unsigned char 	input_mux			: 3;	// config[14:12]
		unsigned char 	pga  				: 3;	// config[11:9]
		unsigned char 	mode 				: 1;	// config[8]
		unsigned char 	data_rate		 	: 3;	// config[7:5]
		unsigned char 	comp_mode 	 		: 1;	// config[4]
		unsigned char 	comp_polarity 		: 1;	// config[3]
		unsigned char 	comp_latch 			: 1;	// config[2]
		unsigned char 	comp_queue 			: 2;	// config[1:0]
#else
		// Little Endian - LSB in smallest addr
		unsigned char 	comp_queue 			: 2;	// config[1:0]
		unsigned char 	comp_latch 			: 1;	// config[2]
		unsigned char 	comp_polarity 		: 1;	// config[3]
		unsigned char 	comp_mode 	 		: 1;	// config[4]
		unsigned char 	data_rate		 	: 3;	// config[7:5]
		unsigned char 	mode 				: 1;	// config[8]
		unsigned char 	pga  				: 3;	// config[11:9]
		unsigned char 	input_mux			: 3;	// config[14:12]
		unsigned char 	status_start		: 1;	// config[15]
		int 			padding 			: 16;
#endif	// BYTE_ORDER
	} f;
} adsRegConfig_t;


// enums to represent parameters
typedef enum
{
	ADS1X15_TYPE_ADS1015	= 0,
	ADS1X15_TYPE_ADS1115,
	ADS1X15_NUM_TYPES
} adsTypes_t;

typedef enum
{
	ADS1X15_CHANNEL_A0		= 0,
	ADS1X15_CHANNEL_A1,
	ADS1X15_CHANNEL_A2,
	ADS1X15_CHANNEL_A3,
	ADS1X15_NUM_CHANNELS
} adsChannels_t;

typedef enum
{
	ADS1X15_MAX_VOLTAGE_6_144V	= 6144,
	ADS1X15_MAX_VOLTAGE_4_096V	= 4096,
	ADS1X15_MAX_VOLTAGE_2_048V	= 2048,
	ADS1X15_MAX_VOLTAGE_1_024V	= 1024,
	ADS1X15_MAX_VOLTAGE_0_512V	= 512,
	ADS1X15_MAX_VOLTAGE_0_256V	= 256
} adsVoltageRanges_t;


// enums to represent register settings
typedef enum
{
	ADS1X15_INPUT_MUX_COMP_A0_A1 	= 0,
	ADS1X15_INPUT_MUX_COMP_A0_A3,
	ADS1X15_INPUT_MUX_COMP_A1_A3,
	ADS1X15_INPUT_MUX_COMP_A2_A3,
	ADS1X15_INPUT_MUX_SINGLE_A0,
	ADS1X15_INPUT_MUX_SINGLE_A1,
	ADS1X15_INPUT_MUX_SINGLE_A2,
	ADS1X15_INPUT_MUX_SINGLE_A3
} adsInputMux_t;

typedef enum
{
	ADS1X15_GAIN_TWOTHIRDS			= ADS1X15_REG_CONFIG_PGA_6_144V,
	ADS1X15_GAIN_ONE				= ADS1X15_REG_CONFIG_PGA_4_096V,
	ADS1X15_GAIN_TWO  				= ADS1X15_REG_CONFIG_PGA_2_048V,
	ADS1X15_GAIN_FOUR 				= ADS1X15_REG_CONFIG_PGA_1_024V,
	ADS1X15_GAIN_EIGHT				= ADS1X15_REG_CONFIG_PGA_0_512V,
	ADS1X15_GAIN_SIXTEEN			= ADS1X15_REG_CONFIG_PGA_0_256V,
	ADS1X15_NUM_GAIN
} adsGain_t;

typedef enum
{
	ADS1X15_MODE_CONTINUOUS_CONV	= 0,
	ADS1X15_MODE_SINGLE_SHOT
} adsMode_t;

typedef enum
{
	ADS1X15_DATARATE_128SPS			= 0,
	ADS1X15_DATARATE_250SPS,
	ADS1X15_DATARATE_490SPS,
	ADS1X15_DATARATE_920SPS,
	ADS1X15_DATARATE_1600SPS,
	ADS1X15_DATARATE_2400SPS,
	ADS1X15_DATARATE_3300SPS
} adsDataRate_t;

typedef enum
{
	ADS1X15_COMP_MODE_TRADITIONAL	= 0,
	ADS1X15_COMP_MODE_WINDOW
} adsCompMode_t;

typedef enum
{
	ADS1X15_COMP_POLARITY_ACTIVE_LOW	= 0,
	ADS1X15_COMP_POLARITY_ACTIVE_HIGH
} adsCompPolarity_t;

typedef enum
{
	ADS1X15_COMP_LATCH_NO_LATCH			= 0,
	ADS1X15_COMP_LATCH_LATCH_EN
} adsCompLatch_t;

typedef enum
{
	ADS1X15_COMP_QUEUE_ONE_COMP			= 0,
	ADS1X15_COMP_QUEUE_TWO_COMP,
	ADS1X15_COMP_QUEUE_FOUR_COMP,
	ADS1X15_COMP_QUEUE_DISABLE_COMP
} adsCompQueue_t;



class ads1X15 : public Module {
public:
	ads1X15(int addr = ADS1X15_I2C_DEVICE_ADDR, int type = ADS1X15_TYPE_ADS1015);
	~ads1X15(void);

	void	Reset			(void);

	void 	SetVerbosity 	(int input);

	int 	ReadMaxVoltage 			(float maxVoltage);
	int 	SetChannel				(int channel);
/*
	int 	SetInputMux 			(int muxSelect);
	int 	SetMode 				(int mode);
	int 	SetDataRate				(int dataRate);
	int 	SetComparatorMode		(int mode);
	int 	SetComparatorPolarity	(int polarity);
	int 	SetComparatorLatch		(int latchEn);
	int 	SetComparatorQueue 		(int queueMode);*/

	int 	SetGain 				(int gain);
	
	int 	ReadAdc					(int channel, int &value);
	int 	ReadLastConversion		(int &value);



private:
	// private functions
	int 	_ReadReg 				(int addr, int &value, int numBytes);
	int 	_WriteReg 				(int addr, int value, int numBytes);

	int 	_ChannelToInputMux		(int channel, int &fieldValue);
	int 	_ReadConverson			(int &value);

	// private members
	int 	devAddr;
	int 	conversionDelay;
	int 	conversionDelayUs;
	int 	conversionBitShift;

	int 	gainValue;
};


#endif // _ONION_ADS1X15_DRIVER_H_