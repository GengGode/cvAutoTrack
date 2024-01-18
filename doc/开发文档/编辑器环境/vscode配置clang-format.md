# VS Code 配置 clang-format

## 安装插件

在 VS Code 中安装插件 `Clang-Format`。

## 配置

在 VS Code 的配置文件中添加如下配置：

```json
{
    "clang-format.executable": "clang-format",
    "clang-format.style": "file",
    "clang-format.formatOnSave": true,
    "clang-format.sortIncludes": true,
    "clang-format.fallbackStyle": "LLVM",
    "clang-format.language": "cpp"
}
```

## 设置插件

在 VS Code 的设置中设置 `clang-format` 插件的 `Executable` 为 `clang-format` 的绝对路径。

windows: `C:\Users\%USERNAME%\.vscode\extensions\ms-vscode.cpptools-1.18.5-win32-x64\LLVM\bin\clang-format.exe`

linux: 自主安装的 `clang-format` 的绝对路径