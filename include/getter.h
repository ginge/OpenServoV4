/* getter.h
 *
 * This is a class masquerading as a "library" in the standard Wiring
 * fashion.
 */

#ifndef _GETTER_H_
#define _GETTER_H_

class GetterClass {
 public:
    GetterClass();
    void begin();
    int getValue();
 private:
    int value;
};

extern GetterClass Getter;

#endif
