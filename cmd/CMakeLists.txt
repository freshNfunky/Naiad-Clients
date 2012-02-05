
project (NCLIENTS_CMD)

add_executable (naiad main)

if ($ENV{EM_COMPILER} STREQUAL "intel")
target_link_libraries (naiad Ni -static-intel)
endif ($ENV{EM_COMPILER} STREQUAL "intel")

if ($ENV{EM_COMPILER} STREQUAL "gcc")
target_link_libraries (naiad Ni pthread)
endif ($ENV{EM_COMPILER} STREQUAL "gcc")

if ($ENV{EM_COMPILER} STREQUAL "MSVC")
target_link_libraries (naiad Ni${EM_D})
endif ($ENV{EM_COMPILER} STREQUAL "MSVC")

install (TARGETS naiad DESTINATION clients/ncmd/bin)
install (TARGETS naiad DESTINATION server/bin)
