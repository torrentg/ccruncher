
if you don't want make a gsl installation that fill your
/usr/include and /usr/lib directories with files, you can
install to this directory using 
./configure --prefix=<path_to_this_directory>
when you compile the gsl project

In this case, you need to alter your environement variables:
GSLROOT=<path_to_this_directory>
export LD_LIBRARY_PATH=$GSLROOT/lib:$LD_LIBRARY_PATH
export LIBRARY_PATH=$GSLROOT/lib:$LIBRARY_PATH
export C_INCLUDE_PATH=$GSLROOT/include:$C_INCLUDE_PATH
export CPLUS_INCLUDE_PATH=$GSLROOT/include:$CPLUS_INCLUDE_PATH

