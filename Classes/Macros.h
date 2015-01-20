#ifndef MACROS_H
#define MACROS_H

#define DELETE_COPY_AND_MOVE(TypeName) \
    TypeName (const TypeName &) = delete; \
    TypeName (const TypeName &&) = delete; \
    TypeName & operator= (const TypeName &) = delete; \
    TypeName & operator= (const TypeName &&) = delete;

#endif // MACROS_H
    