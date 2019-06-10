# Bluefin Programing Language

Bluefin has a syntax similar to C. 
In addition to having a subset of C features, it also includes classes, polymorphism, and method overloading.

TODO: add ptrs, classes, and arrays

# Bluefin Language Standard
This standard is organized in a similar format as "Part 6: Language" of the [C89/90 Standard](https://www.pdf-archive.com/2014/10/02/ansi-iso-9899-1990-1/ansi-iso-9899-1990-1.pdf). 
Where appropriate, it copies verbatium from the C Standard.
Since Bluefin is a hobby project, it goes without saying that this standard is a lot shorter and less detailed than C's Standard.

## 1 Lexical Elements

Tokens: keyword, identifier, numbers, string-literal, operator, punctuator

### 1.1 Keywords

The following keywords are reserved and not to be used as identifiers: 

`break`, `continue`, `if`, `else`, `while` (*control flow*) <br />
`int`, `float`, `string`, `void`, `bool` *(built-in types)* <br />
`struct` *(user-defined type)* <br />
`return` *(for functions)*

### 1.2 Identifiers

**Syntax**: [a-zA-A]+

**Description**: 
An identifier is a sequence of letters from the English alphabet. Unlike other languages, it can't contain digits or underscores.

**Semantics**: 
An identifier is used to denote a variable, a function, or a tag for a struct.

**Scopes of Identifiers**: An identifier is visible (can be referred to) only if it is within its scope. There are three types of scopes:
- File scope (global scope for the whole file)
- Block scope (anything inside `{` ... `}`, including function blocks, while loops, and nested blocks)
- Function prototype scope (eg, `n` in `void f(int n, int a[n])`)

**Types**: There are two categories of types: 
1. Object types, including built-in types and user-defined `struct`s. Built-in types include: `float`, `int`, `string`. 
2. Function types. The return type of a function, including `void`

### 1.3 Numbers

There are two types of numbers: floating and integers. 

#### 1.3.1 Floating

**Syntax**: [digit]+ '.' [digit]+

**Description**: Unlike other languages, exponents aren't valid. Both the fractional and whole-number parts must be present. This implies that the '.' is mandatory. Based on the syntax, leading zeros are allowed (eg, both '00.0' and '007.8' are legal).

**Semantics**: Both fractional and whole-number components are computed base 10.

#### 1.3.2 Integer

**Syntax**: [digit]+

**Description**: An integer contains only digits, no exponents, periods, or conversions to octal or hexademical are allowed.

**Semantics**: The value is computed base 10.

### 1.4 String Literal

**Syntax**: '"' .*? '"'

**Description**: Any sequence of non-quotation characters. This means that escape sequences or quotations aren't allowed within a string.

### 1.5 Operators

The following operators are valid:

`[` `]` `(` `)` `.` *(subscript, function call, member)*<br />
`+` `-` `*` `/` `%` `^` *(arithmetic)* <br />
`=` *(assignment)* <br />
`==` `!=` `>` `<` `>=` `<=` *(relational)* <br />
`&&` `||` `!` *(logical)* <br />

**Constraints**: The operators `[` `]` and `(` `)` must appear in pairs.

**Description**: An operator will cause an operation is to be performed or evaluated that results in a value or some side effect.

### 1.6 Punctuators

The following punctuators are valid:

`[` `]` *(creating arrays)* <br />
`(` `)` *(if stmts)* <br />
`{` `}` *(start and end of block)* <br />
`;` *(end of statement)*

**Constraints**: The punctuators `[` `]`, `(` `)`, and `{` `}` must appear in pairs. 

**Description**: Punctuators don't specify an operation to be performed that would result in a value. They can be thoughts of as separators. Depending on context, some symbols may appear as both punctuators and operators.

## 2 Conversions

### 2.1 Implicit

Implicit conversions happen automatically for arithmetic operands in the form of type promotions. 
Specifically, `int` can be promoted to `float` if the higher type appears in the expression.

### 2.2 Explicit

Casts are not supported in Bluefin (atm).

## 3 Expressions

An expression is a series of operators and operands that can be computed to a value or as a function call that returns void.
There are several kinds of expressions: 
- primary 
- postfix
- unary operator
- binary operator 

### 3.1 Primary expressions

**Syntax**: (Identifier | Number | String Literal | (expr))

**Description**: These are the building blocks for more complex expressions

### 3.2 Postfix operator expressions

**Syntax**: 
*primary-expr* <br />
*postfix-expr* `[` *expr* `]` <br />
*postfix-expr* `(` *argument-expr-list* `)` <br />
*postfix-expr* `.` *identifier*

**Array subscripting**: TODO

**Function calls**: the number of arguments must match the number of parameters

**Structure members**: The first operand of the `.` operator must be a `struct` type and the second operand must be a member of that type

### 3.3 Unary operator expressions

The following are unary operators:

`-`, `!`

**Constraints**: the operand of `-` must be a number type; the operand of `!` must be a bool type.

### 3.4 Binary operator expressions
 
#### 3.4.1 Multiplicative and additive 

**Syntax**: <br />
*add-expr* (+ | -) *mult-expr* <br />
*mult-expr* (* | / | %) *primary-expr* 

**Constraints**: Each operand must be of number type. The operands of `%` must be integer type. It cannot be float.

**Semantics**: Division by zero is undefined behaviour. For division, both number-type operands are promoted to the largest type between the two. If they are both integers, then rounding will occur after the division.

#### 3.4.2 Numerical Relational 

**Syntax**: <br />
*rel-expr* (< | > | <= | >=) *add-expr*

**Constraints**: Both operands must be number types. Bool are not allowed.

#### 3.4.3 Equality

**Syntax**: <br />
*equality* (== | !=) *rel-expr*

**Constraints**: Both operands must be bool types or a number type. Promotion for numbers are allowed.

#### 3.4.4 Logical AND and OR

**Syntax**: <br />
*expr* && *expr* <br />
*expr* || *logical-and* 

#### 3.4.5 Assignment

**Syntax**: <br />
*unary-expr* = *assign-expr*

**Constraints**: The lhs must be an identifier of the same type as the rhs. 

**Description**: Only simple assignments are allowed. Comma-operator assignments are not allowed

## 4 Declarations & Definitions

Declarations in Bluefin are much simpler than in other languages. 

### 4.1 Function Definition

**Syntax**: *type* *identifier* ( *paramList* ) *block*

**Constraints**: Function definition must occur with the prototype. Declarations aren't allowed

### 4.2 Struct Definition

**Syntax**: struct *identifier* { *struct-decl-list* };

**Constraints**: Definition must occur with declaration. Currently, initialization with `s = {...}` isn't allowed.

### 4.3 Variables Declaration and Definition

**Syntax**: *type* *identifier* [= *expr*]?

**Constraints**: The type can be user-defined or built-in. LHS must be of the same type as RHS. 

**Semantics**: A variable declaration (without the `= *expr*) will be assigned a default value of 0 or false if it's a built-in type. 
If the variable is a struct, then all member will be set to 0 or false, and recursively for member structs.
 



