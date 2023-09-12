import pandas as pd
import glob
import os
import sys

def main():
    # Parse Arguements 
    if (len(sys.argv) != 4):
        print("Usage: python3.8 dataCleanup.py <algorithm(str)> <inputGraphPath(str)> <graphType(str)>")
        exit(2)
        
    algorithm = str(sys.argv[1])
    inputGraphPath = str(sys.argv[2])
    graphType = str(sys.argv[3])

    projectFolder = os.getenv('INFOSYS_PROJECT_PATH')

    outputFilePath = f"{projectFolder}/tmp/{algorithm}/{algorithm}-results.csv"
    memoryUsedFilePath = f"{projectFolder}/tmp/{algorithm}/{algorithm}-memory.txt"

    memoryFile = open(memoryUsedFilePath, 'r')
    maxMemoryUsed = int(float(memoryFile.read()))
    memoryFile.close()

    # Read latest result csv
    row = pd.read_csv(outputFilePath)

    # Get actual dataset size
    datasetSize = os.path.getsize(inputGraphPath)/(1024*1024) # in MBs
    row['datasetSize'] = datasetSize
    row['graphType'] = graphType
    row['peakExecutionMemory'] = maxMemoryUsed

    
    modelDataFilePath = f"{projectFolder}/results/{algorithm}Data.csv"

    # Add the row to the csv file
    row.to_csv(modelDataFilePath, mode='a', header=False, index=False)

if __name__ == "__main__":
    main()