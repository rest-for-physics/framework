if [ $(root-config --version | grep "6.") ];then

echo Installing rest...
mkdir -p ../build
cd ../build
rm -rf *
cmake ..
make install -j2
else
	echo ROOT6 required, but not found!
	echo Current ROOT version is: $(root-config --version)
fi