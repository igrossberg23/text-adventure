/************************************************************
* Name: Isaac Grossberg
* Date: 2/13/19
* Function: BuildRooms
* Description: Script to generate a 'graph'
* of rooms that adhere to connectedness
* guidelines
* Note: This is a very convoluted method
* and could be done much more efficiently
* using an actual graph
* **********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>

//Define Room struct
typedef struct{
	char name[7];
	int type;
	char connections[6][10];
	int numConnections;
}Room;

/***********************************
 * Function: RandomNum
 * Input: Maximum value
 * Output: Random number 0-MAX - 1
 * Description: Returns random
 * number between 0 and MAX - 1
 * ********************************/
int RandomNum(int max)
{
	int val = rand() % max;
	return val;
}

/*******************************
 * Function: AssignNames
 * Input: Array of Room ptrs
 * Output: None
 * Description: Assigns names
 * to each room in the array
 * *****************************/
void AssignNames(Room* roomArr[])
{	
	//Array of possible room names
	char roomNames[10][10];
	strcpy(roomNames[0],"Bubbles");
	strcpy(roomNames[1],"Ricky");
	strcpy(roomNames[2],"Julian");
	strcpy(roomNames[3],"JRoc");
	strcpy(roomNames[4],"Lahey");
	strcpy(roomNames[5],"Randy");
	strcpy(roomNames[6],"Barb");
	strcpy(roomNames[7],"Ray");
	strcpy(roomNames[8],"Cory");
	strcpy(roomNames[9],"Trevor");

	int i=6, j=0;
	do
	{	
		int position = RandomNum(10);				 						//Pick random name
		if (strcmp(roomNames[position],"EMPTY") != 0)		//Make sure name hasn't been picked already
		{	
			strcpy(roomArr[j]->name, roomNames[position]);//Assign name
			strcpy(roomNames[position], "EMPTY");		 			//Clear out chosen name
			i--;
			j++;
		}
	} while (i >= 0);
	return;
}

/************************************
 * Function: AssignTypes
 * Input: Array of Room ptrs
 * Output: None
 * Description: Assigns one start
 * and one end room from the given
 * array, remaining  rooms are
 * middle type
 * *********************************/
void AssignTypes(Room* roomArr[])
{
	int start, end;
	do {
		start = RandomNum(7); 										//Pick random start
		end = RandomNum(7);												//Pick random end
	}while (start == end);											//Ensure they aren't the same
	
	roomArr[start]->type=1;											//Designate start
	roomArr[end]->type=3;												//Designate end
	int i=0;
	while (i<7) {
		if (roomArr[i]->type != 1 && roomArr[i]->type != 3)
			roomArr[i]->type = 2;										//middle rooms
		i++;
	}
	return;
}

/***************************************
 * Function: GenerateRooms
 * Input: Array of Room ptrs
 * Output: None
 * Description: Allocates memory for
 * rooms in given array and 
 * initializes their data members
 * *************************************/
void GenerateRooms(Room* roomArr[])
{
	int i=0;			
	for (i=0; i<7; i++) {
		Room* r1 = (Room *) malloc(sizeof(Room));	//Allocate memory for Room
		assert(r1);	
		r1->numConnections = 0;										//Initialize num connections
		roomArr[i] = r1;													//Add Room to array
	}

	AssignTypes(roomArr);												//Assign Types
	AssignNames(roomArr);												//Assign Names
}

/*****************************************************
 * Function: IsGraphFull
 * Input: Array of Room ptrs
 * Output: 1 (Graph full)
 * 	   0 (Graph not full)
 * Description: Returns true if all rooms have 3-6
 * connections, false o.w.
 * **************************************************/
int IsGraphFull(Room* roomArray[])
{
	int i=0;
	while (i < 7) {
		if ( roomArray[i]->numConnections < 3) return 0;
		i++;
	}
	return 1;	//If all pass then Graph is full
}

/*****************************************************
 * Function: GetRandomRoom
 * Input: Array of Room ptrs
 * Output: Single Room ptr
 * Description: Returns random room w/o checking for
 * validity of data members
 * **************************************************/
Room* GetRandomRoom(Room* roomArray[])
{
	int position = RandomNum(7);
	Room* A;
	A = roomArray[position];
	return A;			
}

/*************************************************************
 * Function: CanAddConnectionFrom
 * Input: Room ptr
 * Output: 1 (Room has available connection)
 * 	   0 (No available connections)
 * Description: Returns true if a connection can be
 * added from Room x (< 6 outbound connections), false o.w.
 * **********************************************************/
int CanAddConnectionFrom(Room* x)
{
	if (x->numConnections < 6)
	{
		 return 1;
	}
	else
	{
		return 0;
	}
}

/**********************************************************
 * Function: ConnectionAlreadyExists
 * Input: Two Room ptrs
 * Output: 1 (Connection exists already)
 * 	   0 (Connection doesnt exist)
 * Description: Returns true if a connection from Room x
 * to Room y already exists, false o.w.
 * *******************************************************/
int ConnectionAlreadyExists(Room* x,Room* y)
{
	int i=0;
	while (i<6) {
		if (strcmp(x->connections[i], y->name) == 0)
			return 1;
		i++;
	}
	return 0;
}

/**********************************************************
 * Function: ConnectRoom
 * Input: Two Room ptrs
 * Output: None
 * Description: Connects Rooms x and y together (does not
 * check if connection is valid)
 * *******************************************************/
void ConnectRoom(Room* x, Room* y)
{
	strcpy( x->connections[x->numConnections], y->name );
	x->numConnections++;
	return;
}

/************************************************
 * Function: IsSameRoom
 * Input: Two Room ptrs
 * Output: 1 (Rooms are the same)
 * 	   0 (Rooms are different)
 * Description: Returns true if Rooms x and y
 * are thesame Room, false o.w.
 * *********************************************/
int IsSameRoom(Room* x, Room* y)
{
	if (strcmp(x->name, y->name) == 0) 
		return 1;
	else 
		return 0;
}	

/********************************************************
 * Function: AddRandomConnection
 * Input: Array of Room ptrs
 * Output: None
 * Description: Adds a random, valid connection
 * from one Room to another Room in both directions
 * *****************************************************/
void AddRandomConnection(Room* roomArr[])
{
	Room* A;																			//Create Room
	Room* B;																			//pointers

	while(1)
	{
		A = GetRandomRoom(roomArr);									//Pick room for A
		if (CanAddConnectionFrom(A) == 1)						//Check if A is valid
			break;
	}
	do
	{
		B = GetRandomRoom(roomArr);									//Pick room for B
	}//Validity checks
	while(CanAddConnectionFrom(B) == 0 || IsSameRoom(A,B) == 1 || ConnectionAlreadyExists(A, B) == 1);
	
	ConnectRoom(A, B);														//Make connection one way
	ConnectRoom(B, A); 														//Create returning connection
	return;
}

int main()
{
	srand(time(NULL));
	
	Room* RoomArray[7];														//Create array of Rooms
	GenerateRooms(RoomArray);											//Populate array

	int i=0;
	//Create all connections in a 'graph'
	while (!IsGraphFull(RoomArray) && i<200)
	{	
		AddRandomConnection(RoomArray);
		i++;
	}


	int pid = getpid();														//Get the process ID
	char pidStr[10];															//Create char array
	memset(pidStr, '\0', 10);											//Set array to all 0s
	char dirName[40];
	memset(dirName, '\0', 40);
	strcpy(dirName, "grossbei.rooms.");						//Set beginning of directory path

	sprintf(pidStr, "%d", pid);										//Convert PID to string
	strcat(dirName, pidStr);											//Add PID to end of directory name
	mkdir(dirName, 0755);													//Create directory with permissions
	
	//Create file for each room
	for (i=0; i<7; i++)	
	{
		Room* r1;
		r1 = RoomArray[i];

		//Create & clear strings to be written		
		char roomName[30];		
		memset(roomName, '\0', 30);	
		char connection[30];	
		memset(connection, '\0', 30);	
		char roomType[30];		
		memset(roomType, '\0', 30);
		char pathfile[40];
		memset(pathfile, '\0', 40);
		char fileName[40];
		memset(fileName, '\0', 40);
		
		sprintf(pathfile, "%s/%s_room", dirName, r1->name);											//Pathfile with room name
		int file_descriptor;
		file_descriptor = open(pathfile, O_WRONLY | O_CREAT, 0600);							//Open file to be written
	
		sprintf(roomName, "ROOM NAME: %s\n", r1->name);													//Retrive room name
		write(file_descriptor, roomName, strlen(roomName) * sizeof(char));			//Write room name to file
		
		int j;
		for (j=0; j<r1->numConnections; j++)																		//Loop through connections
		{
			memset(connection, '\0', 30);																					//Reset string
		 	sprintf(connection, "CONNECTION %d: %s\n", j+1, r1->connections[j]);	//Retrieve respective connection name
			write(file_descriptor, connection, strlen(connection) * sizeof(char));//Write connection to file
		}

		strcpy(roomType, "ROOM TYPE: ");																				//Write room type based on
		if (r1->type == 1)																											//type data member
		{
			strcat(roomType, "START_ROOM\n");
		}
		else if (r1->type == 3)
		{
			strcat(roomType, "END_ROOM\n");
		}
		else
		{
			strcat(roomType, "MID_ROOM\n");
		}
		write(file_descriptor, roomType, strlen(roomType) * sizeof(char));

		free(r1);																																//Free Room	
		close(file_descriptor);																									//Close file
	}

return 0;
}
