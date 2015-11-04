#include <main.h>

int main(int argc, char* argv[])
{
	int status;
	int value;

	ads1X15*	adsObj	= new ads1X15;

	// setup the verbosity
	adsObj->SetVerbosity(1);
	
	// run the ADC conversion
	status 	= adsObj->ReadAdc(0, value);

	printf("status is %d, value is %d\n", status, value);


	return 0;
}