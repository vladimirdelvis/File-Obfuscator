# File Obfuscator/De-obfuscator

A program that splits the source file (up to 8 parts) and writes the bytes contained in each part to the corresponding part of the target file randomly using the Mersenne Twister algorithm. These parts are distributed independently to subprocesses.

# How To Use

Open the GUI application. Select the locations of the source and target files. Fill in the Secret 1 and 2 values as desired, select the mode and set the desired number (max 8) of subprocesses. Adjust the ranges of the parts in the tables.

> WARNING 1: Direct use of the CLI application may cause user errors.

> WARNING 2: The C++ library used by CLI programs that perform obfuscation and de-obfuscation must be the same. For this reason, CLI programs are statically linked to the C++ library in case of possible future updates. If libc++ and libstdc++ are used together, you will not be able to decrypt the relevant file.

# Support

Windows (MSYS2: Mingw-w64/Qt6) and linux(Qt6) supported.\
Please aware of warning 2.
