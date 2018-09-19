------------------------------------ English ----------------------------------------------------------------------------------------------

Library Name			:	ArmyAntLib(Win32) / libArmyAnt(Unix)
Development Language	:	C++
Supported Platform		:	Win32, DotNet, Linux, FreeBSD, Android, macOS, iOS
Development IDEs		:	Microsoft Visual Studio 2015, JetBrains CLion, Apple XCode(Unrealized)
Usage					:	The C++ basic classes library for multi platforms, and its bridge interface for many other languages
SUGGESTED CONTENT		:	Stream (Include filesystem, network, pipe, etc.)
							Socket
							AES and MD5 algorithm
							Data structs and its algorithm (Tree, Multi-Value-Dictionary(map), Digraph, State-Machine, Message-Queue, Neuron and Neural-Network, etc.)
							Binary encoder and decoder
						
Welcome to view and use this library named ArmyAntLib
This library is a base class library developed for Group ArmyAnt, used for work and research. The copyright of this library is protected by BSD lisence, everyone whoever obey the lisence can use the library and its source code.
You can view the lisence in the file ARMYANT.COPYRIGHT.BSD_LICENSE.MD at the root of our project directory.
If you want to view the source code project structure, please read the text file "ArmyAntLib/base/DeveloperReadme.txt"
This ReadMe file is written to introduce the use way and build way of the library, and some notice in using and building.

Building in Windows platform:
1. Open the solution file "ArmyAntLib.sln" by Microsoft Visual Studio 2015, then right-click at the project "ArmyAntLib", choose to execute the item "build", to build the library
2. Before build the library, you can choose different configurations among Debug, Release, Debug_Static(to build the static library), and Release_Static(to build the static library too)
3. You can also choose the different target machine type like x86(Win32) and x86_64(x64)

Building in Linux platform:
1. Open the JetBrains Clion project in the subdirectory (not the root directory) named "ArmyAntLib", config the CMakelist.txt and the project configuration, and build it.
2. The configuration items you can change in CMakeList.txt contains:
	(1) set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -m64") use "-m32" to build for 32 bits, and "-m64" for 64 bits. You can also build 32bits target in 32bits system without "-m32"
3. The configuration of CLion project has 4 types: Debug+Shared, Release+Shared, Debug+Static, Release+Static

Building in FreeBSD platform:

Building in macOS platform:

Building notice at all platforms:
1. You need to setup the latest Java Development Kits and Python 3 at your computer, and set the enviroment variable "JAVA_HOME" in your system. You need also set the "PYTHON_INCLUDE" envvar as the header files of python when you build at any platform except Windows.
2. The building out files is in ArmyAntLib/bin, but you can get them in ./LibOut, with all necessary headers and libraries.

Using in any platform:
1. Copy out all files from LibOut directory
2. You need only include the header "ArmyAnt.h" to use this library. If you use the static library, you need to define "AA_USE_STATIC" before include the header
3. If you use the static library, you need join the directory "LibOut/lib/lib"(for 32 bits) or "LibOut/lib/lib64"(for 64 bits) into the library path
4. You should make sure that there has defined "DEBUG" or "_DEBUG" before include the header when you want to use the debug version, and there has defined "_x64" before include when you want to use the 64 bits version.

Building the Java library:
1. After building the C++ shared library, You can run the script file "buildJava.sh" or "buildJava.bat" at the directory "ArmyAntLib/languages/Java", to build the jar file. You can find the out jar file in "LibOut"

Using the Java library:
1. Join the jar file and the shared library file to your Java project, to use the library in Java.

Building the Android Native Library
1.

Using the Android Native Library
1.

Using the Python library:
1. The .py file will be copied to LibOut automatically after build the shared library. You can inport the module in .py file to use it.

------------------------------------ Traditional Chinese ----------------------------------------------------------------------------------

庫名		：	ArmyAntLib（Win32）/ libArmyAnt（Unix）
開發語言	：	C++
支持平臺	：	Win32，DotNet，Linux，FreeBSD，Android，macOS，iOS
生成工具	：	Microsoft Visual Studio 2015，JetBrains CLion，Apple XCode（未實現）
作用		：	跨平臺基礎操作類別庫
包含模塊	：	統一流操作（統一介面訪問檔案、網絡、筦道等等所有流），
				Socket，
				加密（包括AES和MD5），
				資料結構（樹，多值字典，圖，狀態機，消息隊列，神經網路等），
				二進位資料轉換

歡迎瞭解和使用本庫.
本庫是為了ArmyAnt團隊進行工作和開發研究所編寫的基礎類別庫.本庫採用BSD協定進行版權保護.任何同意遵守BSD協定的團體和個人，都可以自由免費使用本庫及原始程式碼. BSD協定的詳細內容可以參考根目錄檔案：ARMYANT.COPYRIGHT.BSD_LICENSE.MD
本庫開發者如想獲取本庫的程式碼工程結構等相關資訊，可以閱讀ArmyAntLib/base/DeveloperReadme.txt文字.
本檔案主要介紹庫的使用方法和生成方法，以及生成和使用過程中的一些注意事項.

在Windows平臺下的生成：
1.用Microsoft Visual Studio 2015打開根目錄的ArmyAntLib.sln解決方案檔案.在項目ArmyAntLib上按一下右鍵，選擇生成，即可生成庫
2.生成之前，可以選擇不同的配寘類型，包括Debug，Release，Debug_Static（生成靜態庫），Release_Static（靜態庫）四種配寘，以及x86（Win32）和x86_64（x64）兩種目標平臺

在Linux平臺下的生成：
1.使用JetBrains CLion打開ArmyAntLib子目錄（而不是根目錄）中的工程，對CMakelist.txt以及CLion的項目配寘作相應配寘修改，然後生成
2.對CMakeList.txt檔案可做的修改包括：
（1）set（CMAKE_CXX_FLAGS“${CMAKE_CXX_FLAGS} -std=c++11 -m64”）這裡-m32代表編譯為32比特庫，-m64代表編譯為64比特庫.
3.對CLion項目配寘有四種選擇，即Debug靜態庫，Release靜態庫，Debug動態庫，Release動態庫

在FreeBSD平臺下的生成：

在macOS平臺下的生成：

所有平臺下C/C++庫生成注意事項：
1.你需要在你的機器上安裝最新的Java Development Kits和Python 3，並配寘好JAVA_HOME環境變數，非Windows平臺還需要定義PYTHON_INCLUDE環境變數（Python標頭檔目錄）
2.理論上，直接生成的檔案在ArmyAntLib/bin中.但是你可以直接從本工程的根目錄的LibOut目錄中獲得生成的庫，包括所需的標頭檔

所有平臺下C/C++庫使用方法：
1.將LibOut裡面的全部內容拷貝出來，到你自己選擇的合適位置，它們就是本庫使用時需要的全部內容
2.在你的C++程式碼中包含標頭檔“ArmyAnt.h”，即可使用本庫.如果要使用本庫的靜態版，還需要在包含ArmyAnt.h之前define AA_USE_STATIC，或者在你的程式編譯器配寘（例如Windows平臺下就是VC++項目配寘的預處理器定義宏）中直接定義此宏.
3.要使用靜態庫，還需要將LibOut/lib/lib（32比特）或者LibOut/lib/lib64（64比特）加入到庫目錄路徑中
4.如果使用本庫調試版本，你需要保證程式中define了DEBUG或者_DEBUG；如果要是用本庫的64比特版本，你需要保證程式中define了_x64

生成Java語言版的庫：
1.將C++動態庫生成成功後，運行ArmyAntLib/languages/Java目錄下的buildJava.bat或者buildJava.sh，即可生成jar庫，生成的jar庫可在LibOut中找到

使用Java語言版的庫：
1.將生成的jar庫和C++動態庫添加到你的Java項目中，即可使用

生成Android Native動態庫：
1.

使用Android Native動態庫：
1.

使用Python語言版的庫：
1.在生成C++動態庫後，會將py檔案一同拷貝到LibOut目錄中，直接import此py檔案即可使用本庫的python版功能

------------------------------------ Simplified Chinese -----------------------------------------------------------------------------------

库名		:	ArmyAntLib(Win32) / libArmyAnt(Unix)
开发语言	:	C++
支持平台	:	Win32, DotNet, Linux, FreeBSD, Android, macOS, iOS
生成工具	:	Microsoft Visual Studio 2015, JetBrains CLion, Apple XCode(未实现)
作用		:	跨平台基础操作类库
包含模块	:	统一流操作(统一接口访问文件、网络、管道等等所有流), 
				Socket, 
				加密(包括AES和MD5),
				数据结构(树,多值字典,图,状态机,消息队列,神经网络等),
				二进制数据转换

欢迎了解和使用本库.
本库是为了ArmyAnt团队进行工作和开发研究所编写的基础类库.本库采用BSD协议进行版权保护. 任何同意遵守BSD协议的团体和个人, 都可以自由免费使用本库及源代码. BSD协议的详细内容可以参考根目录文件: ARMYANT.COPYRIGHT.BSD_LICENSE.MD
本库开发者如想获取本库的代码工程结构等相关信息, 可以阅读ArmyAntLib/base/DeveloperReadme.txt文本.
本文件主要介绍库的使用方法和生成方法, 以及生成和使用过程中的一些注意事项.


在Windows平台下的生成:
1. 用Microsoft Visual Studio 2015打开根目录的ArmyAntLib.sln解决方案文件. 在项目ArmyAntLib上单击右键,选择生成, 即可生成库
2. 生成之前,可以选择不同的配置类型, 包括Debug, Release, Debug_Static(生成静态库), Release_Static(静态库)四种配置, 以及x86(Win32)和x86_64(x64)两种目标平台

在Linux平台下的生成:
1. 使用JetBrains CLion打开ArmyAntLib子目录(而不是根目录)中的工程, 对CMakelist.txt以及CLion的项目配置作相应配置修改, 然后生成
2. 对CMakeList.txt文件可做的修改包括:
	(1) set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -m64") 这里-m32代表编译为32位库, -m64代表编译为64位库.
3. 对CLion项目配置有四种选择,即Debug静态库, Release静态库, Debug动态库, Release动态库

在FreeBSD平台下的生成:

在macOS平台下的生成:

所有平台下C/C++库生成注意事项:
1. 你需要在你的机器上安装最新的Java Development Kits和Python 3, 并配置好JAVA_HOME环境变量, 非Windows平台还需要定义PYTHON_INCLUDE环境变量(Python头文件目录)
2. 理论上,直接生成的文件在ArmyAntLib/bin中. 但是你可以直接从本工程的根目录的LibOut目录中获得生成的库,包括所需的头文件

所有平台下C/C++库使用方法:
1. 将LibOut里面的全部内容拷贝出来, 到你自己选择的合适位置, 它们就是本库使用时需要的全部内容
2. 在你的C++代码中包含头文件"ArmyAnt.h", 即可使用本库. 如果要使用本库的静态版,还需要在包含ArmyAnt.h之前define AA_USE_STATIC, 或者在你的程序编译器配置(例如Windows平台下就是VC++项目配置的预处理器定义宏)中直接定义此宏. 
3. 要使用静态库, 还需要将LibOut/lib/lib(32位)或者LibOut/lib/lib64(64位)加入到库目录路径中
4. 如果使用本库调试版本, 你需要保证程序中define了DEBUG或者_DEBUG; 如果要是用本库的64位版本, 你需要保证程序中define了_x64

生成Java语言版的库:
1. 将C++动态库生成成功后, 运行ArmyAntLib/languages/Java目录下的buildJava.bat或者buildJava.sh, 即可生成jar库,生成的jar库可在LibOut中找到

使用Java语言版的库:
1. 将生成的jar库和C++动态库添加到你的Java项目中,即可使用

生成Android Native动态库：
1.

使用Android Native动态库：
1.

使用Python语言版的库:
1. 在生成C++动态库后, 会将py文件一同拷贝到LibOut目录中,直接import此py文件即可使用本库的python版功能
