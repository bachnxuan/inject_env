cpp_dir := "module/src/main/cpp"

format:
    clang-format -style=file -i {{cpp_dir}}/ksu_mgr.h {{cpp_dir}}/inject.cpp {{cpp_dir}}/ksu_mgr.c
