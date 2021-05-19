.memory
number counter
number pin_number

.code
println "blink the led"
equals pin_number 13

pinMode pin_number OUTPUT

blink:
digitalWrite pin_number HIGH
delay 500
digitalWrite pin_number LOW
delay 500
inc counter

print "Counter at: "
println counter

jump equals counter 100 exit
jump blink

exit:
println "Bye bye!"
