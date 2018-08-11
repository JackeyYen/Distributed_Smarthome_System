# -*- coding: utf-8 -*-
#!/usr/bin/python

import paho.mqtt.client as mqtt
import sqlite3
import json
import time

#设置订阅的topic
def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))
    client.subscribe(str(rc) + "/node/data/sensors")

#显示订阅到的data
def on_message(client, userdata, msg):
    print(msg.topic+" " + ":" + str(msg.payload))

#存成json格式
    sline = json.loads(msg.payload.decode('utf-8'))

#连接或创建数据库
    conn = sqlite3.connect('db.sqlite3')
    c = conn.cursor()
    print ("Opened database successfulliy")

#创建表
#    c.execute('create table myhome_nodedata (id Integer primary key autoincrement , time Text , localshortaddr Text , gateway_id Text , slaveId Text , humidity Integer , temperature Integer , light Integer , noise Integer , co2_simulation Integer , co2_binarization Integer)')
#    print ("create myhome_nodedata table success")
#    conn.commit()

#写入data
    sql = "insert into myhome_nodedata(time,localshortaddr, gateway_id,slaveId, humidity, temperature,light, noise, co2_simulation, co2_binarization)values('%s','%s','%s','%s',%f,%f,%f,%f,%f,%f)" % (time.strftime('%Y-%m-%d/%H:%M:%S'),"F5A1","0","1",sline["humidity"],sline["temperature"],sline["light"],sline["noise"],sline["co2_simulation"],sline["co2_binarization"])
    conn.execute(sql)
    conn.commit()
    print ("Records created successfully insert into myhome_nodedata values")
    c.close()

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
#设置联机服务器的ip地址
client.connect("localhost", 1883, 30)
client.loop_forever()
