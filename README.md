# ext_atomic
A class to extend the C++ atomic interface to non-trivially contructable types

Simply use ext_atomc<T> instead of std::atomic<T>

Trivially contructable types will use std::atomic, but others will fall back to a mutex implementation.

Not yet tested.
