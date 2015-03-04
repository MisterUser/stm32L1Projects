/* * * * * * * * * * *
 * deadBeats_vector.c
 * 
 * Greg Knoll 2014
 * * * * * * * * * * */

#include <stdio.h>
#include <stdlib.h>
#include "deadBeats_vector.h"

void vector_init(Vector *vector) {
  // initialize size and capacity
  vector->size = 0;
  vector->capacity = VECTOR_INITIAL_CAPACITY;

  // allocate memory for vector->data
  vector->data = malloc(sizeof(char) * vector->capacity);
}

void vector_push(Vector *vector, int value) {
  // make sure there's room to expand into
  vector_double_capacity_if_full(vector);

  // append the value and increment vector->size
  vector->data[vector->size++] = value;
}

int vector_get(Vector *vector, int index) {
  if (index >= vector->size || index < 0) {
    //printf("Index %d out of bounds for vector of size %d\n", index, vector->size);
    //exit(1);
  }
  return vector->data[index];
}

void vector_set(Vector *vector, int index, int value) {
  // zero fill the vector up to the desired index
  while (index >= vector->size) {
    vector_push(vector, 0);
  }

  // set the value at the desired index
  vector->data[index] = value;
}

void vector_double_capacity_if_full(Vector *vector) {
  if (vector->size >= vector->capacity) {
    vector->capacity += 10;
    vector->data = realloc(vector->data, sizeof(char) * vector->capacity);
  }
}

void vector_free(Vector *vector) {
  free(vector->data);
}
