
'''
Pyautogui object Win32Window(hWnd=1445060) functions
['__class__', '__delattr__', '__dir__', '__eq__', '__format__', '__ge__', '__getattribute__', '__gt__', '__init__', '__init_subclass__', '__le__', '__lt__', '__ne__', '__new__', '__reduce__', '__reduce_ex__', '__repr__', '__setattr__', '__sizeof__', '__str__', '__subclasshook__', '_getWindowRect', '_setupRectProperties', 'activate', 'close', 'hide', 'maximize', 'minimize', 'move', 'moveRel', 'moveTo', 'resize', 'resizeRel', 'resizeTo', 'restore', 'show']

'''

# mouse location: pyautogui.position()
import time
import pyautogui

LEFT_ = 'left'
RIGHT_ ='right'
center_x = 890
center_y = 530 # 890,530 = center with 1920*1080 LoL Client Window

def mouse_click(button_l_r, x, y):
    pyautogui.click(x,y, button=button_l_r)
    pyautogui.mouseUp(button=button_l_r)

def key_click(key, x, y):
    print('key_click')
    pyautogui.moveTo(x,y)
    time.sleep(0.05)
    pyautogui.keyDown(key)
    time.sleep(0.1)
    pyautogui.keyUp(key)

def activation(act, x, y):
    time.sleep(0.03)
    if act in [LEFT_, RIGHT_]:
        func = lambda act,x,y : mouse_click(act,x,y)
    else:
        func = lambda act,x,y : key_click(act,x,y)
    func(act,x,y)
    time.sleep(1)
    if x < center_x:
        new_x = center_x + (center_x - x)
    else:
        new_x = center_x - (x - center_x)
    if y < center_y:
        new_y = center_y + (center_y - y)
    else:
        new_y = center_y - (y - center_y)
    mouse_click(RIGHT_,new_x,new_y)


# Assume that LoL client is Windows mode AND default size = 1920 * 1080
win = pyautogui.getWindowsWithTitle("League of Legends (TM) Client")[0]
win.moveTo(0,0) # move the window to the top left 
time.sleep(0.1)
pyautogui.click(center_x,center_y, button='left') # get LoL client window handle.  

# movement test - works
for i in range(3):
    activation(RIGHT_,930,600)
    print(i)
for i in range(3):
    activation(RIGHT_,750,400)
    print(i)
print('movement test end')


# skill test(w) - does not work
'''
for i in range(3):
    activation('w',750,400)
    activation('w',930,600)
    print(i)
print('skill test end')
'''