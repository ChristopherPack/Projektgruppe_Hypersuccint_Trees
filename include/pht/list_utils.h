#ifndef PROJECTGROUP_HYPERSUCCINCT_TREES_LISTUTILS_H_
#define PROJECTGROUP_HYPERSUCCINCT_TREES_LISTUTILS_H_

#include <vector>

namespace pht {
    /**
     * Provides list utility functions.
     * 
     * This class provides commonly used functionality for the std::vector class, 
     * which is not implemented or very verbose in the standard library. 
     * 
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
        template<class T> inline static void addAll(std::vector<T>& vector, const std::vector<T>& values) {
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
         */
        template<class T> inline static void distinct(const std::vector<T>& vector1, std::vector<T>& vector2) {
            if(vector1.empty()) {
                return;
            }
            for(T t : vector1) {
                if(!ListUtils::contains(vector2,t)) {
                    vector2.push_back(t);
                }
            }
        }

        /**
         * Returns true if the vector contains the given element. 
         * 
         * @param[in] vector The vector which could contain the element. 
         * @param[in] value The searched element. 
         * @tparam T The type of data stored in the vector. 
         * @return True if the vector contains the value.
         */
        template<class T> inline static bool contains(const std::vector<T>& vector, const T value) {
            if(vector.empty()) {
                return false;
            }
            return std::find(vector.begin(), vector.end(), value) != vector.end();
        }

        /**
         * Sortes the given vector with the given filter function. 
         * 
         * @param[in] vector The vector which should be sorted. 
         * @param[in] predicate The comparator function. 
         * @tparam T The type of data stored in the vector. 
         * @tparam F The type of the comparator function. 
         */
        template<class T, class F> inline static void sort(std::vector<T>& vector, const F comparator) {
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
         * @tparam F The type of the filter function. 
         */
        template<class T, class F> inline static void filter(std::vector<T>& vector, const F predicate) {
            if(vector.empty()) {
                return;
            }
            vector.erase(std::remove_if(vector.begin(), vector.end(), [predicate](T x){ return !predicate(x); }), vector.end());
        }

        /**
         * Maps all elements in vector1 into vector2 with the help of the given function. 
         * 
         * @param[in] vector1 The vector which should be mapped. 
         * @param[in] vector2 The vector to m ap into. 
         * @param[in] mapper The mapping function which converts an element from T1 into T2. 
         * @tparam T1 The type of data stored in the vector. 
         * @tparam T2 The new type of the data after mapping. 
         * @tparam F The type of the mapping function. 
         * @return A new vector with the mapped values. 
         */
        template<class T1, class T2, class F> inline static void map(const std::vector<T1>& vector1, std::vector<T2>& vector2, const F mapper) {
            if(vector1.empty()) {
                return;
            }
            vector2.reserve(vector1.size());
            std::transform(vector1.begin(), vector1.end(), std::back_inserter(vector2), mapper);
        }

        /**
         * Combines all elements in the vector the help of the given function and the given initialization value. 
         * 
         * @param[in] vector The vector which should be mapped. 
         * @param[in] folder The fold function which combines the accumulated value with a new one. 
         * @tparam T The type of data stored in the vector. 
         * @tparam F The type of the fold function. 
         */
        template<class T, class F> inline static T fold(const std::vector<T>& vector, const T value, const F folder) {
            if(vector.empty()) {
                return value;
            }
            T result = value;
            std::for_each(vector.begin(), vector.end(), [&result, folder](T x) mutable { result = folder(result, x); });
            return result;
        }

        /**
         * Executes the given function for all elements in the vector. 
         * 
         * @param[in] vector The vector which should be mapped. 
         * @param[in] function The function to execute. 
         * @tparam T The type of data stored in the vectors. 
         * @tparam F The type of the function. 
         */
        template<class T, class F> inline static void forEach(const std::vector<T>& vector, const F function) {
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
        template<class T> inline static int32_t indexOf(const std::vector<T>& vector, const T value) {
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
         * Removes the given value from the given vector.
         * 
         * @param[in] vector The vector to remove from. 
         * @param[in] value The vector with the values to remove. 
         * @tparam T The type of data stored in the vectors. 
         */
        template<class T> inline static void erase(std::vector<T>& vector, const T value) {
            if(vector.empty()) {
                return;
            }
            vector.erase(std::remove(vector.begin(), vector.end(), value), vector.end());
        }

        /**
         * Removes all the values of the second vector from the first vector.
         * 
         * @param[in] vector The vector to remove from. 
         * @param[in] values The vector with the values to remove. 
         * @tparam T The type of data stored in the vectors. 
         */
        template<class T> inline static void eraseAll(std::vector<T>& vector, const std::vector<T>& values) {
            if(vector.empty() || values.empty()) {
                return;
            }
            vector.erase(std::remove_if(vector.begin(), vector.end(), [values](T value){ return std::find(values.begin(), values.end(), value) != values.end(); }), vector.end());
        }

        /**
         *
         * @tparam T
         * @param vector
         * @return
         */
        template<class T> inline static std::vector<T> reverse(std::vector<T>& vector) {
            std::vector<T> res;
            for(auto i = vector.rbegin(); i != vector.rend(); ++i ) {
                res.push_back(*i);
            }
            return res;
        }
    };
}

#endif//PROJECTGROUP_HYPERSUCCINCT_TREES_LISTUTILS_H_