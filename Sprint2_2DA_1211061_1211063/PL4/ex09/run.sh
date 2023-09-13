n_process_vals=(1 2 4 8 16)
timeOfWork_values=(100 500 1000 10000 50000 500000 1000000)

for n_process in "${n_process_vals[@]}"
do
    for timeOfWork in "${timeOfWork_values[@]}"
    do
        echo "Running with $n_process processes, duration: $timeOfWork"
        make r ARGS="$n_process $timeOfWork"
        echo "##############################################################"
    done
done
