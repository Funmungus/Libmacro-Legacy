find_package(Git)
if (!Git_FOUND)
	set(GIT_EXECUTABLE git)
endif (!Git_FOUND)
execute_process(
	COMMAND "${GIT_EXECUTABLE}" -C "${CMAKE_CURRENT_SOURCE_DIR}" rev-list --count --first-parent master
	OUTPUT_VARIABLE GIT_REVISION
	)
string(STRIP "${GIT_REVISION}" GIT_REVISION)
execute_process(
	COMMAND "${GIT_EXECUTABLE}" -C "${CMAKE_CURRENT_SOURCE_DIR}" branch --show-current
	OUTPUT_VARIABLE GIT_BRANCH
	)
string(STRIP "${GIT_BRANCH}" GIT_BRANCH)
