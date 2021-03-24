.meta
@author sinan islekdemir - sinan@islekdemir.com
@appname Test App
@mversion 0.1

.memory
number counter
number pin_number

.code
sprintln "blink the led"
equals pin_number 13

pinMode pin_number OUTPUT

blink:
digitalWrite pin_number HIGH
delay 1000
digitalWrite pin_number LOW
inc counter

sprint "Counter at: "
sprintln counter

jump equals counter 10 exit
jump blink

exit:
sprintln "Bye bye!"
