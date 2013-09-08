###############
## Libraries ##
###############

import os
import sys
import re

###############
## Constants ##
###############

# Reasons why regular expressions are a bad idea:
# function calls with bitwise compliments (~) directly next to the call will not be matched due to destructor syntax

PATH_FOR_INPUT      = "\\Users\\kronos\\programs\\python\\c_reference_tool\\doom3\\neo"
PATH_FOR_OUTPUT     = "\\Users\\kronos\\programs\\python\\c_reference_tool\\output.csv"
SOURCE_EXTENSIONS   = ('.h', '.cpp', '.mm')
ERROR_FUNCTION      = "Error in function: "
ERROR_FORMATTING    = "\n\t"
ERROR_PARAMETER     = ERROR_FORMATTING + "Incorrect parameter"
ERROR_IO            = ERROR_FORMATTING + "IO Error with file "
IDENTIFIER          = "[A-Z,a-z,_][A-z,a-z,_,0-9]*"
PARAMETERS          = "[&*,A-Z,a-z,_,0-9,=,\s]*"
END_TOKENS          = "[}]|[{]|[(]|[;]"
OPERATOR            = "(?:=|[+]|-|[*]|/|%|[+][+]|[-][-]|==|!=|>|<|>=|<=|~[\s]+|!|&&|[|][|]|&|[|]|^|<<|>>|[+]=|-=|[*]=|/=|%=|&=|[|]=|^=|<<=|>>=|\([\s]*"+\
IDENTIFIER          + "[\s]*\)|delete|new|"+END_TOKENS+")"
COMMENT_SINGLE      = re.compile(r"//.*\n")
COMMENT_DOUBLE      = re.compile(r"/\*.*?\*/", re.DOTALL)
PREPROCESSOR        = re.compile(r"#(?:.*\\\n)*.*$",re.MULTILINE)
FUNCTION_DEFINITION = re.compile(r"[\s]*(?:"+IDENTIFIER+"::)?("+IDENTIFIER+")[\s]*\("+PARAMETERS+"\)[\s]*(?:const)?[\s]*\{", re.MULTILINE)
FUNCTION_CALL       = re.compile(r""+OPERATOR+"[\s]*(?:"+IDENTIFIER+"::)?("+IDENTIFIER+")[\s]*\("+PARAMETERS+"\)", re.MULTILINE)
RE_TYPE             = type(re.compile('null'))

###############
## Variables ##
###############

Temporary         = ""
Index_A           = 0
Index_B           = 1
CSV_Matrix        = []
Files             = []
Defined_Functions = {}
Called_Functions  = {}
Dependancies      = {}

#################
## Subprograms ##
#################

def Walk_Directories():
  List_Of_Files = []
  for Root, Directories, Files in os.walk(PATH_FOR_INPUT):
    for File_Name in Files:
      if File_Name.endswith( SOURCE_EXTENSIONS ):
        List_Of_Files.append(os.path.join(Root, File_Name))
  return List_Of_Files
def Process_File(String_File_Name):
  assert type(String_File_Name) == str
  try:
    File = open(String_File_Name)
    Whole_File = File.read()
    Whole_File = re.sub( COMMENT_DOUBLE, '', Whole_File )
    Whole_File = re.sub( COMMENT_SINGLE, '', Whole_File )
    Whole_File = re.sub( PREPROCESSOR,   '', Whole_File )
    return Whole_File
  except IOError:
    print >> sys.stderr, ERROR_FUNCTION + "Process_File" + ERROR_IO + String_File_Name
def Extract_Functions(String_Text, RE_TYPE_Pattern):
  assert type(String_Text) == str
  assert type(RE_TYPE_Pattern) == RE_TYPE
  List_Of_Strings = re.findall(RE_TYPE_Pattern, String_Text)
  length = len(List_Of_Strings)
  index = 0
  while not index == length:
    if (List_Of_Strings[index] == 'if') or (List_Of_Strings[index] == 'switch') or (List_Of_Strings[index] == 'for') or (List_Of_Strings[index] == 'while'):
      del List_Of_Strings[index]
      length = length - 1
    else:
      index = index + 1
  return List_Of_Strings
def Increment_Key(Dictionary, Key):
  assert type(Dictionary) == dict
  assert type(Key) == str
  if not Key in Dictionary:
    Dictionary[Key] = 1
  else:
    Dictionary[Key] = Dictionary[Key] + 1
def Output_To_CSV(Matrix_Of_Values):
  try:
    assert type(Matrix_Of_Values[0]) == list
    if os.path.exists(PATH_FOR_OUTPUT):
      os.remove(PATH_FOR_OUTPUT)      
    with open(PATH_FOR_OUTPUT, 'w') as Output:
      for Line in Matrix_Of_Values:
        for Value in Line:
          Output.write(Value + ",")
        Output.write("\n")
    Output.close()
  except IndexError:
    print >> sys.stderr, ERROR_FUNCTION + "Output_To_CSV" + ERROR_PARAMETER
  except IOError:
    print >> sys.stderr, ERROR_FUNCTION + "Output_To_CSV" + ERROR_IO + PATH_FOR_OUTPUT

##########
## Main ##
##########

Files = Walk_Directories()
for File in Files:
  Called_Functions[File] = {}
  Dependancies[File] = {}
print 'Processing Files:'
for File in Files:
  print '\tProcessing File: ' + File
  Temporary = Process_File(File)
  
  # For every unique function called in the current file
  # increment the number of times it's called.
  print '\t\tExtracting Function Calls.'
  for Call in Extract_Functions(Temporary, FUNCTION_CALL):
    Increment_Key(Called_Functions[File], Call)
    
  # For every function definition,
  # index the function definition's location
  print '\t\tExtracting Function Definitions.'
  for Definition in Extract_Functions(Temporary, FUNCTION_DEFINITION):
    Defined_Functions[Definition] = File
Temporary = ""

# For every unique function call per file,
# increment the number of times this file depends on another
print 'Matching Calls to Definitions.'
for File in Files:
  for Function in Called_Functions[File]:
    if Function in Defined_Functions and not Dependancies[File] == Defined_Functions[Function]:
      Increment_Key(Dependancies[File], Defined_Functions[Function])
CSV_Matrix = [0 for I in range(0,len(Dependancies))]
print 'Formatting Output'    
for File in Dependancies:
  CSV_Matrix[Index_A] = [0 for I in range(0,len(Dependancies[File]) + 1)]
  CSV_Matrix[Index_A][0] = File[len(PATH_FOR_INPUT) + 1:len(File)]
  for Dependant_File in Dependancies[File]:
    CSV_Matrix[Index_A][Index_B] = Dependant_File[len(PATH_FOR_INPUT) + 1:len(Dependant_File)]
    Index_B = Index_B + 1
  Index_A = Index_A + 1
  Index_B = 1
print 'Writing Output'
Output_To_CSV(CSV_Matrix)
