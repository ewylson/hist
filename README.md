# hist
A simple C program that counts printable ASCII characters in a file and generates a histogram.

Download (Linux only): *link*

## How to use

```
Usage: hist [OPTION]... FILE
Counts printable characters in a file and creates a histogram.

    -l, --letters-only              count only letters
    -d, --digits-only               count only decimal digits
    -a, --alphanumeric-only         count only letters and decimal digits
    -p, --punctuation-only          count only punctuation marks
    -L, --lowercase-letters-only    count only lowercase letters among letters
    -U, --uppercase-letters-only    count only uppercase letters among letters
    -C, --case-ignore               count identical letters with different case as a single character
    -r, --range=SIZE                set the histogram range
        --help                      display this help and exit
        --version                   output version information and exit

Examples:
    hist file.txt     Count all printed characters in the file and display a histogram.
    hist -aC file.txt Count only letters and digits in the file, ignoring case, and display a histogram.
```

## Building from Source Code

### Linux

The program can be compiled manually with any C compiler that supports the C99 standard or later, such as GCC or Clang:
```console
cc main.c -o hist -std=c99 -O2
```

Alternatively, you can build the program using the `make` utility from within the program directory:
```console
make
```

## License

This project is licensed under the MIT License.  
You are free to use, modify, and distribute it under the terms of that license.
