#pragma once
#ifndef STORAGE_H
#define STORAGE_H

struct Task {
	char title[64];
	bool isDone;
	bool isActive;
};

const int MAX_TASKS = 20;
extern Task taskList[MAX_TASKS];
extern int taskCount;

void saveTasks();
void loadTasks();




#endif	

	