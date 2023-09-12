# Graph Generation

Generators for each of the graph types described in the paper.

- `erdos.c` (Erdős–Rényi Random Graph): Here, each pair of nodes gets connected with a constant probability $p$. As such, the density of the graph (i.e. the number of edges) increases as $p$ increases. As the number of nodes $n$ increases, the emergence of a *giant component* becomes more likely. A giant component is a component of the graph that contains an overwhelming fraction of the nodes.
- `rgg.c` (Two-Dimensional Random Geometric Graph - RGG): RGG's are constructed by considering a 2D plane and then randomly generating the $x$ and $y$ coordinates of each node. Then, the newly created node is connected with all other nodes that are in distance $R$ or less. $R$ is a constant parameter that is defined before generating the graph.
- `sf.c` (Scale Free Network - SF): Using the Barabási–Albert *preferential attachment* mechanism, each new node gets connected to a constant number of existing nodes, with a preference for nodes that already have a lot of connections. The result is a power-law degree distribution.
- `sw.c` (Small World Network - SW): Using the Watts-Strogatz model, nodes are arranged in a circle and each node is connected to a small number of its closest neighbors. After that, each edge gets rewired to a random node with a small probability, resulting in a number of distant connections that reduce the overall average path length for the whole network.
