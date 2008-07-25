
if you don't want make a expat installation that fill your
/usr/include and /usr/lib directories with files, you can
install to this directory using 
./configure --prefix=<path_to_this_directory>
when you compile the expat project

In this case, you need to alter your environement variables:
EXPATROOT=<path_to_this_directory>
export LD_LIBRARY_PATH=$EXPATROOT/lib:$LD_LIBRARY_PATH
export LIBRARY_PATH=$EXPATROOT/lib:$LIBRARY_PATH
export C_INCLUDE_PATH=$EXPATROOT/include:$C_INCLUDE_PATH
export CPLUS_INCLUDE_PATH=$EXPATROOT/include:$CPLUS_INCLUDE_PATH
