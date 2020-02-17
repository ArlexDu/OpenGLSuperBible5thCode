## OpenGL超级宝典（第五版）环境配置  

在“OPenGL安装包”文件夹下可以看到Demo、freeglut、glew、gltools、glut五个文件夹，其中Demo是示例程序，其余四个是安装包。

下面简单说一下配置。

### 1.glew配置  

glew文件夹下有compiled和src两个文件夹，其中，compiled文件夹下是已经编译好的安装包，而src文件夹下是源码。在下是用已编译好的安装包配置的，src文件夹下的源码是为安装gltools准备的（见后面附录部分）。有想用源码安装glew的读者，请自行探索。下面说明安装步骤。

1.解压compiled文件夹下的“glew-2.0.0-win32.zip”，得到glew-2.0.0文件夹。

2.找到VS2015的安装目录，如果是安装在C盘的，多半是类似C:\Program Files(x86)\Microsoft Visual Studio 15.0这样的路径；在下是装在其它盘的，目录是“D:\软件安装\VS2015”，为了表述方便，后面一律按在下的目录进行说明。

3.找到C++的头文件目录，如在下的是“D:\软件安装\VS2015\VC\include”，在include目录下新建GL文件夹（如果没有的话）。

4.将1中glew-2.0.0\include\GL文件夹下的4个头文件都拷贝到3中的GL文件夹下。

5.将glew-2.0.0\bin\Release\Win32下的glew32.dll拷贝到D:\软件安装\VS2015\VC\bin目录下。

6.将glew-2.0.0\lib\Release\Win32下的两个lib文件拷贝到D:\软件安装\VS2015\VC\lib目录下。

注意，没必要将dll文件拷贝到C:\Windows\System32或C:\Windows\SysWOW64下。

至此，glew配置完毕。

### 2.glut配置  

1.将glut文件夹下的“glutdlls37beta.zip”解压，得到5个文件，分别是glut.h、gult.dll、glut32.dll、glut.lib、glut32.lib。

2.将glut.h拷贝到D:\软件安装\VS2015\VC\include\GL目录下。

3.将gult.dll、glut32.dll拷贝到D:\软件安装\VS2015\VC\bin目录下。

4.将glut.lib、glut32.lib拷贝到D:\软件安装\VS2015\VC\lib目录下。

至此，glut配置完毕。

### 3.freeglut配置  

1.将freeglut文件夹下的“freeglut-MSVC-3.0.0-2.mp.zip”解压，得到freeglut文件夹。

2.在freeglut/include/GL目录下有4个头文件，将除了glut.h之外的3个头文件拷贝到D:\软件安装\VS2015\VC\include\GL目录下。

3.将freeglut/bin目录下的freeglut.dll文件拷贝到D:\软件安装\VS2015\VC\bin目录下。

4.将freeglut/lib目录下的freeglut.lib文件拷贝到D:\软件安装\VS2015\VC\lib目录下。

至此，freeglut配置完毕。

### 4.gltools配置  

1.将gltools\include目录下的11个头文件全部拷贝到D:\软件安装\VS2015\VC\include目录下（注意是include目录，而不是GL子目录）。

2.将gltools\lib目录下的gltools.lib文件拷贝到D:\软件安装\VS2015\VC\lib目录下。

至此，gltools配置完毕。

至此，OpenGL配置完毕。

转自：https://www.cnblogs.com/zpcdbky/p/6724772.html