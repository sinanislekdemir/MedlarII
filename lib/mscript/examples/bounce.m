.memory

.code
clear
equals $0 5
equals $1 7
equals $2 0

begin:
jump equals $2 0 dir_0
jump equals $2 1 dir_1
jump equals $2 2 dir_2
jump equals $2 3 dir_3

dir_0:
add $0 1 $0
add $1 1 $1
pixel $0 $1 1
jump bigger $0 119 change_2
jump bigger $1 79 change_2
jump begin

dir_1:
pixel $0 $1 0
add $0 1 $0
sub $1 1 $1
pixel $0 $1 1
jump bigger $0 119 change_2
jump smaller $1 1 change_2
jump begin

dir_2:
pixel $0 $1 0
sub $0 1 $0
sub $1 1 $0
pixel $0 $1 1
jump smaller $0 1 change_2
jump smaller $1 1 change_2
jump begin

dir_3:
pixel $0 $1 0
sub $0 1 $0
add $1 1 $1
pixel $0 $1 1
jump bigger $1 79 change_2
jump smaller $0 1 change_2
jump begin

change_2:
add $2 1 $2
mod $2 4 $2
jump begin
