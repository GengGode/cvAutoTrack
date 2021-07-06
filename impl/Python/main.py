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


if __name__ == '__main__':
    sleep(5)
    tracker = AutoTracker(r'./CVAUTOTRACK.dll')
    tracker.init()
    print('1) err', tracker.get_last_error(), '\n')
    print(tracker.get_transform())
    print('2) err', tracker.get_last_error(), '\n')
    print(tracker.get_uid())
    print('3) err', tracker.get_last_error(), '\n')
    tracker.uninit()
