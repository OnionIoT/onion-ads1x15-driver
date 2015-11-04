#ifndef _ONION_ADS1X15_DRIVER_H_
#define _ONION_ADS1X15_DRIVER_H_

#include <module.h>
#include <unistd.h>
		

#define ADS1X15_PRINT_BANNER			"ADS1X15::"

#define ADS1X15_I2C_DEVICE_NUM			(0)
#define ADS1X15_I2C_DEVICE_ADDR			(0x48)

#define ADS1015_CONVERSION_DELAY		(1)
#define ADS1115_CONVERSION_DELAY		(8)

#define ADS1015_CONVERSION_DATA_SHIFT	(4)		// CONVERSION[15:4] = DATA[11:0]
#define ADS1115_CONVERSION_DATA_SHIFT	(0)		// CONVERSION[15:0] = DATA[15:0]

#define ADS1X15_REG_ADDR_CONVERT		(0x00)
#define ADS1X15_REG_ADDR_CONFIG			(0x01)
#define ADS1X15_REG_ADDR_LOWTHRESH		(0x02)
#define ADS1X15_REG_ADDR_HITHRESH		(0x03)


#define ADS1X15_REG_CONFIG_OP_STATUS_START_OFFSET	(15)
#define ADS1X15_REG_CONFIG_INPUT_MUX_OFFSET			(12)
#define ADS1X15_REG_CONFIG_PGA_OFFSET				(9)
#define ADS1X15_REG_CONFIG_MODE_OFFSET				(8)
#define ADS1X15_REG_CONFIG_DATARATE_OFFSET			(5)
#define ADS1X15_REG_CONFIG_COMP_MODE_OFFSET			(4)
#define ADS1X15_REG_CONFIG_COMP_POLARITY_OFFSET		(3)
#define ADS1X15_REG_CONFIG_COMP_LATCH_OFFSET		(2)
#define ADS1X15_REG_CONFIG_COMP_QUEUE_OFFSET		(0)

#define ADS1X15_REG_CONFIG_PGA_6_144V				(0)
#define ADS1X15_REG_CONFIG_PGA_4_096V				(1)
#define ADS1X15_REG_CONFIG_PGA_2_048V				(2)
#define ADS1X15_REG_CONFIG_PGA_1_024V				(3)
#define ADS1X15_REG_CONFIG_PGA_0_512V				(4)
#define ADS1X15_REG_CONFIG_PGA_0_256V				(5)


typedef union adsRegConfig_u {
	int val;
	struct {
		unsigned char comp_queue 		: 2;
		unsigned char comp_latch 		: 1;
		unsigned char comp_polarity 	: 1;
		unsigned char comp_mode 	 	: 1;
		unsigned char data_rate		 	: 3;
		unsigned char mode 				: 1;
		unsigned char pga  				: 3;
		unsigned char input_mux			: 3;
		unsigned char status_start		: 1;
	} f;
} adsRegConfig_t;

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
	ADS1X15_GAIN_SIXTEEN			= ADS1X15_REG_CONFIG_PGA_0_256V
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

	int 	SetChannel		(int channel);
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



private:
	// private functions
	int 	_ReadReg 		(int addr, int &value, int numBytes);
	int 	_WriteReg 		(int addr, int value, int numBytes);

	// private members
	int 	devAddr;
	int 	conversionDelay;
	int 	conversionDelayUs;
	int 	conversionBitShift;

	int 	gainValue;
};


#endif // _ONION_ADS1X15_DRIVER_H_