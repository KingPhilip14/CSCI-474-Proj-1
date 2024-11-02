#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <chrono>
#include <sys/wait.h>

using namespace std;

/**
* Prints the given message and asks the user for an int as input. Prompts the user again if invalid input is given.
*/
int intInput(string message, vector<int> numbers)
{
    int numOfChildren = 0;
    cout << message << endl << "> ";

    // Read in the user's input 
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
    
    int numOfChildren = intInput("How many children would you like to use for the process?\nChoose 1, 2, or 4 children.", childOptions);

    // the options for which file the user can use
    vector<int> fileOptions = { 1, 2, 3 };
    
    int fileNum = intInput("\nWhich file would you like to use (file1, file2, or file3)?\nThere are 1000, 10000, and 100000 lines for each file respectively.\nPlease provide only an integer for the desired file (1, 2, or 3).", fileOptions);

    auto startTime = chrono::high_resolution_clock::now();

    string filename = "file" + to_string(fileNum) + ".dat";

    // calculate the number of lines each child process should read 
    int linesToRead = pow(10.0, fileNum + 2) / numOfChildren;

    // an integer to tell a child which line in the file it needs to start on
    int startingLine = 0;

    int fds[2];
    pipe(fds);

    for(int i = 0; i < numOfChildren; i++)
    {
        // create the number of children specified
        if(fork() == 0)
        {
            ifstream file(filename);

            // an integer to contain the sum from what the child read
            int childSum = 0;

            for(int j = 0; j < linesToRead * (i + 1); j++)
            {
                string line;

                // ignore the lines that have already been read by the previous child
                if(j < startingLine)
                {
                    getline(file, line);
                    continue;
                }

                getline(file, line);
                childSum += stoi(line);
            }

            file.close();

            printf("\nChild %d has the following sum: %d\n", i + 1, childSum);

            // write this child's sum to the pipe
            write(fds[1], &childSum, sizeof(int));

            exit(0);
        }

        // increase where the next child needs to start reading from
        startingLine += linesToRead;
    }

    // start parent code
    int finalSum = 0;

    // calculate the sum by adding the smaller sums each child calculated
    for(int i = 0; i < numOfChildren; i++)
    {
        int childSum = 0;
        read(fds[0], &childSum, sizeof(int));
        finalSum += childSum;
    }

    auto endTime = chrono::high_resolution_clock::now();
    auto finalTime = chrono::duration_cast<chrono::microseconds>(endTime - startTime).count();

    printf("\n\nThe final sum for %s is: %d\n", filename.c_str(), finalSum);
    printf("The total time taken with %d child(ren) for %s is: %ld ms\n\n", numOfChildren, filename.c_str(), finalTime);

    return 0;
}
