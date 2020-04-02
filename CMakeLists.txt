project(win32-res-lib)
cmake_minimum_required(VERSION 3.12)
set( EXECUTABLE_OUTPUT_PATH ../bin)
set(CMAKE_CXX_FLAGS_RELEASE "-s -Wl,-gc-sections -mwindows")

add_executable(df-git-ui src/git-info.cc src/stuff.cc)
target_link_libraries(df-git-ui -lwin32hlp -lexshit 
	-lstdshit -lcomdlg32 -limagehlp -lgdi32)
