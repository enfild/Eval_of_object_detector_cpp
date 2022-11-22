#include "eval_detector.h"

EvalDetector::EvalDetector(float threshold)
{
  m_threshold = threshold;
  m_Report_Data = new std::list<eval_report_element>();
  m_Labeling_Data = new std::list<eval_report_element>();
}

EvalDetector::~EvalDetector()
{
  delete m_Report_Data;
  delete m_Labeling_Data;
}

void EvalDetector::Eval(std::list<eval_report_element> *Labeling_Data,
                        std::list<eval_report_element> *Report_Data)
{
  std::list<std::list<metric_per_class>> general_eval;
  std::list<std::string> usedImagesNames;
  for (auto base_Labeling_Element : *Labeling_Data)
  {
    // check used image
    bool foundUsedImage = (std::find(usedImagesNames.begin(), usedImagesNames.end(),
                                     base_Labeling_Element.path_to_image) != usedImagesNames.end());
    if (foundUsedImage)
      continue;

    std::string image_name = base_Labeling_Element.path_to_image;
    usedImagesNames.push_back(image_name);

    // lists of resulrts for one image
    std::list<eval_report_element> report_matches;
    std::list<eval_report_element> label_matches;
    for (auto matchImageName_label : *Labeling_Data)
    {
      if (matchImageName_label.path_to_image == base_Labeling_Element.path_to_image)
        label_matches.push_back(matchImageName_label);
    }

    for (auto matchImageName_report : *Report_Data)
    {
      if (matchImageName_report.path_to_image == base_Labeling_Element.path_to_image)
        report_matches.push_back(matchImageName_report);
    }
    auto metrics_per_class_one_image = getMetricForOneImage(report_matches, label_matches);
    general_eval.push_back(metrics_per_class_one_image);
  }
  int generalTP = 0;
  int generalFP = 0;
  int generalFN = 0;
  std::array<metric_per_class, 3> *classesMetrics = new std::array<metric_per_class, 3> {
    metric_per_class {labels(0), 0 , 0 ,0},
    metric_per_class {labels(1), 0 , 0 ,0},
    metric_per_class {labels(2), 0 , 0 ,0}
  };

  // accumulate metrics
  for (auto image_metrics : general_eval)
  {
    for (auto output : image_metrics)
    {
      generalTP += output.tp;
      generalFP += output.fp;
      generalFN += output.fn;

      (*classesMetrics)[output.label].tp += output.tp;
      (*classesMetrics)[output.label].fp += output.fp;
      (*classesMetrics)[output.label].fn += output.fn;
    }
  }
  float recall = (float)generalTP / (float)(generalTP + generalFN);
  float precision = (float)generalTP / (float)(generalTP + generalFP);
  std::cout << recall << " - general recall" << std::endl;
  std::cout << precision << " - general precision" << std::endl;

  for (auto label : *classesMetrics)
  {
    float recall = (float)label.tp / (float)(label.tp + label.fn);
    float precision = (float)label.tp / (float)(label.tp + label.fp);
    std::cout << "label - " << enum_string[label.label] << std::endl;
    std::cout << recall << " - recall" << std::endl;
    std::cout << precision << " - precision" << std::endl;
  }
  delete classesMetrics;
}

std::list<metric_per_class> EvalDetector::getMetricForOneImage(std::list<eval_report_element>
                              report_matches, std::list<eval_report_element> label_matches)
{
  std::list<metric_per_class> output_metrics;
  for (int labels_id = 0; labels_id < enum_string->length(); labels_id++)
  {
    auto label =  convertToenum(labels(labels_id));
    int tp = 0;
    int fp = 0;
    int fn = 0;
    int lenReportMatches = 0;
    int lenLabelMatches = 0;
    int lenIntersectedMatches = 0;
    for (auto labeling_Element : label_matches)
    {
      lenLabelMatches++;
      if (label != labeling_Element.label)
        continue;
      for (auto report_Element : report_matches)
      {
        lenReportMatches++;
        if (IoU(report_Element.boundigBox, labeling_Element.boundigBox) >= 0.5)
        {
          lenIntersectedMatches++;
          if (report_Element.threshold >= m_threshold)
          {
            if(report_Element.label == labeling_Element.label)
              tp++;
            else
              fp++;
          }
        }
      }
    }
    int diffIntersected = lenLabelMatches - lenIntersectedMatches;
    fn += diffIntersected ? diffIntersected > 0 : 0;
    metric_per_class m_metric_per_class = {labels(labels_id), tp, fp, fn};
    output_metrics.push_back(m_metric_per_class);
  }
  return output_metrics;
}

const float EvalDetector::IoU(boundig_box bb1, boundig_box bb2)
{
  auto xA = std::max(bb1.x1, bb2.x1);
  auto yA = std::max(bb1.y1, bb2.y1);
  auto xB = std::max(bb1.x2, bb2.x2);
  auto yB = std::max(bb1.y2, bb2.y2);
  auto interArea = std::max(0, xB - xA + 1) * std::max(0, yB - yA + 1);
  auto boxAArea = (bb1.x2 - bb1.x1 + 1) * (bb1.y2 - bb1.y1 + 1);
  auto boxBArea = (bb2.x2 - bb2.x1 + 1) * (bb2.y2 - bb2.y1 + 1);
  float iou = interArea / float(boxAArea + boxBArea - interArea);
  return iou;
}

const std::list<std::string> EvalDetector::ParseLine(std::string str, std::string delimiter)
{
  size_t pos = 0;
  std::string token;
  std::list<std::string> row_string;
  while ((pos = str.find(delimiter)) != std::string::npos)
  {
    token = str.substr(0, pos);
    row_string.push_back(token);
    str.erase(0, pos + delimiter.length());
  }
  row_string.push_back(str);
  return row_string;
}
std::list<eval_report_element> *EvalDetector::ParseDocument(std::string path, bool threshold_flag)
{
  int counterLines = 0;
  std::ifstream file(path);
  std::string delimiter = " ";
  std::string str;
  while (std::getline(file, str))
  {
    std::list<std::string> row_string = ParseLine(str, delimiter);
    std::vector<std::string> result(row_string.begin(), row_string.end());
    eval_report_element eval_report_element;
    eval_report_element.path_to_image = result.at(pos_image_names_in_report);

    std::list<std::string> row_string_bounding_box = ParseLine(result.at(pos_bb_in_report), ",");
    std::vector<std::string> result_bounding_box(row_string_bounding_box.begin(), row_string_bounding_box.end());
    eval_report_element.boundigBox.x1 = stoi(result_bounding_box.at(0));
    eval_report_element.boundigBox.y1 = stoi(result_bounding_box.at(1));
    eval_report_element.boundigBox.x2 = stoi(result_bounding_box.at(2));
    eval_report_element.boundigBox.y2 = stoi(result_bounding_box.at(3));
    eval_report_element.label = result.at(pos_label_in_report);

    if (threshold_flag)
    {
      eval_report_element.threshold = stof(result.at(pos_threshold_in_report));
      m_Report_Data->push_back(eval_report_element);
    }
    else
    {
      m_Labeling_Data->push_back(eval_report_element);
    }
  }
  if (threshold_flag)
  {
    return m_Report_Data;
  }
    
  else
  {
    return m_Labeling_Data;
  }
}