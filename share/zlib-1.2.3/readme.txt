
if you don't want make a zlib installation that fill your
/usr/include and /usr/lib directories with files, you can
install to this directory using 
./configure --prefix=<path_to_this_directory>
when you compile the zlib project

In this case, you need to alter your environement variables:
ZLIBROOT=<path_to_this_directory>
export LD_LIBRARY_PATH=$ZLIBROOT/lib:$LD_LIBRARY_PATH
export LIBRARY_PATH=$ZLIBROOT/lib:$LIBRARY_PATH
export C_INCLUDE_PATH=$ZLIBROOT/include:$C_INCLUDE_PATH
export CPLUS_INCLUDE_PATH=$ZLIBROOT/include:$CPLUS_INCLUDE_PATH
