#ifndef DB_H
#define DB_H

#ifdef __cplusplus
extern "C"
{
#endif

    void DBInit();
    void DBPrintf(char *str, ...);

#ifdef __cplusplus
}
#endif

#endif