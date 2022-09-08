echo "Calling grep"
grep -q -r "warning" error2.log > error.log

if [[ -s error.log ]]
then
	echo "No clean output"
	return 1;
else 
	echo "Macros output was clean"
	return 0;
fi
