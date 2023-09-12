#!/bin/bash
masterIP="192.168.1.1"

masterWorkerPort="65509"
secondWorkerPort="65510"
thirdWorkerPort="65511"
webUIPort="8080"
masterSparkPort="7077"

cores="4"
memory="8g"

masterWorkerCommand="$SPARK_HOME/sbin/spark-daemon.sh start org.apache.spark.deploy.worker.Worker 1 \
--webui-port $webUIPort --port $masterWorkerPort --cores $cores --memory $memory spark://$masterIP:$masterSparkPort"

$masterWorkerCommand

secondWorkerCommand="$SPARK_HOME/sbin/spark-daemon.sh start org.apache.spark.deploy.worker.Worker 2 \
--webui-port $webUIPort --port $secondWorkerPort --cores $cores --memory $memory spark://$masterIP:$masterSparkPort"

ssh user@snf-37291 $secondWorkerCommand

thirdWorkerCommand="$SPARK_HOME/sbin/spark-daemon.sh start org.apache.spark.deploy.worker.Worker 3 \
--webui-port $webUIPort --port $thirdWorkerPort --cores $cores --memory $memory spark://$masterIP:$masterSparkPort"

ssh user@snf-37293 $thirdWorkerCommand
