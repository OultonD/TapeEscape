/* 
    Path.h - For the VHS Eliza Machine
*/

#ifndef Path_h
#define Path_h

#include "Arduino.h"

class Path
{

    public:
        Path(); 
        static const uint8_t numQuestions = 4;
//        void setQuestion(uint8_t n, String line0, String line1);
//        void setAllQuestions(String line0, String line1);
        void setCorrectResult(String s);
        void setIncorrectResult(String s);
        void setAnswerKey(bool key[numQuestions]);
        bool checkResult(bool answers[]);
//        String getQuestionLine0(uint8_t n);
//        String getQuestionLine1(uint8_t n);
        String getCorrectResult();
        String getIncorrectResult();
        uint8_t getNumQuestions();
    private:
//        String _L0[numQuestions];
//        String _L1[numQuestions];
        String _correctResult;
        String _incorrectResult;
        bool _answerKey[numQuestions];
};


#endif
