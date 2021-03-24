.meta
@author sinan islekdemir - sinan@islekdemir.com
@appname Test App
@mversion 0.1

.memory
char string 1024
number test
number num_1
number num_2

.code
sprintln "hello world"
equals test 0

sprintln "add test"
equals num_1 10
equals num_2 5

add num_1 7 num_1
add num_2 num_1 num_2

sprint "num_1 = "
sprintln num_1
sprint "num_2 = "
sprintln num_2

jump equals num_1 17 print_num1_17
jump not num_1 17 print_fail_num1

print_fail_num1:
sprintln "number 1 calculation failed"
jump continue_2

print_num1_17:
sprintln "number 1 calculation success"

continue_2:
jump bigger num_2 17 print_num2_bigger
jump smaller num_2 17 print_fail_num2

print_fail_num2:
sprintln "number 2 calculation failed"
jump continue_3

print_num2_bigger:
sprintln "number 2 calculation success"

continue_3:

begin:
sprintln "I am process test 1, increment"
inc test
delay 100

jump equals test 100 exit_loop
jump begin

exit_loop:
sprintln "bye!"
