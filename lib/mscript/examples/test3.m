.memory
number value
number direction

.code
println "change led brightness"
equals value 0
equals direction 0

begin:
analogWrite 7 value
println value
delay 100

jump equals direction 0 increase
jump equals direction 1 decrease

increase:
add value 1 value
jump equals value 10 change_decrease
jump begin

change_decrease:
equals direction 1
jump begin

decrease:
sub value 1 value
jump equals value 0 change_increase
jump begin

change_increase:
equals direction 0
jump begin
