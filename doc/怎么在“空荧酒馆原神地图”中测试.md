## 空荧酒馆原神地图 自动追踪模块手动更新测试教程

目前`空荧酒馆原神地图`可以自动更新追踪模块。如果需要以开发为目的做测试，可以使用以下的方法手动更新：

#### 所需文件
  - 确保`空荧酒馆原神地图`能够打开，版本不低于Bata3.4.5，可以在打开地图后在筛选页面左下角看到
  ![image](https://user-images.githubusercontent.com/99392726/216538086-cfd32512-568d-4d06-9f15-4343ef376b43.png)
  - 下载最新的`cvAutoTrack.dll`的压缩包，[点击下载](https://github.com/GengGode/cvAutoTrack/releases/latest/download/cvAutoTrack.7z) **解压使用**
    - [历史版本(cvAutoTrack.7z)](https://github.com/GengGode/cvAutoTrack/releases/)

### 第一步，打开资源管理器，在地址栏输入`%APPDATA%`，进入`[用户名] > AppData > Roaming`文件夹

![image](https://user-images.githubusercontent.com/99392726/216538328-4fc01319-553e-4b27-802a-9f1cd952bbd0.png)

### 第二步，点击**向上一级**“⬆”，然后依次打开`LocalLow > 空荧酒馆 > Map > DLL`，这个文件至少有一个`[以版本号为名称的文件夹]`和`version.json`

![image](https://user-images.githubusercontent.com/99392726/216539670-c8389f05-f2d4-4a15-8825-9c6c0f65fde3.png)

### 第三步，如果`空荧酒馆原神地图`正在运行，请关闭

### 第四步，在该目录下新建一个文件夹，文件夹的名称随意，比如这里起名为`test-pre1`
![image](https://user-images.githubusercontent.com/99392726/216540421-bd1623f3-ff42-4af3-9723-944223b75638.png)

### 第五步，将`cvAutoTrack.dll`复制到tsst-pre1中，名称大小写不影响使用
![image](https://user-images.githubusercontent.com/99392726/216540575-e48566cf-d1ff-4f1a-87da-b14d55100a87.png)

### 第六步，打开`空荧酒馆原神地图`，点击右上角的设置“⚙”，选择`自动追踪`，在`版本选择`中选中`test-pre1`，然后开启自动追踪

  * 如果提示要更新，请选择**取消**
  * 更换后软件可能会崩溃，重新打开地图软件即可。

![image](https://user-images.githubusercontent.com/99392726/216541527-9c723522-7c26-44bb-99c7-5b541f6d318c.png)

## 如果对自动追踪的功能有疑问，请添加QQ群联系管理员反馈 [【声讨/反馈】原神地图开发组【空荧酒馆】](https://jq.qq.com/?_wv=1027&k=kFenHVti)
