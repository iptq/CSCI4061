#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

typedef struct {
    char elem[2];
    double x, y, z;
    int charge;
    double mass;
} atom_t;

void write_atoms(char *fname, atom_t *atoms, int len) {
    int fd = open(fname, O_CREAT | O_WRONLY);
    for (int i = 0; i < len; ++i) {
        write(fd, &atoms[i], sizeof(atom_t));
    }
    close(fd);
}

double *read_masses(char *fname, int *setlen) {
    int fd = open(fname, O_RDONLY);
    int size = lseek(fd, 0, SEEK_END);
    double *result = (double *)malloc(size);
    lseek(fd, 0, SEEK_SET);
    int count = 0;
    atom_t atom;
    while (1) {
        int bytes = read(fd, &atom, sizeof(atom_t));
        if (!bytes) break;
        result[count++] = atom.mass;
    }
    close(fd);
    *setlen = count;
    return result;
}

int main() {
    atom_t atoms[5] = {{"C", 0.0, 1.0, 2.5, 0, 12.011},
                       {"O", 1.0, 3.5, 6.0, 0, 15.999}};
    write_atoms("atoms.dat", atoms, 5);
    int len = -1;
    double *masses = read_masses("atoms.dat", &len);
    printf("read %d masses:\n", len);
    for (int i = 0; i < len; ++i) {
        printf("mass[%d] = %lf\n", i, masses[i]);
    }
    free(masses);
    return 0;
}
