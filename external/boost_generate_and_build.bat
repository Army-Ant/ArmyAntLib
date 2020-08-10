@echo "Clone submodules!"
cd ..
git submodule update --init --progress --recursive

@echo "Clone submodules for boost"
cd external/boost
git checkout master
git submodule update --init --progress --recursive

@echo "Build boost library"
call bootstrap.bat
call b2 --build-type=complete
