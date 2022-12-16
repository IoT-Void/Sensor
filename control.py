import sys
import paho.mqtt.client as mqtt
from influxdb import client as influxdb
db = influxdb.InfluxDBClient('3.223.99.135',8086,'iot','iot','mydb')
topic1 = "id/Void/LCR/#"
topic2 = "id/Void/KoneJ/#"
topic3 = "id/Void/Heesane/#"
server = "3.223.99.135"
global cnt,check_a,check_b,check_list,students
cnt = 0
check_a = 0
check_b = 0
check_list = list()
students = 0

def on_connect(client, userdata, flags, rc):
    print("Connected with RC : " + str(rc))
    client.subscribe(topic1)
    client.subscribe(topic2)
    client.subscribe(topic3)
 
def on_message(client, userdata, msg):
    global cnt,check_a,check_b,check_list,students
    first = list()
    second = list()
    measurment = float(msg.payload.decode('utf-8'))
    ## 바깥쪽
    if(msg.topic == "id/Void/Heesane/Outside"):
        print("Outside :",measurment)
        first.append(measurment)
        if(measurment < 100):
            check_list.append('a')
            cnt += 1
            check_a = 1
        
    ## 안쪽
    if(msg.topic == "id/Void/KoneJ/Inside"):
        print("Inside :",measurment)
        second.append(measurment) 
        if(measurment < 100):
            check_list.append('b')
            cnt +=1
            check_b = 1
 
    ## 1초마다의 미세먼지
    if(msg.topic == "id/Void/LCR/Dust"):
        db.write_points([{"measurement":"host",'tags':{"host":"dust","region":"us-north"},'fields':{'value':measurment}}])

    ## 평균 미세먼지
    if(msg.topic == "id/Void/LCR/Avg_dust"):
        db.write_points([{"measurement":"host",'tags':{"host":"Avg_dust","region":"us-north"},'fields':{'value':measurment}}])

    ## 습도
    if(msg.topic == "id/Void/Heesane/Humidity"):
        db.write_points([{"measurement":"host",'tags':{"host":"humidity","region":"us-north"},'fields':{'value':measurment}}])

    ## 온도
    if(msg.topic == "id/Void/Heesane/Temperature"):
        db.write_points([{"measurement":"host",'tags':{"host":"temperature","region":"us-north"},'fields':{'value':measurment}}])

    if(check_a == 1 and check_b == 1):
        if(check_list[cnt-2] == 'a' and check_list[cnt-1] == 'b'):
            students +=1
        elif(check_list[cnt-2] == 'b' and check_list[cnt-1] == 'a'):
            if(students == 0):
                students =0
            else :
                students -=1
        check_a = 0
        check_b = 0 
        print("사람의 수는 ",students,"명 입니다.")    
        students = float(students)
        db.write_points([{"measurement":"host",'tags':{"host":"check","region":"us-north"},'fields':{'value':students}}])





client = mqtt.Client()
client.connect(server, 1883, 60)
client.on_connect = on_connect
client.on_message = on_message

client.loop_forever()