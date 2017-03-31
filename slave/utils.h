//
// Created by cyrbos on 3/15/17.
//

#ifndef SLAVE_UTILS_H
#define SLAVE_UTILS_H

#define CHKERR(X) \
    if ((X) < 0) \
    { \
        perror(#X); \
        exit(errno); \
    }


#endif //SLAVE_UTILS_H
