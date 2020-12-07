from subprocess import call

for i in range(30):
  call(["mpirun", "-np" , "6", "./nbody_simulation", "12", "input_files/nbody_input-16384_16384.in"])
