## Purpose

This subdirectory is designed to contain three parts
* the Prototyper class designed to be part of the SimulatorApp
* the `./fx/FX_DEADLINE_TROPHY.h` header exactly as it is meant to be compiled into the trophy firmware (i.e. our WLED fork)
* some "shim" files to make it work, i.e. appear to the FX header that it doesn't suspect it is being duped


> i.e. for you to prototype a WLED effect, in the best case 
> you only need to switch out the FX_DEADLINE_TROPHY.h
> 
> you can also replace it with a symbolic link to whereever you intend to build the firmware.
> just an idea, guess you're old enough. 


### TODO
some of the shim files are quite taken from WLED / FastLED *as they were*, i.e. should check their license agreement one day.