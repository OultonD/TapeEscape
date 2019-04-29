#include "Arduino.h"
#include "Path.h"

Path::Path()
{
}

//void Path::setQuestion(uint8_t n, String line0, String line1)
//{
//    _L0[n] = line0;
//    _L1[n] = line1;
//}
//
//void Path::setAllQuestions(String line0, String line1)
//{
//  for(int i = 0; i<numQuestions; i++)
//  {
//    _L0[i] = line0;
//    _L1[i] = line1;
//  }
//}
//
//String Path::getQuestionLine0(uint8_t n)
//{
//    return _L0[n];
//}
//
//String Path::getQuestionLine1(uint8_t n)
//{
//    return _L1[n];
//}

uint8_t Path::getNumQuestions()
{
    return numQuestions;
}

void Path::setCorrectResult(String s)
{
  _correctResult = s;
}
void Path::setIncorrectResult(String s)
{
  _incorrectResult = s;
}
void Path::setAnswerKey(bool key[])
{
  for(int i = 0; i<numQuestions; i++)
  {
    _answerKey[i] = key[i];
  }
}

bool Path::checkResult(bool answers[])
{
  bool result = true;
  for(int i = 0; i<numQuestions; i++)
  {
    result = result && answers[i] && _answerKey[i];
  }
  return result;
}

String Path::getCorrectResult()
{
  return _correctResult;
}

String Path::getIncorrectResult()
{
  return _incorrectResult;
}

