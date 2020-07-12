echo "Test without OpenMP"
g++ log.cpp -o main  && ./main
echo "Test with OpenMP"
g++ log.cpp -o main -fopenmp && ./main 
