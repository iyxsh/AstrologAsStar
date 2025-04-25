# Astrolog
 Astrolog for defferent Systems! Such as windows unix andriod apple ...

参数说明
astrolog32.exe 月 日 年 时间 夏令时 时区 经度 纬度 海拔高度 展示名称 地点

参数格式：
月              整数 1-12
日              整数 1-31
年              整数 1900-2100
时间            字符串 "时:分:秒"
夏令时          字符串 "是" 或 "否"
时区            字符串 "时:分:秒"
经度            字符串 "度:分:秒"
纬度            字符串 "度:分:秒"
海拔高度        浮点数
展示名称        字符串
地点            字符串
参数示例：


windows下编译为 vs2022 工程然后使用
cd out/build
cmake ../../ -G "Visual Studio 17 2022" -A Win32


用vs2022打开工程编译后找到astrolog32.exe 运行
 .\astrolog32.exe 3 22 2025 "12:30:00" "No" "-8:00:00" "116:23'50E" "39:54'27N" 39.00 "此时此刻" "北京, 中国" 
如果无法运行，把Astrolog\astrolog32\swe\lib\windows 下的动态库拷贝到运行目录下(astrolog32.exe 所在目录) 然后重新运行即可

linux 下 
cd out/build
cmake ../../
make

找到astrolog32 运行
./astrolog32 3 22 2025 "12:30:00" "No" "-8:00:00" "116:23'50E" "39:54'27N" 39.00 "此时此刻" "北京, 中国" 
