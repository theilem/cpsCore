//
// Created by mirco on 04.01.20.
//

#ifndef CPSCORE_SPLIT_H
#define CPSCORE_SPLIT_H

#include <map>

class BinaryFromArchive;

class BinaryToArchive;

class FileFromArchive;

class FileToArchive;

namespace dp
{

/**
 * @brief General split functionality for loading
 * @param ar From archive
 * @param val any value to be loaded differently than it is stored
 */
template<class Type>
void
split(BinaryFromArchive& ar, Type& val);

/**
 * @brief General split functionality for storing
 * @param ar To archive
 * @param val any value to be stored differently than it is loaded
 */
template<class Type>
void
split(BinaryToArchive& ar, Type& val);

/**
 * @brief General split functionality for loading
 * @param ar From archive
 * @param val any value to be loaded differently than it is stored
 */
template<class Type>
void
split(FileFromArchive& ar, Type& val);

/**
 * @brief General split functionality for storing
 * @param ar To archive
 * @param val any value to be stored differently than it is loaded
 */
template<class Type>
void
split(FileToArchive& ar, Type& val);

void
split(BinaryFromArchive& ar, char* val, std::size_t size);

void
split(BinaryToArchive& ar, char* val, std::size_t size);

void
split(FileFromArchive& ar, char* val, std::size_t size);

void
split(FileToArchive& ar, char* val, std::size_t size);

}

#endif //CPSCORE_SPLIT_H
