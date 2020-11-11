//============================================================================
// TaCoya Harris, Miguel Garcia, Luis Gonzalez
// Compiler Part 2
// CSCE 306
// Prof. St. Clair
// November 18, 2020
// A syntax Analyzer
//============================================================================
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
using namespace std;

class SyntaxAnalyzer{
    private:
        vector<string> lexemes;
        vector<string> tokens;
        vector<string>::iterator lexitr;
        vector<string>::iterator tokitr;
        vector<string> declaredVars;//vector to store declared variables and their types -TH

        // map of variables to datatype (i.e. sum t_integer)
        // sets this up for stage 3
        map<string, string> symboltable;

        // other private methods
        bool vdec();
        int vars();
        bool stmtlist();
        int stmt();
        bool ifstmt();
        bool elsepart();
        bool whilestmt();
        bool assignstmt();
        bool inputstmt();
        bool outputstmt();
        bool expr();
        bool simpleexpr();
        bool term();
        bool logicop();
        bool arithop();
        bool relop();
        bool decVars(string var, string tok);//returns true/false if var has been declared and has correct type
        std::istream& getline_safe(std::istream& input, std::string& output);
    public:
        SyntaxAnalyzer(istream& infile);
        // pre: 1st parameter consists of an open file containing a source code's
        //	valid scanner output.  This data must be in the form
        //			token : lexeme
        // post: the vectors have been populated

        bool parse();
        // pre: none
        // post: The lexemes/tokens have been parsed and the symboltable created.
        // If an error occurs, a message prints indicating the token/lexeme pair
        // that caused the error.  If no error occurs, the symboltable contains all
        // variables and datatypes.

        /*void print(){
        	cout<< "declared variables" << endl;
        	for (int i = 0; i <declaredVars.size();i++){
        		cout <<declaredVars[i] <<endl;
        	}
        }*/

};
SyntaxAnalyzer::SyntaxAnalyzer(istream& infile){
    string line, tok, lex;
    int pos;
    getline_safe(infile, line);
    bool valid = true;
    while(!infile.eof() && (valid)){
        pos = line.find(":");
        tok = line.substr(0, pos);
        lex = line.substr(pos+1, line.length());
        cout << pos << " " << tok << " " << lex << endl;
        tokens.push_back(tok);
        lexemes.push_back(lex);
        getline_safe(infile, line);
    }
    tokitr = tokens.begin();
    lexitr = lexemes.begin();
}

bool SyntaxAnalyzer::parse(){
    if (vdec()){
        if (tokitr!=tokens.end() && *tokitr=="t_main"){
            tokitr++; lexitr++;
            if (tokitr!=tokens.end() && stmtlist()){
            	if (tokitr!=tokens.end()) // should be at end token
                	if (*tokitr == "t_end"){
                		tokitr++; lexitr++;
                		if (tokitr==tokens.end()){  // end was last thing in file
                			cout << "Valid source code file" << endl;
                			return true;
                		}
                		else{
                			cout << "end came too early" << endl;
                		}
                	}
                	else{
                		cout << "invalid statement ending code" << endl;
                }
                else{
                	cout << "no end" << endl;
                }
            }
            else{
            	cout << "bad/no stmtlist" << endl;
            }
        }
        else{
        	cout << "no main" << endl;
        }
    }
    else{
    	cout << "bad var list" << endl;
    }
    return false;

}

//TaCoya
//VDEC -> var VARS {VARS} | 0
bool SyntaxAnalyzer::vdec(){
    if (*tokitr != "t_var")//if not a variable
        return true;
    else{ // token is a variable
        tokitr++; lexitr++;//move to next thing
        int result = 0;// 0 - valid
        result = vars();//result =  0 - valid, 1 - done, or 2 - error
        if (result == 2) //error
            return false;//not valid variable type
        while (result == 0){//while result is valid
            if (tokitr!=tokens.end())//if not at the end
                result = vars(); // parse vars
        }
        if (result == 1){//valid type
        	return true;//variable declared
        }
        else
            return false;//not declared
    }
}

//TaCoya
//VARS -> TYPE id {, id} ;
int SyntaxAnalyzer::vars(){//determines if type is valid
    int result = 0;  // 0 - valid, 1 - done, 2 - error
    string temp,type,var;
    if (*tokitr == "t_integer"){
        temp = "t_integer";
        type = *tokitr;//store type
        tokitr++; lexitr++;
    }
    else if (*tokitr == "t_string"){
        temp = "t_string";
        type = *tokitr;//store type
        tokitr++; lexitr++;
    }
    else
        return 1;
    bool semihit = false;
    while (tokitr != tokens.end() && result == 0 && !semihit){
        if (*tokitr == "t_id"){
            symboltable[*lexitr] = temp;
            var = *lexitr;
            tokitr++; lexitr++;
            if (tokitr != tokens.end() && *tokitr == "s_comma"){
                tokitr++; lexitr++;
            }
            else if (tokitr != tokens.end() && *tokitr == "s_semi"){
                semihit = true;
                declaredVars.push_back(type);//store type in vector
                declaredVars.push_back(var);//store variable in vector
                tokitr++; lexitr++;
            }
            else
                result = 2;
        }
        else{
            result = 2;
        }
    }
    return result;
}

//pre: user passes in variable and the token of the data being assigned to it
//post: returns true/false if variable has been declared and type is correct
bool SyntaxAnalyzer::decVars(string var, string tok){
	for (int i = 0; i < declaredVars.size(); i++){
		if (declaredVars[i] == var){
			cout << "variable was declared" <<endl;
			if (declaredVars[i+1] == tok){
				cout << "correct type" <<endl;
				return true;//variable declared and correct type
			}
		}
	}
	return false;//undeclared var
}
//STMTLIST -> STMT {STMT} | 0
bool SyntaxAnalyzer::stmtlist(){
    int result = stmt();

    while (result == 1){
    	result = stmt();
    }
    if (result == 0)
        return false;
    else
        return true;
}

//STMT -> IFSTMT | WHILESTMT | ASSIGNSTMT | INPUTSTMT | OUTPUTSTMT
int SyntaxAnalyzer::stmt(){  // returns 1 or 2 if valid, 0 if invalid
	if (*tokitr == "t_if"){
        tokitr++; lexitr++;
        //if (ifstmt()) return 1;
        if (tokitr!=tokens.end() && ifstmt()) return 1;//modified-TH
        else return 0;
    }
    else if (*tokitr == "t_while"){
        tokitr++; lexitr++;
        //if (whilestmt()) return 1;
        if (tokitr!=tokens.end() && whilestmt()) return 1;//modified-TH
        else return 0;
    }
    else if (*tokitr == "t_id"){  // assignment starts with identifier
        tokitr++; lexitr++;
        cout << "t_id" << endl;
        //if (assignstmt()) return 1;
        if (tokitr!=tokens.end() && assignstmt()) return 1;//modified-TH
        else return 0;
    }
    else if (*tokitr == "t_input"){
        tokitr++; lexitr++;
        //if (inputstmt()) return 1;
        if (tokitr!=tokens.end() && inputstmt()) return 1;//modified-TH
        else return 0;
    }
    else if (*tokitr == "t_output"){
        tokitr++; lexitr++;
        cout << "t_output" << endl;
        //if (outputstmt()) return 1;
        if (tokitr!=tokens.end() && outputstmt()) return 1;//modified-TH
        else return 0;
    }
    return 2;  //stmtlist can be null
}
//IFSTMT -> if (EXPR) then STMTLIST ELSEPART end if
bool SyntaxAnalyzer::ifstmt(){
	if (tokitr != tokens.end() && *tokitr == "s_lparen"){
			tokitr++; lexitr++;
			if (expr()){
				if (tokitr != tokens.end() && *tokitr == "s_rparen"){
					tokitr++; lexitr++;
					if (tokitr != tokens.end() && *tokitr == "t_then"){
						tokitr++; lexitr++;
						if (stmtlist()){
							if (elsepart()){
								//don't increment when calling function, so pointer is a next thing
								if (tokitr != tokens.end() && *tokitr == "t_end"){
									tokitr++; lexitr++;
									if (tokitr != tokens.end() && *tokitr == "t_if"){
										tokitr++; lexitr++;
										return true;
									}
								}
							}
						}
					}
				}
			}
		}
		return true;
	}

//ELSEPART -> else STMTLIST | 0
bool SyntaxAnalyzer::elsepart(){
	if (*tokitr == "t_else"){
	        tokitr++; lexitr++;
	        //if (stmtlist())
	        if (tokitr!=tokens.end() && stmtlist())//modified-TH
	            return true;
	        else
	            return false;
	    }
	    return true;   // elsepart can be null
	}

//WHILESTMT -> while (EXPR) loop STMTLIST end loop
// Luis Gonzalez
bool SyntaxAnalyzer::whilestmt(){
	return true;
	// write this function
}

//ASSIGNSTMT -> id = EXPR ;
bool SyntaxAnalyzer::assignstmt(){
	if(tokitr != tokens.end() && *tokitr == "t_id"){
		tokitr++; lexitr++;
		if(tokitr != tokens.end() && *tokitr == "s_assign"){
			tokitr++; lexitr++;
			if(expr()){
				if(tokitr != tokens.end() && *tokitr == "s_semi"){
					tokitr++; lexitr++;


					return true;
				}
			}
		}

	}
	return false;
}

//INPUTSTMT -> input ( id )
bool SyntaxAnalyzer::inputstmt(){
    if (*tokitr == "s_lparen"){
        tokitr++; lexitr++;
        if (*tokitr == "t_id"){
            tokitr++; lexitr++;
            if (*tokitr == "s_rparen"){
                tokitr++; lexitr++;
                return true;
            }
        }
    }
    return false;
}

//OUTPUTSTMT -> output (EXPR) | output (string)
bool SyntaxAnalyzer::outputstmt(){
	if(tokitr != tokens.end() && *tokitr == "t_output"){
		tokitr++; lexitr++;
		if(tokitr != tokens.end() && *tokitr == "s_lparen"){
			tokitr++; lexitr++;
			if(expr()){
				if(tokitr != tokens.end() && *tokitr == "s_rparen"){
					tokitr++; lexitr++;
					return true;

				}
			}
		}
		return false;

	}
	else if(tokitr != tokens.end() && *tokitr == "t_output"){
		tokitr++; lexitr++;
		if(tokitr != tokens.end() && *tokitr == "s_lparen"){
			tokitr++; lexitr++;
			if(tokitr != tokens.end() && *tokitr == "t_str"){
				tokitr++; lexitr++;
				if(tokitr != tokens.end() && *tokitr == "s_rparen"){
					tokitr++; lexitr++;
				}
			}
		}
	}
	return false;
}

//EXPR -> SIMPLEEXPR [LOGICOP SIMPLEEXPR]
bool SyntaxAnalyzer::expr(){
    if (simpleexpr()){
	if (logicop()){
		if (simpleexpr())
			return true;
		else
			return false;
	}
	else
		return true;
    }
    else{
	return false;
    }
}

//SIMPLEEXPR -> TERM [ARITHOP | RELOP TERM]
// Luis Gonzalez
bool SyntaxAnalyzer::simpleexpr(){
	return true;
    // write this function
}

//TERM -> int | str | id | (EXPR)
bool SyntaxAnalyzer::term(){
    if ((*tokitr == "t_int")
	|| (*tokitr == "t_str")
	|| (*tokitr == "t_id")){
    	tokitr++; lexitr++;
    	return true;
    }
    else
        if (*tokitr == "s_lparen"){
            tokitr++; lexitr++;
            if (expr())
                if (*tokitr == "s_rparen"){
                    tokitr++; lexitr++;
                    return true;
                }
        }
    return false;
}

//LOGICOP -> && | ||
bool SyntaxAnalyzer::logicop(){
    if ((*tokitr == "s_and") || (*tokitr == "s_or")){
        tokitr++; lexitr++;
        return true;
    }
    else
        return false;
}

//RELOP -> == | < | <= | > | >= | !=
bool SyntaxAnalyzer::arithop(){
    if ((*tokitr == "s_mult") || (*tokitr == "s_plus") || (*tokitr == "s_minus")
        || (*tokitr == "s_div")	|| (*tokitr == "s_mod")){
        tokitr++; lexitr++;
        return true;
    }
    else
        return false;
}

//ARITHOP -> + | - | * | / | %
bool SyntaxAnalyzer::relop(){
    if ((*tokitr == "s_lt") || (*tokitr == "s_gt") || (*tokitr == "s_ge")
        || (*tokitr == "s_eq") || (*tokitr == "s_ne") || (*tokitr == "s_le")){
        tokitr++; lexitr++;
        return true;
    }
    else
    	return false;
}

std::istream& SyntaxAnalyzer::getline_safe(std::istream& input, std::string& output)
{
    char c;
    output.clear();

    input.get(c);
    while (input && c != '\n')
    {
        if (c != '\r' || input.peek() != '\n')
        {
            output += c;
        }
        input.get(c);
    }

    return input;
}


int main(){
    ifstream infile("codelexemes.txt");
    if (!infile){
    	cout << "error opening lexemes.txt file" << endl;
        exit(-1);
    }
    SyntaxAnalyzer sa(infile);
    sa.parse();

    //sa.print();

    return 1;
}
