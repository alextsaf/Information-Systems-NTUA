import org.apache.spark.graphx.GraphLoader
import org.apache.spark.sql.SparkSession
import org.apache.spark._
import org.apache.spark.scheduler._
import org.apache.spark.graphx._
import ch.cern.sparkmeasure._
import org.apache.spark.sql.functions._

object Main {
  val HDFSBaseURL = "hdfs://192.168.1.1:9000"

  def pageRank(sc: SparkContext) : (Long, Long) = {
    // Load the edges as a graph
    val graph = GraphLoader.edgeListFile(sc, s"$HDFSBaseURL//data/graphs/input.txt")
    // Run PageRank
    val ranks = graph.pageRank(0.001).vertices

    val numEdges = graph.numEdges
    val numVertices = graph.numVertices

    return (numVertices, numEdges)
  }

  def connectedComponents(sc: SparkContext) : (Long, Long) = {
    // Load the edges as a graph
    val graph = GraphLoader.edgeListFile(sc, s"$HDFSBaseURL//data/graphs/input.txt")
    // Find the connected components
    val cc = graph.connectedComponents().vertices

    val numEdges = graph.numEdges
    val numVertices = graph.numVertices

    return (numVertices, numEdges)
  }

  def triangleCounting(sc: SparkContext) : (Long, Long) = {
    // Load the edges as a graph
    val graph = GraphLoader
      .edgeListFile(sc, s"$HDFSBaseURL//data/graphs/input.txt")
      .partitionBy(PartitionStrategy.RandomVertexCut)
    // Count the triangles... trigonakia :3
    val triCounts = graph.triangleCount().vertices

    val numEdges = graph.numEdges
    val numVertices = graph.numVertices

    return (numVertices, numEdges)
  }

  def main(args: Array[String]): Unit = {

    // Parse input args
    val algorithmToExecute = args(0)
    val inputFileName = args(1)
    val datasetSize = args(2).toString

    val outputFolder = s"$HDFSBaseURL//data/results/$algorithmToExecute/$algorithmToExecute-$datasetSize"

    // Create a spark session and setup the Stage Metrics
    val spark = SparkSession
      .builder
      .appName(s"$algorithmToExecute-$datasetSize")
      .config("spark.extraListeners", "ch.cern.sparkmeasure.FlightRecorderStageMetrics")
      .getOrCreate()
    val sc = spark.sparkContext
    val stageMetrics = StageMetrics(spark)
    var graphInfo: (Long,Long) = (0,0)
  
    // Actually run the GraphX algorithm that we are interested in
    stageMetrics.runAndMeasure {
      if (algorithmToExecute == "pageRank") {
        graphInfo = pageRank(sc)
      }
      else if (algorithmToExecute == "connectedComponents") {
        graphInfo = connectedComponents(sc)
      }
      else if (algorithmToExecute == "triangleCounting") {
        graphInfo = triangleCounting(sc)
      }
    }

    // Print report for us to see, not really needed - does not affect metrics
    stageMetrics.printReport()

    // Create the metrics dataframe and aggregate it
    val df = stageMetrics.createStageMetricsDF("PerfStageMetrics")
    val aggregatedDF = stageMetrics.aggregateStageMetrics("PerfStageMetrics")

    // Create a dataframe with the metrics that we are actually interested in
    var testDF = aggregatedDF.select("numStages", "elapsedTime", "executorRunTime", "executorCpuTime", "peakExecutionMemory")

    testDF = testDF.withColumn("numNodes", lit(graphInfo._1))
    testDF = testDF.withColumn("numEdges", lit(graphInfo._2))

    testDF.show()
    
    // Write this file to the HDFS in a results folder
    testDF.coalesce(1).write.option("header", true).mode("overwrite").csv(outputFolder)

    // The end
    spark.stop()
  }
}
