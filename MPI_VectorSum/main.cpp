#define _CRT_SECURE_NO_WARNINGS

#include <mpi.h>
#include <iostream>
#include <stdlib.h>
#include <Windows.h>

#include <ctime>
#include <stdio.h>

using namespace std;

int main(int argc, char* argv[]) {
	const int VECTOR_SIZE = 10;

	int vector[VECTOR_SIZE] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	int *resultVector;
	int size, rank;
	int result = 0;

	MPI_Status status;
	int partVectorSize;

	time_t rawTime;
	struct tm * timeInfo;
	char timeBuffer[80];

	MPI_Init(&argc, &argv);

	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	partVectorSize = VECTOR_SIZE / size;

	if (rank == 0) {
		time(&rawTime);
		timeInfo = localtime(&rawTime);

		strftime(timeBuffer, 80, "Now it's %I:%M%p.", timeInfo);
		cout << timeBuffer << endl;

		for (int i = 1; i < size; i++) {
			cout << "Sending to #" << i << endl;
			MPI_Send(&vector[(i - 1) * partVectorSize], partVectorSize, MPI_INT, i, 0, MPI_COMM_WORLD);
		}

		for (int i = 1; i < size; i++) {
			int tmp;
			cout << "Recieving from #" << i << endl;
			MPI_Recv(&tmp, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
			result += tmp;
		}

		for (int i = (size - 1) * partVectorSize; i < VECTOR_SIZE; i++) {
			result += vector[i];
		}

		cout << "result = " << result << endl;
	}
	else {
		resultVector = new int[partVectorSize];

		MPI_Recv(resultVector, partVectorSize, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
		for (int i = 0; i < partVectorSize; i++) {
			result += resultVector[i];
		}

		MPI_Send(&result, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
		delete[] resultVector;
	}

	MPI_Finalize();

	Sleep(2000);

	return 0;
}