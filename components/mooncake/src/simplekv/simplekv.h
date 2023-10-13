/**
 * @file simplekv.h
 * @author Forairaaaaa
 * @brief 
 * @version 0.1
 * @date 2023-05-04
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once
#include <iostream>
#include <string>
#include <cstring>
#include <unordered_map>


namespace SIMPLEKV
{
    /* Structure to store data info */
    struct ValueInfo_t
    {
        // std::string key = "";
        size_t size = 0;
        void* addr = nullptr;


        /* Value convertion */
        template<typename T>
        T value()
        {
            if ((this->addr == nullptr) || (this->size < sizeof(T)))
            {
                return T();
            }
            return *(T*)this->addr;
        }

    };
    

    class SimpleKV
    {
        private:
            std::unordered_map<std::string, ValueInfo_t> _value_map;
            ValueInfo_t _ret_buffer;


        protected:
            /* Memory API */
            /* Free to override for different platforms */
            virtual void _free(void* ptr) { free(ptr); }
            virtual void* _malloc(size_t size) { return malloc(size); }
            virtual void* _memcpy(void* dest, const void* src, size_t n) { return memcpy(dest, src, n); }


        public:
            ~SimpleKV() { DeleteAll(); }


            /**
             * @brief Get the map size (number of elements)
             * 
             * @return size_t 
             */
            inline size_t Size() { return _value_map.size(); }

            /**
             * @brief Check if the passing key is pointing to a data 
             * 
             * @param key 
             * @return true 
             * @return false 
             */
            bool Exist(const std::string& key);

            /**
             * @brief Get all data's memory usage
             * 
             * @return size_t 
             */
            size_t MemoryUsage();

            /**
             * @brief Database will create a new buffer to store the passing data
             * 
             * @param key 
             * @param value 
             * @param size 
             * @return true - ok
             * @return false - already exist or 0 size
             */
            bool Add(const std::string& key, void* value, size_t size);

            /**
             * @brief Database will copy the passing data into the pointing memory
             * 
             * @param key 
             * @param value 
             * @return true - ok
             * @return false - not exist
             */
            bool Put(const std::string& key, void* value);

            /**
             * @brief Get a value info pointer that the passing key is pointing to
             * 
             * @param key 
             * @return ValueInfo_t* addr=nullptr if key not exist
             */
            ValueInfo_t* Get(const std::string& key);

            /**
             * @brief Delect the key pointing data and free the memory  
             * 
             * @param key 
             * @return true - ok
             * @return false - not exist
             */
            bool Delete(const std::string& key);

            /**
             * @brief Delect all data and free the memory 
             * 
             */
            void DeleteAll();

            

            /* Wrap for differnt types */

            /**
             * @brief Database will create a new buffer to store the passing data
             * 
             * @tparam T 
             * @param key 
             * @param value 
             * @return true - ok
             * @return false - already exist or 0 size
             */
            template<typename T>
            inline bool Add(const std::string& key, T value) { return Add(key, (void*)&value, sizeof(T)); }

            /**
             * @brief Database will copy the passing data into the pointing memory
             * 
             * @tparam T 
             * @param key 
             * @param value 
             * @return true - ok
             * @return false - not exist
             */
            template<typename T>
            inline bool Put(const std::string& key, T value) { return Put(key, (void*)&value); }
    };
}
