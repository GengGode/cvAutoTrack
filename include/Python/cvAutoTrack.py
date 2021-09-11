from ctypes import *
from time import sleep


class AutoTracker:
    def __init__(self, dll_path: str):
        self.__lib = CDLL(dll_path)

        # bool init();
        self.__lib.init.restype = c_bool

        # bool uninit();
        self.__lib.uninit.restype = c_bool

        # bool SetHandle(long long int handle);
        self.__lib.SetHandle.argtypes = [c_longlong]
        self.__lib.SetHandle.restype = c_bool

        # bool GetTransform(float &x, float &y, float &a);
        self.__lib.GetTransform.argtypes = [POINTER(c_float), POINTER(c_float), POINTER(c_float)]
        self.__lib.GetTransform.restype = c_bool

        # bool GetPosition(double & x, double & y);
        self.__lib.GetPosition.argtypes = []
        self.__lib.GetPosition.restype = c_bool

        # bool GetDirection(double &a);
        self.__lib.GetDirection.argtypes = [POINTER(c_double)]
        self.__lib.GetDirection.restype = c_bool

        # bool GetUID(int &uid);
        self.__lib.GetUID.argtypes = [POINTER(c_int)]
        self.__lib.GetUID.restype = c_bool

    def init(self) -> bool:
        return self.__lib.init()

    def uninit(self) -> bool:
        return self.__lib.uninit()

    def get_last_error(self) -> int:
        return self.__lib.GetLastErr()

    def set_handle(self, hwnd: int) -> bool:
        return self.__lib.SetHandle(hwnd)

    def get_transform(self) -> (bool, float, float, float):
        x, y, a = c_float(0), c_float(0), c_float(0)
        ret = self.__lib.GetTransform(x, y, a)
        return ret, x.value, y.value, a.value

    def get_position(self) -> (bool, float, float):
        x, y = c_float(0), c_float(0)
        ret = self.__lib.GetPosition(x, y)
        return ret, x.value, y.value

    def get_direction(self) -> (bool, float):
        a = c_float(0)
        ret = self.__lib.GetDirection(a)
        return ret, a.value

    def get_uid(self) -> (bool, int):
        uid = c_int(0)
        ret = self.__lib.GetUID(uid)
        return ret, uid.value


# 以下是对被封装的类的简单演示。
# 使用命令行 `python ./main.py` 直接运行本文件即可。
if __name__ == '__main__':

    # 等待五秒钟以便切换到原神窗口：
    sleep(5)

    # 加载同一目录下的DLL：
    tracker = AutoTracker(r'./CVAUTOTRACK.dll')

    # 初始化并打印错误：
    tracker.init()
    print('1) err', tracker.get_last_error(), '\n')

    # 获取当前人物所在位置以及角度（箭头朝向）并打印错误：
    print(tracker.get_transform())
    print('2) err', tracker.get_last_error(), '\n')

    # 获取UID并打印错误：
    print(tracker.get_uid())
    print('3) err', tracker.get_last_error(), '\n')

    # 卸载相关内存：（这一步不是必须的，但还是建议手动调用）
    tracker.uninit()
