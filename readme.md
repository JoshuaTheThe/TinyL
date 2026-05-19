
# tiny language (TL)
- everything is an expression
- to create an array, just use <code>[a,b,...]</code>
- no loops, use recursion
- maximum of 16 function args
- basically no error checking

# variables
- variables can be created either by the <code>let</code> keyword or by the create-assign (<code>:=</code>) operator.

# operators
- we have + - * / % > <

# EOE (end of expression)
- the EOE marker (`;`) is used to clear the stack ready for the next expression, not just grammar and is thus optional

# Strings
- are just arrays, with a marker

# Arrays
- elements can be of any type, including functions and builtins!!!

# functions
- functions are values assigned to a variable, for simplicity
```
factorial := function (n) {
        if (n < 2) { 1 }
        else       { n * factorial(n - 1) }
};
```
