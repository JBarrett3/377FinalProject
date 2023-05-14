#include <scheduling.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <list>
#include <queue>
#include <string>
#include <fstream>

using namespace std;

int id = 0;
pqueue_arrival read_workload(string filename) {
  pqueue_arrival workload;
  
  fstream file;
  file.open(filename);

  string line;
  string arrival;
  string duration;
  while (1) {
    getline(file,line);
    if (line == "") {
      break;
    }
    stringstream lineStream(line);
    lineStream >> arrival;
    lineStream >> duration;
    Process work {stoi(arrival), -1, stoi(duration), -1, 0, 0, id};
    id++;
    workload.push(work);
    line = "";
  }

  file.close();

  return workload;
}

void show_workload(pqueue_arrival workload) {
  pqueue_arrival xs = workload;
  cout << "Workload:" << endl;
  while (!xs.empty()) {
    Process p = xs.top();
    cout << '\t' << p.arrival << ' ' << p.duration << endl;
    xs.pop();
  }
}

void show_processes(list<Process> processes) {
  list<Process> xs = processes;
  cout << "Processes:" << endl;
  while (!xs.empty()) {
    Process p = xs.front();
    cout << "\tarrival=" << p.arrival << ", duration=" << p.duration
         << ", first_run=" << p.first_run << ", completion=" << p.completion
         << endl;
    xs.pop_front();
  }
}

void updateArrivedJobs(pqueue_arrival *unarrivedJobs, pqueue_arrival *arrivedJobs, int time) {
  while (!(*unarrivedJobs).empty()) {
    Process p = (*unarrivedJobs).top();
    if (p.arrival <= time) {
      (*arrivedJobs).push(p);
    } else {
      break;
    }
    (*unarrivedJobs).pop();
  }
}

void updateArrivedJobs(pqueue_arrival *unarrivedJobs, pqueue_duration *arrivedJobs, int time) {
  while (!(*unarrivedJobs).empty()) {
    Process p = (*unarrivedJobs).top();
    if (p.arrival <= time) {
      (*arrivedJobs).push(p);
    } else {
      break;
    }
    (*unarrivedJobs).pop();
  }
}

list<Process> fifo(pqueue_arrival workload) {
  list<Process> complete;
  int time = 0;

  pqueue_arrival unarrivedJobs = workload;
  pqueue_arrival arrivedJobs;
  updateArrivedJobs(&unarrivedJobs,&arrivedJobs,time);
  
  int firstRun = 0;
  int completion = 0;
  while (!arrivedJobs.empty()) {
    Process oldP = arrivedJobs.top();
    firstRun = time;
    time += oldP.duration;
    completion = time;
    Process newP = {oldP.arrival,firstRun,oldP.duration,completion, 0, 0, oldP.id};
    complete.push_back(newP);
    arrivedJobs.pop();
    updateArrivedJobs(&unarrivedJobs,&arrivedJobs,time);
  }

  return complete;
}

list<Process> sjf(pqueue_arrival workload) {
  list<Process> complete;
  int time = 0;

  pqueue_arrival unarrivedJobs = workload;
  pqueue_duration arrivedJobs;
  updateArrivedJobs(&unarrivedJobs,&arrivedJobs,time);
  
  int firstRun = 0;
  int completion = 0;
  while (!arrivedJobs.empty()) {
    Process oldP = arrivedJobs.top();
    firstRun = time;
    time += oldP.duration;
    completion = time;
    Process newP = {oldP.arrival,firstRun,oldP.duration,completion, 0, 0, oldP.id};
    complete.push_back(newP);
    arrivedJobs.pop();
    updateArrivedJobs(&unarrivedJobs,&arrivedJobs,time);
  }

  return complete;
}

list<Process> stcf(pqueue_arrival workload) {
  list<Process> complete;
  int time = 0;

  pqueue_arrival unarrivedJobs = workload;
  pqueue_duration arrivedJobs;
  updateArrivedJobs(&unarrivedJobs,&arrivedJobs,time);
  
  while (!arrivedJobs.empty()) {
    Process oldP = arrivedJobs.top();
    arrivedJobs.pop();
    if (oldP.first_run == -1) {
      oldP.first_run = time;
    }
    time += 1;
    oldP.duration--;
    //TODO handle potential error with duration no longer reflecting properly
    if (oldP.duration == 0) {
      oldP.completion = time;
      Process newP = {oldP.arrival,oldP.first_run,oldP.duration,oldP.completion, 0, 0, oldP.id};
      complete.push_back(newP);
    } else {
      arrivedJobs.push(oldP);
    }
    updateArrivedJobs(&unarrivedJobs,&arrivedJobs,time);
  }

  return complete;
}

void updateArrivedJobs(pqueue_arrival *unarrivedJobs, queue<Process> *arrivedJobs, int time) {
  while (!(*unarrivedJobs).empty()) {
    Process p = (*unarrivedJobs).top();
    if (p.arrival <= time) {
      (*arrivedJobs).push(p);
    } else {
      break;
    }
    (*unarrivedJobs).pop();
  }
}

void updateArrivedJobs(pqueue_arrival *unarrivedJobs, queue<Process> *arrivedJobs, int time, int* i) {
  while (!(*unarrivedJobs).empty()) {
    Process p = (*unarrivedJobs).top();
    if (p.arrival <= time) {
      (*arrivedJobs).push(p);
      *i = 0;
    } else {
      break;
    }
    (*unarrivedJobs).pop();
  }
}

void updateSnapShots(int levels, queue<Process> queues[], queue<int> snapShots[][8], int time) { //second size in multiDim array required
  for (int i = 0; i < levels; i++) {
    queue<Process> temp;
    while (!queues[i].empty()) {
      Process proc = queues[i].front();
      queues[i].pop();
      snapShots[time][i].push(proc.id);
      temp.push(proc);
    }
    queues[i] = temp;
  }
}

void exportSnapShots(queue<int> snapShots[][8], int levels, int time, string order) {
  ofstream output;
  output.open ("outfile.txt");

  output << "Time          ";
  int digits = 0;
  for (int i = 0; i < time; i++) {
    output << "|";
    output << i;
    if (i == 10 || i == 100 || i == 1000) {
      digits++;
    }
    for (int j = digits; j < id - 1; j++) {
      output << " ";
    }
  }
  output << endl;
  output << endl;

  for (int i = 0; i < levels; i++) {
    output << "In level " << i << " ";
    for (int j = 0; j < time; j++) {
      int printed = 0;
      while (!snapShots[j][i].empty()) {
        int elem = snapShots[j][i].front();
        output << elem;
        snapShots[j][i].pop();
        printed++;
      }
      for (int i = printed; i < id; i++) {
        output << " "; 
      }
      output << "|";
    }
    output << endl;
  }

  output << endl;
  output << "ID of run file";
  for (int i = 0; i < (int)order.length(); i++) {
    output << "|";
    output << order.at(i);
    for (int j = 1; j < id; j++) {
      output << " ";
    }
  }
  output << endl;

  output.close();
}

void moveAllToTop(int levels, queue<Process> queues[]) {
  for (int i = 1; i < levels; i++) {
    while (!queues[i].empty()) {
      Process proc = queues[i].front();
      queues[i].pop();
      proc.completedTimeInQueue = 0;
      queues[0].push(proc);
    }
  } 
}


string order = "";
list<Process> mlfq(pqueue_arrival workload, int levels, int timeSlice, int boost) {
  queue<Process> queues[levels];
  for (int i = 0; i < levels; i++) {
    queues[i] = queue<Process>();
  }
  
  list<Process> complete;
  int time = 0;

  pqueue_arrival unarrivedJobs = workload;
  queue<int> snapShots[200][8]; //levels capped to 8
  updateArrivedJobs(&unarrivedJobs,&queues[0],time);
  
  int i = 0;
  while (i < levels) {
    if (queues[i].empty()) {i++; continue;}
    Process oldP = queues[i].front();
    queues[i].pop();
    if (oldP.first_run == -1) {
      oldP.first_run = time;
    }
    time++;
    oldP.completedTimeInQueue++;
    oldP.completedTimeOverall++;
    if (oldP.duration == oldP.completedTimeOverall) {
      oldP.completion = time;
      Process newP = {oldP.arrival,oldP.first_run,oldP.duration,oldP.completion,oldP.completedTimeInQueue,oldP.completedTimeOverall, oldP.id};
      complete.push_back(newP);
    } else {
      if ((oldP.completedTimeInQueue == timeSlice) && (i != levels-1)) {
        oldP.completedTimeInQueue = 0;
        queues[i+1].push(oldP);
      } else {
        queues[i].push(oldP);
      }
    }
    
    updateArrivedJobs(&unarrivedJobs,&queues[0],time, &i);
    if ((time % boost) == 0) {
      moveAllToTop(levels, queues);
      i = 0;
    }

    updateSnapShots(levels, queues, snapShots, time);
    order.append(to_string(oldP.id));
  }

  exportSnapShots(snapShots, levels, time, order);
  return complete;
}

list<Process> rr(pqueue_arrival workload) {
  list<Process> complete;
  int time = 0;

  pqueue_arrival unarrivedJobs = workload;
  queue<Process> arrivedJobs;
  updateArrivedJobs(&unarrivedJobs,&arrivedJobs,time);
  
  while (!arrivedJobs.empty()) {
    Process oldP = arrivedJobs.front();
    arrivedJobs.pop();
    if (oldP.first_run == -1) {
      oldP.first_run = time;
    }
    time += 1;
    oldP.duration--;
    if (oldP.duration == 0) {
      oldP.completion = time;
      Process newP = {oldP.arrival,oldP.first_run,oldP.duration,oldP.completion, 0, 0, oldP.id};
      complete.push_back(newP);
    } else {
      arrivedJobs.push(oldP);
    }
    updateArrivedJobs(&unarrivedJobs,&arrivedJobs,time);
  }

  return complete;
}

float avg_turnaround(list<Process> processes) {
  float totalTurnaround = 0;
  int numProcesses = 0;
  list<Process> xs = processes;
  
  while (!xs.empty()) {
    Process p = xs.front();
    int pTurnaround = p.completion - p.arrival;
    totalTurnaround += pTurnaround;
    numProcesses += 1;
    xs.pop_front();
  }

  if (numProcesses == 0) { return 0; }
  float avgTurnaround = totalTurnaround / numProcesses;
  return avgTurnaround;
}

float avg_response(list<Process> processes) {
  float totalResponse = 0;
  int numProcesses = 0;
  list<Process> xs = processes;
  
  while (!xs.empty()) {
    Process p = xs.front();
    int pResponse = p.first_run - p.arrival;
    totalResponse += pResponse;
    numProcesses += 1;
    xs.pop_front();
  }

  if (numProcesses == 0) { return 0; }
  float avgResponse = totalResponse / numProcesses;
  return avgResponse;
}

void show_metrics(list<Process> processes) {
  float avg_t = avg_turnaround(processes);
  float avg_r = avg_response(processes);
  show_processes(processes);
  cout << '\n';
  cout << "Average Turnaround Time: " << avg_t << endl;
  cout << "Average Response Time:   " << avg_r << endl;
}
