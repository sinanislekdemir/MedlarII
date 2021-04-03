.meta
@author sinan islekdemir - sinan@islekdemir.com
@appname Test App
@mversion 0.1

.memory
number counter
number pin_number

.code
oprintln "blink the led"
equals pin_number 13

pinMode pin_number OUTPUT

blink:
digitalWrite pin_number HIGH
delay 1000
digitalWrite pin_number LOW
delay 1000
inc counter

oprint "Counter at: "
oprintln counter

jump equals counter 100 exit
jump blink

exit:
oprintln "Bye bye!"
