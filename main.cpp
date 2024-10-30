#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <limits>
#include <cmath>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>

// don't forget <sys/wait.h>
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
    int fileNum = intInput("\nWhich file would you like to use (File 1, File2, or File 3)?\nThere are 1000, 10000, and 100000 lines for each file respectively.\nPlease provide only an integer for the desired file (1, 2, or 3).", fileOptions);

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
            ifstream file(filename);
            
            // an integer to contain the sum from what the child read
            int childSumTotal = 0;

            for(int j = 0; j < linesToRead * i; j++)
            {
                // the number read from the line in the file
                int num = 0;

                // ignore the lines that have already been read by the previous child
                if(j != startingLine)
                {
                    file.ignore(1000, '\n');
                    continue;
                }
                
                childSumTotal += num;
            }

            // increase where the next child needs to start reading from
            startingLine += linesToRead - 1;

            file.close();
        }
    }

    // start parent code
    int completeTotal = 0;

    // calculate the sum by adding the smaller sums each child calculated
    for(int i = 0; i < numOfChildren; i++)
    {
        int childSumTotal = 0;
        // read(fds[0], &childSumTotal, sizeof(int));
        completeTotal += childSumTotal;
    }

    // display final results
    printf("The final sum for %s is: %d", filename, completeTotal);

    // timer (do NOT use clock) processes

    printf("The total time taken with %d children for %s is: ", time, filename);
}
