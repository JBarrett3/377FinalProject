#include <scheduling.h>
#include <iostream>
#include <string>
#include <limits>
float Inf = numeric_limits<float>::infinity(); 

using namespace std;

int main(int argc, char* argv[]) {
  if (argc != 3 && argc != 7) {
    cout << "usage: [fifo|sjf|stcf|rr] workload_file" << endl;
    cout << "OR [mlfq] workload_file levels timeSlice boost interactive" <<endl;
    cout << "OR [modMlfq] workload_file levels timeSlice boost interactive" <<endl;
    exit(1);
  }

  string algorithm = argv[1];
  string workload_file = argv[2];

  pqueue_arrival workload = read_workload(workload_file);

  if (algorithm == "fifo") {
    show_metrics(fifo(workload));
  } else if (algorithm == "sjf") {
    show_metrics(sjf(workload));
  } else if (algorithm == "stcf") {
    show_metrics(stcf(workload));
  } else if (algorithm == "rr") {
    show_metrics(rr(workload));
  } else if (algorithm == "mlfq") {
    if (argc != 7) {
      cout << "usage: [fifo|sjf|stcf|rr] workload_file" << endl;
      cout << "OR [mlfq|modMlfq] workload_file levels timeSlice boost interactive" <<endl;
      exit(1);
    }
    int levels = atoi(argv[3]);
    if (levels > 8) {
      cout << "Only supports up to 8 levels" << endl;
      exit(1);
    }
    int timeSlice = atoi(argv[4]);
    int boost = atoi(argv[5]);
    int interactive = atoi(argv[6]);
    if (interactive != 0 && interactive != 1) {
      cout << "Either interactive (1) or not (0)" << endl;
      exit(1);
    }
    show_metrics(mlfq(workload, levels, timeSlice, boost, interactive));
  } else if (algorithm == "modMlfq") {
    if (argc != 7) {
      cout << "usage: [fifo|sjf|stcf|rr] workload_file" << endl;
      cout << "OR [mlfq|modMlfq] workload_file levels timeSlice boost interactive" <<endl;
      exit(1);
    }
    int levels = atoi(argv[3]);
    if (levels > 8) {
      cout << "Only supports up to 8 levels" << endl;
      exit(1);
     }
    int timeSlice = atoi(argv[4]);
    int boost = atoi(argv[5]);
    int interactive = atoi(argv[6]);
    if (interactive != 0 && interactive != 1) {
      cout << "Either interactive (1) or not (0)" << endl;
      exit(1);
    }
    show_metrics(modMlfq(workload, levels, timeSlice, boost, interactive));
  } else {
    cout << "Error: Unknown algorithm: " << algorithm << endl;
    cout << "usage: [fifo|sjf|stcf|rr] workload_file" << endl;
    cout << "OR [mlfq] workload_file levels timeSlice boost interactive" <<endl;
    cout << "OR [modMlfq] workload_file levels timeSlice boost interactive" <<endl;
    exit(1);
  }

  return 0;
}