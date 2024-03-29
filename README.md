# Differentiator V1.0

## Description
A simple program, calculating the diravative of an expression inserted.
This code represents a practical work of Korney Ivanishin.

## The latest version
The latest version of the Differentiator can be found on its authors
[GitHub page](https://github.com/quaiion/differentiator).

## Documentation
Documentation lines (suitable for doxygen usage) will be soon added into the
header files.

## Dependencies
To successfully compile this program, you will need to have **GCC** compiler & linker and **make** utility installed (if not, you will just have no ability to use the Makefile, but will still be able to compile the project manually). Besides, you will need the **dot** utility from [Graphviz](https://graphviz.org) software package and any available **TeX** distributive so the program will be able to visualize the output.

## Shell specifications
If you are using Windows CMD shell, please, DO NOT FORGET to replace the
line `#define BASH_COMPILATION` with `#define WINCMD_COMPILATION` in **common.cpp**
file. Ignoring this may lead to a bunch of serious troubles.

## How-to-use
To launch the program, compile **diftor.cpp**, **diftor_main.cpp**, **common.cpp**
and **bin_tree.cpp** files together (you can use the Makefile attached despite
the fact that some of mentioned there flags will lead to some false warnings)
and execute the **run_diftor** file. The program will ask you to name the .qdf
text file with the target formula in it. The formula should be printed using
full-bracket style (see the example in **input_example.qdf**). As the result you
will get 2 .png files (expression tree visualizations berfore and after the
differentiation - **dump_init.png** and **dump_fin.png**) and 2 .pdf files (initial and
final expressions formed by LaTeX - **form_init.pdf** and **form_fin.pdf**). You can
find the output examples in **---_example.pdf / ---_example.png files**.

_For more accurate instructions of how to write down an expression please see the ***OPERATIONS.md*** file._

## Contacts
#### Korney Ivanishin, author of the project
email: <korney059@gmail.com>,
GitHub: [quaiion](https://github.com/quaiion)
