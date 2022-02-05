*Here you can find the instructions of how to write down the expression you*
*want to differentiate so the program can understand you.*

First of all, you should use a full-bracket style of printing formula. That
means that every single number or variable, every unary operation (and, of
course, it's argument) and every binary operation should be wrapped in a pair
of external round brackets. This is applied to the whole expression itself as
well. Moreover, no additional pairs of brackets, no spaces and no newlines
should be used.

If your style of typing is wrong and the differentiator cannot understnd what
you meant, you will see a message **Tree formation failed: impossible expression**
**sequence** with a bunch of special information.

The other way to see this kind of message is to misprint the operation symbol.
All in all there are 18 specified operations, the accurate list of them and
their symbols is given below:

##### Unary operations
- Unary plus     --> **+**      --> **+(a)**, where **a** is an argument
- Unary minus    --> **-**      --> **-(a)**, where **a** is an argument
- Sinus          --> **sin**    --> **sin(a)**, where **a** is an argument
- Cosinus        --> **cos**    --> **cos(a)**, where **a** is an argument
- Tangent        --> **tan**    --> **tan(a)**, where **a** is an argument
- Cotangent      --> **cot**    --> **cot(a)**, where **a** is an argument
- Arcsinus       --> **arcsin** --> **arcsin(a)**, where **a** is an argument
- Arccosinus     --> **arccos** --> **arccos(a)**, where **a** is an argument
- Arctangent     --> **arctan** --> **arctan(a)**, where **a** is an argument
- Arccotangent   --> **arccot** --> **arccot(a)**, where **a** is an argument
- Nat. logarithm --> **ln**     --> **ln(a)**, where **a** is an argument
- Exponent       --> **exp**    --> **exp(a)**, where **a** is an argument

It is important to mention, that negative numbers don't have to be written down
as positive ones with a unary minus. Basically, you can write negative numbers
without brackets separating a minus and a number and the program will get it:
**-47** is equivalent to **-(47)**. But this applies **ONLY** to numbers - no
variables, no expressions.

##### Binary operations
- Addition       --> **+**   --> **(a)+(b)**, where **a** and **b** are summands
- Subtraction    --> **-**   --> **(a)-(b)**, where **a** is a minuend, **b** is a subtrahend
- Multiplication --> **\***  --> **(a)*(b)**, where **a** and **b** are multipliers
- Division       --> **/**   --> **(a)/(b)**, where **a** is a divisible and **b** is a divider
- Logarythm      --> **log** --> **log\[a]\(b)**, where **a** is a basis, **b** is an argument
- Exponentiation --> **^**   --> **(a)^(b)**, where **a** is a basis, **b** is a power

To see an example of correct input style, check the **examples** folder on
https://github.com/quaiion/differentiator. *More examples will be added soon.*
