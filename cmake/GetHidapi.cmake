### use hidapi submodule
# build hidapi as static lib
set(BUILD_SHARED_LIBS FALSE)
add_subdirectory(external/hidapi)

# ### old code for using fetchcontent to get hidapi if not detected
# include(FetchContent)
# ### FETCHCONTENT_TRY_FIND_PACKAGE_MODE is set to OPT_IN by default
# ### if find_package(hidapi) is unsuccessful it will fetch from git
# FetchContent_Declare(
#   hidapi
#   GIT_REPOSITORY https://github.com/libusb/hidapi.git
#   GIT_TAG        hidapi-0.14.0
#   FIND_PACKAGE_ARGS
# )
# ### uncomment for static lib
# # option(BUILD_SHARED_LIBS "" OFF)
# FetchContent_MakeAvailable(hidapi)

# ### old code for manual control based on find_package
# find_package(hidapi)
# if(NOT ${hidapi_FOUND})
#   message("HIDAPI NOT FOUND - Fetching from github")
#   include(FetchContent)
#   FetchContent_Declare(
#     hidapi
#     GIT_REPOSITORY https://github.com/libusb/hidapi.git
#     GIT_TAG        hidapi-0.14.0
#   )
#   option(BUILD_SHARED_LIBS "" OFF)
#   FetchContent_MakeAvailable(hidapi)
# else()
#   message("HIDAPI FOUND")
# endif()