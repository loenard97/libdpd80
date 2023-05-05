#ifndef CALLBACKS_H
#define CALLBACKS_H

int transfer_callback(uint16_t* data, int ndata, int dataloss, void* userdata);
int callback_counter(uint16_t* data, int ndata, int dataloss, void* userdata);

#endif
