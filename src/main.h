#ifndef MAIN_H
#define MAIN_H

#ifdef DEBUG
#define IF_DEBUG(x) x
#else
#define IF_DEBUG(x)
#endif

#define GOST_DIGEST_LENGTH 32

int main(int argc, char **argv);

#endif // !MAIN_H
