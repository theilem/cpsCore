# cpsCore

## Cloning

This project uses various external projects. Make sure to clone the submodules:

`git clone https://github.com/theilem/cpsCore --recurse-submodules`

## Components

### Aggregation

Aggregation defines how objects are aggregated together and contains all the aggregation logic. Check the [aggregation tests](https://github.com/theilem/cpsCore/blob/master/tests/Aggregation/AggregatableObjectTest.cpp) for an example of how Aggregation is done.
    
### Configuration
Configuration defines how configurable objects are made and contains all the configuration logic. Check the [configuration tests](https://github.com/theilem/cpsCore/blob/master/tests/Configuration/ConfigurableObjectTest.cpp) for an example of how Configuration is done.

### Synchronization
Synchronization defines all the runnable objects and contains all the synchronization and run stage logic.

### Logger
This contains the logic for the CPSLogger, the logger used in this project.

### Utilities
Utilities contain all helper classes which can be Aggregatable, Configurable, and Runnable. This is a catchall directory for components