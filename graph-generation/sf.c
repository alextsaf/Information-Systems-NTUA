#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))

struct ScaleFreeNetwork {
    int m0;
    int m;
    int num_nodes;
    FILE* output_file;
    int* degree_list;
};

void add_edge(int src, int dst, FILE* fp) {
    fprintf(fp, "%d %d\n", src, dst);
}

int choose_node(int node, unsigned long long* cumulative_sum) {
    unsigned long long rand_val = (unsigned long long)rand() % cumulative_sum[node - 1];
    int left = 0;
    int right = node - 1;
    int target_node = 0;
    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (cumulative_sum[mid] <= rand_val) {
            target_node = mid + 1;
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    return target_node;
}

void add_node(int node, struct ScaleFreeNetwork* network) {
    // Select m nodes to connect to
    int m = MIN(network->m, node);
    int* target_nodes = (int*)malloc(m * sizeof(int));

    // Calculate the cumulative sum of degrees for preferential attachment
    unsigned long long* cumulative_sum = (unsigned long long*)malloc(node * sizeof(unsigned long long));
    cumulative_sum[0] = network->degree_list[0];
    for (int i = 1; i < node - m; i++) {
        cumulative_sum[i] = cumulative_sum[i - 1] + network->degree_list[i];
    }

    // Calculate the cumulative sum for the remaining nodes (node-m to node-1)
    unsigned long long remaining_sum = 0;
    for (int i = node - m; i < node; i++) {
        remaining_sum += network->degree_list[i];
    }

    // Fill the cumulative sum array for the remaining nodes
    for (int i = node - m; i < node; i++) {
        cumulative_sum[i] = cumulative_sum[i - 1] + remaining_sum;
    }

    // Select m nodes based on preferential attachment
    for (int i = 0; i < m; i++) {
        target_nodes[i] = choose_node(node, cumulative_sum);
    }

    // Connect the new node to the selected target nodes
    for (int i = 0; i < m; i++) {
        add_edge(node, target_nodes[i], network->output_file);
        network->degree_list[node]++;
        network->degree_list[target_nodes[i]]++;
    }

    free(target_nodes);
    free(cumulative_sum);
}

void generate_network(struct ScaleFreeNetwork* network) {
    // Create m0 initial nodes with random connections
    for (int i = 0; i < network->m0; i++) {
        add_node(i, network);
    }

    // Add the remaining nodes one by one
    for (int i = network->m0; i < network->num_nodes; i++) {
        add_node(i, network);
    }
}

int main(int argc, char* argv[]) {
    // Set the parameters for the scale-free network
    int m0 = 5;  // Initial number of nodes
    int num_nodes;
    FILE* output_file;
    

    if (argc != 4) {
        printf("Usage: %s num_nodes m-value output_file\n", argv[0]);
        
        return 1;
    } 

    num_nodes = atoi(argv[1]);
    int m = atoi(argv[2]);  // Number of connections for each new node
    const char* output_file_name = argv[3];

    srand(time(NULL)); // Seed the random number generator

    // Create the scale-free network
    output_file = fopen(output_file_name, "w");
    if (output_file != NULL) {
        int* degree_list = (int*)calloc(num_nodes, sizeof(int));
        struct ScaleFreeNetwork sf_network = {m0, m, num_nodes, output_file, degree_list};
        generate_network(&sf_network);
        free(degree_list);

        fclose(output_file);

        printf("Scale-free network edges have been written to %s\n", output_file_name);
    } else {
        printf("Error opening the output file.\n");
        return 1;
    }

    return 0;
}
