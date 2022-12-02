project(submodule_common_code)

#https://matgomes.com/add-library-cmake-create-libraries/
add_library (CommonCodeLib STATIC fault.c
logging.c
netw_common.c
proc_functions.c
timer_functions.c
vectors_points.c
)

# Make sure the compiler can find include files for our library
target_include_directories (CommonCodeLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})