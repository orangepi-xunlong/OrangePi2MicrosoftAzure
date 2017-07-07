#
# OrangePi to Cognative Service
# 
# Face 
#    -> How Old are U
#
from Tkinter import *
import tkMessageBox
import httplib, urllib, base64
import tkFileDialog
import os

input_path  = "/var/www/html/Before.jpeg"
output_path = "/var/www/html/After.jpeg"

#
# Connect Cognitive Service and get result of Face
#
def OrangePi_Face(image):
    
    headers = {
        # Request headers
        'Content-Type': 'application/octet-stream',
        'Ocp-Apim-Subscription-Key': '',
    }
   
 
    params = urllib.urlencode({
    # Request parameters
    'returnFaceId': 'true',
    'returnFaceLandmarks': 'false',
    'returnFaceAttributes': 'age',
    })

    fp = open("/etc/OrangePi/OrangePi_2_CognitiveService_Face.conf",'r')
    line = fp.read(32)
    headers['Ocp-Apim-Subscription-Key'] = line
    fp.close()
    data = open(image, 'rb').read()
    
    try:
        conn = httplib.HTTPSConnection('api.projectoxford.ai')

        conn.request("POST", "/face/v1.0/detect?%s" %params , data , headers)

        response = conn.getresponse()
        data = response.read()
        data = eval(data)  # type list
#	print data # For debuging data
        conn.close()
    except Exception as e:
        print("[Errno {0}] {1}".format(e.errno, e.strerror))

    return data

def OrangePi_Emotion(image):

    headers = {
        # Request headers
        'Content-Type': 'application/octet-stream',
        'Ocp-Apim-Subscription-Key': '',
    }

    fp = open("/etc/OrangePi/OrangePi_2_CognitiveService_Emotion.conf",'r')
    line = fp.read(32)
    headers['Ocp-Apim-Subscription-Key'] = line
    fp.close()
    data = open(image, 'rb').read()

    try:
        conn = httplib.HTTPSConnection('api.projectoxford.ai')

        conn.request("POST", "/emotion/v1.0/recognize" , data , headers)

        response = conn.getresponse()

        data = response.read()
        data = eval(data)  # type list
        conn.close()
    except Exception as e:
        print("[Errno {0}] {1}".format(e.errno, e.strerror))

    return data

#
# Simple Rate algorithm
#
def cmp(a,b,prec):
    if a > b:
	if a - b < prec:
	    return 1
	else:
	    return 0
    else:
	if b - a < prec:
	    return 1
	else:
	    return 0
def OrangePi_rate(w0,h0,t0,l0,w1,h1,t1,l1,prec):
    return cmp(w0,w1,prec) and cmp(h0,h1,prec) and cmp(t0,t1,prec) and cmp(l0,l1,prec)
 
def OrangePi_Store(responseFace , responseEmotion , image):

    import cv2
    import operator

    color = (255,0,0)
    font = cv2.FONT_HERSHEY_DUPLEX
    sortField = operator.itemgetter(1)
    textGap = 15

    im = cv2.imread(image)
    for rect in responseFace:
        height = rect["faceRectangle"]["height"]
        width  = rect["faceRectangle"]["width"]
        left   = rect["faceRectangle"]["left"]
        top    = rect["faceRectangle"]["top"]
        start_point = (rect["faceRectangle"]["left"],rect["faceRectangle"]["top"])
        end_point = ( start_point[0] + width , start_point[1] + height )
        cv2.rectangle(im, start_point, end_point, color, 2)

	# Face
        FaceAttribute = rect["faceAttributes"]
        FaceAttribute = sorted(FaceAttribute.iteritems(),key=sortField,reverse=True)

	# Drow Face 
        cnt = 1
        for tup in FaceAttribute:
            cv2.putText(im, "%s %d" % ( tup[0] ,tup[1]) ,( start_point[0],  start_point[1] + textGap * cnt ), font, 1.9,(0,0,255),1)
            cnt += 1

	# Drow Emotion
	for bec in responseEmotion:
	    Eheight = bec["faceRectangle"]["height"]
	    Ewidth  = bec["faceRectangle"]["width"]
	    Eleft   = bec["faceRectangle"]["left"]
	    Etop    = bec["faceRectangle"]["top"]
	    
	    # Emotion
	    Emotion = bec["scores"]
	    Emotion = sorted(Emotion.iteritems(),key=sortField,reverse=True)

	    if OrangePi_rate(height,width,left,top,Eheight,Ewidth,Eleft,Etop,3):
		cnt = 1
		for tupm in Emotion:
		    cv2.putText(im, "%s %.2f" % ( tupm[0] ,tupm[1]) ,( start_point[0], end_point[1] + textGap * cnt * 3 ), font, 1.9,(0,0,255),1)
		    cnt += 1

    cv2.imwrite(output_path, im, [int(cv2.IMWRITE_JPEG_QUALITY), 100])
    cv2.waitKey(0)
    cv2.destroyAllWindows()

class OrangePi_2_CognitiveService():
    def FaceEmotion(self):
        image = input_path
        response0 =  OrangePi_Face(image)
	response1 =  OrangePi_Emotion(image)
	print "OrangePi process image....."
        OrangePi_Store(response0 , response1 , image)

if __name__ == '__main__':
    print "OrangePi Connect Cognitive Service...."
    OrangePi_2_CognitiveService().FaceEmotion()
    print "OrangePi Finish Operation..."


