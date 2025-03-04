# SimpleCompiler
This is a simple compiler that reads a program written in a small, custom language, validates its syntax and some semantics, and generates output

A recursive descent parser is a top-down parser that works by:
	1) Breaking the grammar of the language into smaller rules.
	2) Using recursive functions to process each rule, starting with the root of the grammar and descending into its components.


# Grammer:
	<program>       ::= { <statement> }
	<statement>     ::= <assignment> 
	                  | <if_statement>
	                  | <while_statement>
	                  | <for_statement>
	                  | <print_statement>
	
	<assignment>    ::= <variable> "=" <expression> ";"
	
	<if_statement>  ::= "if" "(" <condition> ")" "{" { <statement> } "}"
	
	<while_statement> ::= "while" "(" <condition> ")" "{" { <statement> } "}"
	
	<for_statement> ::= "for" "(" <assignment> <condition> ";" <increment> ")" "{" { <statement> } "}"
	
	<increment>     ::= <variable> "=" <expression>
	
	<print_statement> ::= "print" "(" <variable> ")" ";"
	
	<condition>     ::= <expression> <relational_operator> <expression>
	<relational_operator> ::= "<" | ">" | "<=" | ">=" | "==" | "!="
	
	<expression>    ::= <term> { ("+" | "-") <term> }
	<term>          ::= <factor> { ("*" | "/") <factor> }
	<factor>        ::= <number> 
	                  | <variable> 
	                  | "(" <expression> ")"
	
	<number>        ::= [0-9]+
	<variable>      ::= [a-zA-Z][a-zA-Z0-9]*
	
	
# Core features:
	1) Lexical Analysis: The compiler first breaks the input program into tokens using a tokenizer
			Eg: x is a Var, 2 in a NUM, ( is Paren etc
			
	2) Syntax Analysis: After tokenization, the parser checks if the sequence of tokens follows the rules of the language grammar. For example, it verifies if expressions like x = 5 + 3 are valid and builds an abstract syntax tree (AST)

	
	3) Dynamic Memory Management:
		• "Memory for tokens, symbol tables, and the AST is dynamically allocated using malloc, ensuring efficient memory usage."
		• "The program ensures no memory leaks by freeing memory after processing."
		• Demo Point: Explain memory allocation for tokens or tree nodes.

	4) File I/O:
		• "The input program is read from a file, and the output (tokens, errors, or IR) is written to another file."
		• Demo Point: Run the compiler on an input file and show the output file.

	5) Error Handling:
		• "The compiler provides detailed error messages to help the user debug their code, such as syntax errors, type mismatches, or undeclared variables."
		• Demo Point: Show a program that triggers an error and the corresponding message.
	
