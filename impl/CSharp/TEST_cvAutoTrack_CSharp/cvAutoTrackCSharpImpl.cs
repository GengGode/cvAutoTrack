using System;
using System.Reflection;
using System.Runtime.InteropServices;

namespace cvAutoTrackCSharpImpl
{
    internal class cvAutoTrack
    {
        [DllImport("kernel32.dll")]
        private extern static IntPtr LoadLibrary(String path);
        [DllImport("kernel32.dll")]
        private extern static IntPtr GetProcAddress(IntPtr lib, String funcName);
        [DllImport("kernel32.dll")]
        private extern static bool FreeLibrary(IntPtr lib);
        
        private delegate bool verison_Type(byte[] versionBuff);
        private delegate bool init_Type();
        private delegate bool uninit_Type();
        private delegate bool SetUseBitbltCaptureMode_Type();
        private delegate bool SetUseDx11CaptureMode_Type();
        private delegate bool SetHandle_Type(long handle);
        private delegate bool SetWorldCenter_Type(double x, double y);
        private delegate bool SetWorldScale_Type(double scale);
        private delegate bool GetTransformOfMap_Type(ref double x, ref double y, ref double a, ref int mapId);
        private delegate bool GetPositionOfMap_Type(ref double x, ref double y, ref int mapId);
        private delegate bool GetDirection_Type(ref double a);
        private delegate bool GetRotation_Type(ref double a);
        private delegate bool GetStar_Type(ref double x, ref double y, ref bool isEnd);
        private delegate bool GetStarJson_Type(byte[] jsonBuffer);
        private delegate bool GetUID_Type(ref int uid);
        private delegate bool GetInfoLoadPicture_Type(byte[] path, ref int uid, ref double x, ref double y, ref double a);
        private delegate bool GetInfoLoadVideo_Type(byte[] path, byte[] pathOutFile);
        private delegate int GetLastErr_Type();
        private delegate int GetLastErrMsg_Type(byte[] msg_buff, int buff_size);
        private delegate int GetLastErrJson_Type(byte[] json_buff, int buff_size);
        private delegate bool startServe_Type();
        private delegate bool stopServe_Type();
        private delegate bool GetCompileVersion_Type(byte[] version_buff, int buff_size);
        private delegate bool GetCompileTime_Type(byte[] time_buff, int buff_size);
        
        private IntPtr lib = IntPtr.Zero;

        private IntPtr verison_Func = IntPtr.Zero;
        private IntPtr init_Func = IntPtr.Zero;
        private IntPtr uninit_Func = IntPtr.Zero;
        private IntPtr SetUseBitbltCaptureMode_Func = IntPtr.Zero;
        private IntPtr SetUseDx11CaptureMode_Func = IntPtr.Zero;
        private IntPtr SetHandle_Func = IntPtr.Zero;
        private IntPtr SetWorldCenter_Func = IntPtr.Zero;
        private IntPtr SetWorldScale_Func = IntPtr.Zero;
        private IntPtr GetTransformOfMap_Func = IntPtr.Zero;
        private IntPtr GetPositionOfMap_Func = IntPtr.Zero;
        private IntPtr GetDirection_Func = IntPtr.Zero;
        private IntPtr GetRotation_Func = IntPtr.Zero;
        private IntPtr GetStar_Func = IntPtr.Zero;
        private IntPtr GetStarJson_Func = IntPtr.Zero;
        private IntPtr GetUID_Func = IntPtr.Zero;
        private IntPtr GetInfoLoadPicture_Func = IntPtr.Zero;
        private IntPtr GetInfoLoadVideo_Func = IntPtr.Zero;
        private IntPtr GetLastErr_Func = IntPtr.Zero;
        private IntPtr GetLastErrMsg_Func = IntPtr.Zero;
        private IntPtr GetLastErrJson_Func = IntPtr.Zero;
        private IntPtr startServe_Func = IntPtr.Zero;
        private IntPtr stopServe_Func = IntPtr.Zero;
        private IntPtr GetCompileVersion_Func = IntPtr.Zero;
        private IntPtr GetCompileTime_Func = IntPtr.Zero;

        public bool init_dll()
        {
            lib = LoadLibrary("cvAutoTrack.dll");
            if (lib == IntPtr.Zero)
            {
                IsLoadLibrary = false;
                return false;
            }
            verison_Func = GetProcAddress(lib, "verison");
            init_Func = GetProcAddress(lib, "init");
            uninit_Func = GetProcAddress(lib, "uninit");
            SetUseBitbltCaptureMode_Func = GetProcAddress(lib, "SetUseBitbltCaptureMode");
            SetUseDx11CaptureMode_Func = GetProcAddress(lib, "SetUseDx11CaptureMode");
            SetHandle_Func = GetProcAddress(lib, "SetHandle");
            SetWorldCenter_Func = GetProcAddress(lib, "SetWorldCenter");
            SetWorldScale_Func = GetProcAddress(lib, "SetWorldScale");
            GetTransformOfMap_Func = GetProcAddress(lib, "GetTransformOfMap");
            GetPositionOfMap_Func = GetProcAddress(lib, "GetPositionOfMap");
            GetDirection_Func = GetProcAddress(lib, "GetDirection");
            GetRotation_Func = GetProcAddress(lib, "GetRotation");
            GetStar_Func = GetProcAddress(lib, "GetStar");
            GetStarJson_Func = GetProcAddress(lib, "GetStarJson");
            GetUID_Func = GetProcAddress(lib, "GetUID");
            GetInfoLoadPicture_Func = GetProcAddress(lib, "GetInfoLoadPicture");
            GetInfoLoadVideo_Func = GetProcAddress(lib, "GetInfoLoadVideo");
            GetLastErr_Func = GetProcAddress(lib, "GetLastErr");
            GetLastErrMsg_Func = GetProcAddress(lib, "GetLastErrMsg");
            GetLastErrJson_Func = GetProcAddress(lib, "GetLastErrJson");
            startServe_Func = GetProcAddress(lib, "startServe");
            stopServe_Func = GetProcAddress(lib, "stopServe");
            GetCompileVersion_Func = GetProcAddress(lib, "GetCompileVersion");
            GetCompileTime_Func = GetProcAddress(lib, "GetCompileTime");

            if (verison_Func == IntPtr.Zero)
            {
                IsLoadLibrary = false;
            }
            if (init_Func == IntPtr.Zero)
            {
                IsLoadLibrary = false;
            }
            if (uninit_Func == IntPtr.Zero)
            {
                IsLoadLibrary = false;
            }
            if (SetUseBitbltCaptureMode_Func == IntPtr.Zero)
            {
                IsLoadLibrary = false;
            }
            if (SetUseDx11CaptureMode_Func == IntPtr.Zero)
            {
                IsLoadLibrary = false;
            }
            if (SetHandle_Func == IntPtr.Zero)
            {
                IsLoadLibrary = false;
            }
            if (SetWorldCenter_Func == IntPtr.Zero)
            {
                IsLoadLibrary = false;
            }
            if (SetWorldScale_Func == IntPtr.Zero)
            {
                IsLoadLibrary = false;
            }
            if (GetTransformOfMap_Func == IntPtr.Zero)
            {
                IsLoadLibrary = false;
            }
            if (GetPositionOfMap_Func == IntPtr.Zero)
            {
                IsLoadLibrary = false;
            }
            if (GetDirection_Func == IntPtr.Zero)
            {
                IsLoadLibrary = false;
            }
            if (GetRotation_Func == IntPtr.Zero)
            {
                IsLoadLibrary = false;
            }
            if (GetStar_Func == IntPtr.Zero)
            {
                IsLoadLibrary = false;
            }
            if (GetStarJson_Func == IntPtr.Zero)
            {
                IsLoadLibrary = false;
            }
            if (GetUID_Func == IntPtr.Zero)
            {
                IsLoadLibrary = false;
            }
            if (GetInfoLoadPicture_Func == IntPtr.Zero)
            {
                IsLoadLibrary = false;
            }
            if (GetInfoLoadVideo_Func == IntPtr.Zero)
            {
                IsLoadLibrary = false;
            }
            if (GetLastErr_Func == IntPtr.Zero)
            {
                IsLoadLibrary = false;
            }
            if (GetLastErrMsg_Func == IntPtr.Zero)
            {
                IsLoadLibrary = false;
            }
            if (GetLastErrJson_Func == IntPtr.Zero)
            {
                IsLoadLibrary = false;
            }
            if (startServe_Func == IntPtr.Zero)
            {
                IsLoadLibrary = false;
            }
            if (stopServe_Func == IntPtr.Zero)
            {
                IsLoadLibrary = false;
            }
            if (GetCompileVersion_Func == IntPtr.Zero)
            {
                IsLoadLibrary = false;
            }
            if (GetCompileTime_Func == IntPtr.Zero)
            {
                IsLoadLibrary = false;
            }
            IsLoadLibrary = true;
            return IsLoadLibrary;
        }
        public void uninit_dll()
        {
            if (IsLoadLibrary)
            {
                FreeLibrary(lib);
                IsLoadLibrary = false;
            }
        }

        public bool IsLoadLibrary = false;
        
        public cvAutoTrack(bool is_auto_load = true)
        {
            if (is_auto_load)
            {
                init_dll();
            }
        }

        ~cvAutoTrack()
        {
            uninit_dll();
        }

        public bool verison(byte[] versionBuff)
        {
            verison_Type verison_Dete = (verison_Type)Marshal.GetDelegateForFunctionPointer(verison_Func, typeof(verison_Type));
            return verison_Dete(versionBuff);
        }
        public bool init()
        {
            init_Type init_Dete = (init_Type)Marshal.GetDelegateForFunctionPointer(init_Func, typeof(init_Type));
            return init_Dete();
        }
        public bool uninit()
        {
            uninit_Type uninit_Dete = (uninit_Type)Marshal.GetDelegateForFunctionPointer(uninit_Func, typeof(uninit_Type));
            return uninit_Dete();
        }
        public bool SetUseBitbltCaptureMode()
        {
            SetUseBitbltCaptureMode_Type SetUseBitbltCaptureMode_Dete = (SetUseBitbltCaptureMode_Type)Marshal.GetDelegateForFunctionPointer(SetUseBitbltCaptureMode_Func, typeof(SetUseBitbltCaptureMode_Type));
            return SetUseBitbltCaptureMode_Dete();
        }
        public bool SetUseDx11CaptureMode()
        {
            SetUseDx11CaptureMode_Type SetUseDx11CaptureMode_Dete = (SetUseDx11CaptureMode_Type)Marshal.GetDelegateForFunctionPointer(SetUseDx11CaptureMode_Func, typeof(SetUseDx11CaptureMode_Type));
            return SetUseDx11CaptureMode_Dete();
        }
        public bool SetHandle(long handle)
        {
            SetHandle_Type SetHandle_Dete = (SetHandle_Type)Marshal.GetDelegateForFunctionPointer(SetHandle_Func, typeof(SetHandle_Type));
            return SetHandle_Dete(handle);
        }
        public bool SetWorldCenter(float x, float y)
        {
            SetWorldCenter_Type SetWorldCenter_Dete = (SetWorldCenter_Type)Marshal.GetDelegateForFunctionPointer(SetWorldCenter_Func, typeof(SetWorldCenter_Type));
            return SetWorldCenter_Dete(x, y);
        }
        public bool SetWorldScale(float scale)
        {
            SetWorldScale_Type SetWorldScale_Dete = (SetWorldScale_Type)Marshal.GetDelegateForFunctionPointer(SetWorldScale_Func, typeof(SetWorldScale_Type));
            return SetWorldScale_Dete(scale);
        }
        public bool GetTransformOfMap(ref double x, ref double y, ref double a, ref int mapId)
        {
            GetTransformOfMap_Type GetTransformOfMap_Dete = (GetTransformOfMap_Type)Marshal.GetDelegateForFunctionPointer(GetTransformOfMap_Func, typeof(GetTransformOfMap_Type));
            return GetTransformOfMap_Dete(ref x, ref y, ref a, ref mapId);
        }
        public bool GetPositionOfMap(ref double x, ref double y, ref int mapId)
        {
            GetPositionOfMap_Type GetPositionOfMap_Dete = (GetPositionOfMap_Type)Marshal.GetDelegateForFunctionPointer(GetPositionOfMap_Func, typeof(GetPositionOfMap_Type));
            return GetPositionOfMap_Dete(ref x, ref y, ref mapId);
        }
        public bool GetDirection(ref double a)
        {
            GetDirection_Type GetDirection_Dete = (GetDirection_Type)Marshal.GetDelegateForFunctionPointer(GetDirection_Func, typeof(GetDirection_Type));
            return GetDirection_Dete(ref a);
        }
        public bool GetRotation(ref double a)
        {
            GetRotation_Type GetRotation_Dete = (GetRotation_Type)Marshal.GetDelegateForFunctionPointer(GetRotation_Func, typeof(GetRotation_Type));
            return GetRotation_Dete(ref a);
        }
        public bool GetStar(ref double x, ref double y, ref bool isEnd)
        {
            GetStar_Type GetStar_Dete = (GetStar_Type)Marshal.GetDelegateForFunctionPointer(GetStar_Func, typeof(GetStar_Type));
            return GetStar_Dete(ref x, ref y, ref isEnd);
        }
        public bool  GetStarJson(byte[] jsonBuffer)
        {
            GetStarJson_Type GetStarJson_Dete = (GetStarJson_Type)Marshal.GetDelegateForFunctionPointer(GetStarJson_Func, typeof(GetStarJson_Type));
            return GetStarJson_Dete(jsonBuffer);
        }
        public bool  GetUID(ref int uid)
        {
            GetUID_Type GetUID_Dete = (GetUID_Type)Marshal.GetDelegateForFunctionPointer(GetUID_Func, typeof(GetUID_Type));
            return GetUID_Dete(ref uid);
        }
        public bool  GetInfoLoadPicture(byte[] path, ref int uid, ref double x, ref double y, ref double a)
        {
            GetInfoLoadPicture_Type GetInfoLoadPicture_Dete = (GetInfoLoadPicture_Type)Marshal.GetDelegateForFunctionPointer(GetInfoLoadPicture_Func, typeof(GetInfoLoadPicture_Type));
            return GetInfoLoadPicture_Dete(path, ref uid, ref x, ref y, ref a);
        }
        public bool GetInfoLoadVideo(byte[] path, byte[] pathOutFile)
        {
            GetInfoLoadVideo_Type GetInfoLoadVideo_Dete = (GetInfoLoadVideo_Type)Marshal.GetDelegateForFunctionPointer(GetInfoLoadVideo_Func, typeof(GetInfoLoadVideo_Type));
            return GetInfoLoadVideo_Dete(path, pathOutFile);
        }
        public int GetLastErr()
        {
            GetLastErr_Type GetLastErr_Dete = (GetLastErr_Type)Marshal.GetDelegateForFunctionPointer(GetLastErr_Func, typeof(GetLastErr_Type));
            return GetLastErr_Dete();
        }
        public int GetLastErrMsg(byte[] msg_buff, int buff_size)
        {
            GetLastErrMsg_Type GetLastErrMsg_Dete = (GetLastErrMsg_Type)Marshal.GetDelegateForFunctionPointer(GetLastErrMsg_Func, typeof(GetLastErrMsg_Type));
            return GetLastErrMsg_Dete(msg_buff, buff_size);
        }
        public int GetLastErrJson(byte[] json_buff, int buff_size)
        {
            GetLastErrJson_Type GetLastErrJson_Dete = (GetLastErrJson_Type)Marshal.GetDelegateForFunctionPointer(GetLastErrJson_Func, typeof(GetLastErrJson_Type));
            return GetLastErrJson_Dete(json_buff, buff_size);
        }
        public bool startServe()
        {
            startServe_Type startServe_Dete = (startServe_Type)Marshal.GetDelegateForFunctionPointer(startServe_Func, typeof(startServe_Type));
            return startServe_Dete();
        }
        public bool  stopServe()
        {
            stopServe_Type stopServe_Dete = (stopServe_Type)Marshal.GetDelegateForFunctionPointer(stopServe_Func, typeof(stopServe_Type));
            return stopServe_Dete();
        }
        public bool  GetCompileVersion(byte[] version_buff, int buff_size)
        {
            GetCompileVersion_Type GetCompileVersion_Dete = (GetCompileVersion_Type)Marshal.GetDelegateForFunctionPointer(GetCompileVersion_Func, typeof(GetCompileVersion_Type));
            return GetCompileVersion_Dete(version_buff, buff_size);
        }
        public bool  GetCompileTime(byte[] time_buff, int buff_size)
        {
            GetCompileTime_Type GetCompileTime_Dete = (GetCompileTime_Type)Marshal.GetDelegateForFunctionPointer(GetCompileTime_Func, typeof(GetCompileTime_Type));
            return GetCompileTime_Dete(time_buff, buff_size);
        }

    }
}
