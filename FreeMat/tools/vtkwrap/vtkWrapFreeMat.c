/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkWrapOctave.c,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen, Dragan Tubic
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.
  
  =========================================================================*/

/* Derived from octaviz file vtkWrapOctave.c from the octaviz project */

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "vtkParse.h"

int numberOfWrappedFunctions = 0;
FunctionInfo *wrappedFunctions[1000];
extern FunctionInfo *currentFunction;

char* typeNames[17] = { "unknown", "float", "void", "char", "int", "short", "long",
			"double", "unknown", "unknown", "unknown", "unknown", "unknown",
			"unsigned char", "unsigned int", "unsigned short", "unsigned long" };

static void emitTempScalarFromArgs(FILE *fp, int i, int j, const char *typecode)
{
  fprintf(fp,"  if ( !arg[%i].isScalar() )\n",j);
  fprintf(fp,"    throw Exception(\"Expecting a scalar argument\");\n");
  fprintf(fp,"  temp%i = (%s) arg[%i].asDouble();\n",i, typecode, j);
}

static void emitTempArrayFromArgs(FILE *fp, int i, int k, int j, const char *typecode)
{
  fprintf(fp,"  if ( !arg[%i].isScalar() )\n",j);
  fprintf(fp,"    throw Exception(\"Expecting a scalar argument\");");
  fprintf(fp,"  temp%i[%i] = (%s) arg[%i].asDouble();\n",i, k, typecode, j);
}

int isVoidType(int aType)
{
  return (((aType % 10) == 2)&&(!((aType%1000)/100)));
}

void outputFunctionHelp( FILE *fp, FileInfo *data )
{
  int i, j;
  fprintf(fp,"void populate_help_map%s( std::map<std::string,std::string> &help_map ) \n{\n",data->ClassName);
  fprintf(fp,"  help_map[\"List\"] = \"");
  for ( i = 0; i < numberOfWrappedFunctions; i++ )
    {
      fprintf(fp,"%s\\n",wrappedFunctions[i]->Name);
    }
  fprintf(fp,"\";\n");
	
  for ( i = 0; i < numberOfWrappedFunctions; i++ )
    {
      if ( wrappedFunctions[i]->Comment == NULL ) continue;
		
      fprintf(fp,"  help_map[\"%s\"] = \"", wrappedFunctions[i]->Name);
      j = 0;
      while ( wrappedFunctions[i]->Comment[j] != 0 )
	{
	  if ( wrappedFunctions[i]->Comment[j] == '"' ) fprintf(fp,"\\");
			
	  if ( wrappedFunctions[i]->Comment[j] == '\n' )
	    {
	      /* Don't want the last newline */
	      if ( j + 2 < strlen(wrappedFunctions[i]->Comment) )
		{
		  fprintf(fp,"\\n\\\n  ");
		}
	      j++;
	    } else
	    {
	      // double escape everything that is escaped;
	      // gcc doesn't like things like \$, \frac, \sum, ...
	      // in strings; luckily, this happens only in
	      // the 'Descriptions:' field
	      if (wrappedFunctions[i]->Comment[j] == '\\')
		{
		  fprintf(fp, "\\\\");
		  j++;
		}

	      fprintf(fp,"%c",wrappedFunctions[i]->Comment[j++]);
	    }
	}
      fprintf(fp,"\";\n" );
    }
  fprintf(fp,"}\n");
}

void output_temp(FILE *fp, int i, int aType, char *Id, int count)
{
  /* handle VAR FUNCTIONS */
  if (aType == 5000)
    {
      fprintf(fp,"    vtkTclVoidFuncArg *temp%i = new vtkTclVoidFuncArg;\n",i);
      return;
    }
  
  /* ignore void */
  if (((aType % 10) == 2)&&(!((aType%1000)/100)))
    {
      return;
    }

  /* for const * return types prototype with const */
  if ((i == MAX_ARGS) && (aType%2000 >= 1000))
    {
      fprintf(fp,"  const ");
    }
  else
    {
      fprintf(fp,"  ");
    }

  if ((aType%100)/10 == 1)
    {
      fprintf(fp,"unsigned ");
    }

  switch (aType%10)
    {
      /* FIXME 7 should be double but then vtkActor2D wont compile! */
      /* 7 should be double! */
    case 1:   fprintf(fp,"float  "); break;
    case 7:   fprintf(fp,"double "); break;
    case 4:   fprintf(fp,"int    "); break;
    case 5:   fprintf(fp,"short  "); break;
    case 6:   fprintf(fp,"long   "); break;
    case 2:     fprintf(fp,"void   "); break;
    case 3:     fprintf(fp,"char   "); break;
    case 9:     fprintf(fp,"%s ",Id); break;
    case 8: return;
    }

  /* handle array arguements */
  if (count > 1)
    {
      fprintf(fp,"temp%i[%i];\n",i,count);
      return;
    }
  
  switch ((aType%1000)/100)
    {
    case 1: fprintf(fp, " *"); break; /* act " &" */
    case 2: fprintf(fp, "&&"); break;
    case 3: fprintf(fp, " *"); break;
    case 4: fprintf(fp, "&*"); break;
    case 5: fprintf(fp, "*&"); break;
    case 7: fprintf(fp, "**"); break;
    default: fprintf(fp,"  "); break;
    }
  
  fprintf(fp,"temp%i",i);
  fprintf(fp,";\n");
}

/* when the cpp file doesn't have enough info use the hint file */
void use_hints(FILE *fp)
{
  int  i;
  /* use the hint */
  switch (currentFunction->ReturnType%1000)
    {
    case 301: case 307:  
    case 304: case 305: case 306: 
    case 313: case 314: case 315: case 316:
      /* float array */
      fprintf(fp,"  BasicArray<double> tempResult(NTuple(%i,1));\n",currentFunction->HintSize);
      for (i = 0; i < currentFunction->HintSize; i++)
        {
	  fprintf(fp,"  tempResult[%i] = (double)temp%i[%i];\n",i+1,MAX_ARGS,i);
        }
      fprintf(fp,"  retval = Array(tempResult);\n");
      break;
    }
}

void return_result(FILE *fp)
{
  switch (currentFunction->ReturnType%1000)
    {
    case 2:
      /* void, do nothing */
      break;
    case 1: case 7: 
    case 4: case 5: case 6: case 14:
    case 15: case 16: case 13:
    case 3:
      fprintf(fp,"  retval = Array(double(temp%i));\n",MAX_ARGS); 
      break;
    case 303:
      /* string. i.e. char* */ 
      fprintf(fp,"  retval = Array(QString(temp%i));\n",MAX_ARGS);
      break;
    case 109:
    case 309:  
      fprintf(fp,"  retval = MakeVTKPointer((vtkObjectBase*)(temp%i));\n",MAX_ARGS);
      break;
      /* handle functions returning vectors */
      /* this is done by looking them up in a hint file */
    case 301: case 307:
    case 304: case 305: case 306:
    case 313: case 314: case 315: case 316:      
      use_hints(fp);
      break;
    default:
      fprintf(fp,"  retval = Array(QString(\"unable to return result.\"));\n");
      break;
    }
}

void handle_return_prototype(FILE *fp)
{
  switch (currentFunction->ReturnType%1000)
    {
    case 109:
    case 309:  
      /* FIXME
	 fprintf(fp,"    int %sCommand(ClientData, Tcl_Interp *, int, char *[]);\n",currentFunction->ReturnClass);
      */
      break;
    }
}

void get_args(FILE *fp, int i)
{
  int j;
  int start_arg = 1;
  
  /* what arg do we start with */
  for (j = 0; j < i; j++)
    {
      start_arg = start_arg + 
	(currentFunction->ArgCounts[j] ? currentFunction->ArgCounts[j] : 1);
    }
  
  /* handle VAR FUNCTIONS */
  if (currentFunction->ArgTypes[i] == 5000)
    {
      fprintf(fp,"    temp%i->interp = interp;\n",i);
      fprintf(fp,"    temp%i->command = strcpy(new char [strlen(argv[2])+1],argv[2]);\n",i);
      return;
    }

  /* ignore void */
  if (((currentFunction->ArgTypes[i] % 10) == 2)&&
      (!((currentFunction->ArgTypes[i]%1000)/100)))
    {
      return;
    }
  
  switch (currentFunction->ArgTypes[i]%1000)
    {
    case 1: case 7:  
      /* floating point, i.e. double */
      emitTempScalarFromArgs(fp,i,start_arg,"double");
      break;
    case 4: case 5: case 6:
      /* int */ 
      emitTempScalarFromArgs(fp,i,start_arg,"int");
      break;
    case 3:
      /* char */
      emitTempScalarFromArgs(fp,i,start_arg,"char");
      break;
    case 13:
      /* unsigned char */
      emitTempScalarFromArgs(fp,i,start_arg,"unsigned char");
      break;
    case 14:
      /* unsigned int */
      emitTempScalarFromArgs(fp,i,start_arg,"unsigned int");
      break;
    case 15:
      /* unsigned short */
      emitTempScalarFromArgs(fp,i,start_arg,"unsigned short");
      break;
    case 16:
      /* unsigned long */
      emitTempScalarFromArgs(fp,i,start_arg,"unsigned long");
      break;
    case 303:
      /* char* */
      fprintf(fp,"  char tmp_string%i[1024];\n",i);
      fprintf(fp,"  strcpy(tmp_string%i,qPrintable(arg[%i].asString())); \n",i,start_arg);
      fprintf(fp,"  temp%i = tmp_string%i;\n",i,i);
      break;
      /* FLOAT, int, short, long, double FIX */
    case 301: case 304: case 305: case 306: case 307: case 314: case 313: case 315: case 316:
      /* Check if the args(start_arg) is a vector */
      if ( currentFunction->ArgCounts[i] <= 1 )
	{
	  fprintf(fp,"    if ( !arg[%i].isVector() )\n",start_arg,start_arg);
	  fprintf(fp,"       throw Exception(\"expecting a vector\");\n");
	  /* now allocate the array and copy vector */
	  fprintf(fp,"    else \n      {\n");
	  fprintf(fp,"      Array vect = arg[%i].asDenseArray().toClass(Double);\n",start_arg);
	  fprintf(fp,"      BasicArray<double> data = vect.real<double>();\n");
	  fprintf(fp,"      int length = data.length();\n");
	  fprintf(fp,"      temp%i = new %s[length];\n", i, typeNames[currentFunction->ArgTypes[i]%100]);
	  fprintf(fp,"      for ( int k = 0; k < length; k++ ) temp%i[k] = (%s)data[k+1];\n      }\n",i,typeNames[currentFunction->ArgTypes[i]%100]);
	} else
	{
	  fprintf(fp,"  if ( !(arg[%i].isVector() && arg[%i].length() == %i) )\n",
		  start_arg,start_arg,currentFunction->ArgCounts[i]);
	  fprintf(fp,"    throw Exception(\"Mismatch in vector lengths\");\n");
	  /* now allocate the array and copy octave vector */
	  fprintf(fp,"  Array vect = arg[%i].asDenseArray().toClass(Double);\n",start_arg);
	  fprintf(fp,"  BasicArray<double> data = vect.real<double>();\n");
	  fprintf(fp,"  int length = data.length();\n");
	  fprintf(fp,"  for ( int k = 0; k < length; k++ ) temp%i[k] = (%s)data[k+1];\n",
		  i,typeNames[currentFunction->ArgTypes[i]%100]);
	}
      break;
    case 109:
    case 309:
      fprintf(fp,"  temp%i = GetVTKPointer<%s>(arg[%i]);\n",
	      i,currentFunction->ArgClasses[i],start_arg);
      break;
    case 2:    
    case 9:
      break;
    default:
      if (currentFunction->ArgCounts[i] > 1)
        {
	  for (j = 0; j < currentFunction->ArgCounts[i]; j++)
	    {
	      switch (currentFunction->ArgTypes[i]%100)
		{
		case 1: case 7:  
		  /* floating point, i.e. double */
		  emitTempArrayFromArgs(fp, i, j, start_arg, "double");
		  break;
		case 4: case 5: case 6: 
		  /* int */
		  emitTempArrayFromArgs(fp, i, j, start_arg, "int");
		  break;
		case 3:
		  /* char */
		  emitTempArrayFromArgs(fp, i, j, start_arg, "char");
		  break;
		case 13:
		  /* unsigned char */
		  emitTempArrayFromArgs(fp, i, j, start_arg, "unsigned char");
		  break;
		case 14:
		  /* unsigned int */
		  emitTempArrayFromArgs(fp, i, j, start_arg, "unsigned int");
		  break;
		case 15:
		  /* unsigned short */
		  emitTempArrayFromArgs(fp, i, j, start_arg, "unsigned short");
		  break;
		case 16:
		  /* unsigned long */
		  emitTempArrayFromArgs(fp, i, j, start_arg, "unsigned long");
		  break;
		}
	      start_arg++;
	    }
        }
      
    }
}

#if 0
void outputSubsasgnFunction(FILE *fp, FileInfo *data)
{
  int i;
  fprintf(fp,"//@@Signature\n");
  fprintf(fp,"//sgfunction @%s:subsasgn %SubsasgnFunction\n",data->ClassName,
	  data->ClassName);
  fprintf(fp,"//input varargin\n");
  fprintf(fp,"//output varargin\n");
  fprintf(fp,"ArrayVector %sSubsasgnFunction(int nargout, const ArrayVector& arg) {\n",
	  data->ClassName);
  fprintf(fp,"  if (arg.size() != 3) return ArrayVector();\n");
  fprintf(fp,"  %s* vtk_pointer = GetVTKPointer<%s>(arg[0]);\n",data->ClassName,data->ClassName);
  fprintf(fp,"  const StructArray& sa = arg[1].constStructPtr();\n");
  fprintf(fp,"  const BasicArray<Array>& typea = sa[\"type\"];\n");
  fprintf(fp,"  const BasicArray<Array>& subsa = sa[\"subs\"];\n");
  fprintf(fp,"  if ((typea.length() == 1) && (typea[1].asString() == \".\"))\n");
  fprintf(fp,"  {\n");
  for (i=0;i<data->NumberOfFunctions;i++)
    if (data->Functions[i].Name &&
	strncmp(data->Functions[i].Name,"Set",3) == 0)
      {
	fprintf(fp,"    if (subsa[1].asString() == \"%s\")\n",
		data->Functions[i].Name+3);
	fprintf(fp,"    {\n");
	fprintf(fp,"      ArrayVector tmp(arg[0]);\n");
	fprintf(fp,"      tmp.push_back(arg[2]);\n");
	fprintf(fp,"      return %s%sFunction(nargout,tmp);\n",
		data->ClassName,data->Functions[i].Name);
	fprintf(fp,"    }\n");
      }
  fprintf(fp,"  }\n");
  fprintf(fp,"  return ArrayVector();\n");
  fprintf(fp,"}\n");
}
#endif

void outputSubsrefFunction(FILE *fp, FileInfo *data)
{						
  int i;
  fprintf(fp,"//@@Signature\n");
  fprintf(fp,"//gfunction @%s:subsref %sSubsrefFunction\n",data->ClassName,
	  data->ClassName);
  fprintf(fp,"//input varargin\n");
  fprintf(fp,"//output varargout\n");
  fprintf(fp,"ArrayVector %sSubsrefFunction(int nargout, const ArrayVector& arg) {\n",
	  data->ClassName);
  fprintf(fp,"  if (arg.size() != 2) return ArrayVector();\n");
  fprintf(fp,"  %s* vtk_pointer = GetVTKPointer<%s>(arg[0]);\n",data->ClassName,data->ClassName);
  fprintf(fp,"  const StructArray& sa = arg[1].constStructPtr();\n");
  fprintf(fp,"  const BasicArray<Array>& typea = sa[\"type\"];\n");
  fprintf(fp,"  const BasicArray<Array>& subsa = sa[\"subs\"];\n");
  fprintf(fp,"  if ((typea.length() == 1) && (typea[1].asString() == \".\"))\n");
  fprintf(fp,"  {\n");
  for (i=0;i<data->NumberOfFunctions;i++)
    if (data->Functions[i].Name &&
	(strncmp(data->Functions[i].Name,"Get",3) == 0) &&
	(data->Functions[i].NumberOfArguments == 0))
      {
	fprintf(fp,"    if (subsa[1].asString() == \"%s\")\n",
		data->Functions[i].Name+3);
	fprintf(fp,"      return %s%sFunction(nargout,arg);\n",
		data->ClassName,data->Functions[i].Name);
      }
  fprintf(fp,"  }\n");
  fprintf(fp,"  if ((typea.length() == 2) && (typea[1].asString() == \".\")\n");
  fprintf(fp,"      && (typea[2].asString() == \"()\"))\n");
  fprintf(fp,"  {\n");
  for (i=0;i<data->NumberOfFunctions;i++)
    if (data->Functions[i].Name &&
	(strncmp(data->Functions[i].Name,"Get",3) == 0) &&
	(data->Functions[i].NumberOfArguments > 0))
      {
	fprintf(fp,"    if (subsa[1].asString() == \"%s\")\n",
		data->Functions[i].Name+3);
	fprintf(fp,"    {\n");
	fprintf(fp,"      ArrayVector tmp(arg[0]);\n");
	fprintf(fp,"      tmp += ArrayVectorFromCellArray(subsa[2]);\n");
	fprintf(fp,"      return %s%sFunction(nargout,tmp);\n",
		data->ClassName,data->Functions[i].Name);
	fprintf(fp,"    }\n");
      }
  fprintf(fp,"  }\n");
  fprintf(fp,"}\n\n");
}

void outputDisplayFunction(FILE *fp, FileInfo *data)
{
  int i;
  fprintf(fp,"//@@Signature\n");
  fprintf(fp,"//sgfunction @%s:display %sDisplayFunction\n",
	  data->ClassName,data->ClassName);
  fprintf(fp,"//input varargin\n");
  fprintf(fp,"//output varargout\n");
  fprintf(fp,"ArrayVector %sDisplayFunction(int nargout, const ArrayVector& arg, Interpreter *eval) {\n",
	  data->ClassName);
  fprintf(fp,"  if (arg.size() == 0) return ArrayVector();\n");
  fprintf(fp,"  if (arg[0].length() > 1) {\n");
  fprintf(fp,"     PrintArrayClassic(arg[0],100,eval);\n");
  fprintf(fp,"     return ArrayVector();\n");
  fprintf(fp,"  }\n");
  fprintf(fp,"  eval->outputMessage(\"  \" + arg[0].className() + \"\\n\");\n");
  for (i=0;i<data->NumberOfFunctions;i++)
    if (data->Functions[i].Name &&
	(strncmp(data->Functions[i].Name,"Get",3) == 0) &&
	(data->Functions[i].NumberOfArguments == 0))
      {
	fprintf(fp,"  eval->outputMessage(\"  %s : \");\n",data->Functions[i].Name+3);
	fprintf(fp,"  {\n");
	fprintf(fp,"    ArrayVector tmp = %s%sFunction(1,arg[0]);\n",data->ClassName,data->Functions[i].Name);
	fprintf(fp,"    if (tmp.size() > 0)\n");
	fprintf(fp,"      eval->outputMessage(SummarizeArrayCellEntry(tmp[0]));\n");
	fprintf(fp,"    else\n");
	fprintf(fp,"      eval->outputMessage(\"[]\");\n");
	fprintf(fp,"  }\n");
	fprintf(fp,"  eval->outputMessage(\"\\n\");\n");
      }
  fprintf(fp,"  return ArrayVector();\n");
  fprintf(fp,"}\n");
}

void outputOverloadedFunction(FILE *fp, FileInfo *data)
{
  int i;
  fprintf(fp,"//@@Signature\n");
  fprintf(fp,"//gfunction @%s:%s %s%sFunction\n",
	  data->ClassName,currentFunction->Name,
	  data->ClassName,currentFunction->Name);
  fprintf(fp,"//input varargin\n");
  fprintf(fp,"//output varargout\n");
  fprintf(fp,"ArrayVector %s%sFunction(int nargout, const ArrayVector& arg) {\n",
	  data->ClassName,currentFunction->Name);
  for (i=0;i<data->NumberOfFunctions;i++)
    if (data->Functions[i].Name &&
	strcmp(data->Functions[i].Name,currentFunction->Name) == 0
	&& data->Functions[i].IsValid)
      fprintf(fp,"  if (arg.size() == %d) return %s%s%dFunction(nargout,arg);\n",
	      data->Functions[i].NumberOfArguments+1,
	      data->ClassName,
	      data->Functions[i].Name,
	      data->Functions[i].OverloadCount);
  fprintf(fp,"  throw Exception(\"unable to resolve to an overloaded instance of %s%s\");\n",
	  data->ClassName,currentFunction->Name);
  fprintf(fp,"}\n\n");
}

void outputFunction(FILE *fp, FileInfo *data)
{
  int i;
  int args_ok = 1;
 

  /* some functions will not get wrapped no matter what else */
  if (currentFunction->IsOperator || 
      currentFunction->ArrayFailure ||
      !currentFunction->IsPublic ||
      !currentFunction->Name) 
    {
      return;
    }

  /* check to see if we can handle the args */
  for (i = 0; i < currentFunction->NumberOfArguments; i++)
    {
      /* FIXME */
      if (currentFunction->ArgTypes[i] == 5000) return;
      if ((currentFunction->ArgTypes[i]%10) == 8) args_ok = 0;
      /* if its a pointer arg make sure we have the ArgCount */
      if ((currentFunction->ArgTypes[i]%1000 >= 100) &&
	  (currentFunction->ArgTypes[i]%1000 != 303)&&
	  (currentFunction->ArgTypes[i]%1000 != 301)&& /* FLOAT* FIX */
	  (currentFunction->ArgTypes[i]%1000 != 303)&& /* char* FIX */
	  (currentFunction->ArgTypes[i]%1000 != 304)&& /* int* FIX */
	  (currentFunction->ArgTypes[i]%1000 != 305)&& /* short* FIX */
	  (currentFunction->ArgTypes[i]%1000 != 306)&& /* long* FIX */
	  (currentFunction->ArgTypes[i]%1000 != 307)&& /* double* FIX */
	  (currentFunction->ArgTypes[i]%1000 != 313)&& /* unsigned char* FIX */
	  (currentFunction->ArgTypes[i]%1000 != 314)&& /* unsigned int* FIX */
	  (currentFunction->ArgTypes[i]%1000 != 315)&& /* unsigned short* FIX */
	  (currentFunction->ArgTypes[i]%1000 != 316)&& /* unsigned long* FIX */
	  (currentFunction->ArgTypes[i]%1000 != 309)&&
	  (currentFunction->ArgTypes[i]%1000 != 109)) 
	{
	  if (currentFunction->NumberOfArguments > 1 ||
	      !currentFunction->ArgCounts[i])
	    {
	      args_ok = 0;
	    }
	}
      /*			
				if ((currentFunction->ArgTypes[i]%100 >= 10)&&
				(currentFunction->ArgTypes[i] != 13)&&
				(currentFunction->ArgTypes[i] != 14)&&
				(currentFunction->ArgTypes[i] != 15)&&
				(currentFunction->ArgTypes[i] != 16)) args_ok = 0;
      */				
    }
  if ((currentFunction->ReturnType%10) == 8) args_ok = 0;
  if (((currentFunction->ReturnType%1000)/100 != 3)&&
      ((currentFunction->ReturnType%1000)/100 != 1)&&
      ((currentFunction->ReturnType%1000)/100)) args_ok = 0;
  if (currentFunction->NumberOfArguments && 
      (currentFunction->ArgTypes[0] == 5000)
      &&(currentFunction->NumberOfArguments != 1)) args_ok = 0;

  /* we can't handle void * return types */
  if ((currentFunction->ReturnType%1000) == 302) 
    {
      args_ok = 0;
      printf("void* return type!\n");
    }
  
  /* watch out for functions that dont have enough info */
  switch (currentFunction->ReturnType%1000)
    {
    case 301: case 307:
    case 304: case 305: case 306:
    case 313: case 314: case 315: case 316:
      args_ok = currentFunction->HaveHint;
      break;
    }

  currentFunction->IsValid = args_ok;
  /* if the args are OK and it is not a constructor or destructor */
  if (args_ok && 
      strcmp(data->ClassName,currentFunction->Name) &&
      strcmp(data->ClassName,currentFunction->Name + 1))
    {
      char funcname[1024];

      if (currentFunction->OverloadCount == 0)
	strcpy(funcname,currentFunction->Name);
      else
	sprintf(funcname,"%s%d",currentFunction->Name,currentFunction->OverloadCount);

      fprintf(fp,"//@@Signature\n");
      fprintf(fp,"//gfunction @%s:%s %s%sFunction\n",
	      data->ClassName,funcname,
	      data->ClassName,funcname);
      fprintf(fp,"//input ");
      for (i = 0;i < currentFunction->NumberOfArguments+1; i++)
	fprintf(fp,"a%i ",i);
      fprintf(fp,"\n");
      fprintf(fp,"//output ");
      if (isVoidType(currentFunction->ReturnType))
	fprintf(fp,"none\n");
      else
	fprintf(fp,"y\n");
      fprintf(fp,"ArrayVector %s%sFunction(int nargout, const ArrayVector& arg) {\n",
	      data->ClassName,funcname);
      /* process the args */
      fprintf(fp,"  /* Signature %s */\n",currentFunction->Signature); 
      fprintf(fp,"  if (arg.size() < %d) \n",currentFunction->NumberOfArguments+1);
      fprintf(fp,"    throw Exception(\"Function %s for class %s requires %d argument(s)\");\n",
	      funcname,data->ClassName,currentFunction->NumberOfArguments+1);
      fprintf(fp,"  %s* vtk_pointer = GetVTKPointer<%s>(arg[0]);\n",
	      data->ClassName,data->ClassName);
      fprintf(fp,"  Array retval;\n");
      for (i = 0; i < currentFunction->NumberOfArguments; i++)
	{
	  output_temp(fp, i, currentFunction->ArgTypes[i],
		      currentFunction->ArgClasses[i], 
		      currentFunction->ArgCounts[i]);
	}
      output_temp(fp, MAX_ARGS,currentFunction->ReturnType,
		  currentFunction->ReturnClass, 0);
      handle_return_prototype(fp);
    
      /* now get the required args from the stack */
      for (i = 0; i < currentFunction->NumberOfArguments; i++)
	{
	  get_args(fp,i);
	}
    
      switch (currentFunction->ReturnType%1000)
	{
	case 2:
	  fprintf(fp,"  vtk_pointer->%s(",currentFunction->Name);
	  break;
	case 109:
	  fprintf(fp,"  temp%i = &(vtk_pointer)->%s(",MAX_ARGS,currentFunction->Name);
	  break;
	default:
	  fprintf(fp,"  temp%i = (vtk_pointer)->%s(",MAX_ARGS,currentFunction->Name);
	}
      for (i = 0; i < currentFunction->NumberOfArguments; i++)
	{
	  if (i)
	    {
	      fprintf(fp,",");
	    }
	  if (currentFunction->ArgTypes[i] == 109)
	    {
	      fprintf(fp,"*(temp%i)",i);
	    }
	  else if (currentFunction->ArgTypes[i] == 5000)
	    {
	      /* FIXME				
		 fprintf(fp,"vtkTclVoidFunc,(void *)temp%i",i);
	      */				
	    }
	  else
	    {
	      fprintf(fp,"temp%i",i);
	    }
	}
      fprintf(fp,");\n");
      if (currentFunction->NumberOfArguments && 
	  (currentFunction->ArgTypes[0] == 5000))
	{
	  /* FIXME
	     fprintf(fp,"      vtk_pointer->%sArgDelete(vtkTclVoidFuncArgDelete);\n",
	     currentFunction->Name);
	  */
	}
      return_result(fp);
      fprintf(fp,"  return retval;\n");
      fprintf(fp,"}\n\n");
    
      wrappedFunctions[numberOfWrappedFunctions] = currentFunction;
      numberOfWrappedFunctions++;
    }
}

#define max(a,b) (((a) > (b)) ? (a) : (b))

int isOverloadedFunction(int j, FileInfo *data)
{
  int i;
  for (i=0;i<data->NumberOfFunctions;i++)
    if ((i!=j) && 
	(data->Functions[i].Name) &&
	(data->Functions[j].Name) &&
	(strcmp(data->Functions[i].Name,
		data->Functions[j].Name)==0) &&
	(strcmp(data->Functions[i].Name,data->ClassName)))
      return 1;
  return 0;
}

/* print the parsed structures */
void vtkParseOutput(FILE *fp, FileInfo *data)
{
  int i,j;
  int in_example;

  for (i=0;i<data->NumberOfFunctions;i++)
    {
      int overloaded = 0;
      int maxcount = 0;
      for (j=0;j<i;j++)
	{
	  if (data->Functions[i].Name &&
	      data->Functions[j].Name &&
	      strcmp(data->Functions[i].Name,data->ClassName) &&
	      strcmp(data->Functions[i].Name,data->Functions[j].Name)==0)
	    {
	      overloaded = 1;
	      maxcount = max(maxcount,data->Functions[j].OverloadCount);
	    }
	}
      if (isOverloadedFunction(i,data))
	data->Functions[i].OverloadCount = maxcount+1;
      else
	data->Functions[i].OverloadCount = 0;
    }
  
  fprintf(fp,"#include \"VTKWrap.hpp\"\n\n");
  fprintf(fp,"// FreeMat wrapper for %s object\n//\n",data->ClassName);
    
  if (strcmp("vtkObjectBase",data->ClassName) != 0)
    {
      /* Block inclusion of full streams. */
      fprintf(fp,"#define VTK_STREAMS_FWD_ONLY\n");
    }
  /* Needed to avoid problems when putting all files into a single .cc */
  /* In particular this is only needed for Hybrid kit. The class that complains in vtkLineWidget */
  /* fprintf(fp,"#undef None\n"); */
  fprintf(fp,"#include \"%s.h\"\n\n",data->ClassName);

  fprintf(fp,"//@@Signature\n");
  fprintf(fp,"//gfunction @%s:%s %sConstructorFunction\n",data->ClassName,data->ClassName,data->ClassName);
  fprintf(fp,"//input a\n");
  fprintf(fp,"//output p\n");
  fprintf(fp,"ArrayVector %sConstructorFunction(int nargout, const ArrayVector& arg) {\n",data->ClassName);
  fprintf(fp,"  if (arg.size() == 0) {\n");
  fprintf(fp,"    Array ret(MakeVTKPointer(%s::New()));\n",data->ClassName);
  fprintf(fp,"    ret.structPtr().setClassPath(StringVector() << \"%s\");\n",data->ClassName);
  fprintf(fp,"    return ret;\n");
  fprintf(fp,"  } else if (arg[0].className() == \"%s\") {\n",data->ClassName);
  fprintf(fp,"    return arg[0];\n");
  fprintf(fp,"  } else {\n");
  fprintf(fp,"    vtkObjectBase *p = GetVTKPointer<vtkObjectBase>(arg[0]);\n");
  fprintf(fp,"    %s*q = dynamic_cast<%s*>(p);\n",data->ClassName,data->ClassName);
  fprintf(fp,"    if (!q)\n");
  fprintf(fp,"      throw Exception(\"Unable to type convert supplied object to an instance of type %s\");\n",data->ClassName);
  fprintf(fp,"    Array ret(arg[0]);\n");
  fprintf(fp,"    ret.structPtr().setClassPath(StringVector() << \"%s\");\n",data->ClassName);
  fprintf(fp,"    return ret;\n");
  fprintf(fp,"  }\n");
  fprintf(fp,"}\n");
	  
  /* insert function handling code here */
  for (i = 0; i < data->NumberOfFunctions; i++)
    {
      currentFunction = data->Functions + i;
      if ( currentFunction->Name == NULL ) continue;
      if ( strcmp("New",currentFunction->Name) != 0)
	{
	  outputFunction(fp, data);
	}
    }	

  /* insert overloaded function handling code here */
  for (i = 0; i < data->NumberOfFunctions; i++)
    {
      currentFunction = data->Functions + i;
      if (currentFunction->OverloadCount == 1)
	outputOverloadedFunction(fp, data);
    }

  outputDisplayFunction(fp, data);
  outputSubsrefFunction(fp, data);
  //  outputSubsasgnFunction(fp, data);
}

/*
  Change log: 
  13. 01. 2004. Added help for individual methods. 
  Help can be displayed as vtkClass("vtkClassMethod","Help");
  Added narrowing (convert list to scalar) of the return 
  parameter when examining superclasses. 
*/	

