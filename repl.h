/**
 *  repl.h
 *
 *  @author Nathan Campos
 */

#ifndef REPL_H_
#define REPL_H_

#include <string>
#include <vector>

class REPL {
    private:
        void add_history();
        void back_history();
        void forward_history();

    public:
        bool has_started;
        bool string_is_ready;
        std::string input_marker;
        
        std::string current_str;
        unsigned int history_current_position;
        std::vector<std::string> history;

        // Constructor.
        REPL();
        //~REPL();

        void clear();
        void rewrite();

        void read();
        void eval();
};

#endif
