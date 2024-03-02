#ifndef _H_STATE_QUEUE
#define _H_STATE_QUEUE

enum states {
  ATTRACT,
  PENDING,
  FULL_ACTIVE,
  DONE,
};

class StateQueue {
  public:
    StateQueue();
    bool enqueue(states newState);
    bool dequeue(states &out);
    bool isEmpty();

  private:
    states queue[10];
    short int size;
};

#endif