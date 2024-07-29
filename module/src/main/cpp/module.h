#ifndef MODULE_H
#define MODULE_H

#include <stddef.h>

void moduleMain(const char *appDataDir, void *data, size_t length);

void logger(const char *myLogText);

void guiSetup();

#endif