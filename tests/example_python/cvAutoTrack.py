from ctypes import *
import os

'''
DLL Version: 7.4.11
'''
# 将cvAutoTrack.7z置于该文件同级目录下
DLL_PATH = os.path.dirname(os.path.abspath(__file__))+"\\"

if not os.path.exists(DLL_PATH+'CVAUTOTRACK.dll'):
    import py7zr
    with py7zr.SevenZipFile(DLL_PATH+'cvAutoTrack.7z', mode='r') as z:
        z.extractall(DLL_PATH)
        print("unzip succ")

class AutoTracker:
    def __init__(self, dll_path:str = DLL_PATH+"CVAUTOTRACK.dll"):
        self.__lib = CDLL(dll_path)
        self.__lib.verison.restype = c_char
        self.__lib.SetDisableFileLog.restype = c_bool
        self.__lib.SetEnableFileLog.restype = c_bool
        self.__lib.GetUID.restype = c_bool
        self.__lib.GetAllInfo.restype = c_bool
        self.__lib.GetLastErr.restype = c_int
        self.__lib.init.restype = c_bool
        self.__lib.uninit.restype = c_bool
        self.__lib.GetTransformOfMap.restype = c_bool
        self.__lib.GetPositionOfMap.restype = c_bool
        self.__lib.GetDirection.restype = c_bool
        self.__lib.GetRotation.restype = c_bool

    def init(self):
        return self.__lib.init()

    def verison(self): # ?
        ret = self.__lib.SetDisableFileLog()
        return(ret)

    def disable_log(self):
        return self.__lib.SetDisableFileLog()
    
    def enable_log(self):
        return self.__lib.SetEnableFileLog()
    
    def get_uid(self):
        a = pointer(c_int(0))
        ret = self.__lib.GetUID(a)
        return ret, a.contents.value
        
    def uninit(self):
        return self.__lib.uninit()

    def get_last_error(self):
        return self.__lib.GetLastErr()

    def get_transform(self):
        x, y, a, b = pointer(c_double(0)), pointer(c_double(0)), pointer(c_double(0)), pointer(c_int(0))
        ret = self.__lib.GetTransformOfMap(x, y, a, b)
        return ret, x.contents.value, y.contents.value, a.contents.value

    def get_position(self):
        x, y, b = pointer(c_double(0)), pointer(c_double(0)), pointer(c_int(0))
        ret = self.__lib.GetPositionOfMap(x, y, b)
        return ret, x.contents.value, y.contents.value

    def get_direction(self):
        a = pointer(c_double(0))
        ret = self.__lib.GetDirection(a)
        return ret, a.contents.value

    def get_rotation(self):
        a = pointer(c_double(0))
        ret = self.__lib.GetRotation(a)
        return ret, a.contents.value
    
if __name__ == '__main__':
    auto_tracker = AutoTracker()
    print("init")
    print(auto_tracker.init())
    print("get_last_error")
    print(auto_tracker.get_last_error())
    print("get_transform")
    print(auto_tracker.get_transform())
    print("get_position")
    print(auto_tracker.get_position())
    print("get_direction")
    print(auto_tracker.get_direction())
    print("get_rotation")
    print(auto_tracker.get_rotation())
    print("get_uid")
    print(auto_tracker.get_uid())
    # print("get_all_info")
    # print(auto_tracker.get_all_info())
    print("disable_log")
    print(auto_tracker.disable_log())
    print("enable_log")
    print(auto_tracker.enable_log())
    print("version")
    print(auto_tracker.verison())
    print("uninit")
    print(auto_tracker.uninit())
    print("test end")
