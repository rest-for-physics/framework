export DISPLAY=localhost:0.0
restRootMacros -b -q 2> error.log
if [[ -s error.log ]]
then
	return 0;
else 
	return 1;
fi
