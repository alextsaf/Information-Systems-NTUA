#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

struct Point {
    double x;
    double y;
};

double euclidean_distance(struct Point point1, struct Point point2) {
    return sqrt(pow((point1.x - point2.x), 2) + pow((point1.y - point2.y), 2));
}

void random_geometric_graph(int num_nodes, double radius, const char* output_file) {
    FILE* fp = fopen(output_file, "w");
    if (fp == NULL) {
        printf("Error opening the file.\n");
        return;
    }

    struct Point* points = (struct Point*)malloc(num_nodes * sizeof(struct Point));
    if (points == NULL) {
        printf("Memory allocation error.\n");
        fclose(fp);
        return;
    }

    /* Generation can be parallelized. Shared point data sucks because of constant cache movement.
        Split buffer between threads, e.g. 4 threads each thread gets /4 of a buffer and generate for this
        specific region. Reduce buffer in the end (sequentially to not cause bus congestion) */

    srand((unsigned int)time(NULL));
    for (int i = 0; i < num_nodes; i++) {
        points[i].x = (double)rand() / RAND_MAX;
        points[i].y = (double)rand() / RAND_MAX;
    }

    /* At this point, you have (num_nodes * 16bytes) allocated. Memory Mapped file writing could be faster??? */

    for (int i = 0; i < num_nodes; i++) {
        for (int j = i + 1; j < num_nodes; j++) {
            double distance = euclidean_distance(points[i], points[j]);
            if (distance <= radius) {
                fprintf(fp, "%d %d\n", i, j);
            }
        }
    }

    fclose(fp);
    free(points);
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        printf("Usage: %s num_nodes radius output_file\n", argv[0]);
        return 1;
    }

    int num_nodes = atoi(argv[1]);
    double radius = atof(argv[2]);
    const char* output_file = argv[3];

    random_geometric_graph(num_nodes, radius, output_file);

    return 0;
}
