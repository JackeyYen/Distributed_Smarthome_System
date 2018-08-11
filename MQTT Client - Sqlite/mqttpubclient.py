#-*- coding: utf-8 -*-
#!/usr/bin/python

import paho.mqtt.client as mqtt
import sqlite3
import time

#设置联机服务器的ip地址
MQTTHOST = "localhost"
MQTTPORT = 1883
mqttClient = mqtt.Client()

#宣告字符串初始变量为0
str1 = '0'
str2 = '0'

#连接MQTT服务器
def on_mqtt_connect():
    mqttClient.connect(MQTTHOST, MQTTPORT, 60)
    mqttClient.loop_start()

#publish消息
def on_publish(topic, payload, qos):
    mqttClient.publish(topic, payload, qos)

#每5秒执行一次
def sleeptime(hour,min,sec):
    return hour*3600 + min*60 + sec;
second = sleeptime(0,0,5);
while 1==1:
    time.sleep(second);

#连接或创建数据库
    conn = sqlite3.connect('db.sqlite3')
    c = conn.cursor()
    print ("Opened database successfully")

#创建表
    #c.execute('create table myhome_commands (id Integer primary key autoincrement , intent Text , slots Text)')
    #print ("create myhome_commands table success")
    #conn.commit()

#写入假data
    #c.execute("INSERT INTO myhome_commands (intent,slots) \
    #      VALUES ('AC1_OC_OPEN', 'open' )");
    #conn.commit()
    #print ("Records created successfully")

#读取sqlite数据库data
    cursor = c.execute("SELECT intent, slots from myhome_commands")
    for row in cursor:
       payload = '{"intent":"%s","slots":"%s","slaveID":3,"control":1,"command_first_byte":1,"command_second_byte":2,"command_third_byte":3,"command_fourth_byte":4}' \
       %(row[0], row[1])

#判断读取到的字符串是否有变换（语音控制的输出）
       if str1 in row[0]:
         if str2 in row[1]:
           print('not new command')
           break
         else:
           print('new command')
           str1 = row[0]
           str2 = row[1]
           on_mqtt_connect()
           on_publish("0/node/commands/control", payload, 1)
       else:
         print('new command')
         str1 = row[0]
         str2 = row[1]
         on_mqtt_connect()
         on_publish("0/node/commands/control", payload, 1)

#       payload = '{"intent":"%s","slots":"%s","slaveID":3,"control":1,"command_first_byte":1,"command_second_byte":2,"command_third_byte":3,"command_fourth_byte":4}' \
#       %(row[0], row[1])
       print (payload)

    print ("Operation done successfully")
    conn.close()

#    on_mqtt_connect()
#    on_publish("0/node/commands/control", payload, 1)

def main():
    sleeptime()
    while True:
        pass

if __name__ == '__main__':
    main()
