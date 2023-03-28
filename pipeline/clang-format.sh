#!/bin/bash

# If the first argument is just '-', do not replace the files
# instead print the result to stdout.

sed_i="-i .bak"
testername=/tmp/sed_tester.$$.tmp
touch $testername
rm -f $testername.bak
sed ${sed_i} -e '' $testername > /dev/null 2>&1
result=$?
if [ -e $testername.bak ] ; then
	sed_i='-i ""'
else
	sed_i='-i '
fi

macros=(
__host__
__device__
VECCORE_ATT_HOST
VECCORE_ATT_DEVICE
VECCORE_ATT_HOST_DEVICE
VECCORE_FORCE_INLINE
VECCORE_FORCE_NOINLINE
VECGEOM_CUDA_HEADER_HOST
VECGEOM_CUDA_HEADER_DEVICE
VECGEOM_CUDA_HEADER_BOTH
VECGEOM_FORCE_INLINE
)

patternFrom=""
patternTo=""

if [ "-" = "$1" ] ; then
	shift;

	for file in $@; do
		cat $file | eval sed -n -e '"
		# if the first line copy the pattern to the hold buffer
		1h
		# if not the first line then append the pattern to the hold buffer
		1!H
		# if the last line then ...
		$ {
		# copy from the hold to the pattern buffer
		g
		# do the search and replace
		${patternFrom}
		# print
		p
	}"' | clang-format  | clang-format | eval sed -n -e '"
	# if the first line copy the pattern to the hold buffer
	1h
	# if not the first line then append the pattern to the hold buffer
	1!H
	# if the last line then ...
	$ {
	# copy from the hold to the pattern buffer
	g
	# do the search and replace
	${patternTo}
	# print
	p
}"'
																																																																																																			  done
																																																																																																		  else
																																																																																																			  # Shorten the macro and move them out of the way so that they have
																																																																																																			  # no effect on line length calculation.
																																																																																																			  for file in $@; do
																																																																																																				  eval sed  ${sed_i} -n -e '"
																																																																																																				  # if the first line copy the pattern to the hold buffer
																																																																																																				  1h
																																																																																																				  # if not the first line then append the pattern to the hold buffer
																																																																																																				  1!H
																																																																																																				  # if the last line then ...
																																																																																																				  $ {
																																																																																																				  # copy from the hold to the pattern buffer
																																																																																																				  g
																																																																																																				  # do the search and replace
																																																																																																				  ${patternFrom}
																																																																																																				  # print
																																																																																																				  p
																																																																																																			  }"' '"${file}"'
																																																																																																		  done

																																																																																																		  clang-format -i "$@"
																																																																																																		  # Run clang-format a 2nd time, this stabilizes some of the comment positioning.
																																																																																																		  clang-format -i "$@"

																																																																																																		  # Put back the macros.
																																																																																																		  for file in $@; do
																																																																																																			  eval sed   ${sed_i} -n -e '"
																																																																																																			  # if the first line copy the pattern to the hold buffer
																																																																																																			  1h
																																																																																																			  # if not the first line then append the pattern to the hold buffer
																																																																																																			  1!H
																																																																																																			  # if the last line then ...
																																																																																																			  $ {
																																																																																																			  # copy from the hold to the pattern buffer
																																																																																																			  g
																																																																																																			  # do the search and replace
																																																																																																			  ${patternTo}
																																																																																																			  # print
																																																																																																			  p
																																																																																																		  }"' '"${file}"'
																																																																																																	  done
																																																																																																  fi
