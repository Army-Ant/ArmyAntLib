:Start
if exist %PROTOCPP_PATH% (
    @echo "The boost has been exist!"
	goto End
)

@echo "Clone submodules!"
cd ..
git submodule update --progress

@echo "Clone submodules for boost"
cd external/boost
git submodule update --progress

@echo "Build boost library"
./bootstrap.bat
./bjam stage link=static runtime-link=static threading=multi address-model=64 debug release

:End
