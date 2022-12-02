#!/bin/bash

# This script will reformat source files using the clang-format utility.
# Set the list of source directories on the "for" line below.
#
# The file .clang-format in this directory specifies the formatting parameters.
#
# Files are changed in-place, so make sure you don't have anything open in an
# editor, and you may want to commit before formatting in case something bad happens.
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
XML_LINT="xmllint"

if [ $# -eq 0 ]; then
  echo ' '
  echo 'This script formats the source directory /path/to/source/'
  echo 'given as the only argument. Only *.h, *.cxx, *.cc, *.C and *.rml files will be reformatted.'
  echo ' '
  echo 'The formatting will consider the .clang-format file closer to the source directory given.'
  echo ' '
  echo 'Usage: ./reformat-files.sh /path/to/source/'
  echo ' '
  echo 'Use carefully! :)'
  exit 1
fi

echo "Begin formatting!"

if ! type -P $CLANG_FORMAT; then
  echo "WARNING: '$CLANG_FORMAT' was not found in your system! we cannot format code files"
  CLANG_FORMAT_DISABLED=True
fi

export XMLLINT_INDENT="    "
if ! type -P $XML_LINT; then
  echo "WARNING: '$XML_LINT' was not found in your system! we cannot format *.rml files"
  XML_LINT_DISABLED=True
fi

pathToFormat=$(readlink -f $1)

if [ -d "$pathToFormat" ]; then
  for DIRECTORY in $pathToFormat; do
    echo "Formatting code under $DIRECTORY/"
    if [[ -z "$CLANG_FORMAT_DISABLED" ]]; then
      find "$DIRECTORY" \( -name '*.h' -or -name '*.cxx' -or -name '*.cc' -or -name '*.C' \) -print0 | xargs -0 "$CLANG_FORMAT" -i
    fi
    if [[ -z "$XML_LINT_DISABLED" ]]; then
      find "$DIRECTORY" -name "*.rml" -type f -exec $XML_LINT --output '{}' --format '{}' \;
    fi
    echo "Done formatting all files in '$DIRECTORY'"
  done
elif [ -f "$pathToFormat" ]; then
  echo "Formatting file \"$pathToFormat\""
  ext="${pathToFormat##*.}"
  if [[ "$ext" == "h" || "$ext" == "cxx" || "$ext" == "cc" || "$ext" == "C" ]]; then
    echo "$CLANG_FORMAT -i $pathToFormat"
    eval "$CLANG_FORMAT -i $pathToFormat"
  elif [[ "$ext" == "rml" ]]; then
    $XML_LINT --output "$pathToFormat" --format "$pathToFormat"
  else
    echo "Not valid extension $ext, valid extensions are *.h, *.cxx, *.cc, *.C or *.rml"
  fi
else
  echo "$pathToFormat is not valid file or directory"
fi
