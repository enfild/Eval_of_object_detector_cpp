#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <list>
#include <vector>
#include <algorithm>

// Container for coordinates (xyxy)
struct boundig_box
{
    int x1 = 0;
    int y1 = 0;
    int x2 = 0;
    int y2 = 0;
};

// Form of writing reports to txt file
struct eval_report_element
{
    std::string path_to_image;
    boundig_box boundigBox;
    std::string label;
    float threshold = 0.0;
};

enum labels
{
    banana,
    apple,
    coconut
};
const std::string enum_string[] = { "banana", "apple", "coconut"};

// bases metrics for class
struct metric_per_class
{
    labels label;
    int tp = 0;
    int fp = 0;
    int fn = 0;
};

// class of evaluator of detector
class EvalDetector
{
public:
    // Constructor
    EvalDetector(float threshold);
    // Destructor
    ~EvalDetector();

    /// Prepare reports of eval
    void Eval(std::list<eval_report_element> *labeling_Data,
              std::list<eval_report_element> *report_Data);

    /// Parse txt files
    std::list<eval_report_element> *ParseDocument(std::string path, bool threshold);

private:
    /// Read all reports for one image and get tp,fp,fn metrics
    std::list<metric_per_class> getMetricForOneImage(std::list<eval_report_element>
                                report_matches, std::list<eval_report_element> label_matches);

    const std::list<std::string> ParseLine(std::string str, std::string delimiter);

    const float IoU(boundig_box bb1, boundig_box bb2);
    
    /// List for labeled data
    std::list<eval_report_element> *m_Labeling_Data;
    /// List for reported data
    std::list<eval_report_element> *m_Report_Data;

    const inline std::string convertToenum (int val)
    {
        std::string outStr(enum_string[val]);
        return outStr;
    }

    float m_threshold;
    const int pos_image_names_in_report = 0;
    const int pos_bb_in_report = 1;
    const int pos_label_in_report = 2;
    const int pos_threshold_in_report = 3;
};
