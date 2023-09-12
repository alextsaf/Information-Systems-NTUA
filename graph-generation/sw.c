#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

typedef struct {
    int src;
    int dst;
} Edge;

void read_neighbors(FILE* file, int node, Edge* lastline, int* numEdges, Edge* edges) {
    if (lastline->src == -1 && lastline->dst == -1) {
        // Skip the first empty edge
    }
    else {
        edges[*numEdges] = *lastline;
        (*numEdges)++;
    }

    lastline->src = -1;
    lastline->dst = -1;

    char line[100];
    while (fgets(line, sizeof(line), file) != NULL) {
        int src, dst;
        sscanf(line, "%d %d", &src, &dst);

        if (src == node) {
            edges[*numEdges].src = src;
            edges[*numEdges].dst = dst;
            (*numEdges)++;
        }
        else if (src > node) {
            lastline->src = src;
            lastline->dst = dst;
            break;
        }
    }
}

void create_small_world_graph(int num_nodes, int k, float p, const char* firstfile, const char* secondfile) {
    FILE* first = fopen(firstfile, "w");
    if (first == NULL) {
        perror("Error opening first file");
        return;
    }


    // Create a ring lattice graph and write it to the file
    for (int i = 0; i < num_nodes; i++) {
        for (int j = 1; j <= k / 2; j++) {
            int dst1 = (i + j) % num_nodes;
            int dst2 = (i - j + num_nodes) % num_nodes;
            fprintf(first, "%d %d\n", i, dst1);
            fprintf(first, "%d %d\n", i, dst2);
        }
    }
    fclose(first);

	printf("Opened!\n");

    FILE* second = fopen(secondfile, "w");
    if (second == NULL) {
        perror("Error opening second file");
        return;
    }

	first = fopen(firstfile, "r");
    if (first == NULL) {
        perror("Error opening second file");
        return;
    }

    // Read the first file and rewire edges with probability p
    int cnt = -1;
    Edge lastline = {-1, -1};
    Edge* edges = malloc(2 * num_nodes * sizeof(Edge)); // Allocate space for potential maximum number of edges
    if (edges == NULL) {
        perror("Error allocating memory");
        return;
    }

    // Read the file, and with probability p, rewire the edges
    for (int node = 0; node < num_nodes; node++) {
        cnt++;
        int numEdges = 0;
        read_neighbors(first, node, &lastline, &numEdges, edges);

        Edge* oldneigh = malloc(numEdges * sizeof(Edge));
        if (oldneigh == NULL) {
            perror("Error allocating memory");
            return;
        }

        for (int i = 0; i < numEdges; i++) {
            oldneigh[i] = edges[i];
        }

        // Rewire the edges with probability p
        for (int i = 0; i < numEdges; i++) {
            if (rand() / (float)RAND_MAX < p) {
                int new_dst;
                do {
                    new_dst = rand() % num_nodes;
                } while (new_dst == oldneigh[i].src);

                bool found = false;
                for (int j = 0; j < numEdges; j++) {
                    if (edges[j].src == oldneigh[i].src && edges[j].dst == new_dst) {
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    edges[i].dst = new_dst; // Rewire the edge
                }
            }
        }

        // Write the new edges to the file
        for (int i = 0; i < numEdges; i++) {
            fprintf(second, "%d %d\n", edges[i].src, edges[i].dst);
        }

        free(oldneigh);
    }

    free(edges);
	fclose(first);
    fclose(second);
}

int main(int argc, char* argv[]) {
    if (argc != 5) {
        printf("Usage: %s <num_nodes> <k> <p> <output_file>\n", argv[0]);
        return 1;
    }

    int num_nodes = atoi(argv[1]);
    int k = atoi(argv[2]); // Each node is connected to its 4 nearest neighbors (k/2 on each side)
    float p = atof(argv[3]); // Probability of rewiring an edge

    const char* firstfile = "tempfile.txt";
    const char* secondfile = argv[4];

    srand(time(NULL)); // Seed the random number generator

    create_small_world_graph(num_nodes, k, p, firstfile, secondfile);

    return 0;
}
