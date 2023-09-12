name := "infosys"
version := "1.0"
scalaVersion := "2.12.5"
val sparkVersion = "3.1.3"

// All Dependencies (so far)
libraryDependencies += "org.apache.spark"    %% "spark-sql"        % sparkVersion % "provided"
libraryDependencies += "org.apache.spark"    %% "spark-core"        % sparkVersion % "provided"
libraryDependencies += "org.apache.spark"    %% "spark-mllib"      % sparkVersion % "provided"
libraryDependencies += "org.apache.spark"    %% "spark-graphx"      % sparkVersion % "provided"
libraryDependencies += "ch.cern.sparkmeasure" %% "spark-measure" % "0.23"
libraryDependencies += "io.dropwizard.metrics" % "metrics-core" % "4.1.17"
