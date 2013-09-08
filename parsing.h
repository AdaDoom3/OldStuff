// "../Parsing.h" version 0.0
#ifndef PARSING_H
#define PARSING_H
   
   
   
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  // #include "../Settings/Preprocessors.h" 
  // Below are the dependancies in Preprocessors.h
  #define FALSE   0
  #define TRUE    1
  #define SUCCESS TRUE
  #define FAILURE FALSE

  
  
                                                                                                        
//___________________________________________________________________________________________________________________________________________________
//
//  MACROS: Character tests
//  
//  Boolean character tests to add code readability.
//
//  ~ c
//    Character to test 
//
//  ~ SAMPLE CALL:
//    if(IS_ESCAPE(String[i]))
//      ...
//

  #define IS_CHARACTER(c)       (c>' ')
  #define IS_ESCAPE(c)          (c<'\t'||(c<' '&&c>'\t'))
  #define IS_WHITESPACE(c)      (c==' '||c=='\t')
  #define IS_DIGIT(c)           (c>='0'&&c<='9')
  #define IS_DIGIT_SIGN(c)      (c=='-'||c=='+')
  #define IS_DIGIT_SYMBOL(c)    (c=='-'||c=='.'||c=='+')
  #define IS_POSSIBLE_NUMBER(c) (IS_DIGIT(c)||IS_DIGIT_SYMBOL(c))
  
  
  
//___________________________________________________________________________________________________________________________________________________
//
//  MACRO: String trimming
//   
//  Moves the index of a string passed the leading whitespaces
//  
//  ~ s
//    String the be referenced
//
//  ~ i
//    Index of the reference string
//
//  ~ SAMPLE CALL:
//    SKIP_WHITESPACE(Parser->Buffer, Parser->Buffer_Index);
//      

  #define SKIP_WHITESPACE(s,i) for(;IS_WHITESPACE(s[i]);i++)


                                                                                                                                         
//___________________________________________________________________________________________________________________________________________________
//
//  CONSTANTS: Function pointer aliases
//  
//  Aliases for function pointer passing to make calling Next_Number more intuitive.
//
  
  #define FLOAT         &Convert_Float
  #define DOUBLE        &Convert_Double
  #define INT           &Convert_Integer
  #define LONG          &Convert_Long
  #define LONG_SAFE     &Convert_Long_Safe
  #define UNSIGNED_LONG &Convert_Long_Unsigned
  
          
          
//___________________________________________________________________________________________________________________________________________________
//
//  TYPE: Struct_Parser
//
//  Stores the internal state of a file and settings that determine how a file is parsed.
//
//  ~ Input
//    Stored file pointer used by the parser.
//
//  ~ Comment_Prefix
//    The prefix that denotes a comment for the text file being read.
//
//  ~ Skipped_Lines
//    Stores the number of lines skipped during the last operation done with the parser.
//
//  ~ Buffer_Index
//    The index of the current line in Buffer.
//
  
  typedef struct{
    FILE *Input;
    char *Comment_Prefix;
    char *Buffer;
    int  Buffer_Size;
    int  Buffer_Index;
    int  Skipped_Lines; // Important status variable
  }Struct_Parser;
  
       
       
//___________________________________________________________________________________________________________________________________________________
//
//  FUNCTION: Initialize_Parser
// 
//  Returns a parser pointer to an initialized Struct_Parser if successful. On failure it returns NULL.
//    
//  ~ File_Name:
//    Name of the text file to parse.
//    
//  ~ Comment_Prefix:
//    String that signifies the start of a comment for the textfile.
//    
//  ~ Buffer_Size:
//    Size of internal string storage for the parser and as such also the limitation on the length of parsed lines.
//    
//  ~ SAMPLE CALL:
//    Parser = Initialize_Parser("My_Test.txt", "//", 512);
//    if(Parser == NULL)
//      ...error handle...
//     
      
  static inline Struct_Parser *Initialize_Parser(
    const char *File_Name,
    const char *Comment_Prefix,
    const int  Buffer_Size
  ){
    // Open the file pointer, if there is an error return a NULL Struct_Parser pointer 
    FILE *Input = fopen(File_Name, "r");
    if(Input == NULL)
      return NULL;
      
    // Allocate and initialize the parser and its members
    Struct_Parser *Parser = (Struct_Parser *)malloc(sizeof(Struct_Parser));
    Parser->Input = Input;
    Parser->Comment_Prefix = Comment_Prefix;
    Parser->Buffer = (char *)malloc(sizeof(char) * Buffer_Size);
    Parser->Buffer_Size = Buffer_Size;
    Parser->Buffer_Index = 0;
    Parser->Skipped_Lines = 0;
    Parser->Buffer[Parser->Buffer_Index] = '\0';
    return Parser;
  }
  
     
     
//___________________________________________________________________________________________________________________________________________________
//
//  FUNCTION: Finalize_Parser
//  
//  Frees a function pointer and its buffer, then sets it to NULL.
//  
//  ~ SAMPLE CALL:
//    if(!Finalize_Parser(Parser))
//      ...error handle...
//
      
  static inline int Finalize_Parser(
    Struct_Parser *Parser
  ){
    // Free the allocated buffer string inside of the parser 
    free(Parser->Buffer);
    
    // Free the parser 
    free(Parser);
    
    // Set the whole thing to null (Otherwise the pointer to an unallocated memory block remains) 
    Parser = NULL;
  }
  
                                                                                                                           
//___________________________________________________________________________________________________________________________________________________
//
//  FUNCTION: Fetch_Clear_Line
//  
//  Mostly used interally to get the next line - it clears the current parser buffer next valid line (without any comments that start with the parser
//  comment prefix) and also resets the buffer index to the first character in the next valid line. It will return failure if there are no more valid
//  lines.
//  
//  ~ NOTE ON USAGE:
//    It is inline and also rather large - please keep this in mind before using it.
//  
//  ~ SAMPLE CALL:
//    if(!Fetch_Clear_Line(Parser))
//      ...error handle...
//
      
  static inline int Fetch_Clear_Line(
    Struct_Parser *Parser
  ){
    // Used by strstr to store the possible start of a comment 
    char *Comment_Start;
    
    // Loop while taken lines are blank or comment only 
    do{
      
      // Get the next line 
      if(!fgets(Parser->Buffer, Parser->Buffer_Size, Parser->Input))
        return FAILURE;
      Parser->Skipped_Lines++;
      
      // Reset the Buffer_Index 
      Parser->Buffer_Index = 0;
        
      // If a comment is found null terminate the buffer at the comment start 
      if((Comment_Start = strstr(Parser->Buffer, Parser->Comment_Prefix)) != NULL)
        *Comment_Start = '\0';
        
      // Skip passed the beginning white spaces (this is to avoid comment only lines with leading spaces) 
      SKIP_WHITESPACE(Parser->Buffer, Parser->Buffer_Index);
    }while(IS_ESCAPE(Parser->Buffer[Parser->Buffer_Index]));
    return SUCCESS;
  }
  
      
      
//___________________________________________________________________________________________________________________________________________________
//
//  FUNCTION: Is_Next_Match
//  
//  Moves the buffer index over any characters matched from the buffer to the reference ignoring whitespaces (in both the buffer and reference).
//  If it fails (i.e the whole reference was not matched to the buffer) then the buffer index remains in its current position passed any matched
//  characters and FAILURE is returned.
//  
//  ~ Reference:
//    String denoting the next characters to match (spaces are ignored).
//    
//  ~ SAMPLE CALL:
//    if(!Is_Next_Match(Parser, "test_data{"))
//      ...error handle...
//
      
  int Is_Next_Match(
          Struct_Parser *Parser,
    const char          *Reference
  ){
    // Index used for the reference 
    int i = 0;
    
    // Loop through the reference 
    while(Reference[i] != '\0'){
      
      // Skip whitespaces in reference 
      SKIP_WHITESPACE(Reference, i);
      
      // Compare the characters 
      while(IS_CHARACTER(Reference[i])){
        
        // Skip whitespaces in the buffer
        SKIP_WHITESPACE(Parser->Buffer, Parser->Buffer_Index);
        
        // If input is at the end of the line fetch a new one 
        if(IS_ESCAPE(Parser->Buffer[Parser->Buffer_Index]) && !Fetch_Clear_Line(Parser))
          return FAILURE;
          
        // Compare the characters and increment the indexes for the reference and buffer 
        if(Parser->Buffer[Parser->Buffer_Index] != Reference[i++])
          return FAILURE;
        Parser->Buffer_Index++;
      }
    }
    return SUCCESS;
  }
  
  
                                                                                                                  
//___________________________________________________________________________________________________________________________________________________
//
//  FUNCTION: Next_Set
//  
//  Puts the next string in the buffer enclosed by the left and right delimiter into the target. If the string at the current buffer position does
//  not start with a left delimiter it return FAILURE - it also returns FAILURE if no right delimiter was found afterward. The value in the target
//  has the delimiters removed and if the target size does not fit the found string it will set just the values that do and return SUCCESS.
//  
//  ~ Target:
//    String pointer to place the found set.
//    
//  ~ Delimiter_Left/Delimiter_Right:
//    Starting and ending strings of the set. Example: "\"" and "\"" sent as delimiters would find the next string.
//    
//  ~ SAMPLE CALL:
//    if(!Next_Set(Parser, Test_String, "!*", "*!"))
//      ...error handle...
//
  
  #define Next_Set(p,t,dl,dr) Next_Set_(p,t,sizeof(t),dl,dr) // Get rid of that sizeof
  int Next_Set_(
          Struct_Parser *Parser,
          char          *Target,
    const int           Target_Size, // Target_Size is handled by the macro above, so ignore it
    const char          *Delimiter_Left,
    const char          *Delimiter_Right
  ){
    // Index for Target 
    int i = 0;

    // Precalculated length storage of the Delimiters 
    int Length = strlen(Delimiter_Left);

    // Reset Skipped_Lines 
    Parser->Skipped_Lines = 0;
    SKIP_WHITESPACE(Parser->Buffer, Parser->Buffer_Index);

    // If we are at the end of the line get the next 
    if(IS_ESCAPE(Parser->Buffer[Parser->Buffer_Index]) && !Fetch_Clear_Line(Parser))
      return FAILURE;

    // If the next string does not start the set return FAILURE 
    if(strncmp(&Parser->Buffer[Parser->Buffer_Index], Delimiter_Left, Length) != 0)
      return FAILURE;

    // Move the Buffer_Index over 
    Parser->Buffer_Index += Length;
    Length = strlen(Delimiter_Right);

    // Copy the appropriate buffer values into Target (moving the Buffer_Index as we go) 
    do{
      Target[i++] = Parser->Buffer[Parser->Buffer_Index++];

      // Replace all tabs with a white space 
      if(Target[i] == '\t')
        Target[i] = ' ';

      // Get the next line if the end of the current one has been reached 
      if(IS_ESCAPE(Parser->Buffer[Parser->Buffer_Index])){
        if(!Fetch_Clear_Line(Parser))
          return FAILURE;
          
        // In Fetch_Clear_Line the whitespaces are skipped for testing blank lines, here the index is restored so we can put any leading spaces 
        // into the set 
        Parser->Buffer_Index = 0;
      }
    }while(strncmp(&Parser->Buffer[Parser->Buffer_Index], Delimiter_Right, Length) != 0 && i < Target_Size);

    // Terminate the string and return 
    Parser->Buffer_Index += Length;
    Target[i] = '\0';
    return SUCCESS;
  }
  
  
                                                                                                                               
//___________________________________________________________________________________________________________________________________________________
//
//  FUNCTION: Next_String
//  
//  Grabs the next valid string from input into the target. It uses whitespace to deliniate between strings (effectivly words or tokens). If the
//  target size does not fit the found string it will set just the values that do and return SUCCESS. It will return FAILURE when the end of the
//  input has been reached.
//  
//  ~ Target:
//    String pointer to place the found string.
//    
//  ~ SAMPLE CALL:
//    if(!Next_String(Parser, Test_String)
//      ...error handle...
//
     
  #define Next_String(p,t) Next_String_(p,t,sizeof(t)) // Get rid of that sizeof
  int Next_String_(
          Struct_Parser *Parser,
          char          *Target,
    const int           Target_Size // Target_Size is handled by the macro above, so ignore it
  ){
    // Index for Target 
    int i = 0;
    
    // Reset Skipped_Lines 
    Parser->Skipped_Lines = 0;
    SKIP_WHITESPACE(Parser->Buffer, Parser->Buffer_Index);
    
    // If we are at the end of the line get the next 
    if(IS_ESCAPE(Parser->Buffer[Parser->Buffer_Index]) && !Fetch_Clear_Line(Parser))
      return FAILURE;
        
    // Copy the appropriate buffer values into Target (moving the Buffer_Index as we go) 
    do
      Target[i++] = Parser->Buffer[Parser->Buffer_Index++];
    while(IS_CHARACTER(Parser->Buffer[Parser->Buffer_Index]) && i < Target_Size);
    
    // Terminate the string and return 
    Target[i] = '\0';
    return SUCCESS;
  }
  
     
     
//___________________________________________________________________________________________________________________________________________________
//
//  FUNCTION: Next_Number
//  
//  Takes any characters that could be part of a number from input, tests to see if is a valid number, then converts the number string using the
//  passed function pointer (refer to the function pointer constants near the top of the header for possible conversion routines). Faliure can occur
//  from attemping to parse a non-valid number or no number digits found in the input - if the function fails, any characters taken of the non-valid
//  number remain taken.
//  
//  ~ Target:
//    A number (decimal or integer) to place the converted number.
//    
//  ~ Convert:
//    Function pointer that preforms the string to number conversion.
//    
//  ~ SAMPLE CALL:
//    if(!Next_Number(Parser, Test_Float, FLOAT))
//      ...error handle...
//    
//  ~ NOTE:
//    If the number type of the target is not of the type the conversion function assumes, bad things will happen.
//
   
  #define Next_Number(p,t,f) Next_Number_(p,&t,f) // A little something I call pass by reference
  int Next_Number_(
    Struct_Parser *Parser,
    void          *Target, // The macro above takes care of the pointer type casting, so send a regular variable
    void (*Convert)(
            void *Target,
      const char *String
    )
  ){
    // Stores the buffer index of the start of a possible number 
    int  Number_Start;
    
    // Used by buffer for swaping values and storing a decimal count in the buffer check 
    char Temporary = 0;
    
    // Reset Skipped_Lines 
    Parser->Skipped_Lines = 0;
    SKIP_WHITESPACE(Parser->Buffer, Parser->Buffer_Index);
    
    // If we are at the end of the line get the next 
    if(IS_ESCAPE(Parser->Buffer[Parser->Buffer_Index]) && !Fetch_Clear_Line(Parser))
      return FAILURE;
    
    // Set the number starting position 
    Number_Start = Parser->Buffer_Index;
    
    // Fetch characters from input that are valid digits or digit symbols 
    for(;IS_POSSIBLE_NUMBER(Parser->Buffer[Parser->Buffer_Index]);Parser->Buffer_Index++);
      
    // Test for non-valid digit-symbol combinations 
    switch(Parser->Buffer_Index - 1 - Number_Start){
    
      // '' 
      case -1:
        return FAILURE;
      
      // '+', '-', '.' 
      case 0:
        if(IS_DIGIT_SYMBOL(Parser->Buffer[Number_Start]))
          return FAILURE;
        break;
      
      // '+X', '-X', '.X', 'X+', 'X-', 'X.' 
      case 1:
        if(IS_DIGIT_SYMBOL(Parser->Buffer[Number_Start]) && IS_DIGIT_SYMBOL(Parser->Buffer[Number_Start + 1]))
          return FAILURE;
        break;
      
      // Any other combination 
      default: 
      
        // Loop through the buffer section that contains the possible number 
        for(int i = Number_Start;i < Parser->Buffer_Index;i++){
        
          // Testing for non-valid digit-symbol combinations (where 'X' is a digit): 'X..', 'X+', 'X-' 
          if(Temporary > 1 || (i != Number_Start && IS_DIGIT_SIGN(Parser->Buffer[i])))
            return FAILURE;
            
          // Increment the decimal count if one is found 
          if(Parser->Buffer[i] == '.')
            Temporary++;
        }
    }
    
    // Null terminate the buffer passed the end of the number (preserving its previous value in Temporary) 
    Temporary = Parser->Buffer[Parser->Buffer_Index];
    Parser->Buffer[Parser->Buffer_Index] = '\0';
    
    // Convert the portion of the buffer with the valid number to the target using the passed conversion function 
    Convert(Target, &Parser->Buffer[Number_Start]);
    Parser->Buffer[Parser->Buffer_Index] = Temporary;
    return SUCCESS;
  }
  
  
                                                                                                                              
//___________________________________________________________________________________________________________________________________________________
//
//  FUNCTION: Convert_"Number type"    
//  
//  These should only be used internally! However, if you are feeling adventurous and want to create a new convert function...
//  A convert functions MUST follow the same prototype for parameters, that is, a convert function must take a void * to a number and a const char *
//  to a string that holds the unconverted number.
//  
//  ~ Target:
//    A number pointer (decimal or integer) to place the converted number.
//    
//  ~ String:
//    A string containing the VALID decimal number (I stress valid here because it will have been excessivly tested)
//    
//  ~ NOTE:
//    If the number type of the target is not of the type the conversion function assumes, bad things will happen.
//    
//  ~ IMPLEMENTATION OF A CONVERT FUNCTION:
//    #define SHORT &Convert_Short // Function pointer alias
//    static inline void Convert_Short(
//            void *Target,
//      const char *String
//    ){
//      // Typecast the target void pointer to a short integer * and use atoi for conversion of the string
//      *(short int *)Target = (short int)atoi(String);
//    }
//

  static inline void Convert_Float(
          void *Target,
    const char *String
  ){
    // Typecast the target void pointer to a float * and use atoi for conversion of the string 
    *(float *)Target = atof(String);
  }
  
  static inline void Convert_Double(
          void *Target,
    const char *String
  ){
    // Typecast the target void pointer to a double * and use strtod for conversion of the string 
    *(double *)Target = strtod(String, NULL);
  }

  static inline void Convert_Integer(
          void *Target,
    const char *String
  ){
    // Typecast the target void pointer to a integer * and use atoi for conversion of the string 
    *(int *)Target = atoi(String);
  }
  
  static inline void Convert_Long(
          void *Target,
    const char *String
  ){
    // Typecast the target void pointer to a long integer * and use atol for conversion of the string 
    *(long int *)Target = atol(String);
  }
  
  static inline void Convert_Long_Safe(
          void *Target,
    const char *String
  ){
    // Typecast the target void pointer to a long integer * and use strtol for conversion of the string
    // the reason this is considered safer is because strtol has less undefined behavior as atol 
    *(long int *)Target = strtol(String, NULL, 10);
  }
  
  static inline void Convert_Long_Unsigned(
          void *Target,
    const char *String
  ){
    // Typecast the target void pointer to an unsigned long integer and use strtoul for conversion of the string 
    *(unsigned long int *)Target = strtoul(String, NULL, 10);
  }
  
#endif // PARSING_H
