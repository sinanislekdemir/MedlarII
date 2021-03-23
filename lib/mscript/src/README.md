# MScript

MScript is a scripting language with several challenges

* Script shouldn't be loaded fully into RAM because we don't have much RAM
* Script should be read line by line and executed right away
* It should be easy to parse
* It shouldn't compile into binary because we can't afford that either
* Fixed line length to avoid using too much RAM (64)

## Syntax Style

### Sections

Each MSript starts with Meta definitions, followed by Variable Definitions and then the logic code.

### Meta

```
.meta
@author sinan islekdemir
@appname Sample Code
@mversion 0.1            # which version it uses
@load <filename>         # We need to support imports
@load <another-filename>
```

### Variable Definitions

```
.memory
char <name> <size>
byte <name> <size>
number <name> <size - default 1>
```

*Size must be a constant

### Logic Code

```
.code
:label
<some code here>
return
[Function] [Argument1] .. [ArgumentN] [Result]
jump equals <arg1> <arg2> label
jump bigger <arg1> <arg2> label
jump smaller <arg1> <arg2> label
jump not <arg1> <arg2> label
jump label
call label # Call returns back to this position if label ends with return keyword
```

## Example:


```
.meta
@author sinan islekdemir
@appname hello-world
@mversion 0.1

.memory
char text 64

.code
equal text "hello world"
append text " huysuz adam"  # for multi line etc
print text

sprint text # Send through serial port
```
