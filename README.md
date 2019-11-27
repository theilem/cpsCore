# cpsCore

## Dependencies

This project has some dependencies for various functionality. The dependencies are as follows:

```
Prerequisites:
- Eigen3 [header only] (http://eigen.tuxfamily.org)
- Boost [Component: system] (https://www.boost.org/)

Submodules:
- cpp_redis (https://github.com/cpp-redis/cpp_redis)
- Catch2 (https://github.com/catchorg/Catch2)
```

## Cloning and Compiling


```shell script
git clone --recurse-submodules https://github.com/theilem/cpsCore.git 

cd cpsCore
mkdir -p bld/release && cd bld/release

cmake -DCMAKE_BUILD_TYPE=Release ../../
make
make install
```



## Components

### Aggregation

Aggregation defines how objects are aggregated together and contains all the aggregation logic. Check the [aggregation tests](https://github.com/theilem/cpsCore/blob/master/tests/Aggregation/AggregatableObjectTest.cpp) for an example of how Aggregation is done.
    
### Configuration
Configuration defines how configurable objects are made and contains all the configuration logic. Check the [configuration tests](https://github.com/theilem/cpsCore/blob/master/tests/Configuration/ConfigurableObjectTest.cpp) for an example of how Configuration is done.

### Synchronization
Synchronization defines all the runnable objects and contains all the synchronization and run stage logic. Check the [synchronization tests](https://github.com/theilem/cpsCore/blob/master/tests/Synchronization/RunnerTest.cpp) for an example of how Synchronization is done.

### Logger
This contains the logic for the CPSLogger, the logger used in this project.

### Utilities
Utilities contain all helper classes which can be Aggregatable, Configurable, and Runnable. This is a catchall directory for components
