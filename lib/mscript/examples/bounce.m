.memory
number x
number y
number state

.code
clear
equals x 5
equals y 7
equals state 0

begin:
jump equals state 0 dir_0
jump equals state 1 dir_1
jump equals state 2 dir_2
jump equals state 3 dir_3

dir_0:
add x 1 x
add y 1 y
pixel x y 1
jump bigger x 119 change_state
jump bigger y 79 change_state
jump begin

dir_1:
pixel x y 0
add x 1 x
sub y 1 y
pixel x y 1
jump bigger x 119 change_state
jump smaller y 1 change_state
jump begin

dir_2:
pixel x y 0
sub x 1 x
sub y 1 y
pixel x y 1
jump smaller x 1 change_state
jump smaller y 1 change_state
jump begin

dir_3:
pixel x y 0
sub x 1 x
add y 1 y
pixel x y 1
jump bigger y 79 change_state
jump smaller x 1 change_state
jump begin

change_state:
add state 1 state
mod state 4 state
jump begin
