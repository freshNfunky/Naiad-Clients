# in case OMP_ENALBED is NOT defined in your local environment
#if [ -z ${OMP_ENABLED} ]; then
#    echo exporting default OMP_ENABLED = NO
#    export OMP_ENABLED=NO
#fi

#if [ -z ${EM_COMPILER} ]; then
#    echo exporting default EM_COMPILER = gcc
#    export EM_COMPILER=gcc
#fi

#if [ -z ${CC} ]; then
#    echo exporting default CC = gcc
#    export CC=gcc
#fi

#if [ -z ${CXX} ]; then
#    echo exporting default CXX = g++
#    export CXX=g++
#fi

# start the build setup...

# nstudio-proc

remove-item $env:DEV_ROOT_NSTUDIO/stage-$env:EM_PLAT-$env:EM_ARCH/debug -force -recurse
new-item $env:DEV_ROOT_NSTUDIO/stage-$env:EM_PLAT-$env:EM_ARCH/debug -force -type directory 

#new-item $env:DEV_ROOT_NSTUDIO/stage-$env:EM_PLAT-$env:EM_ARCH/debug -force -type directory
#new-item $env:DEV_ROOT_NSTUDIO/stage-$env:EM_PLAT-$env:EM_ARCH/release -force -type directory

$env:dcmakecmd = "cmake -DCMAKE_BUILD_TYPE=DEBUG -DCMAKE_INSTALL_PREFIX=$env:NAIAD_PATH $env:DEV_ROOT_NSTUDIO -G `"Visual Studio 10 Win64`""
#$env:rcmakecmd = "cmake -DCMAKE_BUILD_TYPE=RELEASE -DCMAKE_INSTALL_PREFIX=$env:NAIAD_PATH $env:DEV_ROOT_NSTUDIO -G `"Visual Studio 10 Win64`""

# nstudio-proc

cd $env:DEV_ROOT_NSTUDIO/stage-$env:EM_PLAT-$env:EM_ARCH/debug
$env:dcmakecmd
Invoke-Expression -Command $env:dcmakecmd

#cd $env:DEV_ROOT_NSTUDIO/stage-$env:EM_PLAT-$env:EM_ARCH/release
#$env:rcmakecmd
#Invoke-Expression -Command $env:rcmakecmd

cd ~
