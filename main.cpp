#include <iostream>
#include <string>
#include <vector>

using namespace std;

class Glob {
    
public:
    
    Glob() {
        subPattern = {};
    }
    
    //removed const type to use recursion of splitPatterns
    vector<string> Match(const string& pattern, const vector<string>& filenames) {
        
        vector<string> output;
        
        splitPatterns(pattern); //stores split patterns in subPattern
        
        for (int i=0; i< filenames.size();i++) {
            
            if (matchPatterns(filenames[i])) {
                output.push_back(filenames[i]);
            }
        }
        
        return output;
    }
    
private:
    string q = "?";
    string s = "*";
    vector<string> subPattern;
    
    void splitPatterns(string pattern) {
        
        int qPos = -1;
        int sPos = -1;
        
        if (pattern == "") {
            return;
        } else {
            
            if (pattern == q) {
                //q-mark found
                subPattern.push_back(pattern);
            } else if (pattern == s) {
                //star found
                subPattern.push_back(pattern);
            } else {
                qPos = (int) pattern.find(q);
                sPos = (int) pattern.find(s);
                //plain text found
                if (qPos < 0 && sPos < 0) {
                    subPattern.push_back(pattern);
                } else {
                    
                    //split the pattern further
                    int cutPoint = 0;
                    
                    if (qPos > -1 && sPos > -1) {
                        if (qPos < sPos) {
                            if (qPos == 0) {
                                cutPoint = qPos+1;
                            } else {
                                cutPoint = qPos;
                            }
                        } else {
                            if (sPos == 0) {
                                cutPoint = sPos+1;
                            } else {
                                cutPoint = sPos;
                            }
                        }
                    } else {
                        if (qPos > -1) {
                            if (qPos == 0) {
                                cutPoint = qPos+1;
                            } else {
                                cutPoint = qPos;
                            }
                        } else {
                            if (sPos == 0) {
                                cutPoint = sPos+1;
                            } else {
                                cutPoint = sPos;
                            }
                        }
                    }
                    
                    splitPatterns(pattern.substr(0,cutPoint));
                    splitPatterns(pattern.substr(cutPoint,pattern.length()-cutPoint));
                }
            }
        }
    }
    
    //--
    bool matchPatterns(string filename) {
        
        int qCount = 0;
        int sCount = 0;
        int checkPosition = 0;
        int tempIndex = -1;
        string subFilename = "";
        string tempSubFileName = "";
        string txtPattern = "";
        
        int p;
        for (p=0; p<subPattern.size(); p++) {
            if(subPattern[p] == q) {
                qCount++;                   //count ?s and do nothing
            } else if(subPattern[p] == s) {
                sCount++;                   //count *s and do nothing
            } else {
                txtPattern = subPattern[p];
                subFilename = filename.substr(checkPosition,filename.length()-checkPosition);
                
                if (sCount > 0) {           //case1: preceding *s
                    
                    tempIndex = starLookup(subFilename, txtPattern);
                    
                    if (tempIndex < 0) {
                        return false;
                    } else {
                        checkPosition += tempIndex;
                    }
                    
                } else if(qCount > 0) {     //case2: preceding ?s
                    int i;
                    for (i=qCount-1; i>-1; i--) { //special case to check multiple ?s right to left
                        
                        tempSubFileName = subFilename.substr(i,subFilename.length()-i);
                        
                        tempIndex = qMarkLookup(tempSubFileName, txtPattern);
                        
                        if (tempIndex >= 0) {
                            break;
                        }
                    }
                    
                    if (tempIndex < 0) {
                        return false;
                    } else {
                        checkPosition += tempIndex + i;
                    }
                    
                } else {                    //case3: no wild cards: exact match required
                    
                    tempIndex = exactLookup(subFilename, txtPattern);
                    
                    if (tempIndex < 0) {
                        return false;
                    }
                    checkPosition += txtPattern.length();
                }
                
                //reset counts
                qCount = 0;
                sCount = 0;
            }
        }
        
        //tail-check
        
        if (sCount < 1) { //no trailing *s
            
            checkPosition += txtPattern.length();
            string tail = filename.substr(checkPosition,filename.length()-checkPosition);
            
            if (qCount < 1) { //no trailing ?s
                
                if (tail != "") { //tail should be empty
                    return false;
                }
            } else {
                if (tail.length() > qCount) { //tail length smaller than number of ?s
                    return false;
                }
            }
        }
        
        return true;
    }
    
    //-- three scenarios for lookup
    int exactLookup(string filename, string pattern) {
        
        if (filename.substr(0,pattern.length()) == pattern) {
            return 0;
        }
        
        //no match
        return -1;
    }
    
    
    int qMarkLookup(string filename, string pattern) {
        
        for (int l=0; l<2; l++) {
            if (filename.substr(l,pattern.length()) == pattern) {
                return l;
            }
        }
        
        //no match
        return -1;
    }
    
    int starLookup(string filename, string pattern) {
        //check from right to left since right match precedes left match
        for (int l=(int)filename.length()-(int)pattern.length(); l>-1; l--) {
            if (filename.substr(l,pattern.length()) == pattern) {
                return l;
            }
        }
        
        //no match
        return -1;
    }
    
};

bool vector_equal(const vector<string>& expected, const vector<string>& actual) {
    if (expected.size() != actual.size()) {
        return false;
    }
    for (size_t i = 0; i < expected.size(); ++i) {
        if (expected[i] != actual[i]) {
            return false;
        }
    }
    return true;
}

#ifndef __main__
#define __main__

int main(int argc, char* argv[]) {
    // Should return true
    cout << vector_equal(
                         { "abcd", "dabc", "abc" },
                         Glob().Match("?abc*", { "abcd", "dabc", "abc", "efabc", "eadd" })) << endl;
    // Should return true
    cout << vector_equal(
                         { "abcd", "dabc", "abc" },
                         Glob().Match("?a**c*", { "abcd", "dabc", "abc", "efabc", "eadd" })) << endl;
    //---------------------------
    // my-test Should return true
    cout << vector_equal(
                         { "qqqcdwefewesdsu","qcdesudfdfsdf","cdttttttteffffua"},
                         Glob().Match("???cd*e*?u**?", { "qqqcdwefewesdsu","qcdesudfdfsdf","cdttttttteffffua","qqqqcddrefdfuas","cddjfhdfu","cddfedfdf" })) << endl;
    // my-test Should return true
    cout << vector_equal(
                         { "abcde","cde","cdecde","dfgcde","acde"},
                         Glob().Match("???cde", { "abcde","cde","cdecde","dfgcde","acde","cdefgh","abcdef","aabbcde" })) << endl;
}

#endif
