#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))

struct ScaleFreeNetwork {
    int m0;
    int m;
    int num_nodes;
    int* degree_list;
};

void add_edge_to_buffer(FILE* output_file, char* buffer, int* buffer_index, int buffer_size, int src, int dst) {
    int n = snprintf(buffer + *buffer_index, buffer_size - *buffer_index, "%d %d\n", src, dst);
    *buffer_index += n;
    // If the buffer is full, write its content to the file and reset the buffer_index to 0
    if (*buffer_index >= buffer_size) {
        fwrite(buffer, 1, buffer_size, output_file);
        *buffer_index = 0;
    }
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

void add_node(FILE* output_file, int node, struct ScaleFreeNetwork* network, char* buffer, int* buffer_index, int buffer_size) {
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
        add_edge_to_buffer(output_file, buffer, buffer_index, buffer_size, node, target_nodes[i]);
        network->degree_list[node]++;
        network->degree_list[target_nodes[i]]++;
    }

    free(target_nodes);
    free(cumulative_sum);
}

void generate_network(FILE* output_file, struct ScaleFreeNetwork* network, char* buffer, int buffer_size) {
    int buffer_index = 0;

	// Create m0 initial nodes with random connections
    for (int i = 0; i < network->m0; i++) {
        add_node(output_file, i, network, buffer, &buffer_index, buffer_size);
    }

    // Add the remaining nodes one by one
     for (int i = network->m0; i < network->num_nodes; i++) {
        add_node(output_file, i, network, buffer, &buffer_index, buffer_size);
    }

	// Write any remaining edges in the buffer to the file
    if (buffer_index > 0) {
        fwrite(buffer, 1, buffer_index, output_file);
    }
}

int main(int argc, char* argv[]) {
    // Set the parameters for the scale-free network
    int m0 = 5;  // Initial number of nodes
    int num_nodes;
    FILE* output_file;
    char* output_file_name = "scale_free_network_edges.txt";

    if (argc > 2) {
        num_nodes = atoi(argv[1]);
    } else {
        printf("Please provide the number of nodes and m value as command-line arguments.\n");
        return 1;
    }

    int m = atoi(argv[2]);  // Number of connections for each new node

    srand(time(NULL)); // Seed the random number generator

    // Create the scale-free network
    output_file = fopen(output_file_name, "w");
    if (output_file != NULL) {
        int* degree_list = (int*)calloc(num_nodes, sizeof(int));
        struct ScaleFreeNetwork sf_network = {m0, m, num_nodes, degree_list};

        // Size of the buffer (assuming each edge takes about 20 characters)
        int buffer_size = m * 200;
		printf("%d\n", buffer_size);
        char* buffer = (char*)malloc(buffer_size);
        int buffer_index = 0;

        generate_network(output_file, &sf_network, buffer, buffer_size);

        free(buffer);
        free(degree_list);

        fclose(output_file);

        printf("Scale-free network edges have been written to %s\n", output_file_name);
    } else {
        printf("Error opening the output file.\n");
        return 1;
    }

    return 0;
}
