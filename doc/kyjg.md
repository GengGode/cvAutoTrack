## 空荧酒馆原神地图 自动追踪模块手动更新步骤

#### 所需文件
  - 确保`空荧酒馆原神地图`能够打开，能够在任务管理找到`map.exe`
  - 下载最新的`cvAutoTrack.dll`的压缩包，[点击下载](https://github.com/GengGode/GenshinImpact_AutoTrack_DLL/releases/latest/download/cvAutoTrack.7z) **解压使用**
    - [历史版本(cvAutoTrack.7z)](https://github.com/GengGode/GenshinImpact_AutoTrack_DLL/releases/)
### 第一步，通过任务管理器或者别的方式找到`map.exe`所在目录，打开
![image](https://user-images.githubusercontent.com/47711102/213908723-27480a86-4a01-49b5-bd3e-30ee1e7151df.png)

### 第二步，进入到子目录`Map_Data/Plugins/x86_64/`，应该可以在该目录看到`CVATUOTRACK.dll`
![image](https://user-images.githubusercontent.com/47711102/213908740-6eb661f5-e109-4b8a-b831-b15b4b64fd17.png)

### 第三步，关闭`map.exe`，否则无法替换文件
### 第四步，将压缩包中的`cvAutoTrack.dll`替换目录中的dll，名称大小写不影响使用
![image](https://user-images.githubusercontent.com/47711102/213908788-e43f9a77-8b33-4d0d-8e4b-ff4b196dd798.png)

### 第五步，打开地图，在物品栏下沿可以看到DLL版本号，检查是否和压缩包中的`version.tag`一致
![image](https://user-images.githubusercontent.com/47711102/213908795-f7377678-e8d9-44c2-888b-5ad6d1374395.png)
![image](https://user-images.githubusercontent.com/47711102/213908875-fcbb8891-2116-497f-9d83-51eda8eec55c.png)

## 如果替换后仍旧存在问题，请添加QQ群联系管理员反馈 [【声讨/反馈】原神地图开发组【空荧酒馆】](https://jq.qq.com/?_wv=1027&k=kFenHVti)
