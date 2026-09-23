 cd ../build/ && ninja && cd ../scripts/ && ./run_vi_benchmark200.sh 2>&1 |tee output.txt


# SAVE_EVERY=2
# NUM_ITERATIONS=80
# CHUNK_SIZE=20
# for (( start=SAVE_EVERY; start<=NUM_ITERATIONS; start+=CHUNK_SIZE ));
#  do
#   chunk_end=$(( start + CHUNK_SIZE - SAVE_EVERY )) 
#   echo "start = ${start}"
#   for (( i=start; i<=chunk_end; i+=SAVE_EVERY ));
#   do
#   echo ${i}
# done
# echo "\n"
# done


