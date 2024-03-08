file(REMOVE_RECURSE
  "liblibs_arduino_coral_micro.a"
  "liblibs_arduino_coral_micro.pdb"
)

# Per-language clean rules from dependency scanning.
foreach(lang ASM C)
  include(CMakeFiles/libs_arduino_coral_micro.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
