#include "Arduino.h"

#include "StateQueue.h"



StateQueue::StateQueue() : size(0) {}

bool StateQueue::enqueue(states newState) {
  if (size >= 10) {
    return false;
  }

  queue[size] = newState;
  Serial.print("Queuing: ");  Serial.print(newState);  Serial.print(", spot ");  Serial.println(size);
  size++;
  return true;
}

bool StateQueue::dequeue(states &out) {
  if (size == 0) {
    return false;
  }
  out = queue[0];

  for (short i = 0; (i < size - 1) && (i + 1 < 10);i++) {
    queue[i] = queue[i + 1];
  }
  size--;
  return true;
}

bool StateQueue::isEmpty() {
  return size <= 0;
}