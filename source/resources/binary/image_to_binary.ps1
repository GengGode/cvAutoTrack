# 读取png文件的字节流存储为const char[]数组
# 用于C/C++程序中的图片资源
# 用法：image_to_binary.ps1 <png文件路径> <输出文件路径>
# 例如：image_to_binary.ps1 "C:\Users\user\Desktop\test.png" "C:\Users\user\Desktop\test.h"
# 作者：LiuYan
# 时间：2019-12-26

# 获取参数
$file = $args[0]
$image_type = $args[1]
$value_name = $args[2]
$output_file = $args[3]


# 读取png文件
$png = [System.IO.File]::ReadAllBytes($file)

# 生成const char[]数组
$const_char = "#pragma once`n`nnamespace TianLi::Resources::Binary::Image::"+$image_type+"`n{`n    const unsigned char " +$value_name +  "[] = {`n        " 
$count = 0
$png | ForEach-Object {
    # 自动换行
    if ($count -eq 16) {
        $const_char += "`n        "
        $count = 0
    }
    $count += 1
    $const_char += "0x" + $_.ToString("X2") + ","


}
$const_char += "`n    };`n}`n"

# 写入文件
[System.IO.File]::WriteAllText($output_file, $const_char)

