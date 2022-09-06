export DISPLAY=localhost:0.0
restRootMacros -b -q 2> error.log
if [[ -s error.log ]]
then
	echo "No clean output"
	return 1;
else 
	echo "Macros output was clean"
	return 0;
fi
