#ifndef PROJECTGROUP_HYPERSUCCINCT_TREES_LISTUTILS
#define PROJECTGROUP_HYPERSUCCINCT_TREES_LISTUTILS

#include <algorithm>
#include <functional>
#include <vector>

namespace pht {
    /**
     * Provides list utility functions.
     * 
     * This class provides commonly used functionality for the std::vector class, 
     * which is not implemented or very verbose in the standard library. 
     */
    class ListUtils {
    public:
        /**
         * Appends all the values of the second vector to the first vector. 
         * 
         * @param[in] vector The vector to append to. 
         * @param[in] values The vector with the values to append. 
         * @tparam T The type of data stored in the vectors. 
         */
        template<class T> static void combine(std::vector<T>& vector, const std::vector<T>& values) {
            if(values.empty()) {
                return;
            }
            vector.insert(vector.end(), values.begin(), values.end());
        }

        /**
         * Appends all the values of the second vector to the first vector. 
         * 
         * @param[in] vector The vector to append to. 
         * @param[in] values The vector with the values to append. 
         * @tparam T The type of data stored in the vectors. 
         * @overload
         */
        template<class T> static void combine(std::vector<T>& vector, const std::initializer_list<T> values) {
            if(values.size() == 0) {
                return;
            }
            vector.insert(vector.end(), values.begin(), values.end());
        }
        
        /**
         * Combines the two vectors into a new one. 
         * 
         * @param[in] vector One vector to combine. 
         * @param[in] values The second vector to combine. 
         * @tparam T The type of data stored in the vectors. 
         * @return The combined values. 
         */
        template<class T> static std::vector<T> combined(const std::vector<T>& vectorA, const std::vector<T>& vectorB) {
            if(vectorB.empty()) {
                return std::vector<T>(vectorA);
            }
            std::vector<T> result = vectorA;
            result.insert(result.end(), vectorB.begin(), vectorB.end());
            return result;
        }
        
        /**
         * Combines the two vectors into a new one. 
         * 
         * @param[in] vector One vector to combine. 
         * @param[in] values The second vector to combine. 
         * @tparam T The type of data stored in the vectors. 
         * @return The combined values. 
         * @overload
         */
        template<class T> static std::vector<T> combined(const std::vector<T>& vectorA, const std::initializer_list<T> vectorB) {
            if(vectorB.size() == 0) {
                return std::vector<T>();
            }
            std::vector<T> result = vectorA;
            result.insert(vectorA.end(), vectorB.begin(), vectorB.end());
            return result;
        }

        /**
         * Returns a list with the values which are only once in the given vector. 
         *
         * @param[in] vector The vector to get the distinct values from. 
         * @tparam T The type of data stored in the vectors.
         * @return The distinct values. 
         */
        template<class T> static std::vector<T> distincted(const std::vector<T>& vector) {
            if(vector.empty()) {
                return std::vector<T>(vector);
            }
            std::vector<T> result;
            for(T value : vector) {
                if(std::find(result.begin(), result.end(), value) == result.end()) {
                    result.push_back(value);
                }
            }
            return result;
        }

        /**
         * Removes all duplicates in the given vector. 
         *
         * @param[in] vector The vector to keep only the distinct values in. 
         * @tparam T The type of data stored in the vectors. 
         */
        template<class T> static void distinct(std::vector<T>& vector) {
            if(vector.empty()) {
                return;
            }
            typename std::vector<T>::iterator iterator = vector.begin();
            while(iterator != vector.end()) {
                if(std::find(vector.begin(), iterator, *iterator) != iterator) { 
                    iterator = vector.erase(iterator);
                } else {
                    iterator++;
                }
            }
        }

        /**
         * Returns true if the vector contains any of the given elements. 
         * 
         * @param[in] vector The vector which could contain the element. 
         * @param[in] values The searched elements. 
         * @tparam T The type of data stored in the vector. 
         * @return True if the vector contains any of the values or values is empty. 
         * @overload
         */
        template<class T> static bool containsAny(const std::vector<T>& vector, const std::vector<T>& values) {
            if(values.empty()) {
                return true;
            }
            if(vector.empty()) {
                return false;
            }
            for(T value : values) {
                if(std::find(vector.begin(), vector.end(), value) != vector.end()) {
                    return true;
                }
            }
            return false;
        }

        /**
         * Returns true if the vector contains all the given elements. 
         * 
         * @param[in] vector The vector which could contain the element. 
         * @param[in] values The searched elements. 
         * @tparam T The type of data stored in the vector. 
         * @return True if the vector contains all the values or values is empty. 
         * @overload
         */
        template<class T> static bool containsAll(const std::vector<T>& vector, const std::vector<T>& values) {
            if(values.empty()) {
                return true;
            }
            if(vector.empty()) {
                return false;
            }
            for(T value : values) {
                if(std::find(vector.begin(), vector.end(), value) == vector.end()) {
                    return false;
                }
            }
            return true;
        }

        /**
         * Returns true if the vector contains any of the given elements. 
         * 
         * @param[in] vector The vector which could contain the element. 
         * @param[in] values The searched elements. 
         * @tparam T The type of data stored in the vector. 
         * @return True if the vector contains any of the values or values is empty. 
         * @overload
         */
        template<class T> static bool containsAny(const std::vector<T>& vector, const std::initializer_list<T> values) {
            if(values.size() == 0) {
                return true;
            }
            if(vector.empty()) {
                return false;
            }
            for(T value : values) {
                if(std::find(vector.begin(), vector.end(), value) != vector.end()) {
                    return true;
                }
            }
            return false;
        }

        /**
         * Returns true if the vector contains all of the given elements. 
         * 
         * @param[in] vector The vector which could contain the element. 
         * @param[in] values The searched elements. 
         * @tparam T The type of data stored in the vector. 
         * @return True if the vector contains all the values or values is empty. 
         * @overload
         */
        template<class T> static bool containsAll(const std::vector<T>& vector, const std::initializer_list<T> values) {
            if(values.size() == 0) {
                return true;
            }
            if(vector.empty()) {
                return false;
            }
            for(T value : values) {
                if(std::find(vector.begin(), vector.end(), value) == vector.end()) {
                    return false;
                }
            }
            return true;
        }

        /**
         * Sortes a copy of the given vector with the given filter function. 
         * 
         * @param[in] vector The vector which should be sorted. 
         * @param[in] comparator The comparator function. 
         * @tparam T The type of data stored in the vector. 
         * @return a sorted version of the vector. 
         */
        template<class T> static std::vector<T> sorted(const std::vector<T>& vector, std::function<bool(T,T)> comparator) {
            if(vector.empty()) {
                return std::vector<T>(vector);
            }
            std::vector<T> result = vector;
            std::sort(result.begin(), result.end(), comparator);
            return result;
        }

        /**
         * Sortes the given vector with the given filter function. 
         * 
         * @param[in] vector The vector which should be sorted. 
         * @param[in] comparator The comparator function. 
         * @tparam T The type of data stored in the vector. 
         */
        template<class T> static void sort(std::vector<T>& vector, std::function<bool(T,T)> comparator) {
            if(vector.empty()) {
                return;
            }
            std::sort(vector.begin(), vector.end(), comparator);
        }

        /**
         * Filters out all elements in the vector for which the given function is false. 
         * 
         * @param[in] vector The vector which should be filtered. 
         * @param[in] predicate The filter function. 
         * @tparam T The type of data stored in the vector. 
         */
        template<class T> static void filter(std::vector<T>& vector, std::function<bool(T)> predicate) {
            if(vector.empty()) {
                return;
            }
            vector.erase(std::remove_if(vector.begin(), vector.end(), [predicate](T x){ return !predicate(x); }), vector.end());
        }

        /**
         * Filters out all elements from a copy of the vector for which the given function is false. 
         * 
         * @param[in] vector The vector which should be filtered. 
         * @param[in] predicate The filter function. 
         * @tparam T The type of data stored in the vector. 
         * @return A filtered copy of the vector. 
         */
        template<class T> static std::vector<T> filtered(const std::vector<T>& vector, std::function<bool(T)> predicate) {
            if(vector.empty()) {
                return std::vector<T>(vector);
            }
            std::vector<T> result = vector;
            result.erase(std::remove_if(result.begin(), result.end(), [predicate](T x){ return !predicate(x); }), result.end());
            return result;
        }

        /**
         * Maps all elements in vector1 into vector2 with the help of the given function. 
         * 
         * @param[in] vector The vector which should be mapped. 
         * @param[in] mapper The mapping function which converts an element from T1 into T2. 
         * @tparam T1 The type of data stored in the vector. 
         * @tparam T2 The new type of the data after mapping. 
         * @return A new vector with the mapped values. 
         */
        template<class T1, class T2> static std::vector<T2> mapped(const std::vector<T1>& vector, std::function<T2(T1)> mapper) {
            std::cout << "MAPPED " << std::endl;
            if(vector.empty()) {
                return std::vector<T2>();
            }
            std::vector<T2> result;
            result.reserve(vector.size());
            std::transform(vector.begin(), vector.end(), std::back_inserter(result), mapper);
            return result;
        }

        /**
         * Maps all elements in vector1 into vector2 with the help of the given function. 
         * 
         * @param[in] vector The vector which should be mapped. 
         * @param[out] vector The vector to map into. 
         * @param[in] mapper The mapping function which converts an element from T1 into T2. 
         * @tparam T1 The type of data stored in the vector. 
         * @tparam T2 The new type of the data after mapping. 
         */
        template<class T1, class T2> static void map(const std::vector<T1>& vector, std::vector<T2>& output, std::function<T2(T1)> mapper) {
            if(vector.empty()) {
                return;
            }
            std::for_each(vector.begin(), vector.end(), [&output,mapper](T1 x){output.push_back(mapper(x));});
        }

        /**
         * Combines all elements in the vector the help of the given function and the given initialization value. 
         * 
         * @param[in] vector The vector which should be mapped. 
         * @param[in] value The start value. 
         * @param[in] folder The fold function which combines the accumulated value with a new one. 
         * @tparam T1 The type of data stored in the vector. 
         * @tparam T2 The type of the accumulated data. 
         */
        template<class T1, class T2> static T2 fold(const std::vector<T1>& vector, const T2 value, std::function<T2(T2,T1)> folder) {
            if(vector.empty()) {
                return value;
            }
            T2 result = value;
            std::for_each(vector.begin(), vector.end(), [&result, folder](T1 x) mutable { result = folder(result, x); });
            return result;
        }

        /**
         * Executes the given function for all elements in the vector. 
         * 
         * @param[in] vector The vector which should be mapped. 
         * @param[in] function The function to execute. 
         * @tparam T The type of data stored in the vectors.
         */
        template<class T> static void foreach(const std::vector<T>& vector, std::function<void(T)> function) {
            if(vector.empty()) {
                return;
            }
            std::for_each(vector.begin(), vector.end(), function);
        }

        /**
         * Returns the index of the given element in the given vector. 
         * 
         * @param[in] vector The vector which contains the element. 
         * @param[in] value The element whose index is needed. 
         * @tparam T The type of data stored in the vectors. 
         * @return The index of the element or -1. 
         */
        template<class T> static int32_t indexOf(const std::vector<T>& vector, const T value) {
            if(vector.empty()) {
                return -1;
            }
            auto iter = std::find(vector.begin(), vector.end(), value);
            if(iter == vector.end()) {
                return -1;
            }
            return iter-vector.begin();
        }

        /**
         * Removes all the values of the second vector from the first vector.
         * 
         * @param[in] vector The vector to remove from. 
         * @param[in] values The vector with the values to remove. 
         * @tparam T The type of data stored in the vectors. 
         * @overload
         */
        template<class T> static void erase(std::vector<T>& vector, const std::vector<T>& values) {
            if(vector.empty() || values.empty()) {
                return;
            }
            vector.erase(std::remove_if(vector.begin(), vector.end(), [values](T value){ return std::find(values.begin(), values.end(), value) != values.end(); }), vector.end());
        }

        /**
         * Removes all the values of the second vector from a copy of the first vector.
         * 
         * @param[in] vector The vector to remove from. 
         * @param[in] values The vector with the values to remove. 
         * @tparam T The type of data stored in the vectors. 
         * @return A copy without the values to erase. 
         * @overload
         */
        template<class T> static std::vector<T> erased(const std::vector<T>& vector, const std::vector<T>& values) {
            if(vector.empty() || values.empty()) {
                return std::vector<T>(vector);
            }
            std::vector result = vector;
            result.erase(std::remove_if(result.begin(), result.end(), [values](T value){ return std::find(values.begin(), values.end(), value) != values.end(); }), result.end());
            return result;
        }

        /**
         * Removes all the values of the second vector from the first vector.
         * 
         * @param[in] vector The vector to remove from. 
         * @param[in] values The vector with the values to remove. 
         * @tparam T The type of data stored in the vectors. 
         * @overload
         */
        template<class T> static void erase(std::vector<T>& vector, const std::initializer_list<T> values) {
            if(vector.empty() || (values.size() == 0)) {
                return;
            }
            vector.erase(std::remove_if(vector.begin(), vector.end(), [values](T value){ return std::find(values.begin(), values.end(), value) != values.end(); }), vector.end());
        }

        /**
         * Removes all the values of the second vector from a copy of the first vector.
         * 
         * @param[in] vector The vector to remove from. 
         * @param[in] values The vector with the values to remove. 
         * @tparam T The type of data stored in the vectors. 
         * @return A copy without the values to erase. 
         * @overload
         */
        template<class T> static std::vector<T> erased(const std::vector<T>& vector, const std::initializer_list<T> values) {
            if(vector.empty() || (values.size() == 0)) {
                return std::vector<T>(vector);
            }
            std::vector result = vector;
            result.erase(std::remove_if(result.begin(), result.end(), [values](T value){ return std::find(values.begin(), values.end(), value) != values.end(); }), result.end());
            return result;
        }

        /**
         * Reverses the given list. 
         * 
         * @param vector The to reverse. 
         * @tparam T The type of data stored in the vectors. 
         */
        template<class T> static void reverse(std::vector<T>& vector) {
            if(vector.empty()) {
                return;
            }
            for(uint32_t i = 0; i < vector.size()/2; i++) {
                T tmp = vector.at(vector.size()-1-i);
                vector.at(vector.size()-1-i) = vector.at(i);
                vector.at(i) = tmp;
            }
        }

        /**
         * Reverses the given list into a new list. 
         * 
         * @param vector The to reverse. 
         * @tparam T The type of data stored in the vectors. 
         * @return The reversed list. 
         */
        template<class T> static std::vector<T> reversed(const std::vector<T>& vector) {
            if(vector.empty()) {
                return std::vector<T>(vector);
            }
            std::vector<T> result;
            std::for_each(vector.begin(), vector.end(), [&result](T value){result.insert(result.begin(), value);});
            return result;
        }

        /**
         * Replaces the given value in the given vector. 
         * 
         * @param vector The to replace the value in. 
         * @param value The value to replace. 
         * @param replacement The replacement. 
         * @tparam T The type of data stored in the vectors. 
         */
        template<class T> static void replace(std::vector<T>& vector, const T value, const T replacement) {
            if(vector.empty()) {
                return;
            }
            std::replace(vector.begin(), vector.end(), value, replacement);
        }

        /**
         * Replaces the given value in the given vector. 
         * 
         * @param vector The to replace the value in. 
         * @param value The value to replace. 
         * @param replacement The replacement. 
         * @tparam T The type of data stored in the vectors. 
         * @return A copy with the replaced value. 
         */
        template<class T> static std::vector<T> replaced(const std::vector<T>& vector, const T value, const T replacement) {
            if(vector.empty()) {
                return std::vector<T>(vector);
            }
            std::vector<T> result = vector;
            std::replace(result.begin(), result.end(), value, replacement);
            return result;
        }
    };
}

#endif//PROJECTGROUP_HYPERSUCCINCT_TREES_LISTUTILS