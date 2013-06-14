/*
    Copyright (c) 2013 250bpm s.r.o.

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom
    the Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
    IN THE SOFTWARE.
*/

#ifndef NN_PRIOLIST_INCLUDED
#define NN_PRIOLIST_INCLUDED

#include "../protocol.h"

#include "list.h"

/*  Prioritised list of pipes. */

#define NN_PRIOLIST_SLOTS 16

struct nn_priolist_data {
    struct nn_pipe *pipe;
    int priority;
    struct nn_list_item item;
};

struct nn_priolist_slot {
    struct nn_list pipes;
    struct nn_priolist_data *current;
};

struct nn_priolist {
    struct nn_priolist_slot slots [NN_PRIOLIST_SLOTS];
    int current;
};

void nn_priolist_init (struct nn_priolist *self);
void nn_priolist_term (struct nn_priolist *self);
void nn_priolist_add (struct nn_priolist *self, struct nn_pipe *pipe,
    struct nn_priolist_data *data, int priority);
void nn_priolist_rm (struct nn_priolist *self, struct nn_pipe *pipe,
    struct nn_priolist_data *data);
void nn_priolist_activate (struct nn_priolist *self, struct nn_pipe *pipe,
    struct nn_priolist_data *data);
int nn_priolist_is_active (struct nn_priolist *self);
struct nn_pipe *nn_priolist_getpipe (struct nn_priolist *self);
void nn_priolist_advance (struct nn_priolist *self, int release);

#endif
