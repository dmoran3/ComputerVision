import cv2
import numpy as np
import pyautogui
# pip install pywin32
import win32api, win32con, win32gui
import time
import math
from mss import mss
from PIL import Image


INPUT_WIDTH = 320
INPUT_HEIGHT = 320

SCORE_THRESHOLD = 0.5
CONFIDENCE_THRESHOLD = 0.6
NMS_THRESHOLD = 0.45
size_scale = 1


net = cv2.dnn.readNet("best.onnx")
net.setPreferableBackend(cv2.dnn.DNN_BACKEND_OPENCV)
#net.setPreferableBackend(cv2.dnn.DNN_BACKEND_CUDA)
#net.setPreferableTarget(cv2.dnn.DNN_TARGET_CUDA)

ln = net.getLayerNames()
#print(net.getUnconnectedOutLayers()[0])
ln = [ln[i - 1] for i in net.getUnconnectedOutLayers()]

window = win32gui.FindWindow(None, 'League of Legends (TM) Client')
rect = win32gui.GetWindowRect(window)
#region = rect[0], rect[1], rect[2] - rect[0], rect[3] - rect[1]
region = {'top': rect[1], 'left': rect[0], 'width': rect[2] - rect[0], 'height': rect[3] - rect[1]}
#print(region)

#lux_region = None
lux_found = False
q_found = False

re_cap = 0
while True:
    #time1 = time.time()
    re_cap += 1
    if re_cap >= 15:
        re_cap = 0
        window = win32gui.FindWindow(None, 'League of Legends (TM) Client')
        rect = win32gui.GetWindowRect(window)
        region = {'top': rect[1], 'left': rect[0], 'width': rect[2] - rect[0], 'height': rect[3] - rect[1]}
    # Get image of screen
    
    with mss() as sct:
        sct_img = sct.grab(region)
        # Create the Image
        img = Image.frombytes("RGB", sct_img.size, sct_img.bgra, "raw", "BGRX")
        frame = np.array(img)
    
    # To show frame in Python window at the last part
    original_frame = np.copy(frame)
    # if you want see black box at the center of screen use this code,
    #original_frame = frame
    
    # To hide my character , draw black rect
    my_character_box_size_x = 76
    my_character_box_size_y = 100
    top_left_x = region['width'] // 2 - my_character_box_size_x // 2 - 70
    top_right_y = region['height'] // 2 - my_character_box_size_y // 2 - 40
    cv2.rectangle(frame, (top_left_x,top_right_y), (top_left_x + my_character_box_size_x , top_right_y + my_character_box_size_y), (0, 0, 0), -1)
    
    
    
    frame_height, frame_width = frame.shape[:2]
    
    # Detection
    blob = cv2.dnn.blobFromImage(frame, 1 / 255.0, size=(INPUT_WIDTH, INPUT_HEIGHT), crop=False)
    #print(frame_height, frame_width)
    #print(blob.shape)
    net.setInput(blob)
    outputs = net.forward(net.getUnconnectedOutLayersNames())
    
    class_ids = []
    boxes = []
    confidences = []
    rows = outputs[0].shape[1]
    x_factor = frame_width / INPUT_WIDTH
    y_factor = frame_height / INPUT_HEIGHT

    
    for r in range(rows):
        row = outputs[0][0][r]
        confidence = row[4]
        # Discard bad detections and continue.

        if confidence >= CONFIDENCE_THRESHOLD:
            classes_scores = row[5:]
            # Get the index of max class score.
            class_id = np.argmax(classes_scores)
            
        #  Continue if the class score is above threshold.
            if (classes_scores[class_id] > SCORE_THRESHOLD):
                #print(class_id, confidence, classes_scores[class_id])
                class_ids.append(class_id)
                cx, cy, w, h = row[0], row[1], row[2], row[3]
                left = int((cx - w/2) * x_factor)
                top = int((cy - h/2) * y_factor)
                width = int(w * x_factor)
                height = int(h * y_factor)
                box = np.array([left, top, width, height])
                boxes.append(box)
                confidences.append(float(confidence))
    #time2 = time.time()
    #print('function took {:.3f} ms'.format((time2-time1)*1000.0))
    
    
    indices = cv2.dnn.NMSBoxes(boxes, confidences, CONFIDENCE_THRESHOLD, NMS_THRESHOLD)
    
    
    
    if len(indices) > 0:
        #print(f"Detected:{len(indices)}")
        
        picked_class_ids = []
        class_0_max_confidence = 0
        class_0_max_index = -1
        class_1_max_confidence = 0
        class_1_max_index = -1
        for i in indices.flatten():
            if class_ids[i] == 0:
                if confidences[i] > class_0_max_confidence:
                    class_0_max_confidence = confidences[i]
                    class_0_max_index = i
            elif class_ids[i] == 1:
                if confidences[i] > class_1_max_confidence:
                    class_1_max_confidence = confidences[i]
                    class_1_max_index = i
        
        if class_0_max_index != -1: # class 0 found
            (x, y) = (boxes[class_0_max_index][0], boxes[class_0_max_index][1])
            (w, h) = (boxes[class_0_max_index][2], boxes[class_0_max_index][3])
            
            cv2.rectangle(original_frame, (x, y), (x + w, y + h), (255, 255, 255), 2)
            
            lux_center_x = int(x + (w/2))
            lux_center_y = int(y + (h/2))
            lux_center = (lux_center_x, lux_center_y)
            lux_found = True
        else:
            lux_found = False
        
        
        if class_1_max_index != -1: # class 1 found
            (x, y) = (boxes[class_1_max_index][0], boxes[class_1_max_index][1])
            (w, h) = (boxes[class_1_max_index][2], boxes[class_1_max_index][3])
            print(x,y,w,h)
            cv2.rectangle(original_frame, (x, y), (x + w, y + h), (255, 0, 0), 2)
            q_center_x = int(x + (w/2))
            q_center_y = int(y + (h/2))
            q_center = (q_center_x, q_center_y)
            q_found = True
        else:
            q_found = False
    
    #test Trajectory calc
    if lux_found and q_found:
        cv2.line(original_frame, lux_center, q_center, (255, 255, 0), 5)
        #draw a line between them
        
    
    original_frame = cv2.cvtColor(original_frame, cv2.COLOR_BGR2RGB)
    original_frame = cv2.resize(original_frame, (original_frame.shape[1] // size_scale, original_frame.shape[0] // size_scale))
    cv2.imshow("frame", original_frame)
    cv2.waitKey(1)
    

def dodgeSkill(frame, lux_center, q_center):
    
    
    cv2.line(frame, lux_center, q_center * 2, (0, 0, 255), 1)
    cv2.line(frame, lux_center, q_center, (255, 255, 0), 1)
    '''
    character_trajectory = getTrajectory(lux_center, ourCharacter_center)
    character_trajectory_length = sqrt((character_trajectory[0] * character_trajectory[0]) + (character_trajectory[1] * character_trajectory[1]))
    
    scale_factor = character_trajectory_length / skillshot_trajectory_length
    
    scaled_skill_trajectory = [(skillshot_trajectory[0] * scale_factor), (skillshot_trajectory[1] * scale_factor)]
    

    #distance between trajectory of attack and character
    x_dist_from_skill = scaled_skill_trajectory[0] - character_trajectory[0]
    y_dist_from_skill = scaled_skill_trajectory[1] - character_trajectory[1]
    distance_from_skill = sqrt((x_dist_from_skill * x_dist_from_skill) + (y_dist_from_skill * y_dist_from_skill))
    
    if distance_from_skill < lux_q_skillshot.width:
        if x_dist_from_skill < 0:
            #skill will land to our left, so move right
            x_comp = lux_q_skillshot.width
        else:
            #move left if skill will land to our right or directly on us
            x_comp = 0-lux_q_skillshot.width
            
        if y_dist_from_skill < 0:
            y_comp = lux_q_skillshot.height
        else:
            y_comp = 0 - lux_q_skillshot.height
   
    x_comp = 40
    y_comp = 40
    #input move command to (char_loc_x + x_comp, char_loc_y + y_comp)
    x_dest = char_loc_x + x_comp
    y_dest = char_loc_y + y_comp
    
    #pg.click(x = x_dest, y =  y_dest, button = 'right')
     '''