
var=$(git ls-remote --heads origin ${1})

var2="${var:1:3}"

if [ -z $var2 ]; then echo "Checking out master"; git checkout origin master; else echo "Checking out ${1}"; git checkout origin ${1}; fi
