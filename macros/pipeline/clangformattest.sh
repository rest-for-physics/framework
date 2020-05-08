#!/bin/bash

## Javier Galan got this script from
## https://gitlab.cern.ch/VecGeom/VecGeom/commit/8c723ded243dbfaac59d372b524366db4577a728

#
# This is running a clang-format test
# by doing a filtering step and then analysing
# the result of applying ./scripts/clang-format-and-fix-macros.sh
#

# check that we are in a clean state in order to prevent accidential
# changes
cleanstate=`git status | grep "modified"`
if ! [[ -z $cleanstate ]]; then
	echo "Script must be applied on a clean git state"
	return 0
fi


# Retrieve list of files that were changed in source branch
# with respect to master (target branch)
tb="origin/master"
# we disregard the test/static_analysis directory since this contains LLVM code + headers
filelist=`git diff ${tb}... --name-only | grep -v "test/static_analysis"`

echo $filelist

# function to check if C++ file (based on suffix)
# can probably be done much shorter
function checkCPP(){
	if [[ $1 == *.cc ]];then
		return 0
	elif [[ $1 == *.cpp ]];then
		return 0
	elif [[ $1 == *.cxx ]];then
		return 0
	elif [[ $1 == *.C ]];then
		return 0
	elif [[ $1 == *.c++ ]];then
		return 0
	elif [[ $1 == *.c ]];then
		return 0
	elif [[ $1 == *.CPP ]];then
		return 0
		# header files
	elif [[ $1 == *.h ]];then
		return 0
	elif [[ $1 == *.hpp ]];then
		return 0
	elif [[ $1 == *.hh ]];then
		return 0
	elif [[ $1 == *.icc ]];then
		return 0
	fi
	return 1
}

# check list of files
for f in $filelist; do
	if checkCPP $f; then
		echo "CHECKING MATCHING FILE ${f}"
		# apply the clang-format script
		./macros/pipeline/clang-format.sh ${f}
	fi
done

# check if something was modified
notcorrectlist=`git status | grep "modified"`
# if nothing changed ok
if [[ -z $notcorrectlist ]]; then
	# send a negative message to gitlab
	echo "Excellent. **VERY GOOD FORMATTING!** :thumbsup:"
	exit 0;
else
	echo "The following files have clang-format problems (showing patches)";
	for f in $notcorrectlist; do
		echo $f
		git diff $f
	done

	#We now repair commiting
	git config --global user.email "runner@lfna.unizar.es"
	git config --global user.name "runner"
	git config --global push.default simple
	git remote set-url --push origin git@lfna.unizar.es:rest-development/REST_v2.git
	#git remote set-url --push origin https://runner:uwh7Ui*087@lfna.unizar.es/rest-development/REST_v2.git
	git add -u
	git commit -m "Pipeline clang-format automatic execution"
	git status
	git config --global http.sslverify "false"
    echo "git push origin HEAD:$CI_COMMIT_REF_NAME"
	git push origin HEAD:$CI_COMMIT_REF_NAME
	echo "Clang-format will generate a commit to fix code formatting"
	echo "This build will stop here. But it should be solved in the next try."
	exit 1;
fi

# cleanup changes in git
git reset HEAD --hard

exit 1

