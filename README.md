请留意CMakeList.txt中JH相机驱动文件的引用。
为了避免命名重复，CMakeList.txt是用于京航相机的，CMakeList( HK ).txt是用于海康相机的，实际使用时需要把( HK )去掉
JHCap.h这个文件是安装好京航相机的驱动之后才有的，不能直接把github里的JHCap.h粘贴到你们的文件里，需要你们自己先安装好驱动再从安装后得到的文件中把JHCap.h放入代码。
