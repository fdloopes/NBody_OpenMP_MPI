#include <mpi.h>

#include <cmath>
#include <cstdio>
#include <iostream>

#include "nbody.hpp"

void calculate_force(Particle* this_particle1, Particle* this_particle2,
                     float* force_x, float* force_y, float* force_z) {
    /* Particle_calculate_force */
    float difference_x, difference_y, difference_z;
    float distance_squared, distance;
    float force_magnitude;

    difference_x = this_particle2->position_x - this_particle1->position_x;
    difference_y = this_particle2->position_y - this_particle1->position_y;
    difference_z = this_particle2->position_z - this_particle1->position_z;

    distance_squared = difference_x * difference_x +
                       difference_y * difference_y +
                       difference_z * difference_z;

    distance = std::sqrt(distance_squared);  //sqrtf(distance_squared);

    force_magnitude = GRAVITATIONAL_CONSTANT * (this_particle1->mass) * (this_particle2->mass) / distance_squared;

    *force_x = (force_magnitude / distance) * difference_x;
    *force_y = (force_magnitude / distance) * difference_y;
    *force_z = (force_magnitude / distance) * difference_z;
}

void nbody(Particle* d_particles, Particle* output, int worldSize, int myRank) {
    // de acordo com o rank é a base onde vai começar a calcular
    int base = myRank * number_of_particles / worldSize;
    int fim = (myRank + 1) * number_of_particles / worldSize;
    //printf("%d: minha base é:%d e meu fim é:%d\n", myRank, base, fim);

    for (int id = base; id < fim; id++) {
        Particle* this_particle = &output[id];

        float force_x = 0.0f, force_y = 0.0f, force_z = 0.0f;
        float total_force_x = 0.0f, total_force_y = 0.0f, total_force_z = 0.0f;

        int i;
        for (i = 0; i < number_of_particles; i++) {
            if (i != id) {
                calculate_force(d_particles + id, d_particles + i, &force_x, &force_y, &force_z);

                total_force_x += force_x;
                total_force_y += force_y;
                total_force_z += force_z;
            }
        }

        float velocity_change_x, velocity_change_y, velocity_change_z;
        float position_change_x, position_change_y, position_change_z;

        this_particle->mass = d_particles[id].mass;

        velocity_change_x = total_force_x * (time_interval / this_particle->mass);
        velocity_change_y = total_force_y * (time_interval / this_particle->mass);
        velocity_change_z = total_force_z * (time_interval / this_particle->mass);

        position_change_x = d_particles[id].velocity_x + velocity_change_x * (0.5 * time_interval);
        position_change_y = d_particles[id].velocity_y + velocity_change_y * (0.5 * time_interval);
        position_change_z = d_particles[id].velocity_z + velocity_change_z * (0.5 * time_interval);

        this_particle->velocity_x = d_particles[id].velocity_x + velocity_change_x;
        this_particle->velocity_y = d_particles[id].velocity_y + velocity_change_y;
        this_particle->velocity_z = d_particles[id].velocity_z + velocity_change_z;

        this_particle->position_x = d_particles[id].position_x + position_change_x;
        this_particle->position_y = d_particles[id].position_y + position_change_y;
        this_particle->position_z = d_particles[id].position_z + position_change_z;
    }
}

/* main */
int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "Informe um arquivo com os parâmetros de entrada: ./nbody_simulation <input_file.in>\n";
        std::abort();
    }
    int worldSize, myRank;
    Particle* particle_array = nullptr;
    Particle* particle_array2 = nullptr;

    /* initialize MPI stuff */
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &worldSize);
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
    // constroi a struct para o MPI
    MPI_Datatype Particle_t;
    MPI_Datatype types[8] = {MPI_FLOAT, MPI_FLOAT, MPI_FLOAT, MPI_FLOAT, MPI_FLOAT, MPI_FLOAT, MPI_FLOAT, MPI_FLOAT};
    int blocklen[8] = {1, 1, 1, 1, 1, 1, 1, 1};
    MPI_Aint disp[8];
    Particle temp[1];
    disp[0] = (int)(&temp[0].position_x) - (int)&temp[0];
    disp[1] = (int)(&temp[0].position_y) - (int)&temp[0];
    disp[2] = (int)(&temp[0].position_z) - (int)&temp[0];
    disp[3] = (int)(&temp[0].velocity_x) - (int)&temp[0];
    disp[4] = (int)(&temp[0].velocity_y) - (int)&temp[0];
    disp[5] = (int)(&temp[0].velocity_z) - (int)&temp[0];
    disp[6] = (int)(&temp[0].mass) - (int)&temp[0];
    disp[7] = (int)(&temp[0].pad) - (int)&temp[0];

    MPI_Type_create_struct(8, blocklen, disp, types, &Particle_t);
    MPI_Type_commit(&Particle_t);

    FILE* input_data = fopen(argv[1], "r");
    Particle_input_arguments(input_data);

    particle_array = Particle_array_construct(number_of_particles);
    particle_array2 = Particle_array_construct(number_of_particles);
    long start;

    if (myRank == 0) {
        Particle_array_initialize(particle_array, number_of_particles);
        start = wtime();
        printf("Processando simulação NBody....\n");
    }

    for (int timestep = 1; timestep <= number_of_timesteps; timestep++) {
        MPI_Bcast(particle_array, number_of_particles, Particle_t, 0, MPI_COMM_WORLD);
        MPI_Bcast(particle_array2, number_of_particles, Particle_t, 0, MPI_COMM_WORLD);
        //  printf("%d:primeira particula: %f, %f e %f\n", myRank, particle_array[0].position_x, particle_array[0].position_y, particle_array[0].position_z);
        nbody(particle_array, particle_array2, worldSize, myRank);
        // precisa fazer aqui uma forma de pegar só a parte que cada rank fez

        MPI_Gather(&particle_array[myRank * number_of_particles / worldSize], number_of_particles / worldSize, Particle_t, particle_array, number_of_particles / worldSize, Particle_t, 0, MPI_COMM_WORLD);
        MPI_Gather(&particle_array2[myRank * number_of_particles / worldSize], number_of_particles / worldSize, Particle_t, particle_array2, number_of_particles / worldSize, Particle_t, 0, MPI_COMM_WORLD);

        if (myRank == 0) {
            //  printf("DENTRO DO IF => %d:primeira particula: %f, %f e %f\n", myRank, particle_array[0].position_x, particle_array[0].position_y, particle_array[0].position_z);
            /* swap arrays */
            Particle* tmp = particle_array;
            particle_array = particle_array2;
            particle_array2 = tmp;
            printf("   Iteração %d OK\n", timestep);
        }
    }

    if (myRank == 0) {
        long end = wtime();
        double time = (end - start) / 1000000.0;

        printf("Simulação NBody executada com sucesso.\n");
        printf("Nro. de Partículas: %d\n", number_of_particles);
        printf("Nro. de Iterações: %d\n", number_of_timesteps);
        printf("Tempo: %.8f segundos\n", time);

        FILE* fileptr = fopen("simulations.txt", "a");
        fprintf(fileptr, "%.8f\n", time);
    }

#ifdef VERBOSE
    //Imprimir saída para arquivo
    printf("\nImprimindo saída em arquivo...\n");
    FILE* fileptr = fopen("nbody_simulation.out", "w");
    Particle_array_output_xyz(fileptr, particle_array, number_of_particles);
    printf("Saída da simulação salva no arquivo nbody_simulation.out\n");
#endif

    if (myRank == 0) {
        particle_array = Particle_array_destruct(particle_array, number_of_particles);
        particle_array2 = Particle_array_destruct(particle_array2, number_of_particles);
    } else {
        free(particle_array);
        free(particle_array2);
    }

    /* finalize MPI */
    MPI_Finalize();
    return PROGRAM_SUCCESS_CODE;
}
