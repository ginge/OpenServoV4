/* randomizer.h
 *
 * A useless front-end to the standard C random number interface. This
 * is basically the same thing as the wirish functions randomSeed()
 * and random().
 */

#ifndef _RANDOMIZER_H_
#define _RANDOMIZER_H_

/* Conditional extern "C" so we're safe to call from C++ files */
#ifdef __cplusplus
extern "C" {
#endif

void randomizer_init(int value);
long randomizer_get_value(void);

#ifdef __cplusplus
}
#endif

#endif  /* _RANDOMIZER_H_ */
