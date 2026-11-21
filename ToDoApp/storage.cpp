#include <iostream>
#include <fstream>
#include "storage.h"
#include <ios>
#include <string>
using namespace std;

Task taskList[MAX_TASKS] = { 0 };
int taskCount = 0;
void saveTasks() {

	ofstream myFile("tasks.txt");

	if (!myFile.is_open()) { return; }
	for (int i = 0;i < taskCount; i++) {
		if (!taskList[i].isActive) { continue;}
		
		if (taskList[i].isDone) { myFile << '1';}
		else { myFile << '0';}
		myFile << "|";
		myFile << taskList[i].title << "\n";
	}

	myFile.close();
}

void loadTasks() {
	ifstream myFile("tasks.txt");
	if (!myFile.is_open()) { return; }

	taskCount = 0;
	string temp;

	while (taskCount < MAX_TASKS && getline(myFile, temp)) {
		
		if (temp.length() < 3 || temp[1] != '|') { continue; }
		
		(temp[0] == '1') ? taskList[taskCount].isDone = true : taskList[taskCount].isDone = false;
		
		taskList[taskCount].isActive = true;

		//string to character array copying

		string extracted = temp.substr(2, 63);
		strncpy_s(taskList[taskCount].title, extracted.c_str(), _TRUNCATE);
		
		taskCount++;
	}
	myFile.close();
}

