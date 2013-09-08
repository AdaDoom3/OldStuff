// "../Debug/Parsing_Debug.c" version 0.0
// gcc -std=c99 "Parsing_Debug.c" -o "Parsing_Debug.exe"

  #include <stdio.h>
  // #include "../Core/Parsing.h"
  #include "Parsing.h"

  int main(
    void
  ){
    char     Test_String[512];
    int      Test_Integer;
    long int Test_Long;
    float    Test_Float;
    
    // Initialize_Parser debugging 
    Struct_Parser *Parser = Initialize_Parser("Parsing_Debug.txt", "//", 512);
    printf("Initialize_Parser:\n");
    if(Parser == NULL)
      goto Error_Test_Failed;
    printf("  Input test('!'): %c\n", fgetc(Parser->Input));
    printf("  Comment_Prefix('//'): %s\n", Parser->Comment_Prefix);
    printf("  Buffer(''): %s\n", Parser->Buffer);
    printf("  Buffer_Size(512): %d\n", Parser->Buffer_Size);
    printf("  Buffer_Index(0): %d\n", Parser->Buffer_Index);
    printf("  Skipped_Lines(0): %d\n", Parser->Skipped_Lines);
    printf("  Test SUCCESS!\n");
    
    // Fetch_Clear_Line and Skip_Whitespace debugging 
    printf("Fetch_Clear_Line/Skip_Whitespace:\n");
    if(!Fetch_Clear_Line(Parser))
      goto Error_Test_Failed;
    printf("  Line taken: %s\n", &Parser->Buffer[Parser->Buffer_Index]);
    Fetch_Clear_Line(Parser);
    
    // Next_String debugging 
    printf("Next_String:\n  Parsed strings: (");
    for(int i = 0;i < 23;i++){
      if(!Next_String(Parser, Test_String)){
        printf(" ... !\n");
        goto Error_Test_Failed;
      }
      printf("%s[%d]", Test_String, Parser->Skipped_Lines);
    }
    printf(")\n");
    
    // Next_Number debugging 
    printf("Next_Number:\n  Parsed floats: (");
    for(int i = 0;i < 20;i++){
      if(!Next_Number(Parser, Test_Float, FLOAT))
        printf("_");
      else
        printf("%f", Test_Float); 
      printf("[%d]", Parser->Skipped_Lines);
    }
    printf(")\n");
    printf("  Parsed Integers: (");
    for(int i = 0;i < 22;i++){
      if(!Next_Number(Parser, Test_Long, LONG_SAFE))
        printf("_");
      else
        printf("%d", Test_Long); 
      printf("[%d]", Parser->Skipped_Lines);
    }
    printf(")\n");
    
    // Next_Set debugging 
    printf("Next_Set:\n  Parsed set: ");
    if(!Next_Set(Parser, Test_String, "!*====>", "<====*!")){
      printf("! \n");
      goto Error_Test_Failed;
    }
    printf("%s\n  Test SUCCESS!\n", Test_String);
    
    // Is_Next_Match debugging 
    printf("Is_Next_Match :\n  Case 1: ");
    if(!Is_Next_Match(Parser, "This should match...")){
      printf("! \n");
      goto Error_Test_Failed;
    }
    printf("SUCCESS!\n  Case 2: ");
    if(!Is_Next_Match(Parser, "The string here is supposed to match!")){
      printf("! \n");
      goto Error_Test_Failed;
    }
    printf("SUCCESS!\n  Case 3: ");
    if(!Is_Next_Match(Parser, "(){}")){
      printf("! \n");
      goto Error_Test_Failed;
    }
    printf("SUCCESS!\n  Case 4: ");
    if(Is_Next_Match(Parser, "   JSKL:DJF *(PQ@J#JIJDK:SJFK")){
      printf("! \n");
      goto Error_Test_Failed;
    }
    printf("SUCCESS!\n");
    
    // Finalize
    Finalize_Parser(Parser);
    return SUCCESS;
  Error_Test_Failed:
    printf("  Test FAILED.\n");
    return FAILURE;
  }

// Input (Parsing_Debug.txt)
/*
!//Comment only line (Ignore any following messages denoting success)
     //Another comment only line except this one is a little spaced out (Again, ignore any following messages denoting success)
The test of Fetch_Clear_Line and Skip_Whitespace was a SUCCESS! //Actually if it was this comment would be removed...
A B C DDDDDD EE F G //ShouldntSeeMe!
H II J KKK L M N O //ShouldntSeeMe!
     189 2 3 ! 3 50 9 Z  //ShouldntSeeMe!
      1 22 333 4444 8908 1.987 12465.991 -1332 -1.2343 -0 1.
- -. 1..0 
321 432 543 6 1 8987 981.0 +123213 -234 -0 1903
123
643
+54
!*====>The test of Next_Set 
is
 a
 SUCCESS
!<====*!
This should match...
          The
                              stringhere
is  supposedto match


!
(    ) {  }
*/
