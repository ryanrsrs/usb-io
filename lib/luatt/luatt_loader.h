#ifndef LUATT_LOADER_H
#define LUATT_LOADER_H

// Talks to luatt.py

#define LUATT_MAX_ARGS 6

class Luatt_Loader {
    bool connected;

    struct Buffer_t {
        char* buf;
        size_t size;
        size_t max_size;
        size_t len;
        bool overflow;
        bool is_static;

        Buffer_t(char* static_buf=0, size_t static_buf_size=0);
        ~Buffer_t();

        int add(int ch);
        void reset();
    } Buffer;


    struct Arg {
        size_t off; // index into Buffer.buf[]
        size_t len;
    } Args[LUATT_MAX_ARGS];
    int Args_n;


    struct Raw {
        size_t arg_i; // index into Args[]
        size_t bytes;
    } Raw[LUATT_MAX_ARGS];
    size_t Raw_n;
    size_t Raw_i;
    size_t Raw_read;


    void Reset_Input();
    int Parse_Line();

    void Run_Command();
    void Command_Reset();
    void Command_Eval();
    void Command_Load();
    void Command_Msg();

    void Feed_Char(int ch);

public:
    Luatt_Loader(char* static_buf=0, size_t static_buf_size=0);
    ~Luatt_Loader();

    int Loop();
};

#endif