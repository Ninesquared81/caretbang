# ^!
^! (pronounced caret-bang) is a stack-based esoteric programming language. Data is stored on two stacks, which are both initially empty. Programs in ^! are defined by a sequence of operations which modify the stacks. Any tokens not part of the syntax are ignored as if they were code comments.


### Operations
| Syntax | Effect |
|:------:|--------|
| `^` | Push zero to the main stack |
| `!` | Increment top of stack |
| `+` | Add top two elements of stack |
| `-` | Subtract top element from the next element |
| `*` | Pop from stack and discard the result |
| `:` | Duplicate top element of stack |
| `,` | Read a character from `stdin` and push to stack |
| `.` | Pop from stack and print to `stdout` as a character |
| `%` | Swap top two elements of stack |
| `@` | Rotate top three elements of the stack, like so: `...c b a` &rarr; `...b a c` |
| `>` | Move element from top of main stack to top of auxiliary stack |
| `<` | Move element from top of auxiliary stack to top of main stack |
| `?` | Push `1` to main stack if it contains elements, or `0` if it is empty |
| `;` | Push `1` to main stack if auxiliary stack has elements, or `0` if it is empty |
| `[` _\<loop body>_ `]` | Loop while top of stack is non-zero (pops from stack to check condition) |
| `{` _\<macro name>_ `}(` _\<macro body>_ `)` | Define macro |
| `{` _\<macro name>_ `}` | Execute macro |

### The two stacks
As previously mentioned, data is stored on two stacks. The elements of these stacks are unisgned 8-bit integers, which can be used both as numbers and ASCII characters. The main stack is where most modifications take place. There is also the auxiliary stack, which can only be pushed to and popped from (the element coming from/going to the main stack). It is useful for holding higher elements of the main stack while lower elements are being used.

### Macros
A powerful feature of ^! is macros. These allow a sequence of instructions to be given a shorter name (which can be any string not containing the characters `{` or `}`) and be called again later. When a macro is called in the code, the instruction pointer jumps to the body of the macro, and jumps back to where it came from when finished. This means that macros can be recursive, although it is important to note that the state (of the stacks) is inherited by any recursive call.
