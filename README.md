# Bluefin Compiler
Bluefin is an OOP language with a C-like syntax. It supports several common language idioms including classes, inheritance, polymorphism, types, and control blocks. This compiler translates the source code to LLVM IR. The frontend parser is built with ANTLR. To ensure the compiler is correct, there are 120+ integration tests written with Google Test Framework. 

## The Origin Story

During an internship in early 2016, a coworker introduced me to the topic of 'language design' and casually mentioned that he had built a compiler in a previous job. I'm not sure why, but for some reason, I was awestruck at the thought of designing a custom programming language; it seemed...amazing and powerful. 

A couple months later, I began flirting with language design. I tried writing a DSL with Boo; it really didn't work out. Then, in late 2016, I wrote a Pascal-like interpreter ([Swoosh](https://github.com/NanoBreeze/swoosh)) but eventually gave up on it after the code became too messy. Despite taking a compilers course and doing an internship partly related to compilers, my knowledge was still too shallow. As a result, after graduating, I set out to learn more about compilers, which culminated in developing Bluefin. This was an excellent learning opportunity and although I've only scratched the surface of a deep basin, I'm happy to have gained the skills to build a toy language!

Fun note: Many software tools are named after animals (eg, Gnu, Yacc, Bison, Ant, etc.). Originally, I tried naming this language "Moose" but it was already taken by another software, so I had to settle on "Bluefin" (named after the "Bluefin tuna")


## Features
Admittedly, Bluefin is not a creative language. It basically has C-like syntax and copies parts of C and C++. Its features include:

- **Classes** (with methods, member variables, and custom member initialization). Within a class' scope, forward referencing is allowed (similar to C++). To allow custom member initialization, we generate a LLVM IR constructor for each class (also similar to C++). 
- **Inheritance**: Only single inheritance is permitted. A subclass may access its parent's members and call its parent's methods. If both a subclass and parent class have member variables with same name; then in the subclass, that variable name shall refer to the subclass' variable (like in C++). In LLVM IR, the subclass' members overlay on top of its parent's members. Why does Bluefin not support multiple inheritance? Mostly because it's cumbersome to think about, complicates the implementation, and I really didn't want to deal with virtual/diamond inheritance.
- **Polymorphism**: Ahhhh, the meat of any OOP language! We use vptr and vtable. Any method declared with the `virtual` keyword shall be put into a vtable. Any method that overrides a parent class' virtual method must contain the `override` keyword.
- **Static type checking**: Web developers won't like this but I'm just going to say it: I have beef with dynamic type checking. Bluefin supports static type checking; it also has several built-in primitive types and allows user-defined types (classes)
- **Automatic type promotion**: An `int` may automatically to a `float` but not vice versa because narrowing is illegal. Type promotion may occur in expressions, return statement, or in function call arguments
- **Variables**: Global, local, and class member variables are permitted. In the future, we will add support for calling external functions so we can get memory from the heap
- **Control flow**: `if/else` blocks and `while` loops. They can be nested. The `continue` and `break` keywords also exist, although I haven't implemented semantic checks or code gen for them yet
- **Functions**: All function definitions are global and they cannot be nested. When a function appears in a class, it becomes a method! Right now, we don't support function overloading, but if we change that in the future, we'll need to find a simple name mangling scheme (sorry Itanium), update our polymorphism implementation, and deal with ambiguous funcion/method calls. So far, we don't support function declaration; this should be changed so we can make external library calls and allow cyclic recursion
- **Expressions**: like binary and unary expressions. Normal order of precedence, like in math, is followed (thanks to ANTLR)
- **Keywords**: Thanks to ANTLR
- **Error Messages**: Especially for declarations, resolutions, type checking, and miscellaneous rules. ANTLR provides its own error messages for the parser

## Language Standard
The language standard for Bluefin was initially based off of C89 and can be found in languageStandard.md. Right now, it's very much out of date and hasn't been updated yet to include classes, inheritance, or polymorphism

## Future extensions
Here are some good extensions/bug fixes to add to Bluefin:

- Update code generator to allow assignment of class instance to each other and to function calls
- Update code generator to allow a function to return a class instance (return by pointer in first argument)
- Add pointers, which will clarify pass-by-value vs pass-by-reference
- Add arrays
- Add RTTI and enable downcasting
- Add function overloading
- Add function declarations/prototypes
- Add semantic checking and codegen for 'break' and 'continue' 
- Add code gen for string types
- Include LLVM optimization passes
- Update language standard
- Clarify build process. Right now, this builds only in VS x64 and this repo doesn't include the LLVM libs (which are humongous). Maybe create a Docker image and support building this on Linux too
