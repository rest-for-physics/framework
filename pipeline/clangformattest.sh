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


cd ../../
find . \( -name '*.h' -or -name '*.cxx' -or -name '*.cc' -or -name '*.C' \) -print0 | xargs -0 clang-format -i

# check if something was modified
notcorrectlist=`git status | grep "modified"`
if [[ -z $notcorrectlist ]]; then
	echo "Excellent. **VERY GOOD FORMATTING!** :thumbsup:"
	exit 0;
else
	echo "The following files have clang-format problems (showing patches)";
	for f in $notcorrectlist; do
		echo $f
		#git diff $f
	done

	#We now repair commiting
	git config --global user.email "javier.galan@unizar.es"
	git config --global user.name "clang runner"
	git config --global push.default simple
	git remote set-url --push origin git@github.com:rest-for-physics/framework.git
	git add -u
	git commit -m "Pipeline clang-format automatic execution"
	git status
    echo "git push origin HEAD:master"
	git push origin HEAD:master
	echo "Clang-format should have generated a commit to fix code formatting"
	echo "This validation should be solved in the next run."
	exit 0;
fi

# cleanup changes in git
git reset HEAD --hard

exit 1

