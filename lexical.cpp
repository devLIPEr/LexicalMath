#include <iostream>
#include <map>
#include <fstream>

using namespace std;

struct Token{
    string type;
    int line;
    bool used = false;

    Token(){};
    Token(string type, int line){
        this->type = type;
        this->line = line;
    }

    friend ostream& operator<< (ostream& stream, const Token& token){
        return stream << '<' << token.type << ", " << (token.line > 0 ? to_string(token.line) : "") << '>';
    }
};

bool isDigit(char c){
    return (c > 47 && c < 58);
}

void rejectedBeforeHit(bool &rejected, map<string, Token> &words, string &buf, int line){
    if(rejected){
        if(words.find(buf) == words.end()){
            words[buf] = Token("rejected", line);
            words[buf].used = true;
        }else if(!words[buf].used){
            words[buf].used = true;
        }
        buf = "";
        rejected = false;
    }else if(buf.length()){
        if(words.find(buf) == words.end()){
            words[buf] = Token("number", line);
            words[buf].used = true;
        }
        buf = "";
    }else{
        
    }
}

int main(int argc, char* argv[]){
    char peek;
    uint64_t line = 1;
    string fileName;
    if(argc > 0){
        fileName = argv[1];
    }

    ifstream calc("./"+fileName);
    if(calc.fail()){
        std::cerr << "No file with name " << fileName << " found\n";
        return 0;
    }

    map<string, Token> words;
    // initialize words hashmap
    words["("] = Token("punctuation", 0);
    words[")"] = Token("punctuation", 0);
    words["+"] = Token("keyword", 0);
    words["-"] = Token("keyword", 0);
    words["*"] = Token("keyword", 0);
    words["/"] = Token("keyword", 0);
    words["^"] = Token("keyword", 0);

    peek = calc.peek();
    bool rejected = false;
    string buf = "";
    while(peek != EOF){
        for(;; peek = calc.peek()){
            if(peek == ' ' || peek == '\t'){calc.ignore();}
            else if(peek == '\n'){calc.ignore();line++;}
            else break;
        }
        if(isDigit(peek)){ // recognize both floats and ints
            rejectedBeforeHit(rejected, words, buf, line);
            bool aloneDot = false;
            buf += peek;
            calc.ignore();
            peek = calc.peek();
            for(;; peek = calc.peek()){
                if(peek == '.'){
                    buf += peek;
                    calc.ignore();
                    aloneDot = true;
                }else if(isDigit(peek)){
                    buf += peek;
                    calc.ignore();
                    aloneDot = false;
                }else{
                    if(aloneDot)
                        buf += '0';
                    break;
                }
            }
            if(words.find(buf) == words.end()){
                words[buf] = Token("number", line);
                words[buf].used = true;
            }
        }else{
            if(buf.length()){
                if(words.find(buf) == words.end()){
                    words[buf] = Token("number", line);
                    words[buf].used = true;
                }
                buf = "";
            }
            int s = 0;
            bool valid = true;
            for(;; peek = calc.peek()){
                if(peek == EOF){
                    break;
                }else if(peek == '\n'){
                    line++;
                    calc.ignore();
                }else if(peek == ' ' || peek == '\t'){
                    calc.ignore();
                }else if(isDigit(peek)){
                    if(buf.length()){
                        rejected = true;
                    }
                    break;
                }else{
                    buf += peek;
                    string b1 = buf.substr(s, buf.length()-s);
                    if(words.find(string()+peek) != words.end() && words[string()+peek].type == "keyword"){
                        calc.ignore();
                        string b1 = buf.substr(0, buf.length()-1);
                        if(isDigit(calc.peek())){
                            if(valid){
                                words[string()+peek].used = true;
                                if(b1.length()){
                                    words[b1] = Token("rejected", line);
                                    words[b1].used = true;
                                }
                                buf = "";
                                valid = true;
                            }
                        }else if(valid && ((words.find(string()+char(calc.peek())) != words.end() && words[string()+char(calc.peek())].type != "keyword") || (calc.peek() != '\n' && words.find(string()+char(calc.peek())) == words.end()))){
                            words[string()+peek].used = true;
                            if(b1.length()){
                                words[b1] = Token("rejected", line);
                                words[b1].used = true;
                            }
                            buf = "";
                            valid = true;
                        }else{
                            valid = false;
                        }
                    }else if(words.find(string()+peek) != words.end() && words[string()+peek].type == "punctuation"){
                        calc.ignore();
                        words[string()+peek].used = true;
                        buf = "";
                        valid = true;
                    }else{
                        calc.ignore();
                        if(isDigit(calc.peek())){
                            words[buf] = Token("rejected", line);
                            words[buf].used = true;
                            buf = "";
                            valid = true;
                        }
                    }
                }
            }
        }
    }
    if(buf.length()){
        rejected = true;
    }
    rejectedBeforeHit(rejected, words, buf, line);

    for(pair<string, Token> token : words){
        if(token.second.used){
            cout << token.first << ": " << token.second << '\n';
        }
    }

    return 0;
}
