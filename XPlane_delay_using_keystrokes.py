import pyautogui
import socket
import time


def keypress_shift_v():
    pyautogui.keyDown('shift')
    pyautogui.press('v')
    pyautogui.keyUp('shift')


def keypress_shift_b():
    pyautogui.keyDown('shift')
    pyautogui.press('b')
    pyautogui.keyUp('shift')


def delay_for_me(duration):
    time.sleep(duration)
    if duration == 1:
        keypress_shift_v()
    else:
        keypress_shift_b()


if __name__ == "__main__":
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
