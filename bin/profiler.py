from sparkmeasure import StageMetrics
import requests
import json
import psutil
from pyspark.sql import SparkSession
import threading
import sys
import time
from pathlib import Path

sparkAPI="http://83.212.81.77:4040"

class Profiler:
    def __init__(self, interval, maxMemoryFilename):
        self.createSparkSession()
        self.getApplicationID()
        self.interval = interval
        self.totalMemoryArray = []
        self.resultFileName = maxMemoryFilename

    def createSparkSession(self):
        self.sparkSession = SparkSession.getActiveSession()

    def getApplicationID(self):
        retryCount = 0
        while (retryCount < 10):
            try:
                response = requests.get(f"{sparkAPI}/api/v1/applications")
            except requests.exceptions.ConnectionError:
                print("API is not available, wait for Spark to start")
                time.sleep(3)
                retryCount += 1
                if (retryCount == 10):
                    exit(1)
            else:
                break
        jsonResponse = json.loads(response.text)
        self.applicationID = jsonResponse[0]["id"]

    def getExecutorStats(self):
        try:
            response = requests.get(f"{sparkAPI}/api/v1/applications/{self.applicationID}/executors")
        except requests.exceptions.ConnectionError:
            print("API is not available, benchmarking must have ended")
            self.handleShutdown()
        executorStats = json.loads(response.text)
        return executorStats

    def getMemoryStats(self):
        currentStats = self.getExecutorStats()
        executorID = 0
        totalMemory = 0
        for metric in currentStats:
            memoryUsed = metric["memoryUsed"]/(1024*1024)
            totalMemory += memoryUsed
            self.totalMemoryArray.append(totalMemory)
            print(f"CurrentExecutor: {executorID} --- Memory Used(in MB): {memoryUsed} --- Total Memory: {totalMemory}")
            executorID += 1
        
        # Restart Thread
        self.timer = threading.Timer(self.interval, self.getMemoryStats)
        self.timer.start()

    def benchmark(self):
        self.timer = threading.Timer(self.interval, self.getMemoryStats)
        self.timer.start()

    def handleShutdown(self):
        # Get max memory usage
        maxMemoryUsed = max(self.totalMemoryArray)
        print(f"Max Memory Used: {maxMemoryUsed}")

        Path(self.resultFileName).parents[0].mkdir(parents=True, exist_ok=True)
        resultFile = open(self.resultFileName, "w")
        resultFile.write(str(maxMemoryUsed))
        resultFile.close()
        exit(2)

def main():
    if (len(sys.argv) != 3):
        print("Usage: python3.8 profiler.py <interval(float)> <outputName(str)>")
        exit(3)

    interval = float(sys.argv[1]) # 1st arg (after profiler.py)
    maxMemoryFilename = str(sys.argv[2]) # 2nd arg

    time.sleep(7)
    # Profile Memory with a 10 second interval wait period
    profiler = Profiler(interval, maxMemoryFilename)
    # Start benchmarking, this will stop automatically once it realises that spark session is finished
    profiler.benchmark()

if __name__ == "__main__":
    main()