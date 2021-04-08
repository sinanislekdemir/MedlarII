.meta
@author sinan
@appname File Test

.memory
file readme_file
number location
number size
char buffer 65

.code
oprintln "reading readme.txt"

equals location 0
fopen readme_file "readme.txt"
fsize readme_file size

begin:
fread readme_file locawtion 64 buffer
oprintln buffer
add location 64 location
jump smaller location size begin

exit:
oprintln "the end"
