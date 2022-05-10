import cv2
import numpy as np
import pyautogui as pg

scshot = pg.screenshot(region=(0,0, 1930, 1090)) # lOL = 34

scshot = cv2.cvtColor(np.array(scshot), cv2.COLOR_RGB2BGR)

# locate a single object in a screenshot
lux = pg.locateOnScreen('lux_01.png', confidence=0.5)
print(lux)
# result ====> Box(left=1544, top=229, width=84, height=125)

''' 
# ==== EXAMPLE Algo
while True:
    lux = pg.screenshot(region=(1530,200, 200, 250))
    scshot = cv2.cvtColor(np.array(scshot), cv2.COLOR_RGB2BGR)
    lux = pg.locateOnScreen('lux_01.png', confidence=0.5)
    # result ====> Box(left=1500, top=250, width=84, height=125)
    
    q_skill_lux =  pg.locateOnScreen('lux_q_skill.png', confidence=0.5)
    # q_skill_lux => Box(left=1580, top=255, width=30, height=50)
    # 
    if q_skill_lux is None:
        # lux did not use skill  during the interval time
    else:
        # lux used skill during the interval time
    
    time.wait(0.5) # 0.5sec interval
'''

cv2.imshow('Screenshot', scshot)

cv2.waitKey(0)

cv2.destoryAllWindows()