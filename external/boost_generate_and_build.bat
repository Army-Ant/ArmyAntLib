:Start
if exist boost (
    @echo "The boost has been exist!"
	goto End
)

@echo "Clone submodules!"
cd ..
git submodule update --init --progress

@echo "Clone submodules for boost"
cd external/boost
git submodule update --init --progress

@echo "Build boost library"
./bootstrap.bat
./bjam stage

:End
