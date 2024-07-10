/***********************************************************/
// Christopher Prol
// Prof. Ndjatou
// Programming Languages
// 5/9/2024

/*************   Lab 2  -- Recursive Descent Parser ***************************/
//              scanner lab code is also included

#include <iostream>
#include <ctype.h>
#include <stdio.h>
#include <cstring>
#include <iomanip>

using namespace std;

char tokenBuffer[80];

enum tokenType
    {
    AND=0, BEGIN, END, FOR, IF, NOT,
    OR, READ,WHILE, WRITE, COMMENT, ID, REAL, STRING, PLUS, MULTIPLICATION,
    ASSIGNMENT, EQUAL, GREATERTHAN, LESSTHAN, LEFTP, COMMA,  RIGHTP, SEMICOLON,
    DIVISION, INTEGER, INVALID, ENDFILE=EOF
    };
bool needToken = true;

/********** Function Declarations ***************/
void clearBuffer();// clears token buffer
void skipSpaces();// skips whitespace
void displayToken(); // outputs tokens and lexemes
/********* Finite state functions **************/
tokenType getId();  
tokenType getStrings();
tokenType getReal();
tokenType getComment();
tokenType getAssign();
tokenType getPlus();
tokenType getMul();
tokenType getEqual();
tokenType getGreater();
tokenType getLess();
tokenType getLP();
tokenType getRP();
tokenType getComma();
tokenType getSColon();
//
tokenType lexical_error(); // tests for lexical errors
tokenType scanner();    // scanner 
/********* PARSER (lab2) FUNCTION Declarations *********/
//Parsing Functions
void program();
void stmtlst();
void stmt();
void stmtTail();
void expression();
void expressionTail();
void factor();
void factorTail();
void primary();
void parameter();
void expressionList();
void expressionListTail();
void idList();
void idTail();
//
void syntaxError(int token); // outputs Syntax error info and ends program
int peekToken(); // peeks at next token
int readToken(); // reads token 
int getNextToken(); // gets the next token in the stream
void match(); // matches token with correct code


/************ SKIP SPACES ****************/
void skipSpaces(){
    int ch;
    ch = cin.get();
    while(isspace(ch))
        ch = cin.get();
    cin.putback(ch);
}
/************ CLEAR BUFFER ****************/
void clearBuffer() {
    for (int i = 0; i < 80; i++)
        tokenBuffer[i] = '\0'; // Set each element to the null character
}
/************ DISPLAY TOKENS ****************/
void displayToken(tokenType code){
    static char message[][20] =
        {
        "and", "begin", "end", "for",
        "if", "not", "or", "read", "while", "write",
        "comment", "identifier", "real constant",
        "string", "plus", "multiplication", "assignment",
        "equal", "greater than", "less than", "left parenthesis",
        "comma", "right parenthesis", "semicolon", "division",
        "integer", "invalid"
        };
    int ch = code;
    cout << left << setw(40) << message[ch] << tokenBuffer << endl;

    }
/************ FINITE MACHINES ****************/
// identifiers + Reserved Words
tokenType getId() {
  static char reservedWord[][10] = {  // Use double quotes for string literals
    "AND", "BEGIN", "END", "FOR", "IF", "NOT",
    "OR", "READ", "WHILE", "WRITE"
  };
  tokenType code;
  int ch, i = 0;

  ch = cin.get();
  if (isalpha(ch)) {
    tokenBuffer[i++] = ch;
    ch = cin.get();
    while (isalnum(ch)) {
      tokenBuffer[i++] = ch;
      ch = cin.get();
    }
    cin.putback(ch);

    // Binary search for reserved word
    int first = 0, last = 9, mid;
    bool notFound = true;
    while (first <= last && notFound) {
      mid = (first + last) / 2;
      int answer = strcmp(tokenBuffer, reservedWord[mid]);
      if (answer == 0) {
        notFound = false;
        code = static_cast<tokenType>(mid);  // Explicit cast to tokenType
      } else if (answer > 0) {
        first = mid + 1;
      } else {
        last = mid - 1;
      }
    }
    if (notFound) {
      code = ID;
    }
  } else {
    cin.putback(ch);
    code = INVALID;
  }

  return code;
}
// strings
tokenType getStrings(){
	tokenType code;
	int   ch, i = 0;
	ch  =  cin.get(  );
	if(  ch == '\"')
	{
		tokenBuffer[ i++ ]  = ch;		// store the first symbol into the buffer

		ch  = cin.get(  );
		while( !( ch ==  '\"' ) &&  ch != EOF)
		{
			tokenBuffer[ i++ ]  = ch;		// store the symbol into the buffer

			// read the second symbol and store it into the buffer
			ch  =  cin.get(  );
		}
		if(ch == EOF)
 			code  =  INVALID;
		else
		{
			tokenBuffer[ i++ ]  = ch;		// store the first symbol into the buffer
			code  =  STRING;
		}
	}
	else			// it is not "
 	{
		cin.putback( ch );
		code  =  INVALID;
	}
	return( code );
}
// real integer
tokenType getReal() {
  tokenType code;
  int ch, i = 0;

  ch = cin.get();  // Read first character

  if (isdigit(ch)) {
    tokenBuffer[i++] = ch;
    ch = cin.get();
    while (isdigit(ch)) {
      tokenBuffer[i++] = ch;
      ch = cin.get();
    }

    if (ch == '.') {
      tokenBuffer[i++] = ch;
      ch = cin.get();
      if (isdigit(ch)) {
        tokenBuffer[i++] = ch;
        ch = cin.get();
        while (isdigit(ch)) {
          tokenBuffer[i++] = ch;
          ch = cin.get();
        }
        cin.putback(ch);  // Putback the non-digit after the decimal
        code = REAL;
      } else {
        cin.putback(ch);  // Putback the invalid character after '.'
        code = INVALID;
      }
    } else {
      cin.putback(ch);  // Putback the non-digit after the first integer
      code = INVALID;
    }
  } else {
    cin.putback(ch);  // Putback the non-digit
    code = INVALID;
  }

  return code;
}
// comments
tokenType getComment(){
	tokenType code;
	int   ch, i = 0;
	ch  =  cin.get(  );
	if(  ch == '/'  &&  cin.peek( ) == '*')
	{
		tokenBuffer[ i++ ]  = ch;		// store the first symbol into the buffer

		// read the second symbol and store it into the buffer
		ch  =  cin.get(  );
		tokenBuffer[ i++ ]  = ch;

		ch  = cin.get(  );
		while( !( ch ==  '*'  &&  cin.peek( ) == '/' )  &&   cin.peek( ) != EOF)
		{
			tokenBuffer[ i++ ]  = ch;		// store the first symbol into the buffer

			// read the second symbol and store it into the buffer
			ch  =  cin.get(  );
		}
		if(cin.peek( ) == EOF)
 			code  =  INVALID;
		else
		{
			tokenBuffer[ i++ ]  = ch;		// store the first symbol into the buffer

			// read the second symbol and store it into the buffer
			ch  =  cin.get(  );
			tokenBuffer[ i++ ]  = ch;
			code  =  COMMENT;
		}
	}
	else			// it is not “/*”
 	{
		cin.putback( ch );
		code  =  INVALID;
	}
	return( code );
}
// assignment
tokenType getAssign() {
  tokenType code;
  int ch, i = 0;

  ch = cin.get();  // Read first character

  if (ch == ':' && cin.peek() == '=') {
    tokenBuffer[i++] = ch;      // Store the first symbol (':')
    ch = cin.get();               // Read the second character ('=')
    tokenBuffer[i] = ch;          // Store the second symbol ('=')
    code = ASSIGNMENT;
  } else {
    cin.putback(ch);              // Putback the non-assignment character
    code = INVALID;
  }

  return code;
}
// Addition
tokenType getPlus() {
  tokenType code;
  int ch, i = 0;
  ch = cin.get();
  if (ch == '+') {
    tokenBuffer[i++] = ch;      // store it into the buffer
    code = PLUS;
  } else {                         // it is not the operator +
    cin.putback(ch);
    code = INVALID;
  }
  return code;
}
// multiply
tokenType getMul() {
  tokenType code;
  int ch, i = 0;
  ch = cin.get();
  if (ch == '*') {
    tokenBuffer[i++] = ch;      // store it into the buffer
    code = MULTIPLICATION;
  } else {                         // it is not the operator
    cin.putback(ch);
    code = INVALID;
  }
  return code;
}
// equals
tokenType getEqual() {
  tokenType code;
  int ch, i = 0;
  ch = cin.get();
  if (ch == '=') {
    tokenBuffer[i++] = ch;      // store it into the buffer
    code = EQUAL;
  } else {                         // it is not the operator
    cin.putback(ch);
    code = INVALID;
  }
  return code;
}
// greater than
tokenType getGreater() {
  tokenType code;
  int ch, i = 0;
  ch = cin.get();
  if (ch == '>') {
    tokenBuffer[i++] = ch;      // store it into the buffer
    code = GREATERTHAN;
  } else {                         // it is not the operator
    cin.putback(ch);
    code = INVALID;
  }
  return code;
}
// less than
tokenType getLess() {
  tokenType code;
  int ch, i = 0;
  ch = cin.get();
  if (ch == '<') {
    tokenBuffer[i++] = ch;      // store it into the buffer
    code = LESSTHAN;
  } else {                         // it is not the operator
    cin.putback(ch);
    code = INVALID;
  }
  return code;
}
// left parethesis
tokenType getLP() {
  tokenType code;
  int ch, i = 0;
  ch = cin.get();
  if (ch == '(') {
    tokenBuffer[i++] = ch;      // store it into the buffer
    code = LEFTP;
  } else {                         // it is not the operator
    cin.putback(ch);
    code = INVALID;
  }
  return code;
}
// right parenthesis
tokenType getRP() {
  tokenType code;
  int ch, i = 0;
  ch = cin.get();
  if (ch == ')') {
    tokenBuffer[i++] = ch;      // store it into the buffer
    code = RIGHTP;
  } else {                         // it is not the operator
    cin.putback(ch);
    code = INVALID;
  }
  return code;
}
//comma
tokenType getComma() {
  tokenType code;
  int ch, i = 0;
  ch = cin.get();
  if (ch == ',') {
    tokenBuffer[i++] = ch;      // store it into the buffer
    code = COMMA;
  } else {                         // it is not the operator
    cin.putback(ch);
    code = INVALID;
  }
  return code;
}
// semicolon
tokenType getSColon() {
  tokenType code;
  int ch, i = 0;
  ch = cin.get();
  if (ch == ';') {
    tokenBuffer[i++] = ch;      // store it into the buffer
    code = SEMICOLON;
  } else {                         // it is not the operator
    cin.putback(ch);
    code = INVALID;
  }
  return code;
}
// lexical error
tokenType lexical_error(){
    int ch, i=0;
    tokenType code;
    ch = cin.get();         // read token
    tokenBuffer[i++]=ch;  // store it in the buffer
    code = INVALID;         // code is INVALID
    return code;
}

/******** Scanner *******************/
tokenType scanner(){
    skipSpaces();

    tokenType code;
    int ch, i=0;
    ch = cin.get();
    if (ch == EOF)
        code = ENDFILE;
    else{
        cin.putback(ch);

        switch (ch)
        {
        case '+':
            code = getPlus();
            break;
        case '*':
            code = getMul();
            break;
        case ':':
            code = getAssign();
            break;
        case '=':
            code = getEqual();
            break;
        case '>':
            code=getGreater();
            break;
        case '<':
            code = getLess();
            break;
        case '(':
            code = getLP();
            break;
        case ',':
            code = getComma();
            break;
        case ')':
            code = getRP();
            break;
        case ';':
            code = getSColon();
            break;
        case '/':
            code = getComment();
            break;
        case '\"':
            code = getStrings();
            break;
        default:
            ch = cin.peek();
            if (isdigit(ch))
                code = getReal();
            else if (isalpha(ch))
                code = getId();
            else
                code = lexical_error();

        }
    }
    return code;
}
/********** Lab 2 -- Recursive Descent Parser     ********/

int getNextToken(){
    static int nextToken;
    if (needToken == true )
    {
        nextToken = scanner();
        needToken = false;
    }
    return nextToken;
}
/********* Peek Token ********/
int peekToken(){
return getNextToken();
}
/************ READ TOKEN ****************/
int readToken(){
int token;
token = getNextToken();
needToken = true;
return token;
}
/************ MATCH TOKEN ****************/
void match(int token)
{
    int tokenComp;
    tokenComp = readToken();
    if (tokenComp == token) {
        cout << tokenBuffer;

        if (tokenComp == SEMICOLON) {
            cout << endl;
        }
    } else {
        syntaxError(token);
    }
    clearBuffer();
}
/************ SYNTAX ERROR ****************/
void syntaxError(int token)
{
	cout << "\nSyntaxError: There is an issue with the following Token:\t" << tokenBuffer << endl;
	exit(1);
    if (token == EOF)
        cout << "End of file reached. Program terminating.";
}
/************** Parsing Functions ********************/
void program(){
    match(BEGIN);
    stmtlst();
    match(END);
}

void stmtlst(){
	int nexttoken;
	nexttoken = peekToken();
	switch(ID)
	{
		case ID:
		case WRITE:
        case READ:
			stmt();
			stmtTail();
			break;
		case END:
			return;
		default:
			syntaxError(nexttoken);
	}
}

void stmt() {
    int nexttoken;
    nexttoken = peekToken();
    switch(nexttoken) {
        case ID:
            // Handle ID := <Expression>;
            match(ID);
            match(ASSIGNMENT);
            expression();
            match(SEMICOLON);
            break;
        case WRITE:
            // Handle WRITE (<Parameter>);
            match(WRITE);
            match(LEFTP);
            parameter();
            match(RIGHTP);
            match(SEMICOLON);
            break;
        case READ:
            // Handle READ (<IDlist>);
            match(READ);
            match(LEFTP);
            idList();
            match(RIGHTP);
            match(SEMICOLON);
            break;
        default:
            syntaxError(nexttoken);
    }
}

void stmtTail() {
    int nexttoken;
    nexttoken = peekToken();
    switch(nexttoken) {
        case ID:
        case WRITE:
        case READ:
            stmt();
            stmtTail();
            break;
        case END:
            return;
        default:
            syntaxError(nexttoken);
    }
}

void expression() {
    factor();
    expressionTail();
}

void expressionTail() {
    int nexttoken;
    nexttoken = peekToken();
    switch(nexttoken) {
        case PLUS:
            match(PLUS);
            factor();
            expressionTail();
            break;
        case SEMICOLON:
        case RIGHTP:
            return;
        default:
            syntaxError(nexttoken);
    }
}

void factor() {
    primary();
    factorTail();
}

void factorTail() {
    int nexttoken;
    nexttoken = peekToken();
    switch(nexttoken) {
        case MULTIPLICATION:
            match(MULTIPLICATION);
            primary();
            factorTail();
            break;
        case DIVISION:
            match(DIVISION);
            primary();
            factorTail();
            break;
        case COMMA:
            match(COMMA);
            primary();
            factorTail();
		break;
        case RIGHTP:
        case SEMICOLON:
        case PLUS:
            return;
        default:
            syntaxError(nexttoken);
    }
}

void primary() {
    int nexttoken;
    nexttoken = peekToken();
    switch(nexttoken) {
        case LEFTP:
            match(LEFTP);
            expression();
            match(RIGHTP);
            break;
        case ID:
        case REAL:
        case INTEGER:
            match(nexttoken);
            break;
        default:
            syntaxError(nexttoken);
    }
}

void parameter() {
    int nexttoken;
    nexttoken = peekToken();
    switch(nexttoken) {
        case STRING:
            match(STRING);
            break;
        case ID:
        case REAL:
        case INTEGER:
            expressionList();
            break;
        default:
            syntaxError(nexttoken);
    }
}

void expressionList() {
    expression();
    expressionListTail();
}

void expressionListTail() {
    int nexttoken;
    nexttoken = peekToken();
    switch(nexttoken) {
        case COMMA:
            match(COMMA);
            expression();
            expressionListTail();
            break;
        case RIGHTP:
            return;
        default:

            syntaxError(nexttoken);
    }
}

void idList() {
    match(ID);
    idTail();
}

void idTail() {
    int nexttoken;
    nexttoken = peekToken();
    switch(nexttoken) {
        case COMMA:
            match(COMMA);
            match(ID);
            idTail();
            break;
        case RIGHTP:
            return;
        default:
            syntaxError(nexttoken);
    }
}
/********** MAIN *****************/
int main()
{
    program();
    return 0;
}




