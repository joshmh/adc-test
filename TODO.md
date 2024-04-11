* Use leds to indicate voltage values so we can plug into breadboard
* currently, PB4 is more or less working but not PA10 but maybe it's beacause of the dev board
* Might as well try a non-broken board also

Got PA10 working
Got PA11 working
Looks like PA13 isn't available

* look into resistor arrangement for adcs -- just use regular dividers
* different pins give different values but overall, they are working -- definitely track pretty well, should be okay with callibration
* might want to use a separate ADC in the future


- PB3 -- ADC_IN2
- PB4 -- ADC_IN3 xx xx
- PB1 -- ADC_IN5
- PB2 -- ADC_IN4 xx --
- PB13 -- ADC_IN0
- PB14 -- ADC_IN1
- PA10 -- ADC_IN6 xx x
- PA11 -- ADC_IN7 xx --
- PA12 -- ADC_IN8
- PA13 -- ADC_IN9
- PA14 -- ADC_IN10
- PB15 -- ADC_IN11
