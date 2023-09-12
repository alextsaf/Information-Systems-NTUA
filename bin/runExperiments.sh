#! /bin/bash

# Folder path related variables
projectPath=""
jarLocation="$projectPath/target/scala-2.12/infosys_2.12-1.0.jar"
tmpFolder="$projectPath/tmp"
graphInputPath="$tmpFolder/inputGraphs/input.txt"

export INFOSYS_PROJECT_PATH="$projectPath"

# Spark-HDFS related variables
masterIP="spark://192.168.1.1:7077"

function generateRandomInt() {
    low=$1
    high=$2
    randomInt=$(python3.8 /home/user/bin/randomInt.py $low $high)
    echo $randomInt
}

function generateRandomFloat {
    low=$1
    high=$2
    randomFloat=$(python3.8 /home/user/bin/randomFloat.py $low $high)
    echo $randomFloat
}

function build_jar {
    # Build jar
    cd "$projectPath/infosys/"
    ~/sbt/bin/sbt package
    cd ~
}

function getFromHDFS {
    algorithm=$1
    dataset=$2

    hdfs dfs -get /data/results/$algorithm/$algorithm "$tmpFolder/outputPath"
}

function renameCSVOuts {
    algorithm=$1
    dataset=$2

    for file in $tmpFolder/outputPath/part-*.csv
    do
        dirname=$(dirname $file)
        mv $file $dirname/$algorithm-results.csv
    done
}

function submit_to_spark {
    # Pass arguments to function
    executorMemory=$1
    algorithm=$2

    $SPARK_HOME/bin/spark-submit --class "Main" --packages ch.cern.sparkmeasure:spark-measure_2.12:0.23 \
    --master $masterIP --deploy-mode "client" --executor-cores 4 --executor-memory $executorMemory --driver-memory "1g" \
    $jarLocation $algorithm $graphInputPath $datasetSize 
}

function executeAlgorithm {
    algorithm=$1

    python3.8 /home/user/bin/profiler.py 3 $tmpFolder/outputPath/$algorithm/$algorithm-memory.txt &
    submit_to_spark $executorMemory $algorithm
}

function cleanupOutputs {
    algorithm=$1
    graphInputFile=$2
    graphType=$3

    python3.8 /home/user/bin/dataCleanup.py $algorithm $datasetSize $graphInputFile $graphType
}

function pipeline {
    algorithm=$1
    graphType=$2

    # Run Spark, get output file, rename it and cleanup data
    executeAlgorithm $algorithm
    getFromHDFS $algorithm
    renameCSVOuts $algorithm

    echo "Going to sleep for a bit to make sure that benchmarking is over :)"
    sleep 3

    echo "Cleaning up outputs and adding data to dataset"
    cleanupOutputs $algorithm $graphInputPath $graphType

    echo "Cleaning up after temp result e.t.c."
    # Cleanup temp files
    rm -rf /home/user/datasets/tmp/outputPath/*
    rm -rf /home/user/datasets/tmp/inputGraph/*
}

function generateRandomErdos() {
    numOfNodes=$(generateRandomInt 13000 18000)
    percentageOfEdge=$(generateRandomFloat 0.3 0.45) 
    echo "Generating Erdos Network with Nodes: $numOfNodes and percentage of edge: $percentageOfEdge"

    "$projectPath/dataset_generation/erdos" $numOfNodes $percentageOfEdge $graphInputPath
    hdfs dfs -put -f $graphInputPath /data/graphs/
}

function generateRandomRGG() {
    numOfNodes=$(generateRandomInt 13000 18000)
    radius=$(generateRandomFloat 0.3 0.45)
    echo "Generating RGG Network with Nodes: $numOfNodes and mean radius: $radius"

    "$projectName/dataset_generation/rgg" $numOfNodes $radius $graphInputPath
    hdfs dfs -put -f $graphInputPath /data/graphs/
}

function generateRandomSmallWord() {
    numOfNodes=$(generateRandomInt 12000 22000)
    numberOfEdgesPerNode=$(generateRandomInt 3500 5500)
    percOfReconnection=$(generateRandomFloat 0.5 0.85)

    "$projectName/dataset_generation/sw" $numOfNodes $numberOfEdgesPerNode $percOfReconnection $graphInputPath
    hdfs dfs -put -f $graphInputPath /data/graphs/
}

function generateRandomScaleFree() {
    numOfNodes=$(generateRandomInt 12000 22000)
    numberOfEdgesPerNode=$(generateRandomInt 3500 5500)
    
    "$projectName/dataset_generation/sf" $numOfNodes $numberOfEdgesPerNode  $graphInputPath
    hdfs dfs -put -f $graphInputPath /data/graphs/
}

function runRandomErdos() {
    generateRandomErdos
    echo "Generated Data is: $(du -h "$projectPath/tmp/inputGraph/input.txt") in size"

    pipeline "connectedComponents" "erdos"
    pipeline "pageRank" "erdos"
    pipeline "triangleCounting" "erdos" 

    rm -rf /home/user/datasets/inputGraphs/*
}

function runRandomRGG() {
    generateRandomRGG
    echo "Generated Data is: $(du -h "$projectPath/tmp/inputGraph/input.txt") in size"

    pipeline "connectedComponents" "rgg"
    pipeline "pageRank" "rgg"
    pipeline "triangleCounting" "rgg" 

    rm -rf /home/user/datasets/inputGraphs/*
}

function runRandomSmallWorld() {
    generateRandomSmallWord
    echo "Generated Data is: $(du -h "$projectPath/tmp/inputGraph/input.txt") in size"

    pipeline "connectedComponents" "sw"
    pipeline "pageRank" "sw"
    pipeline "triangleCounting" "sw" 

    rm -rf /home/user/datasets/inputGraphs/*
}

function runRandomScaleFree() {
    generateRandomScaleFree
    echo "Generated Data is: $(du -h "$projectPath/tmp/inputGraph/input.txt") in size"

    pipeline "connectedComponents" "sf"
    pipeline "pageRank" "sf"
    pipeline "triangleCounting" "sf" 

    rm -rf /home/user/datasets/inputGraphs/*
}

if [ -z "$projectPath" ]
then
    echo "Project Path variable is not set. Set it according to your preferences before running this script."
    exit
else
    echo "not empty"
fi