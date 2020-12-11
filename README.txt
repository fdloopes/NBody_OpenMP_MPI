/* Conteúdo do diretório */

main.cpp 	// Chamadas principais do problema NBody com MPI + OpenMP
main_seq.cpp // Chamadas principais do problema NBody apenas com MPI, sem trecho de código com OpenMP
nbody.cpp	// Instanciação de variaveis globais, Argumentos e funções utilizadas no problema
nbody.hpp	// Definições de funções e variaveis
Makefile	// Responsável por facilitar a compilação do programa
input_files	// Diretório que possui os arquivos de entrada

/* Dicas para compilar e executar o programa */

Comando para compilar o programa com OpenMP + MPI: make mpi

Comando para compilar o programa com apenas MPI: make seq

Comando para limpar executaveis: make clean

Comando para executar o programa com MPI + OpenMP:
  mpirun -np <numero de processos> ./nbody_simulation <numero de threads> <arquivo de entrada>

Comando para executar o programa apenas com implementação MPI:
  mpirun -np <numero de processos> ./nbody_simulation_seq <arquivo de entrada>

<numero de processos> // Passar um inteiro relativo ao número de processos que se deseja definir
<numero de threads>   // Passar um inteiro relativo ao número de threads que se deseja definir
<arquivo de entrada>  // Aqui passa o caminho "input_files/nbody_input-<tamanho da entrada>.in"
<tamanho da entrada>  // O tamanho pode ser 16384_16384, 32768_32768 ou 65536_65536

/* Exemplo de comando para executar programa com MPI + OpenMP */

mpirun -np 4 ./nbody_simulation 4 input_files/nbody_input-32768_32768.in

/* Exemplo de comando para executar programa com apenas MPI */

mpirun -np 4 ./nbody_simulation_seq input_files/nbody_input-32768_32768.in
