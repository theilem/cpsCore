# cpsCore

## Aggregation

Aggregation defines how objects are aggregated together and contains all the aggregation logic.

Aggregation is handled as follows:
- Inside AggregatabeObject, checkIsSet<...>() returns a boolean true if all the classes defined in the <...> are set. This funcion supports checking multiple classes in 1 function call. The classes are then resolved at compile time via iterative calls to checkIsSetImpl<Class Object>(), which checks if a single class is aggregated.
    
## Configuration
Configuration defines how configurable objects are made and contains all the configuration logic

## Synchronization
Synchronization defines all the runnable objects and contains all the synchronization and run stage logic

## Utilities
Utilities contain all helper classes which can be Aggregatable, Configurable, and Runnable