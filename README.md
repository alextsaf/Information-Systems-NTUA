# Information-Systems-NTUA
 
Project for "Information Systems" course @ NTUA (9th semester)

# Group Members (infosys07):

- Vasilis Vrettos el18126
- Nikos Pagonas el18175
- Alexandros Tsafos el18211

# Prediction of Apache Spark GraphX Operator Performance

The main objective of this project was to heavily benchmark and profile Scala applications that utilize GraphX operators and create a model that predicts the performance of these operators.

# Project Overview

## Architecture

In a 3 node cluster setup, Apache Spark was installed as well as Hadoop DFS for file sharing between the nodes. Each node runs in an isolated Virtual Machine. These VMs were hosted in Okeanos-Knossos service of GRNET.

## GraphX Operators

The operators that we were tasked with benchmarking were: PageRank, Connected Components and Triangle Counting. Each one of those were heavily profiled using both ready-made libraries and custom software.

## Input Graph Generation

Custom data generators were developed to test the performance of the operators depending on the type of network/graph that they were input. The networks that we tested were:
- Random Network
- Random Geometric Graph
- Small-World Network
- Scale Free Network

## Modeling of Performance

## Installation Instructions

[Apache Spark](spark-installation-instructions.md)

[Apache Hadoop](hadoop-installation-instructions.md)
