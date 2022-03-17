/********************************************
 * Name: Isaac Grossberg
 * Date: 2/13/19
 * Function: Adventure
 * Description: Runs a game where the player
 * moves from room to room and attempts to
 * find the final room.
 * *****************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pthread.h>
#include <dirent.h>
#include <assert.h>

//Global room filepath array
char roomFiles[7][40];

//Mutex initialization
pthread_mutex_t myMutex = PTHREAD_MUTEX_INITIALIZER;

//Define Room struct
typedef struct{
	char name[7];
	int type;
	char connections[6][10];
	int numConnections;
}Room;

//Define Node struct for path linked list
typedef struct Node{
	struct Node* next;
	char name[7];
}Node;

/*****************************************
 * Function: getNewestDir
 * Input: char*
 * Output: None
 * Description: Finds the latest 
 * rooms directory and returns its name
 * (Code from Reading 2.4 Manipulating 
 * Directories; credit: Ben Brewster)
 * **************************************/
void GetNewestDir(char* newestDirName)
{
	int newestDirTime = -1;		//Modified timestamp of newest subdir examined
	char targetDirPrefix[32] = "grossbei.rooms."; //Prefix we're looking for
	memset(newestDirName, '\0', sizeof(newestDirName));

	DIR* dirToCheck;	//Holds the directory we're starting in
	struct dirent *fileInDir; //Holds the current subdir of the starting dir
	struct stat dirAttributes; //Holds information we've gained about subdir

	dirToCheck = opendir("."); //Open up the directory this program was run in

	if (dirToCheck > 0) //Make sure the current directory could be opened
	{
		while((fileInDir = readdir(dirToCheck)) != NULL) //Check each entry in dir
		{
			if (strstr(fileInDir->d_name, targetDirPrefix) != NULL) //If entry has prefix
			{
				stat(fileInDir->d_name, &dirAttributes); //Get attributes
			
				if ((int)dirAttributes.st_mtime > newestDirTime) //If this time is bigger
				{
					newestDirTime = (int)dirAttributes.st_mtime;
					memset(newestDirName, '\0', sizeof(newestDirName));
					strcpy(newestDirName, fileInDir->d_name);
				}
			}
		}
	}

	closedir(dirToCheck); //Close the directory we opened

}

/*******************************************
 * Function: GetRoomNames
 * Input: Array of rooms
 * Output: None
 * Description: Retrieves name information
 * from files and attaches it to room 
 * structs
 * ****************************************/
void GetRoomData(Room* roomArr[])
{
	char buffer[200];							//buffer for reading into
	char parsedStrings[8][10];		//data to be extracted

	int i;
	for (i=0; i<7; i++)																						//Loop through each room
	{
		memset(buffer, '\0', 200);																	//Clear out memory
		int file_descriptor = open(roomFiles[i], O_RDONLY, 0);			//Open file to read
		read(file_descriptor, buffer, 200);													//Read all data from file

		/*Parse through data and retrieve relevant info*/
		sscanf(buffer, "%*s%*s%s %*s%*s%s %*s%*s%s %*s%*s%s %*s%*s%s %*s%*s%s %*s%*s%s %*s%*s%s"
															 , parsedStrings[0], parsedStrings[1]
															 , parsedStrings[2], parsedStrings[3]
															 , parsedStrings[4], parsedStrings[5]
															 , parsedStrings[6], parsedStrings[7]);

		strcpy(roomArr[i]->name, parsedStrings[0]);									//Assign name

		int j=1;
		while (strstr(parsedStrings[j], "ROOM") == NULL)						//Loop through connections
		{
			strcpy(roomArr[i]->connections[j-1], parsedStrings[j]);		//Assign connections
			roomArr[i]->numConnections++;															//Increment connnection number
			j++;
		}

		/*Assign int for room type*/
		if (strstr(parsedStrings[j], "START") != NULL)							
		{
			roomArr[i]->type = 1;
		}
		else if (strstr(parsedStrings[j], "END") != NULL)
		{
			roomArr[i]->type = 3;
		}
		else
		{
			roomArr[i]->type = 2;
		}

		close(file_descriptor);																			//Close file
	}
	return;
}

/*******************************************
 * Function: GenerateRooms
 * Input: Array of rooms
 * Output: None
 * Description: Allocates memory for rooms
 * and calls function to assign them data
 * ****************************************/
void GenerateRooms(Room* roomArr[])
{
	int i;																			
	for (i=0; i<7; i++)
	{	
		Room* r1 = (Room *) malloc(sizeof(Room));	//Allocate memory for room
		assert(r1);																//Confirm allocation
		r1->numConnections = 0;										//Initialize connections to 0
		roomArr[i] = r1;													//Place into global array
	}

		GetRoomData(roomArr);
	return;
}


/*******************************************
 * Function: GetFirstRoom
 * Input: Array of rooms
 * Output: Pointer to start room
 * Description: Checks through array for
 * starter room and returns it
 * ****************************************/
Room* GetFirstRoom(Room* roomArr[])
{
	int i=0;
	while (i<7)
	{
		if (roomArr[i]->type == 1) return roomArr[i];
		i++;
	}
}

/*******************************************
 * Function: ValidateInput
 * Input: String input by user, current Room
 * Output: 1 (Valid input)
 * 				 0 (Invalid input)
 * Description: Checks that user input
 * matches one of the given room's
 * connections
 * *****************************************/
int ValidateInput(char input[], Room* x)
{
	int i=0;
	for (i=0; i < x->numConnections; i++)
	{
		if (strcmp(x->connections[i],input) == 0)
		{
			return 1;
		}
	}
	return 0;
}

/********************************************
 * Function: GetNextRoom
 * Input: User input string, array of Rooms
 * Output: User chosen room
 * Description: Searches through rooms for
 * room with name matching input string
 * *****************************************/ 
Room* GetNextRoom(char input[], Room* roomArr[])
{
	int i;
	for (i=0; i<7; i++)
	{
		if (strcmp(input, roomArr[i]->name) == 0)
			return roomArr[i];
	}
	return NULL;
}

/**********************************************
 * Function: AddToPath
 * Input: Head node, room to add, curr # steps
 * Output: None
 * Description: Adds given room to path and
 * updates number of steps
 * *******************************************/
void AddToPath(Node* head, Room* room)
{
	Node* curr = head;
	
	while (curr->next != NULL)										//Go to end of list
	{
		curr = curr->next;
	}
	Node* newNode = (Node*) malloc(sizeof(Node)); //Create new node
	curr->next = newNode;													//Add to list
	strcpy(newNode->name, room->name);						//Assign name to node
	return;
}

/**********************************************
 * Function: PrintPath
 * Input: Head node to path
 * Output: None
 * Description: Moves through path, printing
 * node names and freeing memory
 * (clears out linked list path)
 * *******************************************/
void PrintPath(Node* head)
{
	Node* curr = head;
	Node* next = curr->next;

	while (next != NULL)													//Go through whole list
	{
		printf("%s\n", next->name);									//Print each node name
		free(curr);																	//Free the node
		curr = next;																//Move to the next
		next = next->next;
	}
	return;
}

/**********************************************
 * Function: SetTime
 * Input: void*
 * Output: void*
 * Description: Creates text file with current
 * time written inside
 * ********************************************/
void* SetTime(void* val)
{
	pthread_mutex_lock(&myMutex);																							//Lock this thread
	//Open file to write to
	int file_descriptor = open("currentTime.txt", O_WRONLY | O_CREAT | O_TRUNC, 0755);
	struct tm *timeData;												
	time_t timeVal;
	char buffer[80];

	time(&timeVal);																														//Get time
	timeData = localtime(&timeVal);																						//Convert to local time

	strftime(buffer, sizeof(buffer), "%I:%M%p, %A, %B %d, %Y", timeData);			//Print formatted to buffer
	write(file_descriptor, buffer, sizeof(buffer));														//Write to file
	
	close(file_descriptor);																										//Close file

	pthread_mutex_unlock(&myMutex);																						//Unlock
	return;
}

/***********************************************
 * Function: GetTime
 * Input: none
 * Output: none
 * Description: Retrieves current time from
 * currentTime file and prints to screen
 * ********************************************/
void* GetTime()
{
	int file_descriptor = open("currentTime.txt", O_RDONLY, 0);		//Open file to read from
	char time[60];
	read(file_descriptor, time, 60);															//Read out time

	printf("%s\n\n", time);																				//Print time
	close(file_descriptor);																				//Close file
	return;
}

//Retrieve Information from files to structs

int main(int argc, char** argv)
{

	/*************************
 * 			SET UP ROUTINE
 **************************/
 	char roomsDirectory[256];															//String to hold 
	memset(roomsDirectory, '\0', 100);										//directory path

	GetNewestDir(roomsDirectory);
	const char* roomsDirectoryConst = roomsDirectory; 		//Convert to const char
	DIR *directory = opendir(roomsDirectoryConst);				//Open the directory
	struct dirent* fileInDir;															//Set up file pointer
	char roomPrefix[6] = "_room";													//Prefix to search for
	char filePath[256];																		//Actual file path to be changed
	strcat(roomsDirectory, "/");													//Adjust directory string
	int i = 0;
	while((fileInDir = readdir(directory)) != NULL)				//Read through all files
	{
		if (strstr(fileInDir->d_name, roomPrefix)!=NULL)		//Check for room prefix
		{
			strcpy(filePath, roomsDirectory);									//Copy over beginning of filepath
			strcat(filePath, fileInDir->d_name);							//Add file name
			memset(roomFiles[i], '\0', 40);										//Clear out string
			strcpy(roomFiles[i], filePath);										//Copy in complete filepath to array
			i++;
		}
	}
	closedir(directory);																	//Close directory
	Room* roomArr[7];																			//Set up struct array for ease of access
	GenerateRooms(roomArr);																//Create rooms from file info
	Room* currRoom;
	currRoom = GetFirstRoom(roomArr);											//Retrieve first room
	char input[100];																			//Buffer for user input
	Node* path = (Node*) malloc(sizeof(Node));						//Create path head node
	int numSteps = 0;																			//Initialize number of steps

	pthread_t myThread;																		//Name a thread
	pthread_mutex_lock(&myMutex);													//Lock thread
	pthread_create(&myThread, NULL, SetTime, NULL);				//Create thread

	/*************************
 * 				BEGIN GAME
 **************************/

	while(1)
	{
		printf("CURRENT LOCATION: %s\n", currRoom->name);		//Print out room name
		printf("POSSIBLE CONNECTIONS: ");										//Print out all connections
		int q=0;
		for (q=0; q<currRoom->numConnections; q++)
		{
			if (q == (currRoom->numConnections - 1))
			{
				printf("%s.\n",currRoom->connections[q]);
			}
			else
			{
				printf("%s, ",currRoom->connections[q]);
			}
		}
		printf("WHERE TO? >");															
		scanf("%s", input);																	//Get user input
		printf("\n");

		int valid = ValidateInput(input, currRoom);					//Valid the input from user
		if (valid)																				
		{
			currRoom = GetNextRoom(input, roomArr);						//Set up the next room
			AddToPath(path, currRoom);												//Add it to the path traveled
			numSteps++;																				//Increment number of steps
		}
		else if (strcmp(input, "time") == 0)
		{
			pthread_mutex_unlock(&myMutex);										//Unlock thread
			pthread_join(myThread, NULL);											//Join the thread
			pthread_mutex_lock(&myMutex);											//Re-lock the thread
			GetTime();
		}
		else
		{
			printf("HUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.\n\n");
		}

		if (currRoom->type == 3)														//WIN CONDITION
		{
			printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
			printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n", numSteps);
			PrintPath(path);
			return 0;
		}
	}
}

