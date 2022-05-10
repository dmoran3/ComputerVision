import cv2
import numpy as np
import pyautogui as pg

scshot = pg.screenshot(region=(0,0, 1930, 1090)) # lOL = 34

scshot = cv2.cvtColor(np.array(scshot), cv2.COLOR_RGB2BGR)

# locate a single object in a screenshot
lux = pg.locateOnScreen('lux_01.png', confidence=0.5)# or pg.locateOnScreen('lux_02.png', confidence=0.5),or pg.locateOnScreen('lux_03.png', confidence=0.5),or pg.locateOnScreen('lux_04.png', confidence=0.5)
#where lux_02, 03, and 04 are the different directions lux may be facing
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

# Dan 5/10
lux = pg.screenshot(region=(1530,200, 200, 250))
scshot = cv2.cvtColor(np.array(scshot), cv2.COLOR_RGB2BGR)

while True: #we should replace this later
    lux = pg.locateOnScreen('lux_01.png', confidence=0.5)
    #lux = Box(left=lux.left, top=lux.top, width=lux.width, height=lux.height)
    lux_center = getCenter(lux)
    
    lux_q_skillshot = pg.locateOnScreen('lux_q_skillshot_01.png', confidence=0.5)
    #lux_q_skillshot = Box(left=lux_q_skillshot.left, top=lux_q_skillshot.top, width=lux_q_skillshot.width, height=lux_q_skillshot.height)
    lux_q_skillshot_center = getCenter(lux_q_skillshot)
    
    skillshot_trajectory = getTrajectory(lux_center, lu_q_skillshot_center)
    
    ourCharacter = lux = pg.locateOnScreen('PALCEHOLDER.png', confidence=0.5)
    #ourCharacter = Box(left=ourCharacter.left, top=ourCharacter.top, width=ourCharacter.width, height=ourCharacter.height)
    ourCharacter_center = getCenter(ourCharacter)
    
    character_trajectory = getTrajectory(lux_center, ourCharacter_center)
    
    #if the trajectories are similar enough: (maybe decide this based on size of Q hitbox and character hitbox)
        #issue movement command perpendicular to skillshot trajectory
        #and move sqrt(skillshot.width^2 + skillshot.height^2) distance in that direction
    
def getCenter(obj):
    center_x = obj.left + (obj.width/2)
    center_y = obj.top + (obj.height/2)
    center = [center_x, center_y]
    return center

def getTrajectory(obj1, obj2):
    traj_x = obj1[0] - obj2[0]
    traj_y = obj1[1] - obj2[1]
    traj = [traj_x, traj_y]
    return traj


'''
https://pyautogui.readthedocs.io/en/latest/screenshot.html


>>> import pyautogui
>>> button7location = pyautogui.locateOnScreen('calc7key.png')
>>> button7location
Box(left=1416, top=562, width=50, height=41)
>>> button7location[0]
1416
>>> button7location.left
1416
>>> button7point = pyautogui.center(button7location)
>>> button7point
Point(x=1441, y=582)
>>> button7point[0]
1441
>>> button7point.x
1441
>>> button7x, button7y = button7point
>>> pyautogui.click(button7x, button7y)  # clicks the center of where the 7 button was found
>>> pyautogui.click('calc7key.png') # a shortcut version to click on the center of where the 7 button was found
'''
cv2.imshow('Screenshot', scshot)

cv2.waitKey(0)

cv2.destoryAllWindows()

