# Strict warning set used for all production code.
function(autolib_set_warnings target)
  set(common -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion -Wshadow
             -Wcast-qual -Wformat=2 -Wundef)
  set(c_only -Wstrict-prototypes -Wmissing-prototypes)
  set(cxx_only -Wold-style-cast -Wnon-virtual-dtor -Woverloaded-virtual)
  if(AUTOLIB_WARNINGS_AS_ERRORS)
    list(APPEND common -Werror)
  endif()
  target_compile_options(${target} PRIVATE
    ${common}
    $<$<COMPILE_LANGUAGE:C>:${c_only}>
    $<$<COMPILE_LANGUAGE:CXX>:${cxx_only}>)
endfunction()
