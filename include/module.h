#ifndef _MODULE_H_
#define _MODULE_H_

#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>





class Module {
public:
	Module(void);
	~Module(void);

	virtual void 	SetVerbosity	(int input);
	void 			SetVerbosity	(bool input);

	void			SetDebugMode	(int input);
	void			SetDebugMode	(bool input);


protected:
	// protected functions
	void 				_SetBit			(int &regVal, int bitNum, int value);
	int 				_GetBit			(int regVal, int bitNum);

	// protected members
	int				verbosityLevel;
	int 			debugLevel;
};

#endif 	// _MODULE_H_