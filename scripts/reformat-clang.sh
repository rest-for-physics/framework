#!/bin/bash

# This script reformats source files using the clang-format utility.
# Set the list of source directories on the "for" line below.
#
# The file .clang-format in this directory specifies the formatting parameters.
#
# Files are changed in-place, so make sure you don't have anything open in an
# editor, and you may want to commit before formatting in case of awryness.
#
# Note that clang-format is not included with OS X or Xcode; you must
# install it yourself.  There are multiple ways to do this:
#
# - If you use Xcode, install the ClangFormat-Xcode plugin. See instructions at
#   <https://github.com/travisjeffery/ClangFormat-Xcode/>.
#   After installation, the executable can be found at
#   $HOME/Library/Application Support/Alcatraz/Plug-ins/ClangFormat/bin/clang-format.
#
# - Download an LLVM release from <http://llvm.org/releases/download.html>.
#   For OS X, use the pre-built binaries for "Darwin".
#
# - Build the LLVM tools from source. See the documentation at <http://llvm.org>.

# Change this if your clang-format executable is somewhere else
CLANG_FORMAT="clang-format"

if [ $# -eq 0 ]; then
    echo ' '
    echo 'This script formats the source directory /path/to/source/'
    echo 'given as the only argument. Only .cxx and .h files will be '
    echo 'reformatted.'
    echo ' '
    echo 'The formatting will consider the .clang-format file closer to'
    echo 'the source directory given. '
    echo ' '
    echo 'Usage : ./reformat-clang.sh /path/to/source/'
    echo ' '
    echo 'Use carefully! :)'

    exit 1
fi

pathNow=$PWD

for DIRECTORY in $1
do
    echo "Formatting code under $DIRECTORY/"
    cd $DIRECTORY
    echo "$DIRECTORY" \( -name '*.h' -or -name '*.cxx' -or -name '*.cc' -or -name '*.C' \) -print0 | xargs -0 "$CLANG_FORMAT" -i
    find "$DIRECTORY" \( -name '*.h' -or -name '*.cxx' -or -name '*.cc' -or -name '*.C' \) -print0 | xargs -0 "$CLANG_FORMAT" -i
    cd $pathNow
    echo "DONE!"
done
