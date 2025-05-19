file(REMOVE_RECURSE
  "liblibs_arduino_coral_micro_wifi.a"
  "liblibs_arduino_coral_micro_wifi.pdb"
)

# Per-language clean rules from dependency scanning.
foreach(lang ASM C)
  include(CMakeFiles/libs_arduino_coral_micro_wifi.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
