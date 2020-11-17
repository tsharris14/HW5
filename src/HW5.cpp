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
        bool declaredCheck(string variable);
        bool typeCheck(string variable, string dataType);
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
	cout << "==============================="<<endl;
	cout<<"*parse method"<<endl;
    if (vdec()){
    	cout<<*tokitr<<endl;
        if (tokitr!=tokens.end() && *tokitr=="t_main"){
            tokitr++; lexitr++;
            if (tokitr!=tokens.end() && stmtlist()){
            	if (tokitr!=tokens.end()){ // should be at end token
            		//cout << *tokitr << endl;
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


//VDEC -> var VARS {VARS} | 0
bool SyntaxAnalyzer::vdec(){
	cout << "*vdec" << endl;
	cout << *tokitr << endl;
    if (*tokitr != "t_var")//if not a variable
        return true;
    else{ // token is a variable
        tokitr++; lexitr++;//move to next thing
        int result = 0;// 0 - valid
        result = vars();//result =  0 - valid, 1 - done, or 2 - error
        cout << result << endl;
        if (result == 2) //error
            return false;//not valid variable type
        while (result == 0){//while result is valid
            if (tokitr!=tokens.end())//if not at the end
                result = vars(); // parse vars
            	cout << result << endl;
        }
        if (result == 1){//valid type
        	return true;//variable declared
        }
        else
            return false;//not declared
    }
}


//VARS -> TYPE id {, id} ;
int SyntaxAnalyzer::vars(){//determines if type is valid
	cout << "*vars method" << endl;
    int result = 0;  // 0 - valid, 1 - done, 2 - error
    string temp,type,variable;
    if (*tokitr == "t_integer"){
        temp = "t_integer";
        type = *tokitr;//store type -TH
        tokitr++; lexitr++;
    }
    else if (*tokitr == "t_string"){
        temp = "t_string";
        type = *tokitr;//store type -TH
        tokitr++; lexitr++;
    }
    else
        return 1;
    bool semihit = false;
    while (tokitr != tokens.end() && result == 0 && !semihit){
        if (*tokitr == "t_id"){
            symboltable[*lexitr] = temp;
            variable = *lexitr; //store variable -TH
            tokitr++; lexitr++;
            if (tokitr != tokens.end() && *tokitr == "s_comma"){
                tokitr++; lexitr++;
            }
            else if (tokitr != tokens.end() && *tokitr == "s_semi"){
                semihit = true;
                declaredVars.push_back(type);//store type in vector
                declaredVars.push_back(variable);//store variable in vector
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


//TaCoya
//pre: user passes in variable / identifier
//post: returns true/false if variable has been declared
bool SyntaxAnalyzer::declaredCheck(string variable){
	cout << "*declaredCheck method" << endl;
	cout << variable << endl;
	map<string,string>::iterator itr; //initialize iterator
	itr = symboltable.find(variable); //search map
	  if (itr != symboltable.end())   //if found, return true
		  return true;//var declared
	  else
		  return false;//var undeclared
}

//TaCoya
//pre: user passes in variable/identifier and the token of the data type being assigned to it
//post: returns true/false if data type of assignment matches data type of declared variable
bool SyntaxAnalyzer::typeCheck(string variable, string dataType){
	cout << "*typeCheck method" << endl;
	cout << variable << endl;
	cout << dataType << endl;
	string varType;
	varType = symboltable[variable];//declared variable type
	cout << varType << endl;

	if (dataType == "t_int"){
		if (varType == "t_integer"){
			cout << "type is correct" <<endl;
			return true;
			}
		else{
			cout<<"incorrect type" << endl;
			return false;
		}
	}
	if (dataType == "t_str"){
		if (varType == "t_string"){
			cout << "type is correct" <<endl;
			return true;
		}
		else{
			cout<<"incorrect type" << endl;
			return false;
		}
	}

}

//STMTLIST -> STMT {STMT} | 0
bool SyntaxAnalyzer::stmtlist(){
	cout << "* stmtlist method" << endl;
    int result = stmt();// returns 1 or 2 if valid, 0 if invalid
    cout << result << endl;
    while (result == 1){
    	result = stmt();
    }
    if (result == 0)
        return false;
    else if (result == 2 || result >= 2)
            return true;
    else
        return true;
}

//STMT -> IFSTMT | WHILESTMT | ASSIGNSTMT | INPUTSTMT | OUTPUTSTMT
int SyntaxAnalyzer::stmt(){  // returns 1 or 2 if valid, 0 if invalid
	cout << "*stmt method" << endl;
	cout << *tokitr << endl;
	if (*tokitr == "t_if"){
        tokitr++; lexitr++;
        //if (ifstmt()) return 1;
        if (tokitr!=tokens.end() && ifstmt()) return 1;//modified-TH
        else return 0;
    }
    else if (*tokitr == "t_while"){
        tokitr++; lexitr++;
        if (whilestmt()) return 2;
        if (tokitr!=tokens.end() && whilestmt()) return 2;//modified-TH
        else return 0;
    }
    else if (*tokitr == "t_id"){  // assignment starts with identifier
    	string variable = *lexitr;
    	/*if (decVars(variable, "null") == false){ //declaration check -TH
    		return 0; //undeclared variable
    	}*/
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
	if (*tokitr == "s_lparen"){
		tokitr++; lexitr++;
		if (expr()){
			if (*tokitr == "s_rparen"){
				tokitr++; lexitr++;
				if (*tokitr == "s_semi"){
					tokitr++; lexitr++;
					if (*tokitr == "t_loop"){
						tokitr++; lexitr++;
						if (stmtlist()){
							if (*tokitr == "t_end"){
								tokitr++; lexitr++;
								if (*tokitr == "t_loop"){
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

//Miguel
//ASSIGNSTMT -> id = EXPR ;
bool SyntaxAnalyzer::assignstmt(){
	cout << "*assignstmt method" << endl;
	cout << *tokitr << endl;
	if(tokitr != tokens.end() && *tokitr == "t_id"){
		string variable = *lexitr;
		cout << variable << endl;//x
		tokitr++; lexitr++;
		if(tokitr != tokens.end() && *tokitr == "s_assign"){
			cout << *tokitr << endl;//t_int
			tokitr++; lexitr++;
			string valueType = *tokitr;
			//cout << valueType << endl;
			/*if(typeCheck(variable, valueType) == false){	// commented it out so my simpleexpr method could work
				return false;
			}*/

			//if{
				if(expr()){
					cout << *tokitr << endl;
					if(tokitr != tokens.end() && *tokitr == "s_semi"){
						tokitr++; lexitr++;
						return true;
					}
				}
			//}
		}
	}
	return false;
}

//INPUTSTMT -> input ( id )
bool SyntaxAnalyzer::inputstmt(){
 	cout << "*inputstmt method" << endl;
 	cout << *tokitr << endl;
    if (*tokitr == "s_lparen"){
        tokitr++; lexitr++;
        if (*tokitr == "t_id"){
        	string variable = *lexitr;
        	if (declaredCheck(variable) == false){
        		return false;
        	}
        	else{
				tokitr++; lexitr++;
				if (*tokitr == "s_rparen"){
					tokitr++; lexitr++;
					return true;
				}
        	}
        }
    }

    return false;
}

//Miguel
//OUTPUTSTMT -> output (EXPR) | output (string)
bool SyntaxAnalyzer::outputstmt(){
	cout << "*outputstmt method" << endl;
	cout << *tokitr << endl;
	if(tokitr != tokens.end() && *tokitr == "t_output"){
		tokitr++; lexitr++;
		cout << *tokitr << endl;
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
	cout << "IN simpleexpr" << endl;

	if (term()){
		cout << *tokitr <<endl;
		if (*tokitr == "s_semi") return true;
		else if (arithop()){
			if (term()){
				return true;
			}
		}
		else if (relop()){
			if (term()){
				return true;
			}
		}
		else return false;
	}
	else return true;
	cout << "before last return" << endl;
	return true;
    // write this function
}

//TERM -> int | str | id | (EXPR)
bool SyntaxAnalyzer::term(){
	cout << "*term method" << endl;
	cout << *tokitr << endl;
    if ((*tokitr == "t_int")
	|| (*tokitr == "t_str")
	|| (*tokitr == "t_id")){
    	tokitr++; lexitr++;
    	return true;
    }
    else
    	cout << *tokitr << endl;
        if (*tokitr == "s_lparen"){
            tokitr++; lexitr++;
            if (expr())
            	cout << *tokitr << endl;
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

//ARITHOP -> + | - | * | / | %
bool SyntaxAnalyzer::arithop(){
	cout << "*arithop method" << endl;
	cout << *tokitr << endl;
    if ((*tokitr == "s_mult") || (*tokitr == "s_plus") || (*tokitr == "s_minus")
        || (*tokitr == "s_div")	|| (*tokitr == "s_mod")){
        tokitr++; lexitr++;
        return true;
    }
    else{
    	cout << "false" << endl;
        return false;
    }
}

//RELOP -> == | < | <= | > | >= | !=
bool SyntaxAnalyzer::relop(){
	cout << "in relop" << endl;
	cout << *tokitr << endl;
    if ((*tokitr == "s_lt") || (*tokitr == "s_gt") || (*tokitr == "s_ge")
        || (*tokitr == "s_eq") || (*tokitr == "s_ne") || (*tokitr == "s_le")){
        tokitr++; lexitr++;
        return true;
    }
    else
    	cout << "false" << endl;
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
    ifstream infile("codelexemes1.txt");
    if (!infile){
    	cout << "error opening lexemes.txt file" << endl;
        exit(-1);
    }
    SyntaxAnalyzer sa(infile);
    sa.parse();

    //sa.print();

    return 1;
}
