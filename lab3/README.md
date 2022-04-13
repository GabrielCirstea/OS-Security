# Lab 3

## Practice 1

Use `ionotify`.

Make a program that will watch all the files inside a folder and will count how
many times each file was accessed.
On creating a sub direcotry the program should start watching this directory as
well.

### Run

Compile it:

```
make p1
```

Run:

```
./p1_main <folder>
```

## Practice 2

Play with curl example and sha-256 algorithm

### Compile and run

```
make p2
./p2_main <file>
```

## Practice 3

Using stuff from above, create a program that will watch all the files inside a
folder and new created folders as well, and will compute the sha-256 hash to
verify it using [virustatal api](https://developers.virustotal.com/reference/overview).
And if it is detected as a virus, the file must be deleted.

### Note

Pretty messy solution.

I've skipped the JSON parsing part and the program "just works".

### Compile and run:

```
make guardian
./guardian <dir_name>
```
