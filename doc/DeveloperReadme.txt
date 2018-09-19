ArmyAntlib库的开发备注和使用说明

本库采用Microsoft Visual Studio, Apple XCode 和 JetBrains CLion 三种IDE进行项目管理和代码一键编译, 编译器分别为 Microsoft VC++ build, LLVM, gcc/g++(cmake)

------------------------------------ English ----------------------------------------------------------------------------------------------
This file is written to introduce the structure, development way, and some notice for this library.

The file directory structure and some brief introduction is as follows:
	<Root>
		.idea <DIR>					This is the project config files directory for JetBrains CLion
		ArmyAntLib <DIR>			This is the main source code project directory for this library
		iOSLibTest <DIR>			This is the test project directory for iOS platform
		JavaLibTest <DIR>			This is the test project directory for Java
		LibOut <DIR>				Here are all the files what this library compiled, and what the users for library needed. You can copy the content out to use this library, do not need any other file operations
		LinuxLibTest <DIR>			This is the test project directory for Linux platform
		macOSLibTest <DIR>			This is the test project directory for mac OS platform
		Win32LibTest <DIR>			This is the test project directory for Win32 platform
		.gitattributes				This is the git attributes file
		.gitignore					This is the gitignore file
		ARMYANT.COPYRIGHT.BSD_LICENSE.MD	This is the text file about BSD lisence, include translation for Chinese
		ArmyAntLib.sln				This is the solution file for Microsoft Visual Studio 2015,include a project for the main source code project and the Win32LibTest project

The structure for the main source code project directory "ArmyAntLib" is as follows:
	ArmyAntLib <DIR>
		.idea <DIR>					This is the project config files directory for JetBrains CLion
		base <DIR>					Here are some basic c + + code files and other code files, these files has particularity in the structure, cannot be classified into "include" or "src" directory. They are all internal code files.
		bin <DIR>					Build target directory, contains all the generated files, as well as some debug symbol files
		docs <DIR>					Many useful documents about this library
		externals <DIR>				When reference the third party codes and projects, they will be joined to this directory. Currently, only Python3.5 and boost 1.59 are joined to this library
		include <DIR>				All C/C++ header files except the main out header(ArmyAntLib.h and C_ArmyAntLib.h) is palced into this directory. In particular, the C++ template class files will be named with ".hpp" extension
		languages <DIR>				All other language interface programs and their codes without C++
		lib <DIR>					All build static library files
		src <DIR>					All C++ source code files is placed here except which in "base"
		tools <DIR>					Third party development tools, include these tools developed by AnmyAnt, which is used in building this library
		ArmyAnt.h					The main C++ out header, user need only include this header to use the library in C++ in any platform
		C_ArmyAnt.h					The main C out header, user need only include this header to use the library in C in any platform
		vc140.idb					Visual Studio 2015 minimum rebuild dependency file
		ArmyAntLib.vcxproj			Visual Studio 2015 project file
		ArmyAntLib.vcxproj.filters	Visual Studio 2015 project filters file
		CMakeLists.txt				The project file for cmake, used for JetBrains CLion
		
The above is the file directory structure. Most IDEs use the file directory structure as the project structure, but Visual Studio does not. We use the VS 2015 to manage the project mainly, so we will introduce the project in VC++.
Open the file "ArmyAntLib.sln" in Visual Studio 2015, you will see the projects in this solution. There are 2 projects : main pro. named "ArmyAntLib", and the test pro. named "Win32LibTest", as they are in file directory.
We will primarily introduce the project "ArmyAntLib", the structure of this project is as follow:
	ArmyAntLib <vc14proj>
		00.other-Projects			There are references of other IDE projects, like CMakeList.txt and ArmyAntLib.xcodeproj
		0.docs						There are references of developer documents and blogs
		01.base						There are references of the files in directory "ArmyAntLib/base"
		02.Third-party-libraries	There are references of third party
		03.mainOutput				There are references of some special header files
			defination				Special defination header
			onlyIncludeFile			Header files only need to include in using
		04.CppLibCode				There are references of main library source files
			cppTemplateUtils		There are references of all C++ template modules
			encoding
			network
			stream
		05.CRefCode					There are references of interface code files for C
		06.JavaRefCode				There are references of interface code files for Java，contains C++ code，java code，and building script
		07.PythonRefCode			There are references of interface code files for Python，contains C++ code and Python code
		08.CSharpRefCode			There are references of interface code files for C#，contains C++ code and C# code
		09.SwiftRefCode				There are references of interface code files for Swift

------------------------------------ Traditional Chinese ----------------------------------------------------------------------------------

本檔案主要介紹本庫的開發結構，開發管道，以及一些注意事項.

整個庫的檔案系統目錄結構如下：
	<Root>
		.idea <DIR>					這是JetBrains CLion的工程資料檔案，不要手動更改
		ArmyAntLib <DIR>			這是本庫原始程式碼工程的主資料夾
		iOSLibTest <DIR>			這是用於iOS平臺測試的工程目錄，涉及到C和Swift
		JavaLibTest <DIR>			這是用於本庫的Java介面在Java上的測試工程目錄
		LibOut <DIR>				這是本庫的編譯後生成總匯目錄.你可以直接將此目錄內容複製出來進行使用，根目錄下的很多測試工程會引用此目錄
		LinuxLibTest <DIR>			這是本庫在Linux平臺下進行測試的工程目錄
		macOSLibTest <DIR>			這是本庫在macOS平臺下進行測試的工程目錄
		Win32LibTest <DIR>			這是本庫在Win32平臺下進行測試的工程目錄
		.gitattributes				本庫Git版本控制系統的設定檔
		.gitignore					本庫Git版本控制系統的提交忽略選項檔案
		ARMYANT.COPYRIGHT.BSD_LICENSE.MD	這是BSD開源協定的原文副本，包括中文譯文
		ArmyAntLib.sln				這是Microsoft Visual Studio 2015的解決方案檔案，包含類別庫源工程和Win32LibTest工程.對於此解決方案的工程詳細結構，將在下麵進行詳細描述

對於庫的原始程式碼主資料夾，其結構說明如下：
	ArmyAntLib <DIR>
		.idea <DIR>					這是JetBrains CLion的工程資料檔案，不要手動更改
		base <DIR>					存放了一些基礎C++程式碼檔案和其他程式碼檔案，這些檔案在結構上具有特殊性，不能歸入include或者src中.它們均是內部程式碼，不會在外部使用本庫時用到
		bin <DIR>					生成目錄，包含全部生成的檔案，以及一些調試符號檔案
		docs <DIR>					一些關於本庫的有用的開發者檔案
		externals <DIR>				引用了來自外部的庫時，外部庫的相關檔或工程會加入到這個目錄.現時只有python3.5和boost1.59這兩個外部庫
		include <DIR>				除庫的獨立輸出標頭檔（ArmyAntLib.h和C_ArmyAntLib.h）以外，所有隨庫一同生成的C/C++標頭檔案，都會放置在這個目錄.包含範本類的標頭檔將會以.hpp為副檔名
		languages <DIR>				所有用於生成其他語言介面的工程和非C/C++程式碼，都會放在這裡，並歸入各自語言的工程之中
		lib <DIR>					Win32生成的動態庫需要配合一個標靜態庫使用，它們被添加在這裡.本庫直接生成的靜態版本也會生成在此處，現時本庫暫時沒有配寘靜態生成功能
		src <DIR>					除base中的一些特例外，所有C++原始檔案都會被放置在這裡
		tools <DIR>					引用了來自外部的（或者ArmyAnt開發的）開發便利性工具時，工具會被放置在這裡
		ArmyAnt.h					庫的C++獨立輸出標頭檔.任何平臺使用本庫時只需要include本檔案即可
		C_ArmyAnt.h					庫的C語言獨立輸出標頭檔，任何平臺使用純C語言開發時，只需要include本檔案即可
		vc140.idb					Visual Studio 2015最小重生成資料庫檔案
		ArmyAntLib.vcxproj			Visual Studio 2015工程管理檔案
		ArmyAntLib.vcxproj.filters	Visual Studio 2015工程生成設定檔
		CMakeLists.txt				CMake工程檔案，由JetBrain CLion管理工程時使用

以上是檔案目錄結構.對於大多數IDE，直接用檔案系統的結構作為工程結構.本項目工程主要用Visual Studio 2015進行管理，而VC++工程並不與檔案目錄結構關聯.囙此對於工程和程式碼檔案的說明，將在VC工程說明中一同進行.
用VS2015打開根目錄的ArmyAntLib.sln檔案，可以看到整個工程，現時包含兩個項目，分別是ArmyAntlib主項目和Win32LibTest即Win32下的測試項目.本文主要對ArmyAntLib項目進行說明
項目結構如下：
	ArmyAntLib <vc14proj>
		00.other-Projects			這裡引用其他IDE的部分項目設定檔，例如CMakeList.txt和ArmyAntLib.xcodeproj，以便對比或修改
		0.docs						這裡引用一些開發者檔案或心得記錄
		01.base						這裡引用物理目錄“base”中的檔案
		02.Third-party-libraries	這裡引用來自外部的引用庫的一些標頭檔，以便查找
		03.mainOutput				這裡引用一些特殊重要性的標頭檔，主要是輸出標頭檔
			defination				特殊定義標頭檔
			onlyIncludeFile			使用時僅需要包含的兩個標頭檔
		04.CppLibCode				這裡引用C++庫邏輯的所有標頭檔和原始檔案
			cppTemplateUtils		這裡集中引用所有C++範本類別模組檔案（都是.hpp尾碼）
			encoding
			network
			stream
		05.CRefCode					這裡引用純C語言介面的標頭檔和邏輯原始檔案
		06.JavaRefCode				這裡引用與Java介面相關的檔案，包括邏輯C++原始檔案，java檔案，以及編譯腳本
		07.PythonRefCode			這裡引用與Python介面相關的檔案，包括邏輯C++原始檔案和Python檔案
		08.CSharpRefCode			這裡引用與C#介面相關的檔案，包括邏輯C++原始檔案和C#檔案
		09.SwiftRefCode				這裡引用與Swift介面相關的檔案

------------------------------------ Simplified Chinese -----------------------------------------------------------------------------------

本文件主要介绍本库的开发结构, 开发方式, 以及一些注意事项.

整个库的文件系统目录结构如下:
	<Root>
		.idea <DIR>					这是JetBrains CLion的工程数据文件, 不要手动更改
		ArmyAntLib <DIR>			这是本库源代码工程的主文件夹
		iOSLibTest <DIR>			这是用于iOS平台测试的工程目录, 涉及到C和Swift
		JavaLibTest <DIR>			这是用于本库的Java接口在Java上的测试工程目录
		LibOut <DIR>				这是本库的编译后生成总汇目录. 你可以直接将此目录内容复制出来进行使用, 根目录下的很多测试工程会引用此目录
		LinuxLibTest <DIR>			这是本库在Linux平台下进行测试的工程目录
		macOSLibTest <DIR>			这是本库在macOS平台下进行测试的工程目录
		Win32LibTest <DIR>			这是本库在Win32平台下进行测试的工程目录
		.gitattributes				本库Git版本控制系统的配置文件
		.gitignore					本库Git版本控制系统的提交忽略选项文件
		ARMYANT.COPYRIGHT.BSD_LICENSE.MD	这是BSD开源协议的原文副本, 包括中文译文
		ArmyAntLib.sln				这是Microsoft Visual Studio 2015的解决方案文件,包含类库源工程和Win32LibTest工程.对于此解决方案的工程详细结构,将在下面进行详细描述

对于库的源代码主文件夹, 其结构说明如下:
	ArmyAntLib <DIR>
		.idea <DIR>					这是JetBrains CLion的工程数据文件, 不要手动更改
		base <DIR>					存放了一些基础C++代码文件和其他代码文件, 这些文件在结构上具有特殊性, 不能归入include或者src中.它们均是内部代码, 不会在外部使用本库时用到
		bin <DIR>					生成目录, 包含全部生成的文件, 以及一些调试符号文件
		docs <DIR>					一些关于本库的有用的开发者文档
		externals <DIR>				引用了第三方库时,第三方库的相关文件或工程会加入到这个目录. 目前只有python3.5和boost1.59这两个第三方库
		include <DIR>				除库的独立输出头文件(ArmyAntLib.h和C_ArmyAntLib.h)以外, 所有随库一同生成的C/C++标头文件, 都会放置在这个目录. 包含模板类的头文件将会以.hpp为扩展名
		languages <DIR>				所有用于生成其他语言接口的工程和非C/C++代码, 都会放在这里, 并归入各自语言的工程之中
		lib <DIR>					Win32生成的动态库需要配合一个标静态库使用,它们被添加在这里. 本库直接生成的静态版本也会生成在此处, 目前本库暂时没有配置静态生成功能
		src <DIR>					除base中的一些特例外, 所有C++源文件都会被放置在这里
		tools <DIR>					引用了第三方的(或者ArmyAnt开发的)开发便利性工具时, 工具会被放置在这里
		ArmyAnt.h					库的C++独立输出头文件. 任何平台使用本库时只需要include本文件即可
		C_ArmyAnt.h					库的C语言独立输出头文件, 任何平台使用纯C语言开发时, 只需要include本文件即可
		vc140.idb					Visual Studio 2015 最小重生成数据库文件
		ArmyAntLib.vcxproj			Visual Studio 2015 工程管理文件
		ArmyAntLib.vcxproj.filters	Visual Studio 2015 工程生成配置文件
		CMakeLists.txt				CMake工程文件, 由JetBrain CLion管理工程时使用
		
以上是文件目录结构.对于大多数IDE, 直接用文件系统的结构作为工程结构. 本项目工程主要用Visual Studio 2015进行管理, 而VC++工程并不与文件目录结构关联. 因此对于工程和代码文件的说明, 将在VC工程说明中一同进行.
用VS2015打开根目录的ArmyAntLib.sln文件, 可以看到整个工程, 目前包含两个项目, 分别是ArmyAntlib主项目和Win32LibTest即Win32下的测试项目. 本文主要对ArmyAntLib项目进行说明
项目结构如下:
	ArmyAntLib <vc14proj>
		00.other-Projects			这里引用其他IDE的部分项目配置文件, 例如CMakeList.txt和ArmyAntLib.xcodeproj, 以便对比或修改
		0.docs						这里引用一些开发者文档或心得记录
		01.base						这里引用物理目录"base"中的文件
		02.Third-party-libraries	这里引用第三方引用库的一些头文件,以便查找
		03.mainOutput				这里引用一些特殊重要性的头文件, 主要是输出头文件
			defination				特殊定义头文件
			onlyIncludeFile			使用时仅需要包含的两个头文件
		04.CppLibCode				这里引用C++库逻辑的所有头文件和源文件
			cppTemplateUtils		这里集中引用所有C++模板类模块文件(都是.hpp后缀)
			encoding
			network
			stream
		05.CRefCode					这里引用纯C语言接口的头文件和逻辑源文件
		06.JavaRefCode				这里引用与Java接口相关的文件, 包括逻辑C++源文件, java文件, 以及编译脚本
		07.PythonRefCode			这里引用与Python接口相关的文件, 包括逻辑C++源文件和Python文件
		08.CSharpRefCode			这里引用与C#接口相关的文件, 包括逻辑C++源文件和C#文件
		09.SwiftRefCode				这里引用与Swift接口相关的文件
