
# Bluefin Language Standard
This standard is organized in a similar format as "Part 6: Language" of the [C89/90 Standard](https://www.pdf-archive.com/2014/10/02/ansi-iso-9899-1990-1/ansi-iso-9899-1990-1.pdf). 
Where appropriate, this standard copies descriptions verbatum from C's standard.
Since Bluefin is a hobby project, it goes without saying that this standard is a lot shorter and less detailed than C's Standard.

## 1 Key Lexical Elements

Tokens: keyword, identifier, numbers, string-literal, operator, punctuator

### 1.1 Keywords

The following keywords are reserved and not to be used as identifiers: 

`break`, `continue`, `if`, `else`, `while` (*control flow*) <br />
`int`, `float`, `string`, `void`, `bool`, `true`, `false` *(built-in types)* <br />
`struct` *(user-defined type)* <br />
`return` *(for functions)* <br />
`override` *(for methods that override superclass' method)*

### 1.2 Identifiers

**Syntax**: [a-zA-A]+

**Description**: 
An identifier is a sequence of letters from the English alphabet. Unlike other languages, it can't contain digits or underscores.

**Semantics**: 
An identifier is used to denote a variable, a function, or a tag for a struct.

**Scopes of Identifiers**: An identifier is visible (can be referred to) only if it is within its scope. There are three types of scopes:
- File scope: global scope for the whole file)
- Block scope: almost anything inside `{` ... `}`, including while loops, and nested blocks.
 The sole exception are functions, in which the body and the parameters share the same scope. 
 For functions, the block scope starts at the '(' and ends at the '}'. Note that the '{' will not create a second scope.

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

### 1.7 Whitespace and Comments

The following are whitespace: ` `, \r, \n, \t

Multi-line comments begin with /* and end with */. Comments do not nest
Single-line comments begin with // 

**Description**: Whitespaces and comments are skipped.

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

**Syntax**: <br />
*primary-expr* -> (Identifier | Number | String Literal | (expr))

**Description**: These are the building blocks for more complex expressions

### 3.2 Postfix operator expressions

**Syntax**: <br />
*postfix-expr* -> *postfix-expr* `[` *expr* `]` <br />
*postfix-expr* -> *postfix-expr* `(` *argument-expr-list* `)` <br />
*postfix-expr* -> *postfix-expr* `.` *identifier* <br />
*postfix-expr* -> *primary-expr*

**Array subscripting**: TODO

**Function calls**: the number of arguments must match the number of parameters

**Structure members**: The first operand of the `.` operator must be a `struct` type and the second operand must be a member of that type

### 3.3 Unary operator expressions

**Syntax**: <br />
*unary-expr* -> (- | !) *postfix-expr* | *postfix-expr*  

**Constraints**: the operand of `-` must be a number type; the operand of `!` must be a bool type.

**Description**: Chaining - and ! together are not allowed.

### 3.4 Binary operator expressions
 
#### 3.4.1 Multiplicative and additive 

**Syntax**: <br />
*mult-expr* -> *mult-expr* (* | / | %) *unary-expr* | *unary-expr* <br />
*add-expr* -> *add-expr* (+ | -) *mult-expr* | *mult-expr*

**Constraints**: Each operand must be of number type. The operands of `%` must be integer type. It cannot be float.

**Semantics**: Division by zero is undefined behaviour. For division, both number-type operands are promoted to the largest type between the two. If they are both integers, then rounding will occur after the division.

#### 3.4.2 Numerical Relational 

**Syntax**: <br />
*rel-expr* -> *rel-expr* (< | > | <= | >=) *add-expr* | *add-expr*

**Constraints**: Both operands must be number types. Promotion rules apply. Bool are not allowed.

**Semantics**: The return value is a bool. 

#### 3.4.3 Equality

**Syntax**: <br />
*equality-expr* -> *equality-expr* (== | !=) *rel-expr* | *rel-expr*

**Constraints**: Both operands must be bool types or a number type. Promotion for numbers are allowed.

#### 3.4.4 Logical AND and OR

**Syntax**: <br />
*logical-AND-expr* -> *logical-AND-expr* && *equality-expr* | *equality-expr* <br />
*logical-OR-expr* -> *logical-OR-expr* || *logical-AND-expr* | *logical-AND-expr*

#### 3.4.5 Assignment

**Syntax**: <br />
*assign-expr* -> *unary-expr* = *assign-expr* | *logical-OR-expr*

**Constraints**: The lhs must be an identifier of the same type as the rhs. 

**Description**: Only simple assignments are allowed. Comma-operator assignments are not allowed. This grammar suggests chaining is allowed (eg, `int a = b = c= d;`).

**Semantics**: The value of an assignment operation is that of its left operand after the assignment.

## 4 Declarations & Definitions

Declarations in Bluefin are much simpler than in other languages. Declaration and definition
must occur before usage. The only exception is in structs, which allows forward references.

### 4.1 Function Definition

**Syntax**: <br />
*funcDef* -> *type* *identifier* ( *paramList? ) (override)? *block* <br />
*paramList* -> *type* *identifier* (, *paramList*) *

**Constraints**: Function definition must occur with the prototype. Declarations alone aren't allowed.
A method must have the override keyword if it has the same name, return type, and parameter types as its superclass' method. 
Otherwise, it must not use the keyword.

### 4.2 Struct Definition

**Syntax**: <br />
*structDef* -> struct *identifier*  (extends parentStruct) ? { *(varDecl | funcDef)* * }; <br />

**Note**: A struct in Bluefin is equivalent to a class in other languages. TODO: change 'struct' to 'class' 
A struct can inherit from a single parent struct, in which it is able to use all the parent's and grandparent's fields and methods
Name hiding occurs when a derived struct has members with the same name as the parent struct and references will
refer to the member in the derived struct.

In addition, a struct's members and methods allow forward referencing. Eg, a method may contain reference
to a member variable that's declared later in the struct. However, if the referred variable isn't a struct member 
(eg, a local variable), then the forward reference is illegal.

**Constraints**: Definition must occur with declaration. Currently, initialization with `s = {...}` isn't allowed.
A struct can inherit from at most one other struct.

### 4.3 Variables Declaration and Definition

**Syntax**: <br />
*varDecl* -> *type* *identifier* [= *expr*]? ';'

**Constraints**: The type can be user-defined or built-in. LHS must be of the same type as RHS. 

**Semantics**: A variable declaration (without the `= *expr*) will be assigned a default value of 0 or false if it's a built-in type. 
If the variable is a struct, then all member will be set to 0 or false, and recursively for member structs.
 
## 5 Statements

A statement specifies an action to be performed. It can either contain expressions or are themselves expressions. There are four types of statements

### 5.1 Block

**Syntax**: { *statement-list* }

**Description**: Blocks contain declarations or other statements, grouping them into one unit. A block also represents a scope.

### 5.2 Expression statement

**Syntax**: *expr* ;

As its name implies, an expression statement is literally an expression followed by a semicolon.
The most important ones are assignment and function calls.

### 5.3 If statement

**Syntax**: if ( *expr* ) { *statments* } [ else { *statement* } ]

**Semantics**: The expression must evaluate to a bool type. 
To solve the 'dangling else' problem, each else is associated with its closest if. 

### 5.4 While statement

**Syntax**: while ( *expr* ) { *loop-body* }

**Semantics**: The expression must evaluate to a bool type.
The loop body comprises of statements and executes continuously until the expression evaluates to false.

### 5.5 Return statement

**Syntax**: return *expr*?

**Constraints**:  <br />
If a function's return type is void:
- a return statement cannot contain an expression
- a return statement is also optional

If a function's return type isn't void:
- A return statement with an expression must be included. The value of the expression is the returned value of the function. 
- In addition, return statements without an expression are prohibited

### 5.6 Break statement

**Constraints**: A break statement can appear only in a loop body.

**Semantics**: A break statement terminates execution of the smallest enclosing while statement (so no more loop iterations.

### 5.7 Continue statement

**Constraints**: A continue statement can appear only in a loop body.

**Semantics**: A continue statement causes the program to jump to the end of its loop body (equivalent to immediately before the }, so the loop can be executed again).

### 5.8 Declaration statement

**Syntax**: *type* ID [= expr]? ';'

**Semantics**: For now, this is just a var declaration. Unlike C89, a declaration can occur anywhere in a block and not necessarily at the top.

