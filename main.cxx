#include "eval_detector.h"

int main(int argc, char *argv[])
{
  EvalDetector *m_EvalDetector = new EvalDetector(atof(argv[3]));
  auto label_data = m_EvalDetector->ParseDocument(argv[1], false);
  auto tested_data = m_EvalDetector->ParseDocument(argv[2], true);
  m_EvalDetector->Eval(label_data, tested_data);
  delete m_EvalDetector;
  return 0;
}