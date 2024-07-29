#ifndef HEADER_H
#define HEADER_H

#define packageName "com.unity.test"

#define hookInput(ret, func, ...) \
    ret (*orig##func)(__VA_ARGS__); \
    ret my##func(__VA_ARGS__)

#endif