#define _CRT_SECURE_NO_WARNINGS

#include <mpi.h>
#include <iostream>
#include <stdlib.h>
#include <Windows.h>

#include <ctime>
#include <stdio.h>

using namespace std;

int main(int argc, char* argv[]) {
	const int VECTOR_SIZE = 100000;
	typedef unsigned long long  VectorType;

	//int vector[VECTOR_SIZE] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	
	VectorType *vector;
	VectorType *resultVector;
	int size, rank;
	VectorType result = 0;

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
		double startTime = 0.0 , finishTime = 0.0;

		time(&rawTime);
		timeInfo = localtime(&rawTime);

		strftime(timeBuffer, 80, "Now it's %I:%M%p.", timeInfo);
		cout << timeBuffer << endl;

		startTime = MPI_Wtime();

		vector = new VectorType[VECTOR_SIZE];
		for (VectorType i = 0; i < VECTOR_SIZE; i++) {
			vector[i] = i;
		}

		for (int i = 1; i < size; i++) {
			cout << "Sending to #" << i << endl;
			MPI_Send(&vector[(i - 1) * partVectorSize], partVectorSize, MPI_UNSIGNED_LONG_LONG, i, 0, MPI_COMM_WORLD);
		}

		for (int i = 1; i < size; i++) {
			VectorType tmp;
			cout << "Recieving from #" << i << endl;
			MPI_Recv(&tmp, 1, MPI_UNSIGNED_LONG_LONG, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
			result += tmp;
		}

		for (VectorType i = (size - 1) * partVectorSize; i < VECTOR_SIZE; i++) {
			result += vector[i];
		}

		cout << "result = " << result << endl;
		
		finishTime = MPI_Wtime();
		cout << "time " << finishTime - startTime << endl;
	}
	else {
		resultVector = new VectorType[partVectorSize];

		MPI_Recv(resultVector, partVectorSize, MPI_UNSIGNED_LONG_LONG, 0, 0, MPI_COMM_WORLD, &status);
		for (VectorType i = 0; i < partVectorSize; i++) {
			result += resultVector[i];
		}

		MPI_Send(&result, 1, MPI_UNSIGNED_LONG_LONG, 0, 0, MPI_COMM_WORLD);
		delete[] resultVector;
	}

	MPI_Finalize();

	Sleep(2000);

	return 0;
}