#include <scheduling.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <list>
#include <queue>
#include <string>

using namespace std;

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
    Process work {stoi(arrival), -1, stoi(duration), -1};
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
    Process newP = {oldP.arrival,firstRun,oldP.duration,completion};
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
    Process newP = {oldP.arrival,firstRun,oldP.duration,completion};
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
      Process newP = {oldP.arrival,oldP.first_run,oldP.duration,oldP.completion};
      complete.push_back(newP);
    } else {
      arrivedJobs.push(oldP);
    }
    updateArrivedJobs(&unarrivedJobs,&arrivedJobs,time);
  }

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
    cout << oldP.arrival << " ";
    arrivedJobs.pop();
    if (oldP.first_run == -1) {
      oldP.first_run = time;
    }
    time += 1;
    oldP.duration--;
    if (oldP.duration == 0) {
      oldP.completion = time;
      Process newP = {oldP.arrival,oldP.first_run,oldP.duration,oldP.completion};
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
