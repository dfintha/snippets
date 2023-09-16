# win32_time.py
# Query functions for the boot and local times on Windows, through the Win32
# API, in Python.
# Author: Dénes Fintha
# Year: 2023
# ---------------------------------------------------------------------------- #

# Implementation

import ctypes
import datetime
from ctypes import wintypes

class SYSTEM_TIMEOFDAY_INFORMATION(ctypes.Structure):
    """
    Python version of the SYSTEM_TIMEOFDAY_INFORMATION structure in the Win32
    API. This is an undocumented structure, used by Windows internals.
    """
    _fields_ = [
        ("BootTime", wintypes.LARGE_INTEGER),
        ("CurrentTime", wintypes.LARGE_INTEGER),
        ("TimeZoneBias", wintypes.LARGE_INTEGER),
        ("TimeZoneId", wintypes.ULONG),
        ("Reserved", wintypes.ULONG),
        ("BootTimeBias", wintypes.ULARGE_INTEGER), # ULONGLONG
        ("SleepTimeBias", wintypes.ULARGE_INTEGER), # ULONGLONG
    ]

class SYSTEMTIME (ctypes.Structure):
    """Python version of the SYSTEMTIME structure in the Win32 API."""
    _fields_ = [
        ("wYear", wintypes.WORD),
        ("wMonth", wintypes.WORD),
        ("wDayOfWeek", wintypes.WORD),
        ("wDay", wintypes.WORD),
        ("wHour", wintypes.WORD),
        ("wMinute", wintypes.WORD),
        ("wSecond", wintypes.WORD),
        ("wMilliseconds", wintypes.WORD),
    ]

def get_boot_time() -> datetime.datetime:
    """Queries the boot time on Windows."""
    ntdll = ctypes.windll.LoadLibrary("ntdll.dll")
    info = SYSTEM_TIMEOFDAY_INFORMATION()
    ntdll.ZwQuerySystemInformation(
        0x03, # SystemTimeOfDayInformation
        ctypes.pointer(info),
        ctypes.sizeof(info),
        wintypes.LPVOID(0))
    uptime = (info.CurrentTime - info.BootTime) / 10000000
    now = datetime.datetime.now()
    return now - datetime.timedelta(seconds=uptime)

def get_local_time() -> datetime.datetime:
    """Queries the local time on Windows."""
    kernel32 = ctypes.windll.LoadLibrary("kernel32.dll")
    info = SYSTEMTIME()
    kernel32.GetLocalTime(ctypes.pointer(info))
    return datetime.datetime(
        info.wYear,
        info.wMonth,
        info.wDay,
        info.wHour,
        info.wMinute,
        info.wSecond,
        info.wMilliseconds * 1000
    )

# Demonstration

def win32_time_test():
    """Tests the time query functions for Windows."""
    print(f" Boot Time: {get_boot_time().strftime('%Y-%m-%d %H:%M:%S')}")
    print(f"Local Time: {get_local_time().strftime('%Y-%m-%d %H:%M:%S')}")

if __name__ == "__main__":
    win32_time_test()
