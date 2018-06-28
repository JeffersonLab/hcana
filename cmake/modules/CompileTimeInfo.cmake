cmake_minimum_required(VERSION 2.8)

# Return the date (yyyy-mm-dd)
macro(DATE RESULT)
	if(WIN32)
		execute_process(COMMAND "cmd" " /C date /T" OUTPUT_VARIABLE ${RESULT})
		string(REGEX REPLACE "(..)/(..)/(....).*" "\\3-\\2-\\1" ${RESULT} ${${RESULT}})
	elseif(UNIX)
		execute_process(COMMAND "date" "+%Y-%m-%d" OUTPUT_VARIABLE ${RESULT})
	else()
		message(SEND_ERROR "Unable to detect date")
		set(${RESULT} UNKNOWN)
	endif()
endmacro()

# Return the time (hh:mm:ss)
macro(TIME RESULT)
	if(WIN32)
		execute_process(COMMAND "cmd" " /C echo %TIME%" OUTPUT_VARIABLE ${RESULT})
		string(REGEX REPLACE "(..:..:..),(..)" "\\1" ${RESULT} ${${RESULT}})
	elseif(UNIX)
		execute_process(COMMAND "date" "+%H:%M:%S" OUTPUT_VARIABLE ${RESULT})
	else()
		message(SEND_ERROR "Unable to detect time")
		set(${RESULT} UNKNOWN)
	endif()
endmacro()


execute_process(COMMAND "date" "+%b:%d:%Y" 
  OUTPUT_VARIABLE HCANA_BUILD_DATE 
  OUTPUT_STRIP_TRAILING_WHITESPACE)

execute_process(COMMAND "date" "+%a %b %d %H:%M:%S %Z %Y" 
  OUTPUT_VARIABLE HCANA_BUILD_DATETIME 
  OUTPUT_STRIP_TRAILING_WHITESPACE)

execute_process(COMMAND "bash" "-c" "echo $(uname -s)-$(uname -r)-$(uname -m)" 
  OUTPUT_VARIABLE HCANA_PLATFORM 
  OUTPUT_STRIP_TRAILING_WHITESPACE)

execute_process(COMMAND "uname" "-n" 
  OUTPUT_VARIABLE HCANA_NODE 
  OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND "pwd" 
  OUTPUT_VARIABLE HCANA_BUILD_DIR 
  OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND "whoami" 
  OUTPUT_VARIABLE HCANA_BUILD_USER 
  OUTPUT_STRIP_TRAILING_WHITESPACE)

execute_process(COMMAND "bash" "-c" "echo $(git rev-parse HEAD 2>/dev/null | cut -c1-7)" 
  OUTPUT_VARIABLE HCANA_GIT_VERSION 
  OUTPUT_STRIP_TRAILING_WHITESPACE)

execute_process(COMMAND "bash" "-c" "echo $(${CMAKE_CXX_COMPILER} --version 2>/dev/null | head -1)" 
  OUTPUT_VARIABLE HCANA_CXX_VERSION 
  OUTPUT_STRIP_TRAILING_WHITESPACE)

execute_process(COMMAND "bash" "-c" "echo $(root-config --version)" 
  OUTPUT_VARIABLE HCANA_ROOT_VERSION 
  OUTPUT_STRIP_TRAILING_WHITESPACE)

execute_process(COMMAND "bash" "-c" "printf '%02d%02d%02d' ${PROJECT_VERSION_MAJOR} ${PROJECT_VERSION_MINOR} ${PROJECT_VERSION_PATCH}" 
  OUTPUT_VARIABLE HCANA_VERCODE 
  OUTPUT_STRIP_TRAILING_WHITESPACE)

