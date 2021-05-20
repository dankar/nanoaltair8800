function(GET_VERSION_STRING STR SUB_FOLDER)
  find_package(Git)
  
  if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/${SUB_FOLDER}/.git")
	execute_process(COMMAND ${GIT_EXECUTABLE} describe --tags --dirty
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      OUTPUT_VARIABLE OUT_STR)
	string(REGEX REPLACE "\n$" "" OUT_STR "${OUT_STR}")
	set(${STR} "${OUT_STR}" PARENT_SCOPE)
  endif()
endfunction()
