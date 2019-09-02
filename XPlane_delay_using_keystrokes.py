import pyautogui
import socket
import time
import subprocess


def keypress_shift_v():
    subprocess.call(['wmctrl', '-a', 'X-System'])
    time.sleep(1)
    pyautogui.keyDown('shift')
    pyautogui.press('v')
    pyautogui.keyUp('shift')

def keypress_shift_b():
    subprocess.call(['wmctrl', '-a', 'X-System'])
    time.sleep(1)
    pyautogui.keyDown('shift')
    pyautogui.press('b')
    pyautogui.keyUp('shift')


def delay_for_me(duration):
    if duration != 10000:
        if duration == 1:
            keypress_shift_v()
        else:
            time.sleep(duration-1)
            keypress_shift_b()
    else:
        keypress_shift_b()


if __name__ == "__main__":
    #ToDo: Change from pyautogui to xdotool to allow multitasking
    pyautogui.FAILSAFE = False
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.bind(('localhost', 50000))
    s.listen(5)
    while 1:
        conn, addr = s.accept()
        data = conn.recv(1024)
        if not data:
            pass
        else:
            delay_time = int(data)
            delay_for_me(delay_time)
            # conn.send("ack")
    conn.close()
