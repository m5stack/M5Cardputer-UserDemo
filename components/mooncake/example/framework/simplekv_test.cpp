/**
 * @file simplekv_test.cpp
 * @author Forairaaaaa
 * @brief https://github.com/Forairaaaaa/simplekv/blob/main/example/basic_usage.cpp
 * @version 0.1
 * @date 2023-08-17
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <simplekv/simplekv.h>
#include <iostream>
#include <cstring>


int main()
{
    SIMPLEKV::SimpleKV db;

    /* -------------------------------------------------------------- */
    printf("\n[Basic types]\n");


    db.Add<int>("Age", 666);
    printf("Age: %d\n", db.Get("Age")->value<int>());
    // > Age: 666

    if (db.Get("Age")->value<int>() != 666)
        return -1;



    db.Put<int>("Age", -777);
    printf("Age: %d\n", db.Get("Age")->value<int>());
    // > Age: -777

    if (db.Get("Age")->value<int>() != -777)
        return -1;



    db.Add<float>("float", 6.666);
    printf("%f\n", db.Get("float")->value<float>());
    // > 6.666000

    if (std::to_string(db.Get("float")->value<float>()) != "6.666000")
        return -1;
    


    db.Add<double>("double", 2333.3333333333);
    printf("%f\n", db.Get("double")->value<double>());
    // > 2333.333333

    if (std::to_string(db.Get("double")->value<double>()) != "2333.333333")
        return -1;
    
    /* -------------------------------------------------------------- */



    /* -------------------------------------------------------------- */
    printf("\n[Array]\n");

    uint16_t data[10] = {0, 3, 6, 9, 12, 15, 18, 21, 24, 27};

    db.Add("Data", data);

    for (int i = 0; i < 10; i++)
    {
        printf("%d ", db.Get("Data")->value<uint16_t*>()[i]);
    }
    printf("\n");
    // > 0 3 6 9 12 15 18 21 24 27

    for (int i = 0; i < 10; i++)
    {
        if (db.Get("Data")->value<uint16_t*>()[i] != data[i])
            return -1;
    }
    /* -------------------------------------------------------------- */
    


    /* -------------------------------------------------------------- */
    printf("\n[Custom type]\n");

    struct MyType_t
    {
        int a;
        float b;
        std::string status;
    };

    MyType_t my_type_1 = {11, 1.11, "nihao"};
    MyType_t my_type_2 = {-22, -2.22, "wohao"};
    MyType_t my_type_temp = {0};

    db.Add("MyType_1", (void*)&my_type_1, sizeof(MyType_t));
    db.Add("MyType_2", (void*)&my_type_2, sizeof(MyType_t));

    my_type_temp = db.Get("MyType_1")->value<MyType_t>();
    printf("%d %.2f %s\n", my_type_temp.a, my_type_temp.b, my_type_temp.status.c_str());
    // > 11 1.11 nihao

    my_type_temp = db.Get("MyType_2")->value<MyType_t>();
    printf("%d %.2f %s\n", my_type_temp.a, my_type_temp.b, my_type_temp.status.c_str());
    // > -22 -2.22 wohao
    /* -------------------------------------------------------------- */



    /* -------------------------------------------------------------- */
    printf("\n[Database info api]\n");

    printf("database has %ld elements\n", db.Size());
    printf("database's memory usage: %ld\n\n", db.MemoryUsage());



    std::string key = "MyType_1";
    printf("delete data \"%s\"\n", key.c_str());
    db.Delete(key);

    printf("database has %ld elements\n", db.Size());
    printf("database's memory usage: %ld\n\n", db.MemoryUsage());



    printf("delete all data\n");
    db.DeleteAll();

    printf("database has %ld elements\n", db.Size());
    printf("database's memory usage: %ld\n\n", db.MemoryUsage());



    key = ":)";
    printf("is data \"%s\" exist? %s\n", key.c_str(), db.Exist(key) ? "yes" : "no");

    printf("add data \"%s\"\n", key.c_str());
    db.Add(key, std::string("qwq qwq qwq qwq"));

    printf("is data \"%s\" exist? %s\n", key.c_str(), db.Exist(key) ? "yes" : "no");

    printf("database has %ld elements\n", db.Size());
    printf("database's memory usage: %ld\n", db.MemoryUsage());
    /* -------------------------------------------------------------- */



    /* -------------------------------------------------------------- */
    printf("\n[Notice]\n");
    
    /* Because the database is actually storing the pointers of data buffer */
    /* When try to store a container like this */
    db.Add<std::string>("cppstring", "w.w w.w w.w w.w");
    printf("%s\n", db.Get("cppstring")->value<std::string>().c_str());

    /* It's normal when the address stay still */
    db.Put<std::string>("cppstring", ":( :(");
    printf("%s\n", db.Get("cppstring")->value<std::string>().c_str());

    /* But when the address moves */
    /* It goes wrong */
    db.Put<std::string>("cppstring", ":( :( :( :( :( :( :( :( :( :( :( :( :( :( :( :( :( ");
    printf("%s\n", db.Get("cppstring")->value<std::string>().c_str());
    /* Same as str::vector */


    db.Delete("cppstring");
    static std::string haha = ":)";


    /* So the best way is to store the container's pointer */
    db.Add<std::string*>("cppstring", &haha);
    printf("%s\n", db.Get("cppstring")->value<std::string*>()->c_str());

    /* Because database only storing the address of the container */
    /* Wherever it goes */
    haha = "so coooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooool!";
    printf("%s\n", db.Get("cppstring")->value<std::string*>()->c_str());

    /* System will get it for you */
    haha = "~";
    printf("%s\n", db.Get("cppstring")->value<std::string*>()->c_str());
    /* -------------------------------------------------------------- */


    return 0;
}
