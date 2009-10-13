Project( FreeMat_www )

cmake_minimum_required(VERSION 2.6)

find_path( XSITE_HOME_DIR xsite )

find_package(PythonInterp REQUIRED)
find_package( Java REQUIRED)
find_program( xsite_exe xsite PATHS ${XSITE_HOME_DIR}/bin)
#set( OUTPUT_DIR ${CMAKE_SOURCE_DIR}/../output CACHE PATH  "Path to output directory")

set( md_files
	${CMAKE_SOURCE_DIR}/pages/download.md
	${CMAKE_SOURCE_DIR}/pages/faq.md
	${CMAKE_SOURCE_DIR}/pages/help.md
	${CMAKE_SOURCE_DIR}/pages/index.md
	${CMAKE_SOURCE_DIR}/pages/news.md
	${CMAKE_SOURCE_DIR}/pages/screenshots.md
)

foreach( mdfile1 ${md_files} )
	get_filename_component( mdname_we ${mdfile1} NAME_WE )
	set( DEP_LIST ${DEP_LIST} ${mdname_we}.html )
endforeach(mdfile1)
	
add_custom_target(WebSite
	COMMAND	${xsite_exe} -S ${CMAKE_SOURCE_DIR}/pages -m sitemap.xml -s ../themes/corporate_slave/skin.html  -o ${CMAKE_SOURCE_DIR}/pages
	DEPENDS ${DEP_LIST}
)
	
foreach( mdfile ${md_files} )
	get_filename_component( mdname_we ${mdfile} NAME_WE )
	add_custom_command( OUTPUT ${mdname_we}.html
		COMMAND	cmd /c ${PYTHON_EXECUTABLE} c:\\Python25\\Scripts\\markdown.py -o html4 ${mdfile} -f ${CMAKE_SOURCE_DIR}/pages/${mdname_we}.html
		DEPENDS ${mdfile}
	)
endforeach(mdfile)