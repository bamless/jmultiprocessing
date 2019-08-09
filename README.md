# jmultiprocessing
A multiprocessing library for the [J*](https://github.com/bamless/jstar) language.
The library is far from complete, and to be really useful some IPC (Inter-process comunication) mechanisms must be implemented.
An object serialization mechanism could be implemented as well, in order to achieve a more transparent IPC (for example, by serializing an object and then sending it to the other process via a pipe) 

## Compatibility
The library uses the fork systemcall internally to spawn new processes that share the same memory layout, an for this reason it's only compatible with POSIX systems. Since Windows does't offer a fork systemcall among its APIs, to achieve a port one must be able to serialize objects in order to pass (at least) the start function along with the arguments to the newly spawned process.

## Compilation and usage
For now the project uses a minimal Makefile to compile the shared library, a more general build solution will be used when Winscock support is added. The DESTDIR environment variable is used during the install target to copy the multiprocessing.jsr and libmultiprocessing.so files to a suitable destination. This should be changed to a path present in the JSTARPATH environment variable, so that the interpreter can load the library (the default is /usr/local/lib/jstar).
