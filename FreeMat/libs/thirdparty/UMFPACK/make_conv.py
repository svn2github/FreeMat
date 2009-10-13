#This is a hack for generating cmake files

import os
import re

#os.chdir(r"UMFPACK")

f=file("make_rules",'r');
lines=f.readlines()
f.close()

source_files=""
f=file("make_rules.cmake",'w')
for line in lines:

    defs=re.findall('(?<=-D)\w+',line)
    source=re.findall('(?<=-c\s../)\S*',line)
    source=source[0]
    output=re.findall('(?<=-o\s)\S*',line)
    output=" ${CMAKE_CURRENT_BINARY_DIR}/Source/" + re.sub('\.o','.c',output[0])
   
    o_str="CONFIGURE_FILE( \n"
    o_str+="${CMAKE_CURRENT_SOURCE_DIR}/"+source
    o_str+=output+"\n"
    o_str+=" COPYONLY)\n"
    source_files += output + '\n'
    f.writelines(o_str)
    dep_str ="SET_SOURCE_FILES_PROPERTIES( " + output + " PROPERTIES "
    if len(defs)> 0 :
        dep_str += ' COMPILE_FLAGS "'
        for d in defs: 
            dep_str += ' -D' + d
        dep_str += ' "'
    dep_str += " OBJECT_DEPENDS " + "${CMAKE_CURRENT_SOURCE_DIR}/" + source
    dep_str += " GENERATED 1)\n"
    f.writelines( dep_str )
#    f.writelines( "MACRO_ADD_FILE_DEPENDENCIES( " + output + " " + "${CMAKE_CURRENT_SOURCE_DIR}/" + source + " " +" )\n")
f.writelines( "ADD_LIBRARY( umfpack \n" + source_files + " )\n" )
f.close()
