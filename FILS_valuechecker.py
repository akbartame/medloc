import paho.mqtt.client as mqtt
import sqlite3
import subprocess
import datetime
import os
import statistics
import re
import csv
import time
import shutil
import threading
import math
import schedule
import pandas as pd
import traceback
import requests
import json
from config import bot_url, LOG_FILE, path, database, broker_address, port, PERIOD
from pathlib import Path
from statistics import variance


# set parameter for detection
now = datetime.datetime.now()
today = now.strftime("%Y-%m-%d")
#PERIOD = 5  # duration in seconds
TX_DELAY_TIME = 0.5  # delay for every data in seconds
LQI_MIN_VALUE = 5.00
LQI_threshold = 89

# set database
conn = sqlite3.connect(database, check_same_thread=False)
cursor = conn.cursor()
lock = threading.Lock()


# create table if not exist (5rcv)
cursor.execute(
    'CREATE TABLE IF NOT EXISTS client1(ts INTEGER, id INTEGER, lq INTEGER, x INTEGER, y INTEGER, z INTEGER)')
cursor.execute(
    'CREATE TABLE IF NOT EXISTS client2(ts INTEGER, id INTEGER, lq INTEGER, x INTEGER, y INTEGER, z INTEGER)')
cursor.execute(
    'CREATE TABLE IF NOT EXISTS client3(ts INTEGER, id INTEGER, lq INTEGER, x INTEGER, y INTEGER, z INTEGER)')
cursor.execute(
    'CREATE TABLE IF NOT EXISTS client4(ts INTEGER, id INTEGER, lq INTEGER, x INTEGER, y INTEGER, z INTEGER)')
cursor.execute(
    'CREATE TABLE IF NOT EXISTS client5(ts INTEGER, id INTEGER, lq INTEGER, x INTEGER, y INTEGER, z INTEGER)')


# Select Fingerprint Data from DB
# select data from db, and convert to dataframe
fingerprint_data = pd.read_sql_query("SELECT * FROM fingerprint", conn)
test_data = ""
#valueChecker = 0
lqDroppedIdentifier = 0
# droppedToFive = False
# lq1Dropped = False 
# lq2Dropped = False 
# lq3Dropped = False 
# lq4Dropped = False 
# lq5Dropped = False 
# lqDroppedIdentifier = 0



# called function when there is a callback
def client1(client, userdata, message):
    # print("fromclient1")
    msg = str(message.payload.decode("utf-8"))
    # filter the necessary data
    filter_data = msg.replace("ts=", "")
    filter_data = filter_data.replace("id=", "")
    filter_data = filter_data.replace("lq=", "")
    filter_data = filter_data.replace("x=", "")
    filter_data = filter_data.replace("y=", "")
    filter_data = filter_data.replace("z=", "")
    filter_data = filter_data.replace(",", "")

    # split data, output is a 'list'
    clean_data = filter_data.split(":")

    if len(clean_data) != 0:
        lock.acquire(True)
        try:
            # checking data if integer or not
            try:
                ID = int(clean_data[6])
                lq = int(clean_data[3])
                x = int(clean_data[10])
                y = int(clean_data[11])
                z = int(clean_data[12])
            # if not integer
            except (ValueError, IndexError):
                ID = 0
                lq = 0
                x = 0
                y = 0
                z = 0
            cursor.execute('INSERT INTO client1(ts,id,lq,x,y,z) VALUES(?,?,?,?,?,?)', ("",
                                                                                       ID, lq, x, y, z))
            conn.commit()

        except:
            try:
                # checking data if integer or not
                ts = int(clean_data[2])
            # if not
            except ValueError:
                ts = 0
            cursor.execute('INSERT INTO client1(ts,id,lq,x,y,z) VALUES(?,?,?,?,?,?)',
                           (ts, "", "", "", "", ""))
            conn.commit()
        lock.release()


def client2(client, userdata, message):
    # prinraw_lqit("fromclient2")
    msg = str(message.payload.decode("utf-8"))
    # filter the necessary data
    filter_data = msg.replace("ts=", "")
    filter_data = filter_data.replace("id=", "")
    filter_data = filter_data.replace("lq=", "")
    filter_data = filter_data.replace("x=", "")
    filter_data = filter_data.replace("y=", "")
    filter_data = filter_data.replace("z=", "")
    filter_data = filter_data.replace(",", "")

    # split data, output is a 'list'
    clean_data = filter_data.split(":")
    # print(clean_data)
    # insert data from 'list' to database based on the index
    # if the output data from db is not correct, change the index
    if len(clean_data) != 0:
        lock.acquire(True)
        try:
            # checking data if integer or not
            try:
                ID = int(clean_data[6])
                lq = int(clean_data[3])
                x = int(clean_data[10])
                y = int(clean_data[11])
                z = int(clean_data[12])
            # if not integer
            except (ValueError, IndexError):
                ID = 0
                lq = 0
                x = 0
                y = 0
                z = 0
            cursor.execute('INSERT INTO client2(ts,id,lq,x,y,z) VALUES(?,?,?,?,?,?)', ("",
                                                                                       ID, lq, x, y, z))
            conn.commit()

        except:
            # checking data if integer or not
            try:
                ts = int(clean_data[2])
            # if not
            except ValueError:
                ts = 0
            cursor.execute('INSERT INTO client2(ts,id,lq,x,y,z) VALUES(?,?,?,?,?,?)',
                           (ts, "", "", "", "", ""))
            conn.commit()
        lock.release()


def client3(client, userdata, message):
    # prinraw_lqit("fromclient3")
    msg = str(message.payload.decode("utf-8"))
    # filter the necessary data
    filter_data = msg.replace("ts=", "")
    filter_data = filter_data.replace("id=", "")
    filter_data = filter_data.replace("lq=", "")
    filter_data = filter_data.replace("x=", "")
    filter_data = filter_data.replace("y=", "")
    filter_data = filter_data.replace("z=", "")
    filter_data = filter_data.replace(",", "")

    # split data, output is a 'list'
    clean_data = filter_data.split(":")

    # insert data from 'list' to database based on the index
    # if the output data from db is not correct, change the index
    if len(clean_data) != 0:
        lock.acquire(True)
        try:
            # checking data if integer or not
            try:
                ID = int(clean_data[6])
                lq = int(clean_data[3])
                x = int(clean_data[10])
                y = int(clean_data[11])
                z = int(clean_data[12])
            # if not integer
            except (ValueError, IndexError):
                ID = 0
                lq = 0
                x = 0
                y = 0
                z = 0
            cursor.execute('INSERT INTO client3(ts,id,lq,x,y,z) VALUES(?,?,?,?,?,?)', ("",
                                                                                       ID, lq, x, y, z))
            conn.commit()

        except:
            # checking data if integer or not
            try:
                ts = int(clean_data[2])
            # if not
            except ValueError:
                ts = 0
            cursor.execute('INSERT INTO client3(ts,id,lq,x,y,z) VALUES(?,?,?,?,?,?)',
                           (ts, "", "", "", "", ""))
            conn.commit()
        lock.release()


def client4(client, userdata, message):
    # print("fromclient4")
    msg = str(message.payload.decode("utf-8"))
    # filter the necessary data
    filter_data = msg.replace("ts=", "")
    filter_data = filter_data.replace("id=", "")
    filter_data = filter_data.replace("lq=", "")
    filter_data = filter_data.replace("x=", "")
    filter_data = filter_data.replace("y=", "")
    filter_data = filter_data.replace("z=", "")
    filter_data = filter_data.replace(",", "")

    # split data, output is a 'list'
    clean_data = filter_data.split(":")
    # print(clean_data)

    # insert data from 'list' to database based on the index
    # if the output data from db is not correct, change the index
    if len(clean_data) != 0:
        lock.acquire(True)
        try:
            # checking data if integer or not
            try:
                ID = int(clean_data[6])
                lq = int(clean_data[3])
                x = int(clean_data[10])
                y = int(clean_data[11])
                z = int(clean_data[12])
            # if not integer
            except (ValueError, IndexError):
                ID = 0
                lq = 0
                x = 0
                y = 0
                z = 0
            cursor.execute('INSERT INTO client4(ts,id,lq,x,y,z) VALUES(?,?,?,?,?,?)', ("",
                                                                                       ID, lq, x, y, z))
            conn.commit()

        except:
            # checking data if integer or not
            try:
                ts = int(clean_data[2])
            # if not
            except ValueError:
                ts = 0
            cursor.execute('INSERT INTO client4(ts,id,lq,x,y,z) VALUES(?,?,?,?,?,?)',
                           (ts, "", "", "", "", ""))
            conn.commit()
        lock.release()


# 5rcv
def client5(client, userdata, message):
    # print("fromclient4")
    msg = str(message.payload.decode("utf-8"))
    # filter the necessary data
    filter_data = msg.replace("ts=", "")
    filter_data = filter_data.replace("id=", "")
    filter_data = filter_data.replace("lq=", "")
    filter_data = filter_data.replace("x=", "")
    filter_data = filter_data.replace("y=", "")
    filter_data = filter_data.replace("z=", "")
    filter_data = filter_data.replace(",", "")

    # split data, output is a 'list'
    clean_data = filter_data.split(":")
    # print(clean_data)

    # insert data from 'list' to database based on the index
    # if the output data from db is not correct, change the index
    if len(clean_data) != 0:
        lock.acquire(True)
        try:
            # checking data if integer or not
            try:
                ID = int(clean_data[6])
                lq = int(clean_data[3])
                x = int(clean_data[10])
                y = int(clean_data[11])
                z = int(clean_data[12])
            # if not integer
            except (ValueError, IndexError):
                ID = 0
                lq = 0
                x = 0
                y = 0
                z = 0
            cursor.execute('INSERT INTO client5(ts,id,lq,x,y,z) VALUES(?,?,?,?,?,?)', ("",
                                                                                       ID, lq, x, y, z))
            conn.commit()

        except:
            # checking data if integer or not
            try:
                ts = int(clean_data[2])
            # if not
            except ValueError:
                ts = 0
            cursor.execute('INSERT INTO client5(ts,id,lq,x,y,z) VALUES(?,?,?,?,?,?)',
                           (ts, "", "", "", "", ""))
            conn.commit()
        lock.release()


# SEND MESSAGE TO TELEGRAM USER
def sendMessage(chatID, text):
    parameter = {'chat_id': chatID, 'text': text}
    message = requests.post(bot_url + 'sendMessage', data=parameter)
    return message


# called when the broker responds to our connection request
def on_connect(client, userdata, flags, rc):
    print("Connected - rc:", rc)


# Called when the broker responds to a subscribe request.
def on_subscribe(client, userdata, mid, granted_qos):
    print("Subscribed:", str(mid), str(granted_qos))


# fungsi export data
def exportData():
    now = datetime.datetime.now()
    hour = now.hour
    minute = now.minute
    second = now.second

    # data tiap tabel langsung export jadi csv file
    db_df1 = pd.read_sql_query(
        "SELECT * FROM client1", conn)
    db_df1.to_csv(today+'\\r1-'+str(hour) + "_"+str(minute)+"_"+str(second) +
                  '.csv', index=False)

    db_df2 = pd.read_sql_query(
        "SELECT * FROM client2", conn)
    db_df2.to_csv(today+'\\r2-'+str(hour) + "_"+str(minute)+"_"+str(second) +
                  '.csv', index=False)

    db_df3 = pd.read_sql_query(
        "SELECT * FROM client3", conn)
    db_df3.to_csv(today+'\\r3-'+str(hour) + "_"+str(minute)+"_"+str(second) +
                  '.csv', index=False)

    db_df4 = pd.read_sql_query(
        "SELECT * FROM client4", conn)
    db_df4.to_csv(today+'\\r4-'+str(hour) + "_"+str(minute)+"_"+str(second) +
                  '.csv', index=False)

    # 5rcv
    db_df5 = pd.read_sql_query(
        "SELECT * FROM client5", conn)
    db_df5.to_csv(today+'\\r5-'+str(hour) + "_"+str(minute)+"_"+str(second) +
                  '.csv', index=False)

    #print("output finished")

    lock.acquire(True)
    # erase all data in the database after append to csv file
    cursor.execute('''DELETE FROM client1''')
    cursor.execute('''DELETE FROM client2''')
    cursor.execute('''DELETE FROM client3''')
    cursor.execute('''DELETE FROM client4''')
    cursor.execute('''DELETE FROM client5''')
    lock.release()
    #print("hapus data")


#------------------------------COLLECTION OF DETECTION FUNCTIONS-------------------------------------#

def now_time():
    now_time = datetime.datetime.now()
    hour = now_time.hour
    minute = now_time.minute
    second = now_time.second
    time = "-" + str(hour) + "_" + str(minute) + "_" + str(second) + ".csv"
    return time


# log
def logTime():
    now_time = datetime.datetime.now()
    hour = now_time.hour
    minute = now_time.minute
    second = now_time.second
    time = str(hour) + ":" + str(minute) + ":" + str(second)
    return time


def now_date():
    now_time = datetime.datetime.now()
    year = now_time.year
    month = now_time.month
    day = now_time.day
    if(day < 10):
        day = "0" + str(day)
    if(month < 10):
        month = "0" + str(month)
    date = str(year) + "-" + str(month) + "-" + str(day)
    return date


def numberID(Data1, Data2, Data3, Data4, Data5):
    num1 = list(set(Data1))
    num2 = list(set(Data2))
    num3 = list(set(Data3))
    num4 = list(set(Data4))
    num5 = list(set(Data5))  # if receivers increse, increce

    allnum = []
    for i in range(len(num1)):
        allnum.append(num1[i])
    for i in range(len(num2)):
        allnum.append(num2[i])
    for i in range(len(num3)):
        allnum.append(num3[i])
    for i in range(len(num4)):
        allnum.append(num4[i])
    # 5rcv
    for i in range(len(num5)):
        allnum.append(num5[i])  # if receivers increse, increce
    number = list(set(allnum))
    return number


# for collect data from csv file then parse it using pandas
def preData(file):  # make lq value list
    ac_list = []
    lq_list = []
    id_list = []

    with open(file, 'r', newline='') as infile:
        df = pd.read_csv(infile)
        raw_lqi = df.lq.fillna(0)
        raw_id = df.id.fillna(0)
        raw_x = df.x.fillna(0)
        raw_y = df.y.fillna(0)
        raw_z = df.z.fillna(0)
        pd.options.display.float_format = '{:,.0f}'.format

        for i in range(len(raw_x)):
            ac = math.sqrt(
                pow(abs(int(raw_x[i])), 2)+pow(abs(int(raw_y[i])), 2)+pow(abs(int(raw_z[i])), 2))
            if ac != 0:
                ac_list.append(ac)

        for data in raw_lqi:
            if data != 0:
                lq_list.append(data)

        for data in raw_id:
            if data != 0:
                id_list.append(data)

        # writer.writerow(raw_lqi)
        return id_list, lq_list, ac_list


# take average of LQ from CSV file of the sensor output
def dataProcess(lqlist, ac_list, num):  # calculate average of lq value and record
    global nomor
    # print("ini lqlist", lqlist)
    #print("ini ac_list", ac_list)
    lock.acquire(True)
    testData = cursor.execute('SELECT * FROM test_data')
    fetchData = testData.fetchall()
    lock.release()

    # jika jumlah data di dalam tabel > 0, nomor + 1 
    # data LQI sebelumnya disimpan 5 siklus untuk value checker
    if len(fetchData) > 0:
        lock.acquire(True)
        noData = cursor.execute(
            'SELECT no FROM test_data ORDER BY no DESC LIMIT 1')
        lock.release()
        lastRowNumber_raw = noData.fetchone()
        lastRowNumber = ''.join(map(str, lastRowNumber_raw))
        nomor = str(int(lastRowNumber) + 1)
        #print("ininomor", nomor)
    # jika tidak, maka nomor = 0
    else:
        nomor = 0
        #print("ininomor 0")

    # jika data nomor terakhir lebih dari 4, maka hapus rowData di nomor yang paling kecil
    if int(nomor) > 4:
        #print("data lebih dari 5, hapus data nomor:", int(nomor)-5)
        lock.acquire(True)
        cursor.execute(
            'DELETE FROM test_data WHERE no={}'.format(str(int(nomor)-5)))
        conn.commit()
        lock.release()
    #else:
        #print("belum sampe 5")

    # looping by transmitter range
    for i in range(len(num)):
        # insert row data
        lock.acquire(True)
        cursor.execute('INSERT INTO test_data(no,id) VALUES(?,?)',
                       (str(nomor), str(num[i])))
        conn.commit()
        lock.release()

        # looping by receiver range (INSERT LQ)
        for k in range(5):  # 5rcv
            if len(lqlist[k][i]) > 1:
                lock.acquire(True)
                lqi_x = str(round(statistics.mean(lqlist[k][i]), 2))  # lqi
                cursor.execute("UPDATE test_data SET lq{} = ? WHERE no={} AND id={}".format(
                    str(k+1), str(nomor), str(num[i])), (str(lqi_x),))

                v = str(round(variance(lqlist[k][i])))  # variance
                cursor.execute("UPDATE test_data SET variance{} = ? WHERE no={} AND id={}".format(
                    str(k+1), str(nomor), str(num[i])), (str(v),))
                conn.commit()
                lock.release()

            else:
                lock.acquire(True)
                lqi_x = str(LQI_MIN_VALUE)  # minimum value of lqi
                cursor.execute("UPDATE test_data SET lq{} = ? WHERE id={}".format(
                    str(k+1), str(num[i])), (str(lqi_x),))

                v = str(LQI_MIN_VALUE)
                cursor.execute("UPDATE test_data SET variance{} = ? WHERE id={}".format(
                    str(k+1), str(num[i])), (str(v),))
                conn.commit()
                lock.release()

        # INSERT ACCE
        lock.acquire(True)
        cursor.execute("UPDATE test_data SET acce = ? WHERE no={} AND id={}".format(
            str(nomor), str(num[i])), (str(ac_list[i]),))
        conn.commit()
        lock.release()


# 5rcv
def testData(data_RX1, data_RX2, data_RX3, data_RX4, data_RX5):
    prData1 = preData(data_RX1)
    prData2 = preData(data_RX2)
    prData3 = preData(data_RX3)
    prData4 = preData(data_RX4)
    prData5 = preData(data_RX5)  # if receivers increse, increce
    # if receivers increse, increce
    number = numberID(prData1[0], prData2[0],
                      prData3[0], prData4[0], prData5[0])
    # if receivers increse, increce
    sepaData = separateData(prData1, prData2, prData3,
                            prData4, prData5, number)
    ac_variance = acvariance(sepaData[1], number)  # variasi accelero
    dataProcess(sepaData[0], ac_variance, number)
    return number


def fileSearch(time, date):
    content2 = []
    file_folder = path + "/" + date
    #print(file_folder)
    content = os.listdir(file_folder)

    r1 = "r1" + time
    for a in content:
        if r1 != a:
            pass
        else:
            content2.append(a)
            break

    r2 = "r2" + time
    for a in content:
        if r2 != a:
            pass
        else:
            content2.append(a)
            break

    r3 = "r3" + time
    for a in content:
        if r3 != a:
            pass
        else:
            content2.append(a)
            break

    r4 = "r4" + time
    for a in content:
        if r4 != a:
            pass
        else:
            content2.append(a)
            break

    r5 = "r5" + time  # 5rcv
    for a in content:
        if r5 != a:
            pass
        else:
            content2.append(a)
            break

    #print(content2)
    return content2


def DataPath(file1, file2, file3, file4, file5, date):
    global DATA_TEST_RX1,  DATA_TEST_RX2, DATA_TEST_RX3, DATA_TEST_RX4, DATA_TEST_RX5
    DATA_TEST_RX1 = Path(date + "/" + file1)
    DATA_TEST_RX2 = Path(date + "/" + file2)
    DATA_TEST_RX3 = Path(date + "/" + file3)
    DATA_TEST_RX4 = Path(date + "/" + file4)
    DATA_TEST_RX5 = Path(date + "/" + file5)  # 5rcv
    print("last received file " + file5)
    #print(date + "/" + file4)


def preTestData():
    num = testData(DATA_TEST_RX1, DATA_TEST_RX2,
                   DATA_TEST_RX3, DATA_TEST_RX4, DATA_TEST_RX5)
    return num


def testProcess(num):  # detect correct rooms by comparing lq values
    global test_data
    global fingerprint_data
    global lqDroppedIdentifier
    room = []
    result = []
    DetecteRoom = []
    euc_dist = []
    moving_id = []
    room_num = []
    # moving_v_id = []
    # variance_list = []
    # v_id = []

    # test data before, and recent
    test_data_before = pd.read_sql_query(
        "SELECT * FROM test_data WHERE no={}".format(str(int(nomor)-1)), conn)
    #print("previous data: \n", test_data_before)

    test_data = pd.read_sql_query(
        "SELECT * FROM test_data WHERE no={}".format(nomor), conn)
    #print("current data: \n", test_data)

    # jika inputan dari user adalah "Y"

    #print("[Value Checker Enabled]")

    for i in range(len(num)):
            euc_dist.append([])

        # looping by index of test_data variable / depends on number of transmitter
    for i in range(len(test_data.index)):
        variance1 = test_data.loc[i, 'variance1']
        variance2 = test_data.loc[i, 'variance2']
        variance3 = test_data.loc[i, 'variance3']
        variance4 = test_data.loc[i, 'variance4']
        variance5 = test_data.loc[i, 'variance5']
        accelero_data = test_data.loc[i, 'acce']
        id = test_data.loc[i, 'id']

            # seleksi data berdasarkan value > 70
        if test_data.loc[i, 'lq1'] > LQI_threshold:
            fingerprint_data = pd.read_sql_query(
                "SELECT * FROM fingerprint WHERE lq1 > 90", conn)
        elif test_data.loc[i, 'lq2'] > LQI_threshold:
            fingerprint_data = pd.read_sql_query(
                "SELECT * FROM fingerprint WHERE lq2 > 90", conn)
        elif test_data.loc[i, 'lq3'] > LQI_threshold:
            fingerprint_data = pd.read_sql_query(
                "SELECT * FROM fingerprint WHERE lq3 > 90", conn)
        elif test_data.loc[i, 'lq4'] > LQI_threshold:
            fingerprint_data = pd.read_sql_query(
                "SELECT * FROM fingerprint WHERE lq4 > 90", conn)
        elif test_data.loc[i, 'lq5'] > LQI_threshold:
            fingerprint_data = pd.read_sql_query(
                "SELECT * FROM fingerprint WHERE lq5 > 90", conn)
        else:
            print("transmitter in different room")
            fingerprint_data = pd.read_sql_query(
                "SELECT * FROM fingerprint", conn)

            # check lq variance value
        if variance1 > 80 or variance2 > 80 or variance3 > 80 or variance4 > 80 or variance5 > 80:
                # lq drop checker
            if lqDroppedIdentifier == 0:
                if variance1 > 80 and test_data.loc[i, 'lq1'] == 5:
                    lqDroppedIdentifier = 1
                if variance2 > 80 and test_data.loc[i, 'lq2'] == 5:
                    lqDroppedIdentifier = 2
                if variance3 > 80 and test_data.loc[i, 'lq3'] == 5:
                    lqDroppedIdentifier = 3
                if variance4 > 80 and test_data.loc[i, 'lq4'] == 5:
                    lqDroppedIdentifier = 4
                if variance5 > 80 and test_data.loc[i, 'lq5'] == 5:
                    lqDroppedIdentifier = 5

            try:
                print( "ini id: ", id)
                print("[LQ VAR > 80]")
                    # check accelero data
                if accelero_data < 150:
                    if lqDroppedIdentifier != 0:
                            # if value dropped to 5, get room data before
                        if test_data.loc[i, 'lq{}'.format(lqDroppedIdentifier)] == 5:
                            DetecteRoom.append(test_data_before.loc[i, 'room'])
                            print("[ACCELERO < 150] : Catch Room Data Before \n", test_data_before.loc[i,'room'])
                                # mengappend dummy data berupa angka "1" ke list 'result'
                                # ini untuk menghindari index error ketika multiple transmitter
                            result.append(1)                                 
                        else:
                            lqDroppedIdentifier = 0
                                # run euclidean distance
                            print("[ACCELERO > 150] : Process recent data with euclidean distance")
                            for j in range(len(fingerprint_data.index)):
                                lqi1 = test_data.loc[i, 'lq1'] - \
                                    fingerprint_data.loc[j, 'lq1']
                                lqi2 = test_data.loc[i, 'lq2'] - \
                                    fingerprint_data.loc[j, 'lq2']
                                lqi3 = test_data.loc[i, 'lq3'] - \
                                    fingerprint_data.loc[j, 'lq3']
                                lqi4 = test_data.loc[i, 'lq4'] - \
                                    fingerprint_data.loc[j, 'lq4']
                                    # 5rcv
                                lqi5 = test_data.loc[i, 'lq5'] - \
                                    fingerprint_data.loc[j, 'lq5']

                                    # 5rcv
                                dist = (pow(lqi1, 2)+pow(lqi2, 2) +
                                        pow(lqi3, 2)+pow(lqi4, 2)+pow(lqi5, 2))
                                x_euc_dist = round(math.sqrt(dist), 2)
                                euc_dist[i].append(x_euc_dist)
                                room.append(fingerprint_data.loc[j, 'room'])

                            result.append(euc_dist[i].index(min(euc_dist[i])))
                                # ini append rum terdeteksi
                            DetecteRoom.append(room[result[i]])
                            room_num.append(
                                fingerprint_data.loc[result[i], 'number'])
                            room.clear()  # clear room list after detection
                    else:
                        DetecteRoom.append(test_data_before.loc[i, 'room'])
                        print("[ACCELERO < 150] : Catch Room Data Before \n", test_data_before.loc[i,'room'])
                            # mengappend dummy data berupa angka "1" ke list 'result'
                            # ini untuk menghindari index error ketika multiple transmitter
                        result.append(1)                   

                if accelero_data >= 150:
                    # run euclidean distance
                    print("[ACCELERO > 150] : Process recent data with euclidean distance")
                    for j in range(len(fingerprint_data.index)):
                        lqi1 = test_data.loc[i, 'lq1'] - \
                            fingerprint_data.loc[j, 'lq1']
                        lqi2 = test_data.loc[i, 'lq2'] - \
                            fingerprint_data.loc[j, 'lq2']
                        lqi3 = test_data.loc[i, 'lq3'] - \
                            fingerprint_data.loc[j, 'lq3']
                        lqi4 = test_data.loc[i, 'lq4'] - \
                            fingerprint_data.loc[j, 'lq4']
                        # 5rcv
                        lqi5 = test_data.loc[i, 'lq5'] - \
                            fingerprint_data.loc[j, 'lq5']

                            # 5rcv
                        dist = (pow(lqi1, 2)+pow(lqi2, 2) +
                                pow(lqi3, 2)+pow(lqi4, 2)+pow(lqi5, 2))
                        x_euc_dist = round(math.sqrt(dist), 2)
                        euc_dist[i].append(x_euc_dist)
                        room.append(fingerprint_data.loc[j, 'room'])

                    result.append(euc_dist[i].index(min(euc_dist[i])))
                        # ini append rum terdeteksi
                    DetecteRoom.append(room[result[i]])
                    room_num.append(
                        fingerprint_data.loc[result[i], 'number'])
                    room.clear()  # clear room list after detection

            except KeyError:
                # run euclidean distance
                print("KeyError in ValueChecker")
                for j in range(len(fingerprint_data.index)):
                    lqi1 = test_data.loc[i, 'lq1'] - \
                        fingerprint_data.loc[j, 'lq1']
                    lqi2 = test_data.loc[i, 'lq2'] - \
                        fingerprint_data.loc[j, 'lq2']
                    lqi3 = test_data.loc[i, 'lq3'] - \
                        fingerprint_data.loc[j, 'lq3']
                    lqi4 = test_data.loc[i, 'lq4'] - \
                        fingerprint_data.loc[j, 'lq4']
                        # 5rcv
                    lqi5 = test_data.loc[i, 'lq5'] - \
                        fingerprint_data.loc[j, 'lq5']

                        # 5rcv
                    dist = (pow(lqi1, 2)+pow(lqi2, 2) +
                            pow(lqi3, 2)+pow(lqi4, 2)+pow(lqi5, 2))
                    x_euc_dist = round(math.sqrt(dist), 2)
                    euc_dist[i].append(x_euc_dist)
                    room.append(fingerprint_data.loc[j, 'room'])

                result.append(euc_dist[i].index(min(euc_dist[i])))
                DetecteRoom.append(room[result[i]])
                room_num.append(
                    fingerprint_data.loc[result[i]: 'number'])
                room.clear()  # clear room list after detection

        else:
                # run euclidean distance
            for j in range(len(fingerprint_data.index)):
                lqi1 = test_data.loc[i, 'lq1'] - \
                    fingerprint_data.loc[j, 'lq1']
                lqi2 = test_data.loc[i, 'lq2'] - \
                    fingerprint_data.loc[j, 'lq2']
                lqi3 = test_data.loc[i, 'lq3'] - \
                    fingerprint_data.loc[j, 'lq3']
                lqi4 = test_data.loc[i, 'lq4'] - \
                    fingerprint_data.loc[j, 'lq4']
                # 5rcv
                lqi5 = test_data.loc[i, 'lq5'] - \
                    fingerprint_data.loc[j, 'lq5']

                # 5rcv
                dist = (pow(lqi1, 2)+pow(lqi2, 2) +
                        pow(lqi3, 2)+pow(lqi4, 2)+pow(lqi5, 2))
                x_euc_dist = round(math.sqrt(dist), 2)
                euc_dist[i].append(x_euc_dist)
                room.append(fingerprint_data.loc[j, 'room'])

            result.append(euc_dist[i].index(min(euc_dist[i])))
            DetecteRoom.append(room[result[i]])
            room_num.append(
                fingerprint_data.loc[result[i]: 'number'])
            room.clear()  # clear room list after detection
            

    ## here for logging ##
    log_time = logTime()
    date = now_date()
    limiter = ","

    # select test data berdasarkan nomor
    testData_df = pd.read_sql_query(
        'SELECT * FROM test_data WHERE no={}'.format(nomor), conn)

    # 5rcv
    for data in range(len(testData_df["id"])):
        ID = testData_df.loc[data, 'id']
        lq1 = testData_df.loc[data, 'lq1']
        lq2 = testData_df.loc[data, 'lq2']
        lq3 = testData_df.loc[data, 'lq3']
        lq4 = testData_df.loc[data, 'lq4']
        lq5 = testData_df.loc[data, 'lq5']

        # append room data at test_data
        lock.acquire(True)
        cursor.execute("UPDATE test_data SET room = ? WHERE no={} AND id={}".format(
            str(nomor), str(ID)), (str(DetecteRoom[data]),))
        conn.commit()
        lock.release()

        with open(LOG_FILE, "a") as logfile:
            logfile.write("\n")
            logfile.write(date)
            logfile.write(limiter)
            logfile.write(log_time)
            logfile.write(limiter)
            logfile.write(str(ID))
            logfile.write(limiter)
            logfile.write(str(lq1))
            logfile.write(limiter)
            logfile.write(str(lq2))
            logfile.write(limiter)
            logfile.write(str(lq3))
            logfile.write(limiter)
            logfile.write(str(lq4))
            logfile.write(limiter)
            logfile.write(str(lq5))
            logfile.write(limiter)
            logfile.write(DetecteRoom[data])
            #print("inidetecteroom", DetecteRoom)

    return DetecteRoom


def fileMove(content, date):
    for a in content:
        file = path + "\\" + date + "\\" + a
        move_folder = path + "\\" + date + "-detected"
        if(os.path.exists(move_folder)):
            pass
        else:
            os.mkdir(move_folder)
        shutil.move(file, move_folder)


def separateData(Data1, Data2, Data3, Data4, Data5, num):  # if receivers increse, increce
    lq_list = [[], [], [], [], []]  # if receivers increse, increce
    ac_list = [[], [], [], [], []]  # if receivers increse, increce

    try:
        for i in range(len(num)):
            lq_list[0].append([])
            ac_list[0].append([])
        for i in range(len(Data1[0])):
            for n in range(len(num)):
                if(num[n] == Data1[0][i]):
                    lq_list[0][n].append(Data1[1][i])
                    ac_list[0][n].append(Data1[2][i])

        for i in range(len(num)):
            lq_list[1].append([])
            ac_list[1].append([])
        for i in range(len(Data2[0])):
            for n in range(len(num)):
                if(num[n] == Data2[0][i]):
                    lq_list[1][n].append(Data2[1][i])
                    ac_list[1][n].append(Data2[2][i])

        for i in range(len(num)):
            lq_list[2].append([])
            ac_list[2].append([])
        for i in range(len(Data3[0])):
            for n in range(len(num)):
                if(num[n] == Data3[0][i]):
                    lq_list[2][n].append(Data3[1][i])
                    ac_list[2][n].append(Data3[2][i])

        for i in range(len(num)):
            lq_list[3].append([])
            ac_list[3].append([])
        for i in range(len(Data4[0])):
            for n in range(len(num)):
                if(num[n] == Data4[0][i]):
                    lq_list[3][n].append(Data4[1][i])
                    # if receivers increse, increce
                    ac_list[3][n].append(Data4[2][i])

        # 5rcv
        for i in range(len(num)):
            lq_list[4].append([])
            ac_list[4].append([])
        for i in range(len(Data5[0])):
            for n in range(len(num)):
                if(num[n] == Data5[0][i]):
                    lq_list[4][n].append(Data5[1][i])
                    ac_list[4][n].append(Data5[2][i])
        # print(lq_list)
        return lq_list, ac_list

    except IndexError:
        print("indexing error occur while inserting to ac_list")
        time.sleep(5)
        job()


# 5rcv
def acvariance(ac_list, num):
    acvariance_list = [[], [], [], [], []]  # if receivers increse, increce
    moving_list = []
    # 5rcv
    for k in range(5):
        for i in range(len(num)):
            if(len(ac_list[k][i]) > 1):
                acvariance_list[k].append(round(variance(ac_list[k][i])))
            else:
                acvariance_list[k].append(0)

    for i in range(len(num)):
        moving_list.append(max(
            acvariance_list[0][i], acvariance_list[1][i], acvariance_list[2][i], acvariance_list[3][i], acvariance_list[4][i]))

    return moving_list


# ROOM DETECTION FUNCTION
def online_detection():
    while True:
        time = now_time()
        date = now_date()

        # search file and process the data
        content = fileSearch(time, date)

        # 5rcv
        if len(content) != 5:
            print("no data lencontent")
            break
        # find csv file and convert to variable
        DataPath(content[0], content[1], content[2],
                 content[3], content[4], date)  # 5rcv

        number = preTestData()  # pretest

        # the main process
        # test_process dilakukan jika testdatanya memenuhi
        test_result = testProcess(number)

        # menampilkan lq data
        
        #print("======================================================")
        print(test_data[['id', 'lq1', 'lq2', 'lq3', 'lq4', 'lq5']])  # 5rcv

        # menampilkan ID dan Lokasi
        print("\n These are current position")
        for i in range(len(number)):
            print("id = ", test_data.iloc[i]['id'], "is at", test_result[i])
        
        print("======================================================")
        print("\n")
        return test_result, number


#------------------------------FUNCTIONS FOR MQTT-------------------------------------#
# set topic publish and subscribe
pubtop = "/Project_IPS/serv"
subtop = "/Project_IPS/#"  # use wildcard '#' so that the data can come simultaneously


client = mqtt.Client()
client.on_connect = on_connect
client.connect(broker_address, port)
# client.publish(pubtop, hours)


# MAIN PROCESS
def job():
    # set broker address and port
    client.on_subscribe = on_subscribe
    # start loop client dan subscribe berdasarkan topic
    client.loop_start()
    client.subscribe(subtop)
    userList = []



    # LOOPING INTI
    while True:
        try:
            # check data from telegram bot API
            end_time = time.time() + PERIOD
            message = requests.get(bot_url + 'getUpdates')
            messageJSON = json.loads(message.text)
            messageText = messageJSON['result'][-1]['message']['text']
            userID = messageJSON['result'][-1]['message']['from']["id"]
            #print("lolos try")

            end_time = time.time() + PERIOD

            # looping 5 detik (5rcv)
            while time.time() <= end_time:
                client.message_callback_add("/Project_IPS/client1", client1)
                client.message_callback_add("/Project_IPS/client2", client2)
                client.message_callback_add("/Project_IPS/client3", client3)
                client.message_callback_add("/Project_IPS/client4", client4)
                client.message_callback_add("/Project_IPS/client5", client5)

            exportData()

            # CLEAR TERMINAL
            # os.system('cls' if os.name == 'nt' else 'clear')

            # Try to get data, if no one data, then handling exception
            try:
                # get data
                data_ruangan = online_detection()
                ID_raw = list(map(int, data_ruangan[1]))
                test_data = pd.read_sql_query(
                    "SELECT * FROM test_data WHERE no={}".format(nomor), conn)
                #print("Transmitter Location: ", data_ruangan[0])

            except TypeError:
                print("no data")

            # CALLBACK TELEGRAM
            if messageText == "/start":
                """
                # check user ID
                if userID in userList:
                    print("user id masih sama")
                else:
                    userList.append(userID)
                    print(userID)
                """

                # kirim pesan ke user telegram
                messageData = []
                if len(ID_raw) > 0:
                    try:
                        # looping for append data from different transmitter_ID
                        for i in range(len(ID_raw)):
                            Message = "[" + str(ID_raw[i]) + "]  " + test_data['lq1'][i].astype(str) + "  " + test_data['lq2'][i].astype(str) + \
                                "  " + test_data['lq3'][i].astype(str) + "  " + test_data['lq4'][i].astype(
                                    str)+"  " + test_data['lq5'][i].astype(str) + "  " + data_ruangan[0][i]
                            messageData.append(Message)

                        headerMessage = "Here's the report from server: \n \n" + \
                                        "ID  lq1  lq2  lq3  lq4  lq5  room" + "\n"
                        bodyMessage = "\n".join(messageData)
                        cleanMessage = headerMessage + bodyMessage

                        for i in range(len(userList)):
                            sendMessage(userList[i], cleanMessage)
                            print(
                                "[BOT: online detection data sent! UserID:", userList[i], "]")

                    except TypeError:
                        traceback.print_exc()
                        print("no data")

                else:
                    print("no transmitter detected")

            elif messageText == "/stop":
                # check user ID
                if userID in userList:
                    userList.remove(userID)
                else:
                    print("user sudah dihapus dari userList")
                print("[BOT: stop sending message to user, waiting /start command]")

        except IndexError:
            #traceback.print_exc()
            # looping 5 detik
            while time.time() <= end_time:
                client.message_callback_add("/Project_IPS/client1", client1)
                client.message_callback_add("/Project_IPS/client2", client2)
                client.message_callback_add("/Project_IPS/client3", client3)
                client.message_callback_add("/Project_IPS/client4", client4)
                client.message_callback_add("/Project_IPS/client5", client5)

            exportData()
            # Try to get data, if no one data, then handling exception
            try:
                # get data
                data_ruangan = online_detection()
                ID_raw = list(map(int, data_ruangan[1]))
                test_data = pd.read_sql_query("SELECT * FROM test_data", conn)
                print("Transmitter Location: ", data_ruangan[0])

            except TypeError:
                traceback.print_exc()
                print("no data")

            #print("tidak ada data di Telegram Bot API")
            time.sleep(1)

    # jika programnya dihentikan
    client.disconnect()
    client.loop_stop()


# MAIN FUNCTION
if __name__ == "__main__":

    now = datetime.datetime.now()
    today = now.strftime("%Y-%m-%d")

    print("===========================")
    print("Welcome to FILS15.4 Project!")
    print("===========================\n")

    try:
        cursor.execute('DELETE FROM test_data')
        conn.commit()
        print("cleaning test_data table . . ")
        time.sleep(2)
    except:
        print("tidak ada data di tabel")

    # 5rcv
    cursor.execute('CREATE TABLE IF NOT EXISTS test_data(no INTEGER, id INTEGER, lq1 INTEGER, variance1 INTEGER, lq2 INTEGER, variance2 INTEGER, lq3 INTEGER, variance3 INTEGER, lq4 INTEGER, variance4 INTEGER, lq5 INTEGER, variance5 INTEGER, acce INTEGER, room STRING)')

    if os.path.isdir(today):
        while True:
            job()
    else:
        os.mkdir(today)
        while True:
            job()
