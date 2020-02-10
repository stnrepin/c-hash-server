#ifndef MAIN_H
#define MAIN_H

#if DEBUG
#define IF_DEBUG(x) x
#else
#define IF_DEBUG(x)
#endif

int main(int argc, char **argv);

#endif // !MAIN_H
