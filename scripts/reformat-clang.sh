# Execute at the root source directory to reformat code following clang-style
find . -regex '.*\.\(cpp\|hpp\|cc\|cxx\|h\)' -exec clang-format -i {} \;
