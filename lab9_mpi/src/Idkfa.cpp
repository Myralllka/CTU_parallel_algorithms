#include <iostream>
#include <string>
#include <mpi.h>

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    int my_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    if (my_rank == 0) {
        // Sends message.
        std::string message = "IDKFA";

        MPI_Request request;
        MPI_Isend(message.c_str(),
                  message.length() + 1,   // +1 is for '\0'
                  MPI_CHAR,
                  1,
                  0,
                  MPI_COMM_WORLD,
                  &request);

        MPI_Wait(&request, MPI_STATUS_IGNORE);
    } else if (my_rank == 1) {
        // Receives message.
        constexpr int buf_size = 256;
        char buf[buf_size];

        MPI_Request request;
        MPI_Irecv(buf,
                  buf_size,  // We "do not know" the size of message, receive at most buf_size elements.
                  MPI_CHAR,
                  0,
                  MPI_ANY_TAG,
                  MPI_COMM_WORLD,
                  &request);

        MPI_Status status;
        MPI_Wait(&request, &status);

        int received_size;
        MPI_Get_count(&status, MPI_CHAR, &received_size);

        std::cout << "Received message size: " << received_size << std::endl;
        std::cout << "Received message content: " << std::string(buf) << std::endl;
    }


    MPI_Finalize();
}
