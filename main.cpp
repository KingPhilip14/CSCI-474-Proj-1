#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <chrono>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

using namespace std;

/**
 * Returns an integer by reading in the user's input. It will print the given message and ask the 
 * user to provide a valid integer. A valid integer is determined by the vector passed in. The vector will contain 
 * the valid integers for the given prompt.
 */
int intInput(string message, vector<int> numbers)
{
    int numOfChildren = 0;
    cout << message << endl << "> ";

    scanf("%d", &numOfChildren);

    while(count(numbers.begin(), numbers.end(), numOfChildren) <= 0)
    {
        cout << "\nAn invalid input was given. Please enter a valid integer.\n" << message << endl << "> ";
        scanf("%d", &numOfChildren);
    }

    return numOfChildren;
}

int main()
{
    // the options for how many children the user can use
    vector<int> childOptions = { 1, 2, 4 };
    
    // store the user's input for how many children they selected
    int numOfChildren = intInput("How many children would you like to use for the process?\nChoose 1, 2, or 4 children.", childOptions);

    // the options for which file the user can use
    vector<int> fileOptions = { 1, 2, 3 };
    
    // store the user's input for the file they selected
    int fileNum = intInput("\nWhich file would you like to use (file1, file2, or file3)?\nThere are 1000, 10000, and 100000 lines for each file respectively.\nPlease provide only an integer for the desired file (1, 2, or 3).", fileOptions);

    auto startTime = chrono::high_resolution_clock::now();

    string filename = "file" + to_string(fileNum) + ".dat";

    // calculate the number of lines each child process should read 
    int linesToRead = pow(10.0, fileNum + 2) / numOfChildren;

    // an integer to tell a child which line in the file it needs to start on
    int startingLine = 0;

    // create the pipe
    int fds[2];
    pipe(fds);

    for(int i = 0; i < numOfChildren; i++)
    {
        // create the number of children specified
        if(fork() == 0)
        {
            // make an ifstream object to read from the file
            // ifstream file(filename);
            FILE * file;

            // an integer to contain the sum from what the child read
            int childSum = 0;

            fseek(file, (i * linesToRead * 5), SEEK_SET);

            for(int j = 0; j < linesToRead * (i + 1); j++)
            {
                int num = 0;
                fscanf(file, "%d\n", &num);
                childSum += num;

                // ignore the lines that have already been read by the previous child
                // if(j != startingLine)
                // {
                //     file.ignore(1000, '\n');
                //     continue;
                // }

                // string line;

                // while(getline(file, line))
                // {
                //     childSum += stoi(line);
                // }
            }

            file.close();

            printf("Child %d has the following sum: %d\n\n", i + 1, childSum);

            // write this child's sum to the pipe
            write(fds[1], &childSum, sizeof(int));

            // increase where the next child needs to start reading from
            startingLine += linesToRead - 1;

            cout << "Starting line: " << startingLine;

            exit(0);
        }
    }

    // start parent code
    int completeTotal = 0;

    // calculate the sum by adding the smaller sums each child calculated
    for(int i = 0; i < numOfChildren; i++)
    {
        int childSum = 0;
        read(fds[0], &childSum, sizeof(int));
        completeTotal += childSum;
    }

    auto endTime = chrono::high_resolution_clock::now();

    auto finalTime = chrono::duration_cast<chrono::microseconds>(endTime - startTime).count();

    // display final results
    printf("\n\nThe final sum for %s is: %d\n", filename.c_str(), completeTotal);
    printf("The total time taken with %d child(ren) for %s is: %ld ms\n\n", numOfChildren, filename.c_str(), finalTime);

    return 0;
}
